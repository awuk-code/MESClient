#include "headerwidget.h"

#include <QSystemTrayIcon>

HeaderWidget::HeaderWidget(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("HeaderWidget");
    setAttribute(Qt::WA_StyledBackground, true);
    initUI();
    initConnect();
}

void HeaderWidget::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}

void HeaderWidget::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}


void HeaderWidget::initUI()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_userIcon = new QLabel(this);

    m_userIcon->setFixedSize(32, 32);
    m_userIcon->setAlignment(Qt::AlignCenter);

    m_userIcon->setPixmap(QIcon(":/res/common/user.svg").pixmap(20, 20));
    m_userInfo = new QLabel("xxx121381238 ",this);
    m_userInfo->setToolTip(tr("用户信息"));
    m_title = new QLabel(tr("生产执行系统"), this);

    m_btnMin = new QPushButton(this);
    m_btnMin->setIcon(QIcon(":/res/common/min.svg"));
    m_btnMax = new QPushButton(this);
    m_btnMax->setIcon(QIcon(":/res/common/max2.svg"));
    m_btnClose = new QPushButton(this);
    m_btnClose->setIcon(QIcon(":/res/common/close.svg"));

    m_btnMin->setFixedSize(30, 30);
    m_btnMax->setFixedSize(30, 30);
    m_btnClose->setFixedSize(30, 30);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(15,0,10,0);
    layout->setSpacing(0);

    // ===== 左区域 =====
    QWidget* leftWidget = new QWidget(this);
    QHBoxLayout* leftLayout = new QHBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0,0,0,0);
    leftLayout->addWidget(m_userIcon);
    leftLayout->addWidget(m_userInfo);
    leftLayout->addStretch();

    // ===== 中区域 =====
    QWidget* centerWidget = new QWidget(this);
    QHBoxLayout* centerLayout = new QHBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0,0,0,0);
    centerLayout->addWidget(m_title);
    centerLayout->setAlignment(Qt::AlignCenter);

    // ===== 右区域 =====
    QWidget* rightWidget = new QWidget(this);
    QHBoxLayout* rightLayout = new QHBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0,0,0,0);
    rightLayout->addStretch();
    rightLayout->addWidget(m_btnMin);
    rightLayout->addWidget(m_btnMax);
    rightLayout->addWidget(m_btnClose);

    // ===== 主布局 =====
    layout->addWidget(leftWidget);
    layout->addWidget(centerWidget);
    layout->addWidget(rightWidget);

    // ⭐ 关键：三等分
    layout->setStretch(0, 1);
    layout->setStretch(1, 1);
    layout->setStretch(2, 1);
}

void HeaderWidget::initConnect()
{

    connect(m_btnClose, &QPushButton::clicked, this, [this](){
        window()->close();
    });
    m_trayIcon = new QSystemTrayIcon(QIcon(":/res/common/user.svg"), this);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this,
            [this](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::Trigger
                    || reason == QSystemTrayIcon::DoubleClick) {
                    window()->showMaximized();
                    window()->raise();
                    window()->activateWindow();
                }
            });
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        m_trayIcon->show();
    }

    connect(m_btnMin, &QPushButton::clicked, this, [this](){
        if (m_trayIcon && QSystemTrayIcon::isSystemTrayAvailable()) {
            window()->hide();
            return;
        }
        window()->showMinimized();
    });;
    connect(m_btnMax, &QPushButton::clicked, this, [this](){
        window()->showMaximized();
        m_btnMax->setIcon(QIcon(":/res/common/max2.svg"));
    });
}
