#ifndef TST_SHARED_H
#define TST_SHARED_H

#include <QtCore>
#include <QtNetwork>
#include <QtCouchDB>

static const QUrl TestUrl = QUrl("http://admin:password@localhost:5984");

Q_DECLARE_METATYPE(QNetworkAccessManager::Operation)

static inline void registerTestMetaTypes()
{
    qRegisterMetaType<CouchClient *>();
    qRegisterMetaType<CouchDatabase *>();
    qRegisterMetaType<CouchError>();
    qRegisterMetaType<CouchResponse *>();
    qRegisterMetaType<CouchRequest::Operation>();
    qRegisterMetaType<QNetworkAccessManager::Operation>();
}

class TestNetworkReply : public QNetworkReply
{
    Q_OBJECT

public:
    TestNetworkReply(const QByteArray &data, QObject *parent = nullptr)
        : QNetworkReply(parent), m_buffer(new QBuffer(this))
    {
        m_buffer->setData(data);
    }

public slots:
    void abort() override { }

protected:
    bool isSequential() const override { return m_buffer->isSequential(); }
    bool atEnd() const override { return m_buffer->atEnd(); }
    qint64 bytesAvailable() const override { return m_buffer->bytesAvailable(); }
    bool open(OpenMode mode) override { setOpenMode(mode); return m_buffer->open(mode); }
    void close() { m_buffer->close(); }
    qint64 pos() const override { return m_buffer->pos(); }
    bool reset() override { return m_buffer->reset(); }
    bool seek(qint64 pos) override { return m_buffer->seek(pos); }
    qint64 size() const override { return m_buffer->size(); }

    qint64 readData(char *data, qint64 maxlen) override { return m_buffer->read(data, maxlen); }
    qint64 writeData(const char *data, qint64 len) override { return m_buffer->write(data, len); }

private:
    friend class TestNetworkAccessManager;
    QBuffer *m_buffer = nullptr;
};

class TestNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    TestNetworkAccessManager(const QByteArray &data = QByteArray(), QObject *parent = nullptr)
        : QNetworkAccessManager(parent), m_data(data) { }

    TestNetworkAccessManager(QNetworkReply::NetworkError error, const QByteArray &data = QByteArray(), QObject *parent = nullptr)
        : QNetworkAccessManager(parent), m_data(data), m_error(error) { }

    QList<Operation> operations;
    QList<QUrl> urls;
    QList<QByteArray> bodies;
    QHash<QByteArray, QByteArray> headers;

protected:
    QNetworkReply *createRequest(Operation operation, const QNetworkRequest &request, QIODevice *dev) override
    {
        operations += operation;
        urls += request.url();
        for (const QByteArray &header : request.rawHeaderList())
            headers.insert(header, request.rawHeader(header));
        bodies += dev ? dev->readAll() : QByteArray();

        TestNetworkReply *reply = new TestNetworkReply(m_data, this);
        reply->setOperation(operation);
        reply->setRequest(request);
        reply->setError(m_error, "");
        reply->open(QIODevice::ReadOnly);
        if (m_error != QNetworkReply::NoError)
            QMetaObject::invokeMethod(reply, "error", Qt::QueuedConnection, Q_ARG(QNetworkReply::NetworkError, m_error));
        else
            QMetaObject::invokeMethod(reply, "readyRead", Qt::QueuedConnection);
        QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
        return reply;
    }

private:
    QByteArray m_data;
    QNetworkReply::NetworkError m_error = QNetworkReply::NoError;
};

#endif // TST_SHARED_H
