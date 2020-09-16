#ifndef COUCHURL_P_H
#define COUCHURL_P_H

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>

class CouchUrl
{
public:
    static QUrl resolve(const QUrl &baseUrl, const QString& path, const QString &revision = QString())
    {
        QUrl url = baseUrl.resolved(path);
        if (!revision.isNull())
            url.setQuery(QString("rev=%1").arg(revision));
        return url;
    }

    static QString database(const QUrl &url)
    {
        return url.path().split("/", QString::SkipEmptyParts).value(0);
    }

    static QString documentId(const QUrl &url)
    {
        return url.path().split("/", QString::SkipEmptyParts).value(1);
    }
};

#endif // COUCHURL_P_H
