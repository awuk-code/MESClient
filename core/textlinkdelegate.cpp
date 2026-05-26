#include "textlinkdelegate.h"
#include "commonfunc.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStyle>
#include <QApplication>
#include <QFontMetrics>

TextLinkDelegate::TextLinkDelegate(QObject *parent)
    : BaseItemDelegate{parent}
{}

QRect TextLinkDelegate::textRect(const QStyleOptionViewItem &option, const QString &text) const
{
    // =========================
    // 1. 字体度量
    // =========================
    QFontMetrics fm(option.font);

    // =========================
    // 2. 计算文本尺寸
    // =========================
    QRect textBounding =
        fm.boundingRect(text);

    int textWidth =
        textBounding.width();

    int textHeight =
        textBounding.height();

    // =========================
    // 3. 居中计算
    // =========================
    int x =
        option.rect.x()
        + (option.rect.width() - textWidth) / 2;

    int y =
        option.rect.y()
        + (option.rect.height() - textHeight) / 2;

    return QRect(
        x,
        y,
        textWidth,
        textHeight);
}

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

    QString text =
        index.data(Qt::DisplayRole).toString();
    // =========================
    // 设置字体（保持默认）
    // =========================
    painter->setFont(option.font);

    // =========================
    // 设置文字颜色为红色
    // =========================
    painter->setPen(Qt::red);

    QRect rect =
        textRect(option, text);

    painter->drawText(
        option.rect,
        Qt::AlignCenter,
        text);

    painter->restore();
}

bool TextLinkDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{

    // 只处理鼠标左键释放事件
    if (event->type() != QEvent::MouseButtonRelease)
    {
        return QStyledItemDelegate::editorEvent(
            event,
            model,
            option,
            index);
    }

    QMouseEvent *mouseEvent =
        static_cast<QMouseEvent *>(event);

    if (mouseEvent->button() != Qt::LeftButton)
    {
        return false;
    }

    QString text =
        index.data(Qt::DisplayRole).toString();

    QRect rect = textRect(option, text);

    if (rect.contains(mouseEvent->pos()))
    {
        QVariant var = index.data(Qt::DisplayRole);
        if(!var.isValid() || var.toString().isEmpty()) return false;

        emit linkClicked(QPersistentModelIndex(index), text);
        return true;
    }

    return false;
}
