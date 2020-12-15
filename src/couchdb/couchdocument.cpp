#include "couchdocument.h"

class CouchDocumentPrivate : public QSharedData
{
public:
    QString id;
    QString revision;
    QByteArray content;
};

CouchDocument::CouchDocument(const QString &id, const QString &revision) :
    d_ptr(new CouchDocumentPrivate)
{
    Q_D(CouchDocument);
    d->id = id;
    d->revision = revision;
}

CouchDocument::~CouchDocument()
{
}

CouchDocument::CouchDocument(const CouchDocument &other)
    : d_ptr(other.d_ptr)
{
}

CouchDocument &CouchDocument::operator=(const CouchDocument &other)
{
    d_ptr.detach();
    d_ptr = other.d_ptr;
    return *this;
}

CouchDocument &CouchDocument::operator=(const QString &id)
{
    d_ptr.detach();
    d_ptr->id = id;
    d_ptr->revision = QString();
    d_ptr->content = QByteArray();
    return *this;
}

bool CouchDocument::operator==(const CouchDocument &other) const
{
    Q_D(const CouchDocument);
    return d_ptr == other.d_ptr || (d->id == other.id() &&
                                    d->revision == other.revision() &&
                                    d->content == other.content());
}

bool CouchDocument::operator!=(const CouchDocument &other) const
{
    return !(*this == other);
}

QString CouchDocument::id() const
{
    Q_D(const CouchDocument);
    return d->id;
}

QString CouchDocument::revision() const
{
    Q_D(const CouchDocument);
    return d->revision;
}

QByteArray CouchDocument::content() const
{
    Q_D(const CouchDocument);
    return d->content;
}

CouchDocument CouchDocument::withRevision(const QString &revision) const
{
    CouchDocument copy(*this);
    copy.d_ptr.detach();
    copy.d_ptr->revision = revision;
    return copy;
}

CouchDocument CouchDocument::withContent(const QByteArray &content) const
{
    CouchDocument copy(*this);
    copy.d_ptr.detach();
    copy.d_ptr->content = content;
    return copy;
}

QJsonObject CouchDocument::toJson() const
{
    Q_D(const CouchDocument);
    QJsonObject json;
    json.insert(QStringLiteral("id"), d->id);
    json.insert(QStringLiteral("rev"), d->revision);
    json.insert(QStringLiteral("doc"), QJsonDocument::fromJson(d->content).object());
    return json;
}

static QString keyValue(const QString &key, QJsonObject &json)
{
    // key -> _key
    return json.take(QStringLiteral("_") + key).toString(json.take(key).toString());
}

static QString childValue(const QString &parentKey, const QString &childKey, QJsonObject &json)
{
    if (!json.contains(parentKey))
        return QString();

    QJsonObject doc = json.value(parentKey).toObject();
    QString value = doc.take(childKey).toString();
    json[parentKey] = doc;
    return value;
}

static QString getValue(const QString &key, QJsonObject &json)
{
    QString value = keyValue(key, json); !value.isNull();
    if (!value.isNull())
        return value;

    value = childValue(QStringLiteral("doc"), key, json);
    if (!value.isNull())
        return value;

    value = childValue(QStringLiteral("value"), key, json);
    if (!value.isNull())
        return value;

    return QString();
}

CouchDocument CouchDocument::fromJson(const QJsonObject &json)
{
    QJsonObject copy(json);
    QString id = getValue(QStringLiteral("id"), copy);
    QString revision = getValue(QStringLiteral("rev"), copy);
    QByteArray content = QJsonDocument(copy.value(QStringLiteral("doc")).toObject(copy)).toJson(QJsonDocument::Compact);
    return CouchDocument(id, revision).withContent(content);
}

QDebug operator<<(QDebug debug, const CouchDocument &document)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "CouchDocument(" << document.id() << ", rev=" << document.revision() << ')';
    return debug;
}
