#include <QtTest>
#include <QtCouchDB>

class tst_error : public QObject
{
    Q_OBJECT

private slots:
    void test();
    void debug();
};

void tst_error::test()
{
    CouchError err1;
    QCOMPARE(err1.error(), QString());
    QCOMPARE(err1.reason(), QString());

    CouchError err2("error", "reason");
    QCOMPARE(err2.error(), "error");
    QCOMPARE(err2.reason(), "reason");

    QVERIFY(err1 == err1);
    QVERIFY(err2 == err2);
    QVERIFY(err1 != err2);

    err1 = err2;
    QCOMPARE(err1.error(), "error");
    QCOMPARE(err1.reason(), "reason");
    QVERIFY(err1 == err2);
}

void tst_error::debug()
{
    QString str;
    QDebug(&str) << CouchError("foo", "bar");
    QCOMPARE(str, "CouchError(foo, \"bar\") ");
}

QTEST_MAIN(tst_error)

#include "tst_error.moc"
