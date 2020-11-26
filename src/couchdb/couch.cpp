#include "couch.h"
#include "couchurl_p.h"

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

CouchRequest Couch::listAllDatabases(const QUrl &baseUrl)
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

CouchRequest Couch::listAllDesignDocuments(const QUrl &databaseUrl)
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

CouchRequest Couch::listAllDocuments(const QUrl &databaseUrl, Query query)
{
    CouchRequest request(CouchRequest::Get);
    QUrl url = CouchUrl::resolve(databaseUrl, QStringLiteral("_all_docs"));
    if (query == Query::IncludeDocs)
        url.setQuery(QStringLiteral("include_docs=true"));
    request.setUrl(url);
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
