#ifndef COUCHCLIENT_H
#define COUCHCLIENT_H

#include <QtCouchDB/couchglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchRequest;
class CouchResponse;
class CouchClientPrivate;

class COUCHDB_EXPORT CouchClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl)

public:
    explicit CouchClient(QObject *parent = nullptr);
    explicit CouchClient(const QUrl &url, QObject *parent = nullptr);
    ~CouchClient();

    QUrl baseUrl() const;
    void setBaseUrl(const QUrl &url);

public slots:
    CouchResponse *listAllDatabases();
    CouchResponse *sendRequest(const CouchRequest &request);

signals:
    void baseUrlChanged(const QUrl &baseUrl);

    void databasesListed(const QStringList &databases);
    void responseReceived(CouchResponse *response);
    void errorOccurred(CouchResponse *response);

private:
    Q_DECLARE_PRIVATE(CouchClient)
    QScopedPointer<CouchClientPrivate> d_ptr;
};

#endif // COUCHCLIENT_H
