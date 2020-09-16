#ifndef COUCHCLIENT_P_H
#define COUCHCLIENT_P_H

#include <QtCouchDB/couchclient.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qhash.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qurl.h>

class CouchQuery;

class CouchClientPrivate
{
    Q_DECLARE_PUBLIC(CouchClient)

public:
    static QByteArray basicAuth(const QString &username, const QString &password)
    {
        return "Basic " + QByteArray(username.toUtf8() + ":" + password.toUtf8()).toBase64();
    }

    QUrl queryUrl(const QStringList& path, const QString &revision = QString()) const
    {
        QUrl u = url;
        u.setPath("/" + path.join("/"));
        if (!revision.isNull())
            u.setQuery(QString("rev=%1").arg(revision));
        return u;
    }

    void queryFinished();
    void replicateDatabase(const QUrl &source, const QUrl &target, const QString &database, bool createTarget, bool continuous, bool cancel = false);

    CouchClient *q_ptr = nullptr;

    QUrl url;
    QScopedPointer<QNetworkAccessManager> networkManager;
    QHash<QNetworkReply *, CouchQuery> currentQueries;
};

#endif // COUCHCLIENT_P_H
