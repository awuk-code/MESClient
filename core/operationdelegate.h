#ifndef OPERATIONDELEGATE_H
#define OPERATIONDELEGATE_H

#include <QPainter>
#include <QMouseEvent>
#include <QStyleOptionButton>

#include "baseitemdelegate.h"

class OperationDelegate : public BaseItemDelegate
{
    Q_OBJECT
public:
    explicit OperationDelegate(QObject *parent = nullptr);

     bool updateRowData(QAbstractItemModel* model,const QModelIndex& index, const QVariantMap& rowData);
     static int minimumColumnWidth();


 signals:
    void sigPrintClicked(int row);

    void sigStartClicked(int row);
    void sigStartClicked(const QVariantMap& rowData);

public:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;
};

#endif // OPERATIONDELEGATE_H
