#include "operationdelegate.h"
#include "commonfunc.h"

#include <QAbstractProxyModel>

OperationDelegate::OperationDelegate(QObject *parent)
    : BaseItemDelegate{parent}
{}

int OperationDelegate::minimumColumnWidth()
{
    // 操作列内有“标签打印”和“开工”两个按钮：
    // 左边距 10 + 按钮 60 + 间距 10 + 按钮 60 + 右边距 10。
    return 150;
}

/**
 * @brief 将修改后的整行数据写回真实 SourceModel
 *
 * 因为传入的 model是代理模型
 * 所以必须先映射到源模型，再调用 SourceModel::setData()。
 */

bool OperationDelegate::updateRowData(QAbstractItemModel *model, const QModelIndex &index, const QVariantMap &rowData)
{
    if(!model || !index.isValid()) return false;

    QModelIndex sourceIndex = index;
    QAbstractItemModel* sourceModel = model;
    // 如果是代理模型，则映射到源模型
    if(auto proxy = qobject_cast<QAbstractProxyModel*>(model)){
        sourceIndex = proxy->mapToSource(index);
        sourceModel = proxy->sourceModel();
    }

    if (!sourceModel || !sourceIndex.isValid())
        return false;

    // BaseTableModel::setData(Qt::UserRole) 会：
    // 1. 更新 m_rows
    // 2. emit dataChanged(...)

    bool ok = sourceModel->setData(
        sourceIndex,
        rowData,
        Qt::UserRole);

    return ok;
}
void OperationDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    //读取整行数据
    QVariantMap rowData =
        index.model()->data(index, Qt::UserRole).toMap();

    bool labelPrinted =
        rowData.value(FIELD_LABEL_PRINTED, false).toBool();

    QRect rect = option.rect;

    // 按钮区域
    QRect printRect(rect.left() + 10,
                    rect.top() + 6,
                    60,
                    rect.height() - 12);

    QRect startRect(rect.left() + 80,
                    rect.top() + 6,
                    60,
                    rect.height() - 12);

    // 绘制按钮
    // 标签打印按钮
    // 未打印：蓝色
    // 已打印：灰色
    QColor printColor =
        labelPrinted ? QColor(0xC0C4CC)
                     : QColor(0x409EFF);


    // 打印
    painter->setBrush(printColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(printRect, 4, 4);

    painter->setPen(Qt::white);
    painter->drawText(printRect,
                      Qt::AlignCenter,
                      tr("标签打印"));

    // 开工按钮
    // 未打印：灰色
    // 已打印：绿色
    QString status =  rowData.value("status").toString();
    QColor startColor;
    if (status != tr("已完工"))
    {
        startColor = QColor("#67C23A");
    }
    else
    {
        startColor = QColor("#C0C4CC");
    }

    painter->setBrush(startColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(startRect, 4, 4);

    painter->setPen(Qt::white);
    painter->drawText(startRect,
                      Qt::AlignCenter,
                      tr("开工"));

    painter->restore();
}

QSize OperationDelegate::sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    Q_UNUSED(index)

    QSize size = BaseItemDelegate::sizeHint(option, index);
    size.setWidth(qMax(size.width(), minimumColumnWidth()));
    return size;
}

bool OperationDelegate::editorEvent(
    QEvent *event,
    QAbstractItemModel *model,
    const QStyleOptionViewItem &option,
    const QModelIndex &index)
{
    if (!model || !index.isValid())
        return false;

    if (event->type() != QEvent::MouseButtonRelease)
        return false;

    auto* mouseEvent = static_cast<QMouseEvent*>(event);

    QVariantMap rowData =
        model->data(index, Qt::UserRole).toMap();

    QRect rect = option.rect;

    QRect printRect(rect.left() + 10,
                    rect.top() + 6,
                    60,
                    rect.height() - 12);

    QRect startRect(rect.left() + 80,
                    rect.top() + 6,
                    60,
                    rect.height() - 12);

    QPoint pos = mouseEvent->pos();

    //------------------------
    if (printRect.contains(pos))
    {
        // if(!labelPrinted){

        //更新状态：已打印
        rowData[FIELD_LABEL_PRINTED] = true;
        // 写回模型（内部会自动刷新界面）
        if (updateRowData(model, index, rowData))
        {
            // 后续可在此连接实际打印机逻辑
            emit sigPrintClicked(index.row());
        }
        //  }
        return true;
    }

    //点击开工-----------只有已打印才能点击
    if (startRect.contains(pos))
    {
        QString status =
            rowData.value("status").toString();

        // 开工按钮只限制已完工任务不可再次开工；后续若需要恢复“必须先标签打印”，在这里加接口/状态判断。
        if (status != tr("已完工"))
        {
            rowData[FIELD_STARTED] = true;
            rowData["status"] = tr("已开工");

            if (updateRowData(model, index, rowData))
            {
                // 后续在页面中接收此信号：
                // 1. 调用接口通知 MES 系统
                // 2. 跳转到工序站点页面
                emit sigStartClicked(index.row());
                emit sigStartClicked(rowData);
            }
        }

        return true;
    }



    return false;
}
