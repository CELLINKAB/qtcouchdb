INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/couch.h \
    $$PWD/couchclient.h \
    $$PWD/couchclient_p.h \
    $$PWD/couchdbglobal.h \
    $$PWD/couchdblistener.h \
    $$PWD/couchquery.h \
    $$PWD/couchurl_p.h \
    $$PWD/couchresponse.h

SOURCES += \
    $$PWD/couch.cpp \
    $$PWD/couchclient.cpp \
    $$PWD/couchdblistener.cpp \
    $$PWD/couchquery.cpp \
    $$PWD/couchresponse.cpp
