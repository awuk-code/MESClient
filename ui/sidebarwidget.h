#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QStyle>

class QAbstractButton;

class SideBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SideBarWidget(QWidget *parent = nullptr);
    void setCurrentPageIndex(int index);

signals:
    void sigPageChanged(int index);

private:
    void initUI();
    void initConnect();
    void onMenuButtonClicked(QAbstractButton* clickedBtn);
    QWidget* createMenuItem(const QString &icon_normal,
                            const QString &icon_checked,
                            const QString &text,
                            const QString &hoverText,
                            int id);

    QButtonGroup* m_group{nullptr};


    int maxMenuCnt = 20;
};

#endif // SIDEBARWIDGET_H
