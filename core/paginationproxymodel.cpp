#include "paginationproxymodel.h"

#include <QtGlobal>

PaginationProxyModel::PaginationProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

int PaginationProxyModel::currentPage() const
{
    return m_currentPage;
}

int PaginationProxyModel::pageSize() const
{
    return m_pageSize;
}

int PaginationProxyModel::pageCount() const
{
    const int rows = totalRows();
    if (rows <= 0)
        return 1;

    return (rows + m_pageSize - 1) / m_pageSize;
}

int PaginationProxyModel::totalRows() const
{
    return sourceModel() ? sourceModel()->rowCount() : 0;
}

void PaginationProxyModel::setCurrentPage(int page)
{
    const int nextPage = boundedPage(page);
    if (m_currentPage == nextPage)
        return;

    m_currentPage = nextPage;
    invalidateFilter();
    emit pageInfoChanged();
}

void PaginationProxyModel::setPageSize(int pageSize)
{
    const int nextPageSize = qMax(1, pageSize);
    if (m_pageSize == nextPageSize)
        return;

    m_pageSize = nextPageSize;
    m_currentPage = boundedPage(m_currentPage);
    invalidateFilter();
    emit pageInfoChanged();
}

void PaginationProxyModel::firstPage()
{
    setCurrentPage(1);
}

void PaginationProxyModel::previousPage()
{
    setCurrentPage(m_currentPage - 1);
}

void PaginationProxyModel::nextPage()
{
    setCurrentPage(m_currentPage + 1);
}

bool PaginationProxyModel::filterAcceptsRow(
    int sourceRow,
    const QModelIndex& sourceParent) const
{
    Q_UNUSED(sourceParent)

    const int startRow = (m_currentPage - 1) * m_pageSize;
    const int endRow = startRow + m_pageSize;
    return sourceRow >= startRow && sourceRow < endRow;
}

int PaginationProxyModel::boundedPage(int page) const
{
    return qBound(1, page, pageCount());
}
