#include "operationdelegate.h"

OperationDelegate::OperationDelegate(QObject *parent)
    : BaseItemDelegate{parent}
{}
void OperationDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &) const
{
    painter->save();

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
    painter->setRenderHint(QPainter::Antialiasing);

    // 打印
    painter->setBrush(QColor("#409EFF"));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(printRect, 4, 4);

    painter->setPen(Qt::white);
    painter->drawText(printRect,
                      Qt::AlignCenter,
                      "标签打印");

    // 开工
    painter->setBrush(QColor("#67C23A"));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(startRect, 4, 4);

    painter->setPen(Qt::white);
    painter->drawText(startRect,
                      Qt::AlignCenter,
                      "开工");

    painter->restore();
}

bool OperationDelegate::editorEvent(
    QEvent *event,
    QAbstractItemModel *,
    const QStyleOptionViewItem &option,
    const QModelIndex &index)
{
    if (event->type() != QEvent::MouseButtonRelease)
        return false;

    auto* mouseEvent = static_cast<QMouseEvent*>(event);

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

    if (printRect.contains(pos))
    {
        emit sigPrintClicked(index.row());
    }

    if (startRect.contains(pos))
    {
        emit sigStartClicked(index.row());
    }

    return true;
}
