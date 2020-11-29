#include <QtCore>
#include <QtCouchDB>
#include <cstdlib>
#include <iostream>

static bool parseOptions(QCommandLineParser &cmdLine)
{
    cmdLine.addHelpOption();
    cmdLine.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    cmdLine.addOptions({
        {{"u", "url"}, "CouchDB URL (default: http://localhost:5984)", "url", "http://localhost:5984"},
        {{"db", "database"}, "CouchDB Database", "database", ""},
        {{"dd", "design", "design-document"}, "CouchDB Design Document", "design-document", ""},
        {{"v", "view"}, "CouchDB View", "view", ""},
    });
    cmdLine.addPositionalArgument("command", "CouchDB command (available: list-dbs, list-designs, list-views, list-rows)", "command");
    if (!cmdLine.parse(QCoreApplication::arguments()))
        return false;
    if (cmdLine.isSet("help"))
        cmdLine.showHelp(EXIT_SUCCESS);
    return true;
}

static CouchClient *createClient(const QUrl &url)
{
    CouchClient *client = new CouchClient(url, qApp);
    QObject::connect(client, &CouchClient::databasesListed, [](const QStringList &databases) {
        std::cout << qPrintable(databases.join(" ")) << std::endl;
    });
    QObject::connect(client, &CouchClient::errorOccurred, [](const CouchError &error) {
        std::cout << "Error: " << qPrintable(error.reason()) << std::endl;
        exit(EXIT_FAILURE);
    });
    return client;
}

static CouchDatabase *createDatabase(const QString &name, CouchClient *client)
{
    CouchDatabase *database = new CouchDatabase(name, client);
    QObject::connect(database, &CouchDatabase::designDocumentsListed, [](const QStringList &designDocs) {
        std::cout << qPrintable(designDocs.join(" ")) << std::endl;
    });
    return database;
}

static CouchDesignDocument *createDesignDocument(const QString &name, CouchDatabase *database)
{
    CouchDesignDocument *designDocument = new CouchDesignDocument(name, database);
    QObject::connect(designDocument, &CouchDesignDocument::viewsListed, [](const QStringList &views) {
        std::cout << qPrintable(views.join(" ")) << std::endl;
    });
    return designDocument;
}

static CouchView *createView(const QString &name, CouchDesignDocument *designDocument)
{
    CouchView *view = new CouchView(name, designDocument);
    QObject::connect(view, &CouchView::rowsListed, [](const QJsonArray &rows) {
        std::cout << QJsonDocument(rows).toJson(QJsonDocument::Indented).constData();
    });
    return view;
}

static int runApp(const QCommandLineParser &cmdLine)
{
    QScopedPointer<CouchClient> client(createClient(QUrl(cmdLine.value("url"))));
    QScopedPointer<CouchDatabase> database(createDatabase(cmdLine.value("database"), client.data()));
    QScopedPointer<CouchDesignDocument> designDocument(createDesignDocument(cmdLine.value("design-document"), database.data()));
    QScopedPointer<CouchView> view(createView(cmdLine.value("view"), designDocument.data()));

    typedef std::function<void()> Command;
    const QHash<QString, Command> commands = {
        { "list-dbs", [&]() { client->listDatabases(); } },
        { "list-design-documents", [&]() { database->listDesignDocuments(); } },
        { "list-views", [&]() { designDocument->listFullViews(); } },
        { "list-rows", [&]() { view->listFullRows(); } },
    };

    QString arg = cmdLine.positionalArguments().value(0);
    Command command = commands.value(arg, [=]() {
        std::cerr << qPrintable(QCoreApplication::applicationName()) << ": Unknown command: '" << qPrintable(arg) << "'." << std::endl;
    });
    if (!command)
        return EXIT_FAILURE;
    command();

    QObject::connect(client.data(), &CouchClient::responseReceived, qApp, &QCoreApplication::quit, Qt::QueuedConnection);

    return QCoreApplication::exec();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser cmdLine;
    if (!parseOptions(cmdLine))
        return EXIT_FAILURE;
    return runApp(cmdLine);
}
