#ifndef COUCHDBQUERY_H
#define COUCHDBQUERY_H

#include <QtCouchDB/couchdbglobal.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qhash.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qurl.h>

class CouchDBQueryPrivate;

QT_FORWARD_DECLARE_CLASS(QUrl)

class COUCHDB_EXPORT CouchDBQuery
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

    CouchDBQuery(Operation operation = Unknown);
    ~CouchDBQuery();

    CouchDBQuery(const CouchDBQuery &other);
    CouchDBQuery &operator=(const CouchDBQuery &other);

    bool operator==(const CouchDBQuery &other) const;
    bool operator!=(const CouchDBQuery &other) const;

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
    Q_DECLARE_PRIVATE(CouchDBQuery)
    QExplicitlySharedDataPointer<CouchDBQueryPrivate> d_ptr;
};

#endif // COUCHDBQUERY_H
