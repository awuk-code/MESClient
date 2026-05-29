#include "repairstationmodel.h"
#include "jsondataloader.h"

RepairStationModel::RepairStationModel(QObject *parent)
    : BaseTableModel{parent}
{
    m_checkBoxDelegate = new CheckBoxDelegate(this);
    m_textLinkDelegate = new TextLinkDelegate(this);
    initConnect();
    setColumnHeader();
    setColumnData();
}

void RepairStationModel::initConnect()
{


}

void RepairStationModel::setColumnHeader()
{
    m_columns =
        {
            {
                tr("选择"),
                "checked",
                true,
                false,
                ColumnType::CheckBox,
                FilterType::None,
                m_checkBoxDelegate
            },
            {
                tr("序号"),
                "rowNumber",
                true,
                false,
                ColumnType::RowNumber,
            },
            {
                tr("异常处理单号"),
                "exceptionHandleNoLink",
                true,
                false,
                ColumnType::TextLink,
                FilterType::Keyword,
                m_textLinkDelegate
            },
            {
                tr("异常处理单号"),
                "exceptionHandleNo",
                true,
                false,
                ColumnType::Normal,
                FilterType::Keyword
            },
            {
                tr("生产任务单号"),
                "taskNo",
                true,
                false,
                ColumnType::Normal,
                FilterType::Keyword
            },
            {
                tr("返工任务单号"),
                "reworkTaskNo",
                true,
                false,
                ColumnType::TextLink,
                FilterType::Keyword,
                m_textLinkDelegate
            },
            {
                tr("生产工艺路线"),
                "routeName"
            },
            {
                tr("产品型号"),
                "productModel",
                true,
                false,
                ColumnType::Normal,
                FilterType::Keyword
            },
            {
                tr("产品SN"),
                "productSN"
            },
            {
                tr("异常数量"),
                "abnormalCount"
            },
            {
                tr("异常类型"),
                "abnormalType"
            },
            {
                tr("异常现象"),
                "abnormalPhenomenon"
            },
            {
                tr("异常图片"),
                "abnormalImage",
                true,
                false,
                ColumnType::TextLink,
                FilterType::None,
                m_textLinkDelegate
            },
            {
                tr("异常上报工序"),
                "reportProcess"
            },
            {
                tr("异常上报时间"),
                "reportTime"
            },
            {
                tr("产线编码"),
                "lineNo"
            },
            {
                tr("工作站编号"),
                "stationNo"
            },
            {
                tr("上报人员"),
                "reporter"
            },
            {
                tr("异常处理方式"),
                "handleMethod"
            },

        };
}

void RepairStationModel::setColumnData()
{
    // 当前阶段的维修站列表数据从 etc/repair_station_rows.json 读取；
    // 后续对接后台时，只需要把这里的数据来源替换成接口返回。
    setRows(JsonDataLoader::loadRows("repair_station_rows.json"));
}
