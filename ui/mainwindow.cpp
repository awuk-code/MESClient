#include "mainwindow.h"
#include "configmanager.h"
#include "MDebug.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("MainWindow");
    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint);
    resize(1200, 800);
    initUI();
    initConnect();
}

void MainWindow::initUI()
{
    m_header = new HeaderWidget(this);
    m_sidebar = new SideBarWidget(this);

    m_subHeader = new SubHeaderWidget(this);
    m_subHeader->setFixedHeight(40);
    m_stack = new QStackedWidget(this);

    m_pageProduction = new ProductionTaskPage(this);
    //注册页面
    NavigationManager::instance()->registerPage(
        PageType::ProductionTask,
        m_pageProduction);


    m_stack->addWidget(m_pageProduction); //id = 0

    m_pageProcess = new ProcessStationPage(this);

    m_pageProcess->setReworkTaskMode(false);


    m_stack->addWidget(m_pageProcess);
    NavigationManager::instance()->registerPage(
        PageType::ProcessStation,
        m_pageProcess);
    m_pageRepairStation = new RepairStationPage(this);

    m_stack->addWidget(m_pageRepairStation);
    NavigationManager::instance()->registerPage(
        PageType::RepairStation,
        m_pageRepairStation);

    m_pageRepairJudge = new RepairJudgePage(this);
    m_stack->addWidget(m_pageRepairJudge);
    NavigationManager::instance()->registerPage(
        PageType::RepairJudge,
        m_pageRepairJudge);

    m_pageReworkTask = new ProcessStationPage(this);
    m_pageReworkTask->setReworkTaskMode(true);
    m_stack->addWidget(m_pageReworkTask);
    NavigationManager::instance()->registerPage(
        PageType::ReworkTask,
        m_pageReworkTask);

    m_stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ===== 下半部分（左 Sidebar + 右 Central）=====
    QWidget* rightWidget = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0,0,0,0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(m_subHeader);
    rightLayout->addWidget(m_stack);

    QWidget* bottomWidget = new QWidget(this);
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(0);
    bottomLayout->addWidget(m_sidebar);
    bottomLayout->addWidget(rightWidget);

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
    //m_sidebar->setFixedWidth(80);

    setLayout(mainLayout);
    updateSubHeaderNavigation(1);
}

void MainWindow::initConnect()
{
    connect(m_sidebar, &SideBarWidget::sigPageChanged,
            this, &MainWindow::onPageChanged);

    //   connect(ProcessStationRightPanel, &ProcessStationRightPanel::requestOpenPage, this, &MainWindow::openPage);

    connect(NavigationManager::instance(), &NavigationManager::sigOpenPage,
             this, &MainWindow::onOpenPage);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ConfigManager::instance().saveConfig();
    funcDebug() <<"关闭窗口写入时间";
    event->accept();
}

void MainWindow::onPageChanged(int index/*, QString &title*/)
{
    funcDebug()<<"收到页面切换信号index= "<<index;
    m_stack->setCurrentIndex(index-1);
    updateSubHeaderNavigation(index);
}

void MainWindow::updateSubHeaderNavigation(int index)
{
    if (!m_subHeader)
        return;

    m_subHeader->setProcessNavigationVisible(false);

    switch (index)
    {
    case 1:
        m_subHeader->setPageNavigation({QStringLiteral("生产任务"), QStringLiteral("生产任务列表")});
        break;

    case 2:
        m_subHeader->setPageNavigation({QStringLiteral("工序站点")});
        m_subHeader->setProcessNavigationVisible(true);
        // TODO: 当前工序需要通过接口获取，接口返回后调用
        // m_subHeader->setCurrentProcessName(processName) 刷新这里的工序导航。
        m_subHeader->setCurrentProcessName(QString());
        break;

    case 3:
        m_subHeader->setPageNavigation({QStringLiteral("维修站"), QStringLiteral("维修站列表")});
        break;

    default:
        m_subHeader->setPageNavigation({QStringLiteral("未知页面")});
        break;
    }

}

void MainWindow::updateSubHeaderNavigation(PageType type)
{
    if (!m_subHeader)
        return;

    m_subHeader->setProcessNavigationVisible(false);

    switch (type)
    {
    case PageType::ProductionTask:
        m_subHeader->setPageNavigation({QStringLiteral("生产任务"), QStringLiteral("生产任务列表")});
        break;

    case PageType::ProcessStation:
        m_subHeader->setPageNavigation({QStringLiteral("工序站点")});
        m_subHeader->setProcessNavigationVisible(true);
        // TODO: 当前工序需要通过接口获取，接口返回后调用
        // m_subHeader->setCurrentProcessName(processName) 刷新这里的工序导航。
        m_subHeader->setCurrentProcessName(QString());
        break;

    case PageType::RepairStation:
        m_subHeader->setPageNavigation({QStringLiteral("维修站"), QStringLiteral("维修站列表")});
        break;

    case PageType::RepairJudge:
        m_subHeader->setPageNavigation({QStringLiteral("维修站"), QStringLiteral("维修判定")});
        break;

    case PageType::ReworkTask:
        m_subHeader->setPageNavigation({QStringLiteral("维修站"), QStringLiteral("返工任务单")});
        m_subHeader->setProcessNavigationVisible(true);
        // TODO: 当前工序需要通过接口获取，接口返回后调用
        // m_subHeader->setCurrentProcessName(processName) 刷新返工任务单工序导航。
        m_subHeader->setCurrentProcessName(QString());
        break;

    default:
        m_subHeader->setPageNavigation({QStringLiteral("未知页面")});
        break;
    }
}

void MainWindow::openPage(const QString &pageId)
{
    if(!m_pageMap.contains(pageId))
    {
        funcDebug() << "页面不存在:" << pageId;
        return;
    }
    QWidget* page = m_pageMap.value(pageId);

    m_stack->setCurrentWidget(page);

    funcDebug() << "打开页面:" << pageId;
}

void MainWindow::onOpenPage(PageType type)
{
    QWidget* page =
        NavigationManager::instance()->page(type);

    if (!page)
    {
        return;
    }

    if (type == PageType::RepairJudge && m_pageRepairJudge)
    {
        m_pageRepairJudge->setRepairData(
            NavigationManager::instance()->pageData(type));
    }
    else if (type == PageType::ProcessStation && m_pageProcess)
    {
        m_pageProcess->setProductionTaskData(
            NavigationManager::instance()->pageData(type));
    }

    m_stack->setCurrentWidget(page);
    updateSubHeaderNavigation(type);
}
