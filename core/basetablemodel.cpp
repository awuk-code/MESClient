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

void BaseTableModel::afterCellEdited(int row, const QString &field, const QVariant &value)
{
    Q_UNUSED(row)
    Q_UNUSED(field)
    Q_UNUSED(value)

    // 默认不做任何事情。
    // 子类（如 ProcessStationModel）可按需重写。
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
        return int(Qt::AlignCenter);
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

        const auto& column = m_columns[section];
        if (column.filterType == FilterType::Date ||
            column.filterType == FilterType::Priority)
        {
            return column.title + QStringLiteral("    ");
        }

        return column.title;

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

    if(col.type == ColumnType::LineEdit){
        flags |= Qt::ItemIsEditable;
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
// 1. CheckBox 列处理
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
    // 2. 整行数据更新
    if(role == Qt::UserRole){
        m_rows[index.row()] = value.toMap();
        emit dataChanged(index, index);
        return true;
    }
    // 3. LineEdit / 可编辑列处理
    if((role == Qt::EditRole || role == Qt::DisplayRole)
        && col.editable){
        // // 3.1 写入当前字段,将当前单元格对应字段写回到行数据中
        m_rows[index.row()][col.field] = value;

        // 3.2 子类扩展点
        //     例如：
        //     - 输入物料标签码后自动带出 EPR 编码和批次号
        //     - 输入物料 SN 后自动校验
        // -----------------------------------------------------
        //
        // 需要在 BaseTableModel 中声明：
        //
        // protected:
        //     virtual void afterCellEdited(
        //         int row,
        //         const QString& field,
        //         const QVariant& value);
        //
        // 并提供默认空实现。
         afterCellEdited(index.row(), col.field, value);
        // 3.3 刷新整行
        //     因为 afterCellEdited() 可能修改了同一行中的其他字段
        //     （例如 EPR、批次号）
        // -----------------------------------------------------
        QModelIndex left  = this->index(index.row(), 0);
        QModelIndex right = this->index(
            index.row(),
            columnCount() - 1);

        emit dataChanged(
            left,
            right,
            {Qt::DisplayRole, Qt::EditRole});

        return true;
    }

    return false;
}

QString BaseTableModel::columnField(int column) const
{
    if(column < 0 || column >= m_columns.size())
    {
        return QString();
    }

    return m_columns[column].field;
}
