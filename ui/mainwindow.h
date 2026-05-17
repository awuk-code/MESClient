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
#include "toggleswitchwidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event)override;
    void resizeEvent(QResizeEvent* event)override;
    bool eventFilter(QObject* obj, QEvent* event)override;
private:
    void initUI();
    void initConnect();


private slots:
    void onPageChanged(int index/*, QString &title*/);

private:
    QPoint m_dragPos;           // 记录鼠标按下时的位置
    bool m_isResizing = false;  // 是否正在拉伸

    HeaderWidget* m_header{nullptr};
    SideBarWidget* m_sidebar{nullptr};
    SubHeaderWidget* m_subHeader{nullptr};
    QStackedWidget* m_stack{nullptr};

    ProductionTaskPage* m_pageProduction{nullptr};



    QPushButton* m_sizeBtn{nullptr};
};

#endif // MAINWINDOW_H
