#include "couchdblistener.h"
#include "couchclient.h"
#include "couchclient_p.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QDebug>

class CouchDBListenerPrivate
{
public:
    virtual ~CouchDBListenerPrivate()
    {
        if (retryTimer)
            retryTimer->stop();
        if (reply)
            reply->abort();

        if (reply)
            delete reply;
        if (retryTimer)
            delete retryTimer;

        if (networkManager)
            delete networkManager;
    }

    QUrl server;
    QNetworkAccessManager *networkManager = nullptr;
    QString database;
    QString documentId;
    QNetworkReply *reply = nullptr;
    QTimer *retryTimer = nullptr;
    QMap<QString,QString> parameters;
    QMap<QString,QString> revisionsMap;
};

CouchDBListener::CouchDBListener(const QUrl &server, QObject *parent) :
    QObject(parent),
    d_ptr(new CouchDBListenerPrivate)
{
    Q_D(CouchDBListener);
    d->server = server;
    d->networkManager = new QNetworkAccessManager(this);
    connect(d->networkManager, SIGNAL(finished(QNetworkReply*)),  this, SLOT(listenFinished(QNetworkReply*)));

    d->retryTimer = new QTimer(this);
    d->retryTimer->setInterval(500);
    d->retryTimer->setSingleShot(true);
    connect(d->retryTimer, SIGNAL(timeout()), this, SLOT(start()));

    d->parameters.insert("filter", "app/docFilter");
    d->parameters.insert("feed", "continuous");
    d->parameters.insert("heartbeat", "10000");
    d->parameters.insert("timeout", "60000");
}

CouchDBListener::~CouchDBListener()
{
}

QUrl CouchDBListener::server() const
{
    Q_D(const CouchDBListener);
    return d->server;
}

QString CouchDBListener::database() const
{
    Q_D(const CouchDBListener);
    return d->database;
}

void CouchDBListener::setDatabase(const QString &database)
{
    Q_D(CouchDBListener);
    d->database = database;
}

QString CouchDBListener::documentId() const
{
    Q_D(const CouchDBListener);
    return d->documentId;
}

void CouchDBListener::setDocumentId(const QString &documentId)
{
    Q_D(CouchDBListener);
    d->documentId = documentId;
    d->parameters.insert("name", d->documentId);
}

QString CouchDBListener::revision(const QString &documentId) const
{
    Q_D(const CouchDBListener);

    QString id = d->documentId;
    if (!documentId.isEmpty())
        id = documentId;

    return d->revisionsMap.value(id);
}

void CouchDBListener::setCookieJar(QNetworkCookieJar *cookieJar)
{
    Q_D(CouchDBListener);
    d->networkManager->setCookieJar(cookieJar);
}

void CouchDBListener::setParam(const QString &name, const QString &value)
{
    Q_D(CouchDBListener);
    d->parameters.insert(name, value);
}

void CouchDBListener::launch()
{
    Q_D(CouchDBListener);
    d->retryTimer->start();
}

void CouchDBListener::start()
{
    Q_D(CouchDBListener);

    QUrlQuery urlQuery;
    QMapIterator<QString, QString> i(d->parameters);
    while (i.hasNext()) {
        i.next();
        urlQuery.addQueryItem(i.key(), i.value());
    }

    QUrl url = d->server;
    url.setPath(QStringLiteral("%1/_changes").arg(d->database));
    url.setQuery(urlQuery);

    qDebug() << url;

    QNetworkRequest request(url);
    QString username = d->server.userName();
    QString password = d->server.password();
    if (!username.isEmpty() && !password.isEmpty())
        request.setRawHeader("Authorization", CouchClientPrivate::basicAuth(username, password));

    d->reply = d->networkManager->get(request);
    connect(d->reply, SIGNAL(readyRead()), this, SLOT(readChanges()));
}

void CouchDBListener::readChanges()
{
    Q_D(CouchDBListener);

    const QByteArray replyBA = d->reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(replyBA);

    if (!document.object().contains("changes"))
        return;

    QString revision = document.object().value("changes").toArray().first().toObject().value("rev").toString();
    QString id = d->documentId.isEmpty() ? document.object().value("id").toString() : d->documentId;

    //If the revision is the same as previous changes return
    if (d->revisionsMap.value(id) == revision)
        return;

    d->revisionsMap.insert(id, revision);
    emit changesMade(revision);
}

void CouchDBListener::listenFinished(QNetworkReply *reply)
{
    Q_D(CouchDBListener);

    // Check the network reply for errors.
    QNetworkReply::NetworkError netError = reply->error();
    if (netError != QNetworkReply::NoError) {
        qWarning() << "ERROR";
        switch (netError) {
        case QNetworkReply::ContentNotFoundError:
            qWarning() << "The content was not found on the server";
            break;

        case QNetworkReply::HostNotFoundError:
            qWarning() << "The server was not found";
            break;

        default:
            qWarning() << reply->errorString();
            break;
        }
    }
    reply->deleteLater();
    d->retryTimer->start();
}
