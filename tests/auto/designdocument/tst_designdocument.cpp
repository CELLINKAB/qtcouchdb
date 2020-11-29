#include <QtTest>
#include <QtCouchDB>

#include "tst_shared.h"

static const QByteArray TestDesignDocuments = R"({"rows":[{"key":"_design/foo"},{"key":"_design/bar"},{"key":"baz"}]})";
static const QByteArray TestDocument1 = R"({"id":"doc1","rev":"rev1","doc":{"foo":"bar"}})";
static const QByteArray TestDocument2 = R"({"id":"doc2","rev":"rev2","doc":{"baz":"qux"}})";
static const QByteArray TestDocuments = R"({"rows":[)" + TestDocument1 + "," + TestDocument2 + "]}";
static const QByteArray TestViews = R"({"views":{"foo":{},"bar":{},"baz":{}}})";

class tst_designdocument : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void database();
    void responses();
    void url();
    void name();
    void listViews_data();
    void listViews();
    void error();
};

void tst_designdocument::initTestCase()
{
    registerTestMetaTypes();
}

void tst_designdocument::database()
{
    CouchDesignDocument designDocument;

    QSignalSpy databaseSpy(&designDocument, &CouchDesignDocument::databaseChanged);
    QVERIFY(databaseSpy.isValid());

    QSignalSpy clientSpy(&designDocument, &CouchDesignDocument::clientChanged);
    QVERIFY(clientSpy.isValid());

    CouchDatabase database;
    QVERIFY(!designDocument.database());
    designDocument.setDatabase(&database);
    QCOMPARE(designDocument.database(), &database);
    QCOMPARE(databaseSpy.count(), 1);
    QCOMPARE(clientSpy.count(), 0);

    CouchClient client;
    QVERIFY(!designDocument.client());
    database.setClient(&client);
    QCOMPARE(designDocument.client(), &client);
    QCOMPARE(clientSpy.count(), 1);
    QCOMPARE(databaseSpy.count(), 1);

    database.setClient(nullptr);
    QVERIFY(!designDocument.client());
    QCOMPARE(clientSpy.count(), 2);
    QCOMPARE(databaseSpy.count(), 1);

    designDocument.setDatabase(nullptr);
    QVERIFY(!designDocument.database());
    QCOMPARE(databaseSpy.count(), 2);
    QCOMPARE(clientSpy.count(), 2);
}

void tst_designdocument::responses()
{
    CouchDesignDocument designDocument("tst_designdocument");
    QVERIFY(!designDocument.listViewIds());
    QVERIFY(!designDocument.listFullViews());

    CouchDatabase database("tst_database");
    designDocument.setDatabase(&database);
    QVERIFY(!designDocument.listViewIds());
    QVERIFY(!designDocument.listFullViews());

    CouchClient client(TestUrl);
    database.setClient(&client);
    QVERIFY(designDocument.listViewIds());
    QVERIFY(designDocument.listFullViews());
}

void tst_designdocument::url()
{
    CouchDesignDocument designDocument("tst_designdocument");
    QCOMPARE(designDocument.url(), QUrl());

    CouchClient client(TestUrl);
    CouchDatabase database("tst_database", &client);

    QSignalSpy urlChanged(&designDocument, &CouchDesignDocument::urlChanged);
    QVERIFY(urlChanged.isValid());

    designDocument.setDatabase(&database);
    QCOMPARE(designDocument.url(), TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument")));
    QCOMPARE(urlChanged.count(), 1);

    designDocument.setName("tst_designdocument");
    QCOMPARE(urlChanged.count(), 1);

    database.setName("tst_database2");
    QCOMPARE(designDocument.url(), TestUrl.resolved(QUrl("/tst_database2/_design/tst_designdocument")));
    QCOMPARE(urlChanged.count(), 2);

    client.setBaseUrl(TestUrl.resolved(QUrl("/client")));
    QCOMPARE(designDocument.url(), TestUrl.resolved(QUrl("/client/tst_database2/_design/tst_designdocument")));
    QCOMPARE(urlChanged.count(), 3);
}

void tst_designdocument::name()
{
    CouchDesignDocument designDocument;
    QCOMPARE(designDocument.name(), QString());

    QSignalSpy nameChanged(&designDocument, &CouchDesignDocument::nameChanged);
    QVERIFY(nameChanged.isValid());

    designDocument.setName("tst_designdocument");
    QCOMPARE(designDocument.name(), "tst_designdocument");
    QCOMPARE(nameChanged.count(), 1);
}

void tst_designdocument::listViews_data()
{
    QTest::addColumn<QString>("method");
    QTest::addColumn<QUrl>("expectedUrl");

    QTest::newRow("ids") << "listViewIds" << QUrl("/tst_database/_design/tst_designdocument");
    QTest::newRow("full") << "listFullViews" << QUrl("/tst_database/_design/tst_designdocument?include_docs=true");
}

void tst_designdocument::listViews()
{
    QFETCH(QString, method);
    QFETCH(QUrl, expectedUrl);

    CouchClient client(TestUrl);
    CouchDatabase database("tst_database", &client);
    CouchDesignDocument designDocument("tst_designdocument", &database);

    QSignalSpy designDocumentSpy(&designDocument, &CouchDesignDocument::viewsListed);
    QVERIFY(designDocumentSpy.isValid());

    TestNetworkAccessManager manager(TestViews);
    client.setNetworkAccessManager(&manager);

    QVERIFY(QMetaObject::invokeMethod(&designDocument, method.toLatin1()));
    QCOMPARE(manager.operations, {QNetworkAccessManager::GetOperation});
    QCOMPARE(manager.urls, {TestUrl.resolved(expectedUrl)});

    QVERIFY(designDocumentSpy.wait());
    QVariantList args = designDocumentSpy.takeFirst();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.first().toStringList(), QStringList({"bar", "baz", "foo"}));
}

void tst_designdocument::error()
{
    CouchClient client(TestUrl);
    CouchDatabase database("tst_database", &client);
    CouchDesignDocument designDocument("tst_designdocument", &database);

    TestNetworkAccessManager manager(QNetworkReply::UnknownServerError);
    client.setNetworkAccessManager(&manager);

    QSignalSpy errorSpy(&designDocument, &CouchDesignDocument::errorOccurred);
    QVERIFY(errorSpy.isValid());

    designDocument.listFullViews();
    QVERIFY(errorSpy.wait());
}

QTEST_MAIN(tst_designdocument)

#include "tst_designdocument.moc"
