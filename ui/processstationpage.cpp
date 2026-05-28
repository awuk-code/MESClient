#include "processstationpage.h"
#include "pdfviewwidget.h"
#include "processstationmodel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QStyle>
#include <QRadioButton>
#include <QComboBox>
#include <QDebug>

ProcessStationPage::ProcessStationPage(QWidget *parent)
    : QWidget{parent}
{
    initUI();
    initConnect();
}

void ProcessStationPage::initUI()
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    m_leftPanel = new ProcessStationLeftPanel(this);

    m_rightPanel = new ProcessStationRightPanel(this);

    layout->addWidget(m_leftPanel);
    layout->addWidget(m_rightPanel);
    layout->setStretch(0, 2);
    layout->setStretch(1,3);
}

void ProcessStationPage::initConnect()
{
    connect(m_rightPanel, &ProcessStationRightPanel::toggleRequested,
            this, [=](bool isChecked){
                m_leftPanel->setVisible(!isChecked);
            });
}

void ProcessStationPage::setReworkTaskMode(bool advancedPermission)
{
    if (m_leftPanel)
    {
        m_leftPanel->setDisplayMode(advancedPermission
                                    ? ProcessStationLeftPanel::DisplayMode::ReworkTask
                                    : ProcessStationLeftPanel::DisplayMode::NormalTask);
    }

    if (m_rightPanel)
        m_rightPanel->setReplacementMaterialVisible(true);

    qDebug() << __FUNCTION__ << "advanced permission:" << advancedPermission;
}

void ProcessStationPage::setProductionTaskData(const QVariantMap &rowData)
{
    if (!m_leftPanel || rowData.isEmpty())
        return;

    TaskList taskInfoValues = {
        rowData.value("taskNo"),
        rowData.value("productModel"),
        rowData.value("erpCode"),
        rowData.value("workCount"),
        rowData.value("finishTime"),
        rowData.value("routeName"),
        rowData.value("lineNo")
    };

    // 生产任务点击“开工”后带入当前任务数据；后续接接口时可在这里补充更多字段映射。
    m_leftPanel->setDisplayMode(ProcessStationLeftPanel::DisplayMode::NormalTask);
    m_leftPanel->setTaskInfoValue(taskInfoValues);

    qDebug() << __FUNCTION__
             << "taskNo =" << rowData.value("taskNo").toString()
             << "productModel =" << rowData.value("productModel").toString();
}

ProcessStationLeftPanel::ProcessStationLeftPanel(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("ProcessStationLeftPanel");
    initUI();
    initConnect();

    // 任务单信息示例数据（7项，与 taskInfoKeys() 一一对应）
    TaskList taskInfoValues = {
        "RW20260519001",      // 生产任务单号
        "XK-500A",            // 产品型号
        "ERP-2026-001",       // 产品ERP编码
        1000,                 // 生产任务工单数量
        "2026-05-31",         // 生产任务单完成日期
        "V2.3",               // 配置项配套表版本
        "LINE-01"             // 生产线编号
    };

    // 任务单状态示例数据（7项，与 taskStatusKeys() 一一对应）
    TaskList taskStatusValues = {
        320,                  // 当前工序完成数量
        5,                    // 当前工序NG数量
        "装配",               // 上一工序
        "测试",               // 当前工序
        "包装",               // 下一工序
        "已核对",             // 物料核对状态
        "ST-001"              // 工站编号
    };

    // 填充表格
    setTaskInfoValue(taskInfoValues);
    setTaskStatusValue(taskStatusValues);

    // 返工任务单高级权限使用的异常品信息示例数据，默认先创建并隐藏，后续接权限时直接切换显示。
    TaskList abnormalInfoValues = {
        "YC12352625123",          // 异常处理单号
        "WN1K25001254005-R001",   // 返工任务单号
        "ZX-IRLD-0292-124",       // 生产工艺流程
        "三防冲击检查",             // 异常上报工序
        "外观不良",                 // 异常类型
        1,                         // 异常数量
        tr("查看图片")              // 异常图片
    };

    TaskList reworkTaskStatusValues = {
        0,               // 当前工序完成数量
        0,               // 当前工序NG数量
        "装配电检",        // 上一工序
        "返工维修",        // 当前工序
        "模块装配检验",     // 下一工序
        "待更换物料",       // 物料核对状态
        "Line1-OC-2"     // 工站编号
    };

    setAbnormalInfoValue(abnormalInfoValues);
    setReworkTaskStatusValue(reworkTaskStatusValues);
    setDisplayMode(DisplayMode::NormalTask);
    qDebug() << "ProcessStationLeftPanel init, default mode NormalTask";
}

void ProcessStationLeftPanel::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0,0);
    mainLayout->setSpacing(8);

    m_taskInfo = createTaskWidget(tr("任务单信息"), m_taskInfoTitleLabel, m_taskInfoTable);
    m_abnormalInfo = createTaskWidget(tr("异常品信息"), m_abnormalInfoTitleLabel, m_abnormalInfoTable);
    m_taskStatus = createTaskWidget(tr("任务单状态"), m_taskStatusTitleLabel, m_taskStatusTable);
    qDebug() << __FUNCTION__ <<"m_taskInfoTable: "<<m_taskInfoTable
             <<"m_abnormalInfoTable:"<<m_abnormalInfoTable
             <<"m_taskStatusTable:"<<m_taskStatusTable;
    m_pass = createPassWidget(tr("扫码过站"));

    mainLayout->addWidget(m_taskInfo);
    mainLayout->addWidget(m_abnormalInfo);
    mainLayout->addWidget(m_taskStatus);
    mainLayout->addWidget(m_pass);
    // 扫码区域占据剩余空间
    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 0);
    mainLayout->setStretch(3, 1);
}

void ProcessStationLeftPanel::initConnect()
{

}

QWidget *ProcessStationLeftPanel::createTaskWidget(const QString &title, QLabel* &titleLabel, QTableWidget *&tableout)
{
    QWidget* widget = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    QWidget* titleWidget = createTaskTitleWidget(title, titleLabel, widget);
    layout->addWidget(titleWidget);

    QTableWidget* table = createTaskTable(widget);
    tableout = table;

    // 表格区域单独缩进，保持原左侧布局样式。
    QHBoxLayout* tableLayout = new QHBoxLayout;
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(0);
    tableLayout->addSpacing(28);
    tableLayout->addWidget(table);

    layout->addLayout(tableLayout);

    auto toggleBtn = titleWidget->findChild<QToolButton*>();
    if (toggleBtn)
    {
        connect(toggleBtn, &QToolButton::toggled,
                this,
                [=](bool checked)
                {
                    table->setVisible(checked);

                    if (checked)
                    {
                        toggleBtn->setIcon(
                            QIcon(":/res/common/expand.svg"));
                        toggleBtn->setToolTip(tr("收起"));
                    }
                    else
                    {
                        toggleBtn->setIcon(
                            QIcon(":/res/common/collapse.svg"));
                        toggleBtn->setToolTip(tr("展开"));
                    }
                });
    }

    qDebug() << __FUNCTION__ << "created task widget:" << title
             << "table:" << table;

    return widget;
}

QWidget *ProcessStationLeftPanel::createTaskTitleWidget(const QString &title, QLabel* &titleLabel, QWidget *parentWidget)
{
    QWidget* widget = new QWidget(parentWidget);

    // =========================
    // 标题区域
    // =========================

    QHBoxLayout* titleLayout = new QHBoxLayout(widget);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* icon = new QLabel(widget);
    QLabel* text = new QLabel(widget);
    icon->setPixmap(QPixmap(":res/common/rect.svg").scaled(24,24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    text->setText(title);
    titleLabel = text;

    QToolButton* toggleBtn = new QToolButton(widget);
    toggleBtn->setCheckable(true);
    toggleBtn->setChecked(true);
    toggleBtn->setAutoRaise(true);
    toggleBtn->setCursor(Qt::PointingHandCursor);
    toggleBtn->setFixedSize(20, 20);
    toggleBtn->setIconSize(QSize(16, 16));
    toggleBtn->setIcon(QIcon(":/res/common/expand.svg"));
    toggleBtn->setToolTip(tr("收起"));

    // 去掉按钮边框和背景
    toggleBtn->setStyleSheet(R"(
    QToolButton {
        border: none;
        background: transparent;
        padding: 0px;
    }
)");

    titleLayout->addWidget(icon);
    titleLayout->addWidget(text);
    titleLayout->addWidget(toggleBtn);
    titleLayout->addStretch();

    qDebug() << __FUNCTION__ << "created title:" << title;
    return widget;
}

QTableWidget *ProcessStationLeftPanel::createTaskTable(QWidget *parentWidget)
{
    QTableWidget* table = new QTableWidget(7, 2, parentWidget);

    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setFocusPolicy(Qt::NoFocus);
    table->setAlternatingRowColors(true);
    table->setShowGrid(true);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setColumnWidth(0, 45);
    table->setColumnWidth(1, 55);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    for (int row = 0; row < 7; ++row)
    {
        QTableWidgetItem* item0 = new QTableWidgetItem;
        QTableWidgetItem* item1 = new QTableWidgetItem;

        item0->setTextAlignment(Qt::AlignCenter);
        item1->setTextAlignment(Qt::AlignCenter);

        table->setItem(row, 0, item0);
        table->setItem(row, 1, item1);
    }

    int height =
        table->rowCount() * table->verticalHeader()->defaultSectionSize() + 4;
    table->setFixedHeight(height);

    qDebug() << __FUNCTION__ << "created table:" << table;
    return table;
}

QWidget *ProcessStationLeftPanel::createPassWidget(const QString &title)
{
    QWidget* widget = new QWidget(this);

    auto* layout = new QVBoxLayout(widget);

    QHBoxLayout* titleLayout = new QHBoxLayout;
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* icon = new QLabel(widget);
    QLabel* text = new QLabel(widget);
    icon->setPixmap(QPixmap(":res/common/rect.svg").scaled(24,24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    text->setText(title);

    titleLayout->addWidget(icon);
    titleLayout->addWidget(text);
    titleLayout->addStretch();
    layout->addLayout(titleLayout);


    QHBoxLayout* contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // 左侧缩进
    contentLayout->addSpacing(28);

    // 实际内容容器
    QWidget* contentWidget = new QWidget(widget);
    QVBoxLayout* contentVBox = new QVBoxLayout(contentWidget);
    contentVBox->setContentsMargins(0, 0, 0, 0);
    contentVBox->setSpacing(8);

    // ==========================================================
    // 1. 过站类别
    // ==========================================================
    {
        QHBoxLayout* row = new QHBoxLayout;
        row->setContentsMargins(0, 0, 0, 0);

        QLabel* label = new QLabel(tr("过站类别："), contentWidget);
        QPushButton* typeBtn = new QPushButton(tr("正常过站"), contentWidget);

        row->addWidget(label);
        row->addWidget(typeBtn);
        row->addStretch();

        contentVBox->addLayout(row);
    }

    // ==========================================================
    // 2. 过站方式
    // ==========================================================
    {
        QHBoxLayout* row = new QHBoxLayout;
        row->setContentsMargins(0, 0, 0, 0);

        QLabel* label = new QLabel(tr("过站方式："), contentWidget);

        // 单选按钮
        QRadioButton* scanInRadio = new QRadioButton(tr("扫入"), contentWidget);
        QRadioButton* passRadio   = new QRadioButton(tr("PASS"), contentWidget);
        QRadioButton* ngRadio     = new QRadioButton(tr("NG"), contentWidget);

        // 默认选中“扫入”
        scanInRadio->setChecked(true);

        // 普通按钮
        QPushButton* pauseBtn      = new QPushButton(tr("暂停"), contentWidget);
        QPushButton* resumeBtn     = new QPushButton(tr("解除暂停"), contentWidget);
        QPushButton* rollbackBtn   = new QPushButton(tr("工序回退"), contentWidget);

        row->addWidget(label);
        row->addWidget(scanInRadio);
        row->addWidget(passRadio);
        row->addWidget(ngRadio);
        row->addSpacing(12);
        row->addWidget(pauseBtn);
        row->addWidget(resumeBtn);
        row->addWidget(rollbackBtn);
        row->addStretch();

        contentVBox->addLayout(row);
    }

    // ==========================================================
    // 3. 操作
    // ==========================================================
    {
        QHBoxLayout* row = new QHBoxLayout;
        row->setContentsMargins(0, 0, 0, 0);

        QLabel* label = new QLabel(tr("扫码操作："), contentWidget);

        QLineEdit* scanEdit = new QLineEdit(contentWidget);
        scanEdit->setPlaceholderText(tr("请扫描产品条码..."));

        QPushButton* actionBtn = new QPushButton(tr("执行"), contentWidget);

        row->addWidget(label);
        row->addWidget(scanEdit);
        row->addWidget(actionBtn);
        row->addStretch();

        contentVBox->addLayout(row);
    }

    // ==========================================================
    // 4. 状态信息（标签与表格在同一行）
    // ==========================================================
    {
        QHBoxLayout* row = new QHBoxLayout;
        row->setContentsMargins(0, 0, 0, 0);
        row->setSpacing(8);

        // 左侧标签
        QLabel* label = new QLabel(tr("状态信息："), contentWidget);
        row->addWidget(label, 0, Qt::AlignTop);
        // 右侧表格
        QTableView* tableView = new QTableView(contentWidget);

        QStandardItemModel* model =
            new QStandardItemModel(0, 5, tableView);

        model->setHorizontalHeaderLabels(QStringList()
                                         << tr("序号")
                                         << tr("产品SN")
                                         << tr("PASS")
                                         << tr("NG")
                                         << tr("暂停"));

        tableView->setModel(model);

        // 表格属性
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setAlternatingRowColors(true);
        tableView->verticalHeader()->hide();

        // 列宽按比例自动拉伸
        tableView->horizontalHeader()->setSectionResizeMode(
            QHeaderView::Stretch);

        // 固定一个合适的高度
        tableView->setMinimumHeight(180);

        // 布局：
        // label 固定宽度，tableView 占据剩余空间
        row->addWidget(tableView, 1);

        // 加入内容布局
        contentVBox->addLayout(row);
    }

    // 将内容区域加入外层布局
    contentLayout->addWidget(contentWidget);
    layout->addLayout(contentLayout);

    return widget;
}

TaskList ProcessStationLeftPanel::taskInfoKeys() const
{
    TaskList keys = {
        tr("生产任务单号"),
        tr("产品型号"),
        tr("产品EPR编码"),
        tr("生产任务工单数量"),
        tr("生产任务单完成日期"),
        tr("配置项配套表版本"),
        tr("生产线编号")
    };
    return keys;
}

TaskList ProcessStationLeftPanel::taskStatusKeys() const
{
    TaskList keys = {
        tr("当前工序完成数量"),
        tr("当前工序NG数量"),
        tr("上一工序"),
        tr("当前工序"),
        tr("下一工序"),
        tr("物料和对状态"),
        tr("工站编号")
    };
    return keys;
}

TaskList ProcessStationLeftPanel::abnormalInfoKeys() const
{
    TaskList keys = {
        tr("异常处理单号"),
        tr("返工任务单号"),
        tr("生产工艺流程"),
        tr("异常上报工序"),
        tr("异常类型"),
        tr("异常数量"),
        tr("异常图片")
    };
    return keys;
}

void ProcessStationLeftPanel::setTaskData(QTableWidget *table, const TaskList &keys, const TaskList &values)
{
    if (!table)
        return;

    const int rowCount = qMin(table->rowCount(), keys.size());

    for (int row = 0; row < rowCount; ++row)
    {
        // 第一列固定为字段名（后续可自行设置）
        if (!table->item(row, 0))
            table->setItem(row, 0, new QTableWidgetItem);

        if (!table->item(row, 1))
            table->setItem(row, 1, new QTableWidgetItem);

        // 第一列：字段名
        table->item(row, 0)->setText(keys[row].toString());

        // 第二列：字段值（values 不足时置空）
        if (row < values.size())
            table->item(row, 1)->setText(values[row].toString());
        else
            table->item(row, 1)->setText("");
    }

}

void ProcessStationLeftPanel::setTaskInfoValue(TaskList &values)
{
    m_taskInfoValue = values;
    setTaskData(m_taskInfoTable, taskInfoKeys(), values);
}

void ProcessStationLeftPanel::setTaskStatusValue(TaskList &values)
{
    m_taskStatusValue = values;
    setTaskData(m_taskStatusTable, taskStatusKeys(), values);
}

void ProcessStationLeftPanel::setAbnormalInfoValue(TaskList &values)
{
    m_abnormalInfoValue = values;
    setTaskData(m_abnormalInfoTable, abnormalInfoKeys(), values);
    qDebug() << __FUNCTION__ << "rows:" << values.size();
}

void ProcessStationLeftPanel::setReworkTaskStatusValue(TaskList &values)
{
    m_reworkTaskStatusValue = values;
    qDebug() << __FUNCTION__ << "rows:" << values.size();

    if (m_displayMode == DisplayMode::ReworkTask)
        setTaskData(m_taskStatusTable, taskStatusKeys(), values);
}

void ProcessStationLeftPanel::setDisplayMode(DisplayMode mode)
{
    m_displayMode = mode;

    const bool isReworkTask = (mode == DisplayMode::ReworkTask);

    if (m_taskInfo)
        m_taskInfo->setVisible(!isReworkTask);

    if (m_abnormalInfo)
        m_abnormalInfo->setVisible(isReworkTask);

    if (m_taskStatusTitleLabel)
        m_taskStatusTitleLabel->setText(isReworkTask ? tr("返工维修任务单状态") : tr("任务单状态"));

    if (isReworkTask)
        setTaskData(m_taskStatusTable, taskStatusKeys(), m_reworkTaskStatusValue);
    else
        setTaskData(m_taskStatusTable, taskStatusKeys(), m_taskStatusValue);

    qDebug() << __FUNCTION__ << "mode:" << (isReworkTask ? "ReworkTask" : "NormalTask")
             << "taskInfoVisible:" << (m_taskInfo ? m_taskInfo->isVisible() : false)
             << "abnormalInfoVisible:" << (m_abnormalInfo ? m_abnormalInfo->isVisible() : false);
}


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
        new QLabel("资料上传页面"));

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

    m_productSnLabel = new QLabel(tr("产品SN："), this);

    m_productSnCombo = new QComboBox(this);
    m_productSnCombo->setMinimumWidth(180);
    m_productSnCombo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_productSnCombo->addItem(tr("全部"));

    m_exportBtn = new QPushButton(tr("保存"), this);
    m_exportBtn->setProperty("buttonRole", "save");
    connect(m_exportBtn, &QPushButton::clicked, this, []() {
        // TODO: 保存按钮逻辑待定，后续接口或本地保存规则确定后在这里补充。
        qDebug() << "ProcessStationRightPanel save clicked, wait implementation";
    });

    layout->addWidget(m_searchEdit);
    layout->addWidget(m_productSnLabel);
    layout->addWidget(m_productSnCombo);
    layout->addStretch();
    layout->addWidget(m_exportBtn);
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

    const int replacementIndex = replacementMaterialTabIndex();

    const bool useKeywordSearch =
        index == 0 || index == 1;

    const bool useProductSnSearch =
        index == 3 || index == 4 || index == replacementIndex;

    const bool showSearchControls =
        useKeywordSearch || useProductSnSearch;

    m_searchWidget->setVisible(true);

    if (!showSearchControls)
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
             << "productSnSearch:" << useProductSnSearch;
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

    tabBar()->setTabVisible(index, visible);
    qDebug() << __FUNCTION__ << "visible:" << visible << "index:" << index;
}
