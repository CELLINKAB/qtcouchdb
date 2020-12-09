#include "couch.h"
#include "couchurl_p.h"

static QUrl queryUrl(QUrl url, const CouchQuery &query)
{
    QUrlQuery q;
    if (query.limit() > 0)
        q.addQueryItem(QStringLiteral("limit"), QString::number(query.limit()));
    if (query.skip() > 0)
        q.addQueryItem(QStringLiteral("skip"), QString::number(query.skip()));
    if (query.order() == Qt::DescendingOrder)
        q.addQueryItem(QStringLiteral("descending"), QStringLiteral("true"));
    if (query.includeDocs())
        q.addQueryItem(QStringLiteral("include_docs"), QStringLiteral("true"));
    url.setQuery(q);
    return url;
}

Couch::Couch(QObject *parent) : QObject(parent)
{
}

QUrl Couch::databaseUrl(const QUrl &clientUrl, const QString &name)
{
    return CouchUrl::resolve(clientUrl, name);
}

QUrl Couch::designDocumentUrl(const QUrl &databaseUrl, const QString &name)
{
    return CouchUrl::resolve(CouchUrl::resolve(databaseUrl, QStringLiteral("_design")), name);
}

QUrl Couch::viewUrl(const QUrl &designDocumentUrl, const QString &name)
{
    return CouchUrl::resolve(CouchUrl::resolve(designDocumentUrl, QStringLiteral("_view")), name);
}

CouchDocument Couch::document(const QString &id, const QString &revision, const QByteArray &content)
{
    CouchDocument document(id, revision);
    document.setContent(content);
    return document;
}

CouchQuery Couch::query(int limit, int skip, Qt::SortOrder order, bool includeDocs)
{
    CouchQuery query;
    query.setLimit(limit);
    query.setSkip(skip);
    query.setOrder(order);
    query.setIncludeDocs(includeDocs);
    return query;
}

CouchRequest Couch::listDatabases(const QUrl &clientUrl)
{
    CouchRequest request(CouchRequest::Get);
    request.setUrl(CouchUrl::resolve(clientUrl, QStringLiteral("_all_dbs")));
    return request;
}

CouchRequest Couch::createDatabase(const QUrl &databaseUrl)
{
    CouchRequest request(CouchRequest::Put);
    request.setUrl(databaseUrl);
    return request;
}

CouchRequest Couch::deleteDatabase(const QUrl &databaseUrl)
{
    CouchRequest request(CouchRequest::Delete);
    request.setUrl(databaseUrl);
    return request;
}

CouchRequest Couch::listDesignDocuments(const QUrl &databaseUrl)
{
    CouchRequest request(CouchRequest::Get);
    request.setUrl(CouchUrl::resolve(databaseUrl, QStringLiteral("_design_docs")));
    return request;
}

CouchRequest Couch::createDesignDocument(const QUrl &designDocumentUrl)
{
    CouchRequest request(CouchRequest::Put);
    request.setUrl(designDocumentUrl);
    return request;
}

CouchRequest Couch::deleteDesignDocument(const QUrl &designDocumentUrl)
{
    CouchRequest request(CouchRequest::Delete);
    request.setUrl(designDocumentUrl);
    return request;
}

CouchRequest Couch::listViewIds(const QUrl &designDocumentUrl)
{
    return queryViews(designDocumentUrl, CouchQuery());
}

CouchRequest Couch::listFullViews(const QUrl &designDocumentUrl)
{
    return queryViews(designDocumentUrl, CouchQuery::full());
}

CouchRequest Couch::queryViews(const QUrl &designDocumentUrl, const CouchQuery &query)
{
    CouchRequest request(CouchRequest::Get);
    request.setUrl(queryUrl(designDocumentUrl, query));
    return request;
}

CouchRequest Couch::listRowIds(const QUrl &viewUrl)
{
    return queryRows(viewUrl, CouchQuery());
}

CouchRequest Couch::listFullRows(const QUrl &viewUrl)
{
    return queryRows(viewUrl, CouchQuery::full());
}

CouchRequest Couch::queryRows(const QUrl &viewUrl, const CouchQuery &query)
{
    CouchRequest request(CouchRequest::Get);
    request.setUrl(queryUrl(viewUrl, query));
    return request;
}

CouchRequest Couch::listDocumentIds(const QUrl &databaseUrl)
{
    return queryDocuments(databaseUrl, CouchQuery());
}

CouchRequest Couch::listFullDocuments(const QUrl &databaseUrl)
{
    return queryDocuments(databaseUrl, CouchQuery::full());
}

CouchRequest Couch::queryDocuments(const QUrl &databaseUrl, const CouchQuery &query)
{
    CouchRequest request(CouchRequest::Get);
    QUrl url = CouchUrl::resolve(databaseUrl, QStringLiteral("_all_docs"));
    request.setUrl(queryUrl(url, query));
    return request;
}

CouchRequest Couch::createDocument(const QUrl &databaseUrl, const CouchDocument &document)
{
    CouchRequest request(CouchRequest::Post);
    request.setUrl(databaseUrl);
    request.setBody(document.content());
    return request;
}

CouchRequest Couch::getDocument(const QUrl &databaseUrl, const CouchDocument &document)
{
    CouchRequest request(CouchRequest::Get);
    request.setUrl(CouchUrl::resolve(databaseUrl, document.id(), document.revision()));
    return request;
}

CouchRequest Couch::updateDocument(const QUrl &databaseUrl, const CouchDocument &document)
{
    CouchRequest request(CouchRequest::Put);
    request.setUrl(CouchUrl::resolve(databaseUrl, document.id(), document.revision()));
    request.setBody(document.content());
    return request;
}

CouchRequest Couch::deleteDocument(const QUrl &databaseUrl, const CouchDocument &document)
{
    CouchRequest request(CouchRequest::Delete);
    request.setUrl(CouchUrl::resolve(databaseUrl, document.id(), document.revision()));
    return request;
}

static QByteArray fromDocumentList(const QList<CouchDocument> &documents, bool deleted = false)
{
    QJsonArray docs;
    for (const CouchDocument &document : documents) {
        QJsonObject doc = document.toJson();
        if (deleted)
            doc.insert(QStringLiteral("_deleted"), true);
        docs += document.toJson();
    }

    QJsonObject json;
    json.insert(QStringLiteral("docs"), docs);
    return QJsonDocument(json).toJson(QJsonDocument::Compact);
}

CouchRequest Couch::insertDocuments(const QUrl &databaseUrl, const QList<CouchDocument> &documents)
{
    CouchRequest request(CouchRequest::Post);
    request.setUrl(databaseUrl);
    request.setBody(fromDocumentList(documents));
    return request;
}

CouchRequest Couch::updateDocuments(const QUrl &databaseUrl, const QList<CouchDocument> &documents)
{
    CouchRequest request(CouchRequest::Post);
    request.setUrl(databaseUrl);
    request.setBody(fromDocumentList(documents));
    return request;
}

CouchRequest Couch::deleteDocuments(const QUrl &databaseUrl, const QList<CouchDocument> &documents)
{
    CouchRequest request(CouchRequest::Post);
    request.setUrl(databaseUrl);
    request.setBody(fromDocumentList(documents, true));
    return request;
}

QString Couch::toDatabase(const QByteArray &response)
{
    return QString::fromUtf8(response);
}

QStringList Couch::toDatabaseList(const QByteArray &response)
{
    QJsonArray json = QJsonDocument::fromJson(response).array();

    QStringList databases;
    for (const QJsonValue &value : json)
        databases += value.toString();
    return databases;
}

QString Couch::toDesignDocument(const QByteArray &response)
{
    return QString::fromUtf8(response);
}

static QString trimPrefix(QString str, const QString &prefix)
{
    if (!str.startsWith(prefix))
        return str;

    return str.mid(prefix.length());
}

static QString toDesignDocumentName(const QJsonObject &json)
{
    QString key = json.value(QStringLiteral("key")).toString();
    return trimPrefix(key, QStringLiteral("_design/"));
}

QStringList Couch::toDesignDocumentList(const QByteArray &response)
{
    QJsonDocument json = QJsonDocument::fromJson(response);
    QJsonArray rows = json.object().value(QStringLiteral("rows")).toArray();

    QStringList designs;
    for (const QJsonValue &value : rows)
        designs += toDesignDocumentName(value.toObject());
    return designs;
}

CouchDocument Couch::toDocument(const QByteArray &response)
{
    QJsonDocument json = QJsonDocument::fromJson(response);
    return CouchDocument::fromJson(json.object());
}

QList<CouchDocument> Couch::toDocumentList(const QByteArray &response)
{
    QJsonDocument json = QJsonDocument::fromJson(response);
    QJsonArray rows = json.object().value(QStringLiteral("rows")).toArray();

    QList<CouchDocument> docs;
    for (const QJsonValue &value : rows)
        docs += CouchDocument::fromJson(value.toObject());
    return docs;
}

QStringList Couch::toViews(const QByteArray &response)
{
    QJsonDocument json = QJsonDocument::fromJson(response);
    QJsonObject views = json.object().value(QStringLiteral("views")).toObject();
    return views.keys();
}

QJsonArray Couch::toRows(const QByteArray &response)
{
    QJsonDocument json = QJsonDocument::fromJson(response);
    QJsonArray rows = json.object().value(QStringLiteral("rows")).toArray();
    if (rows.isEmpty() || !rows.first().toObject().contains(QStringLiteral("doc")))
        return rows;

    QJsonArray fullRows;
    for (const QJsonValue &row : qAsConst(rows))
        fullRows += row.toObject().value(QStringLiteral("doc"));
    return fullRows;
}
