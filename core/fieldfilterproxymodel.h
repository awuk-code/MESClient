#ifndef FIELDFILTERPROXYMODEL_H
#define FIELDFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

class FieldFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FieldFilterProxyModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    //搜索过滤
    void setFilterColumn(int column);     // 指定过滤列（核心）
    void addSearchFields(const QString& field);
    void setKeyword(const QString& keyword);
    void setStatus(QVariant status);           // 可选
    //表头过滤
    void setFieldFilter(const QString &field, const QVariant &value);

protected:
    bool filterAcceptsRow(int srcRow, const QModelIndex &srcParent) const override;

private:
    int m_filterColumn = 0;
    QString m_keyword;
    QStringList searchFields;
    QVariant m_status;

    QMap<QString, QVariant> m_fieldFilters; //用于表头图标过滤
};

#endif // FIELDFILTERPROXYMODEL_H
