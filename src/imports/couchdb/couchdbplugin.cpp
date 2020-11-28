#include <QtCouchDB/couch.h>
#include <QtCouchDB/couchclient.h>
#include <QtCouchDB/couchdatabase.h>
#include <QtCouchDB/couchdesigndocument.h>
#include <QtCouchDB/couchdocument.h>
#include <QtCouchDB/coucherror.h>
#include <QtCouchDB/couchquery.h>
#include <QtCouchDB/couchrequest.h>
#include <QtCouchDB/couchresponse.h>
#include <QtCouchDB/couchview.h>
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
    qRegisterMetaType<CouchDocument>();
    qRegisterMetaType<CouchError>();
    qRegisterMetaType<CouchQuery>();
    qRegisterMetaType<CouchRequest>();

    qmlRegisterSingletonType<Couch>(uri, 1, 0, "Couch", [](QQmlEngine *engine, QJSEngine *) -> QObject * {
        return new Couch(engine);
    });
    qmlRegisterType<CouchClient>(uri, 1, 0, "CouchClient");
    qmlRegisterType<CouchDatabase>(uri, 1, 0, "CouchDatabase");
    qmlRegisterType<CouchDesignDocument>(uri, 1, 0, "CouchDesignDocument");
    qmlRegisterUncreatableType<CouchResponse>(uri, 1, 0, "CouchResponse", tr("Use CouchClient.sendRequest()"));
    qmlRegisterType<CouchView>(uri, 1, 0, "CouchView");
}

#include "couchdbplugin.moc"
