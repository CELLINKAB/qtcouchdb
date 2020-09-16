#include <QtCouchDB>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CouchClient client;
    client.setServer(QUrl("http://admin:password@localhost:5984"));

    // \0/
    QObject::connect(&client, &CouchClient::installationChecked, [&](const CouchResponse &response) {
        qDebug() << "-> installationChecked" << response.toJson();
        QObject::connect(&client, &CouchClient::sessionStarted, [&](const CouchResponse &response) {
            qDebug() << "-> sessionStarted" << response.toJson();
            QObject::connect(&client, &CouchClient::databaseCreated, [&](const CouchResponse &response) {
                qDebug() << "-> databaseCreated" << response.toJson();
                QObject::connect(&client, &CouchClient::databasesListed, [&](const CouchResponse &response) {
                    qDebug() << "-> databasesListed" << response.toJson();
                    QObject::connect(&client, &CouchClient::databaseDeleted, [&](const CouchResponse &response) {
                        qDebug() << "-> databaseDeleted" << response.toJson();
                        QObject::connect(&client, &CouchClient::sessionEnded, [&](const CouchResponse &response) {
                            qDebug() << "-> sessionEnded" << response.toJson();
                            app.quit();
                        });
                        client.endSession();
                    });
                    client.deleteDatabase("qtcouchdb");
                });
                client.listDatabases();
            });
            client.createDatabase("qtcouchdb");
        });
        client.startSession("admin", "password");
    });
    client.checkInstallation();

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
