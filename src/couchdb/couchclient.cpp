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

    QUrl url;
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
    d->url = url;
    setNetworkAccessManager(new QNetworkAccessManager(this));
}

CouchClient::~CouchClient()
{
}

QUrl CouchClient::url() const
{
    Q_D(const CouchClient);
    return d->url;
}

void CouchClient::setUrl(const QUrl &url)
{
    Q_D(CouchClient);
    if (d->url == url)
        return;

    d->url = url;
    emit urlChanged(url);
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

CouchResponse *CouchClient::listDatabases()
{
    Q_D(CouchClient);
    CouchRequest request = Couch::listDatabases(d->url);
    CouchResponse *response = sendRequest(request);
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit databasesListed(Couch::toDatabaseList(data));
    });
    return response;
}

CouchResponse *CouchClient::createDatabase(const QString &database)
{
    Q_D(CouchClient);
    CouchRequest request = Couch::createDatabase(Couch::databaseUrl(d->url, database));
    CouchResponse *response = sendRequest(request);
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit databaseCreated(Couch::toDatabase(data));
    });
    return response;
}

CouchResponse *CouchClient::deleteDatabase(const QString &database)
{
    Q_D(CouchClient);
    CouchRequest request = Couch::deleteDatabase(Couch::databaseUrl(d->url, database));
    CouchResponse *response = sendRequest(request);
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit databaseDeleted(Couch::toDatabase(data));
    });
    return response;
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

    QString username = d->url.userName();
    QString password = d->url.password();
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
