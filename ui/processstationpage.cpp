#include "processstationpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QStyle>

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
    layout->setStretch(0, 1);
    layout->setStretch(1,1);
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
}

void ProcessStationLeftPanel::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0,0);
    mainLayout->setSpacing(8);

    m_taskInfo = createTaskWidget(tr("任务单信息"));
    m_taskStatus = createTaskWidget(tr("任务单状态"));
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

QWidget *ProcessStationLeftPanel::createTaskWidget(const QString &title)
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

    // table->setHorizontalHeaderLabels(QStringList() << "字段" << "内容");
    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    // 基本属性
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setFocusPolicy(Qt::NoFocus);
    table->setAlternatingRowColors(true);
    table->setShowGrid(true);

    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    // 初始化单元格
    for (int row = 0; row < 7; ++row)
    {
        table->setItem(row, 0, new QTableWidgetItem);
        table->setItem(row, 1, new QTableWidgetItem);
    }

    // 固定高度
    int height =
        table->rowCount() * table->verticalHeader()->defaultSectionSize() +
        4;
    table->setFixedHeight(height);

    layout->addWidget(table);
    // 样式：无外框，仅保留浅灰分隔线
    table->setStyleSheet(R"(
QTableWidget {
    border: none;
    background: white;
    alternate-background-color: #fafafa;
    gridline-color: #e8e8e8;
}

QTableWidget::item {
    border: none;
    padding: 4px 8px;
}
)");

    // 保存表格指针
    if (title == "任务单信息")
        m_taskInfoTable = table;
    else if (title == "任务单状态")
        m_taskStatusTable = table;

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
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);


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

    // 输入区域
    auto* row = new QHBoxLayout;

    QLineEdit* scanEdit = new QLineEdit(widget);
    scanEdit->setPlaceholderText("请扫描条码...");

    QPushButton* passBtn = new QPushButton("过站", widget);

    row->addWidget(scanEdit);
    row->addWidget(passBtn);

    layout->addLayout(row);
    layout->addStretch();

    return widget;
}

void ProcessStationLeftPanel::setTaskData(QTableWidget *table, const QList<QVariant> &values)
{
    if (!table)
        return;

    const int rowCount = qMin(table->rowCount(), values.size());

    for (int row = 0; row < rowCount; ++row)
    {
        // 第一列固定为字段名（后续可自行设置）
        if (!table->item(row, 0))
            table->setItem(row, 0, new QTableWidgetItem);

        if (!table->item(row, 1))
            table->setItem(row, 1, new QTableWidgetItem);

        // 这里只填充第二列的值
        table->item(row, 1)->setText(values[row].toString());
    }
}


ProcessStationRightPanel::ProcessStationRightPanel(QWidget *parent)
    : BasePageWidget(parent)
{
    setupPage();
}

TabConfigs ProcessStationRightPanel::Tabs() const
{
    TabConfigs tabs;
    tabs.append({
        tr("物料核对"),
        QVariant::fromValue(PageType::TABLE)
    });

    tabs.append({
        tr("工艺路线"),
        QVariant::fromValue(PageType::TABLE)
    });

    tabs.append({
        tr("工艺文件"),
        QVariant::fromValue(PageType::PDF)
    });

    tabs.append({
        tr("引用文件"),
        QVariant::fromValue(PageType::PDF)
    });

    tabs.append({
        tr("工序物料信息"),
        QVariant::fromValue(PageType::TABLE)
    });

    tabs.append({
        tr("工具设备"),
        QVariant::fromValue(PageType::TABLE)
    });

    tabs.append({
        tr("资料上传"),
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
    return tr("请输入物料信息");
}

QAbstractItemModel *ProcessStationRightPanel::createModel()
{
    return nullptr;
}

FieldFilterProxyModel *ProcessStationRightPanel::createProxy(const QVariant &data)
{
    return nullptr;
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

}
