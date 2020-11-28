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
    void responses();
    void networkAccessManager();
    void headers();
    void sendRequest_data();
    void sendRequest();
    void listAllDatabases();
    void createDeleteDatabase_data();
    void createDeleteDatabase();
    void error();
    void busy();
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

void tst_client::responses()
{
    CouchClient client;
    QVERIFY(!client.createDatabase("tst_database"));
    QVERIFY(!client.deleteDatabase("tst_database"));

    client.setBaseUrl(TestUrl);
    QVERIFY(client.createDatabase("tst_database"));
    QVERIFY(client.deleteDatabase("tst_database"));
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

void tst_client::createDeleteDatabase_data()
{
    QTest::addColumn<QString>("method");
    QTest::addColumn<QNetworkAccessManager::Operation>("expectedOperation");
    QTest::addColumn<QUrl>("expectedUrl");
    QTest::addColumn<QString>("expectedSignal");

    QTest::newRow("create") << "createDatabase" << QNetworkAccessManager::PutOperation << TestUrl.resolved(QUrl("/tst_database")) << "databaseCreated(QString)";
    QTest::newRow("delete") << "deleteDatabase" << QNetworkAccessManager::DeleteOperation << TestUrl.resolved(QUrl("/tst_database")) << "databaseDeleted(QString)";
}

void tst_client::createDeleteDatabase()
{
    QFETCH(QString, method);
    QFETCH(QNetworkAccessManager::Operation, expectedOperation);
    QFETCH(QUrl, expectedUrl);
    QFETCH(QString, expectedSignal);

    CouchClient client(TestUrl);

    QSignalSpy databaseSpy(&client, QByteArray::number(QSIGNAL_CODE) + expectedSignal.toLatin1());
    QVERIFY(databaseSpy.isValid());

    TestNetworkAccessManager manager;
    client.setNetworkAccessManager(&manager);

    QVERIFY(QMetaObject::invokeMethod(&client, method.toLatin1(), Q_ARG(QString, "tst_database")));
    QCOMPARE(manager.operations, {expectedOperation});
    QCOMPARE(manager.urls, {expectedUrl});

    QVERIFY(databaseSpy.wait());
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

void tst_client::busy()
{
    CouchClient client(TestUrl);
    QVERIFY(!client.isBusy());

    TestNetworkAccessManager manager;
    client.setNetworkAccessManager(&manager);

    QSignalSpy busySpy(&client, &CouchClient::busyChanged);
    QVERIFY(busySpy.isValid());

    client.listAllDatabases();
    QVERIFY(client.isBusy());
    QCOMPARE(busySpy.count(), 1);
    QCOMPARE(busySpy.takeFirst().value(0), true);

    QVERIFY(busySpy.wait());
    QCOMPARE(busySpy.count(), 1);
    QCOMPARE(busySpy.takeFirst().value(0), false);
}

QTEST_MAIN(tst_client)

#include "tst_client.moc"
