#include "repairstationmodel.h"

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
    QVector<QVariantMap> rows;

    rows.append({
        {"exceptionHandleNo", "EX20260527001"},
        {"exceptionHandleNoLink", "EX20260527001"},
        {"taskNo", "MO20250401001"},
        {"routeName", "装配工艺路线"},
        {"productModel", "XH-200"},
        {"productSN", "SN202605270001"},
        {"abnormalCount", 1},
        {"abnormalType", "外观异常"},
        {"abnormalPhenomenon", "壳体划伤"},
        {"abnormalImage", "查看图片"},
        {"reportProcess", "装配"},
        {"reportTime", "2026-05-27 09:30"},
        {"lineNo", "LINE-01"},
        {"stationNo", "ST-01"},
        {"reporter", "张三"},
        {"status", "待处理"}
    });

    rows.append({
        {"exceptionHandleNo", "EX20260527002"},
        {"exceptionHandleNoLink", "EX20260527002"},
        {"taskNo", "MO20250401002"},
        {"routeName", "测试工艺路线"},
        {"productModel", "XH-300"},
        {"productSN", "SN202605270002"},
        {"abnormalCount", 2},
        {"abnormalType", "功能异常"},
        {"abnormalPhenomenon", "通电无响应"},
        {"abnormalImage", "查看图片"},
        {"reportProcess", "测试"},
        {"reportTime", "2026-05-27 10:20"},
        {"lineNo", "LINE-02"},
        {"stationNo", "ST-02"},
        {"reporter", "李四"},
        {"handleMethod", "维修后复测"},
        {"status", "审核中"}
    });

    rows.append({
        {"exceptionHandleNo", "EX20260527003"},
        {"exceptionHandleNoLink", "EX20260527003"},
        {"reworkTaskNo", "RW20260527001"},
        {"routeName", "返工工艺路线"},
        {"productModel", "XH-200"},
        {"productSN", "SN202605270003"},
        {"abnormalCount", 1},
        {"abnormalType", "装配异常"},
        {"abnormalPhenomenon", "螺丝漏装"},
        {"abnormalImage", "查看图片"},
        {"reportProcess", "终检"},
        {"reportTime", "2026-05-27 11:15"},
        {"lineNo", "LINE-01"},
        {"stationNo", "ST-03"},
        {"reporter", "王五"},
        {"status", "待返工"}
    });

    rows.append({
        {"exceptionHandleNo", "EX20260527004"},
        {"exceptionHandleNoLink", "EX20260527004"},
        {"taskNo", "MO20250401004"},
        {"routeName", "老化测试路线"},
        {"productModel", "XH-500"},
        {"productSN", "SN202605270004"},
        {"abnormalCount", 1},
        {"abnormalType", "测试异常"},
        {"abnormalPhenomenon", "老化失败"},
        {"abnormalImage", "查看图片"},
        {"reportProcess", "老化"},
        {"reportTime", "2026-05-27 13:40"},
        {"lineNo", "LINE-03"},
        {"stationNo", "ST-04"},
        {"reporter", "赵六"},
        {"handleMethod", "更换电源模块"},
        {"status", "已处理"}
    });

    setRows(rows);
}
