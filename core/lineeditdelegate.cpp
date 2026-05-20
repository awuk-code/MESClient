#include "lineeditdelegate.h"

LineEditDelegate::LineEditDelegate(QObject *parent)
    : BaseItemDelegate{parent}
{}

QWidget *LineEditDelegate::createEditor(
    QWidget *parent,
    const QStyleOptionViewItem &,
    const QModelIndex &) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setFrame(false);
    editor->setAlignment(Qt::AlignCenter);
    return editor;
}

void LineEditDelegate::setEditorData(
    QWidget *editor,
    const QModelIndex &index) const
{
    QString value = index.data(Qt::EditRole).toString();

    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (lineEdit)
        lineEdit->setText(value);
}

void LineEditDelegate::setModelData(
    QWidget *editor,
    QAbstractItemModel *model,
    const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (lineEdit)
        model->setData(index, lineEdit->text(), Qt::EditRole);
}

void LineEditDelegate::updateEditorGeometry(
    QWidget *editor,
    const QStyleOptionViewItem &option,
    const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
