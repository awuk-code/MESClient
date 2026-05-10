#include "checkboxdelegate.h"

CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}

void CheckBoxDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    Q_UNUSED(painter)

    Qt::CheckState state =
        static_cast<Qt::CheckState>(
            index.data(Qt::CheckStateRole).toInt());

    QStyleOptionButton checkbox;
    checkbox.state |= QStyle::State_Enabled;

    if (state == Qt::Checked)
        checkbox.state |= QStyle::State_On;
    else
        checkbox.state |= QStyle::State_Off;

    QRect indicator =
        QApplication::style()->subElementRect(
            QStyle::SE_CheckBoxIndicator,
            &checkbox);

    checkbox.rect = QRect(
        option.rect.x() +
            (option.rect.width() - indicator.width()) / 2,
        option.rect.y() +
            (option.rect.height() - indicator.height()) / 2,
        indicator.width(),
        indicator.height());

    QApplication::style()->drawControl(
        QStyle::CE_CheckBox,
        &checkbox,
        painter);
}

bool CheckBoxDelegate::editorEvent(QEvent *event,
                                   QAbstractItemModel *model,
                                   const QStyleOptionViewItem &,
                                   const QModelIndex &index)
{
    if (event->type() != QEvent::MouseButtonRelease)
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
