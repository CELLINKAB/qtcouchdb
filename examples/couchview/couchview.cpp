#include <QtCouchDB>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CouchClient client;
    client.setServerUrl(QUrl("http://admin:password@localhost:5984"));

    // \0/
    CouchResponse *response = client.sendRequest(Couch::checkInstallation(client.serverUrl()));
    QObject::connect(response, &CouchResponse::received, [&client, response]() {
        qDebug() << "-> checkInstallation" << response->toJson();
        CouchResponse *response = client.sendRequest(Couch::startSession(client.serverUrl()));
        QObject::connect(response, &CouchResponse::received, [&client, response]() {
            qDebug() << "-> startSession" << response->toJson();
            CouchResponse *response = client.sendRequest(Couch::createDatabase(client.databaseUrl("qtcouchdb")));
            QObject::connect(response, &CouchResponse::received, [&client, response]() {
                qDebug() << "-> createDatabase" << response->toJson();
                CouchResponse *response = client.sendRequest(Couch::listDatabases(client.serverUrl()));
                QObject::connect(response, &CouchResponse::received, [&client, response]() {
                    qDebug() << "-> listDatabases" << response->toJson();
                    CouchResponse *response = client.sendRequest(Couch::deleteDatabase(client.databaseUrl("qtcouchdb")));
                    QObject::connect(response, &CouchResponse::received, [&client, response]() {
                        qDebug() << "-> deleteDatabase" << response->toJson();
                        CouchResponse *response = client.sendRequest(Couch::endSession(client.serverUrl()));
                        QObject::connect(response, &CouchResponse::received, [response]() {
                            qDebug() << "-> endSession" << response->toJson();
                            QCoreApplication::quit();
                        });
                    });
                });
            });
        });
    });

//    void documentsListed(const CouchDBResponse &response);
//    void revisionRetrieved(const CouchDBResponse &response);
//    void documentRetrieved(const CouchDBResponse &response);
//    void documentUpdated(const CouchDBResponse &response);
//    void documentDeleted(const CouchDBResponse &response);
//    void attachmentUploaded(const CouchDBResponse &response);
//    void attachmentDeleted(const CouchDBResponse &response);
//    void databaseReplicated(const CouchDBResponse &response);

    return app.exec();
}
