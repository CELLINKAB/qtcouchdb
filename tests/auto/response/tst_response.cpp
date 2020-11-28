#include <QtTest>
#include <QtCouchDB>

class tst_response : public QObject
{
    Q_OBJECT

private slots:
    void json();
    void request();
    void error();
};

void tst_response::json()
{
    CouchResponse response;
    QJsonDocument json(QJsonObject({{"foo","bar"}}));
    response.setData(json.toJson());
    QCOMPARE(response.data(), json.toJson());
    QCOMPARE(response.toJson(), json.object());
}

void tst_response::request()
{
    CouchRequest request(CouchRequest::Post);
    request.setUrl(QUrl("foo:bar"));
    request.setBody("foobar");
    request.setHeader("foo", "bar");

    CouchResponse response(request);
    QCOMPARE(response.request(), request);
}

void tst_response::error()
{
    QJsonDocument json(QJsonObject({{"error","foo"},{"reason","bar"}}));
    CouchResponse response;
    response.setData(json.toJson());
    QCOMPARE(response.error(), CouchError("foo", "bar"));
}

QTEST_MAIN(tst_response)

#include "tst_response.moc"
