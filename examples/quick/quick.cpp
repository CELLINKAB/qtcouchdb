#include <QtQuickControls2>

class MaterialIcons : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fontFamily MEMBER fontFamily CONSTANT)
    Q_PROPERTY(QString add MEMBER add CONSTANT)
    Q_PROPERTY(QString arrow_back MEMBER arrow_back CONSTANT)
    Q_PROPERTY(QString close MEMBER close CONSTANT)
    Q_PROPERTY(QString error MEMBER error CONSTANT)
    Q_PROPERTY(QString menu MEMBER menu CONSTANT)

public:
    explicit MaterialIcons(QObject *parent = nullptr) : QObject(parent) { }

    static inline const QString fontFamily = "MaterialIcons";
    static inline const QString add = "\ue145";
    static inline const QString arrow_back = "\ue5c4";
    static inline const QString close = "\ue5cd";
    static inline const QString error = "\ue000";
    static inline const QString menu = "\ue5d2";
};

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("CouchDB");
    QCoreApplication::setOrganizationName("Apache");
    QCoreApplication::setOrganizationDomain("couchdb.apache.org");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QLocale locale = QLocale::system();
    locale.setNumberOptions(QLocale::OmitGroupSeparator);
    QLocale::setDefault(locale);

    QQuickStyle::setStyle("Material");
    QFontDatabase::addApplicationFont(":/fonts/MaterialIcons-Regular.ttf");

    qmlRegisterSingletonType<MaterialIcons>("MaterialIcons", 1, 0, "MaterialIcons", [](QQmlEngine *engine, QJSEngine *) -> QObject * {
        return new MaterialIcons(engine);
    });

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/quick.qml"));

    return app.exec();
}

#include "quick.moc"
