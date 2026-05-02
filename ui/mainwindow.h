#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once

#include <QWidget>
#include <QStackedWidget>

class HeaderWidget;
class SideBarWidget;
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void initUI();
    void initConnect();

private slots:
    void onPageChanged(int index);

private:
    HeaderWidget* m_header{nullptr};
    SideBarWidget* m_sidebar{nullptr};
    QStackedWidget* m_stack{nullptr};
};

#endif // MAINWINDOW_H
