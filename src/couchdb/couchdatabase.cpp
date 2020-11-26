#include "couchdatabase.h"
#include "couchclient.h"
#include "couchrequest.h"
#include "couchresponse.h"

class CouchDatabasePrivate
{
    Q_DECLARE_PUBLIC(CouchDatabase)

public:
    CouchResponse *response(CouchResponse *response)
    {
        Q_Q(CouchDatabase);
        QObject::connect(response, &CouchResponse::errorOccurred, [=](const CouchError &error) {
            emit q->errorOccurred(error);
        });
        return response;
    }

    CouchDatabase *q_ptr = nullptr;
    QString name;
    CouchClient *client = nullptr;
};

CouchDatabase::CouchDatabase(QObject *parent)
    : CouchDatabase(QString(), nullptr, parent)
{
}

CouchDatabase::CouchDatabase(const QString &name, CouchClient *client, QObject *parent)
    : QObject(parent),
    d_ptr(new CouchDatabasePrivate)
{
    Q_D(CouchDatabase);
    d->q_ptr = this;
    d->name = name;
    d->client = client;
}

CouchDatabase::~CouchDatabase()
{
}

QUrl CouchDatabase::url() const
{
    Q_D(const CouchDatabase);
    if (!d->client || d->name.isEmpty())
        return QUrl();

    return Couch::databaseUrl(d->client->baseUrl(), d->name);
}

QString CouchDatabase::name() const
{
    Q_D(const CouchDatabase);
    return d->name;
}

void CouchDatabase::setName(const QString &name)
{
    Q_D(CouchDatabase);
    if (d->name == name)
        return;

    d->name = name;
    emit urlChanged(url());
    emit nameChanged(name);
}

CouchClient *CouchDatabase::client() const
{
    Q_D(const CouchDatabase);
    return d->client;
}

void CouchDatabase::setClient(CouchClient *client)
{
    Q_D(CouchDatabase);
    if (d->client == client)
        return;

    d->client = client;
    emit urlChanged(url());
    emit clientChanged(client);
}

CouchResponse *CouchDatabase::createDatabase()
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::createDatabase(url());
    CouchResponse *response = d->client->sendRequest(request);
    connect(response, &CouchResponse::received, [=](const QByteArray &) {
        emit databaseCreated();
    });
    return d->response(response);
}

CouchResponse *CouchDatabase::deleteDatabase()
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::deleteDatabase(url());
    CouchResponse *response = d->client->sendRequest(request);
    connect(response, &CouchResponse::received, [=](const QByteArray &) {
        emit databaseDeleted();
    });
    return d->response(response);
}

static QString trimPrefix(QString str, const QString &prefix)
{
    if (!str.startsWith(prefix))
        return str;

    return str.mid(prefix.length());
}

static QString toDesignDocumentName(const QJsonObject &json)
{
    QString key = json.value(QStringLiteral("key")).toString();
    return trimPrefix(key, QStringLiteral("_design/"));
}

static QStringList toDesignDocumentList(const QJsonArray &json)
{
    QStringList designs;
    for (const QJsonValue &value : json)
        designs += toDesignDocumentName(value.toObject());
    return designs;
}

CouchResponse *CouchDatabase::listAllDesignDocuments()
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::listAllDesignDocuments(url());
    CouchResponse *response = d->client->sendRequest(request);
    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        QJsonDocument json = QJsonDocument::fromJson(data);
        QJsonArray rows = json.object().value(QStringLiteral("rows")).toArray();
        emit designDocumentsListed(toDesignDocumentList(rows));
    });
    return d->response(response);
}

static QList<CouchDocument> toDocumentList(const QJsonArray &json)
{
    QList<CouchDocument> docs;
    for (const QJsonValue &value : json)
        docs += CouchDocument::fromJson(value.toObject());
    return docs;
}

CouchResponse *CouchDatabase::listAllDocuments()
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::listAllDocuments(url());
    CouchResponse *response = d->client->sendRequest(request);
    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        QJsonDocument json = QJsonDocument::fromJson(data);
        QJsonArray rows = json.object().value(QStringLiteral("rows")).toArray();
        emit documentsListed(toDocumentList(rows));
    });
    return d->response(response);
}

CouchResponse *CouchDatabase::queryDocuments(const CouchQuery &query)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::queryDocuments(url(), query);
    CouchResponse *response = d->client->sendRequest(request);
    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        QJsonDocument json = QJsonDocument::fromJson(data);
        QJsonArray rows = json.object().value(QStringLiteral("rows")).toArray();
        emit documentsListed(toDocumentList(rows));
    });
    return d->response(response);
}

CouchResponse *CouchDatabase::createDocument(const QByteArray &content)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::createDocument(url(), content);
    CouchResponse *response = d->client->sendRequest(request);
    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        QJsonDocument json = QJsonDocument::fromJson(data);
        emit documentCreated(CouchDocument::fromJson(json.object()));
    });
    return d->response(response);
}

CouchResponse *CouchDatabase::getDocument(const CouchDocument &document)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::getDocument(url(), document.id(), document.revision());
    CouchResponse *response = d->client->sendRequest(request);
    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        QJsonDocument json = QJsonDocument::fromJson(data);
        emit documentReceived(CouchDocument::fromJson(json.object()));
    });
    return d->response(response);
}

CouchResponse *CouchDatabase::updateDocument(const CouchDocument &document)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::updateDocument(url(), document.id(), document.revision(), document.content());
    CouchResponse *response = d->client->sendRequest(request);
    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        QJsonDocument json = QJsonDocument::fromJson(data);
        emit documentUpdated(CouchDocument::fromJson(json.object()));
    });
    return d->response(response);
}

CouchResponse *CouchDatabase::deleteDocument(const CouchDocument &document)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::deleteDocument(url(), document.id(), document.revision());
    CouchResponse *response = d->client->sendRequest(request);
    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        QJsonDocument json = QJsonDocument::fromJson(data);
        emit documentDeleted(CouchDocument::fromJson(json.object()));
    });
    return d->response(response);
}
