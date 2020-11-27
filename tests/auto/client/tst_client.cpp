#include <QtTest/qtest.h>
#include <QtTest/qsignalspy.h>
#include <QtCore/qbuffer.h>
#include <QtCouchDB/couchclient.h>
#include <QtCouchDB/couchrequest.h>
#include <QtCouchDB/couchresponse.h>

static const QUrl TestUrl = QUrl("http://admin:password@localhost:5984");
static const QByteArray TestData = R"(["_replicator","_users","foo","bar"])";

class tst_client : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void baseUrl();
    void autoDeleteReplies();
    void networkAccessManager();
    void headers();
    void sendRequest_data();
    void sendRequest();
    void listAllDatabases();
    void error();
};

class TestNetworkReply : public QNetworkReply
{
    Q_OBJECT

public:
    TestNetworkReply(QObject *parent = nullptr)
        : QNetworkReply(parent), m_buffer(new QBuffer(this))
    {
        m_buffer->setData(TestData);
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
    TestNetworkAccessManager(QObject *parent = nullptr) : QNetworkAccessManager(parent) { }
    TestNetworkAccessManager(QNetworkReply::NetworkError error, QObject *parent = nullptr)
        : QNetworkAccessManager(parent), m_error(error) { }

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

        TestNetworkReply *reply = new TestNetworkReply(this);
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
    QNetworkReply::NetworkError m_error = QNetworkReply::NoError;
};

Q_DECLARE_METATYPE(QNetworkAccessManager::Operation)

void tst_client::initTestCase()
{
    qRegisterMetaType<CouchError>();
    qRegisterMetaType<CouchResponse *>();
    qRegisterMetaType<CouchRequest::Operation>();
    qRegisterMetaType<QNetworkAccessManager::Operation>();
}

void tst_client::baseUrl()
{
    CouchClient client;
    QCOMPARE(client.baseUrl(), QUrl());

    QSignalSpy baseUrlChanged(&client, &CouchClient::baseUrlChanged);
    QVERIFY(baseUrlChanged.isValid());

    client.setBaseUrl(TestUrl);
    QCOMPARE(client.baseUrl(), TestUrl);
    QCOMPARE(baseUrlChanged.count(), 1);

    client.setBaseUrl(TestUrl);
    QCOMPARE(baseUrlChanged.count(), 1);
}

void tst_client::autoDeleteReplies()
{
    CouchClient client;

    QNetworkAccessManager *defaultManager = client.networkAccessManager();
    QVERIFY(defaultManager);
    QVERIFY(defaultManager->autoDeleteReplies());

    QNetworkAccessManager customManager;
    QVERIFY(!customManager.autoDeleteReplies());
    client.setNetworkAccessManager(&customManager);
    QVERIFY(customManager.autoDeleteReplies());
}

void tst_client::networkAccessManager()
{
    CouchClient client;

    QPointer<QNetworkAccessManager> defaultManager = client.networkAccessManager();
    QVERIFY(defaultManager);

    client.setNetworkAccessManager(nullptr);
    QCOMPARE(client.networkAccessManager(), defaultManager);

    QPointer<TestNetworkAccessManager> ownedManager = new TestNetworkAccessManager(&client);
    client.setNetworkAccessManager(ownedManager);
    QCOMPARE(client.networkAccessManager(), ownedManager);

    QCoreApplication::sendPostedEvents(defaultManager, QEvent::DeferredDelete);
    QTRY_VERIFY(defaultManager.isNull());

    QPointer<TestNetworkAccessManager> nonOwnedManager = new TestNetworkAccessManager;
    client.setNetworkAccessManager(nonOwnedManager);
    QCOMPARE(client.networkAccessManager(), nonOwnedManager);

    QCoreApplication::sendPostedEvents(ownedManager, QEvent::DeferredDelete);
    QTRY_VERIFY(ownedManager.isNull());

    client.setNetworkAccessManager(new QNetworkAccessManager(&client));

    QCoreApplication::sendPostedEvents(nonOwnedManager, QEvent::DeferredDelete);
    QVERIFY(!nonOwnedManager.isNull());
}

void tst_client::headers()
{
    CouchClient client;
    client.setBaseUrl(TestUrl);

    TestNetworkAccessManager manager;
    client.setNetworkAccessManager(&manager);

    CouchRequest request(CouchRequest::Post);
    request.setUrl(TestUrl);
    request.setBody(TestData);
    request.setHeader("foo", "bar");

    CouchResponse *response = client.sendRequest(request);
    QVERIFY(response);
    QCOMPARE(response->request(), request);

    QVERIFY(manager.headers.contains("foo"));
    QCOMPARE(manager.headers.value("foo"), QByteArray("bar"));
    QVERIFY(manager.headers.contains("Accept"));
    QCOMPARE(manager.headers.value("Accept"), QByteArray("application/json"));
    QVERIFY(manager.headers.contains("Content-Type"));
    QCOMPARE(manager.headers.value("Content-Type"), QByteArray("application/json"));
    QVERIFY(manager.headers.contains("Content-Length"));
    QCOMPARE(manager.headers.value("Content-Length"), QByteArray::number(TestData.length()));
}

void tst_client::sendRequest_data()
{
    QTest::addColumn<CouchRequest::Operation>("operation");
    QTest::addColumn<QNetworkAccessManager::Operation>("expectedOperation");
    QTest::addColumn<QByteArray>("expectedBody");

    QTest::newRow("get") << CouchRequest::Get << QNetworkAccessManager::GetOperation << QByteArray();
    QTest::newRow("put") << CouchRequest::Put << QNetworkAccessManager::PutOperation << TestData;
    QTest::newRow("post") << CouchRequest::Post << QNetworkAccessManager::PostOperation << TestData;
    QTest::newRow("delete") << CouchRequest::Delete << QNetworkAccessManager::DeleteOperation << QByteArray();
}

void tst_client::sendRequest()
{
    QFETCH(CouchRequest::Operation, operation);
    QFETCH(QNetworkAccessManager::Operation, expectedOperation);
    QFETCH(QByteArray, expectedBody);

    CouchClient client;
    client.setBaseUrl(TestUrl);

    TestNetworkAccessManager manager;
    client.setNetworkAccessManager(&manager);

    CouchRequest request(operation);
    request.setUrl(TestUrl);
    request.setBody(TestData);

    CouchResponse *response = client.sendRequest(request);
    QVERIFY(response);
    QCOMPARE(response->request(), request);
    QCOMPARE(manager.operations, {expectedOperation});
    QCOMPARE(manager.urls, {TestUrl});
    QCOMPARE(manager.bodies, QList<QByteArray>({expectedBody}));
}

void tst_client::listAllDatabases()
{
    CouchClient client;
    client.setBaseUrl(TestUrl);

    QSignalSpy databaseSpy(&client, &CouchClient::databasesListed);
    QVERIFY(databaseSpy.isValid());

    TestNetworkAccessManager manager;
    client.setNetworkAccessManager(&manager);

    client.listAllDatabases();
    QCOMPARE(manager.operations, {QNetworkAccessManager::GetOperation});
    QCOMPARE(manager.urls, {TestUrl.resolved(QUrl("/_all_dbs"))});

    QVERIFY(databaseSpy.wait());
    QVariantList args = databaseSpy.takeFirst();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.first().toStringList(), QStringList({"_replicator", "_users", "foo", "bar"}));
}

void tst_client::error()
{
    CouchClient client;
    client.setBaseUrl(TestUrl);

    TestNetworkAccessManager manager(QNetworkReply::UnknownServerError);
    client.setNetworkAccessManager(&manager);

    QSignalSpy receiveSpy(&client, &CouchClient::responseReceived);
    QVERIFY(receiveSpy.isValid());

    QSignalSpy errorSpy(&client, &CouchClient::errorOccurred);
    QVERIFY(errorSpy.isValid());

    client.listAllDatabases();
    QVERIFY(errorSpy.wait());
    QVERIFY(receiveSpy.isEmpty());
}

QTEST_MAIN(tst_client)

#include "tst_client.moc"
