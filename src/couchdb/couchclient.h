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
    CouchResponse *sendRequest(const CouchRequest &request);

signals:
    void responseReceived(CouchResponse *response);

private:
    Q_DECLARE_PRIVATE(CouchClient)
    QScopedPointer<CouchClientPrivate> d_ptr;
};

#endif // COUCHCLIENT_H
