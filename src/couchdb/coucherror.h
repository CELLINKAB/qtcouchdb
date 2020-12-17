#ifndef COUCHERROR_H
#define COUCHERROR_H

#include <QtCouchDB/couchglobal.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qdebug.h>
#include <QtCore/qhash.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qurl.h>

class CouchErrorPrivate;

QT_FORWARD_DECLARE_CLASS(QUrl)

class COUCHDB_EXPORT CouchError
{
    Q_GADGET
    Q_PROPERTY(int code READ code)
    Q_PROPERTY(QString error READ error)
    Q_PROPERTY(QString reason READ reason)

public:
    CouchError(const QString &error = QString(), const QString &reason = QString());
    CouchError(int code, const QString &error = QString(), const QString &reason = QString());
    ~CouchError();

    CouchError(const CouchError &other);
    CouchError &operator=(const CouchError &other);

    bool operator==(const CouchError &other) const;
    bool operator!=(const CouchError &other) const;

    int code() const;
    QString error() const;
    QString reason() const;

    CouchError withCode(int code) const;

    static CouchError fromJson(const QJsonObject &json);

private:
    Q_DECLARE_PRIVATE(CouchError)
    QExplicitlySharedDataPointer<CouchErrorPrivate> d_ptr;
};

COUCHDB_EXPORT QDebug operator<<(QDebug debug, const CouchError &error);

Q_DECLARE_METATYPE(CouchError)

#endif // COUCHERROR_H
