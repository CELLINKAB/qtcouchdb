#include "couchclient.h"
#include "couchclient_p.h"
#include "couchrequest.h"
#include "couchresponse.h"
#include "couchurl_p.h"

#include <QtCore/qloggingcategory.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>

Q_LOGGING_CATEGORY(lcCouchDB, "qtcouchdb")

CouchClient::CouchClient(QObject *parent) : CouchClient(QUrl(), parent)
{
}

CouchClient::CouchClient(const QUrl &serverUrl, QObject *parent) :
    QObject(parent),
    d_ptr(new CouchClientPrivate)
{
    Q_D(CouchClient);
    d->q_ptr = this;
    d->serverUrl = serverUrl;
    d->networkManager = new QNetworkAccessManager(this);
    d->networkManager->setAutoDeleteReplies(true);
    connect(d->networkManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) { d->queryFinished(reply); });
}

CouchClient::~CouchClient()
{
}

QUrl CouchClient::serverUrl() const
{
    Q_D(const CouchClient);
    return d->serverUrl;
}

void CouchClient::setServerUrl(const QUrl &serverUrl)
{
    Q_D(CouchClient);
    d->serverUrl = serverUrl;
}

QUrl CouchClient::databaseUrl(const QString &databaseName) const
{
    Q_D(const CouchClient);
    return CouchUrl::resolve(d->serverUrl, databaseName);
}

QUrl CouchClient::documentUrl(const QString &databaseName, const QString &documentId, const QString &revision) const
{
    return CouchUrl::resolve(databaseUrl(databaseName), documentId, revision);
}

QUrl CouchClient::attachmentUrl(const QString &databaseName, const QString &documentId, const QString &attachmentName, const QString &revision) const
{
    return CouchUrl::resolve(documentUrl(databaseName, documentId), attachmentName, revision);
}

CouchResponse *CouchClient::sendRequest(const CouchRequest &request)
{
    Q_D(CouchClient);
    CouchResponse *response = new CouchResponse(request, this);

    QNetworkRequest networkRequest(request.url());
    networkRequest.setOriginatingObject(response);

    const auto headers = request.headers();
    for (auto it = headers.cbegin(); it != headers.cend(); ++it)
        networkRequest.setRawHeader(it.key(), it.value());

    QString username = d->serverUrl.userName();
    QString password = d->serverUrl.password();
    if (!username.isEmpty() && !password.isEmpty())
        networkRequest.setRawHeader("Authorization", CouchClientPrivate::basicAuth(username, password));

    qCDebug(lcCouchDB) << "invoke:" << request.operation() << networkRequest.url().toString();

    switch (request.operation()) {
    case CouchRequest::CheckInstallation:
    case CouchRequest::ListDatabases:
    case CouchRequest::ListDocuments:
    case CouchRequest::RetrieveDocument:
        d->networkManager->get(networkRequest);
        break;
    case CouchRequest::StartSession:
    case CouchRequest::ReplicateDatabase:
        d->networkManager->post(networkRequest, request.body());
        break;
    case CouchRequest::EndSession:
    case CouchRequest::DeleteDatabase:
    case CouchRequest::DeleteDocument:
    case CouchRequest::DeleteAttachment:
        d->networkManager->deleteResource(networkRequest);
        break;
    case CouchRequest::CreateDatabase:
    case CouchRequest::UpdateDocument:
    case CouchRequest::UploadAttachment:
        d->networkManager->put(networkRequest, request.body());
        break;
    case CouchRequest::RetrieveRevision:
        d->networkManager->head(networkRequest);
        break;
    default:
        Q_UNREACHABLE();
        break;
    }

    return response;
}

void CouchClientPrivate::queryFinished(QNetworkReply *reply)
{
    Q_Q(CouchClient);
    CouchResponse *response = qobject_cast<CouchResponse *>(reply->request().originatingObject());
    if (!response)
        return;

    QByteArray data;
    bool hasError = false;
    if (reply->error() == QNetworkReply::NoError) {
        data = reply->readAll();
    } else {
        qWarning() << reply->errorString();
        hasError = true;
    }
    response->setData(data);

    CouchRequest request = response->request();
    QJsonDocument json = QJsonDocument::fromJson(data);
    response->setRevision(json.object().value("revision").toString());
    response->setStatus(hasError || (request.operation() != CouchRequest::CheckInstallation && request.operation() != CouchRequest::RetrieveDocument &&
            !json["ok"].toBool()) ? CouchResponse::Error : CouchResponse::Success);

    switch (request.operation()) {
    case CouchRequest::CheckInstallation:
        if (!hasError)
            response->setStatus(!json["couchdb"].isUndefined() ? CouchResponse::Success : CouchResponse::Error);
        break;
    case CouchRequest::StartSession:
        if (hasError && reply->error() >= 201 && reply->error() <= 299)
            response->setStatus(CouchResponse::AuthError);
        break;
    case CouchRequest::RetrieveRevision:
        response->setRevision(QString::fromUtf8(reply->rawHeader("ETag")).remove("\""));
        break;
    default:
        break;
    }

    emit response->received();
    emit q->responseReceived(response);

    response->deleteLater(); // ### TODO: CouchClient::autoDeleteResponses
}
