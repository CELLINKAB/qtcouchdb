INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/couch.h \
    $$PWD/couchclient.h \
    $$PWD/couchclient_p.h \
    $$PWD/couchdbglobal.h \
    $$PWD/couchrequest.h \
    $$PWD/couchresponse.h \
    $$PWD/couchurl_p.h

SOURCES += \
    $$PWD/couch.cpp \
    $$PWD/couchclient.cpp \
    $$PWD/couchrequest.cpp \
    $$PWD/couchresponse.cpp
