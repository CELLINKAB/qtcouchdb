#ifndef COUCHREQUEST_H
#define COUCHREQUEST_H

#include <QtCouchDB/couchglobal.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qdebug.h>
#include <QtCore/qhash.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qurl.h>

class CouchRequestPrivate;

QT_FORWARD_DECLARE_CLASS(QUrl)

class COUCHDB_EXPORT CouchRequest
{
    Q_GADGET
    Q_PROPERTY(QUrl url READ url)
    Q_PROPERTY(Operation operation READ operation)
    Q_PROPERTY(QByteArray body READ body)

public:
    enum Operation
    {
        Get,
        Put,
        Post,
        Delete
    };
    Q_ENUM(Operation)

    CouchRequest(Operation operation = Get);
    ~CouchRequest();

    CouchRequest(const CouchRequest &other);
    CouchRequest &operator=(const CouchRequest &other);

    bool operator==(const CouchRequest &other) const;
    bool operator!=(const CouchRequest &other) const;

    QUrl url() const;
    void setUrl(const QUrl &url);

    Operation operation() const;
    void setOperation(Operation operation);

    QByteArray body() const;
    void setBody(const QByteArray &body);

    QHash<QByteArray, QByteArray> headers() const;
    QByteArray header(const QByteArray &header) const;
    void setHeader(const QByteArray &header, const QByteArray &value);

private:
    Q_DECLARE_PRIVATE(CouchRequest)
    QExplicitlySharedDataPointer<CouchRequestPrivate> d_ptr;
};

COUCHDB_EXPORT QDebug operator<<(QDebug debug, const CouchRequest &request);

Q_DECLARE_METATYPE(CouchRequest)
Q_DECLARE_METATYPE(CouchRequest::Operation)

#endif // COUCHREQUEST_H
