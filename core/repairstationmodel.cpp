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
                "选择",
                "checked",
                50,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::CheckBox,
                QHeaderView::Fixed,
                FilterType::None,
                m_checkBoxDelegate
            },
            {
                "序号",
                "rowNumber",
                50,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::RowNumber,
                QHeaderView::Fixed
            },
            {
                "异常处理单号",
                "exceptionHandleNoLink",
                170,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::TextLink,
                QHeaderView::Fixed,
                FilterType::Keyword,
                m_textLinkDelegate
            },
            {
                "异常处理单号",
                "exceptionHandleNo",
                170,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::Fixed,
                FilterType::Keyword
            },
            {
                "生产任务单号",
                "taskNo",
                180,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::Fixed,
                FilterType::Keyword
            },
            {
                "返工任务单号",
                "reworkTaskNo",
                180,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::TextLink,
                QHeaderView::Fixed,
                FilterType::Keyword,
                m_textLinkDelegate
            },
            {
                "生产工艺路线",
                "routeName",
                180
            },
            {
                "产品型号",
                "productModel",
                180,
                true,
                false,
                false,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::ResizeToContents,
                FilterType::Keyword
            },
            {
                "产品SN",
                "productSN",
                180
            },
            {
                "异常数量",
                "abnormalCount",
                100
            },
            {
                "异常类型",
                "abnormalType",
                130
            },
            {
                "异常现象",
                "abnormalPhenomenon",
                180
            },
            {
                "异常图片",
                "abnormalImage",
                120,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::TextLink,
                QHeaderView::Fixed,
                FilterType::None,
                m_textLinkDelegate
            },
            {
                "异常上报工序",
                "reportProcess",
                150
            },
            {
                "异常上报时间",
                "reportTime",
                160
            },
            {
                "产线编码",
                "lineNo",
                130
            },
            {
                "工作站编号",
                "stationNo",
                130
            },
            {
                "上报人员",
                "reporter",
                120
            },
            {
                "异常处理方式",
                "handleMethod",
                160
            },

        };
}

void RepairStationModel::setColumnData()
{
    // 当前阶段的维修站列表数据从 etc/repair_station_rows.json 读取；
    // 后续对接后台时，只需要把这里的数据来源替换成接口返回。
    setRows(JsonDataLoader::loadRows("repair_station_rows.json"));
}
