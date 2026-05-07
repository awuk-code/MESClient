#include "basetablemodel.h"

BaseTableModel::BaseTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{}

void BaseTableModel::setRows(const QVector<QVariantMap> &rows)
{
    beginResetModel();
    m_rows = rows;
    endResetModel();
}

const QVector<ColumnConfig>& BaseTableModel::columns() const
{
    return m_columns;
}

int BaseTableModel::rowCount(const QModelIndex &) const
{
    return m_rows.size();
}

int BaseTableModel::columnCount(const QModelIndex &) const
{
    return m_columns.size();
}

QVariant BaseTableModel::data(
    const QModelIndex &index,
    int role) const
{
    if (!index.isValid())
        return {};
    if (index.row() >= m_rows.size())
        return {};

    if (index.column() >= m_columns.size())
        return {};
    const auto& row = m_rows[index.row()];
    // const auto& col = m_columns[index.column()];
    const auto& col = m_columns[index.column()];

    //checkbox
    if (role == Qt::CheckStateRole)
    {
        if (col.type == ColumnType::CheckBox)
        {
            return m_checkedRows.contains(index.row())
            ? Qt::Checked
            : Qt::Unchecked;
        }
    }
    // 显示数据
    if (role == Qt::DisplayRole)
    {
        // 序号列
        if (col.type == ColumnType::RowNumber)
        {
            return index.row() + 1;
        }

        return row.value(col.field);
    }

    // 对齐
    if (role == Qt::TextAlignmentRole)
    {
        // return col.alignment;  //qt6
        return QVariant::fromValue(col.alignment);
    }

    return {};
}

QVariant BaseTableModel::headerData(
    int section,
    Qt::Orientation orientation,
    int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
    {
        if (section < 0 || section >= m_columns.size())
            return {};
        return m_columns[section].title;

    }

    return {};
}

Qt::ItemFlags BaseTableModel::flags(
    const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    const auto& col = m_columns[index.column()];

    if (col.type == ColumnType::CheckBox)
    {
        flags |= Qt::ItemIsUserCheckable;
    }

    return flags;
}

bool BaseTableModel::setData(
    const QModelIndex &index,
    const QVariant &value,
    int role)
{
    if (!index.isValid())
        return false;

    const auto& col = m_columns[index.column()];

    if (col.type == ColumnType::CheckBox &&
        role == Qt::CheckStateRole)
    {
        if (value == Qt::Checked)
        {
            m_checkedRows.insert(index.row());
        }
        else
        {
            m_checkedRows.remove(index.row());
        }

        emit dataChanged(index, index);

        return true;
    }

    return false;
}
