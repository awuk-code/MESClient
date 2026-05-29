#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once

#include <QWidget>
#include <QStackedWidget>
#include <QMouseEvent>

#include "headerwidget.h"
#include "subheaderwidget.h"
#include "sidebarwidget.h"
#include "productiontaskpage.h"
#include "processstationpage.h"
#include "repairjudgepage.h"
#include "repairstationpage.h"

#include "navigationmanager.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event)override;
private:
    void initUI();
    void initConnect();
    void updateSubHeaderNavigation(int index);
    void updateSubHeaderNavigation(PageType type);
    void updateSidebarSelection(PageType type);


private slots:
    //stack的页面跳转
    void onPageChanged(int index/*, QString &title*/);
    //文本链接的跳转
    void openPage(const QString& pageId);
    void onOpenPage(PageType type);
private:
    QPoint m_dragPos;           // 记录鼠标按下时的位置
    bool m_isResizing = false;  // 是否正在拉伸

    HeaderWidget* m_header{nullptr};
    SideBarWidget* m_sidebar{nullptr};
    SubHeaderWidget* m_subHeader{nullptr};
    QStackedWidget* m_stack{nullptr};

    ProductionTaskPage* m_pageProduction{nullptr};
    ProcessStationPage* m_pageProcess{nullptr};
    RepairStationPage* m_pageRepairStation{nullptr};
    RepairJudgePage* m_pageRepairJudge{nullptr};
    ProcessStationPage* m_pageReworkTask{nullptr};


    QMap<QString, QWidget*> m_pageMap;
};

#endif // MAINWINDOW_H
