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
                database.listAllDocuments();
            });
        } else {
            database.listAllDocuments();
        }
    });

    QObject::connect(&database, &CouchDatabase::documentsListed, [&](const QList<CouchDocumentId> &documents) {
        qDebug() << "-> listed DOCS:" << documents;

        if (documents.isEmpty()) {
            database.createDocument(R"({"foo":"bar"})");
            QObject::connect(&database, &CouchDatabase::documentCreated, [&](const CouchDocumentId &documentId) {
                qDebug() << "-> created DOC:" << documentId;
                database.deleteDocument(documentId);
            });
        } else {
            database.deleteDocument(documents.first());
        }
    });

    QObject::connect(&database, &CouchDatabase::documentDeleted, [&](const CouchDocumentId &documentId) {
        qDebug() << "-> deleted DOC:" << documentId;

        QObject::connect(&database, &CouchDatabase::databaseDeleted, [&database]() {
            qDebug() << "-> deleted DB:" << database.name();
            QCoreApplication::quit();
        });
        database.deleteDatabase();
    });

    return app.exec();
}
