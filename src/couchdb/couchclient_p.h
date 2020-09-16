#ifndef COUCHCLIENT_P_H
#define COUCHCLIENT_P_H

#include <QtCore/qbytearray.h>
#include <QtCore/qhash.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qurl.h>

class CouchQuery;

class CouchClientPrivate
{
public:
    static QByteArray basicAuth(const QString &username, const QString &password)
    {
        return "Basic " + QByteArray(username.toUtf8() + ":" + password.toUtf8()).toBase64();
    }

    QUrl queryUrl(const QStringList& path, const QString &revision = QString()) const
    {
        QUrl url = server;
        url.setPath("/" + path.join("/"));
        if (!revision.isNull())
            url.setQuery(QString("rev=%1").arg(revision));
        return url;
    }

    QUrl server;
    QScopedPointer<QNetworkAccessManager> networkManager;
    QHash<QNetworkReply *, CouchQuery> currentQueries;
};

#endif // COUCHCLIENT_P_H
