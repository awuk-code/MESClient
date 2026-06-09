#include "matadapter.h"

#include <QHash>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringList>

namespace
{
struct AdapterOpt
{
    QString groupField = QStringLiteral("productSN");
    QString itemsField = QStringLiteral("materials");
    QString rowNoField = QStringLiteral("rowNumber");

    QString metaGroupKey = QStringLiteral("_groupKey");
    QString metaGroupIndex = QStringLiteral("_groupIndex");
    QString metaGroupFirst = QStringLiteral("_groupFirst");
    QString metaGroupCount = QStringLiteral("_groupRowCount");
    QString metaItemIndex = QStringLiteral("_itemIndex");

    QStringList parentFields;
    QStringList itemFields;
};

QVector<QVariantMap> mapsFromJson(const QJsonArray& array)
{
    QVector<QVariantMap> rows;
    rows.reserve(array.size());

    for (const QJsonValue& value : array)
    {
        if (!value.isObject())
            continue;

        rows.append(value.toObject().toVariantMap());
    }

    return rows;
}

bool isMetaKey(const QString& key, const AdapterOpt& opt)
{
    return key == opt.metaGroupKey ||
           key == opt.metaGroupIndex ||
           key == opt.metaGroupFirst ||
           key == opt.metaGroupCount ||
           key == opt.metaItemIndex;
}

QString safeGroupKey(const QVariantMap& row, const QString& fallback, const AdapterOpt& opt)
{
    const QString key = row.value(opt.groupField).toString().trimmed();
    return key.isEmpty() ? fallback : key;
}
}

struct MatAdapter::Opt : public AdapterOpt
{
};

bool MatAdapter::isCompound(const QJsonArray& data)
{
    return isCompound(mapsFromJson(data));
}

bool MatAdapter::isCompound(const QVector<QVariantMap>& data)
{
    const Opt option = opt();
    for (const QVariantMap& row : data)
    {
        if (hasItems(row, option))
            return true;
    }

    return false;
}

QVector<QVariantMap> MatAdapter::split(const QJsonArray& compoundData)
{
    return split(mapsFromJson(compoundData));
}

QVector<QVariantMap> MatAdapter::split(const QVector<QVariantMap>& compoundData)
{
    const Opt option = opt();
    if (isCompound(compoundData))
        return expandNestedRows(compoundData, option);

    return expandFlatRows(compoundData, option);
}

QJsonArray MatAdapter::combine(const QVector<QVariantMap>& splitRows)
{
    const Opt option = opt();
    QJsonArray result;
    QVector<QString> groupKeys;
    QHash<QString, QVector<QVariantMap>> groups;

    for (int rowIndex = 0; rowIndex < splitRows.size(); ++rowIndex)
    {
        const QVariantMap& row = splitRows[rowIndex];
        const QString fallback = QStringLiteral("__row_%1").arg(rowIndex);
        const QString groupKey = row.value(option.metaGroupKey).toString().trimmed().isEmpty()
            ? safeGroupKey(row, fallback, option)
            : row.value(option.metaGroupKey).toString().trimmed();

        if (!groups.contains(groupKey))
            groupKeys.append(groupKey);

        groups[groupKey].append(row);
    }

    for (const QString& groupKey : groupKeys)
    {
        const QVector<QVariantMap> groupRows = groups.value(groupKey);
        if (groupRows.isEmpty())
            continue;

        QVariantMap parent;
        const QVariantMap first = stripMeta(groupRows.first(), option);

        if (option.parentFields.isEmpty())
        {
            parent.insert(option.groupField, first.value(option.groupField));
        }
        else
        {
            for (const QString& field : option.parentFields)
                parent.insert(field, first.value(field));
        }

        QVariantList items;
        for (const QVariantMap& rawRow : groupRows)
        {
            const QVariantMap row = stripMeta(rawRow, option);
            QVariantMap item;

            if (option.itemFields.isEmpty())
            {
                for (auto it = row.constBegin(); it != row.constEnd(); ++it)
                {
                    const QString key = it.key();
                    if (key == option.groupField ||
                        key == option.rowNoField ||
                        key == option.itemsField)
                        continue;

                    item.insert(key, it.value());
                }
            }
            else
            {
                for (const QString& field : option.itemFields)
                    item.insert(field, row.value(field));
            }

            items.append(item);
        }

        parent.insert(option.itemsField, items);
        result.append(QJsonObject::fromVariantMap(parent));
    }

    return result;
}

QJsonArray MatAdapter::combine(const QJsonArray& originalData,
                               const QVector<QVariantMap>& splitRows)
{
    const Opt option = opt();
    if (!isCompound(originalData))
        return combine(splitRows);

    QVector<QString> groupKeys;
    QHash<QString, QVector<QVariantMap>> groups;
    for (int rowIndex = 0; rowIndex < splitRows.size(); ++rowIndex)
    {
        const QVariantMap& row = splitRows[rowIndex];
        const QString fallback = QStringLiteral("__row_%1").arg(rowIndex);
        const QString groupKey = row.value(option.metaGroupKey).toString().trimmed().isEmpty()
            ? safeGroupKey(row, fallback, option)
            : row.value(option.metaGroupKey).toString().trimmed();

        if (!groups.contains(groupKey))
            groupKeys.append(groupKey);

        groups[groupKey].append(row);
    }

    QJsonArray result;
    const QVector<QVariantMap> originalRows = mapsFromJson(originalData);
    for (int originalIndex = 0; originalIndex < originalRows.size(); ++originalIndex)
    {
        QVariantMap parent = originalRows[originalIndex];
        const QString fallback = QStringLiteral("__group_%1").arg(originalIndex);
        const QString groupKey = safeGroupKey(parent, fallback, option);
        const QVector<QVariantMap> editedRows = groups.value(groupKey);

        if (editedRows.isEmpty())
        {
            result.append(QJsonObject::fromVariantMap(parent));
            continue;
        }

        const QVariantMap firstRow = stripMeta(editedRows.first(), option);
        for (auto it = firstRow.constBegin(); it != firstRow.constEnd(); ++it)
        {
            const QString key = it.key();
            if (key != option.itemsField &&
                key != option.rowNoField &&
                parent.contains(key))
                parent.insert(key, it.value());
        }

        QVariantList items = parent.value(option.itemsField).toList();
        for (int itemIndex = 0; itemIndex < editedRows.size(); ++itemIndex)
        {
            QVariantMap item = itemIndex < items.size()
                ? items[itemIndex].toMap()
                : QVariantMap();

            const QVariantMap editedItem = itemFromRow(editedRows[itemIndex], option);
            for (auto it = editedItem.constBegin(); it != editedItem.constEnd(); ++it)
            {
                if (!parent.contains(it.key()))
                    item.insert(it.key(), it.value());
            }

            if (itemIndex < items.size())
                items[itemIndex] = item;
            else
                items.append(item);
        }

        parent.insert(option.itemsField, items);
        result.append(QJsonObject::fromVariantMap(parent));
    }

    for (const QString& groupKey : groupKeys)
    {
        bool existsInOriginal = false;
        for (int originalIndex = 0; originalIndex < originalRows.size(); ++originalIndex)
        {
            const QString fallback = QStringLiteral("__group_%1").arg(originalIndex);
            if (safeGroupKey(originalRows[originalIndex], fallback, option) == groupKey)
            {
                existsInOriginal = true;
                break;
            }
        }

        if (!existsInOriginal)
        {
            const QJsonArray extraGroups = combine(groups.value(groupKey));
            for (const QJsonValue& value : extraGroups)
                result.append(value);
        }
    }

    return result;
}

MatAdapter::Opt MatAdapter::opt()
{
    return Opt();
}

bool MatAdapter::hasItems(const QVariantMap& row, const Opt& opt)
{
    return row.contains(opt.itemsField) && row.value(opt.itemsField).canConvert<QVariantList>();
}

QVector<QVariantMap> MatAdapter::expandNestedRows(const QVector<QVariantMap>& backendRows,
                                                  const Opt& opt)
{
    QVector<QVariantMap> rows;
    int groupIndex = 1;

    for (int i = 0; i < backendRows.size(); ++i)
    {
        QVariantMap parent = backendRows[i];
        const QVariantList items = parent.take(opt.itemsField).toList();
        const QString groupKey = safeGroupKey(parent, QStringLiteral("__group_%1").arg(i), opt);

        QVector<QVariantMap> groupRows;
        if (items.isEmpty())
        {
            groupRows.append(parent);
        }
        else
        {
            groupRows.reserve(items.size());
            for (const QVariant& itemValue : items)
            {
                QVariantMap row = parent;
                const QVariantMap item = itemValue.toMap();
                for (auto it = item.constBegin(); it != item.constEnd(); ++it)
                    row.insert(it.key(), it.value());

                row.insert(opt.groupField, groupKey);
                groupRows.append(row);
            }
        }

        appendGroup(&rows, groupRows, groupKey, groupIndex, opt);
        ++groupIndex;
    }

    return rows;
}

QVector<QVariantMap> MatAdapter::expandFlatRows(const QVector<QVariantMap>& backendRows,
                                                const Opt& opt)
{
    QVector<QVariantMap> rows;
    QVector<QString> groupKeys;
    QHash<QString, QVector<QVariantMap>> groups;

    for (int i = 0; i < backendRows.size(); ++i)
    {
        const QVariantMap row = backendRows[i];
        const QString groupKey = safeGroupKey(row, QStringLiteral("__row_%1").arg(i), opt);
        if (!groups.contains(groupKey))
            groupKeys.append(groupKey);

        groups[groupKey].append(row);
    }

    int groupIndex = 1;
    for (const QString& groupKey : groupKeys)
    {
        appendGroup(&rows, groups.value(groupKey), groupKey, groupIndex, opt);
        ++groupIndex;
    }

    return rows;
}

void MatAdapter::appendGroup(QVector<QVariantMap>* out,
                             const QVector<QVariantMap>& groupRows,
                             const QString& groupKey,
                             int groupIndex,
                             const Opt& opt)
{
    if (!out || groupRows.isEmpty())
        return;

    const int groupCount = groupRows.size();
    for (int i = 0; i < groupRows.size(); ++i)
    {
        QVariantMap row = groupRows[i];
        row.insert(opt.groupField, groupKey);
        row.insert(opt.rowNoField, groupIndex);
        row.insert(opt.metaGroupKey, groupKey);
        row.insert(opt.metaGroupIndex, groupIndex);
        row.insert(opt.metaGroupFirst, i == 0);
        row.insert(opt.metaGroupCount, groupCount);
        row.insert(opt.metaItemIndex, i + 1);
        out->append(row);
    }
}

QVariantMap MatAdapter::itemFromRow(const QVariantMap& row, const Opt& opt)
{
    QVariantMap item;
    const QVariantMap cleanRow = stripMeta(row, opt);
    for (auto it = cleanRow.constBegin(); it != cleanRow.constEnd(); ++it)
    {
        const QString key = it.key();
        if (key == opt.groupField ||
            key == opt.rowNoField ||
            key == opt.itemsField)
            continue;

        item.insert(key, it.value());
    }

    return item;
}

QVariantMap MatAdapter::stripMeta(const QVariantMap& row, const Opt& opt)
{
    QVariantMap result;
    for (auto it = row.constBegin(); it != row.constEnd(); ++it)
    {
        if (!isMetaKey(it.key(), opt))
            result.insert(it.key(), it.value());
    }

    return result;
}
