TEMPLATE = app
QT += couchdb

SOURCES += \
    cli.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/cli
INSTALLS += target
