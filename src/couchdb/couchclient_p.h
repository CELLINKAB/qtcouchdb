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
    void queryFinished(QNetworkReply *reply);

    QUrl url;
    CouchClient *q_ptr = nullptr;
    QNetworkAccessManager *networkManager = nullptr;
};

#endif // COUCHCLIENT_P_H
