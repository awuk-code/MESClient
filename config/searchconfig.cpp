#include "searchconfig.h"

#include "fieldfilterproxymodel.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QStringList>

namespace
{
QStringList fieldsFromJson(const QJsonArray& array)
{
    QStringList fields;
    for (const QJsonValue& value : array)
    {
        const QString field = value.toString().trimmed();
        if (!field.isEmpty() && !fields.contains(field))
            fields.append(field);
    }
    return fields;
}

QJsonArray fieldsToJson(const QStringList& fields)
{
    QJsonArray array;
    for (const QString& field : fields)
    {
        const QString cleanField = field.trimmed();
        if (!cleanField.isEmpty())
            array.append(cleanField);
    }
    return array;
}

QStringList allowedFieldNames(const QVector<SearchFieldItem>& items)
{
    QStringList fields;
    for (const SearchFieldItem& item : items)
        fields.append(item.field);
    return fields;
}

QStringList onlyAllowedFields(const QStringList& fields, const QString& module)
{
    const QStringList allowed = allowedFieldNames(SearchConfig::availableFields(module));
    QStringList result;
    for (const QString& field : fields)
    {
        if (allowed.contains(field) && !result.contains(field))
            result.append(field);
    }
    return result;
}
}

QString SearchConfig::productionTaskModule()
{
    return QStringLiteral("productionTask");
}

QString SearchConfig::processStationModule()
{
    return QStringLiteral("processStation");
}

QString SearchConfig::repairStationModule()
{
    return QStringLiteral("repairStation");
}

QVector<SearchFieldItem> SearchConfig::availableFields(const QString& module)
{
    if (module == productionTaskModule())
    {
        return {
            {QObject::tr("生产任务单号"), QStringLiteral("taskNo")},
            {QObject::tr("产品型号"), QStringLiteral("productModel")},
            {QObject::tr("产品ERP编码"), QStringLiteral("erpCode")},
            {QObject::tr("产品名称"), QStringLiteral("productName")},
            {QObject::tr("产品类型"), QStringLiteral("productType")},
            {QObject::tr("生产线编号"), QStringLiteral("lineNo")},
            {QObject::tr("生产工艺路线"), QStringLiteral("routeName")},
            {QObject::tr("info任务单号"), QStringLiteral("infoTaskNo")}
        };
    }

    if (module == processStationModule())
    {
        return {
            {QObject::tr("物料编码"), QStringLiteral("materialCode")},
            {QObject::tr("物料名称"), QStringLiteral("materialName")},
            {QObject::tr("物料标签码"), QStringLiteral("materialLabelCode")},
            {QObject::tr("EPR编码"), QStringLiteral("EPR")},
            {QObject::tr("批次号"), QStringLiteral("batchNo")},
            {QObject::tr("工序编号"), QStringLiteral("processCode")},
            {QObject::tr("工序名称"), QStringLiteral("processName")}
        };
    }

    if (module == repairStationModule())
    {
        return {
            {QObject::tr("异常处理单号"), QStringLiteral("exceptionHandleNo")},
            {QObject::tr("生产任务单号"), QStringLiteral("taskNo")},
            {QObject::tr("返工任务单号"), QStringLiteral("reworkTaskNo")},
            {QObject::tr("产品型号"), QStringLiteral("productModel")},
            {QObject::tr("产品SN"), QStringLiteral("productSN")},
            {QObject::tr("异常类型"), QStringLiteral("abnormalType")},
            {QObject::tr("异常现象"), QStringLiteral("abnormalPhenomenon")},
            {QObject::tr("异常上报工序"), QStringLiteral("reportProcess")},
            {QObject::tr("产线编码"), QStringLiteral("lineNo")},
            {QObject::tr("工作站编号"), QStringLiteral("stationNo")},
            {QObject::tr("上报人员"), QStringLiteral("reporter")},
            {QObject::tr("异常处理方式"), QStringLiteral("handleMethod")}
        };
    }

    return {};
}

QStringList SearchConfig::selectedFields(const QString& module)
{
    ensureConfigFile();

    const QJsonObject root = loadRoot();
    const QJsonObject moduleObj = root.value(module).toObject();
    if (!moduleObj.contains(QStringLiteral("fields")))
        return defaultSelectedFields(module);

    const QStringList fields = fieldsFromJson(moduleObj.value(QStringLiteral("fields")).toArray());
    return onlyAllowedFields(fields, module);
}

void SearchConfig::setSelectedFields(const QString& module, const QStringList& fields)
{
    ensureConfigFile();

    QJsonObject root = loadRoot();
    QJsonObject moduleObj = root.value(module).toObject();
    moduleObj.insert(QStringLiteral("fields"), fieldsToJson(onlyAllowedFields(fields, module)));
    moduleObj.insert(QStringLiteral("placeholder"), placeholder(module));
    root.insert(module, moduleObj);
    saveRoot(root);
}

QString SearchConfig::placeholder(const QString& module)
{
    if (module == productionTaskModule())
        return QObject::tr("请输入任务单号、产品型号、ERP编码");

    if (module == processStationModule())
        return QObject::tr("请输入物料信息或工序信息");

    if (module == repairStationModule())
        return QObject::tr("请输入异常单号、任务单号、产品SN");

    return QObject::tr("请输入搜索内容");
}

void SearchConfig::apply(FieldFilterProxyModel* proxy, const QString& module)
{
    if (!proxy)
        return;

    const QStringList fields = selectedFields(module);
    for (const QString& field : fields)
        proxy->addSearchFields(field);
}

QString SearchConfig::configPath()
{
    QDir appDir(QCoreApplication::applicationDirPath());
    return appDir.filePath(QStringLiteral("config/search_fields.json"));
}

void SearchConfig::ensureConfigFile()
{
    QFileInfo fileInfo(configPath());
    QDir dir = fileInfo.dir();
    if (!dir.exists())
        dir.mkpath(QStringLiteral("."));

    if (fileInfo.exists())
        return;

    saveRoot(defaultRoot());
}

QJsonObject SearchConfig::loadRoot()
{
    QFile file(configPath());
    if (!file.open(QIODevice::ReadOnly))
        return defaultRoot();

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    return doc.isObject() ? doc.object() : defaultRoot();
}

void SearchConfig::saveRoot(const QJsonObject& root)
{
    QFile file(configPath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}

QJsonObject SearchConfig::defaultRoot()
{
    QJsonObject root;
    for (const QString& module : {
             productionTaskModule(),
             processStationModule(),
             repairStationModule()
         })
    {
        QJsonObject moduleObj;
        moduleObj.insert(QStringLiteral("placeholder"), placeholder(module));
        moduleObj.insert(QStringLiteral("fields"), fieldsToJson(defaultSelectedFields(module)));
        root.insert(module, moduleObj);
    }
    return root;
}

QStringList SearchConfig::defaultSelectedFields(const QString& module)
{
    if (module == productionTaskModule())
        return {QStringLiteral("taskNo"), QStringLiteral("productModel"),
                QStringLiteral("erpCode"), QStringLiteral("productName")};

    if (module == processStationModule())
        return {QStringLiteral("materialCode"), QStringLiteral("materialName"),
                QStringLiteral("processCode"), QStringLiteral("processName")};

    if (module == repairStationModule())
        return {QStringLiteral("exceptionHandleNo"), QStringLiteral("taskNo"),
                QStringLiteral("reworkTaskNo"), QStringLiteral("productModel"),
                QStringLiteral("productSN")};

    return {};
}
