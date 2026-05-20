#ifndef PRODUCTIONTASKMODEL_H
#define PRODUCTIONTASKMODEL_H

#include <QAbstractTableModel>
#include "basetablemodel.h"
#include "checkboxdelegate.h"
#include "operationdelegate.h"

class ProductionTaskModel : public BaseTableModel
{
public:
    explicit ProductionTaskModel(QObject *parent = nullptr);

    void initConnect();
    void setColumnHeader();
    void setColumnData();

    CheckBoxDelegate* m_checkBoxDelegate {nullptr};
    OperationDelegate* opDelegate{nullptr};
};

#endif // PRODUCTIONTASKMODEL_H
