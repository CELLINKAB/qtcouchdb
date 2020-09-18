#ifndef COUCHURL_P_H
#define COUCHURL_P_H

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>

static const QChar Slash = QLatin1Char('/');

class CouchUrl
{
public:
    static QUrl resolve(const QUrl &baseUrl, const QString& path, const QString &revision = QString())
    {
        QUrl url = baseUrl;
        url.setPath(join(baseUrl.path(), path));
        if (!revision.isNull())
            url.setQuery(QStringLiteral("rev=%1").arg(revision));
        return url;
    }

    static QString join(const QString &basePath, const QString &subPath)
    {
        if (basePath.endsWith(Slash) || subPath.startsWith(Slash))
            return basePath + subPath;
        return basePath + Slash + subPath;
    }

    static QString database(const QUrl &url)
    {
        return url.path().split(Slash, QString::SkipEmptyParts).value(0);
    }

    static QString documentId(const QUrl &url)
    {
        return url.path().split(Slash, QString::SkipEmptyParts).value(1);
    }
};

#endif // COUCHURL_P_H
