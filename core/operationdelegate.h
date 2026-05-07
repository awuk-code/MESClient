#ifndef OPERATIONDELEGATE_H
#define OPERATIONDELEGATE_H

#include <QPainter>
#include <QMouseEvent>

#include "baseitemdelegate.h"

class OperationDelegate : public BaseItemDelegate
{
    Q_OBJECT
public:
    explicit OperationDelegate(QObject *parent = nullptr);

signals:
    void sigPrintClicked(int row);

    void sigStartClicked(int row);

public:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;
};

#endif // OPERATIONDELEGATE_H
