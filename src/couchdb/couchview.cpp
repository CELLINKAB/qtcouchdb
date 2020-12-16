#include "couchview.h"
#include "couchclient.h"
#include "couchdatabase.h"
#include "couchdesigndocument.h"
#include "couchrequest.h"
#include "couchresponse.h"

class CouchViewPrivate
{
    Q_DECLARE_PUBLIC(CouchView)

public:
    CouchResponse *response(CouchResponse *response)
    {
        Q_Q(CouchView);
        QObject::connect(response, &CouchResponse::errorOccurred, [=](const CouchError &error) {
            emit q->errorOccurred(error);
        });
        return response;
    }

    CouchView *q_ptr = nullptr;
    QString name;
    CouchDesignDocument *designDocument = nullptr;
};

CouchView::CouchView(QObject *parent)
    : CouchView(QString(), nullptr, parent)
{
}

CouchView::CouchView(const QString &name, CouchDesignDocument *designDocument, QObject *parent)
    : QObject(parent),
    d_ptr(new CouchViewPrivate)
{
    Q_D(CouchView);
    d->q_ptr = this;
    d->name = name;
    setDesignDocument(designDocument);
}

CouchView::~CouchView()
{
}

QUrl CouchView::url() const
{
    Q_D(const CouchView);
    if (!d->designDocument || d->name.isEmpty())
        return QUrl();

    return Couch::viewUrl(d->designDocument->url(), d->name);
}

QString CouchView::name() const
{
    Q_D(const CouchView);
    return d->name;
}

void CouchView::setName(const QString &name)
{
    Q_D(CouchView);
    if (d->name == name)
        return;

    d->name = name;
    emit urlChanged(url());
    emit nameChanged(name);
}

CouchClient *CouchView::client() const
{
    Q_D(const CouchView);
    if (!d->designDocument)
        return nullptr;

    return d->designDocument->client();
}

CouchDatabase *CouchView::database() const
{
    Q_D(const CouchView);
    if (!d->designDocument)
        return nullptr;

    return d->designDocument->database();
}

CouchDesignDocument *CouchView::designDocument() const
{
    Q_D(const CouchView);
    return d->designDocument;
}

void CouchView::setDesignDocument(CouchDesignDocument *designDocument)
{
    Q_D(CouchView);
    if (d->designDocument == designDocument)
        return;

    QUrl oldUrl = url();
    CouchClient *oldClient = client();
    CouchDatabase *oldDatabase = database();

    if (d->designDocument)
        d->designDocument->disconnect(this);
    if (designDocument) {
        connect(designDocument, &CouchDesignDocument::urlChanged, this, &CouchView::urlChanged);
        connect(designDocument, &CouchDesignDocument::clientChanged, this, &CouchView::clientChanged);
        connect(designDocument, &CouchDesignDocument::databaseChanged, this, &CouchView::databaseChanged);
    }

    d->designDocument = designDocument;
    if (oldUrl != url())
        emit urlChanged(url());
    if (oldClient != client())
        emit clientChanged(client());
    if (oldDatabase != database())
        emit databaseChanged(database());
    emit designDocumentChanged(designDocument);
}

CouchResponse *CouchView::listRowIds()
{
    return queryRows(CouchQuery());
}

CouchResponse *CouchView::listFullRows()
{
    return queryRows(CouchQuery::full());
}

CouchResponse *CouchView::queryRows(const CouchQuery &query)
{
    Q_D(CouchView);
    CouchClient *client = d->designDocument ? d->designDocument->client() : nullptr;
    if (!client)
        return nullptr;

    CouchRequest request = Couch::queryRows(url(), query);
    CouchResponse *response = client->sendRequest(request);
    if (!response)
        return nullptr;

    connect(response, &CouchResponse::received, [=](const QByteArray &data) {
        emit rowsListed(Couch::toDocumentList(data));
    });
    return d->response(response);
}
