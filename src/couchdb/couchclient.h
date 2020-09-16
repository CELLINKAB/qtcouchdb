#ifndef COUCHCLIENT_H
#define COUCHCLIENT_H

#include <QtCouchDB/couchdbglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchQuery;
class CouchResponse;
class CouchClientPrivate;

class COUCHDB_EXPORT CouchClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl serverUrl READ serverUrl WRITE setServerUrl)

public:
    explicit CouchClient(QObject *parent = nullptr);
    explicit CouchClient(const QUrl &serverUrl, QObject *parent = nullptr);
    ~CouchClient();

    QUrl serverUrl() const;
    void setServerUrl(const QUrl &serverUrl);

    Q_INVOKABLE QUrl databaseUrl(const QString &databaseName) const;
    Q_INVOKABLE QUrl documentUrl(const QString &databaseName, const QString &documentId, const QString &revision = QString()) const;
    Q_INVOKABLE QUrl attachmentUrl(const QString &databaseName, const QString &documentId, const QString &attachmentName, const QString &revision = QString()) const;

public slots:
    void checkInstallation();

    void startSession(const QString &username, const QString &password);
    void endSession();

    void listDatabases();
    void createDatabase(const QString &database);
    void deleteDatabase(const QString &database);

    void listDocuments(const QString &database);
    void retrieveRevision(const QString &database, const QString &documentId);
    void retrieveDocument(const QString &database, const QString &documentId);
    void updateDocument(const QString &database, const QString &documentId, const QByteArray &document);
    void deleteDocument(const QString &database, const QString &documentId, const QString &revision);

    void uploadAttachment(const QString &database, const QString &documentId, const QString &attachmentName, const QByteArray &attachment,
                          const QString &mimeType, const QString &revision);
    void deleteAttachment(const QString &database, const QString &documentId, const QString &attachmentName, const QString &revision);

    void replicateDatabaseFrom(const QUrl &sourceServer, const QString &sourceDatabase, const QString &targetDatabase,
                               bool createTarget, bool continuous, bool cancel = false);
    void replicateDatabaseTo(const QUrl &targetServer, const QString &sourceDatabase, const QString &targetDatabase,
                             bool createTarget, bool continuous, bool cancel = false);

signals:
    void installationChecked(const CouchResponse &response);
    void sessionStarted(const CouchResponse &response);
    void sessionEnded(const CouchResponse &response);
    void databasesListed(const CouchResponse &response);
    void databaseCreated(const CouchResponse &response);
    void databaseDeleted(const CouchResponse &response);
    void documentsListed(const CouchResponse &response);
    void revisionRetrieved(const CouchResponse &response);
    void documentRetrieved(const CouchResponse &response);
    void documentUpdated(const CouchResponse &response);
    void documentDeleted(const CouchResponse &response);
    void attachmentUploaded(const CouchResponse &response);
    void attachmentDeleted(const CouchResponse &response);
    void databaseReplicated(const CouchResponse &response);

protected:
    void executeQuery(const CouchQuery &query);

private:
    Q_DECLARE_PRIVATE(CouchClient)
    QScopedPointer<CouchClientPrivate> d_ptr;
};

#endif // COUCHCLIENT_H
