#include "couch.h"
#include "couchurl_p.h"

Couch::Couch(QObject *parent) : QObject(parent)
{
}

CouchQuery Couch::checkInstallation(const QUrl &url)
{
    CouchQuery query(CouchQuery::CheckInstallation);
    query.setUrl(url);
    return query;
}

CouchQuery Couch::startSession(const QUrl &url)
{
    CouchQuery query(CouchQuery::StartSession);
    query.setUrl(CouchUrl::resolve(url, "_session"));
    query.setBody(QUrlQuery({{"name", url.userName()}, {"password", url.password()}}).toString(QUrl::FullyEncoded).toUtf8());
    query.setHeader("Accept", "application/json");
    query.setHeader("Content-Type", "application/x-www-form-urlencoded");
    return query;
}

CouchQuery Couch::endSession(const QUrl &url)
{
    CouchQuery query(CouchQuery::EndSession);
    query.setUrl(CouchUrl::resolve(url, "_session"));
    return query;
}

CouchQuery Couch::listDatabases(const QUrl &url)
{
    CouchQuery query(CouchQuery::ListDatabases);
    query.setUrl(CouchUrl::resolve(url, "_all_dbs"));
    return query;
}

CouchQuery Couch::createDatabase(const QUrl &databaseUrl)
{
    CouchQuery query(CouchQuery::CreateDatabase);
    query.setUrl(databaseUrl);
    query.setDatabase(CouchUrl::database(databaseUrl));
    return query;
}

CouchQuery Couch::deleteDatabase(const QUrl &databaseUrl)
{
    CouchQuery query(CouchQuery::DeleteDatabase);
    query.setUrl(databaseUrl);
    query.setDatabase(CouchUrl::database(databaseUrl));
    return query;
}

CouchQuery Couch::listDocuments(const QUrl &databaseUrl)
{
    CouchQuery query(CouchQuery::ListDocuments);
    query.setUrl(CouchUrl::resolve(databaseUrl, "_all_docs"));
    query.setDatabase(CouchUrl::database(databaseUrl));
    return query;
}

CouchQuery Couch::retrieveRevision(const QUrl &documentUrl)
{
    CouchQuery query(CouchQuery::RetrieveRevision);
    query.setUrl(documentUrl);
    query.setDatabase(CouchUrl::database(documentUrl));
    query.setDocumentId(CouchUrl::documentId(documentUrl));
    return query;
}

CouchQuery Couch::retrieveDocument(const QUrl &documentUrl)
{
    CouchQuery query(CouchQuery::RetrieveDocument);
    query.setUrl(documentUrl);
    query.setDatabase(CouchUrl::database(documentUrl));
    query.setDocumentId(CouchUrl::documentId(documentUrl));
    return query;
}

CouchQuery Couch::updateDocument(const QUrl &documentUrl, const QByteArray &content)
{
    CouchQuery query(CouchQuery::UpdateDocument);
    query.setUrl(documentUrl);
    query.setDatabase(CouchUrl::database(documentUrl));
    query.setDocumentId(CouchUrl::documentId(documentUrl));
    query.setHeader("Accept", "application/json");
    query.setHeader("Content-Type", "application/json");
    query.setHeader("Content-Length", QByteArray::number(content.size()));
    query.setBody(content);
    return query;
}

CouchQuery Couch::deleteDocument(const QUrl &documentUrl)
{
    CouchQuery query(CouchQuery::DeleteDocument);
    query.setUrl(documentUrl);
    query.setDatabase(CouchUrl::database(documentUrl));
    query.setDocumentId(CouchUrl::documentId(documentUrl));
    return query;
}

CouchQuery Couch::uploadAttachment(const QUrl &attachmentUrl, const QByteArray &content, const QString &mimeType)
{
    CouchQuery query(CouchQuery::DeleteDocument);
    query.setUrl(attachmentUrl);
    query.setDatabase(CouchUrl::database(attachmentUrl));
    query.setDocumentId(CouchUrl::documentId(attachmentUrl));
    query.setHeader("Content-Type", mimeType.toLatin1());
    query.setHeader("Content-Length", QByteArray::number(content.size()));
    query.setBody(content);
    return query;
}

CouchQuery Couch::deleteAttachment(const QUrl &attachmentUrl)
{
    CouchQuery query(CouchQuery::DeleteAttachment);
    query.setUrl(attachmentUrl);
    query.setDatabase(CouchUrl::database(attachmentUrl));
    query.setDocumentId(CouchUrl::documentId(attachmentUrl));
    return query;
}

CouchQuery Couch::replicateDatabase(const QUrl &sourceUrl, const QUrl &targetUrl,
                                    bool create, bool continuous, bool cancel)
{
    QJsonObject json;
    json.insert("source", sourceUrl.toString());
    json.insert("target", targetUrl.toString());
    json.insert("create_target", create);
    json.insert("continuous", continuous);
    json.insert("cancel", cancel);

    CouchQuery query(CouchQuery::ReplicateDatabase);
    query.setUrl(CouchUrl::resolve(sourceUrl, "/_replicate"));
    query.setDatabase(CouchUrl::database(sourceUrl));
    query.setHeader("Accept", "application/json");
    query.setHeader("Content-Type", "application/json");
    query.setBody(QJsonDocument(json).toJson());

    return query;
}
