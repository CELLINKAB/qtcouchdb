#include "couchdbresponse.h"

#include <QJsonDocument>
#include <QJsonObject>

class CouchDBResponsePrivate
{
public:
    CouchDBQuery *query = nullptr; // not ownded
    CouchDBReplyStatus status = COUCHDB_ERROR;
    QString revisionData;
    QByteArray data;
    QJsonDocument document;
};

CouchDBResponse::CouchDBResponse(QObject *parent) :
    QObject(parent),
    d_ptr(new CouchDBResponsePrivate)
{
}

CouchDBResponse::~CouchDBResponse()
{
}

CouchDBQuery *CouchDBResponse::query() const
{
    Q_D(const CouchDBResponse);
    return d->query;
}

void CouchDBResponse::setQuery(CouchDBQuery *query)
{
    Q_D(CouchDBResponse);
    d->query = query;
}

CouchDBReplyStatus CouchDBResponse::status() const
{
    Q_D(const CouchDBResponse);
    return d->status;
}

void CouchDBResponse::setStatus(CouchDBReplyStatus status)
{
    Q_D(CouchDBResponse);
    d->status = status;
}

QString CouchDBResponse::revisionData() const
{
    Q_D(const CouchDBResponse);
    return d->revisionData;
}

void CouchDBResponse::setRevisionData(const QString &revision)
{
    Q_D(CouchDBResponse);
    d->revisionData = revision;
}

QByteArray CouchDBResponse::data() const
{
    Q_D(const CouchDBResponse);
    return d->data;
}

void CouchDBResponse::setData(const QByteArray &data)
{
    Q_D(CouchDBResponse);
    d->data = data;
    d->document = QJsonDocument::fromJson(data);

    if(d->document.isNull() || d->document.isEmpty())
    {
        d->document = QJsonDocument();
    }
    else
    {
        if(d->document.object().contains("revision"))
        {
            d->revisionData = d->document.object().value("revision").toString();
        }
    }
}

QJsonDocument CouchDBResponse::document() const
{
    Q_D(const CouchDBResponse);
    return d->document;
}

QJsonObject CouchDBResponse::documentObj() const
{
    Q_D(const CouchDBResponse);
    return d->document.object();
}
