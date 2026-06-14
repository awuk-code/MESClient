#include "processstationrightpanel.h"

#include "fieldfilterproxymodel.h"
#include "paginationproxymodel.h"


#include "processstationmodel.h"
#include "searchconfig.h"


#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QTabBar>
#include <QTableView>
#include <QVBoxLayout>

ProcessStationRightPanel::ProcessStationRightPanel(QWidget *parent)
    : BasePageWidget(parent)
{
    m_processPdfPage = new PdfViewWidget(this);
    m_processPdfPage->loadPdf("D:/C++11.pdf");

    m_referencePdfPage = new PdfViewWidget(this);

    m_referencePdfPage->loadPdf("D:/C++11.pdf");

    m_uploadPage = new DataUPloadWidget(this);

    setupPage();
    setReplacementMaterialVisible(m_replacementMaterialVisible);
    qDebug() << "ProcessStationRightPanel init, replacement material tab visible:"
             << m_replacementMaterialVisible;

    // m_tabWidget 为 BasePageWidget 中的成员
    connect(tabBar(), &QTabBar::currentChanged,
            this, &ProcessStationRightPanel::updateTableModelByTab);
    connect(tabBar(), &QTabBar::currentChanged,
            this, &ProcessStationRightPanel::updateSearchBarByTab);

    updateTableModelByTab(tabBar()->currentIndex());
    updateSearchBarByTab(tabBar()->currentIndex());

}


TabConfigs ProcessStationRightPanel::Tabs() const
{
    TabConfigs tabs;

    tabs.append({
        tr("物料核对"),
        PageDisplayType::TABLE,
        QVariant::fromValue(PageDisplayType::TABLE)
    });

    tabs.append({
        tr("工艺路线"),
        PageDisplayType::TABLE,
        QVariant::fromValue(PageDisplayType::TABLE)
    });

    tabs.append({
        tr("工艺文件"),
        PageDisplayType::PDF,
        QVariant::fromValue(PageDisplayType::PDF),
        m_processPdfPage
    });

    tabs.append({
        tr("工序物料信息"),
        PageDisplayType::TABLE,
        QVariant::fromValue(PageDisplayType::TABLE)
    });

    tabs.append({
        tr("工具设备"),
        PageDisplayType::TABLE,
        QVariant::fromValue(PageDisplayType::TABLE)
    });

    tabs.append({
        tr("资料上传"),
        PageDisplayType::NORMAL,
        QVariant::fromValue(PageDisplayType::NORMAL)
       , m_uploadPage
    });
    tabs.append({
        tr("引用文件"),
        PageDisplayType::PDF,
        QVariant::fromValue(PageDisplayType::PDF)
        ,m_referencePdfPage
    });

    tabs.append({
        tr("更换物料信息"),
        PageDisplayType::TABLE,
        QVariant::fromValue(PageDisplayType::TABLE)
    });

    return tabs;

}

QString ProcessStationRightPanel::pageTitle() const
{
    return tr("工艺信息");
}

QString ProcessStationRightPanel::searchInfo() const
{
    return m_currentSearchInfo.isEmpty()
    ? SearchConfig::placeholder(SearchConfig::processStationModule())
    : m_currentSearchInfo;;
}

QAbstractItemModel *ProcessStationRightPanel::createModel()
{
    // 创建工艺站点右侧页面专用模型
    // BasePageWidget 会将该模型绑定到所有 TABLE 类型的 Tab 页面中
    ProcessStationModel* model = new ProcessStationModel(this);

    // 默认显示第一个 Tab（物料核对）的数据
    model->setTableType(ProcessStationModel::MaterialCheck);

    return model;
}

FieldFilterProxyModel *ProcessStationRightPanel::createProxy(const QVariant &data)
{
    // data 中保存的是当前 Tab 的类型：
    // QVariant::fromValue(PageDisplayType::TABLE / PDF / NORMAL)

    PageDisplayType pageDisplayType = data.value<PageDisplayType>();


    // 只有 TABLE 类型才创建代理模型
    if (pageDisplayType != PageDisplayType::TABLE)
        return nullptr;

    auto proxy = new FieldFilterProxyModel(this);
    SearchConfig::apply(proxy, SearchConfig::processStationModule());

    // 使用 BasePageWidget 中创建的源模型
    proxy->setSourceModel(m_model);

    return proxy;
}

void ProcessStationRightPanel::addWidgetToTitle(QHBoxLayout *layout)
{
    layout->addStretch();
    m_toggle = new ToggleSwitchWidget(this);
    layout->addWidget(m_toggle);
    connect(m_toggle, &ToggleSwitchWidget::toggled,
            this, &ProcessStationRightPanel::toggleRequested);
}

void ProcessStationRightPanel::setupSearchLayout(QHBoxLayout *layout)
{
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setMinimumWidth(180);
    m_searchEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_searchEdit->addAction(QIcon(":/res/common/search.svg"),
                            QLineEdit::LeadingPosition);

    m_productSnLabel = new QLabel(tr("产品SN："), this);

    m_productSnCombo = new QComboBox(this);
    m_productSnCombo->setMinimumWidth(180);
    m_productSnCombo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_productSnCombo->addItem(tr("全部"));
    connect(m_productSnCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this]() {
                if (!m_productSnCombo)
                    return;

                const QString text = m_productSnCombo->currentText();
                m_currentProductSn = (text == tr("全部")) ? QString() : text;
                applyCurrentProductSn();
            });

    m_exportBtn = new QPushButton(tr("保存"), this);
    m_exportBtn->setProperty("buttonRole", "save");
    connect(m_exportBtn, &QPushButton::clicked,
            this, &ProcessStationRightPanel::onSaveBtnClicked);

    layout->addWidget(m_searchEdit);
    layout->addWidget(m_productSnLabel);
    layout->addWidget(m_productSnCombo);
    layout->addStretch();
    setupPaginationLayout(layout);
    layout->addWidget(m_exportBtn);
}

void ProcessStationRightPanel::onSaveBtnClicked()
{
    // TODO(backend-request): request backend to save right-side process information.
    // Suggested API: /api/process-station/process-info/save
    // Suggested request: taskNo, processCode, productSN, material rows, tool rows, upload file ids.
    // For compound material data, combine edited rows with MatAdapter::combine(...).
    // TODO: 保存按钮逻辑待定，后续接口或本地保存规则确定后在这里补充。
    qDebug() << "ProcessStationRightPanel save clicked, wait implementation";
}

void ProcessStationRightPanel::addScannedProductSn(const QString& productSn)
{
    const QString sn = productSn.trimmed();
    if (sn.isEmpty() || !m_productSnCombo)
        return;

    if (m_productSnCombo->findText(sn) < 0)
        m_productSnCombo->addItem(sn);

    m_productSnCombo->setCurrentText(sn);
    m_currentProductSn = sn;
    applyCurrentProductSn();

    qDebug() << __FUNCTION__ << "current product SN:" << sn;
}

void ProcessStationRightPanel::clearProductSnList()
{
    m_currentProductSn.clear();

    if (m_productSnCombo)
    {
        m_productSnCombo->blockSignals(true);
        m_productSnCombo->clear();
        m_productSnCombo->addItem(tr("全部"));
        m_productSnCombo->setCurrentIndex(0);
        m_productSnCombo->blockSignals(false);
    }

    applyCurrentProductSn();
}

bool ProcessStationRightPanel::validatePassReady(const QString& productSn, QString* message) const
{
    const QString sn = productSn.trimmed();
    if (sn.isEmpty())
    {
        if (message)
            *message = tr("请先扫描或输入产品SN。");
        return false;
    }

    if (m_productSnCombo && m_productSnCombo->findText(sn) < 0)
    {
        if (message)
            *message = tr("右侧产品SN列表中没有%1，请先扫入该SN。").arg(sn);
        return false;
    }

    if (m_currentProductSn != sn)
    {
        if (message)
            *message = m_currentProductSn.isEmpty()
                ? tr("请先在右侧产品SN下拉框选择%1后再PASS。").arg(sn)
                : tr("右侧当前产品SN为%1，请先切换到%2后再PASS。")
                      .arg(m_currentProductSn, sn);
        return false;
    }

//    if (m_uploadPage && !m_uploadPage->isComplete(message))
//        return false;

    // TODO(backend): 工序物料信息、工具设备是否必填，以及哪些行必须绑定，应由后台按当前工序返回。
    // 当前右侧表格共用 ProcessStationModel，切换页签会重载模型，暂不能可靠保存所有页签的完成状态。
    // 后续应改为按页签/产品SN缓存右侧填写状态，再在这里统一校验。
    return true;
}

void ProcessStationRightPanel::setCurrentSearchInfo(const QString &info)
{
    m_currentSearchInfo = info;
    updateSearchInfo();  // BasePageWidget 中已有刷新接口
}

void ProcessStationRightPanel::updateSearchBarByTab(int index)
{
    if (!m_searchWidget)
        return;

    const bool isPdfTab =
        index == 2 || index == 6;

    const bool useKeywordSearch =
        index == 0 || index == 1;

    const bool useProductSnSearch =
        !isPdfTab && !useKeywordSearch;

    m_searchWidget->setVisible(!isPdfTab);

    if (isPdfTab)
    {
        if (m_searchEdit)
        {
            m_searchEdit->clear();
            m_searchEdit->setVisible(false);
        }
        if (m_productSnLabel)
            m_productSnLabel->setVisible(false);
        if (m_productSnCombo)
            m_productSnCombo->setVisible(false);
        if (m_exportBtn)
            m_exportBtn->setVisible(false);

        qDebug() << __FUNCTION__ << "tab index:" << index
                 << "pdf tab, search area hidden";
        return;
    }

    if (m_searchEdit)
    {
        m_searchEdit->setVisible(useKeywordSearch);
        if (index == 0)
            m_searchEdit->setPlaceholderText(tr("请输入物料信息"));
        else if (index == 1)
            m_searchEdit->setPlaceholderText(tr("请输入工序名称"));

        if (!useKeywordSearch)
            m_searchEdit->clear();
    }

    if (m_productSnLabel)
        m_productSnLabel->setVisible(useProductSnSearch);

    if (m_productSnCombo)
        m_productSnCombo->setVisible(useProductSnSearch);

    if (m_exportBtn)
        m_exportBtn->setVisible(true);

    qDebug() << __FUNCTION__ << "tab index:" << index
             << "keywordSearch:" << useKeywordSearch
             << "productSnSearch:" << useProductSnSearch
             << "pdfTab:" << isPdfTab;
}

void ProcessStationRightPanel::updateTableModelByTab(int index)
{
    auto model = qobject_cast<ProcessStationModel*>(m_model);
    if (!model)
        return;

    switch (index)
    {
    case 0:
        model->setTableType(ProcessStationModel::MaterialCheck);
        break;

    case 1:
        model->setTableType(ProcessStationModel::ProcessRoute);
        break;

    case 3:
        model->setTableType(ProcessStationModel::ProcessMaterial);
        break;

    case 4:
        model->setTableType(ProcessStationModel::ToolEquipment);
        break;

    case 7:
        model->setTableType(ProcessStationModel::ReplacementMaterial);
        break;

    default:
        break;
    }

    qDebug() << __FUNCTION__ << "tab index:" << index
             << "table type:" << static_cast<int>(model->tableType());

    // 当前页面复用同一个模型，切换表格类型后需要重新设置列委托。
    auto table = qobject_cast<QTableView*>(m_stack->widget(index));
    if (table)
    {
        setupColumns(table, table->property("tabData"));
    }

    applyCurrentProductSn();
}

void ProcessStationRightPanel::applyCurrentProductSn()
{
    if (m_uploadPage)
        m_uploadPage->setProductSn(m_currentProductSn);

    auto table = qobject_cast<QTableView*>(m_stack ? m_stack->currentWidget() : nullptr);
    if (!table)
        return;

    auto pageProxy = currentPaginationProxy();
    auto proxy = pageProxy
        ? qobject_cast<FieldFilterProxyModel*>(pageProxy->sourceModel())
        : qobject_cast<FieldFilterProxyModel*>(table->model());
    if (!proxy)
        return;

    proxy->setFieldFilter(QStringLiteral("productSN"), QVariant());
    proxy->setFieldFilter(QStringLiteral("productSNCode"), QVariant());

    const QString field = productSnFieldForTab(tabBar() ? tabBar()->currentIndex() : -1);
    if (!field.isEmpty() && !m_currentProductSn.isEmpty())
        proxy->setFieldFilter(field, m_currentProductSn);

    if (pageProxy)
        pageProxy->firstPage();

    qDebug() << __FUNCTION__
             << "tab:" << (tabBar() ? tabBar()->currentIndex() : -1)
             << "field:" << field
             << "productSN:" << m_currentProductSn;
}

QString ProcessStationRightPanel::productSnFieldForTab(int index) const
{
    switch (index)
    {
    case 3:
    case 4:
        return QStringLiteral("productSN");
    case 7:
        return QStringLiteral("productSNCode");
    default:
        return QString();
    }
}

int ProcessStationRightPanel::replacementMaterialTabIndex() const
{
    return 7;
}

void ProcessStationRightPanel::setReplacementMaterialVisible(bool visible)
{
    m_replacementMaterialVisible = visible;

    if (!tabBar())
        return;

    const int index = replacementMaterialTabIndex();
    if (index < 0 || index >= tabBar()->count())
        return;

    if (!visible && tabBar()->currentIndex() == index)
        tabBar()->setCurrentIndex(0);

    tabBar()->setTabEnabled(index, visible);
    tabBar()->setTabText(index, visible ? tr("替换料件") : QString());
    tabBar()->setTabToolTip(index, visible ? QString() : tr("替换料件"));
    qDebug() << __FUNCTION__ << "visible:" << visible << "index:" << index;
}
