#include "processstationpage.h"
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
}

void ProcessStationLeftPanel::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0,0);
    mainLayout->setSpacing(8);

    m_taskInfo = createTaskWidget(tr("任务单信息"), m_taskInfoTable);
    m_taskStatus = createTaskWidget(tr("任务单状态"), m_taskStatusTable);
    qDebug() << __FUNCTION__ <<"m_taskInfoTable: "<<m_taskInfoTable
             <<"m_taskStatusTable:"<<m_taskStatusTable;
    m_pass = createPassWidget(tr("扫码过站"));

    mainLayout->addWidget(m_taskInfo);
    mainLayout->addWidget(m_taskStatus);
    mainLayout->addWidget(m_pass);
    // 扫码区域占据剩余空间
    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 1);
}

void ProcessStationLeftPanel::initConnect()
{

}

QWidget *ProcessStationLeftPanel::createTaskWidget(const QString &title, QTableWidget *&tableout)
{
    QWidget* widget = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    // =========================
    // 标题区域
    // =========================

    QHBoxLayout* titleLayout = new QHBoxLayout;
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* icon = new QLabel(widget);
    QLabel* text = new QLabel(widget);
    icon->setPixmap(QPixmap(":res/common/rect.svg").scaled(24,24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    text->setText(title);
    QToolButton* toggleBtn = new QToolButton(widget);
    toggleBtn->setCheckable(true);
    toggleBtn->setChecked(true);
    toggleBtn->setAutoRaise(true);
    toggleBtn->setCursor(Qt::PointingHandCursor);
    toggleBtn->setFixedSize(20, 20);
    toggleBtn->setIconSize(QSize(16, 16));
    toggleBtn->setIcon(QIcon(":/res/common/expand.svg"));

    // 去掉按钮边框和背景
    toggleBtn->setStyleSheet(R"(
    QToolButton {
        border: none;
        background: transparent;
        padding: 0px;
    }
)");
    toggleBtn->setIcon(QIcon(":/res/common/expand.svg"));

    titleLayout->addWidget(icon);
    titleLayout->addWidget(text);
    titleLayout->addWidget(toggleBtn);
    titleLayout->addStretch();

    layout->addLayout(titleLayout);

    // =========================
    // 表格
    // =========================

    QTableWidget* table = new QTableWidget(7, 2, widget);
    tableout = table;
    qDebug() << __FUNCTION__ <<"当前表格："<<table<<"m_taskInfoTable:"<<m_taskInfoTable
             <<"m_taskStatusTable:"<<m_taskStatusTable;

    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    // 基本属性
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setFocusPolicy(Qt::NoFocus);
    table->setAlternatingRowColors(true);
    table->setShowGrid(true);

    //table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    //table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 设置初始比例（只有在表格第一次显示时生效，之后由 Stretch 保持比例）
    table->setColumnWidth(0, 45);
    table->setColumnWidth(1, 55);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // 初始化单元格
    for (int row = 0; row < 7; ++row)
    {
        QTableWidgetItem* item0 = new QTableWidgetItem;
        QTableWidgetItem* item1 = new QTableWidgetItem;

        // 设置文本水平和垂直居中
        item0->setTextAlignment(Qt::AlignCenter);
        item1->setTextAlignment(Qt::AlignCenter);

        table->setItem(row, 0, item0);
        table->setItem(row, 1, item1);
    }

    // 固定高度
    int height =
        table->rowCount() * table->verticalHeader()->defaultSectionSize() + 4;
    table->setFixedHeight(height);

    // 替代 layout->addWidget(table);
    QHBoxLayout* tableLayout = new QHBoxLayout;
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(0);
    tableLayout->addSpacing(28);
    tableLayout->addWidget(table);

    layout->addLayout(tableLayout);

    connect(toggleBtn, &QToolButton::toggled,
            this,
            [=](bool checked)
            {
                // checked = true  -> 展开
                // checked = false -> 收起

                table->setVisible(checked);

                if (checked)
                {
                    toggleBtn->setIcon(
                        QIcon(":/res/common/expand.svg"));
                    toggleBtn->setToolTip("收起");
                }
                else
                {
                    toggleBtn->setIcon(
                        QIcon(":/res/common/collapse.svg"));
                    toggleBtn->setToolTip("展开");
                }
            });

    return widget;
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


ProcessStationRightPanel::ProcessStationRightPanel(QWidget *parent)
    : BasePageWidget(parent)
{
    setupPage();
    // m_tabWidget 为 BasePageWidget 中的成员
    connect(tabBar(), &QTabBar::currentChanged,
            this, &ProcessStationRightPanel::updateTableModelByTab);

    updateTableModelByTab(tabBar()->currentIndex());
    // 初始化第一页
     // 3. 获取 ProcessStationModel
    auto model =
        qobject_cast<ProcessStationModel*>(m_model);
    if (model)
    {
        // ----------------------------------------------------
        // 4. 获取产品SN代理
        // ----------------------------------------------------
        //
        // 这里假设你在 ProcessStationModel 中提供了：
        //
        // ProductSnDelegate* productSnDelegate() const;
        //
        // 返回成员变量 m_productSnDelegate。
        // ----------------------------------------------------
        auto delegate = model->textLinkDelegate();

        if (delegate)
        {

            connect(delegate,
                    &TextLinkDelegate::linkClicked,
                    this,
                    [=](int row, const QString& field)
                    {qDebug() << __FUNCTION__ <<"""""sds"<<field;
                        // ------------------------------------
                        // 6. 获取该行完整数据
                        // ------------------------------------
                        QVariantMap rowData =
                            model->rowData(row);

                        // 产品SN
                        QString productSN =
                            rowData.value(field).toString();

                        // 其他字段也可以一起取出
                        QString materialCode =
                            rowData.value("materialCode").toString();

                        QString materialName =
                            rowData.value("materialName").toString();

                        // ------------------------------------
                        // 7. 在这里编写业务逻辑
                        // ------------------------------------
                        //
                        // 常见用途：
                        // - 打开产品详情窗口
                        // - 查询追溯信息
                        // - 打开维修记录
                        // - 调用接口获取完整信息
                        // ------------------------------------

                        qDebug() << "点击物料编码:";
                        qDebug() << "row =" << row;
                        qDebug() << "field =" << field;
                        qDebug() << "productSN =" << productSN;
                        qDebug() << "materialCode =" << materialCode;
                        qDebug() << "materialName =" << materialName;

                    });
        }
    }

    // --------------------------------------------------------
    // 8. 初始化显示第一个 Tab 的数据
    // --------------------------------------------------------
    updateTableModelByTab(tabBar()->currentIndex());
}

TabConfigs ProcessStationRightPanel::Tabs() const
{
    TabConfigs tabs;
    tabs.append({
        tr("物料核对"),
                 PageType::TABLE,
        QVariant::fromValue(PageType::TABLE)
    });

    tabs.append({
        tr("工艺路线"),
                 PageType::TABLE,
        QVariant::fromValue(PageType::TABLE)
    });

    tabs.append({
        tr("工艺文件"),
                 PageType::PDF,
        QVariant::fromValue(PageType::PDF)
    });

    tabs.append({
        tr("引用文件"),
                 PageType::PDF,
        QVariant::fromValue(PageType::PDF)
    });

    tabs.append({
        tr("工序物料信息"),
                 PageType::TABLE,
        QVariant::fromValue(PageType::TABLE)
    });

    tabs.append({
        tr("工具设备"),
                 PageType::TABLE,
        QVariant::fromValue(PageType::TABLE)
    });

    tabs.append({
        tr("资料上传"),
            PageType::NORMAL,
        QVariant::fromValue(PageType::NORMAL)
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
    // QVariant::fromValue(PageType::TABLE / PDF / NORMAL)

    PageType pageType = data.value<PageType>();

    // 只有 TABLE 类型才创建代理模型
    if (pageType != PageType::TABLE)
        return nullptr;

    auto proxy = new FieldFilterProxyModel(this);

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

void ProcessStationRightPanel::setCurrentSearchInfo(const QString &info)
{
m_currentSearchInfo = info;
     updateSearchInfo();  // BasePageWidget 中已有刷新接口
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

    case 4:
        model->setTableType(ProcessStationModel::ProcessMaterial);
        break;

    case 5:
        model->setTableType(ProcessStationModel::ToolEquipment);
        break;

    default:
        break;
    }
}
