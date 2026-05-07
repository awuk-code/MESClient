#include "productiontaskmodel.h"
#include "operationdelegate.h"

#include <QDebug>

ProductionTaskModel::ProductionTaskModel(QObject *parent)
    : BaseTableModel(parent)
{
    // ===== 1. 创建 delegate =====
    OperationDelegate* opDelegate =
        new OperationDelegate(this);

    // ===== 2. 绑定信号 =====
    connect(opDelegate,
            &OperationDelegate::sigPrintClicked,
            this,
            [](int row)
            {
                qDebug() << "打印 row =" << row;
            });

    connect(opDelegate,
            &OperationDelegate::sigStartClicked,
            this,
            [](int row)
            {
                qDebug() << "开工 row =" << row;
            });

    // ===== 3. 列配置 =====
    m_columns =
        {
            {"选择", "", 50, true, Qt::AlignCenter, ColumnType::CheckBox},

            {"序号", "", 70, true, Qt::AlignCenter, ColumnType::RowNumber},

            {"任务单号", "taskNo", 180},

            {"产品编码", "productCode", 150},

            {"产品名称", "productName", 200},

            {"计划数量", "planCount", 100},

            {"状态", "status", 100},

            {
                "操作",
                "action",
                180,
                true,
                Qt::AlignCenter,
                ColumnType::Operation,
                opDelegate
            }
        };

    // ===== 4. 模拟数据 =====
    QVector<QVariantMap> rows;

    rows.append({
        {"taskNo", "MO20250401001"},
        {"productCode", "P10001"},
        {"productName", "减速器"},
        {"planCount", 100},
        {"status", "未开工"},
        {"action", ""}
    });

    rows.append({
        {"taskNo", "MO20250401002"},
        {"productCode", "P10002"},
        {"productName", "电机"},
        {"planCount", 200},
        {"status", "已开工"},
        {"action", ""}
    });

    setRows(rows);
}
