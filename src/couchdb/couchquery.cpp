#include "couchquery.h"

class CouchQueryPrivate : public QSharedData
{
public:
    QUrl url;
    CouchQuery::Operation operation = CouchQuery::Unknown;
    QString database;
    QString documentId;
    QByteArray body;
    QHash<QByteArray, QByteArray> headers;
};

CouchQuery::CouchQuery(Operation operation) :
    d_ptr(new CouchQueryPrivate)
{
    Q_D(CouchQuery);
    d->operation = operation;
}

CouchQuery::~CouchQuery()
{
}

CouchQuery::CouchQuery(const CouchQuery &other)
    : d_ptr(other.d_ptr)
{
}

CouchQuery &CouchQuery::operator=(const CouchQuery &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

bool CouchQuery::operator==(const CouchQuery &other) const
{
    Q_D(const CouchQuery);
    return d_ptr == other.d_ptr || (d->operation == other.operation() &&
                                    d->database == other.database() &&
                                    d->documentId == other.documentId() &&
                                    d->body == other.body() &&
                                    d->headers == other.headers());
}

bool CouchQuery::operator!=(const CouchQuery &other) const
{
    return !(*this == other);
}

QUrl CouchQuery::url() const
{
    Q_D(const CouchQuery);
    return d->url;
}

void CouchQuery::setUrl(const QUrl &url)
{
    Q_D(CouchQuery);
    d->url = url;
}

CouchQuery::Operation CouchQuery::operation() const
{
    Q_D(const CouchQuery);
    return d->operation;
}

void CouchQuery::setOperation(Operation operation)
{
    Q_D(CouchQuery);
    d->operation = operation;
}

QString CouchQuery::database() const
{
    Q_D(const CouchQuery);
    return d->database;
}

void CouchQuery::setDatabase(const QString &database)
{
    Q_D(CouchQuery);
    d->database = database;
}

QString CouchQuery::documentId() const
{
    Q_D(const CouchQuery);
    return d->documentId;
}

void CouchQuery::setDocumentId(const QString &documentId)
{
    Q_D(CouchQuery);
    d->documentId = documentId;
}

QByteArray CouchQuery::body() const
{
    Q_D(const CouchQuery);
    return d->body;
}

void CouchQuery::setBody(const QByteArray &body)
{
    Q_D(CouchQuery);
    d->body = body;
}

QHash<QByteArray, QByteArray> CouchQuery::headers() const
{
    Q_D(const CouchQuery);
    return d->headers;
}

QByteArray CouchQuery::header(const QByteArray &header) const
{
    Q_D(const CouchQuery);
    return d->headers.value(header);
}

void CouchQuery::setHeader(const QByteArray &header, const QByteArray &value)
{
    Q_D(CouchQuery);
    d->headers.insert(header, value);
}
