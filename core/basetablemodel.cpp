#include "basetablemodel.h"

BaseTableModel::BaseTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{}

QVariantMap BaseTableModel::rowData(int row) const
{
    if (row < 0 || row >= m_rows.size())
    {
        return {};
    }

    return m_rows[row];
}

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

    const auto& col = m_columns[index.column()];

    // ===== 对齐 =====
    if (role == Qt::TextAlignmentRole)
    {
        return QVariant::fromValue(col.alignment);
    }

    // ===== checkbox =====
    if (col.type == ColumnType::CheckBox)
    {
        if (role == Qt::CheckStateRole)
        {
            bool checked =
                m_checkedRows.contains(index.row());

            return checked
                       ? Qt::Checked
                       : Qt::Unchecked;
        }

        return {};
    }

    // ===== 显示数据 =====
    if (role == Qt::DisplayRole)
    {
        // ===== 普通数据 =====

        const auto& row = m_rows[index.row()];

        return row.value(col.field);
    }
/////////////////////////////////
    if(role == Qt::UserRole)
        return m_rows[index.row()];


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
    if(role == Qt::UserRole){
        m_rows[index.row()] = value.toMap();
        emit dataChanged(index, index);
        return true;
    }

    return false;
}
