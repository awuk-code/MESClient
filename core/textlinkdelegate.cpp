#include "textlinkdelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStyle>

TextLinkDelegate::TextLinkDelegate(QObject *parent)
    : BaseItemDelegate{parent}
{}

void TextLinkDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    // =========================
    // 绘制选中背景
    // =========================
    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(
            option.rect,
            option.palette.highlight());
    }

    // =========================
    // 设置字体（保持默认）
    // =========================
    painter->setFont(option.font);

    // =========================
    // 设置文字颜色为红色
    // =========================
    painter->setPen(Qt::red);

    // =========================
    // 绘制文本（居中）
    // =========================
    painter->drawText(
        option.rect,
        Qt::AlignCenter,
        index.data(Qt::DisplayRole).toString());

    painter->restore();
}

bool TextLinkDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model)
    Q_UNUSED(option)

    // 只处理鼠标左键释放事件
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEvent =
            static_cast<QMouseEvent *>(event);

        if (mouseEvent->button() == Qt::LeftButton)
        {
            emit linkClicked(
                index.row(),
                index.data(Qt::DisplayRole).toString());

            return true;    // 事件已处理
        }
    }

    return QStyledItemDelegate::editorEvent(
        event, model, option, index);
}
