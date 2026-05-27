#ifndef LINEEDITDELEGATE_H
#define LINEEDITDELEGATE_H

#include "baseitemdelegate.h"
//lineedit代理
#include <QStyledItemDelegate>
#include <QLineEdit>

class LineEditDelegate : public BaseItemDelegate
{
    Q_OBJECT
public:
    explicit LineEditDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    // 空的表格输入单元格未进入编辑状态时，也绘制占位提示文字。
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override;

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
};


#endif // LINEEDITDELEGATE_H
