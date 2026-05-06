#include "productiontaskmodel.h"

ProductionTaskModel::ProductionTaskModel(QObject *parent)
    : QAbstractTableModel{parent}
{
    m_list = {
        {1,"A001","Line1",100,0},
        {2,"A002","Line1",200,1},
        {3,"A003","Line2",150,2},
        {4,"A004","Line2",300,0}
    };
}

int ProductionTaskModel::rowCount(const QModelIndex &) const { return m_list.size(); }
int ProductionTaskModel::columnCount(const QModelIndex &) const { return 5; }

QVariant ProductionTaskModel::data(const QModelIndex &i, int role) const
{
    if (!i.isValid()) return {};

    const auto& item = m_list[i.row()];

    if (role == Qt::DisplayRole)
    {
        switch(i.column())
        {
        case 0: return item.id;
        case 1: return item.product;
        case 2: return item.line;
        case 3: return item.count;
        case 4:
            return item.status==0?"未开工":item.status==1?"已开工":"已完成";
        }
    }
    // ===== 状态原始值（给过滤用）=====
    if (role == Qt::UserRole && i.column() == 4)
    {
        return item.status;
    }

    return {};
}

QVariant ProductionTaskModel::headerData(int s, Qt::Orientation o, int role) const
{
    if (o!=Qt::Horizontal || role!=Qt::DisplayRole) return {};
    return QStringList{"ID","产品","产线","数量","状态"}[s];
}

const QVector<TaskItem>& ProductionTaskModel::list() const { return m_list; }
