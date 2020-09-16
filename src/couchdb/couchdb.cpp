#include "couchdb.h"
#include "couchdb_p.h"
#include "couchdbquery.h"
#include "couchdblistener.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QDebug>

CouchDB::CouchDB(QObject *parent) :
    QObject(parent),
    d_ptr(new CouchDBPrivate)
{
    Q_D(CouchDB);
    d->networkManager.reset(new QNetworkAccessManager(this));
}

CouchDB::~CouchDB()
{
}

QUrl CouchDB::server() const
{
    Q_D(const CouchDB);
    return d->server;
}

void CouchDB::setServer(const QUrl &server)
{
    Q_D(CouchDB);
    d->server = server;
}

void CouchDB::executeQuery(CouchDBQuery *query)
{
    Q_D(CouchDB);

    QString username = d->server.userName();
    QString password = d->server.password();
    if (!username.isEmpty() && !password.isEmpty())
        query->request()->setRawHeader("Authorization", CouchDBPrivate::basicAuth(username, password));

    qDebug() << "Invoked url:" << query->operation() << query->request()->url().toString();

    QNetworkReply *reply;
    switch (query->operation()) {
    case COUCHDB_CHECKINSTALLATION:
    default:
        reply = d->networkManager->get(*query->request());
        break;
    case COUCHDB_STARTSESSION:
        reply = d->networkManager->post(*query->request(), query->body());
        break;
    case COUCHDB_ENDSESSION:
        reply = d->networkManager->deleteResource(*query->request());
        break;
    case COUCHDB_LISTDATABASES:
        reply = d->networkManager->get(*query->request());
        break;
    case COUCHDB_CREATEDATABASE:
        reply = d->networkManager->put(*query->request(), query->body());
        break;
    case COUCHDB_DELETEDATABASE:
        reply = d->networkManager->deleteResource(*query->request());
        break;
    case COUCHDB_LISTDOCUMENTS:
        reply = d->networkManager->get(*query->request());
        break;
    case COUCHDB_RETRIEVEREVISION:
        reply = d->networkManager->head(*query->request());
        break;
    case COUCHDB_RETRIEVEDOCUMENT:
        reply = d->networkManager->get(*query->request());
        break;
    case COUCHDB_UPDATEDOCUMENT:
        reply = d->networkManager->put(*query->request(), query->body());
        break;
    case COUCHDB_DELETEDOCUMENT:
        reply = d->networkManager->deleteResource(*query->request());
        break;
    case COUCHDB_UPLOADATTACHMENT:
        reply = d->networkManager->put(*query->request(), query->body());
        break;
    case COUCHDB_DELETEATTACHMENT:
        reply = d->networkManager->deleteResource(*query->request());
        break;
    case COUCHDB_REPLICATEDATABASE:
        reply = d->networkManager->post(*query->request(), query->body());
        break;
    }

    if (query->operation() != COUCHDB_REPLICATEDATABASE) {
        connect(query, SIGNAL(timeout()), SLOT(queryTimeout()));
        query->startTimeoutTimer();
    }

    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
    d->currentQueries[reply] = query;
}

void CouchDB::queryFinished()
{
    Q_D(CouchDB);

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    QByteArray data;
    CouchDBQuery *query = d->currentQueries[reply];
    bool hasError = false;
    if (reply->error() == QNetworkReply::NoError) {
        data = reply->readAll();
    } else {
        qWarning() << reply->errorString();
        hasError = true;
    }


    QJsonDocument json = QJsonDocument::fromJson(data);
    CouchDBResponse response = CouchDBResponse::fromJson(json, query);
    response.setStatus(hasError || (query->operation() != COUCHDB_CHECKINSTALLATION && query->operation() != COUCHDB_RETRIEVEDOCUMENT &&
            !json["ok"].toBool()) ? CouchDBResponse::Error : CouchDBResponse::Success);

    switch (query->operation()) {
    case COUCHDB_CHECKINSTALLATION:
    default:
        if (!hasError)
            response.setStatus(!json["couchdb"].isUndefined() ? CouchDBResponse::Success : CouchDBResponse::Error);
        emit installationChecked(response);
        break;
    case COUCHDB_STARTSESSION:
        if (hasError && reply->error() >= 201 && reply->error() <= 299)
            response.setStatus(CouchDBResponse::AuthError);
        emit sessionStarted(response);
        break;
    case COUCHDB_ENDSESSION:
        emit sessionEnded(response);
        break;
    case COUCHDB_LISTDATABASES:
        emit databasesListed(response);
        break;
    case COUCHDB_CREATEDATABASE:
        emit databaseCreated(response);
        break;
    case COUCHDB_DELETEDATABASE:
        emit databaseDeleted(response);
        break;
    case COUCHDB_LISTDOCUMENTS:
        emit documentsListed(response);
        break;
    case COUCHDB_RETRIEVEREVISION: {
        QString revision = reply->rawHeader("ETag");
        revision.remove("\"");
        response.setRevision(revision);
        emit revisionRetrieved(response);
        break;
    }
    case COUCHDB_RETRIEVEDOCUMENT:
        emit documentRetrieved(response);
        break;
    case COUCHDB_UPDATEDOCUMENT:
        emit documentUpdated(response);
        break;
    case COUCHDB_DELETEDOCUMENT:
        emit documentDeleted(response);
        break;
    case COUCHDB_UPLOADATTACHMENT:
        emit attachmentUploaded(response);
        break;
    case COUCHDB_DELETEATTACHMENT:
        emit attachmentDeleted(response);
        break;
    case COUCHDB_REPLICATEDATABASE:
        emit databaseReplicated(response);
        break;
    }

    d->currentQueries.remove(reply);
    reply->deleteLater();
    delete query;
}

void CouchDB::queryTimeout()
{
    CouchDBQuery *query = qobject_cast<CouchDBQuery*>(sender());
    if (!query)
        return;

    qWarning() << query->url() << "timed out. Retrying...";

    executeQuery(query);
}

void CouchDB::checkInstallation()
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->server);
    query->setOperation(COUCHDB_CHECKINSTALLATION);

    executeQuery(query);
}

void CouchDB::startSession(const QString &username, const QString &password)
{
    Q_D(CouchDB);

    QUrlQuery postData;
    postData.addQueryItem("name", username);
    postData.addQueryItem("password", password);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({"_session"}));
    query->setOperation(COUCHDB_STARTSESSION);
    query->request()->setRawHeader("Accept", "application/json");
    query->request()->setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    query->setBody(postData.toString(QUrl::FullyEncoded).toUtf8());

    executeQuery(query);
}

void CouchDB::endSession()
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({"_session"}));
    query->setOperation(COUCHDB_ENDSESSION);

    executeQuery(query);
}

void CouchDB::listDatabases()
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({"_all_dbs"}));
    query->setOperation(COUCHDB_LISTDATABASES);

    executeQuery(query);
}

void CouchDB::createDatabase(const QString &database)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({database}));
    query->setOperation(COUCHDB_CREATEDATABASE);
    query->setDatabase(database);

    executeQuery(query);
}

void CouchDB::deleteDatabase(const QString &database)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({database}));
    query->setOperation(COUCHDB_DELETEDATABASE);
    query->setDatabase(database);

    executeQuery(query);
}

void CouchDB::listDocuments(const QString &database)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({database, "_all_docs"}));
    query->setOperation(COUCHDB_LISTDOCUMENTS);
    query->setDatabase(database);

    executeQuery(query);
}

void CouchDB::retrieveRevision(const QString &database, const QString &id)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({database, id}));
    query->setOperation(COUCHDB_RETRIEVEREVISION);
    query->setDatabase(database);
    query->setDocumentId(id);

    executeQuery(query);
}

void CouchDB::retrieveDocument(const QString &database, const QString &id)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({database, id}));
    query->setOperation(COUCHDB_RETRIEVEDOCUMENT);
    query->setDatabase(database);
    query->setDocumentId(id);

    executeQuery(query);
}

void CouchDB::updateDocument(const QString &database, const QString &id, QByteArray document)
{
    Q_D(CouchDB);

    QByteArray postDataSize = QByteArray::number(document.size());

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({database, id}));
    query->setOperation(COUCHDB_UPDATEDOCUMENT);
    query->setDatabase(database);
    query->setDocumentId(id);
    query->request()->setRawHeader("Accept", "application/json");
    query->request()->setRawHeader("Content-Type", "application/json");
    query->request()->setRawHeader("Content-Length", postDataSize);
    query->setBody(document);

    executeQuery(query);
}

void CouchDB::deleteDocument(const QString &database, const QString &id, const QString &revision)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({database, id}, revision));
    query->setOperation(COUCHDB_DELETEDOCUMENT);
    query->setDatabase(database);
    query->setDocumentId(id);

    executeQuery(query);
}

void CouchDB::uploadAttachment(const QString &database, const QString &id, const QString &attachmentName,
                               QByteArray attachment, QString mimeType, const QString &revision)
{
    Q_D(CouchDB);

    QByteArray postDataSize = QByteArray::number(attachment.size());

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({database, id, attachmentName}, revision));
    query->setOperation(COUCHDB_DELETEDOCUMENT);
    query->setDatabase(database);
    query->setDocumentId(id);
    query->request()->setRawHeader("Content-Type", mimeType.toLatin1());
    query->request()->setRawHeader("Content-Length", postDataSize);
    query->setBody(attachment);

    executeQuery(query);
}

void CouchDB::deleteAttachment(const QString &database, const QString &id, const QString &attachmentName, const QString &revision)
{
    Q_D(CouchDB);

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(d->queryUrl({database, id, attachmentName}, revision));
    query->setOperation(COUCHDB_DELETEATTACHMENT);
    query->setDatabase(database);
    query->setDocumentId(id);

    executeQuery(query);
}

void CouchDB::replicateDatabaseFrom(const QUrl &sourceServer, const QString &sourceDatabase, const QString &targetDatabase,
                                    bool createTarget, bool continuous, bool cancel)
{
    Q_D(CouchDB);

    QUrl source = sourceServer;
    source.setPath(sourceDatabase);

    QUrl target = d->server;
    target.setPath(targetDatabase);

    replicateDatabase(source, target, targetDatabase, createTarget, continuous, cancel);
}

void CouchDB::replicateDatabaseTo(const QUrl &targetServer, const QString &sourceDatabase, const QString &targetDatabase,
                                    bool createTarget, bool continuous, bool cancel)
{
    Q_D(CouchDB);

    QUrl source = d->server;
    source.setPath(sourceDatabase);

    QUrl target = targetServer;
    target.setPath(targetDatabase);

    replicateDatabase(source, target, targetDatabase, createTarget, continuous, cancel);
}

void CouchDB::replicateDatabase(const QUrl &source, const QUrl &target, const QString &database, bool createTarget,
                                bool continuous, bool cancel)
{
    Q_D(CouchDB);

    if (!cancel)
        qDebug() << "Starting replication from" << source << "to" << target;
    else
        qDebug() << "Cancelling replication from" << source << "to" << target;

    QJsonObject object;
    object.insert("source", source.toString());
    object.insert("target", target.toString());
    object.insert("create_target", createTarget);
    object.insert("continuous", continuous);
    object.insert("cancel", cancel);
    QJsonDocument document(object);

    QUrl url = d->server;
    url.setPath("_replicate");

    CouchDBQuery *query = new CouchDBQuery(this);
    query->setUrl(url);
    query->setOperation(COUCHDB_REPLICATEDATABASE);
    query->setDatabase(database);
    query->request()->setRawHeader("Accept", "application/json");
    query->request()->setRawHeader("Content-Type", "application/json");
    query->setBody(document.toJson());

    executeQuery(query);
}

CouchDBListener *CouchDB::createListener(const QString &database, const QString &documentId)
{
    Q_D(CouchDB);

    CouchDBListener *listener = new CouchDBListener(d->server);
    listener->setCookieJar(d->networkManager->cookieJar());
    d->networkManager->cookieJar()->setParent(nullptr);
    listener->setDatabase(database);
    listener->setDocumentId(documentId);
    listener->launch();

    qDebug() << "Created listener for database:" << database << ", document:" << documentId;

    return listener;
}
