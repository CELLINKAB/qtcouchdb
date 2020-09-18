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
    Q_PROPERTY(QUrl url READ url WRITE setUrl)

public:
    explicit CouchClient(QObject *parent = nullptr);
    explicit CouchClient(const QUrl &url, QObject *parent = nullptr);
    ~CouchClient();

    QUrl url() const;
    void setUrl(const QUrl &url);

public slots:
    CouchResponse *send(const CouchRequest &request);

signals:
    void received(CouchResponse *response);

private:
    Q_DECLARE_PRIVATE(CouchClient)
    QScopedPointer<CouchClientPrivate> d_ptr;
};

#endif // COUCHCLIENT_H
