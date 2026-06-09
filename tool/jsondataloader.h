#ifndef JSONDATALOADER_H
#define JSONDATALOADER_H

#include <QString>
#include <QVariantMap>
#include <QVector>

class JsonDataLoader
{
public:
    static QVector<QVariantMap> loadRows(const QString& fileName,
                                         const QString& arrayKey = QString());
    static QVariantMap loadObject(const QString& fileName);

private:
    static QString resolveEtcFile(const QString& fileName);
};

#endif // JSONDATALOADER_H
