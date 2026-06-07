#include "checkboxdelegate.h"

#include <QFontMetrics>

namespace
{
QRect centeredIndicatorRect(const QRect& cellRect, const QFont& font)
{
    const int size = CheckBoxDelegate::indicatorSize(font);
    return QRect(cellRect.left() + (cellRect.width() - size) / 2,
                 cellRect.top() + (cellRect.height() - size) / 2,
                 size,
                 size);
}
}

CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
    : BaseItemDelegate{parent}
{}

int CheckBoxDelegate::indicatorSize(const QFont& font)
{
    QFontMetrics metrics(font);
    return qMax(18, metrics.height() + 2);
}

void CheckBoxDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    Qt::CheckState state =
        static_cast<Qt::CheckState>(
            index.data(Qt::CheckStateRole).toInt());

    const QRect boxRect =
        centeredIndicatorRect(option.rect, option.font);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const bool checked = state == Qt::Checked;
    const QColor borderColor = checked ? QColor("#880088") : QColor("#909399");
    const QColor fillColor = checked ? QColor("#880088") : QColor("#FFFFFF");
    const qreal penWidth = qMax<qreal>(1.5, boxRect.width() / 10.0);

    painter->setPen(QPen(borderColor, penWidth));
    painter->setBrush(fillColor);
    painter->drawRoundedRect(
        boxRect.adjusted(1, 1, -1, -1),
        3,
        3);

    if (checked)
    {
        QPen checkPen(Qt::white, qMax<qreal>(2.0, boxRect.width() / 7.0));
        checkPen.setCapStyle(Qt::RoundCap);
        checkPen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(checkPen);

        QPainterPath path;
        path.moveTo(boxRect.left() + boxRect.width() * 0.26,
                    boxRect.top() + boxRect.height() * 0.53);
        path.lineTo(boxRect.left() + boxRect.width() * 0.43,
                    boxRect.top() + boxRect.height() * 0.70);
        path.lineTo(boxRect.left() + boxRect.width() * 0.74,
                    boxRect.top() + boxRect.height() * 0.34);
        painter->drawPath(path);
    }

    painter->restore();
}

bool CheckBoxDelegate::editorEvent(QEvent *event,
                                   QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
    if (event->type() != QEvent::MouseButtonRelease)
        return false;

    auto* mouseEvent = static_cast<QMouseEvent*>(event);
    const QRect hitRect =
        centeredIndicatorRect(option.rect, option.font).adjusted(-6, -6, 6, 6);
    if (!hitRect.contains(mouseEvent->pos()))
        return false;

    Qt::CheckState state =
        static_cast<Qt::CheckState>(
            index.data(Qt::CheckStateRole).toInt());

    Qt::CheckState newState =
        (state == Qt::Checked)
            ? Qt::Unchecked
            : Qt::Checked;

    return model->setData(
        index,
        newState,
        Qt::CheckStateRole);
}

QSize CheckBoxDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    QSize size = BaseItemDelegate::sizeHint(option, index);
    const int indicator = indicatorSize(option.font);
    size.setWidth(qMax(size.width(), indicator + 24));
    size.setHeight(qMax(size.height(), indicator + 10));
    return size;
}
