#ifndef COUCHDOCUMENT_H
#define COUCHDOCUMENT_H

#include <QtCouchDB/couchglobal.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qdebug.h>
#include <QtCore/qhash.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qurl.h>

class CouchDocumentPrivate;

class COUCHDB_EXPORT CouchDocument
{
    Q_GADGET
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(QString revision READ revision)
    Q_PROPERTY(QByteArray content READ content)

public:
    CouchDocument(const QString &id = QString(), const QString &revision = QString());
    ~CouchDocument();

    CouchDocument(const CouchDocument &other);
    CouchDocument &operator=(const CouchDocument &other);
    CouchDocument &operator=(const QString &id);

    bool operator==(const CouchDocument &other) const;
    bool operator!=(const CouchDocument &other) const;

    QString id() const;
    QString revision() const;
    QByteArray content() const;

    CouchDocument withRevision(const QString &revision) const;
    CouchDocument withContent(const QByteArray &content) const;

    QJsonObject toJson() const;
    static CouchDocument fromJson(const QJsonObject &json);

private:
    Q_DECLARE_PRIVATE(CouchDocument)
    QExplicitlySharedDataPointer<CouchDocumentPrivate> d_ptr;
};

COUCHDB_EXPORT QDebug operator<<(QDebug debug, const CouchDocument &document);

Q_DECLARE_METATYPE(CouchDocument)

#endif // COUCHDOCUMENT_H
