#ifndef CUSTOMHEADERVIEW_H
#define CUSTOMHEADERVIEW_H

#include <QHeaderView>
#include <QSet>

class CustomHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    explicit CustomHeaderView(Qt::Orientation orientation, QWidget* parent = nullptr );
    void setFilterFields(const QSet<QString >& fields);

protected:
    void paintSection(QPainter* painter,
                      const QRect &rect,
                      int logicalIndex) const override;

private:
    QSet<QString> m_fielterFields;
};

#endif // CUSTOMHEADERVIEW_H
