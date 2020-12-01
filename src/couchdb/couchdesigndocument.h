#ifndef COUCHDESIGNDOCUMENT_H
#define COUCHDESIGNDOCUMENT_H

#include <QtCouchDB/couchglobal.h>
#include <QtCouchDB/couch.h>
#include <QtCouchDB/coucherror.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchClient;
class CouchDatabase;
class CouchResponse;
class CouchDesignDocumentPrivate;

class COUCHDB_EXPORT CouchDesignDocument : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(CouchDatabase *database READ database WRITE setDatabase NOTIFY databaseChanged)

public:
    explicit CouchDesignDocument(QObject *parent = nullptr);
    explicit CouchDesignDocument(const QString &name, CouchDatabase *database = nullptr, QObject *parent = nullptr);
    ~CouchDesignDocument();

    QUrl url() const;

    QString name() const;
    void setName(const QString &name);

    CouchClient *client() const;
    CouchDatabase *database() const;
    void setDatabase(CouchDatabase *database);

public slots:
    CouchResponse *listViewIds();
    CouchResponse *listFullViews();
    CouchResponse *queryViews(const CouchQuery &query);

signals:
    void urlChanged(const QUrl &url);
    void nameChanged(const QString &name);
    void clientChanged(CouchClient *client);
    void databaseChanged(CouchDatabase *database);
    void errorOccurred(const CouchError &error);

    void viewsListed(const QStringList &view);

private:
    Q_DECLARE_PRIVATE(CouchDesignDocument)
    QScopedPointer<CouchDesignDocumentPrivate> d_ptr;
};

#endif // COUCHDESIGNDOCUMENT_H
