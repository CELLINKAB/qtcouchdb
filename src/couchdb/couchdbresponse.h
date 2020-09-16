#ifndef COUCHDBRESPONSE_H
#define COUCHDBRESPONSE_H

#include <QtCouchDB/couchdbglobal.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>

class CouchDBQuery;
class CouchDBResponsePrivate;

QT_FORWARD_DECLARE_CLASS(QJsonDocument)

class COUCHDB_EXPORT CouchDBResponse
{
    Q_GADGET
    Q_PROPERTY(Status status READ status)
    Q_PROPERTY(QString revision READ revision)
    Q_PROPERTY(QByteArray data READ data)

public:
    CouchDBResponse(CouchDBQuery *query = nullptr);
    ~CouchDBResponse();

    CouchDBResponse(const CouchDBResponse &other);
    CouchDBResponse &operator=(const CouchDBResponse &other);

    bool operator==(const CouchDBResponse &other) const;
    bool operator!=(const CouchDBResponse &other) const;

    CouchDBQuery *query() const;

    enum Status { Success, Error, AuthError, Timeout };
    Q_ENUM(Status)

    Status status() const;
    void setStatus(Status status);

    QString revision() const;
    void setRevision(const QString &revision);

    QByteArray data() const;
    void setData(const QByteArray &data);

    QJsonDocument toJson() const;
    static CouchDBResponse fromJson(const QJsonDocument &json, CouchDBQuery *query = nullptr);

private:
    Q_DECLARE_PRIVATE(CouchDBResponse)
    QExplicitlySharedDataPointer<CouchDBResponsePrivate> d_ptr;
};

#endif // COUCHDBRESPONSE_H
