#include "couchdocument.h"

class CouchDocumentIdPrivate : public QSharedData
{
public:
    QString id;
    QString revision;
};

CouchDocumentId::CouchDocumentId(const QString &id, const QString &revision) :
    d_ptr(new CouchDocumentIdPrivate)
{
    Q_D(CouchDocumentId);
    d->id = id;
    d->revision = revision;
}

CouchDocumentId::~CouchDocumentId()
{
}

CouchDocumentId::CouchDocumentId(const CouchDocumentId &other)
    : d_ptr(other.d_ptr)
{
}

CouchDocumentId &CouchDocumentId::operator=(const CouchDocumentId &other)
{
    d_ptr.detach();
    d_ptr = other.d_ptr;
    return *this;
}

CouchDocumentId &CouchDocumentId::operator=(const QString &id)
{
    d_ptr.detach();
    d_ptr->id = id;
    d_ptr->revision = QString();
    return *this;
}

bool CouchDocumentId::operator==(const CouchDocumentId &other) const
{
    Q_D(const CouchDocumentId);
    return d_ptr == other.d_ptr || (d->id == other.id() &&
                                    d->revision == other.revision());
}

bool CouchDocumentId::operator!=(const CouchDocumentId &other) const
{
    return !(*this == other);
}

QString CouchDocumentId::id() const
{
    Q_D(const CouchDocumentId);
    return d->id;
}

QString CouchDocumentId::revision() const
{
    Q_D(const CouchDocumentId);
    return d->revision;
}

static QString fallbackRevision(const QJsonObject &json)
{
    return json.value(QStringLiteral("value")).toObject().value(QStringLiteral("rev")).toString();
}

CouchDocumentId CouchDocumentId::fromJson(const QJsonObject &json)
{
    QString id = json.value(QStringLiteral("id")).toString();
    QString revision = json.value(QStringLiteral("rev")).toString(fallbackRevision(json));
    return CouchDocumentId(id, revision);
}

QDebug operator<<(QDebug debug, const CouchDocumentId &document)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "CouchDocumentId(" << document.id() << ", rev=" << document.revision() << ')';
    return debug;
}
