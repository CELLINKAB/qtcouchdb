#include "coucherror.h"

class CouchErrorPrivate : public QSharedData
{
public:
    QString error;
    QString reason;
};

CouchError::CouchError(const QString &error, const QString &reason) :
    d_ptr(new CouchErrorPrivate)
{
    Q_D(CouchError);
    d->error = error;
    d->reason = reason;
}

CouchError::~CouchError()
{
}

CouchError::CouchError(const CouchError &other)
    : d_ptr(other.d_ptr)
{
}

CouchError &CouchError::operator=(const CouchError &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

bool CouchError::operator==(const CouchError &other) const
{
    Q_D(const CouchError);
    return d_ptr == other.d_ptr || (d->error == other.error() &&
                                    d->reason == other.reason());
}

bool CouchError::operator!=(const CouchError &other) const
{
    return !(*this == other);
}

QString CouchError::error() const
{
    Q_D(const CouchError);
    return d->error;
}

QString CouchError::reason() const
{
    Q_D(const CouchError);
    return d->reason;
}

CouchError CouchError::fromJson(const QJsonObject &json)
{
    QString error = json.value(QStringLiteral("error")).toString();
    QString reason = json.value(QStringLiteral("reason")).toString();
    return CouchError(error, reason);
}

QDebug operator<<(QDebug debug, const CouchError &error)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "CouchError(" << error.error() << ", " << error.reason() << ')';
    return debug;
}
