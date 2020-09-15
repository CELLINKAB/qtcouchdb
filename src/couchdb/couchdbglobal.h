#ifndef COUCHDBGLOBAL_H
#define COUCHDBGLOBAL_H

#include <QtCore/qglobal.h>

#ifndef COUCHDB_EXPORT
#  ifndef QT_STATIC
#    if defined(QT_BUILD_COUCHDB_LIB)
#      define COUCHDB_EXPORT Q_DECL_EXPORT
#    else
#      define COUCHDB_EXPORT Q_DECL_IMPORT
#    endif
#  else
#    define COUCHDB_EXPORT
#  endif
#endif

#endif // COUCHDBGLOBAL_H
