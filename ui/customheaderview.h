#ifndef CUSTOMHEADERVIEW_H
#define CUSTOMHEADERVIEW_H

#include <QHeaderView>
#include <QSet>

class CustomHeaderView : public QHeaderView
{
    Q_OBJECT
public:
   explicit CustomHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);
};

#endif // CUSTOMHEADERVIEW_H
