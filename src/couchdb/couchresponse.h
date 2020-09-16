#ifndef COUCHRESPONSE_H
#define COUCHRESPONSE_H

#include <QtCouchDB/couchdbglobal.h>
#include <QtCouchDB/couchquery.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>

class CouchResponsePrivate;

QT_FORWARD_DECLARE_CLASS(QJsonDocument)

class COUCHDB_EXPORT CouchResponse
{
    Q_GADGET
    Q_PROPERTY(Status status READ status)
    Q_PROPERTY(QString revision READ revision)
    Q_PROPERTY(QByteArray data READ data)

public:
    CouchResponse(const CouchQuery &query = CouchQuery());
    ~CouchResponse();

    CouchResponse(const CouchResponse &other);
    CouchResponse &operator=(const CouchResponse &other);

    bool operator==(const CouchResponse &other) const;
    bool operator!=(const CouchResponse &other) const;

    CouchQuery query() const;

    enum Status { Success, Error, AuthError, Timeout };
    Q_ENUM(Status)

    Status status() const;
    void setStatus(Status status);

    QString revision() const;
    void setRevision(const QString &revision);

    QByteArray data() const;
    void setData(const QByteArray &data);

    QJsonDocument toJson() const;
    static CouchResponse fromJson(const QJsonDocument &json, const CouchQuery &query = CouchQuery());

private:
    Q_DECLARE_PRIVATE(CouchResponse)
    QExplicitlySharedDataPointer<CouchResponsePrivate> d_ptr;
};

#endif // COUCHRESPONSE_H
