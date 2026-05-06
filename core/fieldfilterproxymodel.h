#ifndef FIELDFILTERPROXYMODEL_H
#define FIELDFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class FieldFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FieldFilterProxyModel(QObject *parent = nullptr);

    void setFilterColumn(int column);     // 指定过滤列（核心）
    void setKeyword(const QString& keyword);
    void setStatus(int status);           // 可选

protected:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const override;

private:
    int m_filterColumn = 0;
    QString m_keyword;
    int m_status = -1;
};

#endif // FIELDFILTERPROXYMODEL_H
