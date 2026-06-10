#ifndef SEARCHCONFIG_H
#define SEARCHCONFIG_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QJsonObject>

class FieldFilterProxyModel;

struct SearchFieldItem
{
    QString title;
    QString field;
};

class SearchConfig
{
public:
    static QString productionTaskModule();
    static QString processStationModule();
    static QString repairStationModule();

    static QVector<SearchFieldItem> availableFields(const QString& module);
    static QStringList selectedFields(const QString& module);
    static void setSelectedFields(const QString& module, const QStringList& fields);
    static QString placeholder(const QString& module);
    static void apply(FieldFilterProxyModel* proxy, const QString& module);

private:
    static QString configPath();
    static void ensureConfigFile();
    static QJsonObject loadRoot();
    static void saveRoot(const QJsonObject& root);
    static QJsonObject defaultRoot();
    static QStringList defaultSelectedFields(const QString& module);
};

#endif // SEARCHCONFIG_H
