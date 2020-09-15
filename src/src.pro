TEMPLATE = subdirs

SUBDIRS += \
    couchdb \
    imports

imports.depends += \
    couchdb
