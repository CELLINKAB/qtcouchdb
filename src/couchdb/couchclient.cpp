﻿#include "couchclient.h"
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
    d->baseUrl = url;
    d->networkManager = new QNetworkAccessManager(this);
    d->networkManager->setAutoDeleteReplies(true);
    connect(d->networkManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) { d->queryFinished(reply); });
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

static QStringList toDatabaseList(const QJsonArray &array)
{
    QStringList databases;
    for (const QJsonValue &value : array)
        databases += value.toString();
    return databases;
}

CouchResponse *CouchClient::listAllDatabases()
{
    Q_D(CouchClient);
    CouchRequest request = Couch::listAllDatabases(d->baseUrl);
    CouchResponse *response = sendRequest(request);
    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        QJsonArray json = QJsonDocument::fromJson(data).array();
        emit databasesListed(toDatabaseList(json));
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

    QByteArray data = reply->readAll();
    response->setData(data);

    if (reply->error() == QNetworkReply::NoError) {
        emit response->received(data);
        emit q->responseReceived(response);
    } else {
        QJsonDocument json = QJsonDocument::fromJson(data);
        CouchError error = CouchError::fromJson(json.object());
        emit response->errorOccurred(error);
        emit q->errorOccurred(error);
    }

    response->deleteLater(); // ### TODO: CouchClient::autoDeleteResponses
}
