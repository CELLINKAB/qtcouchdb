#include "couchdbquery.h"

class CouchDBQueryPrivate : public QSharedData
{
public:
    QUrl url;
    CouchDBQuery::Operation operation = CouchDBQuery::Unknown;
    QString database;
    QString documentId;
    QByteArray body;
    QHash<QByteArray, QByteArray> headers;
};

CouchDBQuery::CouchDBQuery(Operation operation) :
    d_ptr(new CouchDBQueryPrivate)
{
    Q_D(CouchDBQuery);
    d->operation = operation;
}

CouchDBQuery::~CouchDBQuery()
{
}

CouchDBQuery::CouchDBQuery(const CouchDBQuery &other)
    : d_ptr(other.d_ptr)
{
}

CouchDBQuery &CouchDBQuery::operator=(const CouchDBQuery &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

bool CouchDBQuery::operator==(const CouchDBQuery &other) const
{
    Q_D(const CouchDBQuery);
    return d_ptr == other.d_ptr || (d->operation == other.operation() &&
                                    d->database == other.database() &&
                                    d->documentId == other.documentId() &&
                                    d->body == other.body() &&
                                    d->headers == other.headers());
}

bool CouchDBQuery::operator!=(const CouchDBQuery &other) const
{
    return !(*this == other);
}

QUrl CouchDBQuery::url() const
{
    Q_D(const CouchDBQuery);
    return d->url;
}

void CouchDBQuery::setUrl(const QUrl &url)
{
    Q_D(CouchDBQuery);
    d->url = url;
}

CouchDBQuery::Operation CouchDBQuery::operation() const
{
    Q_D(const CouchDBQuery);
    return d->operation;
}

void CouchDBQuery::setOperation(Operation operation)
{
    Q_D(CouchDBQuery);
    d->operation = operation;
}

QString CouchDBQuery::database() const
{
    Q_D(const CouchDBQuery);
    return d->database;
}

void CouchDBQuery::setDatabase(const QString &database)
{
    Q_D(CouchDBQuery);
    d->database = database;
}

QString CouchDBQuery::documentId() const
{
    Q_D(const CouchDBQuery);
    return d->documentId;
}

void CouchDBQuery::setDocumentId(const QString &documentId)
{
    Q_D(CouchDBQuery);
    d->documentId = documentId;
}

QByteArray CouchDBQuery::body() const
{
    Q_D(const CouchDBQuery);
    return d->body;
}

void CouchDBQuery::setBody(const QByteArray &body)
{
    Q_D(CouchDBQuery);
    d->body = body;
}

QHash<QByteArray, QByteArray> CouchDBQuery::headers() const
{
    Q_D(const CouchDBQuery);
    return d->headers;
}

QByteArray CouchDBQuery::header(const QByteArray &header) const
{
    Q_D(const CouchDBQuery);
    return d->headers.value(header);
}

void CouchDBQuery::setHeader(const QByteArray &header, const QByteArray &value)
{
    Q_D(CouchDBQuery);
    d->headers.insert(header, value);
}
