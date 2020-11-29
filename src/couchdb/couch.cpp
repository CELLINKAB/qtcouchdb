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

QUrl Couch::databaseUrl(const QUrl &baseUrl, const QString &name)
{
    return CouchUrl::resolve(baseUrl, name);
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

CouchRequest Couch::listDatabases(const QUrl &baseUrl)
{
    CouchRequest request(CouchRequest::Get);
    request.setUrl(CouchUrl::resolve(baseUrl, QStringLiteral("_all_dbs")));
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

CouchRequest Couch::createDocument(const QUrl &databaseUrl, const QByteArray &content)
{
    CouchRequest request(CouchRequest::Post);
    request.setUrl(databaseUrl);
    request.setBody(content);
    return request;
}

CouchRequest Couch::getDocument(const QUrl &databaseUrl, const QString &documentId, const QString &revision)
{
    CouchRequest request(CouchRequest::Get);
    request.setUrl(CouchUrl::resolve(databaseUrl, documentId, revision));
    return request;
}

CouchRequest Couch::updateDocument(const QUrl &databaseUrl, const QString &documentId, const QString &revision, const QByteArray &content)
{
    CouchRequest request(CouchRequest::Put);
    request.setUrl(CouchUrl::resolve(databaseUrl, documentId, revision));
    request.setBody(content);
    return request;
}

CouchRequest Couch::deleteDocument(const QUrl &databaseUrl, const QString &documentId, const QString &revision)
{
    CouchRequest request(CouchRequest::Delete);
    request.setUrl(CouchUrl::resolve(databaseUrl, documentId, revision));
    return request;
}
