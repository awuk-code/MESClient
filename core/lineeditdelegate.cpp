#include "lineeditdelegate.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QStyle>

LineEditDelegate::LineEditDelegate(QObject *parent)
    : BaseItemDelegate{parent}
{}

// 根据当前列标题生成输入提示，后续新增 LineEdit 列可自动使用自己的列名。
static QString lineEditPlaceholderText(const QModelIndex &index)
{
    QString columnTitle;
    if (index.model())
    {
        columnTitle = index.model()
                          ->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole)
                          .toString();
    }

    return columnTitle.isEmpty()
               ? LineEditDelegate::tr("请输入或扫入")
               : LineEditDelegate::tr("请输入或扫入%1").arg(columnTitle);
}

QWidget *LineEditDelegate::createEditor(
    QWidget *parent,
    const QStyleOptionViewItem &,
    const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setFrame(false);
    editor->setAlignment(Qt::AlignCenter);
    // 进入编辑状态后，由 QLineEdit 自身显示占位提示。
    editor->setPlaceholderText(lineEditPlaceholderText(index));
    return editor;
}

void LineEditDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    const QString placeholderText = lineEditPlaceholderText(index);
    const QString text = index.data(Qt::DisplayRole).toString();
    if (!text.isEmpty())
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    // 未编辑且内容为空时，在表格单元格里直接绘制占位提示。
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.text = placeholderText;
    opt.palette.setColor(
        QPalette::Text,
        opt.palette.color(QPalette::PlaceholderText));
    opt.palette.setColor(
        QPalette::HighlightedText,
        opt.palette.color(QPalette::PlaceholderText));

    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
}

QSize LineEditDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    const QString text = index.data(Qt::DisplayRole).toString();
    if (text.isEmpty())
    {
        QFontMetrics fm(option.font);
        size.setWidth(qMax(size.width(), fm.horizontalAdvance(lineEditPlaceholderText(index)) + 16));
    }

    return size;
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
