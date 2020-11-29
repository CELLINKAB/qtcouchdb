#ifndef COUCHCLIENT_H
#define COUCHCLIENT_H

#include <QtCouchDB/couchglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchError;
class CouchRequest;
class CouchResponse;
class CouchClientPrivate;
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

class COUCHDB_EXPORT CouchClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)

public:
    explicit CouchClient(QObject *parent = nullptr);
    explicit CouchClient(const QUrl &url, QObject *parent = nullptr);
    ~CouchClient();

    QUrl url() const;
    void setUrl(const QUrl &url);

    bool isBusy() const;

    QNetworkAccessManager *networkAccessManager() const;
    void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);

public slots:
    bool listDatabases();
    bool createDatabase(const QString &database);
    bool deleteDatabase(const QString &database);

    CouchResponse *sendRequest(const CouchRequest &request);

signals:
    void urlChanged(const QUrl &url);
    void busyChanged(bool busy);

    void databasesListed(const QStringList &databases);
    void databaseCreated(const QString &database);
    void databaseDeleted(const QString &database);

    void responseReceived(CouchResponse *response);
    void errorOccurred(const CouchError &error);

private:
    Q_DECLARE_PRIVATE(CouchClient)
    QScopedPointer<CouchClientPrivate> d_ptr;
};

#endif // COUCHCLIENT_H
