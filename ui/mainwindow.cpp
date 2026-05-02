#include "mainwindow.h"
#include "HeaderWidget.h"
#include "SidebarWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    setWindowFlags(Qt::FramelessWindowHint);
    resize(1200, 800);
    initUI();
    initConnect();

}

void MainWindow::initUI()
{
    m_header = new HeaderWidget(this);
    m_sidebar = new SideBarWidget(this);
    m_stack = new QStackedWidget(this);



    m_stack->addWidget(new QLabel(QStringLiteral("任务界面1")));
    m_stack->addWidget(new QLabel(QStringLiteral("任务界面2")));
    m_stack->addWidget(new QLabel(QStringLiteral("任务界面3")));
    m_stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ===== 下半部分（左 Sidebar + 右 Central）=====
    QWidget* bottomWidget = new QWidget(this);
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(0);

    bottomLayout->addWidget(m_sidebar);
    bottomLayout->addWidget(m_stack);

    // Sidebar 固定，Central 自适应
    bottomLayout->setStretch(0, 0);
    bottomLayout->setStretch(1, 1);

    // ===== 主布局（上 Header，下内容）=====
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_header);
    mainLayout->addWidget(bottomWidget);

    // Header 固定高度
    m_header->setFixedHeight(50);
    // Sidebar 固定宽度
    m_sidebar->setFixedWidth(120);

    setLayout(mainLayout);
}

void MainWindow::initConnect()
{
    connect(m_sidebar, &SideBarWidget::sigPageChanged,
            this, &MainWindow::onPageChanged);
}

void MainWindow::onPageChanged(int index)
{
    m_stack->setCurrentIndex(index);
}
