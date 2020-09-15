#include "couchdbserver.h"

class CouchDBServerPrivate
{
public:
    CouchDBServerPrivate() :
        url("localhost"),
        port(5984)
    {}

    QString url;
    int  port;
    QString username;
    QString password;
    QByteArray credential;
};

CouchDBServer::CouchDBServer(QObject *parent) :
    QObject(parent),
    d_ptr(new CouchDBServerPrivate)
{
}

CouchDBServer::~CouchDBServer()
{
    delete d_ptr;
}

QString CouchDBServer::url() const
{
    Q_D(const CouchDBServer);
    return d->url;
}

void CouchDBServer::setUrl(const QString& url)
{
    Q_D(CouchDBServer);
    if(d->url == url) return;
    d->url = url;
}

int CouchDBServer::port() const
{
    Q_D(const CouchDBServer);
    return d->port;
}

void CouchDBServer::setPort(const int& port)
{
    Q_D(CouchDBServer);
    if(d->port == port) return;
    d->port = port;
}

QString CouchDBServer::baseURL(const bool& withCredential) const
{
    Q_D(const CouchDBServer);
    QString url;

    if(withCredential && hasCredential()) url = QString("http://%1:%2@%3:%4").arg(d->username, d->password, d->url, QString::number(d->port));
    else url = QString("http://%1:%2").arg(d->url, QString::number(d->port));

    return url;
}

QByteArray CouchDBServer::credential() const
{
    Q_D(const CouchDBServer);
    return d->credential;
}

void CouchDBServer::setCredential(const QString& username, const QString& password)
{
    Q_D(CouchDBServer);
    d->username = username;
    d->password = password;
    d->credential = QByteArray(QString("%1:%2").arg(username, password).toLatin1()).toBase64();
}

bool CouchDBServer::hasCredential() const
{
    Q_D(const CouchDBServer);
    return (d->credential != "");
}

