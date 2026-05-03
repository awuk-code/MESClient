#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once

#include <QWidget>
#include <QStackedWidget>
#include "headerwidget.h"
#include "subheaderwidget.h"
#include "sidebarwidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void initUI();
    void initConnect();
    void closeEvent(QCloseEvent* event)override;

private slots:
    void onPageChanged(int index/*, QString &title*/);

private:
    HeaderWidget* m_header{nullptr};
    SideBarWidget* m_sidebar{nullptr};
    SubHeaderWidget* m_subHeader{nullptr};
    QStackedWidget* m_stack{nullptr};
};

#endif // MAINWINDOW_H
