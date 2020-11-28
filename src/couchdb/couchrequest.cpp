#include "couchrequest.h"

class CouchRequestPrivate : public QSharedData
{
public:
    QUrl url;
    CouchRequest::Operation operation = CouchRequest::Get;
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
    d_ptr.detach();
    d_ptr = other.d_ptr;
    return *this;
}

bool CouchRequest::operator==(const CouchRequest &other) const
{
    Q_D(const CouchRequest);
    return d_ptr == other.d_ptr || (d->operation == other.operation() &&
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
    d_ptr.detach();
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
    d_ptr.detach();
    d->operation = operation;
}

QByteArray CouchRequest::body() const
{
    Q_D(const CouchRequest);
    return d->body;
}

void CouchRequest::setBody(const QByteArray &body)
{
    Q_D(CouchRequest);
    d_ptr.detach();
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
    d_ptr.detach();
    d->headers.insert(header, value);
}

static QString maskedUrl(QUrl url)
{
    if (!url.password().isEmpty())
        url.setPassword(QStringLiteral("xxxxx"));
    return url.toString();
}

static const char *toKey(CouchRequest::Operation operation)
{
    return QMetaEnum::fromType<CouchRequest::Operation>().key(operation);
}

QDebug operator<<(QDebug debug, const CouchRequest &request)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "CouchRequest(" << toKey(request.operation()) << ", " << qPrintable(maskedUrl(request.url()));
    if (request.operation() == CouchRequest::Post)
        debug.nospace() << ", " << request.body();
    debug.nospace() << ')';
    return debug;
}
