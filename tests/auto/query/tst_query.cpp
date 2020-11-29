#include <QtTest>
#include <QtCouchDB>

class tst_query : public QObject
{
    Q_OBJECT

private slots:
    void test();
    void debug();
};

void tst_query::test()
{
    CouchQuery q1;
    QCOMPARE(q1.limit(), 0);
    QCOMPARE(q1.skip(), 0);
    QCOMPARE(q1.order(), Qt::AscendingOrder);
    QCOMPARE(q1.includeDocs(), false);

    CouchQuery q2;
    q2.setLimit(1);
    q2.setSkip(2);
    q2.setOrder(Qt::DescendingOrder);
    q2.setIncludeDocs(true);
    QCOMPARE(q2.limit(), 1);
    QCOMPARE(q2.skip(), 2);
    QCOMPARE(q2.order(), Qt::DescendingOrder);
    QCOMPARE(q2.includeDocs(), true);

    QVERIFY(q1 != q2);
    QVERIFY(q1 == CouchQuery(q1));
    QVERIFY(q2 == CouchQuery(q2));

    q1 = q2;
    QCOMPARE(q1.limit(), 1);
    QCOMPARE(q1.skip(), 2);
    QCOMPARE(q1.order(), Qt::DescendingOrder);
    QCOMPARE(q1.includeDocs(), true);
    QVERIFY(q1 == q2);

    q2.setOrder(Qt::AscendingOrder);
    QCOMPARE(q1.order(), Qt::DescendingOrder);
    QCOMPARE(q2.order(), Qt::AscendingOrder);
    QVERIFY(q1 != q2);
}

void tst_query::debug()
{
    QString str;
    CouchQuery query;
    query.setLimit(1);
    query.setSkip(2);
    query.setIncludeDocs(true);
    query.setOrder(Qt::DescendingOrder);
    query.setIncludeDocs(true);
    QDebug(&str) << query;
    QCOMPARE(str, "CouchQuery(limit=1, skip=2, descending=true, include_docs=true) ");
}

QTEST_MAIN(tst_query)

#include "tst_query.moc"
