#ifndef COUCHCURSOR_H
#define COUCHCURSOR_H

#include <QtCouchDB/couchglobal.h>
#include <QtCore/qdebug.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>

class CouchCursorPrivate;

class COUCHDB_EXPORT CouchCursor
{
    Q_GADGET
    Q_PROPERTY(int limit READ limit WRITE setLimit)
    Q_PROPERTY(int skip READ skip WRITE setSkip)
    Q_PROPERTY(Qt::SortOrder order READ order WRITE setOrder)

public:
    CouchCursor();
    ~CouchCursor();

    CouchCursor(const CouchCursor &other);
    CouchCursor &operator=(const CouchCursor &other);

    bool operator==(const CouchCursor &other) const;
    bool operator!=(const CouchCursor &other) const;

    int limit() const;
    void setLimit(int limit);

    int skip() const;
    void setSkip(int skip);

    Qt::SortOrder order() const;
    void setOrder(Qt::SortOrder order);

private:
    Q_DECLARE_PRIVATE(CouchCursor)
    QExplicitlySharedDataPointer<CouchCursorPrivate> d_ptr;
};

COUCHDB_EXPORT QDebug operator<<(QDebug debug, const CouchCursor &cursor);

#endif // COUCHCURSOR_H
