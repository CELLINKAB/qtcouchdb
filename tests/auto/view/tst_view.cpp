#include <QtTest>
#include <QtCouchDB>

#include "tst_shared.h"

static const QByteArray TestRows = R"({"rows":[{"foo":"bar"},{"baz":"qux"}]})";

class tst_view : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void designDocument();
    void responses();
    void url();
    void name();
    void listRows_data();
    void listRows();
    void queryRows_data();
    void queryRows();
    void error();
};

void tst_view::initTestCase()
{
    registerTestMetaTypes();
}

void tst_view::designDocument()
{
    CouchView view;

    QSignalSpy designDocumentSpy(&view, &CouchView::designDocumentChanged);
    QVERIFY(designDocumentSpy.isValid());

    QSignalSpy databaseSpy(&view, &CouchView::databaseChanged);
    QVERIFY(databaseSpy.isValid());

    QSignalSpy clientSpy(&view, &CouchView::clientChanged);
    QVERIFY(clientSpy.isValid());

    CouchDesignDocument designDocument;
    QVERIFY(!view.database());
    view.setDesignDocument(&designDocument);
    QCOMPARE(view.designDocument(), &designDocument);
    QCOMPARE(designDocumentSpy.count(), 1);
    QCOMPARE(databaseSpy.count(), 0);
    QCOMPARE(clientSpy.count(), 0);

    CouchDatabase database;
    QVERIFY(!view.database());
    designDocument.setDatabase(&database);
    QCOMPARE(view.database(), &database);
    QCOMPARE(designDocumentSpy.count(), 1);
    QCOMPARE(databaseSpy.count(), 1);
    QCOMPARE(clientSpy.count(), 0);

    CouchClient client;
    QVERIFY(!view.client());
    database.setClient(&client);
    QCOMPARE(view.client(), &client);
    QCOMPARE(designDocumentSpy.count(), 1);
    QCOMPARE(databaseSpy.count(), 1);
    QCOMPARE(clientSpy.count(), 1);

    database.setClient(nullptr);
    QVERIFY(!view.client());
    QCOMPARE(designDocumentSpy.count(), 1);
    QCOMPARE(databaseSpy.count(), 1);
    QCOMPARE(clientSpy.count(), 2);

    designDocument.setDatabase(nullptr);
    QVERIFY(!view.database());
    QCOMPARE(designDocumentSpy.count(), 1);
    QCOMPARE(databaseSpy.count(), 2);
    QCOMPARE(clientSpy.count(), 2);

    view.setDesignDocument(nullptr);
    QVERIFY(!view.designDocument());
    QCOMPARE(designDocumentSpy.count(), 2);
    QCOMPARE(databaseSpy.count(), 2);
    QCOMPARE(clientSpy.count(), 2);
}

void tst_view::responses()
{
    CouchView view("tst_view");
    QVERIFY(!view.listRowIds());
    QVERIFY(!view.listFullRows());
    QVERIFY(!view.queryRows(CouchQuery()));

    CouchDesignDocument designDocument("tst_designdocument");
    view.setDesignDocument(&designDocument);
    QVERIFY(!view.listRowIds());
    QVERIFY(!view.listFullRows());
    QVERIFY(!view.queryRows(CouchQuery()));

    CouchDatabase database("tst_database");
    designDocument.setDatabase(&database);
    QVERIFY(!view.listRowIds());
    QVERIFY(!view.listFullRows());
    QVERIFY(!view.queryRows(CouchQuery()));

    CouchClient client(TestUrl);
    database.setClient(&client);
    QVERIFY(view.listRowIds());
    QVERIFY(view.listFullRows());
    QVERIFY(view.queryRows(CouchQuery()));
}

void tst_view::url()
{
    CouchView view("tst_view");
    QCOMPARE(view.url(), QUrl());

    CouchClient client(TestUrl);
    CouchDatabase database("tst_database", &client);
    CouchDesignDocument designDocument("tst_designdocument", &database);

    QSignalSpy urlChanged(&view, &CouchView::urlChanged);
    QVERIFY(urlChanged.isValid());

    view.setDesignDocument(&designDocument);
    QCOMPARE(view.url(), TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument/_view/tst_view")));
    QCOMPARE(urlChanged.count(), 1);

    view.setName("tst_view");
    QCOMPARE(urlChanged.count(), 1);

    view.setDesignDocument(&designDocument);
    QCOMPARE(view.url(), TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument/_view/tst_view")));
    QCOMPARE(urlChanged.count(), 1);

    designDocument.setDatabase(&database);
    QCOMPARE(view.url(), TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument/_view/tst_view")));
    QCOMPARE(urlChanged.count(), 1);

    designDocument.setName("tst_designdocument2");
    QCOMPARE(view.url(), TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument2/_view/tst_view")));
    QCOMPARE(urlChanged.count(), 2);

    database.setName("tst_database2");
    QCOMPARE(view.url(), TestUrl.resolved(QUrl("/tst_database2/_design/tst_designdocument2/_view/tst_view")));
    QCOMPARE(urlChanged.count(), 3);

    client.setBaseUrl(TestUrl.resolved(QUrl("/client")));
    QCOMPARE(view.url(), TestUrl.resolved(QUrl("/client/tst_database2/_design/tst_designdocument2/_view/tst_view")));
    QCOMPARE(urlChanged.count(), 4);
}

void tst_view::name()
{
    CouchView view;
    QCOMPARE(view.name(), QString());

    QSignalSpy nameChanged(&view, &CouchView::nameChanged);
    QVERIFY(nameChanged.isValid());

    view.setName("tst_view");
    QCOMPARE(view.name(), "tst_view");
    QCOMPARE(nameChanged.count(), 1);
}

void tst_view::listRows_data()
{
    QTest::addColumn<QString>("method");
    QTest::addColumn<QUrl>("expectedUrl");

    QTest::newRow("ids") << "listRowIds" << QUrl("/tst_database/_design/tst_designdocument/_view/tst_view");
    QTest::newRow("full") << "listFullRows" << QUrl("/tst_database/_design/tst_designdocument/_view/tst_view?include_docs=true");
}

void tst_view::listRows()
{
    QFETCH(QString, method);
    QFETCH(QUrl, expectedUrl);

    CouchClient client(TestUrl);
    CouchDatabase database("tst_database", &client);
    CouchDesignDocument designDocument("tst_designdocument", &database);
    CouchView view("tst_view", &designDocument);

    QSignalSpy viewSpy(&view, &CouchView::rowsListed);
    QVERIFY(viewSpy.isValid());

    TestNetworkAccessManager manager(TestRows);
    client.setNetworkAccessManager(&manager);

    QVERIFY(QMetaObject::invokeMethod(&view, method.toLatin1()));
    QCOMPARE(manager.operations, {QNetworkAccessManager::GetOperation});
    QCOMPARE(manager.urls, {TestUrl.resolved(expectedUrl)});

    QVERIFY(viewSpy.wait());
    QVariantList args = viewSpy.takeFirst();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.first().value<QJsonArray>(), QJsonArray({QJsonObject({{"foo", "bar"}}), QJsonObject({{"baz", "qux"}})}));
}

void tst_view::queryRows_data()
{
    QTest::addColumn<CouchQuery>("query");
    QTest::addColumn<QUrl>("expectedUrl");

    QTest::newRow("none") << CouchQuery() << TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument/_view/tst_view"));
    {
        CouchQuery query;
        query.setLimit(1);
        QTest::newRow("limit") << query << TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument/_view/tst_view?limit=1"));
    }
    {
        CouchQuery query;
        query.setSkip(2);
        QTest::newRow("skip") << query << TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument/_view/tst_view?skip=2"));
    }
    {
        CouchQuery query;
        query.setOrder(Qt::DescendingOrder);
        QTest::newRow("descending") << query << TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument/_view/tst_view?descending=true"));
    }
    {
        CouchQuery query;
        query.setIncludeDocs(true);
        QTest::newRow("include_docs") << query << TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument/_view/tst_view?include_docs=true"));
    }
    {
        CouchQuery query;
        query.setLimit(1);
        query.setSkip(2);
        query.setIncludeDocs(true);
        query.setOrder(Qt::DescendingOrder);
        query.setIncludeDocs(true);
        QTest::newRow("all") << query << TestUrl.resolved(QUrl("/tst_database/_design/tst_designdocument/_view/tst_view?limit=1&skip=2&descending=true&include_docs=true"));
    }
}

void tst_view::queryRows()
{
    QFETCH(CouchQuery, query);
    QFETCH(QUrl, expectedUrl);

    CouchClient client(TestUrl);
    CouchDatabase database("tst_database", &client);
    CouchDesignDocument designDocument("tst_designdocument", &database);
    CouchView view("tst_view", &designDocument);

    QSignalSpy rowSpy(&view, &CouchView::rowsListed);
    QVERIFY(rowSpy.isValid());

    TestNetworkAccessManager manager(TestRows);
    client.setNetworkAccessManager(&manager);

    view.queryRows(query);
    QCOMPARE(manager.operations, {QNetworkAccessManager::GetOperation});
    QCOMPARE(manager.urls, {expectedUrl});

    QJsonObject row1 = QJsonDocument::fromJson("{\"foo\":\"bar\"}").object();
    QJsonObject row2 = QJsonDocument::fromJson("{\"baz\":\"qux\"}").object();

    QVERIFY(rowSpy.wait());
    QVariantList args = rowSpy.takeFirst();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.first().value<QJsonArray>(), QJsonArray({row1, row2}));
}

void tst_view::error()
{
    CouchClient client(TestUrl);
    CouchDatabase database("tst_database", &client);
    CouchDesignDocument designDocument("tst_designdocument", &database);
    CouchView view("tst_view", &designDocument);

    TestNetworkAccessManager manager(QNetworkReply::UnknownServerError);
    client.setNetworkAccessManager(&manager);

    QSignalSpy errorSpy(&view, &CouchView::errorOccurred);
    QVERIFY(errorSpy.isValid());

    view.listFullRows();
    QVERIFY(errorSpy.wait());
}

QTEST_MAIN(tst_view)

#include "tst_view.moc"
