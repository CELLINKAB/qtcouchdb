#include "couchclient.h"
#include "couchclient_p.h"
#include "couchquery.h"
#include "couchresponse.h"
#include "couchdblistener.h"

#include <QtCore/qloggingcategory.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>

Q_LOGGING_CATEGORY(lcCouchDB, "qtcouchdb")

CouchClient::CouchClient(QObject *parent) :
    QObject(parent),
    d_ptr(new CouchClientPrivate)
{
    Q_D(CouchClient);
    d->q_ptr = this;
    d->networkManager.reset(new QNetworkAccessManager(this));
}

CouchClient::~CouchClient()
{
}

QUrl CouchClient::server() const
{
    Q_D(const CouchClient);
    return d->server;
}

void CouchClient::setServer(const QUrl &server)
{
    Q_D(CouchClient);
    d->server = server;
}

void CouchClient::executeQuery(const CouchQuery &query)
{
    Q_D(CouchClient);

    QNetworkRequest request(query.url());
    const auto headers = query.headers();
    for (auto it = headers.cbegin(); it != headers.cend(); ++it)
        request.setRawHeader(it.key(), it.value());

    QString username = d->server.userName();
    QString password = d->server.password();
    if (!username.isEmpty() && !password.isEmpty())
        request.setRawHeader("Authorization", CouchClientPrivate::basicAuth(username, password));

    qCDebug(lcCouchDB) << "Invoke url:" << query.operation() << request.url().toString();

    QNetworkReply *reply = nullptr;
    switch (query.operation()) {
    case CouchQuery::CheckInstallation:
        reply = d->networkManager->get(request);
        break;
    case CouchQuery::StartSession:
        reply = d->networkManager->post(request, query.body());
        break;
    case CouchQuery::EndSession:
        reply = d->networkManager->deleteResource(request);
        break;
    case CouchQuery::ListDatabases:
        reply = d->networkManager->get(request);
        break;
    case CouchQuery::CreateDatabase:
        reply = d->networkManager->put(request, query.body());
        break;
    case CouchQuery::DeleteDatabase:
        reply = d->networkManager->deleteResource(request);
        break;
    case CouchQuery::ListDocuments:
        reply = d->networkManager->get(request);
        break;
    case CouchQuery::RetrieveRevision:
        reply = d->networkManager->head(request);
        break;
    case CouchQuery::RetrieveDocument:
        reply = d->networkManager->get(request);
        break;
    case CouchQuery::UpdateDocument:
        reply = d->networkManager->put(request, query.body());
        break;
    case CouchQuery::DeleteDocument:
        reply = d->networkManager->deleteResource(request);
        break;
    case CouchQuery::UploadAttachment:
        reply = d->networkManager->put(request, query.body());
        break;
    case CouchQuery::DeleteAttachment:
        reply = d->networkManager->deleteResource(request);
        break;
    case CouchQuery::ReplicateDatabase:
        reply = d->networkManager->post(request, query.body());
        break;
    default:
        Q_UNREACHABLE();
        break;
    }

    connect(reply, &QNetworkReply::finished, [=]() { d->queryFinished(); });
    d->currentQueries.insert(reply, query);
}

void CouchClientPrivate::queryFinished()
{
    Q_Q(CouchClient);

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(q->sender());
    if (!reply)
        return;

    QByteArray data;
    CouchQuery query = currentQueries.value(reply);
    bool hasError = false;
    if (reply->error() == QNetworkReply::NoError) {
        data = reply->readAll();
    } else {
        qWarning() << reply->errorString();
        hasError = true;
    }

    QJsonDocument json = QJsonDocument::fromJson(data);
    CouchResponse response = CouchResponse::fromJson(json, query);
    response.setStatus(hasError || (query.operation() != CouchQuery::CheckInstallation && query.operation() != CouchQuery::RetrieveDocument &&
            !json["ok"].toBool()) ? CouchResponse::Error : CouchResponse::Success);

    switch (query.operation()) {
    case CouchQuery::CheckInstallation:
    default:
        if (!hasError)
            response.setStatus(!json["couchdb"].isUndefined() ? CouchResponse::Success : CouchResponse::Error);
        emit q->installationChecked(response);
        break;
    case CouchQuery::StartSession:
        if (hasError && reply->error() >= 201 && reply->error() <= 299)
            response.setStatus(CouchResponse::AuthError);
        emit q->sessionStarted(response);
        break;
    case CouchQuery::EndSession:
        emit q->sessionEnded(response);
        break;
    case CouchQuery::ListDatabases:
        emit q->databasesListed(response);
        break;
    case CouchQuery::CreateDatabase:
        emit q->databaseCreated(response);
        break;
    case CouchQuery::DeleteDatabase:
        emit q->databaseDeleted(response);
        break;
    case CouchQuery::ListDocuments:
        emit q->documentsListed(response);
        break;
    case CouchQuery::RetrieveRevision: {
        QString revision = reply->rawHeader("ETag");
        revision.remove("\"");
        response.setRevision(revision);
        emit q->revisionRetrieved(response);
        break;
    }
    case CouchQuery::RetrieveDocument:
        emit q->documentRetrieved(response);
        break;
    case CouchQuery::UpdateDocument:
        emit q->documentUpdated(response);
        break;
    case CouchQuery::DeleteDocument:
        emit q->documentDeleted(response);
        break;
    case CouchQuery::UploadAttachment:
        emit q->attachmentUploaded(response);
        break;
    case CouchQuery::DeleteAttachment:
        emit q->attachmentDeleted(response);
        break;
    case CouchQuery::ReplicateDatabase:
        emit q->databaseReplicated(response);
        break;
    }

    currentQueries.remove(reply);
    reply->deleteLater();
}

void CouchClient::checkInstallation()
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::CheckInstallation);
    query.setUrl(d->server);

    executeQuery(query);
}

void CouchClient::startSession(const QString &username, const QString &password)
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::StartSession);
    query.setUrl(d->queryUrl({"_session"}));
    query.setBody(QUrlQuery({{"name", username}, {"password", password}}).toString(QUrl::FullyEncoded).toUtf8());
    query.setHeader("Accept", "application/json");
    query.setHeader("Content-Type", "application/x-www-form-urlencoded");

    executeQuery(query);
}

void CouchClient::endSession()
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::EndSession);
    query.setUrl(d->queryUrl({"_session"}));

    executeQuery(query);
}

void CouchClient::listDatabases()
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::ListDatabases);
    query.setUrl(d->queryUrl({"_all_dbs"}));

    executeQuery(query);
}

void CouchClient::createDatabase(const QString &database)
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::CreateDatabase);
    query.setUrl(d->queryUrl({database}));
    query.setDatabase(database);

    executeQuery(query);
}

void CouchClient::deleteDatabase(const QString &database)
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::DeleteDatabase);
    query.setUrl(d->queryUrl({database}));
    query.setDatabase(database);

    executeQuery(query);
}

void CouchClient::listDocuments(const QString &database)
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::ListDocuments);
    query.setUrl(d->queryUrl({database, "_all_docs"}));
    query.setDatabase(database);

    executeQuery(query);
}

void CouchClient::retrieveRevision(const QString &database, const QString &id)
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::RetrieveRevision);
    query.setUrl(d->queryUrl({database, id}));
    query.setDatabase(database);
    query.setDocumentId(id);

    executeQuery(query);
}

void CouchClient::retrieveDocument(const QString &database, const QString &id)
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::RetrieveDocument);
    query.setUrl(d->queryUrl({database, id}));
    query.setDatabase(database);
    query.setDocumentId(id);

    executeQuery(query);
}

void CouchClient::updateDocument(const QString &database, const QString &id, const QByteArray &document)
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::UpdateDocument);
    query.setUrl(d->queryUrl({database, id}));
    query.setDatabase(database);
    query.setDocumentId(id);
    query.setHeader("Accept", "application/json");
    query.setHeader("Content-Type", "application/json");
    query.setHeader("Content-Length", QByteArray::number(document.size()));
    query.setBody(document);

    executeQuery(query);
}

void CouchClient::deleteDocument(const QString &database, const QString &id, const QString &revision)
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::DeleteDocument);
    query.setUrl(d->queryUrl({database, id}, revision));
    query.setDatabase(database);
    query.setDocumentId(id);

    executeQuery(query);
}

void CouchClient::uploadAttachment(const QString &database, const QString &id, const QString &attachmentName,
                               const QByteArray &attachment, const QString &mimeType, const QString &revision)
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::DeleteDocument);
    query.setUrl(d->queryUrl({database, id, attachmentName}, revision));
    query.setDatabase(database);
    query.setDocumentId(id);
    query.setHeader("Content-Type", mimeType.toLatin1());
    query.setHeader("Content-Length", QByteArray::number(attachment.size()));
    query.setBody(attachment);

    executeQuery(query);
}

void CouchClient::deleteAttachment(const QString &database, const QString &id, const QString &attachmentName, const QString &revision)
{
    Q_D(CouchClient);

    CouchQuery query(CouchQuery::DeleteAttachment);
    query.setUrl(d->queryUrl({database, id, attachmentName}, revision));
    query.setDatabase(database);
    query.setDocumentId(id);

    executeQuery(query);
}

void CouchClient::replicateDatabaseFrom(const QUrl &sourceServer, const QString &sourceDatabase, const QString &targetDatabase,
                                    bool createTarget, bool continuous, bool cancel)
{
    Q_D(CouchClient);

    QUrl source = sourceServer;
    source.setPath(sourceDatabase);

    QUrl target = d->server;
    target.setPath(targetDatabase);

    d->replicateDatabase(source, target, targetDatabase, createTarget, continuous, cancel);
}

void CouchClient::replicateDatabaseTo(const QUrl &targetServer, const QString &sourceDatabase, const QString &targetDatabase,
                                  bool createTarget, bool continuous, bool cancel)
{
    Q_D(CouchClient);

    QUrl source = d->server;
    source.setPath(sourceDatabase);

    QUrl target = targetServer;
    target.setPath(targetDatabase);

    d->replicateDatabase(source, target, targetDatabase, createTarget, continuous, cancel);
}

void CouchClientPrivate::replicateDatabase(const QUrl &source, const QUrl &target, const QString &database, bool createTarget,
                                           bool continuous, bool cancel)
{
    Q_Q(CouchClient);

    if (!cancel)
        qCDebug(lcCouchDB) << "Starting replication from" << source << "to" << target;
    else
        qCDebug(lcCouchDB) << "Cancelling replication from" << source << "to" << target;

    QJsonObject object;
    object.insert("source", source.toString());
    object.insert("target", target.toString());
    object.insert("create_target", createTarget);
    object.insert("continuous", continuous);
    object.insert("cancel", cancel);
    QJsonDocument document(object);

    QUrl url = server;
    url.setPath("/_replicate");

    CouchQuery query(CouchQuery::ReplicateDatabase);
    query.setUrl(url);
    query.setDatabase(database);
    query.setHeader("Accept", "application/json");
    query.setHeader("Content-Type", "application/json");
    query.setBody(document.toJson());

    q->executeQuery(query);
}

CouchDBListener *CouchClient::createListener(const QString &database, const QString &documentId)
{
    Q_D(CouchClient);

    CouchDBListener *listener = new CouchDBListener(d->server);
    listener->setCookieJar(d->networkManager->cookieJar());
    d->networkManager->cookieJar()->setParent(nullptr);
    listener->setDatabase(database);
    listener->setDocumentId(documentId);
    listener->launch();

    qCDebug(lcCouchDB) << "Created listener for database:" << database << ", document:" << documentId;

    return listener;
}
