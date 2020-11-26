#include "couchquery.h"

class CouchQueryPrivate : public QSharedData
{
public:
    int limit = 0;
    int skip = 0;
    Qt::SortOrder order = Qt::AscendingOrder;
    bool includeDocs = false;
};

CouchQuery::CouchQuery()
    : d_ptr(new CouchQueryPrivate)
{
}

CouchQuery::~CouchQuery()
{
}

CouchQuery::CouchQuery(const CouchQuery &other)
    : d_ptr(other.d_ptr)
{
}

CouchQuery &CouchQuery::operator=(const CouchQuery &other)
{
    d_ptr.detach();
    d_ptr = other.d_ptr;
    return *this;
}

bool CouchQuery::operator==(const CouchQuery &other) const
{
    Q_D(const CouchQuery);
    return d_ptr == other.d_ptr || (d->limit == other.limit() &&
                                    d->skip == other.skip() &&
                                    d->order == other.order());
}

bool CouchQuery::operator!=(const CouchQuery &other) const
{
    return !(*this == other);
}

int CouchQuery::limit() const
{
    Q_D(const CouchQuery);
    return d->limit;
}

void CouchQuery::setLimit(int limit)
{
    Q_D(CouchQuery);
    if (d->limit == limit)
        return;

    d_ptr.detach();
    d->limit = limit;
}

int CouchQuery::skip() const
{
    Q_D(const CouchQuery);
    return d->skip;
}

void CouchQuery::setSkip(int skip)
{
    Q_D(CouchQuery);
    if (d->skip == skip)
        return;

    d_ptr.detach();
    d->skip = skip;
}

Qt::SortOrder CouchQuery::order() const
{
    Q_D(const CouchQuery);
    return d->order;
}

void CouchQuery::setOrder(Qt::SortOrder order)
{
    Q_D(CouchQuery);
    if (d->order == order)
        return;

    d_ptr.detach();
    d->order = order;
}

bool CouchQuery::includeDocs() const
{
    Q_D(const CouchQuery);
    return d->includeDocs;
}

void CouchQuery::setIncludeDocs(bool includeDocs)
{
    Q_D(CouchQuery);
    if (d->includeDocs == includeDocs)
        return;

    d_ptr.detach();
    d->includeDocs = includeDocs;
}

QDebug operator<<(QDebug debug, const CouchQuery &query)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "CouchQuery(limit=" << query.limit() << ", skip=" << query.skip()
                              << ", descending=" << (query.order() == Qt::DescendingOrder)
                              << ", include_docs=" << query.includeDocs() << ')';
    return debug;
}
