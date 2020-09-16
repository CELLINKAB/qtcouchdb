#include "couchdbresponse.h"

#include <QtCore/qjsondocument.h>

class CouchDBResponsePrivate : public QSharedData
{
public:
    CouchDBQuery *query = nullptr; // not owned
    CouchDBResponse::Status status = CouchDBResponse::Error;
    QString revision;
    QByteArray data;
};

CouchDBResponse::CouchDBResponse(CouchDBQuery *query) :
    d_ptr(new CouchDBResponsePrivate)
{
    Q_D(CouchDBResponse);
    d->query = query;
}

CouchDBResponse::~CouchDBResponse()
{
}

CouchDBResponse::CouchDBResponse(const CouchDBResponse &other)
    : d_ptr(other.d_ptr)
{
}

CouchDBResponse &CouchDBResponse::operator=(const CouchDBResponse &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

bool CouchDBResponse::operator==(const CouchDBResponse &other) const
{
    Q_D(const CouchDBResponse);
    return d_ptr == other.d_ptr || (d->query == other.query() &&
                                    d->status == other.status() &&
                                    d->revision == other.revision() &&
                                    d->data == other.data());
}

bool CouchDBResponse::operator!=(const CouchDBResponse &other) const
{
    return !(*this == other);
}

CouchDBQuery *CouchDBResponse::query() const
{
    Q_D(const CouchDBResponse);
    return d->query;
}

CouchDBResponse::Status CouchDBResponse::status() const
{
    Q_D(const CouchDBResponse);
    return d->status;
}

void CouchDBResponse::setStatus(Status status)
{
    Q_D(CouchDBResponse);
    d->status = status;
}

QString CouchDBResponse::revision() const
{
    Q_D(const CouchDBResponse);
    return d->revision;
}

void CouchDBResponse::setRevision(const QString &revision)
{
    Q_D(CouchDBResponse);
    d->revision = revision;
}

QByteArray CouchDBResponse::data() const
{
    Q_D(const CouchDBResponse);
    return d->data;
}

void CouchDBResponse::setData(const QByteArray &data)
{
    Q_D(CouchDBResponse);
    d->data = data;
}

QJsonDocument CouchDBResponse::toJson() const
{
    Q_D(const CouchDBResponse);
    return QJsonDocument::fromJson(d->data);
}

CouchDBResponse CouchDBResponse::fromJson(const QJsonDocument &json, CouchDBQuery *query)
{
    CouchDBResponse response(query);
    response.setData(json.toJson());
    response.setRevision(json.object().value("revision").toString());
    return response;
}
