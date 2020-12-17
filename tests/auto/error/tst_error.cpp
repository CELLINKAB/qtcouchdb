#include <QtTest>
#include <QtCouchDB>

class tst_error : public QObject
{
    Q_OBJECT

private slots:
    void test();
    void withCode();
    void debug();
};

void tst_error::test()
{
    CouchError err1;
    QCOMPARE(err1.code(), -1);
    QCOMPARE(err1.error(), QString());
    QCOMPARE(err1.reason(), QString());

    CouchError err2(1, "error", "reason");
    QCOMPARE(err2.code(), 1);
    QCOMPARE(err2.error(), "error");
    QCOMPARE(err2.reason(), "reason");

    QVERIFY(err1 != err2);
    QVERIFY(err1 == CouchError(err1));
    QVERIFY(err2 == CouchError(err2));

    err1 = err2;
    QCOMPARE(err1.code(), 1);
    QCOMPARE(err1.error(), "error");
    QCOMPARE(err1.reason(), "reason");
    QVERIFY(err1 == err2);
}

void tst_error::withCode()
{
    CouchError error = CouchError("error", "reason").withCode(123);
    QCOMPARE(error.code(), 123);
    QCOMPARE(error.error(), "error");
    QCOMPARE(error.reason(), "reason");
}

void tst_error::debug()
{
    QString str;
    QDebug(&str) << CouchError(1, "foo", "bar");
    QCOMPARE(str, "CouchError(1, foo, \"bar\") ");
}

QTEST_MAIN(tst_error)

#include "tst_error.moc"
