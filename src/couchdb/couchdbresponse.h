#ifndef COUCHDBRESPONSE_H
#define COUCHDBRESPONSE_H

#include <QtCouchDB/couchdbglobal.h>
#include <QtCouchDB/couchdbenums.h>
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>

class CouchDBQuery;
class CouchDBResponsePrivate;

class COUCHDB_EXPORT CouchDBResponse : public QObject
{
    Q_OBJECT

public:
    explicit CouchDBResponse(QObject *parent = nullptr);
    ~CouchDBResponse();

    CouchDBQuery *query() const;
    void setQuery(CouchDBQuery *query);

    CouchDBReplyStatus status() const;
    void setStatus(CouchDBReplyStatus status);

    QString revisionData() const;
    void setRevisionData(const QString &revision);

    QByteArray data() const;
    void setData(const QByteArray &data);

    QJsonDocument document() const;
    QJsonObject documentObj() const;

private:
    Q_DECLARE_PRIVATE(CouchDBResponse)
    QScopedPointer<CouchDBResponsePrivate> d_ptr;
};

#endif // COUCHDBRESPONSE_H
