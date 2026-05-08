#include "fieldfilterproxymodel.h"
#include "basetablemodel.h"


FieldFilterProxyModel::FieldFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{}

void FieldFilterProxyModel::setFilterColumn(int column)
{
    m_filterColumn = column;
    invalidateFilter();
}

void FieldFilterProxyModel::setKeyword(const QString &keyword)
{
    m_keyword = keyword;
    invalidateFilter();
}

void FieldFilterProxyModel::setStatus(int status)
{
    m_status = status;
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
{
    auto model = qobject_cast<BaseTableModel*>(sourceModel());

    if(!model) return true;

    QVariantMap row =
        model->rowData(srcRow);

    QString status =
        row.value("status").toString();


    // ===== Tab状态过滤 =====
    switch (m_status)
    {
    case -1:   return false;
        break;
    case 0:
        if (status != "未开工")
            return false;
        break;

    case 1:
        if (status != "已开工")
            return false;
        break;

    case 2:
        if (status != "已完工")
            return false;
        break;
    }


    // ===== 关键字过滤 =====
    if (!m_keyword.isEmpty())
    {
        QString text = row.value("taskNo").toString();
        if(!text.contains(m_keyword, Qt::CaseInsensitive)){
            return false;
        }
    }

    return true;
}
