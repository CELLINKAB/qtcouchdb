#ifndef COUCH_H
#define COUCH_H

#include <QtCouchDB/couchglobal.h>
#include <QtCouchDB/couchrequest.h>
#include <QtCore/qobject.h>

class COUCHDB_EXPORT Couch : public QObject
{
    Q_OBJECT

public:
    explicit Couch(QObject *parent = nullptr);

    enum class Query { Default, IncludeDocs };
    Q_ENUM(Query)

    Q_INVOKABLE static QUrl databaseUrl(const QUrl &baseUrl, const QString &name);

    Q_INVOKABLE static CouchRequest listAllDatabases(const QUrl &baseUrl);
    Q_INVOKABLE static CouchRequest createDatabase(const QUrl &databaseUrl);
    Q_INVOKABLE static CouchRequest deleteDatabase(const QUrl &databaseUrl);

    Q_INVOKABLE static CouchRequest listAllDocuments(const QUrl &databaseUrl, Query query = Query::Default);
    Q_INVOKABLE static CouchRequest createDocument(const QUrl &databaseUrl, const QByteArray &content);
    Q_INVOKABLE static CouchRequest getDocument(const QUrl &databaseUrl, const QString &documentId, const QString &revision = QString());
    Q_INVOKABLE static CouchRequest updateDocument(const QUrl &databaseUrl, const QString &documentId, const QString &revision, const QByteArray &content);
    Q_INVOKABLE static CouchRequest deleteDocument(const QUrl &databaseUrl, const QString &documentId, const QString &revision);
};

#endif // COUCH_H
