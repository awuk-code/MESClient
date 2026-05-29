#include "processstationmodel.h"
#include "jsondataloader.h"
#include <QMessageBox>
#include <QDebug>

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

    case ReplacementMaterial:
        setReplacementMaterialHeader();
        setReplacementMaterialData();
        break;
    }

    qDebug() << __FUNCTION__ << "ProcessStationModel table type:" << static_cast<int>(type);
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
    // 当前阶段的物料核对数据从 JSON 读取；后续对接后台时替换为接口返回即可。
    setRows(JsonDataLoader::loadRows("process_station_material_check.json"));
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
    // 当前阶段的工艺路线数据从 JSON 读取；工艺路线标题和列结构仍保留在代码中。
    setRows(JsonDataLoader::loadRows("process_station_route.json"));
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
    // 当前阶段的工序物料信息从 JSON 读取；物料SN仍保留为空，方便显示输入提示。
    setRows(JsonDataLoader::loadRows("process_station_materials.json"));
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
    // 当前阶段的工具设备数据从 JSON 读取；后续按当前工序调用后台接口替换。
    setRows(JsonDataLoader::loadRows("process_station_tool_equipment.json"));
}

void ProcessStationModel::setReplacementMaterialHeader()
{
    m_columns =
        {
            {"产品SN码",   "productSNCode", 180, true, false, false, Qt::AlignCenter, ColumnType::Normal, QHeaderView::ResizeToContents},
            {"物料类型",   "materialType",  140, true, false, false, Qt::AlignCenter, ColumnType::Normal, QHeaderView::ResizeToContents},
            {"物料SN",     "materialSN",    200, true, false, false, Qt::AlignCenter, ColumnType::Normal, QHeaderView::ResizeToContents},
            {"EPR编码",    "EPR",           150, true, false, false, Qt::AlignCenter, ColumnType::Normal, QHeaderView::ResizeToContents},
            {"物料批次号", "materialBatch", 180, true, false, false, Qt::AlignCenter, ColumnType::Normal, QHeaderView::ResizeToContents},
            {"物料名称",   "materialName",  180, true, false, false, Qt::AlignCenter, ColumnType::Normal, QHeaderView::ResizeToContents},
            {"物料型号",   "materialModel", 220, true, false, false, Qt::AlignCenter, ColumnType::Normal, QHeaderView::ResizeToContents}
        };

    qDebug() << __FUNCTION__ << "replacement material columns:" << m_columns.size();
}

void ProcessStationModel::setReplacementMaterialData()
{
    // 当前阶段的更换物料信息从 JSON 读取；返工权限接入后可按任务单号重新拉取。
    const QVector<QVariantMap> rows =
        JsonDataLoader::loadRows("process_station_replacement_materials.json");
    setRows(rows);
    qDebug() << __FUNCTION__ << "replacement material rows:" << rows.size();
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
