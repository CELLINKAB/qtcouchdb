#include <QtCouchDB>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    CouchClient client;
    client.setBaseUrl(QUrl("http://admin:password@localhost:5984"));
    client.listAllDatabases();

    CouchDatabase database("qtcouchdb", &client);

    QObject::connect(&database, &CouchDatabase::errorOccurred, [&](const CouchError &error) {
        qDebug() << "-> DB error:" << error;
        QCoreApplication::quit();
    });

    QObject::connect(&client, &CouchClient::databasesListed, [&](const QStringList &databases) {
        qDebug() << "-> listed DBs:" << databases;

        if (!databases.contains("qtcouchdb")) {
            database.createDatabase();
            QObject::connect(&database, &CouchDatabase::databaseCreated, [&database]() {
                qDebug() << "-> created DB:" << database.name();
                database.listAllDocuments(Couch::Query::IncludeDocs);
            });
        } else {
            database.listAllDocuments(Couch::Query::IncludeDocs);
        }
    });

    QObject::connect(&database, &CouchDatabase::documentsListed, [&](const QList<CouchDocument> &documents) {
        qDebug() << "-> listed DOCS:" << documents;

        if (documents.isEmpty()) {
            database.createDocument(R"({"foo":"bar"})");
            QObject::connect(&database, &CouchDatabase::documentCreated, [&](const CouchDocument &document) {
                qDebug() << "-> created DOC:" << document << document.toJson();
                database.listAllDocuments(Couch::Query::IncludeDocs);
            });
        } else {
            database.deleteDocument(documents.first());
        }
    });

    QObject::connect(&database, &CouchDatabase::documentDeleted, [&](const CouchDocument &document) {
        qDebug() << "-> deleted DOC:" << document << document.toJson();

        QObject::connect(&database, &CouchDatabase::databaseDeleted, [&database]() {
            qDebug() << "-> deleted DB:" << database.name();
            QCoreApplication::quit();
        });
        database.deleteDatabase();
    });

    return app.exec();
}
