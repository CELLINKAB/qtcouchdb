#include "couch.h"
#include "couchurl_p.h"

Couch::Couch(QObject *parent) : QObject(parent)
{
}

CouchRequest Couch::checkInstallation(const QUrl &serverUrl)
{
    CouchRequest request(CouchRequest::CheckInstallation);
    request.setUrl(serverUrl);
    return request;
}

CouchRequest Couch::startSession(const QUrl &serverUrl)
{
    CouchRequest request(CouchRequest::StartSession);
    request.setUrl(CouchUrl::resolve(serverUrl, "_session"));
    request.setBody(QUrlQuery({{"name", serverUrl.userName()}, {"password", serverUrl.password()}}).toString(QUrl::FullyEncoded).toUtf8());
    request.setHeader("Accept", "application/json");
    request.setHeader("Content-Type", "application/x-www-form-urlencoded");
    return request;
}

CouchRequest Couch::endSession(const QUrl &serverUrl)
{
    CouchRequest request(CouchRequest::EndSession);
    request.setUrl(CouchUrl::resolve(serverUrl, "_session"));
    return request;
}

CouchRequest Couch::listDatabases(const QUrl &serverUrl)
{
    CouchRequest request(CouchRequest::ListDatabases);
    request.setUrl(CouchUrl::resolve(serverUrl, "_all_dbs"));
    return request;
}

CouchRequest Couch::createDatabase(const QUrl &databaseUrl)
{
    CouchRequest request(CouchRequest::CreateDatabase);
    request.setUrl(databaseUrl);
    request.setDatabase(CouchUrl::database(databaseUrl));
    return request;
}

CouchRequest Couch::deleteDatabase(const QUrl &databaseUrl)
{
    CouchRequest request(CouchRequest::DeleteDatabase);
    request.setUrl(databaseUrl);
    request.setDatabase(CouchUrl::database(databaseUrl));
    return request;
}

CouchRequest Couch::listDocuments(const QUrl &databaseUrl)
{
    CouchRequest request(CouchRequest::ListDocuments);
    request.setUrl(CouchUrl::resolve(databaseUrl, "_all_docs"));
    request.setDatabase(CouchUrl::database(databaseUrl));
    return request;
}

CouchRequest Couch::retrieveRevision(const QUrl &documentUrl)
{
    CouchRequest request(CouchRequest::RetrieveRevision);
    request.setUrl(documentUrl);
    request.setDatabase(CouchUrl::database(documentUrl));
    request.setDocumentId(CouchUrl::documentId(documentUrl));
    return request;
}

CouchRequest Couch::retrieveDocument(const QUrl &documentUrl)
{
    CouchRequest request(CouchRequest::RetrieveDocument);
    request.setUrl(documentUrl);
    request.setDatabase(CouchUrl::database(documentUrl));
    request.setDocumentId(CouchUrl::documentId(documentUrl));
    return request;
}

CouchRequest Couch::updateDocument(const QUrl &documentUrl, const QByteArray &content)
{
    CouchRequest request(CouchRequest::UpdateDocument);
    request.setUrl(documentUrl);
    request.setDatabase(CouchUrl::database(documentUrl));
    request.setDocumentId(CouchUrl::documentId(documentUrl));
    request.setHeader("Accept", "application/json");
    request.setHeader("Content-Type", "application/json");
    request.setHeader("Content-Length", QByteArray::number(content.size()));
    request.setBody(content);
    return request;
}

CouchRequest Couch::deleteDocument(const QUrl &documentUrl)
{
    CouchRequest request(CouchRequest::DeleteDocument);
    request.setUrl(documentUrl);
    request.setDatabase(CouchUrl::database(documentUrl));
    request.setDocumentId(CouchUrl::documentId(documentUrl));
    return request;
}

CouchRequest Couch::uploadAttachment(const QUrl &attachmentUrl, const QByteArray &content, const QString &mimeType)
{
    CouchRequest request(CouchRequest::DeleteDocument);
    request.setUrl(attachmentUrl);
    request.setDatabase(CouchUrl::database(attachmentUrl));
    request.setDocumentId(CouchUrl::documentId(attachmentUrl));
    request.setHeader("Content-Type", mimeType.toLatin1());
    request.setHeader("Content-Length", QByteArray::number(content.size()));
    request.setBody(content);
    return request;
}

CouchRequest Couch::deleteAttachment(const QUrl &attachmentUrl)
{
    CouchRequest request(CouchRequest::DeleteAttachment);
    request.setUrl(attachmentUrl);
    request.setDatabase(CouchUrl::database(attachmentUrl));
    request.setDocumentId(CouchUrl::documentId(attachmentUrl));
    return request;
}

CouchRequest Couch::replicateDatabase(const QUrl &sourceUrl, const QUrl &targetUrl,
                                    bool create, bool continuous, bool cancel)
{
    QJsonObject json;
    json.insert("source", sourceUrl.toString());
    json.insert("target", targetUrl.toString());
    json.insert("create_target", create);
    json.insert("continuous", continuous);
    json.insert("cancel", cancel);

    CouchRequest request(CouchRequest::ReplicateDatabase);
    request.setUrl(CouchUrl::resolve(sourceUrl, "/_replicate"));
    request.setDatabase(CouchUrl::database(sourceUrl));
    request.setHeader("Accept", "application/json");
    request.setHeader("Content-Type", "application/json");
    request.setBody(QJsonDocument(json).toJson());
    return request;
}
