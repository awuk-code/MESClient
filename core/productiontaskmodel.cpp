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
                qDebug() << __FUNCTION__ << ProductionTaskModel::tr("打印 row =") << row;
            });

    connect(opDelegate,
            QOverload<int>::of(&OperationDelegate::sigStartClicked),
            this,
            [](int row)
            {
                qDebug() << __FUNCTION__ << ProductionTaskModel::tr("开工 row =") << row;
            });

}

void ProductionTaskModel::setColumnHeader()
{
    m_columns =
        {
            // 选择框
            {
                tr("选择"),                // title
                "checked",                // field
                true,                     // visible
                false,                    // editable
                ColumnType::CheckBox,     // type
                FilterType::None,         // filterType
                m_checkBoxDelegate        // delegate
            },

            // 序号
            {
                tr("序号"),
                "rowNumber",
                true,
                false,
                ColumnType::RowNumber,
            },

            // 生产任务单号
            {
                tr("生产任务单号"),
                "taskNo",
                true,
                false,
                ColumnType::Normal,
                FilterType::Keyword
            },

            // 产品型号
            {
                tr("产品型号"),
                "productModel",
                true,
                false,
                ColumnType::Normal,
                FilterType::Keyword
            },

            // ERP编码
            {
                tr("产品ERP编码"),
                "erpCode",
                true,
                false,
                ColumnType::Normal,
                FilterType::Keyword
            },

            // 产品名称
            {
                tr("产品名称"),
                "productName",
                true,
                false,
                ColumnType::Normal,
                FilterType::Keyword
            },

            // 产品类型
            {
                tr("产品类型"),
                "productType"
            },

            // 生产线编号
            {
                tr("生产线编号"),
                "lineNo"
            },

            // 工单数量
            {
                tr("工单数量"),
                "workCount"
            },

            // 是否分配
            {
                tr("是否分配"),
                "isAssigned"
            },

            // info任务单号
            {
                tr("info任务单号"),
                "infoTaskNo"
            },

            // 工艺路线
            {
                tr("生产工艺路线"),
                "routeName"
            },

            // 计划开始时间
            {
                tr("计划开始时间"),
                "startTime",
                true,
                false,
                ColumnType::Normal,
                FilterType::Date
            },

            // 计划完成时间
            {
                tr("计划完成时间"),
                "finishTime",
                true,
                false,
                ColumnType::Normal,
                FilterType::Date
            },

            // 优先级
            {
                tr("优先级"),
                "priority",
                true,
                false,
                ColumnType::Normal,
                FilterType::Priority
            },

            // 操作列
            {
                tr("操作"),
                "action",
                true,
                false,
                ColumnType::Operation,
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
