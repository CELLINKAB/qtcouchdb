#include "couchdesigndocument.h"
#include "couchclient.h"
#include "couchdatabase.h"
#include "couchrequest.h"
#include "couchresponse.h"

class CouchDesignDocumentPrivate
{
    Q_DECLARE_PUBLIC(CouchDesignDocument)

public:
    CouchResponse *response(CouchResponse *response)
    {
        Q_Q(CouchDesignDocument);
        QObject::connect(response, &CouchResponse::errorOccurred, [=](const CouchError &error) {
            emit q->errorOccurred(error);
        });
        return response;
    }

    CouchDesignDocument *q_ptr = nullptr;
    QString name;
    CouchDatabase *database = nullptr;
};

CouchDesignDocument::CouchDesignDocument(QObject *parent)
    : CouchDesignDocument(QString(), nullptr, parent)
{
}

CouchDesignDocument::CouchDesignDocument(const QString &name, CouchDatabase *database, QObject *parent)
    : QObject(parent),
    d_ptr(new CouchDesignDocumentPrivate)
{
    Q_D(CouchDesignDocument);
    d->q_ptr = this;
    d->name = name;
    setDatabase(database);
}

CouchDesignDocument::~CouchDesignDocument()
{
}

QUrl CouchDesignDocument::url() const
{
    Q_D(const CouchDesignDocument);
    if (!d->database || d->name.isEmpty())
        return QUrl();

    return Couch::designDocumentUrl(d->database->url(), d->name);
}

QString CouchDesignDocument::name() const
{
    Q_D(const CouchDesignDocument);
    return d->name;
}

void CouchDesignDocument::setName(const QString &name)
{
    Q_D(CouchDesignDocument);
    if (d->name == name)
        return;

    d->name = name;
    emit urlChanged(url());
    emit nameChanged(name);
}

CouchClient *CouchDesignDocument::client() const
{
    Q_D(const CouchDesignDocument);
    if (!d->database)
        return nullptr;

    return d->database->client();
}

CouchDatabase *CouchDesignDocument::database() const
{
    Q_D(const CouchDesignDocument);
    return d->database;
}

void CouchDesignDocument::setDatabase(CouchDatabase *database)
{
    Q_D(CouchDesignDocument);
    if (d->database == database)
        return;

    QUrl oldUrl = url();
    CouchClient *oldClient = client();

    if (d->database)
        d->database->disconnect(this);
    if (database) {
        connect(database, &CouchDatabase::urlChanged, this, &CouchDesignDocument::urlChanged);
        connect(database, &CouchDatabase::clientChanged, this, &CouchDesignDocument::clientChanged);
    }

    d->database = database;
    if (oldUrl != url())
        emit urlChanged(url());
    if (oldClient != client())
        emit clientChanged(client());
    emit databaseChanged(database);
}

CouchResponse *CouchDesignDocument::listAllViews()
{
    Q_D(CouchDesignDocument);
    CouchClient *client = d->database ? d->database->client() : nullptr;
    if (!client)
        return nullptr;

    CouchRequest request = Couch::listAllViews(url());
    CouchResponse *response = client->sendRequest(request);
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        QJsonDocument json = QJsonDocument::fromJson(data);
        QJsonObject views = json.object().value(QStringLiteral("views")).toObject();
        emit viewsListed(views.keys());
    });
    return d->response(response);
}
