#include <QtTest>
#include <QtCouchDB>

class tst_request : public QObject
{
    Q_OBJECT

private slots:
    void test();
    void debug();
};

void tst_request::test()
{
    typedef QHash<QByteArray, QByteArray> QByteArrayHash;

    CouchRequest r1;
    QCOMPARE(r1.url(), QUrl());
    QCOMPARE(r1.operation(), CouchRequest::Get);
    QCOMPARE(r1.body(), QByteArray());
    QCOMPARE(r1.headers(), QByteArrayHash());

    CouchRequest r2(CouchRequest::Post);
    r2.setUrl(QUrl("foo:bar"));
    r2.setBody("foobar");
    r2.setHeader("foo", "bar");
    QCOMPARE(r2.url(), QUrl("foo:bar"));
    QCOMPARE(r2.operation(), CouchRequest::Post);
    QCOMPARE(r2.body(), QByteArray("foobar"));
    QCOMPARE(r2.headers(), QByteArrayHash({{"foo", "bar"}}));
    QCOMPARE(r2.header("foo"), "bar");

    QVERIFY(r1 == r1);
    QVERIFY(r2 == r2);
    QVERIFY(r1 != r2);

    r1 = r2;
    QCOMPARE(r1.url(), QUrl("foo:bar"));
    QCOMPARE(r1.operation(), CouchRequest::Post);
    QCOMPARE(r1.body(), QByteArray("foobar"));
    QCOMPARE(r1.headers(), QByteArrayHash({{"foo", "bar"}}));
    QCOMPARE(r1.header("foo"), "bar");
    QVERIFY(r1 == r2);

    r1.setHeader("baz", "qux");
    QCOMPARE(r1.headers(), QByteArrayHash({{"foo", "bar"}}));
    QCOMPARE(r2.headers(), QByteArrayHash({{"foo", "bar"},{"baz","qux"}}));
    QCOMPARE(r1.header("baz"), QByteArray());
    QCOMPARE(r2.header("baz"), "qux");
    QVERIFY(r1 != r2);
}

void tst_request::debug()
{
    QString str;
    CouchRequest request;
    request.setOperation(CouchRequest::Post);
    request.setUrl(QUrl("http://admin:password@couch.db:5984"));
    request.setHeader("foo", "bar");
    request.setBody("foobar");
    QDebug(&str) << request;
    QCOMPARE(str, "CouchRequest(Post, http://admin:xxxxx@couch.db:5984, \"foobar\") ");
}

QTEST_MAIN(tst_request)

#include "tst_request.moc"
