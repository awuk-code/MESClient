#include "processstationrightpanel.h"

#include "fieldfilterproxymodel.h"
#include "pdfviewwidget.h"
#include "processstationmodel.h"
#include "toggleswitchwidget.h"

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

    m_uploadPage =
        new QWidget(this);
    auto uploadLayout =
        new QVBoxLayout(m_uploadPage);

    uploadLayout->addWidget(
        new QLabel(tr("资料上传页面")));

    uploadLayout->addStretch();
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
        ,m_uploadPage
    });
    tabs.append({
        tr("引用文件"),
        PageDisplayType::PDF,
        QVariant::fromValue(PageDisplayType::PDF)
        , m_referencePdfPage
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
    ? tr("请输入物料信息")
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
    proxy->addSearchFields("materialCode");
    proxy->addSearchFields("materialName");
    proxy->addSearchFields("processName");
    proxy->addSearchFields("productSNCode");
    proxy->addSearchFields("materialSN");

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

    m_exportBtn = new QPushButton(tr("保存"), this);
    m_exportBtn->setProperty("buttonRole", "save");
    connect(m_exportBtn, &QPushButton::clicked,
            this, &ProcessStationRightPanel::onSaveBtnClicked);

    layout->addWidget(m_searchEdit);
    layout->addWidget(m_productSnLabel);
    layout->addWidget(m_productSnCombo);
    layout->addStretch();
    layout->addWidget(m_exportBtn);
}

void ProcessStationRightPanel::onSaveBtnClicked()
{
    // TODO: 保存按钮逻辑待定，后续接口或本地保存规则确定后在这里补充。
    qDebug() << "ProcessStationRightPanel save clicked, wait implementation";
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

    tabBar()->setTabVisible(index, visible);
    qDebug() << __FUNCTION__ << "visible:" << visible << "index:" << index;
}
