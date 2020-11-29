#include <QtTest>
#include <QtCouchDB>

#include "tst_shared.h"

static const QByteArray TestDesignDocuments = R"({"rows":[{"key":"_design/foo"},{"key":"_design/bar"},{"key":"baz"}]})";
static const QByteArray TestDocument1 = R"({"id":"doc1","rev":"rev1","doc":{"foo":"bar"}})";
static const QByteArray TestDocument2 = R"({"id":"doc2","rev":"rev2","doc":{"baz":"qux"}})";
static const QByteArray TestDocuments = R"({"rows":[)" + TestDocument1 + "," + TestDocument2 + "]}";

class tst_database : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void client();
    void responses();
    void url();
    void name();
    void listDesignDocuments();
    void createDeleteDesignDocument_data();
    void createDeleteDesignDocument();
    void listDocuments_data();
    void listDocuments();
    void queryDocuments_data();
    void queryDocuments();
    void document_data();
    void document();
    void error();
};

void tst_database::initTestCase()
{
    registerTestMetaTypes();
}

void tst_database::client()
{
    CouchDatabase database;
    QVERIFY(!database.client());

    QSignalSpy clientSpy(&database, &CouchDatabase::clientChanged);
    QVERIFY(clientSpy.isValid());

    CouchClient client;
    database.setClient(&client);
    QCOMPARE(database.client(), &client);
    QCOMPARE(clientSpy.count(), 1);

    database.setClient(nullptr);
    QVERIFY(!database.client());
    QCOMPARE(clientSpy.count(), 2);
}

void tst_database::responses()
{
    CouchDatabase database("tst_database");
    QVERIFY(!database.listDesignDocuments());
    QVERIFY(!database.createDesignDocument(QString()));
    QVERIFY(!database.deleteDesignDocument(QString()));
    QVERIFY(!database.listDocumentIds());
    QVERIFY(!database.listFullDocuments());
    QVERIFY(!database.queryDocuments(CouchQuery()));
    QVERIFY(!database.createDocument(CouchDocument()));
    QVERIFY(!database.getDocument(CouchDocument()));
    QVERIFY(!database.updateDocument(CouchDocument()));
    QVERIFY(!database.deleteDocument(CouchDocument()));

    CouchClient client(TestUrl);
    database.setClient(&client);
    QVERIFY(database.listDesignDocuments());
    QVERIFY(database.createDesignDocument(QString()));
    QVERIFY(database.deleteDesignDocument(QString()));
    QVERIFY(database.listDocumentIds());
    QVERIFY(database.listFullDocuments());
    QVERIFY(database.queryDocuments(CouchQuery()));
    QVERIFY(database.createDocument(CouchDocument()));
    QVERIFY(database.getDocument(CouchDocument()));
    QVERIFY(database.updateDocument(CouchDocument()));
    QVERIFY(database.deleteDocument(CouchDocument()));
}

void tst_database::url()
{
    CouchDatabase database("tst_database");
    QCOMPARE(database.url(), QUrl());

    CouchClient client(TestUrl);

    QSignalSpy urlChanged(&database, &CouchDatabase::urlChanged);
    QVERIFY(urlChanged.isValid());

    database.setClient(&client);
    QCOMPARE(database.url(), TestUrl.resolved(QUrl("/tst_database")));
    QCOMPARE(urlChanged.count(), 1);

    database.setName("tst_database");
    QCOMPARE(urlChanged.count(), 1);

    client.setBaseUrl(TestUrl.resolved(QUrl("/client")));
    QCOMPARE(database.url(), TestUrl.resolved(QUrl("/client/tst_database")));
    QCOMPARE(urlChanged.count(), 2);
}

void tst_database::name()
{
    CouchDatabase database;
    QCOMPARE(database.name(), QString());

    QSignalSpy nameChanged(&database, &CouchDatabase::nameChanged);
    QVERIFY(nameChanged.isValid());

    database.setName("tst_database");
    QCOMPARE(database.name(), "tst_database");
    QCOMPARE(nameChanged.count(), 1);
}

void tst_database::listDesignDocuments()
{
    CouchClient client;
    client.setBaseUrl(TestUrl);

    CouchDatabase database("tst_database", &client);

    QSignalSpy databaseSpy(&database, &CouchDatabase::designDocumentsListed);
    QVERIFY(databaseSpy.isValid());

    TestNetworkAccessManager manager(TestDesignDocuments);
    client.setNetworkAccessManager(&manager);

    database.listDesignDocuments();
    QCOMPARE(manager.operations, {QNetworkAccessManager::GetOperation});
    QCOMPARE(manager.urls, {TestUrl.resolved(QUrl("/tst_database/_design_docs"))});

    QVERIFY(databaseSpy.wait());
    QVariantList args = databaseSpy.takeFirst();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.first().toStringList(), QStringList({"foo", "bar", "baz"}));
}

void tst_database::createDeleteDesignDocument_data()
{
    QTest::addColumn<QString>("method");
    QTest::addColumn<QNetworkAccessManager::Operation>("expectedOperation");
    QTest::addColumn<QUrl>("expectedUrl");
    QTest::addColumn<QString>("expectedSignal");

    QTest::newRow("create") << "createDesignDocument" << QNetworkAccessManager::PutOperation << TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument")) << "designDocumentCreated(QString)";
    QTest::newRow("delete") << "deleteDesignDocument" << QNetworkAccessManager::DeleteOperation << TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument")) << "designDocumentDeleted(QString)";
}

void tst_database::createDeleteDesignDocument()
{
    QFETCH(QString, method);
    QFETCH(QNetworkAccessManager::Operation, expectedOperation);
    QFETCH(QUrl, expectedUrl);
    QFETCH(QString, expectedSignal);

    CouchClient client(TestUrl);
    CouchDatabase database("tst_database", &client);

    QSignalSpy designDocumentSpy(&database, QByteArray::number(QSIGNAL_CODE) + expectedSignal.toLatin1());
    QVERIFY(designDocumentSpy.isValid());

    TestNetworkAccessManager manager;
    client.setNetworkAccessManager(&manager);

    QVERIFY(QMetaObject::invokeMethod(&database, method.toLatin1(), Q_ARG(QString, "tst_designdocument")));
    QCOMPARE(manager.operations, {expectedOperation});
    QCOMPARE(manager.urls, {expectedUrl});

    QVERIFY(designDocumentSpy.wait());
}

void tst_database::listDocuments_data()
{
    QTest::addColumn<QString>("method");
    QTest::addColumn<QUrl>("expectedUrl");

    QTest::newRow("ids") << "listDocumentIds" << QUrl("/tst_database/_all_docs");
    QTest::newRow("full") << "listFullDocuments" << QUrl("/tst_database/_all_docs?include_docs=true");
}

void tst_database::listDocuments()
{
    QFETCH(QString, method);
    QFETCH(QUrl, expectedUrl);

    CouchClient client;
    client.setBaseUrl(TestUrl);

    CouchDatabase database("tst_database", &client);

    QSignalSpy documentSpy(&database, &CouchDatabase::documentsListed);
    QVERIFY(documentSpy.isValid());

    TestNetworkAccessManager manager(TestDocuments);
    client.setNetworkAccessManager(&manager);

    QVERIFY(QMetaObject::invokeMethod(&database, method.toLatin1()));
    QCOMPARE(manager.operations, {QNetworkAccessManager::GetOperation});
    QCOMPARE(manager.urls, {TestUrl.resolved(expectedUrl)});

    CouchDocument doc1 = CouchDocument::fromJson(QJsonDocument::fromJson(TestDocument1).object());
    CouchDocument doc2 = CouchDocument::fromJson(QJsonDocument::fromJson(TestDocument2).object());

    QVERIFY(documentSpy.wait());
    QVariantList args = documentSpy.takeFirst();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.first().value<QList<CouchDocument>>(), QList<CouchDocument>({doc1, doc2}));
}

void tst_database::queryDocuments_data()
{
    QTest::addColumn<CouchQuery>("query");
    QTest::addColumn<QUrl>("expectedUrl");

    QTest::newRow("none") << CouchQuery() << TestUrl.resolved(QUrl("/tst_database/_all_docs"));
    {
        CouchQuery query;
        query.setLimit(1);
        QTest::newRow("limit") << query << TestUrl.resolved(QUrl("/tst_database/_all_docs?limit=1"));
    }
    {
        CouchQuery query;
        query.setSkip(2);
        QTest::newRow("skip") << query << TestUrl.resolved(QUrl("/tst_database/_all_docs?skip=2"));
    }
    {
        CouchQuery query;
        query.setOrder(Qt::DescendingOrder);
        QTest::newRow("descending") << query << TestUrl.resolved(QUrl("/tst_database/_all_docs?descending=true"));
    }
    {
        CouchQuery query;
        query.setIncludeDocs(true);
        QTest::newRow("include_docs") << query << TestUrl.resolved(QUrl("/tst_database/_all_docs?include_docs=true"));
    }
    {
        CouchQuery query;
        query.setLimit(1);
        query.setSkip(2);
        query.setIncludeDocs(true);
        query.setOrder(Qt::DescendingOrder);
        query.setIncludeDocs(true);
        QTest::newRow("all") << query << TestUrl.resolved(QUrl("/tst_database/_all_docs?limit=1&skip=2&descending=true&include_docs=true"));
    }
}

void tst_database::queryDocuments()
{
    QFETCH(CouchQuery, query);
    QFETCH(QUrl, expectedUrl);

    CouchClient client;
    client.setBaseUrl(TestUrl);

    CouchDatabase database("tst_database", &client);

    QSignalSpy documentSpy(&database, &CouchDatabase::documentsListed);
    QVERIFY(documentSpy.isValid());

    TestNetworkAccessManager manager(TestDocuments);
    client.setNetworkAccessManager(&manager);

    database.queryDocuments(query);
    QCOMPARE(manager.operations, {QNetworkAccessManager::GetOperation});
    QCOMPARE(manager.urls, {expectedUrl});

    CouchDocument doc1 = CouchDocument::fromJson(QJsonDocument::fromJson(TestDocument1).object());
    CouchDocument doc2 = CouchDocument::fromJson(QJsonDocument::fromJson(TestDocument2).object());

    QVERIFY(documentSpy.wait());
    QVariantList args = documentSpy.takeFirst();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.first().value<QList<CouchDocument>>(), QList<CouchDocument>({doc1, doc2}));
}

void tst_database::document_data()
{
    QTest::addColumn<QString>("method");
    QTest::addColumn<QNetworkAccessManager::Operation>("expectedOperation");
    QTest::addColumn<QUrl>("expectedUrl");
    QTest::addColumn<QString>("expectedSignal");

    QTest::newRow("create") << "createDocument" << QNetworkAccessManager::PostOperation << TestUrl.resolved(QUrl("/tst_database")) << "documentCreated(CouchDocument)";
    QTest::newRow("get") << "getDocument" << QNetworkAccessManager::GetOperation << TestUrl.resolved(QUrl("/tst_database/doc1?rev=rev1")) << "documentReceived(CouchDocument)";
    QTest::newRow("update") << "updateDocument" << QNetworkAccessManager::PutOperation << TestUrl.resolved(QUrl("/tst_database/doc1?rev=rev1")) << "documentUpdated(CouchDocument)";
    QTest::newRow("delete") << "deleteDocument" << QNetworkAccessManager::DeleteOperation << TestUrl.resolved(QUrl("/tst_database/doc1?rev=rev1")) << "documentDeleted(CouchDocument)";
}

void tst_database::document()
{
    QFETCH(QString, method);
    QFETCH(QNetworkAccessManager::Operation, expectedOperation);
    QFETCH(QUrl, expectedUrl);
    QFETCH(QString, expectedSignal);

    CouchClient client;
    client.setBaseUrl(TestUrl);

    CouchDatabase database("tst_database", &client);

    QSignalSpy documentSpy(&database, QByteArray::number(QSIGNAL_CODE) + expectedSignal.toLatin1());
    QVERIFY(documentSpy.isValid());

    TestNetworkAccessManager manager(TestDocument1);
    client.setNetworkAccessManager(&manager);

    CouchDocument doc1 = CouchDocument::fromJson(QJsonDocument::fromJson(TestDocument1).object());

    QVERIFY(QMetaObject::invokeMethod(&database, method.toLatin1(), Q_ARG(CouchDocument, doc1)));
    QCOMPARE(manager.operations, {expectedOperation});
    QCOMPARE(manager.urls, {expectedUrl});

    QVERIFY(documentSpy.wait());
    QVariantList args = documentSpy.takeFirst();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.first().value<CouchDocument>(), doc1);
}

void tst_database::error()
{
    CouchClient client;
    client.setBaseUrl(TestUrl);

    CouchDatabase database("tst_database", &client);

    TestNetworkAccessManager manager(QNetworkReply::UnknownServerError);
    client.setNetworkAccessManager(&manager);

    QSignalSpy errorSpy(&database, &CouchDatabase::errorOccurred);
    QVERIFY(errorSpy.isValid());

    database.listFullDocuments();
    QVERIFY(errorSpy.wait());
}

QTEST_MAIN(tst_database)

#include "tst_database.moc"
