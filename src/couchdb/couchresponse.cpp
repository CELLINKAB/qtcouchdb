#include "couchresponse.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsondocument.h>

class CouchResponsePrivate
{
public:
    CouchRequest request;
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

CouchError CouchResponse::error() const
{
    return CouchError::fromJson(toJson());
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

QJsonObject CouchResponse::toJson() const
{
    Q_D(const CouchResponse);
    return QJsonDocument::fromJson(d->data).object();
}
