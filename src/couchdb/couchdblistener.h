#ifndef COUCHDBLISTENER_H
#define COUCHDBLISTENER_H

#include <QtCouchDB/couchdbglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchDBListenerPrivate;

QT_FORWARD_DECLARE_CLASS(QUrl)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)
QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

class COUCHDB_EXPORT CouchDBListener : public QObject
{
    Q_OBJECT

public:
    CouchDBListener(const QUrl &server, QObject *parent = nullptr);
    ~CouchDBListener();

    QUrl server() const;

    QString database() const;
    void setDatabase(const QString &database);
    
    QString documentId() const;
    void setDocumentId(const QString &documentId);

    QString revision(const QString &documentId = "") const;

    void setCookieJar(QNetworkCookieJar *cookieJar);

    void setParam(const QString &name, const QString &value);
    
    void launch();

signals:
    void changesMade(const QString &revision);

private slots:
    void start();
    void readChanges();
    void listenFinished(QNetworkReply *reply);

private:
    Q_DECLARE_PRIVATE(CouchDBListener)
    QScopedPointer<CouchDBListenerPrivate> d_ptr;
};

#endif // COUCHDBLISTENER_H
