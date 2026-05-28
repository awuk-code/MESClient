#include "repairstationpage.h"
#include "repairstationmodel.h"
#include "navigationmanager.h"

RepairStationPage::RepairStationPage(QWidget *parent)
    : BasePageWidget(parent)
{
    setObjectName("RepairStationPage");
    setupPage();
    connect(tabBar(), &QTabBar::currentChanged,
            this, &RepairStationPage::updateRepairJudgeButton);
    updateRepairJudgeButton();

    auto model = qobject_cast<RepairStationModel*>(m_model);
    if(model)
    {
        auto delegate = model->textLinkDelegate();
        if(delegate)
        {
            connect(delegate, &TextLinkDelegate::linkClicked,
                    this, [=](const QPersistentModelIndex& proxyIndex, const QString &text){
                        Q_UNUSED(text);

                        auto proxy =
                            qobject_cast<const FieldFilterProxyModel*>(
                                proxyIndex.model());

                        if (!proxy)
                            return;
                        // 映射回 source model
                        QModelIndex sourceIndex =
                            proxy->mapToSource(proxyIndex);
                        if (!sourceIndex.isValid())
                            return;

                        int row = sourceIndex.row();

                        QVariantMap rowData =
                            model->rowData(row);

                        const QString field =
                            model->columnField(sourceIndex.column());

                        QString exceptionNo =
                            rowData.value("exceptionHandleNo").toString();

                        if(field == "exceptionHandleNoLink")
                        {
                            openRepairJudgePage(rowData);
                        }
                        else if(field == "reworkTaskNo")
                        {
                            openReworkTaskPage(rowData);
                        }
                        else if(field == "abnormalImage")
                        {
                            emit sigIMGView(exceptionNo);
                        }
                    });
        }
    }
}

QAbstractItemModel *RepairStationPage::createModel()
{
    return new RepairStationModel(this);
}

TabConfigs RepairStationPage::Tabs() const
{
    return
        {
         {"待处理", PageDisplayType::TABLE, QVariant::fromValue(FilterStatus::PENDING)},
         {"审核中", PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::REVIEW)},
         {"待返工", PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::WAITRETURN)},
         {"已处理", PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::PROCESSED)},
         };
}

FieldFilterProxyModel *RepairStationPage::createProxy(const QVariant &data)
{
    auto proxy = new FieldFilterProxyModel(this);
    proxy->addSearchFields("taskNo");
    proxy->addSearchFields("reworkTaskNo");
    proxy->addSearchFields("exceptionHandleNo");
    proxy->addSearchFields("exceptionHandleNoLink");
    proxy->addSearchFields("productModel");
    proxy->addSearchFields("productSN");
    proxy->setStatus(data); // 👉 Tab控制状态

    return proxy;
}

QString RepairStationPage::pageTitle() const
{
    return "维修站";
}

QString RepairStationPage::searchInfo() const
{
    return "请输入表单号";
}

void RepairStationPage::addWidgetToTitle(QHBoxLayout *layout)
{
    layout->addStretch();
}

void RepairStationPage::setupSearchLayout(QHBoxLayout *layout)
{
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setMinimumWidth(180);
    m_searchEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_repairJudgeBtn =
        new QPushButton(QString::fromUtf8("\xE7\xBB\xB4\xE4\xBF\xAE\xE5\x88\xA4\xE5\xAE\x9A"), this);
    m_exportBtn =
        new QPushButton(QString::fromUtf8("\xE5\xAF\xBC\xE5\x87\xBA\xE6\x8A\xA5\xE8\xA1\xA8"), this);

    connect(m_repairJudgeBtn, &QPushButton::clicked, this, [this]() {
        if (tabBar()->currentIndex() != 0)
            return;

        auto table =
            qobject_cast<QTableView*>(m_stack->currentWidget());
        if (!table)
            return;

        const QVariantMap rowData =
            rowDataFromProxyIndex(table->currentIndex());
        openRepairJudgePage(rowData);
    });

    layout->addWidget(m_searchEdit);
    layout->addStretch();
    layout->addWidget(m_repairJudgeBtn);
    layout->addWidget(m_exportBtn);
}

void RepairStationPage::updateRepairJudgeButton()
{
    if (!m_repairJudgeBtn)
        return;

    m_repairJudgeBtn->setEnabled(
        tabBar()->currentIndex() == 0);
}

QVariantMap RepairStationPage::rowDataFromProxyIndex(
    const QModelIndex& proxyIndex) const
{
    auto proxy =
        qobject_cast<const FieldFilterProxyModel*>(
            proxyIndex.model());
    auto model =
        qobject_cast<RepairStationModel*>(m_model);

    if (!proxy || !model)
        return {};

    QModelIndex sourceIndex =
        proxy->mapToSource(proxyIndex);
    if (!sourceIndex.isValid())
        return {};

    return model->rowData(sourceIndex.row());
}

void RepairStationPage::openRepairJudgePage(
    const QVariantMap& rowData)
{
    if (rowData.isEmpty())
        return;

    NavigationManager::instance()->openPage(
        PageType::RepairJudge,
        rowData);
}

void RepairStationPage::openReworkTaskPage(
    const QVariantMap& rowData)
{
    if (rowData.isEmpty())
        return;

    NavigationManager::instance()->openPage(
        PageType::ReworkTask,
        rowData);
}

bool RepairStationPage::isColumnVisibleForTab(
    const ColumnConfig& column,
    const QVariant& tabData) const
{
    if (!BasePageWidget::isColumnVisibleForTab(column, tabData))
        return false;

    const auto status = tabData.value<FilterStatus>();

    if (column.field == "reworkTaskNo")
        return status == FilterStatus::WAITRETURN;

    if (column.field == "exceptionHandleNoLink")
        return status != FilterStatus::WAITRETURN;

    if (column.field == "exceptionHandleNo")
        return status == FilterStatus::WAITRETURN;

    if (column.field == "taskNo")
        return status != FilterStatus::WAITRETURN;

    if (column.field == "handleMethod")
        return status == FilterStatus::REVIEW ||
               status == FilterStatus::PROCESSED;

    return true;
}
