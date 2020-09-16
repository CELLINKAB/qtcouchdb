#include "couchresponse.h"

#include <QtCore/qjsondocument.h>

class CouchResponsePrivate
{
public:
    CouchRequest request;
    CouchResponse::Status status = CouchResponse::Error;
    QString revision;
    QByteArray data;
};

CouchResponse::CouchResponse(const CouchRequest &request, QObject *parent) :
    QObject(parent),
    d_ptr(new CouchResponsePrivate)
{
    Q_D(CouchResponse);
    d->request = request;
}

CouchResponse::~CouchResponse()
{
}

CouchRequest CouchResponse::request() const
{
    Q_D(const CouchResponse);
    return d->request;
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
