#ifndef COUCHDBQUERY_H
#define COUCHDBQUERY_H

#include <QtCouchDB/couchdbglobal.h>
#include <QtCouchDB/couchdbenums.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchDBServer;
class CouchDBQueryPrivate;

QT_FORWARD_DECLARE_CLASS(QNetworkRequest)

class COUCHDB_EXPORT CouchDBQuery : public QObject
{
    Q_OBJECT

public:
    explicit CouchDBQuery(CouchDBServer *server, QObject *parent = 0);
    ~CouchDBQuery();

    CouchDBServer* server() const;
    QNetworkRequest* request() const;

    QUrl url() const;
    void setUrl(const QUrl& url);

    CouchDBOperation operation() const;
    void setOperation(const CouchDBOperation& operation);

    QString database() const;
    void setDatabase(const QString& database);

    QString documentID() const;
    void setDocumentID(const QString& documentID);

    QByteArray body() const;
    void setBody(const QByteArray& body);

signals:
    void timeout();

public slots:
    void startTimeoutTimer();

private:
    Q_DECLARE_PRIVATE(CouchDBQuery)
    QScopedPointer<CouchDBQueryPrivate> d_ptr;
};

#endif // COUCHDBQUERY_H
