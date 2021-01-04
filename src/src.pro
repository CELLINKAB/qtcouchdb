TEMPLATE = subdirs

SUBDIRS += \
    couchdb

qtHaveModule(qml) {
    SUBDIRS += \
        imports

    imports.depends += \
        couchdb
}
