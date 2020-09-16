#include "couchresponse.h"

#include <QtCore/qjsondocument.h>

class CouchResponsePrivate : public QSharedData
{
public:
    CouchQuery query;
    CouchResponse::Status status = CouchResponse::Error;
    QString revision;
    QByteArray data;
};

CouchResponse::CouchResponse(const CouchQuery &query) :
    d_ptr(new CouchResponsePrivate)
{
    Q_D(CouchResponse);
    d->query = query;
}

CouchResponse::~CouchResponse()
{
}

CouchResponse::CouchResponse(const CouchResponse &other)
    : d_ptr(other.d_ptr)
{
}

CouchResponse &CouchResponse::operator=(const CouchResponse &other)
{
    d_ptr = other.d_ptr;
    return *this;
}

bool CouchResponse::operator==(const CouchResponse &other) const
{
    Q_D(const CouchResponse);
    return d_ptr == other.d_ptr || (d->query == other.query() &&
                                    d->status == other.status() &&
                                    d->revision == other.revision() &&
                                    d->data == other.data());
}

bool CouchResponse::operator!=(const CouchResponse &other) const
{
    return !(*this == other);
}

CouchQuery CouchResponse::query() const
{
    Q_D(const CouchResponse);
    return d->query;
}

CouchResponse::Status CouchResponse::status() const
{
    Q_D(const CouchResponse);
    return d->status;
}

void CouchResponse::setStatus(Status status)
{
    Q_D(CouchResponse);
    d->status = status;
}

QString CouchResponse::revision() const
{
    Q_D(const CouchResponse);
    return d->revision;
}

void CouchResponse::setRevision(const QString &revision)
{
    Q_D(CouchResponse);
    d->revision = revision;
}

QByteArray CouchResponse::data() const
{
    Q_D(const CouchResponse);
    return d->data;
}

void CouchResponse::setData(const QByteArray &data)
{
    Q_D(CouchResponse);
    d->data = data;
}

QJsonDocument CouchResponse::toJson() const
{
    Q_D(const CouchResponse);
    return QJsonDocument::fromJson(d->data);
}

CouchResponse CouchResponse::fromJson(const QJsonDocument &json, const CouchQuery &query)
{
    CouchResponse response(query);
    response.setData(json.toJson());
    response.setRevision(json.object().value("revision").toString());
    return response;
}
