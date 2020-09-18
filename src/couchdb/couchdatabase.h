#ifndef COUCHDATABASE_H
#define COUCHDATABASE_H

#include <QtCouchDB/couchglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchClient;
class CouchResponse;
class CouchDatabasePrivate;

class COUCHDB_EXPORT CouchDatabase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(CouchClient *client READ client WRITE setClient NOTIFY clientChanged)

public:
    explicit CouchDatabase(QObject *parent = nullptr);
    explicit CouchDatabase(const QString &name, CouchClient *client = nullptr, QObject *parent = nullptr);
    ~CouchDatabase();

    QUrl url() const;

    QString name() const;
    void setName(const QString &name);

    CouchClient *client() const;
    void setClient(CouchClient *client);

public slots:
    CouchResponse *create();
    CouchResponse *destroy();
    CouchResponse *fetch(const QString &id, const QString &revision = QString());
    CouchResponse *list();
    CouchResponse *update(const QString &id, const QByteArray &content);
    CouchResponse *remove(const QString &id);

signals:
    void urlChanged(const QUrl &url);
    void nameChanged(const QString &name);
    void clientChanged(CouchClient *client);

private:
    Q_DECLARE_PRIVATE(CouchDatabase)
    QScopedPointer<CouchDatabasePrivate> d_ptr;
};

#endif // COUCHDATABASE_H
