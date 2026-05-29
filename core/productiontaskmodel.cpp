#include "productiontaskmodel.h"
#include "jsondataloader.h"
#include "operationdelegate.h"

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
            QOverload<int>::of(&OperationDelegate::sigStartClicked),
            this,
            [](int row)
            {
                qDebug() << __FUNCTION__ << "开工 row =" << row;
            });

}

void ProductionTaskModel::setColumnHeader()
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

            // 计划开始时间
            {
                "计划开始时间",
                "startTime",
                130,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::Fixed,
                FilterType::Date
            },

            // 计划完成时间
            {
                "计划完成时间",
                "finishTime",
                130,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::Fixed,
                FilterType::Date
            },

            // 优先级
            {
                "优先级",
                "priority",
                100,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::Normal,
                QHeaderView::Fixed,
                FilterType::Priority
            },

            // 操作列
            {
                "操作",
                "action",
                180,
                true,
                false,
                true,
                Qt::AlignCenter,
                ColumnType::Operation,
                QHeaderView::Fixed,
                FilterType::None,
                opDelegate
            }
        };

}

void ProductionTaskModel::setColumnData()
{
    // 当前阶段的列表数据从 etc/production_tasks.json 读取；
    // 后续对接后台时，只需要把这里的数据来源替换成接口返回。
    setRows(JsonDataLoader::loadRows("production_tasks.json"));
}
