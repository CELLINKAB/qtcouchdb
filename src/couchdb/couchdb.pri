INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/couchdb.h \
    $$PWD/couchdbenums.h \
    $$PWD/couchdbglobal.h \
    $$PWD/couchdbserver.h \
    $$PWD/couchdbresponse.h \
    $$PWD/couchdbquery.h \
    $$PWD/couchdblistener.h

SOURCES += \
    $$PWD/couchdb.cpp \
    $$PWD/couchdbserver.cpp \
    $$PWD/couchdbresponse.cpp \
    $$PWD/couchdbquery.cpp \
    $$PWD/couchdblistener.cpp
