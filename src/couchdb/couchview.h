#ifndef COUCHVIEW_H
#define COUCHVIEW_H

#include <QtCouchDB/couchglobal.h>
#include <QtCouchDB/couch.h>
#include <QtCouchDB/couchdocument.h>
#include <QtCouchDB/coucherror.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchClient;
class CouchDatabase;
class CouchDesignDocument;
class CouchQuery;
class CouchResponse;
class CouchViewPrivate;

class COUCHDB_EXPORT CouchView : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(CouchDesignDocument *designDocument READ designDocument WRITE setDesignDocument NOTIFY designDocumentChanged)

public:
    explicit CouchView(QObject *parent = nullptr);
    explicit CouchView(const QString &name, CouchDesignDocument *designDocument = nullptr, QObject *parent = nullptr);
    ~CouchView();

    QUrl url() const;

    QString name() const;
    void setName(const QString &name);

    CouchClient *client() const;
    CouchDatabase *database() const;
    CouchDesignDocument *designDocument() const;
    void setDesignDocument(CouchDesignDocument *designDocument);

public slots:
    CouchResponse *listAllRows();
    CouchResponse *queryRows(const CouchQuery &query);

signals:
    void urlChanged(const QUrl &url);
    void nameChanged(const QString &name);
    void designDocumentChanged(CouchDesignDocument *designDocument);
    void errorOccurred(const CouchError &error);

    void rowsListed(const QJsonArray &rows);

private:
    Q_DECLARE_PRIVATE(CouchView)
    QScopedPointer<CouchViewPrivate> d_ptr;
};

#endif // COUCHVIEW_H
