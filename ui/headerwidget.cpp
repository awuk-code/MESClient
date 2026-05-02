#include "headerwidget.h"

HeaderWidget::HeaderWidget(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("HeaderWidget");
    setStyleSheet(R"(
    #HeaderWidget {
        background: white;
        border-bottom: 1px solid #e8e8e8;
    }
)");
    initUI();
    initConnect();
}

void HeaderWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        m_dragPos = event->globalPos() - window()->frameGeometry().topLeft();
}

void HeaderWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
        window()->move(event->globalPos() - m_dragPos);
}

void HeaderWidget::mouseReleaseEvent(QMouseEvent *event)
{

}

void HeaderWidget::initUI()
{
    m_userIcon = new QLabel(this);

    m_userIcon->setFixedSize(32, 32);
    m_userIcon->setAlignment(Qt::AlignCenter);

    m_userIcon->setPixmap(QIcon(":/res/common/user.svg").pixmap(20, 20));
    m_userInfo = new QLabel("xxx121381238 ",this);
    m_title = new QLabel(QStringLiteral("生产执行系统"), this);

    m_btnMin = new QPushButton(this);
    m_btnMin->setIcon(QIcon(":/res/common/min.svg"));
    m_btnMax = new QPushButton(this);
    m_btnMax->setIcon(QIcon(":/res/common/max1.svg"));
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
    connect(m_btnMin, &QPushButton::clicked, this, [this](){
        window()->showMinimized();
    });;
    connect(m_btnMax, &QPushButton::clicked, this, [this](){
        if(window()->isMaximized()){
            window()->showNormal();
            m_btnMax->setIcon(QIcon(":/res/common/max1.svg"));
        }else{
            window()->showMaximized();
            m_btnMax->setIcon(QIcon(":/res/common/max2.svg"));
        }
    });
}
