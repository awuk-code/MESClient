#include "productiontaskpage.h"
#include "productiontaskmodel.h"
#include "fieldfilterproxymodel.h"
#include "navigationmanager.h"
#include "searchconfig.h"
#include "toggleswitchwidget.h"

#include <QHBoxLayout>
#include <QStackedWidget>
#include <QTableView>
#include <QDebug>

ProductionTaskPage::ProductionTaskPage(QWidget *parent)
    : BasePageWidget(parent)
{
    setupPage();

    auto model = dynamic_cast<ProductionTaskModel*>(m_model);
    if (model && model->opDelegate)
    {
        connect(model->opDelegate, QOverload<int>::of(&OperationDelegate::sigStartClicked),
                this, &ProductionTaskPage::startProcessByProxyRow);
        connect(model->opDelegate, QOverload<const QVariantMap&>::of(&OperationDelegate::sigStartClicked),
                this, &ProductionTaskPage::startProcessByRowData);
    }
}

QAbstractItemModel* ProductionTaskPage::createModel()
{
    return new ProductionTaskModel(this);
}

TabConfigs ProductionTaskPage::Tabs() const
{
    return
        {
            {tr("未开工"), PageDisplayType::TABLE, QVariant::fromValue(FilterStatus::WAITWORK)},
            {tr("已开工"), PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::ALREADY)},
            {tr("已完工"), PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::FINISH)},
        };
}

FieldFilterProxyModel* ProductionTaskPage::createProxy(const QVariant& data)
{
    auto proxy = new FieldFilterProxyModel(this);
    SearchConfig::apply(proxy, SearchConfig::productionTaskModule());

    //已经在FieldFilterProxyModel设置了关键字过滤
   // proxy->setFilterColumn(0);  // 👉 按“任务单号”过滤
    proxy->setStatus(data); // 👉 Tab控制状态

    return proxy;
}

QString ProductionTaskPage::pageTitle() const
{
    return tr("生产任务列表");
}

QString ProductionTaskPage::searchInfo() const
{
    return SearchConfig::placeholder(SearchConfig::productionTaskModule());
}

void ProductionTaskPage::addWidgetToTitle(QHBoxLayout *layout)
{
    ToggleSwitchWidget* m_switch = new ToggleSwitchWidget(this);
    layout->addStretch();
    layout->addWidget(m_switch);
}

void ProductionTaskPage::startProcessByProxyRow(int proxyRow)
{
    Q_UNUSED(proxyRow)
}

void ProductionTaskPage::startProcessByRowData(const QVariantMap &rowData)
{
    auto table =
        qobject_cast<QTableView*>(m_stack ? m_stack->currentWidget() : nullptr);
    if (!table)
        return;

    if (rowData.isEmpty())
        return;

    qDebug() << __FUNCTION__
             << "start process taskNo =" << rowData.value("taskNo").toString()
             << "productModel =" << rowData.value("productModel").toString();

    // 开工按钮点击后进入工序站点页面；后续如果需要调用开工接口，可在跳转前补充接口请求。
    NavigationManager::instance()->openPage(
        PageType::ProcessStation,
        rowData);
}
