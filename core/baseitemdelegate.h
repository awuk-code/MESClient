#ifndef BASEITEMDELEGATE_H
#define BASEITEMDELEGATE_H

#include <QStyledItemDelegate>

class BaseItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BaseItemDelegate(QObject *parent = nullptr);
};

#endif // BASEITEMDELEGATE_H
