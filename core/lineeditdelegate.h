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
