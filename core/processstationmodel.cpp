#include "processstationmodel.h"

ProcessStationModel::ProcessStationModel(QObject *parent)
    : BaseTableModel(parent)
{
    setTableType(MaterialCheck);
}

void ProcessStationModel::setTableType(TableType type)
{
    m_tableType = type;

    switch (type)
    {
    case MaterialCheck:
        setMaterialCheckHeader();
        setMaterialCheckData();
        break;

    case ProcessRoute:
        setProcessRouteHeader();
        setProcessRouteData();
        break;

    case ProcessMaterial:
        setProcessMaterialHeader();
        setProcessMaterialData();
        break;

    case ToolEquipment:
        setToolEquipmentHeader();
        setToolEquipmentData();
        break;
    }
}

ProcessStationModel::TableType ProcessStationModel::tableType() const
{
    return m_tableType;
}

void ProcessStationModel::setMaterialCheckHeader()
{
    m_columns =
        {
            {"序号", "rowNumber", 60},
            {"物料编码", "materialCode", 180},
            {"物料名称", "materialName", 220},
            {"需求数量", "requiredQty", 100},
            {"实发数量", "actualQty", 100},
            {"核对状态", "checkStatus", 120}
        };
}

void ProcessStationModel::setMaterialCheckData()
{
    QVector<QVariantMap> rows;

    rows.append({
        {"materialCode", "MAT001"},
        {"materialName", "主板"},
        {"requiredQty", 1},
        {"actualQty", 1},
        {"checkStatus", "PASS"}
    });

    rows.append({
        {"materialCode", "MAT002"},
        {"materialName", "外壳"},
        {"requiredQty", 1},
        {"actualQty", 1},
        {"checkStatus", "PASS"}
    });

    setRows(rows);
}

void ProcessStationModel::setProcessRouteHeader()
{
    m_columns =
        {
            {"序号", "rowNumber", 60},
            {"工序编号", "processCode", 150},
            {"工序名称", "processName", 180},
            {"工位", "stationNo", 120},
            {"状态", "status", 120}
        };
}

void ProcessStationModel::setProcessRouteData()
{
    QVector<QVariantMap> rows;

    rows.append({
        {"processCode", "OP001"},
        {"processName", "装配"},
        {"stationNo", "A01"},
        {"status", "已完成"}
    });

    rows.append({
        {"processCode", "OP002"},
        {"processName", "测试"},
        {"stationNo", "T01"},
        {"status", "进行中"}
    });

    rows.append({
        {"processCode", "OP003"},
        {"processName", "包装"},
        {"stationNo", "P01"},
        {"status", "未开始"}
    });

    setRows(rows);
}
void ProcessStationModel::setProcessMaterialHeader()
{
    m_columns =
        {
            {"序号", "rowNumber", 60},
            {"物料编码", "materialCode", 180},
            {"物料名称", "materialName", 220},
            {"单位", "unit", 100},
            {"用量", "quantity", 100},
            {"备注", "remark", 200}
        };
}

void ProcessStationModel::setProcessMaterialData()
{
    QVector<QVariantMap> rows;

    rows.append({
        {"materialCode", "RM001"},
        {"materialName", "螺丝"},
        {"unit", "PCS"},
        {"quantity", 4},
        {"remark", ""}
    });

    rows.append({
        {"materialCode", "RM002"},
        {"materialName", "标签"},
        {"unit", "PCS"},
        {"quantity", 1},
        {"remark", "贴于背面"}
    });

    setRows(rows);
}

void ProcessStationModel::setToolEquipmentHeader()
{
    m_columns =
        {
            {"序号", "rowNumber", 60},
            {"设备编号", "equipmentCode", 150},
            {"设备名称", "equipmentName", 180},
            {"状态", "status", 100},
            {"校验日期", "verifyDate", 150}
        };
}

void ProcessStationModel::setToolEquipmentData()
{
    QVector<QVariantMap> rows;

    rows.append({
        {"equipmentCode", "EQ001"},
        {"equipmentName", "扭力枪"},
      //  {"status", "正常"},
        {"verifyDate", "2026-05-01"}
    });

    rows.append({
        {"equipmentCode", "EQ002"},
        {"equipmentName", "条码枪"},
       // {"status", "正常"},
        {"verifyDate", "2026-04-20"}
    });

    setRows(rows);
}
