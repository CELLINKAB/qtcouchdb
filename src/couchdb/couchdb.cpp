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

void CouchDB::executeQuery(const CouchDBQuery &query)
{
    Q_D(CouchDB);

    QNetworkRequest request(query.url());
    const auto headers = query.headers();
    for (auto it = headers.cbegin(); it != headers.cend(); ++it)
        request.setRawHeader(it.key(), it.value());

    QString username = d->server.userName();
    QString password = d->server.password();
    if (!username.isEmpty() && !password.isEmpty())
        request.setRawHeader("Authorization", CouchDBPrivate::basicAuth(username, password));

    qDebug() << "Invoke url:" << query.operation() << request.url().toString();

    QNetworkReply *reply = nullptr;
    switch (query.operation()) {
    case CouchDBQuery::CheckInstallation:
        reply = d->networkManager->get(request);
        break;
    case CouchDBQuery::StartSession:
        reply = d->networkManager->post(request, query.body());
        break;
    case CouchDBQuery::EndSession:
        reply = d->networkManager->deleteResource(request);
        break;
    case CouchDBQuery::ListDatabases:
        reply = d->networkManager->get(request);
        break;
    case CouchDBQuery::CreateDatabase:
        reply = d->networkManager->put(request, query.body());
        break;
    case CouchDBQuery::DeleteDatabase:
        reply = d->networkManager->deleteResource(request);
        break;
    case CouchDBQuery::ListDocuments:
        reply = d->networkManager->get(request);
        break;
    case CouchDBQuery::RetrieveRevision:
        reply = d->networkManager->head(request);
        break;
    case CouchDBQuery::RetrieveDocument:
        reply = d->networkManager->get(request);
        break;
    case CouchDBQuery::UpdateDocument:
        reply = d->networkManager->put(request, query.body());
        break;
    case CouchDBQuery::DeleteDocument:
        reply = d->networkManager->deleteResource(request);
        break;
    case CouchDBQuery::UploadAttachment:
        reply = d->networkManager->put(request, query.body());
        break;
    case CouchDBQuery::DeleteAttachment:
        reply = d->networkManager->deleteResource(request);
        break;
    case CouchDBQuery::ReplicateDatabase:
        reply = d->networkManager->post(request, query.body());
        break;
    default:
        Q_UNREACHABLE();
        break;
    }

    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
    d->currentQueries.insert(reply, query);
}

void CouchDB::queryFinished()
{
    Q_D(CouchDB);

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    QByteArray data;
    CouchDBQuery query = d->currentQueries.value(reply);
    bool hasError = false;
    if (reply->error() == QNetworkReply::NoError) {
        data = reply->readAll();
    } else {
        qWarning() << reply->errorString();
        hasError = true;
    }

    QJsonDocument json = QJsonDocument::fromJson(data);
    CouchDBResponse response = CouchDBResponse::fromJson(json, query);
    response.setStatus(hasError || (query.operation() != CouchDBQuery::CheckInstallation && query.operation() != CouchDBQuery::RetrieveDocument &&
            !json["ok"].toBool()) ? CouchDBResponse::Error : CouchDBResponse::Success);

    switch (query.operation()) {
    case CouchDBQuery::CheckInstallation:
    default:
        if (!hasError)
            response.setStatus(!json["couchdb"].isUndefined() ? CouchDBResponse::Success : CouchDBResponse::Error);
        emit installationChecked(response);
        break;
    case CouchDBQuery::StartSession:
        if (hasError && reply->error() >= 201 && reply->error() <= 299)
            response.setStatus(CouchDBResponse::AuthError);
        emit sessionStarted(response);
        break;
    case CouchDBQuery::EndSession:
        emit sessionEnded(response);
        break;
    case CouchDBQuery::ListDatabases:
        emit databasesListed(response);
        break;
    case CouchDBQuery::CreateDatabase:
        emit databaseCreated(response);
        break;
    case CouchDBQuery::DeleteDatabase:
        emit databaseDeleted(response);
        break;
    case CouchDBQuery::ListDocuments:
        emit documentsListed(response);
        break;
    case CouchDBQuery::RetrieveRevision: {
        QString revision = reply->rawHeader("ETag");
        revision.remove("\"");
        response.setRevision(revision);
        emit revisionRetrieved(response);
        break;
    }
    case CouchDBQuery::RetrieveDocument:
        emit documentRetrieved(response);
        break;
    case CouchDBQuery::UpdateDocument:
        emit documentUpdated(response);
        break;
    case CouchDBQuery::DeleteDocument:
        emit documentDeleted(response);
        break;
    case CouchDBQuery::UploadAttachment:
        emit attachmentUploaded(response);
        break;
    case CouchDBQuery::DeleteAttachment:
        emit attachmentDeleted(response);
        break;
    case CouchDBQuery::ReplicateDatabase:
        emit databaseReplicated(response);
        break;
    }

    d->currentQueries.remove(reply);
    reply->deleteLater();
}

void CouchDB::checkInstallation()
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::CheckInstallation);
    query.setUrl(d->server);

    executeQuery(query);
}

void CouchDB::startSession(const QString &username, const QString &password)
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::StartSession);
    query.setUrl(d->queryUrl({"_session"}));
    query.setBody(QUrlQuery({{"name", username}, {"password", password}}).toString(QUrl::FullyEncoded).toUtf8());
    query.setHeader("Accept", "application/json");
    query.setHeader("Content-Type", "application/x-www-form-urlencoded");

    executeQuery(query);
}

void CouchDB::endSession()
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::EndSession);
    query.setUrl(d->queryUrl({"_session"}));

    executeQuery(query);
}

void CouchDB::listDatabases()
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::ListDatabases);
    query.setUrl(d->queryUrl({"_all_dbs"}));

    executeQuery(query);
}

void CouchDB::createDatabase(const QString &database)
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::CreateDatabase);
    query.setUrl(d->queryUrl({database}));
    query.setDatabase(database);

    executeQuery(query);
}

void CouchDB::deleteDatabase(const QString &database)
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::DeleteDatabase);
    query.setUrl(d->queryUrl({database}));
    query.setDatabase(database);

    executeQuery(query);
}

void CouchDB::listDocuments(const QString &database)
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::ListDocuments);
    query.setUrl(d->queryUrl({database, "_all_docs"}));
    query.setDatabase(database);

    executeQuery(query);
}

void CouchDB::retrieveRevision(const QString &database, const QString &id)
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::RetrieveRevision);
    query.setUrl(d->queryUrl({database, id}));
    query.setDatabase(database);
    query.setDocumentId(id);

    executeQuery(query);
}

void CouchDB::retrieveDocument(const QString &database, const QString &id)
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::RetrieveDocument);
    query.setUrl(d->queryUrl({database, id}));
    query.setDatabase(database);
    query.setDocumentId(id);

    executeQuery(query);
}

void CouchDB::updateDocument(const QString &database, const QString &id, const QByteArray &document)
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::UpdateDocument);
    query.setUrl(d->queryUrl({database, id}));
    query.setDatabase(database);
    query.setDocumentId(id);
    query.setHeader("Accept", "application/json");
    query.setHeader("Content-Type", "application/json");
    query.setHeader("Content-Length", QByteArray::number(document.size()));
    query.setBody(document);

    executeQuery(query);
}

void CouchDB::deleteDocument(const QString &database, const QString &id, const QString &revision)
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::DeleteDocument);
    query.setUrl(d->queryUrl({database, id}, revision));
    query.setDatabase(database);
    query.setDocumentId(id);

    executeQuery(query);
}

void CouchDB::uploadAttachment(const QString &database, const QString &id, const QString &attachmentName,
                               const QByteArray &attachment, const QString &mimeType, const QString &revision)
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::DeleteDocument);
    query.setUrl(d->queryUrl({database, id, attachmentName}, revision));
    query.setDatabase(database);
    query.setDocumentId(id);
    query.setHeader("Content-Type", mimeType.toLatin1());
    query.setHeader("Content-Length", QByteArray::number(attachment.size()));
    query.setBody(attachment);

    executeQuery(query);
}

void CouchDB::deleteAttachment(const QString &database, const QString &id, const QString &attachmentName, const QString &revision)
{
    Q_D(CouchDB);

    CouchDBQuery query(CouchDBQuery::DeleteAttachment);
    query.setUrl(d->queryUrl({database, id, attachmentName}, revision));
    query.setDatabase(database);
    query.setDocumentId(id);

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
    url.setPath("/_replicate");

    CouchDBQuery query(CouchDBQuery::ReplicateDatabase);
    query.setUrl(url);
    query.setDatabase(database);
    query.setHeader("Accept", "application/json");
    query.setHeader("Content-Type", "application/json");
    query.setBody(document.toJson());

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
