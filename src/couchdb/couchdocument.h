#ifndef COUCHDOCUMENT_H
#define COUCHDOCUMENT_H

#include <QtCouchDB/couchglobal.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qdebug.h>
#include <QtCore/qhash.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qurl.h>

class CouchDocumentIdPrivate;

class COUCHDB_EXPORT CouchDocumentId
{
    Q_GADGET
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(QString revision READ revision)

public:
    CouchDocumentId(const QString &id, const QString &revision = QString());
    ~CouchDocumentId();

    CouchDocumentId(const CouchDocumentId &other);
    CouchDocumentId &operator=(const CouchDocumentId &other);
    CouchDocumentId &operator=(const QString &id);

    bool operator==(const CouchDocumentId &other) const;
    bool operator!=(const CouchDocumentId &other) const;

    QString id() const;
    QString revision() const;

    static CouchDocumentId fromJson(const QJsonObject &json);

private:
    Q_DECLARE_PRIVATE(CouchDocumentId)
    QExplicitlySharedDataPointer<CouchDocumentIdPrivate> d_ptr;
};

COUCHDB_EXPORT QDebug operator<<(QDebug debug, const CouchDocumentId &document);

#endif // COUCHDOCUMENT_H
