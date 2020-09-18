#ifndef COUCH_H
#define COUCH_H

#include <QtCouchDB/couchglobal.h>
#include <QtCore/qobject.h>

class COUCHDB_EXPORT Couch : public QObject
{
    Q_OBJECT

public:
    explicit Couch(QObject *parent = nullptr);
};

#endif // COUCH_H
