#ifndef COUCHRESPONSE_H
#define COUCHRESPONSE_H

#include <QtCouchDB/couchglobal.h>
#include <QtCouchDB/coucherror.h>
#include <QtCouchDB/couchrequest.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchResponsePrivate;

QT_FORWARD_DECLARE_CLASS(QJsonDocument)

class COUCHDB_EXPORT CouchResponse : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray data READ data)

public:
    CouchResponse(const CouchRequest &request = CouchRequest(), QObject *parent = nullptr);
    ~CouchResponse();

    CouchRequest request() const;
    CouchError error() const;

    QByteArray data() const;
    void setData(const QByteArray &data);

    QJsonObject toJson() const;

signals:
    void received(const QByteArray &data);
    void errorOccurred(const CouchError &error);

private:
    Q_DECLARE_PRIVATE(CouchResponse)
    QScopedPointer<CouchResponsePrivate> d_ptr;
};

#endif // COUCHRESPONSE_H
