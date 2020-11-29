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
    if (d_ptr->limit == limit)
        return;

    d_ptr.detach();
    d_ptr->limit = limit;
}

int CouchQuery::skip() const
{
    Q_D(const CouchQuery);
    return d->skip;
}

void CouchQuery::setSkip(int skip)
{
    if (d_ptr->skip == skip)
        return;

    d_ptr.detach();
    d_ptr->skip = skip;
}

Qt::SortOrder CouchQuery::order() const
{
    Q_D(const CouchQuery);
    return d->order;
}

void CouchQuery::setOrder(Qt::SortOrder order)
{
    if (d_ptr->order == order)
        return;

    d_ptr.detach();
    d_ptr->order = order;
}

bool CouchQuery::includeDocs() const
{
    Q_D(const CouchQuery);
    return d->includeDocs;
}

void CouchQuery::setIncludeDocs(bool includeDocs)
{
    if (d_ptr->includeDocs == includeDocs)
        return;

    d_ptr.detach();
    d_ptr->includeDocs = includeDocs;
}

CouchQuery CouchQuery::full()
{
    CouchQuery query;
    query.setIncludeDocs(true);
    return query;
}

QDebug operator<<(QDebug debug, const CouchQuery &query)
{
    QDebugStateSaver saver(debug);
    debug.nospace().noquote() << "CouchQuery(limit=" << query.limit() << ", skip=" << query.skip()
                              << ", descending=" << (query.order() == Qt::DescendingOrder)
                              << ", include_docs=" << query.includeDocs() << ')';
    return debug;
}
