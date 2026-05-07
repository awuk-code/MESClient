#ifndef BASETABLEMODEL_H
#define BASETABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QVariantMap>

#include "commonfunc.h"

class BaseTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BaseTableModel(QObject *parent = nullptr);

    // 设置数据
    void setRows(const QVector<QVariantMap>& rows);

    // 获取列配置
    const QVector<ColumnConfig>& columns() const;

protected:
    QVector<ColumnConfig> m_columns;
    QVector<QVariantMap> m_rows;

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;

    QSet<int> m_checkedRows;
Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role) override;

};

#endif // BASETABLEMODEL_H
