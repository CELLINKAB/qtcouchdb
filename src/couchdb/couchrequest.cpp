#include "couchrequest.h"

class CouchRequestPrivate : public QSharedData
{
public:
    QUrl url;
    CouchRequest::Operation operation = CouchRequest::Unknown;
    QString database;
    QString documentId;
    QByteArray body;
    QHash<QByteArray, QByteArray> headers;
};

CouchRequest::CouchRequest(Operation operation) :
    d_ptr(new CouchRequestPrivate)
{
    Q_D(CouchRequest);
    d->operation = operation;
}

CouchRequest::~CouchRequest()
{
}

CouchRequest::CouchRequest(const CouchRequest &other)
    : d_ptr(other.d_ptr)
{
}

CouchRequest &CouchRequest::operator=(const CouchRequest &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

bool CouchRequest::operator==(const CouchRequest &other) const
{
    Q_D(const CouchRequest);
    return d_ptr == other.d_ptr || (d->operation == other.operation() &&
                                    d->database == other.database() &&
                                    d->documentId == other.documentId() &&
                                    d->body == other.body() &&
                                    d->headers == other.headers());
}

bool CouchRequest::operator!=(const CouchRequest &other) const
{
    return !(*this == other);
}

QUrl CouchRequest::url() const
{
    Q_D(const CouchRequest);
    return d->url;
}

void CouchRequest::setUrl(const QUrl &url)
{
    Q_D(CouchRequest);
    d->url = url;
}

CouchRequest::Operation CouchRequest::operation() const
{
    Q_D(const CouchRequest);
    return d->operation;
}

void CouchRequest::setOperation(Operation operation)
{
    Q_D(CouchRequest);
    d->operation = operation;
}

QString CouchRequest::database() const
{
    Q_D(const CouchRequest);
    return d->database;
}

void CouchRequest::setDatabase(const QString &database)
{
    Q_D(CouchRequest);
    d->database = database;
}

QString CouchRequest::documentId() const
{
    Q_D(const CouchRequest);
    return d->documentId;
}

void CouchRequest::setDocumentId(const QString &documentId)
{
    Q_D(CouchRequest);
    d->documentId = documentId;
}

QByteArray CouchRequest::body() const
{
    Q_D(const CouchRequest);
    return d->body;
}

void CouchRequest::setBody(const QByteArray &body)
{
    Q_D(CouchRequest);
    d->body = body;
}

QHash<QByteArray, QByteArray> CouchRequest::headers() const
{
    Q_D(const CouchRequest);
    return d->headers;
}

QByteArray CouchRequest::header(const QByteArray &header) const
{
    Q_D(const CouchRequest);
    return d->headers.value(header);
}

void CouchRequest::setHeader(const QByteArray &header, const QByteArray &value)
{
    Q_D(CouchRequest);
    d->headers.insert(header, value);
}