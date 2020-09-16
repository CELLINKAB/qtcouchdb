INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/couchdb.h \
    $$PWD/couchdb_p.h \
    $$PWD/couchdbglobal.h \
    $$PWD/couchdbresponse.h \
    $$PWD/couchdbquery.h \
    $$PWD/couchdblistener.h

SOURCES += \
    $$PWD/couchdb.cpp \
    $$PWD/couchdbresponse.cpp \
    $$PWD/couchdbquery.cpp \
    $$PWD/couchdblistener.cpp
