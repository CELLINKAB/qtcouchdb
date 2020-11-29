#include <QtCore>
#include <QtCouchDB>
#include <cstdlib>
#include <iostream>

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

    CouchClient client(QUrl(cmdLine.value("url")));
    CouchDatabase database(cmdLine.value("database"), &client);
    CouchDesignDocument designDocument(cmdLine.value("design-document"), &database);
    CouchView view(cmdLine.value("view"), &designDocument);

    QString command = cmdLine.positionalArguments().value(0);
    if (command == "list-dbs" || command == "list-databases") {
        client.listDatabases();
    } else if (command == "list-designs" || command == "list-design-documents") {
        database.listDesignDocuments();
    } else if (command == "list-views") {
        designDocument.listFullViews();
    } else if (command == "list-rows") {
        view.listFullRows();
    } else {
        std::cerr << qPrintable(app.applicationName()) << ": Unknown command: '" << qPrintable(command) << "'." << std::endl;
        exit(EXIT_FAILURE);
    }

    QObject::connect(&client, &CouchClient::databasesListed, [&](const QStringList &databases) {
        std::cout << qPrintable(databases.join(" ")) << std::endl;
    });
    QObject::connect(&database, &CouchDatabase::designDocumentsListed, [&](const QStringList &designDocs) {
        std::cout << qPrintable(designDocs.join(" ")) << std::endl;
    });
    QObject::connect(&designDocument, &CouchDesignDocument::viewsListed, [&](const QStringList &views) {
        std::cout << qPrintable(views.join(" ")) << std::endl;
    });
    QObject::connect(&view, &CouchView::rowsListed, [&](const QJsonArray &rows) {
        std::cout << QJsonDocument(rows).toJson(QJsonDocument::Indented).constData();
    });

    QObject::connect(&client, &CouchClient::errorOccurred, [&](const CouchError &error) {
        std::cout << "Error: " << qPrintable(error.reason()) << std::endl;
        exit(EXIT_FAILURE);
    });
    QObject::connect(&client, &CouchClient::responseReceived, &app, &QCoreApplication::quit, Qt::QueuedConnection);

    return app.exec();
}
