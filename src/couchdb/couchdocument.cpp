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

void CouchDocument::setContent(const QByteArray &content)
{
    d_ptr.detach();
    d_ptr->content = content;
}

QJsonObject CouchDocument::toJson() const
{
    Q_D(const CouchDocument);
    return QJsonDocument::fromJson(d->content).object();
}

static QString jsonValue(const QString &key, QJsonObject &json)
{
    // key -> _key -> value.key
    QString value = json.take(QStringLiteral("value")).toObject().value(key).toString();
    QString _key = json.take(QStringLiteral("_") + key).toString(value);
    return json.take(key).toString(_key);
}

CouchDocument CouchDocument::fromJson(const QJsonObject &obj)
{
    QJsonObject json = obj;
    QString id = jsonValue(QStringLiteral("id"), json );
    QString revision = jsonValue(QStringLiteral("rev"), json );
    CouchDocument doc(id, revision);
    doc.setContent(QJsonDocument(json.value(QStringLiteral("doc")).toObject(json )).toJson(QJsonDocument::Compact));
    return doc;
}

QDebug operator<<(QDebug debug, const CouchDocument &document)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "CouchDocument(" << document.id() << ", rev=" << document.revision() << ')';
    return debug;
}
