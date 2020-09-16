#ifndef COUCHRESPONSE_H
#define COUCHRESPONSE_H

#include <QtCouchDB/couchglobal.h>
#include <QtCouchDB/couchrequest.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchResponsePrivate;

QT_FORWARD_DECLARE_CLASS(QJsonDocument)

class COUCHDB_EXPORT CouchResponse : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status)
    Q_PROPERTY(QString revision READ revision)
    Q_PROPERTY(QByteArray data READ data)

public:
    CouchResponse(const CouchRequest &query = CouchRequest(), QObject *parent = nullptr);
    ~CouchResponse();

    CouchRequest query() const;

    enum Status { Success, Error, AuthError, Timeout };
    Q_ENUM(Status)

    Status status() const;
    void setStatus(Status status);

    QString revision() const;
    void setRevision(const QString &revision);

    QByteArray data() const;
    void setData(const QByteArray &data);

    QJsonDocument toJson() const;

private:
    Q_DECLARE_PRIVATE(CouchResponse)
    QScopedPointer<CouchResponsePrivate> d_ptr;
};

#endif // COUCHRESPONSE_H
