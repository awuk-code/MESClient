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
            {tr("序号"),       "rowNumber", true, false, ColumnType::RowNumber},

            {tr("物料编码"),   "materialCode", true, false, ColumnType::TextLink},

            {tr("物料名称"),   "materialName"},

            {tr("单套数量"),   "singledQty"},

            {tr("生产所需数量"), "actualQty"},

            // 可输入列：物料标签码
            {tr("物料标签码"), "materialLabelCode", true, true, ColumnType::LineEdit},

            {tr("EPR编码"),    "EPR"},

            {tr("批次号"),     "batchNo"}
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
            {tr("序号"),                 "rowNumber", true, false, ColumnType::RowNumber},
            {tr("工序编号"),             "processCode"},
            {tr("工序名称"),             "processName"},
            {tr("并行步骤"),             "parallelStep"},
            {tr("当前工序待完成数量"),   "remainingQty"}
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
            {tr("序号"),       "rowNumber", true, false, ColumnType::RowNumber},

            {tr("产品SN"),     "productSN"},

            {tr("物料编码"),   "materialCode"},

            {tr("物料名称"),   "materialName"},

            {tr("物料型号"),   "materialModel"},

            {tr("数量"),       "quantity"},

            // 可输入列：物料SN
            // 关键设置：
            // 1. editable = true
            // 2. type = ColumnType::LineEdit
            {tr("物料SN"),     "materialSN", true, true, ColumnType::LineEdit},

            {tr("物料批次号"), "materialBatch"},

            {tr("序列号"),     "serialNumber"}
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
            {tr("序号"),       "rowNumber", true, false, ColumnType::RowNumber},

            {tr("产品SN"),   "productSN"},
            {tr("设备名称"), "equipmentName"},
            {tr("设备编号"), "equipmentCode"},
            {tr("规格型号"), "specModel"}
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
            {tr("产品SN码"),   "productSNCode"},
            {tr("物料类型"),   "materialType"},
            {tr("物料SN"),     "materialSN"},
            {tr("EPR编码"),    "EPR"},
            {tr("物料批次号"), "materialBatch"},
            {tr("物料名称"),   "materialName"},
            {tr("物料型号"),   "materialModel"}
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
