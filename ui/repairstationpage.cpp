#include "repairstationpage.h"
#include "basedialogwidget.h"
#include "repairstationmodel.h"
#include "fieldfilterproxymodel.h"
#include "navigationmanager.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableView>
#include <QVBoxLayout>

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
         {tr("待处理"), PageDisplayType::TABLE, QVariant::fromValue(FilterStatus::PENDING)},
         {tr("审核中"), PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::REVIEW)},
         {tr("待返工"), PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::WAITRETURN)},
         {tr("已处理"), PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::PROCESSED)},
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
    return tr("维修站");
}

QString RepairStationPage::searchInfo() const
{
    return tr("请输入表单号");
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
    m_searchEdit->addAction(QIcon(":/res/common/search.svg"),
                            QLineEdit::LeadingPosition);

    m_repairJudgeBtn =
        new QPushButton(tr("维修判定"), this);
    m_exportBtn =
        new QPushButton(tr("导出报表"), this);
    m_exportBtn->setProperty("buttonRole", "export");

    connect(m_repairJudgeBtn, &QPushButton::clicked,
            this, &RepairStationPage::onRepairJudgeBtnClicked);

    layout->addWidget(m_searchEdit);
    layout->addStretch();
    layout->addWidget(m_repairJudgeBtn);
    layout->addWidget(m_exportBtn);
}

void RepairStationPage::onRepairJudgeBtnClicked()
{
    if (tabBar()->currentIndex() != 0)
        return;

    auto table =
        qobject_cast<QTableView*>(m_stack->currentWidget());
    if (!table)
        return;

    // 维修判定按钮必须基于用户当前选中的异常品行跳转，避免没有上下文时打开空页面。
    const auto selection = table->selectionModel();
    if (!selection || !selection->hasSelection() || !table->currentIndex().isValid())
    {
        showSelectRowDialog();
        return;
    }

    const QVariantMap rowData =
        rowDataFromProxyIndex(table->currentIndex());
    if (rowData.isEmpty())
    {
        showSelectRowDialog();
        return;
    }

    openRepairJudgePage(rowData);
}

void RepairStationPage::updateRepairJudgeButton()
{
    if (!m_repairJudgeBtn)
        return;

    m_repairJudgeBtn->setEnabled(
        tabBar()->currentIndex() == 0);
}

void RepairStationPage::showSelectRowDialog()
{
    BaseDialogWidget dialog(this);
    dialog.setTitle(tr("维修判定"));
    dialog.cancelButton()->hide();
    dialog.confirmButton()->setText(tr("确定"));

    auto label = new QLabel(tr("请先选中一条待处理的异常品记录。"), &dialog);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    dialog.contentLayout()->addWidget(label);

    qDebug() << __FUNCTION__ << "repair judge clicked without selected row";
    dialog.exec();
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
