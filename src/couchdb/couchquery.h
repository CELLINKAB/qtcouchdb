#ifndef COUCHQUERY_H
#define COUCHQUERY_H

#include <QtCouchDB/couchdbglobal.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qhash.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qurl.h>

class CouchQueryPrivate;

QT_FORWARD_DECLARE_CLASS(QUrl)

class COUCHDB_EXPORT CouchQuery
{
    Q_GADGET
    Q_PROPERTY(Operation operation READ operation)
    Q_PROPERTY(QString database READ database)
    Q_PROPERTY(QString documentId READ documentId)
    Q_PROPERTY(QByteArray body READ body)

public:
    enum Operation
    {
        Unknown,
        CheckInstallation,
        StartSession,
        EndSession,
        ListDatabases,
        CreateDatabase,
        DeleteDatabase,
        ListDocuments,
        RetrieveRevision,
        RetrieveDocument,
        UpdateDocument,
        DeleteDocument,
        UploadAttachment,
        DeleteAttachment,
        ReplicateDatabase
    };
    Q_ENUM(Operation)

    CouchQuery(Operation operation = Unknown);
    ~CouchQuery();

    CouchQuery(const CouchQuery &other);
    CouchQuery &operator=(const CouchQuery &other);

    bool operator==(const CouchQuery &other) const;
    bool operator!=(const CouchQuery &other) const;

    QUrl url() const;
    void setUrl(const QUrl &url);

    Operation operation() const;
    void setOperation(Operation operation);

    QString database() const;
    void setDatabase(const QString &database);

    QString documentId() const;
    void setDocumentId(const QString &documentId);

    QByteArray body() const;
    void setBody(const QByteArray &body);

    QHash<QByteArray, QByteArray> headers() const;
    QByteArray header(const QByteArray &header) const;
    void setHeader(const QByteArray &header, const QByteArray &value);

private:
    Q_DECLARE_PRIVATE(CouchQuery)
    QExplicitlySharedDataPointer<CouchQueryPrivate> d_ptr;
};

#endif // COUCHQUERY_H
