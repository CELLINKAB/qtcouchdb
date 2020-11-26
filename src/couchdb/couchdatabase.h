#ifndef COUCHDATABASE_H
#define COUCHDATABASE_H

#include <QtCouchDB/couchglobal.h>
#include <QtCouchDB/couch.h>
#include <QtCouchDB/couchdocument.h>
#include <QtCouchDB/coucherror.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchClient;
class CouchQuery;
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

    CouchResponse *listAllDesignDocuments();

    CouchResponse *listAllDocuments();
    CouchResponse *queryDocuments(const CouchQuery &query);
    CouchResponse *createDocument(const QByteArray &content);
    CouchResponse *getDocument(const CouchDocument &document);
    CouchResponse *updateDocument(const CouchDocument &document);
    CouchResponse *deleteDocument(const CouchDocument &document);

signals:
    void urlChanged(const QUrl &url);
    void nameChanged(const QString &name);
    void clientChanged(CouchClient *client);
    void errorOccurred(const CouchError &error);

    void databaseCreated();
    void databaseDeleted();

    void designDocumentsListed(const QStringList &designDocuments);

    void documentsListed(const QList<CouchDocument> &documents);
    void documentCreated(const CouchDocument &document);
    void documentReceived(const CouchDocument &document);
    void documentUpdated(const CouchDocument &document);
    void documentDeleted(const CouchDocument &document);

private:
    Q_DECLARE_PRIVATE(CouchDatabase)
    QScopedPointer<CouchDatabasePrivate> d_ptr;
};

#endif // COUCHDATABASE_H
