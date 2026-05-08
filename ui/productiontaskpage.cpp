#include "productiontaskpage.h"
#include "productiontaskmodel.h"
#include "fieldfilterproxymodel.h"

ProductionTaskPage::ProductionTaskPage(QWidget *parent)
    : BasePageWidget(parent)
{
    setupPage();
    qDebug() << "ProductionTaskPage ctor";
}

QAbstractItemModel* ProductionTaskPage::createModel()
{
    return new ProductionTaskModel(this);
}



QVector<TabConfig> ProductionTaskPage::tabs() const
{
    return
        {
            {"未开工", 0},
            {"已开工", 1},
            {"已完工", 2},
            {"已完工", 2}
        };
}

FieldFilterProxyModel* ProductionTaskPage::createProxy(int tabIndex)
{
    auto proxy = new FieldFilterProxyModel(this);

    proxy->setFilterColumn(0);  // 👉 按“任务单号”过滤
    proxy->setStatus(tabIndex); // 👉 Tab控制状态

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
