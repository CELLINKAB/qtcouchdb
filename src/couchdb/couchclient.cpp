#include "couchclient.h"
#include "couch.h"
#include "couchrequest.h"
#include "couchresponse.h"

#include <QtCore/qloggingcategory.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>

Q_LOGGING_CATEGORY(lcCouchDB, "qtcouchdb", QtWarningMsg)

class CouchClientPrivate
{
    Q_DECLARE_PUBLIC(CouchClient)

public:
    void queryFinished(QNetworkReply *reply);

    QUrl baseUrl;
    int activeRequests = 0;
    CouchClient *q_ptr = nullptr;
    QNetworkAccessManager *networkAccessManager = nullptr;
};

CouchClient::CouchClient(QObject *parent) : CouchClient(QUrl(), parent)
{
}

CouchClient::CouchClient(const QUrl &url, QObject *parent) :
    QObject(parent),
    d_ptr(new CouchClientPrivate)
{
    Q_D(CouchClient);
    d->q_ptr = this;
    d->baseUrl = url;
    setNetworkAccessManager(new QNetworkAccessManager(this));
}

CouchClient::~CouchClient()
{
}

QUrl CouchClient::baseUrl() const
{
    Q_D(const CouchClient);
    return d->baseUrl;
}

void CouchClient::setBaseUrl(const QUrl &baseUrl)
{
    Q_D(CouchClient);
    if (d->baseUrl == baseUrl)
        return;

    d->baseUrl = baseUrl;
    emit baseUrlChanged(baseUrl);
}

bool CouchClient::isBusy() const
{
    Q_D(const CouchClient);
    return d->activeRequests > 0;
}

QNetworkAccessManager *CouchClient::networkAccessManager() const
{
    Q_D(const CouchClient);
    return d->networkAccessManager;
}

void CouchClient::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
    Q_D(CouchClient);
    if (!networkAccessManager)
        return;

    if (d->networkAccessManager) {
        if (d->networkAccessManager->parent() == this)
            d->networkAccessManager->deleteLater();
        else
            d->networkAccessManager->disconnect(this);
    }

    d->networkAccessManager = networkAccessManager;
    connect(networkAccessManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) { d->queryFinished(reply); });
}

static QStringList toDatabaseList(const QJsonArray &array)
{
    QStringList databases;
    for (const QJsonValue &value : array)
        databases += value.toString();
    return databases;
}

bool CouchClient::listDatabases()
{
    Q_D(CouchClient);
    CouchRequest request = Couch::listDatabases(d->baseUrl);
    CouchResponse *response = sendRequest(request);
    if (!response)
        return false;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        QJsonArray json = QJsonDocument::fromJson(data).array();
        emit databasesListed(toDatabaseList(json));
    });
    return true;
}

bool CouchClient::createDatabase(const QString &database)
{
    Q_D(CouchClient);
    CouchRequest request = Couch::createDatabase(Couch::databaseUrl(d->baseUrl, database));
    CouchResponse *response = sendRequest(request);
    if (!response)
        return false;

    connect(response, &CouchResponse::received, [=](const QByteArray &) {
        emit databaseCreated(database);
    });
    return true;
}

bool CouchClient::deleteDatabase(const QString &database)
{
    Q_D(CouchClient);
    CouchRequest request = Couch::deleteDatabase(Couch::databaseUrl(d->baseUrl, database));
    CouchResponse *response = sendRequest(request);
    if (!response)
        return false;

    connect(response, &CouchResponse::received, [=](const QByteArray &) {
        emit databaseDeleted(database);
    });
    return true;
}

static QByteArray basicAuth(const QString &username, const QString &password)
{
    return "Basic " + QByteArray(username.toUtf8() + ":" + password.toUtf8()).toBase64();
}

CouchResponse *CouchClient::sendRequest(const CouchRequest &request)
{
    Q_D(CouchClient);
    if (!request.isValid())
        return nullptr;

    CouchResponse *response = new CouchResponse(request, this);
    QNetworkRequest networkRequest(request.url());
    networkRequest.setOriginatingObject(response);

    const auto headers = request.headers();
    for (auto it = headers.cbegin(); it != headers.cend(); ++it)
        networkRequest.setRawHeader(it.key(), it.value());

    QString username = d->baseUrl.userName();
    QString password = d->baseUrl.password();
    if (!username.isEmpty() && !password.isEmpty())
        networkRequest.setRawHeader("Authorization", basicAuth(username, password));

    QByteArray body = request.body();
    if (!body.isEmpty()) {
        networkRequest.setRawHeader("Accept", "application/json");
        networkRequest.setRawHeader("Content-Type", "application/json");
        networkRequest.setRawHeader("Content-Length", QByteArray::number(body.size()));
    }

    qCDebug(lcCouchDB) << request;

    switch (request.operation()) {
    case CouchRequest::Get:
        d->networkAccessManager->get(networkRequest);
        break;
    case CouchRequest::Put:
        d->networkAccessManager->put(networkRequest, request.body());
        break;
    case CouchRequest::Post:
        d->networkAccessManager->post(networkRequest, request.body());
        break;
    case CouchRequest::Delete:
        d->networkAccessManager->deleteResource(networkRequest);
        break;
    // LCOV_EXCL_START
    default:
        Q_UNREACHABLE();
        break;
    // LCOV_EXCL_STOP
    }

    if (++d->activeRequests == 1)
        emit busyChanged(true);

    return response;
}

void CouchClientPrivate::queryFinished(QNetworkReply *reply)
{
    Q_Q(CouchClient);
    CouchResponse *response = qobject_cast<CouchResponse *>(reply->request().originatingObject());
    Q_ASSERT(response);

    QByteArray data = reply->readAll();
    response->setData(data);

    QNetworkReply::NetworkError networkError = reply->error();

    if (networkError == QNetworkReply::NoError) {
        emit response->received(data);
        emit q->responseReceived(response);
    } else {
        QByteArray key = QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(networkError);
        CouchError error(QString::fromLatin1(key), reply->errorString());
        emit response->errorOccurred(error);
        emit q->errorOccurred(error);
    }

    reply->deleteLater();
    response->deleteLater(); // ### TODO: CouchClient::autoDeleteResponses

    if (--activeRequests == 0)
        emit q->busyChanged(false);
}
