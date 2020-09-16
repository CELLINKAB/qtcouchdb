#ifndef COUCH_H
#define COUCH_H

#include <QtCouchDB/couchdbglobal.h>
#include <QtCouchDB/couchquery.h>
#include <QtCore/qobject.h>

class COUCHDB_EXPORT Couch : public QObject
{
    Q_OBJECT

public:
    explicit Couch(QObject *parent = nullptr);

    Q_INVOKABLE static CouchQuery checkInstallation(const QUrl &serverUrl);

    Q_INVOKABLE static CouchQuery startSession(const QUrl &serverUrl);
    Q_INVOKABLE static CouchQuery endSession(const QUrl &serverUrl);

    Q_INVOKABLE static CouchQuery listDatabases(const QUrl &serverUrl);
    Q_INVOKABLE static CouchQuery createDatabase(const QUrl &databaseUrl);
    Q_INVOKABLE static CouchQuery deleteDatabase(const QUrl &databaseUrl);

    Q_INVOKABLE static CouchQuery listDocuments(const QUrl &databaseUrl);
    Q_INVOKABLE static CouchQuery retrieveRevision(const QUrl &documentUrl);
    Q_INVOKABLE static CouchQuery retrieveDocument(const QUrl &documentUrl);
    Q_INVOKABLE static CouchQuery updateDocument(const QUrl &documentUrl,
                                                 const QByteArray &content);
    Q_INVOKABLE static CouchQuery deleteDocument(const QUrl &documentUrl);

    Q_INVOKABLE static CouchQuery uploadAttachment(const QUrl &attachmentUrl,
                                                   const QByteArray &content,
                                                   const QString &mimeType);
    Q_INVOKABLE static CouchQuery deleteAttachment(const QUrl &attachmentUrl);

    Q_INVOKABLE static CouchQuery replicateDatabase(const QUrl &sourceUrl,
                                                    const QUrl &targetUrl,
                                                    bool create,
                                                    bool continuous,
                                                    bool cancel = false);
};

#endif // COUCH_H
