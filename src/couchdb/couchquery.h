#ifndef COUCHQUERY_H
#define COUCHQUERY_H

#include <QtCouchDB/couchglobal.h>
#include <QtCore/qdebug.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>

class CouchQueryPrivate;

class COUCHDB_EXPORT CouchQuery
{
    Q_GADGET
    Q_PROPERTY(int limit READ limit WRITE setLimit)
    Q_PROPERTY(int skip READ skip WRITE setSkip)
    Q_PROPERTY(Qt::SortOrder order READ order WRITE setOrder)
    Q_PROPERTY(bool includeDocs READ includeDocs WRITE setIncludeDocs)

public:
    CouchQuery();
    ~CouchQuery();

    CouchQuery(const CouchQuery &other);
    CouchQuery &operator=(const CouchQuery &other);

    bool operator==(const CouchQuery &other) const;
    bool operator!=(const CouchQuery &other) const;

    int limit() const;
    void setLimit(int limit);

    int skip() const;
    void setSkip(int skip);

    Qt::SortOrder order() const;
    void setOrder(Qt::SortOrder order);

    bool includeDocs() const;
    void setIncludeDocs(bool includeDocs);

private:
    Q_DECLARE_PRIVATE(CouchQuery)
    QExplicitlySharedDataPointer<CouchQueryPrivate> d_ptr;
};

COUCHDB_EXPORT QDebug operator<<(QDebug debug, const CouchQuery &query);

Q_DECLARE_METATYPE(CouchQuery)

#endif // COUCHQUERY_H
