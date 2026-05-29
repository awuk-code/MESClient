#include "sidebarwidget.h"
#include <QDebug>
#include "configmanager.h"
SideBarWidget::SideBarWidget(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("SideBarWidget");
    initUI();
    initConnect();
}

void SideBarWidget::initUI()
{
    setMinimumWidth(35);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 10, 5, 10);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop);

    m_group = new QButtonGroup(this);
    m_group->setExclusive(true);


    layout->addWidget(createMenuItem(":/res/menu/menu.svg", ":/res/menu/menu.svg", QStringLiteral("menu"), QStringLiteral("hover"), 0));
    layout->addSpacing(0);

    layout->addWidget(createMenuItem(":/res/menu/task1.svg", ":/res/menu/task2.svg", tr("生产任务"), tr("生产任务hover"), 1));
    layout->addSpacing(15);
    layout->addWidget(createMenuItem(":/res/menu/process1.svg", ":/res/menu/process2.svg", tr("工序站点"), tr("工序站点hover"), 2));
    layout->addSpacing(15);
    layout->addWidget(createMenuItem(":/res/menu/repair1.svg", ":/res/menu/repair2.svg", tr("维修站"), tr("维修站hover"), 3));

    layout->addStretch();

    layout->addWidget(createMenuItem(":/res/menu/set.svg", ":/res/menu/set.svg", tr("设置"), tr("设置"), 19));
    layout->addSpacing(5);
    layout->addWidget(createMenuItem(":/res/menu/exit.svg", ":/res/menu/exit.svg", QStringLiteral("exit"), QStringLiteral("exit"), 20));


    // layout->addWidget(m_setBtn);
    // layout->addSpacing(5);
    // layout->addWidget(m_exitBtn);

}

void SideBarWidget::initConnect()
{
    connect(m_group, &QButtonGroup::buttonClicked, this, [this](QAbstractButton* clickedBtn){
        int id =m_group->id(clickedBtn);

        if(id != 0 && id <= maxMenuCnt-2){
            qDebug() << __FUNCTION__"id===="<<id;
            emit sigPageChanged(id);
        }
        if(maxMenuCnt==id){
             ConfigManager::instance().saveConfig();
            if(window())
                window()->close();
        }
        qDebug() << __FUNCTION__ << tr("点击菜单：") << m_group->id(clickedBtn);

        for(QAbstractButton* btn : m_group->buttons()){
            QString path = (btn == clickedBtn) ? ("icon_checked") : ("icon_normal");
            btn->setIcon(QIcon(btn->property(path.toStdString().c_str()).toString()));
        }
    });
}

void SideBarWidget::setCurrentPageIndex(int index)
{
    if (!m_group)
        return;

    auto checkedBtn = m_group->button(index);
    if (!checkedBtn)
        return;

    checkedBtn->setChecked(true);

    for (QAbstractButton* btn : m_group->buttons())
    {
        const QString path =
            (btn == checkedBtn) ? QStringLiteral("icon_checked")
                                : QStringLiteral("icon_normal");
        btn->setIcon(QIcon(btn->property(path.toStdString().c_str()).toString()));
    }

    qDebug() << __FUNCTION__ << tr("同步侧边栏选中菜单:") << index;
}

QWidget *SideBarWidget::createMenuItem(const QString &icon_normal, const QString &icon_checked, const QString &text, const QString &hoverText, int id)
{
    qDebug() << __FUNCTION__"current Btn id = "<<id;
    QToolButton* btn = new QToolButton(this);
    btn->setObjectName("menuBtn");
    btn->setCheckable(true);
    btn->setToolTip(hoverText);

    btn->setProperty("icon_normal", icon_normal);
    btn->setProperty("icon_checked", icon_checked);
    btn->setProperty("menuID", id);

    if(id==0 || id > maxMenuCnt-2){
        btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        btn->setFixedSize(60, 40);
    }else{
        btn->setText(text);
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setFixedSize(60,60);
    }

    btn->setIconSize(QSize(24,24));

    if (id == 1){
        btn->setIcon(QIcon(icon_checked));
        btn->setChecked(true);
    }   else{
        btn->setIcon(QIcon(icon_normal));
    }
    btn->setStyleSheet("border:none; background:transparent;");

    m_group->addButton(btn, id);


    return btn;
}
