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
    setClient(client);
}

CouchDatabase::~CouchDatabase()
{
}

QUrl CouchDatabase::url() const
{
    Q_D(const CouchDatabase);
    if (!d->client || d->name.isEmpty())
        return QUrl();

    return Couch::databaseUrl(d->client->url(), d->name);
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

    if (d->client)
        d->client->disconnect(this);
    if (client)
        connect(client, &CouchClient::urlChanged, this, &CouchDatabase::urlChanged);

    d->client = client;
    emit urlChanged(url());
    emit clientChanged(client);
}

CouchResponse *CouchDatabase::listDesignDocuments()
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::listDesignDocuments(url());
    CouchResponse *response = d->client->sendRequest(request);
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit designDocumentsListed(Couch::toDesignDocumentList(data));
    });
    return d->response(response);
}

CouchResponse *CouchDatabase::createDesignDocument(const QString &designDocument)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::createDesignDocument(Couch::designDocumentUrl(url(), designDocument));
    CouchResponse *response = d->client->sendRequest(request);
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit designDocumentCreated(Couch::toDesignDocument(data));
    });
    return d->response(response);
}

CouchResponse *CouchDatabase::deleteDesignDocument(const QString &designDocument)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::deleteDesignDocument(Couch::designDocumentUrl(url(), designDocument));
    CouchResponse *response = d->client->sendRequest(request);
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit designDocumentDeleted(Couch::toDesignDocument(data));
    });
    return d->response(response);
}

CouchResponse *CouchDatabase::listDocumentIds()
{
    return queryDocuments(CouchQuery());
}

CouchResponse *CouchDatabase::listFullDocuments()
{
    return queryDocuments(CouchQuery::full());
}

CouchResponse *CouchDatabase::queryDocuments(const CouchQuery &query)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::queryDocuments(url(), query);
    CouchResponse *response = d->client->sendRequest(request);
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit documentsListed(Couch::toDocumentList(data));
    });
    return d->response(response);
}

CouchResponse *CouchDatabase::createDocument(const CouchDocument &document)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request = Couch::createDocument(url(), QJsonDocument(document.toJson()).toJson(QJsonDocument::Compact));
    CouchResponse *response = d->client->sendRequest(request);
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit documentCreated(Couch::toDocument(data));
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
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit documentReceived(Couch::toDocument(data));
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
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit documentUpdated(Couch::toDocument(data));
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
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit documentDeleted(Couch::toDocument(data));
    });
    return d->response(response);
}
