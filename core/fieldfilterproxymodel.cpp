#include "fieldfilterproxymodel.h"
#include "basetablemodel.h"
#include "qdatetime.h"
#include "commonfunc.h"

FieldFilterProxyModel::FieldFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_status(QVariant::fromValue(FilterStatus::UNKNOWN))
{}

void FieldFilterProxyModel::setFilterColumn(int column)
{
    m_filterColumn = column;
    invalidateFilter();
}

void FieldFilterProxyModel::addSearchFields(const QString &field)
{
    if (field.isEmpty() || searchFields.contains(field))
        return;

    searchFields.append(field);
    invalidateFilter();
}

void FieldFilterProxyModel::setKeyword(const QString &keyword)
{
    m_keyword = keyword;
    invalidateFilter();
}

void FieldFilterProxyModel::setStatus(QVariant status)
{
    m_status = status;
    invalidateFilter();
}

void FieldFilterProxyModel::setFieldFilter(const QString &field, const QVariant &value)
{
    if (!value.isValid() || value.isNull())
        m_fieldFilters.remove(field);
    else
        m_fieldFilters[field] = value;

    invalidateFilter();
}

QVariant FieldFilterProxyModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid())
    {
        return {};
    }

    auto model =
        qobject_cast<BaseTableModel*>(sourceModel());

    if (!model)
    {
        return QSortFilterProxyModel::data(index, role);
    }

    const auto& columns = model->columns();

    if (index.column() >= columns.size())
    {
        return {};
    }

    const auto& col = columns[index.column()];

    // ===== 序号列 =====

    if (col.type == ColumnType::RowNumber)
    {
        if (role == Qt::DisplayRole)
        {
            // ⭐ 使用proxy row
            return index.row() + 1;
        }
    }

    return QSortFilterProxyModel::data(index, role);
}

bool FieldFilterProxyModel::filterAcceptsRow(int srcRow, const QModelIndex &srcParent) const
{//获取基础模型
    auto model = qobject_cast<BaseTableModel*>(sourceModel());

    if(!model) return true;
//获取当前行数据
    QVariantMap row =
        model->rowData(srcRow);

    // ===== Tab状态过滤 =====
    QString status =  row.value("status").toString();

    FilterStatus filterStatus = FilterStatus::UNKNOWN;
    if (m_status.canConvert<FilterStatus>())
    {
        filterStatus = m_status.value<FilterStatus>();
    }
    else if (m_status.isValid())
    {
        bool ok = false;
        const int statusValue = m_status.toInt(&ok);
        if (ok)
            filterStatus = static_cast<FilterStatus>(statusValue);
    }
    switch (filterStatus)
    {
    case FilterStatus::UNKNOWN:
        break;

    case FilterStatus::WAITWORK:
        if (status != tr("未开工"))
            return false;
        break;

    case FilterStatus::ALREADY:
        if (status != tr("已开工"))
            return false;
        break;

    case FilterStatus::FINISH:
        if (status != tr("已完工"))
            return false;
        break;

    case FilterStatus::PENDING:
        if (status != tr("待处理"))
            return false;
        break;

    case FilterStatus::REVIEW:
        if (status != tr("审核中"))
            return false;
        break;

    case FilterStatus::WAITRETURN:
        if (status != tr("待返工"))
            return false;
        break;

    case FilterStatus::PROCESSED:
        if (status != tr("已处理"))
            return false;
        break;

    default:
        break;
    }


    // ===== 关键字过滤 =====
    if (!m_keyword.isEmpty() && !searchFields.isEmpty())
    {
        bool matched = false;

        for (const QString& field : searchFields)
        {
            QString value = row.value(field).toString();
//忽略大小写匹配
            if (value.contains(m_keyword, Qt::CaseInsensitive))
            {
                matched = true;
                break;
            }
        }
 // 所有字段均未匹配关键字，则隐藏该行
        if (!matched)
        {
            return false;
        }
    }

    //=========表头字段过滤====
    // m_fieldFilters 的结构：
    // {
    //     "startTime"  -> QDate(2026-05-13),
    //     "priority"   -> "高"
    // }
    for (auto it = m_fieldFilters.constBegin();
         it != m_fieldFilters.constEnd();
         ++it)
    {
        // 当前过滤字段，例如 "priority"
        QString field = it.key();

        // 当前过滤值，例如 "高" 或 QDate
        QVariant filterValue = it.value();

        // 当前行对应字段的值
        QVariant cellValue = row.value(field);

        // -------------------------
        // 日期类型过滤
        // -------------------------
        if (filterValue.type() == QVariant::Date)
        {
            // 用户选择的日期
            QDate selectedDate = filterValue.toDate();

            // 表格中的日期字符串，例如 "2026-05-13"
            QString dateText = cellValue.toString();

            // 根据你的日期格式解析
            QDate rowDate =
                QDate::fromString(dateText, "yyyy-MM-dd");

            // 日期不一致，则隐藏
            if (rowDate != selectedDate)
                return false;
        }
        else
        {
            // -------------------------
            // 普通字符串过滤
            // 例如 priority = "高"
            // -------------------------
            if (cellValue.toString() != filterValue.toString())
                return false;
        }
    }
    return true;
}

