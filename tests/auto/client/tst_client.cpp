#include <QtTest>
#include <QtCouchDB>

#include "tst_shared.h"

static const QByteArray TestDatabases = R"(["_replicator","_users","foo","bar"])";

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

void tst_client::initTestCase()
{
    registerTestMetaTypes();
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

    QPointer<TestNetworkAccessManager> ownedManager = new TestNetworkAccessManager(QByteArray(), &client);
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
    request.setBody(TestDatabases);
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
    QCOMPARE(manager.headers.value("Content-Length"), QByteArray::number(TestDatabases.length()));
}

void tst_client::sendRequest_data()
{
    QTest::addColumn<CouchRequest::Operation>("operation");
    QTest::addColumn<QNetworkAccessManager::Operation>("expectedOperation");
    QTest::addColumn<QByteArray>("expectedBody");

    QTest::newRow("get") << CouchRequest::Get << QNetworkAccessManager::GetOperation << QByteArray();
    QTest::newRow("put") << CouchRequest::Put << QNetworkAccessManager::PutOperation << TestDatabases;
    QTest::newRow("post") << CouchRequest::Post << QNetworkAccessManager::PostOperation << TestDatabases;
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
    request.setBody(TestDatabases);

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

    TestNetworkAccessManager manager(TestDatabases);
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
