#include <QtCouchDB/couch.h>
#include <QtCouchDB/couchclient.h>
#include <QtQml/qqml.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlextensionplugin.h>

class CouchDBPlugin: public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override;
};

void CouchDBPlugin::registerTypes(const char *uri)
{
    qmlRegisterSingletonType<Couch>(uri, 1, 0, "Couch", [](QQmlEngine *engine, QJSEngine *) -> QObject * {
        return new Couch(engine);
    });
    qmlRegisterType<CouchClient>(uri, 1, 0, "CouchClient");
}

#include "couchdbplugin.moc"
