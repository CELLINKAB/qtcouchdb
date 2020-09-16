#ifndef COUCHCLIENT_P_H
#define COUCHCLIENT_P_H

#include <QtCouchDB/couchclient.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>
#include <QtCore/qurl.h>

class CouchClientPrivate
{
    Q_DECLARE_PUBLIC(CouchClient)

public:
    static QByteArray basicAuth(const QString &username, const QString &password)
    {
        return "Basic " + QByteArray(username.toUtf8() + ":" + password.toUtf8()).toBase64();
    }

    void queryFinished(QNetworkReply *reply);
    void replicateDatabase(const QUrl &source, const QUrl &target, const QString &database, bool createTarget, bool continuous, bool cancel = false);

    QUrl serverUrl;
    CouchClient *q_ptr = nullptr;
    QNetworkAccessManager *networkManager = nullptr;
};

#endif // COUCHCLIENT_P_H
