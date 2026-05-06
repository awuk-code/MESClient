#ifndef PRODUCTIONTASKMODEL_H
#define PRODUCTIONTASKMODEL_H

#include <QAbstractTableModel>

struct TaskItem
{
    int id;
    QString product;
    QString line;
    int count;
    int status; // 0未开工 1已开工 2已完成
};
class ProductionTaskModel : public QAbstractTableModel
{
public:
    explicit ProductionTaskModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &, int role) const override;
    QVariant headerData(int, Qt::Orientation, int) const override;

    const QVector<TaskItem>& list() const;

private:
    QVector<TaskItem> m_list;


};

#endif // PRODUCTIONTASKMODEL_H
