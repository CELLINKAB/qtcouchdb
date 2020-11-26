#include "couchcursor.h"

class CouchCursorPrivate : public QSharedData
{
public:
    int limit = 0;
    int skip = 0;
    Qt::SortOrder order = Qt::AscendingOrder;
};

CouchCursor::CouchCursor()
    : d_ptr(new CouchCursorPrivate)
{
}

CouchCursor::~CouchCursor()
{
}

CouchCursor::CouchCursor(const CouchCursor &other)
    : d_ptr(other.d_ptr)
{
}

CouchCursor &CouchCursor::operator=(const CouchCursor &other)
{
    d_ptr.detach();
    d_ptr = other.d_ptr;
    return *this;
}

bool CouchCursor::operator==(const CouchCursor &other) const
{
    Q_D(const CouchCursor);
    return d_ptr == other.d_ptr || (d->limit == other.limit() &&
                                    d->skip == other.skip() &&
                                    d->order == other.order());
}

bool CouchCursor::operator!=(const CouchCursor &other) const
{
    return !(*this == other);
}

int CouchCursor::limit() const
{
    Q_D(const CouchCursor);
    return d->limit;
}

void CouchCursor::setLimit(int limit)
{
    Q_D(CouchCursor);
    if (d->limit == limit)
        return;

    d_ptr.detach();
    d->limit = limit;
}

int CouchCursor::skip() const
{
    Q_D(const CouchCursor);
    return d->skip;
}

void CouchCursor::setSkip(int skip)
{
    Q_D(CouchCursor);
    if (d->skip == skip)
        return;

    d_ptr.detach();
    d->skip = skip;
}

Qt::SortOrder CouchCursor::order() const
{
    Q_D(const CouchCursor);
    return d->order;
}

void CouchCursor::setOrder(Qt::SortOrder order)
{
    Q_D(CouchCursor);
    if (d->order == order)
        return;

    d_ptr.detach();
    d->order = order;
}

QDebug operator<<(QDebug debug, const CouchCursor &cursor)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "CouchCursor(limit=" << cursor.limit() << ", skip=" << cursor.skip()
                              << ", descending=" << (cursor.order() == Qt::DescendingOrder) << ')';
    return debug;
}
