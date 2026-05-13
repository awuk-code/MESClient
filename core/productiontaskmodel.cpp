#include "productiontaskmodel.h"
#include "../ui/operationdelegate.h"

#include <QDebug>

ProductionTaskModel::ProductionTaskModel(QObject *parent)
    : BaseTableModel(parent)
{
    // ===== 1. 创建 delegate =====
    m_checkBoxDelegate = new CheckBoxDelegate(this);
    opDelegate =  new OperationDelegate(this);
    initConnect();
    setColumnHeader();
    setColumnData();
}

void ProductionTaskModel::initConnect()
{
    // ===== 2. 绑定信号 =====
    connect(opDelegate,
            &OperationDelegate::sigPrintClicked,
            this,
            [](int row)
            {
                qDebug() << __FUNCTION__ <<"打印 row =" << row;
            });

    connect(opDelegate,
            &OperationDelegate::sigStartClicked,
            this,
            [](int row)
            {
                qDebug() << __FUNCTION__ << "开工 row =" << row;
            });

}

void ProductionTaskModel::setColumnHeader()
{
    // ===== 3. 列配置 =====
    m_columns =
        {
            // 选择框
            {
                "选择",
                "checked",
                50,
                true,
                Qt::AlignCenter,
                ColumnType::CheckBox,
                QHeaderView::Fixed,
                //添加选择框代理
                m_checkBoxDelegate
            },

            // 序号
            {
                "序号",
                "rowNumber",
                50,
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
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::Fixed
            },

            // 产品型号
            {
                "产品型号",
                "productModel",
                180,
                true,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::ResizeToContents
            },

            // ERP编码
            {
                "产品ERP编码",
                "erpCode",
                180
            },

            // 产品名称
            {
                "产品名称",
                "productName",
                180
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

            // 开始时间
            {
                "计划开始时间",
                "startTime",
                130,
                true,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::Fixed
            },

            // 完成时间
            {
                "计划完成时间",
                "finishTime",
                130,
                true,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::Fixed

            },

            // 优先级
        {
            "优先级",
            "priority",
            100,
            true,
            Qt::AlignCenter,
            ColumnType::Normal,
            QHeaderView::Fixed
        },

            // 操作列
            {
                "操作",
                "action",
                180,
                true,
                Qt::AlignCenter,
                ColumnType::Operation,
                QHeaderView::Fixed,
                opDelegate
            }
        };
}

void ProductionTaskModel::setColumnData()
{
    // ===== 4. 模拟数据 =====
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

        {FIELD_LABEL_PRINTED, false},
        {FIELD_STARTED, false},
        {"action", ""}

    });

    rows.append({
        {"taskNo", "MO20250401002"},
        {"productModel", "XH-300"},
        {"erpCode", "ERP-002"},
        {"productName", "2"},
        {"productType", "2"},
        {"lineNo", "LINE-02"},
        {"workCount", 200},
        {"isAssigned", "未分配"},
        {"infoTaskNo", "INFO-0002"},
        {"routeName", "测试工艺路线"},
        {"startTime", "2026-05-09 08:00"},
        {"finishTime", "2026-05-11 18:00"},
        {"priority", "中"},

        {"status", "已开工"},
        {FIELD_LABEL_PRINTED, true},
        {FIELD_STARTED, true},
        {"action", ""}
    });

    rows.append({
        {"taskNo", "MO20250401002"},
        {"productModel", "XH-300"},
        {"erpCode", "ERP-002"},
        {"productName", "3"},
        {"productType", "3"},
        {"lineNo", "LINE-02"},
        {"workCount", 200},
        {"isAssigned", "未分配"},
        {"infoTaskNo", "INFO-0002"},
        {"routeName", "测试工艺路线"},
        {"startTime", "2026-05-09 08:00"},
        {"finishTime", "2026-05-11 18:00"},
        {"priority", "中"},

        {"status", "已完工"},
        {FIELD_LABEL_PRINTED, true},
        {FIELD_STARTED, false},
        {"action", ""}
    });
    rows.append({
        {"taskNo", "MO20250401001"},
        {"productModel", "XH-200"},
        {"erpCode", "ERP-001"},
        {"productName", "4"},
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
        {"status", "已开工"},
        {FIELD_LABEL_PRINTED, true},
        {FIELD_STARTED, true},
        {"action", ""}
    });

    setRows(rows);
}
