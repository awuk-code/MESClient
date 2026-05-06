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


    m_stack->addWidget(m_pageProduction); //id = 0
    m_stack->addWidget(new QLabel(QStringLiteral("任务界面2")));
    m_stack->addWidget(new QLabel(QStringLiteral("任务界面3")));
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

    m_sizeBtn = new QPushButton("/",this);
    m_sizeBtn->setFixedSize(16,16);
    m_sizeBtn->setCursor(Qt::SizeFDiagCursor);
    m_sizeBtn->setStyleSheet("QPushButton{ background: transparent; border:none; }");
    m_sizeBtn->installEventFilter(this);
}

void MainWindow::initConnect()
{
    connect(m_sidebar, &SideBarWidget::sigPageChanged,
            this, &MainWindow::onPageChanged);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ConfigManager::instance().saveConfig();
    funcDebug() <<"关闭窗口写入时间";
    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_sizeBtn->move(this->width() - m_sizeBtn->width(),
                    this->height() - m_sizeBtn->height());
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_sizeBtn){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            if(mouseEvent->button() == Qt::LeftButton){
                m_isResizing = true;
                m_dragPos = mouseEvent->globalPos();
                return true;
            }
            break;
        case QEvent::MouseMove:
            if (m_isResizing) {
                QPoint delta = mouseEvent->globalPos() - m_dragPos;

                int newWidth = this->width() + delta.x();
                int newHeight = this->height() + delta.y();

                if (newWidth >= minimumWidth() && newHeight >= minimumHeight()) {
                    this->resize(newWidth, newHeight);
                    m_dragPos = mouseEvent->globalPos(); // 更新参考点
                }
                return true;
            }
            break;
        case QEvent::MouseButtonRelease:
            m_isResizing = false;
            return true;

        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void MainWindow::onPageChanged(int index/*, QString &title*/)
{
    funcDebug()<<"收到页面切换信号index= "<<index;
    m_stack->setCurrentIndex(index-1);
}
