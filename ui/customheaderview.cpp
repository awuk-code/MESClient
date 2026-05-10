#include "customheaderview.h"


CustomHeaderView::CustomHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    qDebug() << __FUNCTION__ <<"CustomHeaderView created";
    setDefaultAlignment(Qt::AlignCenter);
}

void CustomHeaderView::setFilterFields(const QSet<QString> &fields)
{
    m_fielterFields = fields;
    viewport()->update();
}

void CustomHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    QHeaderView::paintSection(painter, rect, logicalIndex);
}
