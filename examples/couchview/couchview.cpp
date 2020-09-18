#include <QtCouchDB>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CouchClient client;
    client.setUrl(QUrl("http://admin:password@localhost:5984"));

    // \0/
    CouchDatabase database("qtcouchdb", &client);
    CouchResponse *response = database.create();
    QObject::connect(response, &CouchResponse::received, [&database, response]() {
        qDebug() << "-> create" << response->toJson();
        CouchResponse *response = database.list();
        QObject::connect(response, &CouchResponse::received, [&database, response]() {
            qDebug() << "-> list" << response->toJson();
            CouchResponse *response = database.destroy();
            QObject::connect(response, &CouchResponse::received, [&database, response]() {
                qDebug() << "-> destroy" << response->toJson();
                QCoreApplication::quit();
            });
        });
    });

    return app.exec();
}
