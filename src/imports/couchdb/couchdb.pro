TARGET = couchdbplugin
TARGETPATH = CouchDB
IMPORT_VERSION = 1.0

QT += qml couchdb

DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

OTHER_FILES += \
    qmldir

SOURCES += \
    $$PWD/couchdbplugin.cpp

CONFIG += no_cxx_module
load(qml_plugin)
