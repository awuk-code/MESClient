#include "sidebarwidget.h"

SideBarWidget::SideBarWidget(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("SideBarWidget");
    initUI();
}

void SideBarWidget::initUI()
{

    setFixedWidth(110);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 10, 0, 10);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop);

    m_group = new QButtonGroup(this);
    layout->addWidget(createMenuItem(":res/menu/task1.svg", QStringLiteral("生产任务"), QStringLiteral("生产任务"), 0));
    layout->addWidget(createMenuItem(":res/menu/process1.svg", QStringLiteral("工序站点"), QStringLiteral("工序站点"), 1));
    layout->addWidget(createMenuItem(":res/menu/repair1.svg", QStringLiteral("维修站"), QStringLiteral("维修站"), 2));
    layout->addStretch();
    setStyleSheet(R"(
    #SideBarWidget {
        background-color: #001529;
        border-right: 2px solid #1890ff; /* 蓝色更明显，你可以改颜色 */
        border-top: none;
        border-left: none;
        border-bottom: none;
    }
    #menuBtn {
        background-color: transparent;
        border: none;
    }
)");
    connect(m_group, &QButtonGroup::buttonClicked, this, [this](QAbstractButton* btn){
        emit sigPageChanged(m_group->id(btn));
    });
}

QWidget *SideBarWidget::createMenuItem(const QString &icon, const QString &iconName, const QString &hoverText, int id)
{
    QPushButton* btn = new QPushButton(this);
    btn->setCheckable(true);
    btn->setObjectName("menuBtn");
    btn->setFixedSize(100, 80);
    btn->setToolTip(hoverText);
   // btn->setStyleSheet("border:none; background:transparent;");

    if (id == 0) btn->setChecked(true);

    m_group->addButton(btn, id);

    // ===== 内部布局 =====
    QVBoxLayout* layout = new QVBoxLayout(btn);
    layout->setContentsMargins(0,8,0,8);
    layout->setSpacing(5);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* iconLabel = new QLabel(btn);
    iconLabel->setPixmap(QIcon(icon).pixmap(18,18));
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel* textLabel = new QLabel(iconName, btn);

    layout->addWidget(iconLabel);
    layout->addWidget(textLabel);

    return btn;
}
