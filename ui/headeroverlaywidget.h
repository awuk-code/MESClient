#ifndef HEADEROVERLAYWIDGET_H
#define HEADEROVERLAYWIDGET_H

#include <QWidget>
#include <QHeaderView>
#include <QMouseEvent>
#include <QDateEdit>
#include <QPushButton>
#include "commonfunc.h"
#include "datefilterpopup.h"
#include <QMenu>
#include <QAction>
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


protected:
    void paintEvent(QPaintEvent *) override;
protected:
    void mousePressEvent(QMouseEvent *event) override;
private:
    void showDatePopup(const QRect& iconRect);
    void showPriorityPopup(const QRect &iconRect);

signals:
    //表头过滤信号
    void filterSelected(const QString &field, const QVariant &value);

private:
    QHeaderView *m_header = nullptr;
    QSet<QString> m_fields;
    QHash<int, QRect> m_iconRects;
    QHash<int, QString> m_columnFields;

    QString m_currentField; //记录当前点击字段

};

#endif // HEADEROVERLAYWIDGET_H
