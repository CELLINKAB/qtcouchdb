#include "coucherror.h"

#include <QtCore/qjsonobject.h>

class CouchErrorPrivate : public QSharedData
{
public:
    int code = -1;
    QString error;
    QString reason;
};

CouchError::CouchError(const QString &error, const QString &reason)
    : d_ptr(new CouchErrorPrivate)
{
    Q_D(CouchError);
    d->error = error;
    d->reason = reason;
}

CouchError::CouchError(int code, const QString &error, const QString &reason)
    : CouchError(error, reason)
{
    Q_D(CouchError);
    d->code = code;
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
    d_ptr.detach();
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

int CouchError::code() const
{
    Q_D(const CouchError);
    return d->code;
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

CouchError CouchError::withCode(int code) const
{
    CouchError copy(*this);
    copy.d_ptr.detach();
    copy.d_ptr->code = code;
    return copy;
}

CouchError CouchError::fromJson(const QJsonObject &json)
{
    QString error = json.value(QStringLiteral("error")).toString();
    QString reason = json.value(QStringLiteral("reason")).toString();
    return CouchError(-1, error, reason);
}

QDebug operator<<(QDebug debug, const CouchError &error)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "CouchError(" << error.code() << ", " << qPrintable(error.error()) << ", " << error.reason() << ')';
    return debug;
}
