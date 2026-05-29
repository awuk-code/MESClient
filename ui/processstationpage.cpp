#include "processstationpage.h"
#include "jsondataloader.h"
#include "pdfviewwidget.h"
#include "processstationmodel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
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

    // 生产任务点击“开工”后带入当前任务数据；左侧信息区按字段名从 rowData 中取值。
    m_leftPanel->setDisplayMode(ProcessStationLeftPanel::DisplayMode::NormalTask);
    m_leftPanel->setTaskInfoData(rowData);

    qDebug() << __FUNCTION__
             << "taskNo =" << rowData.value("taskNo").toString()
             << "productModel =" << rowData.value("productModel").toString();
}

void ProcessStationPage::setReworkTaskData(const QVariantMap &rowData)
{
    if (!m_leftPanel || rowData.isEmpty())
        return;

    // 返工任务单从维修站的返工任务单号进入，异常品信息直接显示该行产品的所有异常字段。
    m_leftPanel->setDisplayMode(ProcessStationLeftPanel::DisplayMode::ReworkTask);
    m_leftPanel->setAbnormalInfoData(rowData);

    qDebug() << __FUNCTION__
             << "reworkTaskNo =" << rowData.value("reworkTaskNo").toString()
             << "exceptionHandleNo =" << rowData.value("exceptionHandleNo").toString()
             << "productSN =" << rowData.value("productSN").toString();
}

ProcessStationLeftPanel::ProcessStationLeftPanel(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("ProcessStationLeftPanel");
    initUI();
    initConnect();

    // 左侧默认展示数据从 JSON 读取；通过生产任务/返工任务跳转进来时，
    // 任务单信息和异常品信息会被点击行数据覆盖，不再重新读 JSON。
    const QVariantMap defaultData =
        JsonDataLoader::loadObject("process_station_left_panel.json");
    setTaskInfoData(defaultData.value("taskInfo").toMap());
    setTaskStatusData(defaultData.value("taskStatus").toMap());
    setAbnormalInfoData(defaultData.value("abnormalInfo").toMap());
    setReworkTaskStatusData(defaultData.value("reworkTaskStatus").toMap());

    setDisplayMode(DisplayMode::NormalTask);
    qDebug() << "ProcessStationLeftPanel init, default mode NormalTask";
}

void ProcessStationLeftPanel::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0,0);
    mainLayout->setSpacing(8);

    m_taskInfo = createTaskWidget(tr("任务单信息"), m_taskInfoTitleLabel, m_taskInfoLayout);
    m_abnormalInfo = createTaskWidget(tr("异常品信息"), m_abnormalInfoTitleLabel, m_abnormalInfoLayout);
    m_taskStatus = createTaskWidget(tr("任务单状态"), m_taskStatusTitleLabel, m_taskStatusLayout);
    qDebug() << __FUNCTION__ <<"m_taskInfoLayout: "<<m_taskInfoLayout
             <<"m_abnormalInfoLayout:"<<m_abnormalInfoLayout
             <<"m_taskStatusLayout:"<<m_taskStatusLayout;
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

QWidget *ProcessStationLeftPanel::createTaskWidget(const QString &title, QLabel* &titleLabel, QGridLayout *&layoutout)
{
    QWidget* widget = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    QWidget* titleWidget = createTaskTitleWidget(title, titleLabel, widget);
    layout->addWidget(titleWidget);

    QWidget* infoWidget = new QWidget(widget);
    QGridLayout* infoLayout = createInfoGrid(infoWidget);
    layoutout = infoLayout;

    // 信息区域单独缩进，保持原左侧布局样式；内部改为维修判定页同类的字段-值布局。
    QHBoxLayout* tableLayout = new QHBoxLayout;
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(0);
    tableLayout->addSpacing(28);
    tableLayout->addWidget(infoWidget);

    layout->addLayout(tableLayout);

    auto toggleBtn = titleWidget->findChild<QToolButton*>();
    if (toggleBtn)
    {
        connect(toggleBtn, &QToolButton::toggled,
                this,
                [=](bool checked)
                {
                    infoWidget->setVisible(checked);

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
             << "layout:" << infoLayout;

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

QGridLayout *ProcessStationLeftPanel::createInfoGrid(QWidget *parentWidget)
{
    auto grid = new QGridLayout(parentWidget);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(8);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(3, 1);
    qDebug() << __FUNCTION__ << "created info grid:" << grid;
    return grid;
}

QLabel *ProcessStationLeftPanel::createInfoTitleLabel(const QString &text, QWidget *parentWidget)
{
    auto label = new QLabel(text, parentWidget);
    label->setMinimumWidth(110);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setStyleSheet("QLabel{color:#606266;}");
    return label;
}

QLabel *ProcessStationLeftPanel::createInfoValueLabel(const QString &text, QWidget *parentWidget)
{
    auto label = new QLabel(text, parentWidget);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setWordWrap(true);
    label->setStyleSheet("QLabel{color:#303133;}");
    return label;
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

QVector<QPair<QString, QString>> ProcessStationLeftPanel::taskInfoFields() const
{
    return {
        {tr("生产任务单号"), "taskNo"},
        {tr("产品型号"), "productModel"},
        {tr("产品EPR编码"), "erpCode"},
        {tr("生产任务工单数量"), "workCount"},
        {tr("生产任务单完成日期"), "finishTime"},
        {tr("配置项配套表版本"), "routeName"},
        {tr("生产线编号"), "lineNo"}
    };
}

QVector<QPair<QString, QString>> ProcessStationLeftPanel::taskStatusFields() const
{
    return {
        {tr("当前工序完成数量"), "finishedCount"},
        {tr("当前工序NG数量"), "ngCount"},
        {tr("上一工序"), "previousProcess"},
        {tr("当前工序"), "currentProcess"},
        {tr("下一工序"), "nextProcess"},
        {tr("物料核对状态"), "materialCheckStatus"},
        {tr("工站编号"), "stationNo"}
    };
}

QVector<QPair<QString, QString>> ProcessStationLeftPanel::abnormalInfoFields() const
{
    return {
        {tr("异常处理单号"), "exceptionHandleNo"},
        {tr("返工任务单号"), "reworkTaskNo"},
        {tr("生产任务单号"), "taskNo"},
        {tr("生产工艺流程"), "routeName"},
        {tr("产品型号"), "productModel"},
        {tr("产品SN"), "productSN"},
        {tr("异常数量"), "abnormalCount"},
        {tr("异常类型"), "abnormalType"},
        {tr("异常现象"), "abnormalPhenomenon"},
        {tr("异常图片"), "abnormalImage"},
        {tr("异常上报工序"), "reportProcess"},
        {tr("异常上报时间"), "reportTime"},
        {tr("产线编码"), "lineNo"},
        {tr("工作站编号"), "stationNo"},
        {tr("上报人员"), "reporter"},
        {tr("异常处理方式"), "handleMethod"},
        {tr("当前状态"), "status"}
    };
}

QVariantMap ProcessStationLeftPanel::valuesToRowData(
    const QVector<QPair<QString, QString>>& fields,
    const TaskList& values) const
{
    QVariantMap rowData;
    for (int i = 0; i < fields.size() && i < values.size(); ++i)
        rowData.insert(fields[i].second, values[i]);

    return rowData;
}

void ProcessStationLeftPanel::setTaskData(
    QGridLayout *layout,
    const QVector<QPair<QString, QString>>& fields,
    const QVariantMap &rowData,
    int fieldPairsPerRow)
{
    if (!layout)
        return;

    // 左侧信息区统一规则：7项及以下单列显示，超过7项再自动分成两列，避免区域过高。
    if (fieldPairsPerRow <= 0)
        fieldPairsPerRow = fields.size() <= 7 ? 1 : 2;
    fieldPairsPerRow = qMax(1, fieldPairsPerRow);

    while (auto item = layout->takeAt(0))
    {
        if (auto widget = item->widget())
            widget->deleteLater();

        delete item;
    }

    auto parentWidget = layout->parentWidget();
    for (int i = 0; i < fields.size(); ++i)
    {
        const int row = i / fieldPairsPerRow;
        const int labelColumn = (i % fieldPairsPerRow) * 2;
        const QString title = fields[i].first;
        const QString field = fields[i].second;
        const QString value =
            rowData.value(field).toString().isEmpty()
                ? QStringLiteral("-")
                : rowData.value(field).toString();

        layout->addWidget(
            createInfoTitleLabel(title, parentWidget),
            row,
            labelColumn);
        layout->addWidget(
            createInfoValueLabel(value, parentWidget),
            row,
            labelColumn + 1);
    }

    qDebug() << __FUNCTION__ << "fields:" << fields.size() << "data keys:" << rowData.keys();
}

void ProcessStationLeftPanel::setTaskInfoValue(TaskList &values)
{
    m_taskInfoValue = values;
    setTaskInfoData(valuesToRowData(taskInfoFields(), values));
}

void ProcessStationLeftPanel::setTaskStatusValue(TaskList &values)
{
    m_taskStatusValue = values;
    setTaskStatusData(valuesToRowData(taskStatusFields(), values));
}

void ProcessStationLeftPanel::setAbnormalInfoValue(TaskList &values)
{
    m_abnormalInfoValue = values;
    setAbnormalInfoData(valuesToRowData(abnormalInfoFields(), values));
    qDebug() << __FUNCTION__ << "rows:" << values.size();
}

void ProcessStationLeftPanel::setReworkTaskStatusValue(TaskList &values)
{
    m_reworkTaskStatusValue = values;
    m_reworkTaskStatusData = valuesToRowData(taskStatusFields(), values);
    qDebug() << __FUNCTION__ << "rows:" << values.size();

    if (m_displayMode == DisplayMode::ReworkTask)
        setTaskData(m_taskStatusLayout, taskStatusFields(), m_reworkTaskStatusData);
}

void ProcessStationLeftPanel::setTaskInfoData(const QVariantMap &rowData)
{
    m_taskInfoData = rowData;
    setTaskData(m_taskInfoLayout, taskInfoFields(), rowData);
}

void ProcessStationLeftPanel::setTaskStatusData(const QVariantMap &rowData)
{
    m_taskStatusData = rowData;
    if (m_displayMode == DisplayMode::NormalTask)
        setTaskData(m_taskStatusLayout, taskStatusFields(), rowData);
}

void ProcessStationLeftPanel::setAbnormalInfoData(const QVariantMap &rowData)
{
    m_abnormalInfoData = rowData;
    setTaskData(m_abnormalInfoLayout, abnormalInfoFields(), rowData);
}

void ProcessStationLeftPanel::setReworkTaskStatusData(const QVariantMap &rowData)
{
    m_reworkTaskStatusData = rowData;
    if (m_displayMode == DisplayMode::ReworkTask)
        setTaskData(m_taskStatusLayout, taskStatusFields(), rowData);
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
        setTaskData(m_taskStatusLayout, taskStatusFields(), m_reworkTaskStatusData);
    else
        setTaskData(m_taskStatusLayout, taskStatusFields(), m_taskStatusData);

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
