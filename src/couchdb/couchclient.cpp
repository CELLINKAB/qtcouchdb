#include "couchclient.h"
#include "couchrequest.h"
#include "couchresponse.h"

#include <QtCore/qloggingcategory.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>

Q_LOGGING_CATEGORY(lcCouchDB, "qtcouchdb")

class CouchClientPrivate
{
    Q_DECLARE_PUBLIC(CouchClient)

public:
    void queryFinished(QNetworkReply *reply);

    QUrl url;
    CouchClient *q_ptr = nullptr;
    QNetworkAccessManager *networkManager = nullptr;
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
    d->networkManager = new QNetworkAccessManager(this);
    d->networkManager->setAutoDeleteReplies(true);
    connect(d->networkManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) { d->queryFinished(reply); });
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
    d->url = url;
}

static QByteArray basicAuth(const QString &username, const QString &password)
{
    return "Basic " + QByteArray(username.toUtf8() + ":" + password.toUtf8()).toBase64();
}

static QString maskedUrl(QUrl url)
{
    if (!url.password().isEmpty())
        url.setPassword(QStringLiteral("xxxxx"));
    return url.toString();
}

CouchResponse *CouchClient::send(const CouchRequest &request)
{
    Q_D(CouchClient);
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

    qCDebug(lcCouchDB) << request.operation() << maskedUrl(request.url());

    switch (request.operation()) {
    case CouchRequest::Get:
        d->networkManager->get(networkRequest);
        break;
    case CouchRequest::Put:
        d->networkManager->put(networkRequest, request.body());
        break;
    case CouchRequest::Post:
        d->networkManager->post(networkRequest, request.body());
        break;
    case CouchRequest::Delete:
        d->networkManager->deleteResource(networkRequest);
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

    if (reply->error() == QNetworkReply::NoError) {
        response->setData(reply->readAll());
        emit response->received();
        emit q->received(response);
    } else {
        // ### TODO: provide error
        qWarning() << reply->errorString();
    }

    response->deleteLater(); // ### TODO: CouchClient::autoDeleteResponses
}
