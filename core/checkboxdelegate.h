#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H
/**
 * 复选框列代理
 *
 * */

#include <QStyledItemDelegate>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionViewItem>

#include "baseitemdelegate.h"

class CheckBoxDelegate : public BaseItemDelegate
{
    Q_OBJECT
public:
    explicit CheckBoxDelegate(QObject *parent = nullptr);

    static int indicatorSize(const QFont& font);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;


};

#endif // CHECKBOXDELEGATE_H
