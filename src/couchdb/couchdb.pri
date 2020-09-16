INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/couchclient.h \
    $$PWD/couchclient_p.h \
    $$PWD/couchdbglobal.h \
    $$PWD/couchdblistener.h \
    $$PWD/couchquery.h \
    $$PWD/couchresponse.h

SOURCES += \
    $$PWD/couchclient.cpp \
    $$PWD/couchdblistener.cpp \
    $$PWD/couchquery.cpp \
    $$PWD/couchresponse.cpp
