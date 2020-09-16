#ifndef COUCHCLIENT_P_H
#define COUCHCLIENT_P_H

#include <QtCouchDB/couchclient.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qhash.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qurl.h>

class CouchRequest;

class CouchClientPrivate
{
    Q_DECLARE_PUBLIC(CouchClient)

public:
    static QByteArray basicAuth(const QString &username, const QString &password)
    {
        return "Basic " + QByteArray(username.toUtf8() + ":" + password.toUtf8()).toBase64();
    }

    void queryFinished();
    void replicateDatabase(const QUrl &source, const QUrl &target, const QString &database, bool createTarget, bool continuous, bool cancel = false);

    CouchClient *q_ptr = nullptr;

    QUrl serverUrl;
    QScopedPointer<QNetworkAccessManager> networkManager;
    QHash<QNetworkReply *, CouchRequest> currentQueries;
};

#endif // COUCHCLIENT_P_H
