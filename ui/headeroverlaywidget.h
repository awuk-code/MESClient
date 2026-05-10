#ifndef HEADEROVERLAYWIDGET_H
#define HEADEROVERLAYWIDGET_H

#include <QWidget>
#include <QHeaderView>
#include <QMouseEvent>
#include <QDateEdit>
#include <QPushButton>
#include "commonfunc.h"
#include "datefilterpopup.h"

class HeaderOverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeaderOverlayWidget(QHeaderView *header,QWidget *parent = nullptr);
    void setFilterFields(const QSet<QString> &fields);
    QRect calcIconRect(int column) const;
    void setColumnFields(const QHash<int, QString>& map)
    {
        m_columnFields = map;
    }
    void showDatePopup(const QRect& iconRect);

protected:
    void paintEvent(QPaintEvent *) override;
protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QHeaderView *m_header = nullptr;
    QSet<QString> m_fields;
QHash<int, QRect> m_iconRects;
    QHash<int, QString> m_columnFields;

};

#endif // HEADEROVERLAYWIDGET_H
