TARGET = QtCouchDB
MODULE = couchdb

QT = core network
QT += qml ### TODO

include(couchdb.pri)

load(qt_module)

### TODO:
DEFINES -= QT_NO_CAST_TO_ASCII QT_ASCII_CAST_WARNINGS
