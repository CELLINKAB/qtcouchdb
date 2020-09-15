#ifndef COUCHDBQUERY_H
#define COUCHDBQUERY_H

#include <QObject>

#include "couchdbenums.h"
#include "couchdbglobal.h"

class QNetworkRequest;
class CouchDBServer;
class CouchDBQueryPrivate;
class COUCHDB_EXPORT CouchDBQuery : public QObject
{
    Q_OBJECT
public:
    explicit CouchDBQuery(CouchDBServer *server, QObject *parent = 0);
    virtual ~CouchDBQuery();

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
    CouchDBQueryPrivate * const d_ptr;
};

#endif // COUCHDBQUERY_H
