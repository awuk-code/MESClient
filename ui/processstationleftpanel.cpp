#include "processstationleftpanel.h"
#include "basedialogwidget.h"

#include <QAbstractItemView>
#include <QButtonGroup>
#include <QDebug>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyle>
#include <QTableView>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

namespace
{
constexpr int STATUS_COL_NO = 0;
constexpr int STATUS_COL_PRODUCT_SN = 1;
constexpr int STATUS_COL_PASS = 2;
constexpr int STATUS_COL_NG = 3;
constexpr int STATUS_COL_PAUSE = 4;

class NgPassDialog : public BaseDialogWidget
{
public:
    explicit NgPassDialog(const QString& productSn, QWidget* parent = nullptr)
        : BaseDialogWidget(parent),
        m_productSn(productSn)
    {
        setTitle(tr("NG异常登记"));
        if (confirmButton())
            confirmButton()->setText(tr("提交"));
        if (cancelButton())
            cancelButton()->setText(tr("取消"));

        auto snLabel = new QLabel(tr("产品SN：%1").arg(productSn), this);
        m_reasonEdit = new QTextEdit(this);
        m_reasonEdit->setPlaceholderText(tr("请输入NG原因或异常说明"));
        m_reasonEdit->setMinimumHeight(120);

        contentLayout()->addWidget(snLabel);
        contentLayout()->addWidget(m_reasonEdit);
    }

    QString reason() const
    {
        return m_reasonEdit ? m_reasonEdit->toPlainText().trimmed() : QString();
    }

protected:
    bool onConfirm() override
    {
        // NG弹窗先做最基础的非空校验，后续可以在这里补充异常类型、图片等必填项。
        if (reason().isEmpty())
        {
            QMessageBox::warning(this, tr("NG异常登记"), tr("请填写NG原因。"));
            return false;
        }

        qDebug() << __FUNCTION__
                 << "productSn:" << m_productSn
                 << "reason:" << reason();
        return true;
    }

private:
    QString m_productSn;
    QTextEdit* m_reasonEdit{nullptr};
};
}

ProcessStationLeftPanel::ProcessStationLeftPanel(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("ProcessStationLeftPanel");
    initUI();
    initConnect();

    // 工序站点默认可能由侧边栏直接进入，此时没有任务行数据：
    // 只初始化字段名称，不显示 JSON 虚拟数据；点击开工/返工任务单后再由行数据填充。
    setTaskInfoData(QVariantMap());
    setTaskStatusData(QVariantMap());
    setAbnormalInfoData(QVariantMap());
    setReworkTaskStatusData(QVariantMap());

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
    // 标题等级样式入口：左侧二级标题，后续修改字体时在 QSS 的 sectionTitle 中统一设置。
    text->setProperty("labelRole", "sectionTitle");
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
    toggleBtn->setProperty("buttonRole", "panelToggle");

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
    label->setProperty("labelRole", "fieldName");
    return label;
}

QLabel *ProcessStationLeftPanel::createInfoValueLabel(const QString &text, QWidget *parentWidget)
{
    auto label = new QLabel(text, parentWidget);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setWordWrap(true);
    label->setProperty("labelRole", "fieldValue");
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
    // 标题等级样式入口：左侧二级标题，后续修改字体时在 QSS 的 sectionTitle 中统一设置。
    text->setProperty("labelRole", "sectionTitle");

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

        // 过站方式需要在点击“执行”时读取，所以保存为成员变量。
        m_scanInRadio = new QRadioButton(tr("扫入"), contentWidget);
        m_passRadio   = new QRadioButton(tr("PASS"), contentWidget);
        m_ngRadio     = new QRadioButton(tr("NG"), contentWidget);

        auto passModeGroup = new QButtonGroup(contentWidget);
        passModeGroup->addButton(m_scanInRadio);
        passModeGroup->addButton(m_passRadio);
        passModeGroup->addButton(m_ngRadio);

        // 默认选中“扫入”
        m_scanInRadio->setChecked(true);

        // 暂停/解除暂停可以随时操作当前输入框产品SN，或当前选中的状态表行。
        m_pauseBtn      = new QPushButton(tr("暂停"), contentWidget);
        m_resumeBtn     = new QPushButton(tr("解除暂停"), contentWidget);
        QPushButton* rollbackBtn   = new QPushButton(tr("工序回退"), contentWidget);

        row->addWidget(label);
        row->addWidget(m_scanInRadio);
        row->addWidget(m_passRadio);
        row->addWidget(m_ngRadio);
        row->addSpacing(12);
        row->addWidget(m_pauseBtn);
        row->addWidget(m_resumeBtn);
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

        m_scanEdit = new QLineEdit(contentWidget);
        m_scanEdit->setPlaceholderText(tr("请扫描产品条码..."));

        m_executeBtn = new QPushButton(tr("执行"), contentWidget);

        row->addWidget(label);
        row->addWidget(m_scanEdit);
        row->addWidget(m_executeBtn);
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
        m_statusTableView = new QTableView(contentWidget);

        m_statusModel =
            new QStandardItemModel(0, 5, m_statusTableView);

        m_statusModel->setHorizontalHeaderLabels(QStringList()
                                                 << tr("序号")
                                                 << tr("产品SN")
                                                 << tr("PASS")
                                                 << tr("NG")
                                                 << tr("暂停"));

        m_statusTableView->setModel(m_statusModel);

        // 表格属性
        m_statusTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_statusTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_statusTableView->setSelectionMode(QAbstractItemView::SingleSelection);
        m_statusTableView->setAlternatingRowColors(true);
        m_statusTableView->verticalHeader()->hide();

        // 列宽按比例自动拉伸
        m_statusTableView->horizontalHeader()->setSectionResizeMode(
            QHeaderView::Stretch);

        // 固定一个合适的高度
        m_statusTableView->setMinimumHeight(180);

        // 布局：
        // label 固定宽度，tableView 占据剩余空间
        row->addWidget(m_statusTableView, 1);

        // 加入内容布局
        contentVBox->addLayout(row);
    }

    // 将内容区域加入外层布局
    contentLayout->addWidget(contentWidget);
    layout->addLayout(contentLayout);

    bindPassWidgetActions();

    return widget;
}

void ProcessStationLeftPanel::bindPassWidgetActions()
{
    if (m_executeBtn)
    {
        connect(m_executeBtn, &QPushButton::clicked,
                this, &ProcessStationLeftPanel::onExecuteBtnClicked);
    }

    if (m_scanEdit)
    {
        connect(m_scanEdit, &QLineEdit::returnPressed,
                this, &ProcessStationLeftPanel::onExecuteBtnClicked);
    }

    if (m_pauseBtn)
    {
        connect(m_pauseBtn, &QPushButton::clicked,
                this, &ProcessStationLeftPanel::onPauseBtnClicked);
    }

    if (m_resumeBtn)
    {
        connect(m_resumeBtn, &QPushButton::clicked,
                this, &ProcessStationLeftPanel::onResumeBtnClicked);
    }
}

void ProcessStationLeftPanel::onExecuteBtnClicked()
{
    if (m_scanInRadio && m_scanInRadio->isChecked())
    {
        const QString productSn = m_scanEdit ? m_scanEdit->text().trimmed() : QString();
        if (!validateProductSn(productSn))
            return;

        handleScanInAction(productSn);

        if (m_scanEdit)
            m_scanEdit->clear();
    }
    else if (m_passRadio && m_passRadio->isChecked())
    {
        // PASS 针对状态信息表当前选中的产品，不再判断输入框内容。
        const QString productSn = currentProductSnFromSelection();
        if (!validateSelectedProductSn(productSn))
            return;

        handlePassResult(productSn);
    }
    else if (m_ngRadio && m_ngRadio->isChecked())
    {
        // NG 针对状态信息表当前选中的产品，不再判断输入框内容。
        const QString productSn = currentProductSnFromSelection();
        if (!validateSelectedProductSn(productSn))
            return;

        handleNgResult(productSn);
    }
}

void ProcessStationLeftPanel::onPauseBtnClicked()
{
    handlePauseResult(true);
}

void ProcessStationLeftPanel::onResumeBtnClicked()
{
    handlePauseResult(false);
}

void ProcessStationLeftPanel::handleScanInAction(const QString& productSn)
{
    int row = statusRowForProductSn(productSn);
    if (row < 0)
        row = appendStatusRow(productSn);

    if (m_statusTableView)
        m_statusTableView->selectRow(row);

    // 扫入只表示产品进入当前工序处理队列，不直接改变 PASS/NG 汇总数量。
    updateTaskStatusRealtime();
    qDebug() << __FUNCTION__ << "scan in productSn:" << productSn << "row:" << row;
}

void ProcessStationLeftPanel::handlePassResult(const QString& productSn)
{
    const int row = statusRowForProductSn(productSn);
    if (row < 0)
    {
        QMessageBox::warning(this, tr("扫码过站"), tr("请先扫入该产品SN。"));
        qDebug() << __FUNCTION__ << "product not scanned:" << productSn;
        return;
    }

    if (!validatePassCondition(productSn))
        return;

    setStatusMark(row, STATUS_COL_PASS, true, QColor(0, 150, 80));
    setStatusMark(row, STATUS_COL_NG, false, QColor());
    updateTaskStatusRealtime();

    qDebug() << __FUNCTION__ << "PASS productSn:" << productSn << "row:" << row;
}

void ProcessStationLeftPanel::handleNgResult(const QString& productSn)
{
    const int row = statusRowForProductSn(productSn);
    if (row < 0)
    {
        QMessageBox::warning(this, tr("扫码过站"), tr("请先扫入该产品SN。"));
        qDebug() << __FUNCTION__ << "product not scanned:" << productSn;
        return;
    }

    NgPassDialog dialog(productSn, this);
    if (dialog.exec() != QDialog::Accepted)
    {
        qDebug() << __FUNCTION__ << "NG canceled productSn:" << productSn;
        return;
    }

    setStatusMark(row, STATUS_COL_NG, true, QColor(210, 72, 72));
    setStatusMark(row, STATUS_COL_PASS, false, QColor());
    updateTaskStatusRealtime();

    qDebug() << __FUNCTION__
             << "NG productSn:" << productSn
             << "reason:" << dialog.reason()
             << "row:" << row;
}

void ProcessStationLeftPanel::handlePauseResult(bool paused)
{
    // 暂停/解除暂停针对状态信息表当前选中的产品，不再判断输入框内容。
    const QString productSn = currentProductSnFromSelection();
    if (!validateSelectedProductSn(productSn))
        return;

    const int row = statusRowForProductSn(productSn);
    if (row < 0)
    {
        QMessageBox::warning(this, tr("扫码过站"), tr("请先扫入该产品SN。"));
        qDebug() << __FUNCTION__ << "product not scanned:" << productSn;
        return;
    }

    setStatusMark(row, STATUS_COL_PAUSE, paused, QColor(230, 145, 56));
    updateTaskStatusRealtime();

    qDebug() << __FUNCTION__
             << "productSn:" << productSn
             << "paused:" << paused
             << "row:" << row;
}

bool ProcessStationLeftPanel::validateProductSn(const QString& productSn) const
{
    if (!productSn.isEmpty())
        return true;

    QMessageBox::warning(
        const_cast<ProcessStationLeftPanel*>(this),
        tr("扫码过站"),
        tr("请先扫描或输入产品SN。"));
    qDebug() << __FUNCTION__ << "empty productSn";
    return false;
}

bool ProcessStationLeftPanel::validateSelectedProductSn(const QString& productSn) const
{
    if (!productSn.isEmpty())
        return true;

    QMessageBox::warning(
        const_cast<ProcessStationLeftPanel*>(this),
        tr("扫码过站"),
        tr("请先在状态信息中选中产品SN。"));
    qDebug() << __FUNCTION__ << "empty selected productSn";
    return false;
}

bool ProcessStationLeftPanel::validatePassCondition(const QString& productSn) const
{
    Q_UNUSED(productSn)

    // PASS 前应校验右侧物料/工艺数据是否填写完成，以及资料上传是否完成。
    // 目前右侧表格和上传页还没有统一的完成状态接口，所以这里先保留统一入口。
    // 后续接入时只需要在这个函数中读取右侧状态，不需要改扫码过站主流程。
    qDebug() << __FUNCTION__ << "right-side material/upload validation reserved";
    return true;
}

QString ProcessStationLeftPanel::currentProductSnFromSelection() const
{
    if (!m_statusTableView || !m_statusModel)
        return QString();

    const QModelIndex currentIndex = m_statusTableView->currentIndex();
    if (!currentIndex.isValid())
        return QString();

    return m_statusModel->index(currentIndex.row(), STATUS_COL_PRODUCT_SN)
        .data(Qt::DisplayRole)
        .toString();
}

int ProcessStationLeftPanel::statusRowForProductSn(const QString& productSn) const
{
    return m_productStatusRows.value(productSn, -1);
}

int ProcessStationLeftPanel::appendStatusRow(const QString& productSn)
{
    if (!m_statusModel)
        return -1;

    const int row = m_statusModel->rowCount();
    m_statusModel->insertRow(row);
    m_statusModel->setItem(row, STATUS_COL_NO,
                           new QStandardItem(QString::number(row + 1)));
    m_statusModel->setItem(row, STATUS_COL_PRODUCT_SN,
                           new QStandardItem(productSn));
    m_statusModel->setItem(row, STATUS_COL_PASS, new QStandardItem);
    m_statusModel->setItem(row, STATUS_COL_NG, new QStandardItem);
    m_statusModel->setItem(row, STATUS_COL_PAUSE, new QStandardItem);

    m_productStatusRows.insert(productSn, row);
    return row;
}

void ProcessStationLeftPanel::setStatusMark(
    int row,
    int column,
    bool checked,
    const QColor& color)
{
    if (!m_statusModel || row < 0 || row >= m_statusModel->rowCount())
        return;

    auto item = m_statusModel->item(row, column);
    if (!item)
    {
        item = new QStandardItem;
        m_statusModel->setItem(row, column, item);
    }

    item->setText(checked ? QStringLiteral("✓") : QString());
    item->setTextAlignment(Qt::AlignCenter);
    item->setForeground(checked ? QBrush(color) : QBrush());

    QFont font = item->font();
    font.setBold(checked);
    item->setFont(font);
}

void ProcessStationLeftPanel::updateTaskStatusRealtime()
{
    if (!m_statusModel)
        return;

    int passCount = 0;
    int ngCount = 0;
    for (int row = 0; row < m_statusModel->rowCount(); ++row)
    {
        if (!m_statusModel->index(row, STATUS_COL_PASS).data(Qt::DisplayRole).toString().isEmpty())
            ++passCount;
        if (!m_statusModel->index(row, STATUS_COL_NG).data(Qt::DisplayRole).toString().isEmpty())
            ++ngCount;
    }

    m_taskStatusData.insert("finishedCount", passCount);
    m_taskStatusData.insert("ngCount", ngCount);

    if (m_displayMode == DisplayMode::NormalTask)
        setTaskData(m_taskStatusLayout, taskStatusFields(), m_taskStatusData);

    qDebug() << __FUNCTION__
             << "finishedCount:" << passCount
             << "ngCount:" << ngCount;
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
        const QString value = rowData.value(field).toString();

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

void ProcessStationLeftPanel::clearPassStatus()
{
    // 切换任务或侧边栏直接进入时，清空当前工序的产品实时过站表。
    // 任务单状态中的完成数/NG数也同步归零，避免沿用上一个产品的实时状态。
    m_productStatusRows.clear();
    if (m_statusModel)
        m_statusModel->removeRows(0, m_statusModel->rowCount());

    m_taskStatusData.insert("finishedCount", 0);
    m_taskStatusData.insert("ngCount", 0);

    if (m_displayMode == DisplayMode::NormalTask)
        setTaskData(m_taskStatusLayout, taskStatusFields(), m_taskStatusData);

    qDebug() << __FUNCTION__ << "pass status cleared";
}
