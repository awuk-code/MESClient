#include "fieldfilterproxymodel.h"


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

bool FieldFilterProxyModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    // ===== 关键字过滤 =====
    if (!m_keyword.isEmpty())
    {
        QModelIndex index = sourceModel()->index(row, m_filterColumn);
        QString value = sourceModel()->data(index, Qt::DisplayRole).toString();

        if (!value.contains(m_keyword, Qt::CaseInsensitive))
            return false;
    }

    // ===== 状态过滤（假设状态在第4列，用UserRole更规范）=====
    if (m_status != -1)
    {
        QModelIndex statusIndex = sourceModel()->index(row, 4);
        int status = sourceModel()->data(statusIndex, Qt::UserRole).toInt();

        if (status != m_status)
            return false;
    }

    return true;
}
