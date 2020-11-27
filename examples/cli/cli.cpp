#include <QtCore>
#include <QtCouchDB>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser cmdLine;
    cmdLine.addHelpOption();
    cmdLine.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    cmdLine.addOptions({
        {{"u", "url"}, "CouchDB URL (default: http://localhost:5984)", "url", "http://localhost:5984"},
        {{"db", "database"}, "CouchDB Database", "database", ""},
        {{"dd", "design", "design-document"}, "CouchDB Design Document", "design-document", ""},
        {{"v", "view"}, "CouchDB View", "view", ""},
    });
    cmdLine.addPositionalArgument("command", "CouchDB command (available: list-dbs, list-designs, list-views, list-rows)", "command");
    cmdLine.process(QCoreApplication::arguments());

    CouchClient client;
    client.setBaseUrl(QUrl(cmdLine.value("url")));

    CouchDatabase database(cmdLine.value("database"), &client);
    CouchDesignDocument designDocument(cmdLine.value("design-document"), &database);
    CouchView view(cmdLine.value("view"), &designDocument);

    QString command = cmdLine.positionalArguments().value(0);
    if (command == "list-dbs" || command == "list-databases") {
        client.listAllDatabases();
    } else if (command == "list-designs" || command == "list-design-documents") {
        database.listAllDesignDocuments();
    } else if (command == "list-views") {
        designDocument.listAllViews();
    } else if (command == "list-rows") {
        view.listAllRows();
    } else {
        QTextStream(stderr) << app.applicationName() << ": Unknown command: '" << command << "'." << Qt::endl;
        exit(EXIT_FAILURE);
    }

    QObject::connect(&client, &CouchClient::databasesListed, [&](const QStringList &databases) {
        QTextStream(stdout) << databases.join(" ") << Qt::endl;
    });
    QObject::connect(&database, &CouchDatabase::designDocumentsListed, [&](const QStringList &designDocs) {
        QTextStream(stdout) << designDocs.join(" ") << Qt::endl;
    });
    QObject::connect(&designDocument, &CouchDesignDocument::viewsListed, [&](const QStringList &views) {
        QTextStream(stdout) << views.join(" ") << Qt::endl;
    });
    QObject::connect(&view, &CouchView::rowsListed, [&](const QJsonArray &rows) {
        QTextStream(stdout) << QJsonDocument(rows).toJson(QJsonDocument::Indented);
    });

    QObject::connect(&client, &CouchClient::errorOccurred, [&](CouchResponse *response) {
        QTextStream(stderr) << response->error().error() << response->error().reason() << Qt::endl;
        exit(EXIT_FAILURE);
    });
    QObject::connect(&client, &CouchClient::responseReceived, &app, &QCoreApplication::quit, Qt::QueuedConnection);

    return app.exec();
}
