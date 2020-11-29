#include <QtTest>
#include <QtCouchDB>

class tst_document : public QObject
{
    Q_OBJECT

private slots:
    void test();
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

    doc2.setContent("content");
    QCOMPARE(doc1.content(), QByteArray());
    QCOMPARE(doc2.content(), QByteArray("content"));
    QVERIFY(doc1 != doc2);

    doc1 = "id1";
    QCOMPARE(doc1.id(), "id1");
    QCOMPARE(doc1.revision(), QString());
    QCOMPARE(doc1.content(), QByteArray());
}

void tst_document::debug()
{
    QString str;
    QDebug(&str) << CouchDocument("foo", "2");
    QCOMPARE(str, "CouchDocument(foo, rev=2) ");
}

QTEST_MAIN(tst_document)

#include "tst_document.moc"
