#include "jsondataloader.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

QVector<QVariantMap> JsonDataLoader::loadRows(const QString& fileName,
                                              const QString& arrayKey)
{
    QVector<QVariantMap> rows;
    const QString filePath = resolveEtcFile(fileName);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << __FUNCTION__ << "open failed:" << filePath << file.errorString();
        return rows;
    }

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << __FUNCTION__ << "parse failed:" << filePath << error.errorString();
        return rows;
    }

    QVariant data;
    if (arrayKey.isEmpty())
    {
        data = doc.toVariant();
    }
    else if (doc.isObject())
    {
        data = doc.object().toVariantMap().value(arrayKey);
    }

    const QVariantList list = data.toList();
    for (const QVariant& item : list)
    {
        const QVariantMap row = item.toMap();
        if (!row.isEmpty())
            rows.append(row);
    }

    qDebug() << __FUNCTION__ << "loaded:" << filePath << "rows:" << rows.size();
    return rows;
}

QVariantMap JsonDataLoader::loadObject(const QString& fileName)
{
    const QString filePath = resolveEtcFile(fileName);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << __FUNCTION__ << "open failed:" << filePath << file.errorString();
        return {};
    }

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
    {
        qDebug() << __FUNCTION__ << "parse failed:" << filePath << error.errorString();
        return {};
    }

    const QVariantMap object = doc.object().toVariantMap();
    qDebug() << __FUNCTION__ << "loaded:" << filePath << "keys:" << object.keys();
    return object;
}

QString JsonDataLoader::resolveEtcFile(const QString& fileName)
{
    const QString relativePath = QDir("etc").filePath(fileName);
    QStringList baseDirs;
    baseDirs << QDir::currentPath()
             << QCoreApplication::applicationDirPath();

    QDir currentDir(QDir::currentPath());
    for (int i = 0; i < 6; ++i)
    {
        baseDirs << currentDir.absolutePath();
        if (!currentDir.cdUp())
            break;
    }

    QDir appDir(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 6; ++i)
    {
        baseDirs << appDir.absolutePath();
        if (!appDir.cdUp())
            break;
    }

    baseDirs.removeDuplicates();
    for (const QString& baseDir : baseDirs)
    {
        const QString filePath = QDir(baseDir).filePath(relativePath);
        if (QFile::exists(filePath))
            return filePath;
    }

    const QString resourcePath = QStringLiteral(":/etc/%1").arg(fileName);
    if (QFile::exists(resourcePath))
        return resourcePath;

    qDebug() << __FUNCTION__ << "not found, fallback path:" << relativePath;
    return relativePath;
}
