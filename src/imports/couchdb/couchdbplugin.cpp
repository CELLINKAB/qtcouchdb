#include <QtCouchDB/couchdb.h>
#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqml.h>

class CouchDBPlugin: public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override;
};

void CouchDBPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<CouchDB>(uri, 1, 0, "CouchDB");
}

#include "couchdbplugin.moc"
