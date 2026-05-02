#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QLabel>

class SideBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SideBarWidget(QWidget *parent = nullptr);

signals:
    void sigPageChanged(int index);

private:
    void initUI();
    QWidget* createMenuItem(const QString& icon, const QString &iconName, const QString &hoverText, int id);


    QButtonGroup* m_group{nullptr};
    QPushButton* m_produceBtn{nullptr};
    QPushButton* m_processBtn{nullptr};
    QPushButton* m_repairBtn{nullptr};
};

#endif // SIDEBARWIDGET_H
