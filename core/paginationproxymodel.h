#ifndef PAGINATIONPROXYMODEL_H
#define PAGINATIONPROXYMODEL_H

#include <QSortFilterProxyModel>

class PaginationProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit PaginationProxyModel(QObject* parent = nullptr);

    int currentPage() const;
    int pageSize() const;
    int pageCount() const;
    int totalRows() const;

    void setCurrentPage(int page);
    void setPageSize(int pageSize);
    void firstPage();
    void previousPage();
    void nextPage();

signals:
    void pageInfoChanged();

protected:
    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex& sourceParent) const override;

private:
    int boundedPage(int page) const;

    int m_currentPage{1};
    int m_pageSize{50};
};

#endif // PAGINATIONPROXYMODEL_H
