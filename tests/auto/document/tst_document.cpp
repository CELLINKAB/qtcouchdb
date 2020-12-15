#include <QtTest>
#include <QtCouchDB>

class tst_document : public QObject
{
    Q_OBJECT

private slots:
    void test();
    void json_data();
    void json();
    void debug();
};

void tst_document::test()
{
    CouchDocument doc1;
    QCOMPARE(doc1.id(), QString());
    QCOMPARE(doc1.revision(), QString());
    QCOMPARE(doc1.content(), QByteArray());

    CouchDocument doc2("id", "rev");
    QCOMPARE(doc2.id(), "id");
    QCOMPARE(doc2.revision(), "rev");
    QCOMPARE(doc2.content(), QByteArray());

    QVERIFY(doc1 != doc2);
    QVERIFY(doc1 == CouchDocument(doc1));
    QVERIFY(doc2 == CouchDocument(doc2));

    doc1 = doc2;
    QCOMPARE(doc1.id(), "id");
    QCOMPARE(doc1.revision(), "rev");
    QCOMPARE(doc2.content(), QByteArray());
    QVERIFY(doc1 == doc2);

    CouchDocument doc3 = doc2.withRevision("rev3");
    QCOMPARE(doc1.revision(), "rev");
    QCOMPARE(doc2.revision(), "rev");
    QCOMPARE(doc3.revision(), "rev3");
    QVERIFY(doc2 != doc3);

    CouchDocument doc4 = doc3.withContent("content");
    QCOMPARE(doc1.content(), QByteArray());
    QCOMPARE(doc2.content(), QByteArray());
    QCOMPARE(doc3.content(), QByteArray());
    QCOMPARE(doc4.content(), QByteArray("content"));
    QVERIFY(doc3 != doc4);

    doc1 = "id1";
    QCOMPARE(doc1.id(), "id1");
    QCOMPARE(doc1.revision(), QString());
    QCOMPARE(doc1.content(), QByteArray());
}

void tst_document::json_data()
{
    QTest::addColumn<QJsonObject>("json");
    QTest::addColumn<QJsonObject>("expectedJson");

    QJsonObject content;
    content.insert("foo", "bar");

    QJsonObject doc({{"id","id1"},{"rev","rev1"},{"doc",content}});

    QTest::newRow("id,rev") << doc << doc;
    QTest::newRow("_id,_rev") << QJsonObject({{"_id","id1"},{"_rev","rev1"},{"doc",content}}) << doc;
    QTest::newRow("doc") << QJsonObject({{"_id","id1"},{"_rev","rev1"},{"foo","bar"}}) << doc;
}

void tst_document::json()
{
    QFETCH(QJsonObject, json);
    QFETCH(QJsonObject, expectedJson);

    CouchDocument doc = CouchDocument::fromJson(json);
    QCOMPARE(doc.id(), "id1");
    QCOMPARE(doc.revision(), "rev1");
    QCOMPARE(doc.content(), R"({"foo":"bar"})");
    QCOMPARE(doc.toJson(), expectedJson);
}

void tst_document::debug()
{
    QString str;
    QDebug(&str) << CouchDocument("foo", "2");
    QCOMPARE(str, "CouchDocument(foo, rev=2) ");
}

QTEST_MAIN(tst_document)

#include "tst_document.moc"
