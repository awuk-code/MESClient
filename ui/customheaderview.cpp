#include "customheaderview.h"

CustomHeaderView::CustomHeaderView(
    Qt::Orientation orientation,
    QWidget *parent)
    : QHeaderView(orientation, parent)
{
    setDefaultAlignment(Qt::AlignCenter);
}
