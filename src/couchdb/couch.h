#ifndef COUCH_H
#define COUCH_H

#include <QtCouchDB/couchglobal.h>
#include <QtCouchDB/couchdocument.h>
#include <QtCouchDB/couchquery.h>
#include <QtCouchDB/couchrequest.h>
#include <QtCore/qobject.h>

class COUCHDB_EXPORT Couch : public QObject
{
    Q_OBJECT

public:
    explicit Couch(QObject *parent = nullptr);

    Q_INVOKABLE static QUrl databaseUrl(const QUrl &clientUrl, const QString &name);
    Q_INVOKABLE static QUrl designDocumentUrl(const QUrl &databaseUrl, const QString &name);
    Q_INVOKABLE static QUrl viewUrl(const QUrl &designDocumentUrl, const QString &name);

    Q_INVOKABLE static CouchDocument document(const QString &id, const QString &revision, const QByteArray &content);
    Q_INVOKABLE static CouchQuery query(int limit, int skip, Qt::SortOrder order, bool includeDocs);

    Q_INVOKABLE static CouchRequest listDatabases(const QUrl &clientUrl);
    Q_INVOKABLE static CouchRequest createDatabase(const QUrl &databaseUrl);
    Q_INVOKABLE static CouchRequest deleteDatabase(const QUrl &databaseUrl);

    Q_INVOKABLE static CouchRequest listDesignDocuments(const QUrl &databaseUrl);
    Q_INVOKABLE static CouchRequest createDesignDocument(const QUrl &designDocumentUrl);
    Q_INVOKABLE static CouchRequest deleteDesignDocument(const QUrl &designDocumentUrl);

    Q_INVOKABLE static CouchRequest listViewIds(const QUrl &designDocumentUrl);
    Q_INVOKABLE static CouchRequest listFullViews(const QUrl &designDocumentUrl);
    Q_INVOKABLE static CouchRequest queryViews(const QUrl &designDocumentUrl, const CouchQuery &query);

    Q_INVOKABLE static CouchRequest listRowIds(const QUrl &viewUrl);
    Q_INVOKABLE static CouchRequest listFullRows(const QUrl &viewUrl);
    Q_INVOKABLE static CouchRequest queryRows(const QUrl &viewUrl, const CouchQuery &query);

    Q_INVOKABLE static CouchRequest listDocumentIds(const QUrl &databaseUrl);
    Q_INVOKABLE static CouchRequest listFullDocuments(const QUrl &databaseUrl);
    Q_INVOKABLE static CouchRequest queryDocuments(const QUrl &databaseUrl, const CouchQuery &query);
    Q_INVOKABLE static CouchRequest createDocument(const QUrl &databaseUrl, const CouchDocument &document);
    Q_INVOKABLE static CouchRequest getDocument(const QUrl &databaseUrl, const CouchDocument &document);
    Q_INVOKABLE static CouchRequest updateDocument(const QUrl &databaseUrl, const CouchDocument &document);
    Q_INVOKABLE static CouchRequest deleteDocument(const QUrl &databaseUrl, const CouchDocument &document);

    Q_INVOKABLE static CouchRequest insertDocuments(const QUrl &databaseUrl, const QList<CouchDocument> &documents);
    Q_INVOKABLE static CouchRequest updateDocuments(const QUrl &databaseUrl, const QList<CouchDocument> &documents);
    Q_INVOKABLE static CouchRequest deleteDocuments(const QUrl &databaseUrl, const QList<CouchDocument> &documents);

    static QString toDatabase(const QByteArray &response);
    static QStringList toDatabaseList(const QByteArray &response);

    static QString toDesignDocument(const QByteArray &response);
    static QStringList toDesignDocumentList(const QByteArray &response);

    static CouchDocument toDocument(const QByteArray &response);
    static QList<CouchDocument> toDocumentList(const QByteArray &response);

    static QStringList toViews(const QByteArray &response);
};

#endif // COUCH_H
