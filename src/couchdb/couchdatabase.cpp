#include "couchdatabase.h"
#include "couchclient.h"
#include "couchrequest.h"
#include "couchurl_p.h"

class CouchDatabasePrivate
{
public:
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

    return CouchUrl::resolve(d->client->url(), d->name);
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

CouchResponse *CouchDatabase::create()
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request(CouchRequest::Put);
    request.setUrl(url());
    return d->client->send(request);
}

CouchResponse *CouchDatabase::destroy()
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request(CouchRequest::Delete);
    request.setUrl(url());
    return d->client->send(request);
}

CouchResponse *CouchDatabase::fetch(const QString &id, const QString &revision)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request(CouchRequest::Get);
    request.setUrl(CouchUrl::resolve(url(), id, revision));
    return d->client->send(request);
}

CouchResponse *CouchDatabase::list()
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request(CouchRequest::Get);
    request.setUrl(CouchUrl::resolve(url(), QStringLiteral("_all_docs")));
    return d->client->send(request);
}

CouchResponse *CouchDatabase::update(const QString &id, const QByteArray &content)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request(CouchRequest::Put);
    request.setUrl(CouchUrl::resolve(url(), id));
    request.setBody(content);
    return d->client->send(request);
}

CouchResponse *CouchDatabase::remove(const QString &id)
{
    Q_D(CouchDatabase);
    if (!d->client)
        return nullptr;

    CouchRequest request(CouchRequest::Delete);
    request.setUrl(CouchUrl::resolve(url(), id));
    return d->client->send(request);
}
