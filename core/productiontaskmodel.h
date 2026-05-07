#ifndef PRODUCTIONTASKMODEL_H
#define PRODUCTIONTASKMODEL_H

#include <QAbstractTableModel>
#include "basetablemodel.h"

class ProductionTaskModel : public BaseTableModel
{
public:
    explicit ProductionTaskModel(QObject *parent = nullptr);
};

#endif // PRODUCTIONTASKMODEL_H
