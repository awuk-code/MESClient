#include "productiontaskpage.h"
#include "productiontaskmodel.h"
#include "fieldfilterproxymodel.h"

ProductionTaskPage::ProductionTaskPage(QWidget *parent)
    : BasePageWidget(parent)
{
    setupPage();
}

QAbstractItemModel* ProductionTaskPage::createModel()
{
    return new ProductionTaskModel(this);
}

TabConfigs ProductionTaskPage::Tabs() const
{
    return
        {
            {"未开工", PageType::TABLE, QVariant::fromValue(FilterStatus::WAITWORK)},
            {"已开工", PageType::TABLE,QVariant::fromValue(FilterStatus::ALREADY)},
            {"已完工", PageType::TABLE,QVariant::fromValue(FilterStatus::FINISH)},
        };
}

FieldFilterProxyModel* ProductionTaskPage::createProxy(const QVariant& data)
{
    auto proxy = new FieldFilterProxyModel(this);

    //已经在FieldFilterProxyModel设置了关键字过滤
   // proxy->setFilterColumn(0);  // 👉 按“任务单号”过滤
    proxy->setStatus(data); // 👉 Tab控制状态

    return proxy;
}

QString ProductionTaskPage::pageTitle() const
{
    return "生产任务列表";
}

QString ProductionTaskPage::searchInfo() const
{
    return "请输入表单号";
}

void ProductionTaskPage::addWidgetToTitle(QHBoxLayout *layout)
{
    ToggleSwitchWidget* m_switch = new ToggleSwitchWidget(this);
    layout->addStretch();
    layout->addWidget(m_switch);
}
