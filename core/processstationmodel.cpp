#include "processstationmodel.h"
#include <QMessageBox>

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
            // title          field                width visible editable fixedWidth alignment         type                    resizeMode                   filterType        delegate
            {"序号",           "rowNumber",          60,   true,  false,  true,      Qt::AlignCenter,  ColumnType::RowNumber,  QHeaderView::Fixed},

            {"物料编码",       "materialCode",      180,  true,  false,  false,     Qt::AlignCenter,  ColumnType::TextLink,     QHeaderView::ResizeToContents, FilterType::None},

            {"物料名称",       "materialName",      220,  true,  false,  false,     Qt::AlignCenter,  ColumnType::Normal,     QHeaderView::ResizeToContents},

            {"单套数量",       "singledQty",        100,  true,  false,  false,     Qt::AlignCenter,  ColumnType::Normal,     QHeaderView::ResizeToContents},

            {"生产所需数量",   "actualQty",         120,  true,  false,  false,     Qt::AlignCenter,  ColumnType::Normal,     QHeaderView::ResizeToContents},

            // 可输入列：物料标签码
            {"物料标签码",     "materialLabelCode", 220,  true,  true,   false,     Qt::AlignCenter,  ColumnType::LineEdit,   QHeaderView::Interactive,
             FilterType::None},

            {"EPR编码",        "EPR",               130,  true,  false,  false,     Qt::AlignCenter,  ColumnType::Normal,     QHeaderView::ResizeToContents},

            {"批次号",         "batchNo",           130,  true,  false,  false,     Qt::AlignCenter,  ColumnType::Normal,     QHeaderView::ResizeToContents}
        };
}

void ProcessStationModel::setMaterialCheckData()
{
    QVector<QVariantMap> rows;

    rows.append({
        {"materialCode", "MAT001"},
        {"materialName", "主板"},
        {"singledQty", 1},                 // 对应 单套数量
        {"actualQty", 100},                // 对应 生产所需数量
        {"materialLabelCode", "LBL202605001"},
        {"EPR", "EPR-001"},
        {"batchNo", "BATCH-001"}
    });

    rows.append({
        {"materialCode", "MAT002"},
        {"materialName", "外壳"},
        {"singledQty", 1},
        {"actualQty", 100},
        {"materialLabelCode", "LBL202605002"},
        {"EPR", "EPR-002"},
        {"batchNo", "BATCH-002"}
    });

    rows.append({
        {"materialCode", "MAT003"},
        {"materialName", "显示屏"},
        {"singledQty", 1},
        {"actualQty", 100},
        {"materialLabelCode", "LBL202605003"},
        {"EPR", "EPR-003"},
        {"batchNo", "BATCH-003"}
    });

    setRows(rows);
}

void ProcessStationModel::setProcessRouteHeader()
{
    m_columns =
        {
            {"序号",           "rowNumber",          60,   true,  false,  true,      Qt::AlignCenter,  ColumnType::RowNumber,  QHeaderView::Fixed},
            {"工序编号",           "processCode",     150},
            {"工序名称",           "processName",     180},
            {"并行步骤",           "parallelStep",    120},
            {"当前工序待完成数量", "remainingQty",    160}
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
            // title      field            width  visible editable fixedWidth alignment          type                    resizeMode
            {"序号",           "rowNumber",          60,   true,  false,  true,      Qt::AlignCenter,  ColumnType::RowNumber,  QHeaderView::Fixed},

            {"产品SN",     "productSN",      180,  true,   false,  false,     Qt::AlignCenter,   ColumnType::Normal,     QHeaderView::ResizeToContents},

            {"物料编码",   "materialCode",   180,  true,   false,  false,     Qt::AlignCenter,   ColumnType::Normal,     QHeaderView::ResizeToContents},

            {"物料名称",   "materialName",   220,  true,   false,  false,     Qt::AlignCenter,   ColumnType::Normal,     QHeaderView::ResizeToContents},

            {"物料型号",   "materialModel",  180,  true,   false,  false,     Qt::AlignCenter,   ColumnType::Normal,     QHeaderView::ResizeToContents},

            {"数量",       "quantity",       100,  true,   false,  false,     Qt::AlignCenter,   ColumnType::Normal,     QHeaderView::ResizeToContents},

            // 可输入列：物料SN
            // 关键设置：
            // 1. editable = true
            // 2. type = ColumnType::LineEdit
            {"物料SN",     "materialSN",     200,  true,   true,   false,     Qt::AlignCenter,   ColumnType::LineEdit,   QHeaderView::Interactive, FilterType::None},

            {"物料批次号", "materialBatch",  180,  true,   false,  false,     Qt::AlignCenter,   ColumnType::Normal,     QHeaderView::ResizeToContents},

            {"序列号",     "serialNumber",   180,  true,   false,  false,     Qt::AlignCenter,   ColumnType::Normal,     QHeaderView::ResizeToContents}
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
            {"序号",           "rowNumber",          60,   true,  false,  true,      Qt::AlignCenter,  ColumnType::RowNumber,  QHeaderView::Fixed},

            {"产品SN",   "productSN",     180},
            {"设备名称", "equipmentName", 200},
            {"设备编号", "equipmentCode", 180},
            {"规格型号", "specModel",     180}
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

void ProcessStationModel::requestMaterialCheckInfo(int row, const QString &labelCode)
{
    if (labelCode.isEmpty())
        return;

    // ====================================================
    // 这里未来替换成 HTTP 请求
    // ====================================================

    QVariantMap response;

    response["EPR"] =
        "EPR-" + labelCode.right(3);

    response["batchNo"] =
        "BATCH-" + labelCode.right(3);

    // ====================================================
    // 回填数据
    // ====================================================

    applyMaterialCheckInfo(row, response);
}

void ProcessStationModel::requestProcessMaterialInfo(int row, const QString &materialSN)
{
    if (row < 0 || row >= m_rows.size())
        return;

    QString sn = materialSN.trimmed();

    if (sn.isEmpty())
        return;

    // 2. 未来这里替换成 HTTP 接口
    // 3. 当前阶段：模拟接口返回

    QVariantMap response;


    response["materialBatch"] =
        "BATCH-" + sn.right(3);

    response["serialNumber"] =
        "SERIAL-" + sn.right(5);
    applyProcessMaterialInfo(row, response);
}

void ProcessStationModel::applyMaterialCheckInfo(int row, const QVariantMap &data)
{
    if (row < 0 || row >= m_rows.size())
        return;

    m_rows[row]["EPR"] =
        data.value("EPR");

    m_rows[row]["batchNo"] =
        data.value("batchNo");

    emit dataChanged(
        index(row, 0),
        index(row, columnCount() - 1));
}

void ProcessStationModel::applyProcessMaterialInfo(int row, const QVariantMap &data)
{
    if (row < 0 || row >= m_rows.size())
        return;

    m_rows[row]["materialBatch"] =
        data.value("materialBatch");

    m_rows[row]["serialNumber"] =
        data.value("serialNumber");

    emit dataChanged(
        index(row, 0),
        index(row, columnCount() - 1));
}


void ProcessStationModel::afterCellEdited(int row, const QString &field, const QVariant &value)
{
    //自动带出数据
    if(field == "materialLabelCode")
    {
        requestMaterialCheckInfo(row, value.toString());
    }

    else if(field == "materialSN")
    {
        requestProcessMaterialInfo(row, value.toString());
    }
}
