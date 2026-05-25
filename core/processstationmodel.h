#ifndef PROCESSSTATIONMODEL_H
#define PROCESSSTATIONMODEL_H

#include "basetablemodel.h"
#include "textlinkdelegate.h"

class ProcessStationModel : public BaseTableModel
{
    Q_OBJECT
public:
    enum TableType
    {
        MaterialCheck,      // 物料核对
        ProcessRoute,       // 工艺路线
        ProcessMaterial,    // 工序物料信息
        ToolEquipment       // 工具设备
    };
    Q_ENUM(TableType)

    explicit ProcessStationModel(QObject *parent = nullptr);

    // 切换表格类型（自动更新表头和数据）
    void setTableType(TableType type);

    // 当前表格类型
    TableType tableType() const;

private:
    // 物料核对
    void setMaterialCheckHeader();
    void setMaterialCheckData();
    TabConfigs materialCheckHeader() const;

    // 工艺路线
    void setProcessRouteHeader();
    void setProcessRouteData();

    // 工序物料信息
    void setProcessMaterialHeader();
    void setProcessMaterialData();

    // 工具设备
    void setToolEquipmentHeader();
    void setToolEquipmentData();

    //接口请求函数
    void requestMaterialCheckInfo(int row, const QString& labelCode);
    void requestProcessMaterialInfo(int row, const QString& materialSN);

    void applyMaterialCheckInfo(int row, const QVariantMap& data);
    void applyProcessMaterialInfo(int row, const QVariantMap& data);

protected:
    void afterCellEdited(
        int row,
        const QString& field,
        const QVariant& value) override;

private:
    TableType m_tableType{MaterialCheck};

};

#endif // PROCESSSTATIONMODEL_H
