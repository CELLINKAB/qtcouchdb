#ifndef COUCHDATABASE_H
#define COUCHDATABASE_H

#include <QtCouchDB/couchglobal.h>
#include <QtCouchDB/couchdocument.h>
#include <QtCouchDB/coucherror.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchClient;
class CouchResponse;
class CouchDatabasePrivate;

class COUCHDB_EXPORT CouchDatabase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(CouchClient *client READ client WRITE setClient NOTIFY clientChanged)

public:
    explicit CouchDatabase(QObject *parent = nullptr);
    explicit CouchDatabase(const QString &name, CouchClient *client = nullptr, QObject *parent = nullptr);
    ~CouchDatabase();

    QUrl url() const;

    QString name() const;
    void setName(const QString &name);

    CouchClient *client() const;
    void setClient(CouchClient *client);

public slots:
    CouchResponse *createDatabase();
    CouchResponse *deleteDatabase();

    CouchResponse *listAllDocuments();
    CouchResponse *createDocument(const QByteArray &content);
    CouchResponse *getDocument(const CouchDocumentId &document);
    CouchResponse *updateDocument(const CouchDocumentId &document, const QByteArray &content);
    CouchResponse *deleteDocument(const CouchDocumentId &document);

signals:
    void urlChanged(const QUrl &url);
    void nameChanged(const QString &name);
    void clientChanged(CouchClient *client);
    void errorOccurred(const CouchError &error);

    void databaseCreated();
    void databaseDeleted();

    void documentsListed(const QList<CouchDocumentId> &documents);
    void documentCreated(const CouchDocumentId &document);
    void documentReceived(const CouchDocumentId &document);
    void documentUpdated(const CouchDocumentId &document);
    void documentDeleted(const CouchDocumentId &document);

private:
    Q_DECLARE_PRIVATE(CouchDatabase)
    QScopedPointer<CouchDatabasePrivate> d_ptr;
};

#endif // COUCHDATABASE_H
