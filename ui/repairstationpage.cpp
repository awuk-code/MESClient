#include "repairstationpage.h"
#include "basedialogwidget.h"
#include "repairstationmodel.h"
#include "fieldfilterproxymodel.h"
#include "navigationmanager.h"
#include "searchconfig.h"

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
                            showAbnormalImageDialog(rowData);
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
    SearchConfig::apply(proxy, SearchConfig::repairStationModule());
    proxy->setStatus(data); // 👉 Tab控制状态

    return proxy;
}

QString RepairStationPage::pageTitle() const
{
    return tr("维修站");
}

QString RepairStationPage::searchInfo() const
{
    return SearchConfig::placeholder(SearchConfig::repairStationModule());
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
    m_repairJudgeBtn->setProperty("buttonRole", "primary");
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

    auto messageLabel = new QLabel(
        tr("请先在“待处理”列表中勾选一条异常品记录，再点击“维修判定”。"),
        &dialog);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);
    dialog.contentLayout()->addWidget(messageLabel);

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

void RepairStationPage::showAbnormalImageDialog(const QVariantMap& rowData)
{
    BaseDialogWidget dialog(this);
    dialog.setTitle(tr("异常图片"));
    dialog.cancelButton()->hide();
    dialog.confirmButton()->setText(tr("确定"));

    const QString exceptionNo =
        rowData.value("exceptionHandleNo").toString();
    const QString imageText =
        rowData.value("abnormalImage").toString();

    auto label = new QLabel(
        tr("异常单号：%1\n\n当前记录的异常图片为“%2”。后续接入后台图片地址后，可在此处直接预览异常照片。")
            .arg(exceptionNo.isEmpty() ? tr("-") : exceptionNo,
                 imageText.isEmpty() ? tr("-") : imageText),
        &dialog);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setWordWrap(true);
    dialog.contentLayout()->addWidget(label);

    dialog.exec();
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
