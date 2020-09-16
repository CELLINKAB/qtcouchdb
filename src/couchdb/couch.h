#ifndef COUCH_H
#define COUCH_H

#include <QtCouchDB/couchdbglobal.h>
#include <QtCouchDB/couchrequest.h>
#include <QtCore/qobject.h>

class COUCHDB_EXPORT Couch : public QObject
{
    Q_OBJECT

public:
    explicit Couch(QObject *parent = nullptr);

    Q_INVOKABLE static CouchRequest checkInstallation(const QUrl &serverUrl);

    Q_INVOKABLE static CouchRequest startSession(const QUrl &serverUrl);
    Q_INVOKABLE static CouchRequest endSession(const QUrl &serverUrl);

    Q_INVOKABLE static CouchRequest listDatabases(const QUrl &serverUrl);
    Q_INVOKABLE static CouchRequest createDatabase(const QUrl &databaseUrl);
    Q_INVOKABLE static CouchRequest deleteDatabase(const QUrl &databaseUrl);

    Q_INVOKABLE static CouchRequest listDocuments(const QUrl &databaseUrl);
    Q_INVOKABLE static CouchRequest retrieveRevision(const QUrl &documentUrl);
    Q_INVOKABLE static CouchRequest retrieveDocument(const QUrl &documentUrl);
    Q_INVOKABLE static CouchRequest updateDocument(const QUrl &documentUrl,
                                                 const QByteArray &content);
    Q_INVOKABLE static CouchRequest deleteDocument(const QUrl &documentUrl);

    Q_INVOKABLE static CouchRequest uploadAttachment(const QUrl &attachmentUrl,
                                                   const QByteArray &content,
                                                   const QString &mimeType);
    Q_INVOKABLE static CouchRequest deleteAttachment(const QUrl &attachmentUrl);

    Q_INVOKABLE static CouchRequest replicateDatabase(const QUrl &sourceUrl,
                                                    const QUrl &targetUrl,
                                                    bool create,
                                                    bool continuous,
                                                    bool cancel = false);
};

#endif // COUCH_H
