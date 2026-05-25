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
            // 选择框
            {
                "选择",                    // title
                "checked",                // field
                50,                       // width
                true,                     // visible
                false,                    // editable
                true,                     // fixedWidth
                Qt::AlignCenter,          // alignment
                ColumnType::CheckBox,     // type
                QHeaderView::Fixed,       // resizeMode
                FilterType::None,         // filterType
                m_checkBoxDelegate        // delegate
            },

            // 序号
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

            // 生产任务单号
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

            // 产品型号
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

            // ERP编码
            {
                "产品ERP编码",
                "erpCode",
                180,
                true,
                false,
                false,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::ResizeToContents,
                FilterType::Keyword
            },

            // 产品名称
            {
                "产品名称",
                "productName",
                180,
                true,
                false,
                false,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::ResizeToContents,
                FilterType::Keyword
            },

            // 产品类型
            {
                "产品类型",
                "productType",
                120
            },

            // 生产线编号
            {
                "生产线编号",
                "lineNo",
                150
            },

            // 工单数量
            {
                "工单数量",
                "workCount",
                120
            },

            // 是否分配
            {
                "是否分配",
                "isAssigned",
                120
            },

            // info任务单号
            {
                "info任务单号",
                "infoTaskNo",
                180
            },

            // 工艺路线
            {
                "生产工艺路线",
                "routeName",
                180
            },

        };
}

void RepairStationModel::setColumnData()
{
    QVector<QVariantMap> rows;

    rows.append({
                 {"taskNo", "MO20250401001"},
                 {"productModel", "XH-200"},
                 {"erpCode", "ERP-001"},
                 {"productName", "1"},
                 {"productType", "成品"},
                 {"lineNo", "LINE-01"},
                 {"workCount", 100},
                 {"isAssigned", "已分配"},
                 {"infoTaskNo", "INFO-0001"},
                 {"routeName", "装配工艺路线"},
                 {"startTime", "2026-05-08 08:00"},
                 {"finishTime", "2026-05-10 18:00"},
                 {"priority", "高"},

                 // ⭐ 不显示，但过滤需要
                 {"status", "未开工"},


                 });
    setRows(rows);
}
