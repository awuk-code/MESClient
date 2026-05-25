#ifndef REPAIRSTATIONMODEL_H
#define REPAIRSTATIONMODEL_H

#include "basetablemodel.h"
#include "checkboxdelegate.h"
#include "textlinkdelegate.h"

class RepairStationModel : public BaseTableModel
{
    Q_OBJECT
public:
    explicit RepairStationModel(QObject *parent = nullptr);

    TextLinkDelegate* textLinkDelegate() const
    {
        return m_textLinkDelegate;
    }
private:
    CheckBoxDelegate* m_checkBoxDelegate {nullptr};
    TextLinkDelegate* m_textLinkDelegate{nullptr};
    void initConnect();
    void setColumnHeader();
    void setColumnData();
};

#endif // REPAIRSTATIONMODEL_H
