#include <QtCouchDB>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CouchDB db;
    db.setServer(QUrl("http://admin:password@localhost:5984"));

    // \0/
    QObject::connect(&db, &CouchDB::installationChecked, [&](const CouchDBResponse &response) {
        qDebug() << "-> installationChecked" << response.toJson();
        QObject::connect(&db, &CouchDB::sessionStarted, [&](const CouchDBResponse &response) {
            qDebug() << "-> sessionStarted" << response.toJson();
            QObject::connect(&db, &CouchDB::databaseCreated, [&](const CouchDBResponse &response) {
                qDebug() << "-> databaseCreated" << response.toJson();
                QObject::connect(&db, &CouchDB::databasesListed, [&](const CouchDBResponse &response) {
                    qDebug() << "-> databasesListed" << response.toJson();
                    QObject::connect(&db, &CouchDB::databaseDeleted, [&](const CouchDBResponse &response) {
                        qDebug() << "-> databaseDeleted" << response.toJson();
                        QObject::connect(&db, &CouchDB::sessionEnded, [&](const CouchDBResponse &response) {
                            qDebug() << "-> sessionEnded" << response.toJson();
                            app.quit();
                        });
                        db.endSession();
                    });
                    db.deleteDatabase("qtcouchdb");
                });
                db.listDatabases();
            });
            db.createDatabase("qtcouchdb");
        });
        db.startSession("admin", "password");
    });
    db.checkInstallation();

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
