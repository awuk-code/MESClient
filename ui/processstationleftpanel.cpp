#include "processstationleftpanel.h"
#include "basedialogwidget.h"
#include "processstationinfosection.h"

#include <QAbstractItemView>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QStringList>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyle>
#include <QTableView>
#include <QTextEdit>
#include <QVBoxLayout>

namespace
{
constexpr int STATUS_COL_NO = 0;
constexpr int STATUS_COL_PRODUCT_SN = 1;
constexpr int STATUS_COL_PASS = 2;
constexpr int STATUS_COL_NG = 3;
constexpr int STATUS_COL_PAUSE = 4;

QString formatDuration(qint64 seconds)
{
    const qint64 safeSeconds = qMax<qint64>(0, seconds);
    const qint64 h = safeSeconds / 3600;
    const qint64 m = (safeSeconds % 3600) / 60;
    const qint64 s = safeSeconds % 60;
    return QStringLiteral("%1:%2:%3")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'));
}

class NgAbnormalDialog : public BaseDialogWidget
{
public:
    explicit NgAbnormalDialog(const QString& productSn, QWidget* parent = nullptr)
        : BaseDialogWidget(parent),
          m_productSn(productSn)
    {
        setTitle(tr("NG异常登记"));
        if (confirmButton())
            confirmButton()->setText(tr("确认"));
        if (cancelButton())
            cancelButton()->setText(tr("取消"));

        auto snRow = new QHBoxLayout;
        snRow->addWidget(new QLabel(tr("产品SN："), this));
        m_productSnCombo = new QComboBox(this);
        m_productSnCombo->addItem(productSn);
        m_productSnCombo->setMinimumWidth(220);
        snRow->addWidget(m_productSnCombo);
        snRow->addStretch();
        contentLayout()->addLayout(snRow);

        auto typeRow = new QHBoxLayout;
        typeRow->addWidget(new QLabel(tr("异常类型："), this));
        m_typeCombo = new QComboBox(this);
        m_typeCombo->addItems({
            tr("外观异常"),
            tr("结构异常")
        });
        m_typeCombo->setMinimumWidth(220);
        typeRow->addWidget(m_typeCombo);
        typeRow->addStretch();
        contentLayout()->addLayout(typeRow);

        auto imageRow = new QHBoxLayout;
        imageRow->addWidget(new QLabel(tr("图片上传："), this));
        m_imageEdit = new QLineEdit(this);
        m_imageEdit->setReadOnly(true);
        m_imageEdit->setPlaceholderText(tr("请选择异常图片"));
        auto browseBtn = new QPushButton(tr("上传图片"), this);
        connect(browseBtn, &QPushButton::clicked, this, [this]() {
            const QString filePath = QFileDialog::getOpenFileName(
                this,
                tr("选择异常图片"),
                QString(),
                tr("图片文件 (*.png *.jpg *.jpeg *.bmp)"));
            if (!filePath.isEmpty() && m_imageEdit)
                m_imageEdit->setText(filePath);
        });
        imageRow->addWidget(m_imageEdit, 1);
        imageRow->addWidget(browseBtn);
        contentLayout()->addLayout(imageRow);

        contentLayout()->addWidget(new QLabel(tr("异常现象说明"), this));
        m_phenomenonEdit = new QTextEdit(this);
        m_phenomenonEdit->setPlaceholderText(tr("请填写异常现象、判定依据或现场说明"));
        m_phenomenonEdit->setMinimumHeight(90);
        contentLayout()->addWidget(m_phenomenonEdit);

        auto nextRow = new QHBoxLayout;
        nextRow->addWidget(new QLabel(tr("下一工序："), this));
        m_nextProcessLabel = new QLabel(tr("维修站"), this);
        m_nextProcessLabel->setMinimumWidth(220);
        nextRow->addWidget(m_nextProcessLabel);
        nextRow->addStretch();
        contentLayout()->addLayout(nextRow);
    }

    QString abnormalType() const
    {
        return m_typeCombo ? m_typeCombo->currentText() : QString();
    }

    QString phenomenon() const
    {
        return m_phenomenonEdit ? m_phenomenonEdit->toPlainText().trimmed() : QString();
    }

    QString imagePath() const
    {
        return m_imageEdit ? m_imageEdit->text().trimmed() : QString();
    }

    QString nextProcess() const
    {
        return m_nextProcessLabel ? m_nextProcessLabel->text() : QString();
    }

    QString summary() const
    {
        return tr("异常类型：%1；异常现象：%2；异常图片：%3；下一工序：%4")
            .arg(abnormalType(), phenomenon(), imagePath(), nextProcess());
    }

protected:
    bool onConfirm() override
    {
        if (abnormalType().isEmpty())
        {
            QMessageBox::warning(this, tr("NG异常登记"), tr("请选择异常类型。"));
            return false;
        }

        if (imagePath().isEmpty())
        {
            QMessageBox::warning(this, tr("NG异常登记"), tr("请上传异常图片。"));
            return false;
        }

        if (phenomenon().isEmpty())
        {
            QMessageBox::warning(this, tr("NG异常登记"), tr("请填写异常现象说明。"));
            return false;
        }

        qDebug() << __FUNCTION__
                 << "productSn:" << m_productSn
                 << "type:" << abnormalType()
                 << "image:" << imagePath()
                 << "phenomenon:" << phenomenon()
                 << "nextProcess:" << nextProcess();
        return true;
    }

private:
    QString m_productSn;
    QComboBox* m_productSnCombo{nullptr};
    QComboBox* m_typeCombo{nullptr};
    QLineEdit* m_imageEdit{nullptr};
    QTextEdit* m_phenomenonEdit{nullptr};
    QLabel* m_nextProcessLabel{nullptr};
};

class PauseReasonDialog : public BaseDialogWidget
{
public:
    explicit PauseReasonDialog(const QString& productSn, QWidget* parent = nullptr)
        : BaseDialogWidget(parent)
    {
        setTitle(tr("暂停原因登记"));
        if (confirmButton())
            confirmButton()->setText(tr("确认暂停"));

        auto snRow = new QHBoxLayout;
        snRow->addWidget(new QLabel(tr("产品SN："), this));
        m_productSnCombo = new QComboBox(this);
        m_productSnCombo->addItem(productSn);
        m_productSnCombo->setMinimumWidth(220);
        snRow->addWidget(m_productSnCombo);
        snRow->addStretch();
        contentLayout()->addLayout(snRow);

        auto typeRow = new QHBoxLayout;
        typeRow->addWidget(new QLabel(tr("暂停类型："), this));

        const QStringList types = {
            tr("物料异常"),
            tr("设备仪器工装异常"),
            tr("工艺文件异常"),
            tr("环境异常"),
            tr("人员调度"),
            tr("其他")
        };

        m_typeGroup = new QButtonGroup(this);
        m_typeGroup->setExclusive(true);

        for (const QString& type : types)
        {
            auto checkBox = new QCheckBox(type, this);
            m_typeCheckBoxes.append(checkBox);
            m_typeGroup->addButton(checkBox);
            typeRow->addWidget(checkBox);
            connect(checkBox, &QCheckBox::toggled,
                    this, [this]() { updateCodePrompt(); });
        }
        typeRow->addStretch();
        contentLayout()->addLayout(typeRow);

        m_codeRowWidget = new QWidget(this);
        auto codeRow = new QHBoxLayout(m_codeRowWidget);
        codeRow->setContentsMargins(0, 0, 0, 0);
        m_codeLabel = new QLabel(tr("关联编码："), m_codeRowWidget);
        m_codeEdit = new QLineEdit(m_codeRowWidget);
        codeRow->addWidget(m_codeLabel);
        codeRow->addWidget(m_codeEdit, 1);
        contentLayout()->addWidget(m_codeRowWidget);

        contentLayout()->addWidget(new QLabel(tr("具体说明"), this));
        m_descriptionEdit = new QTextEdit(this);
        m_descriptionEdit->setMinimumHeight(90);
        m_descriptionEdit->setPlaceholderText(tr("请填写暂停原因或现场说明"));
        contentLayout()->addWidget(m_descriptionEdit);

        updateCodePrompt();
    }

    QString pauseType() const
    {
        return selectedTypes().join(tr("、"));
    }

    QString relatedCode() const
    {
        return m_codeEdit ? m_codeEdit->text().trimmed() : QString();
    }

    QString description() const
    {
        return m_descriptionEdit ? m_descriptionEdit->toPlainText().trimmed() : QString();
    }

protected:
    bool onConfirm() override
    {
        const QStringList types = selectedTypes();
        if (types.isEmpty())
        {
            QMessageBox::warning(this, tr("暂停原因登记"), tr("请至少勾选一种暂停类型。"));
            return false;
        }

        if (relatedCode().isEmpty())
        {
            QMessageBox::warning(
                this,
                tr("暂停原因登记"),
                tr("请填写%1编码。").arg(types.join(tr("、"))));
            return false;
        }

        if (description().isEmpty())
        {
            QMessageBox::warning(this, tr("暂停原因登记"), tr("请填写具体说明。"));
            return false;
        }

        return true;
    }

private:
    QStringList selectedTypes() const
    {
        QStringList types;

        if (m_typeGroup && m_typeGroup->checkedButton())
            types.append(m_typeGroup->checkedButton()->text());

        // 多选逻辑已停用：暂停类型现在只能单选。
        // for (auto checkBox : m_typeCheckBoxes)
        // {
        //     if (checkBox && checkBox->isChecked())
        //         types.append(checkBox->text());
        // }

        return types;
    }

    void updateCodePrompt()
    {
        const QStringList types = selectedTypes();
        const bool hasSelection = !types.isEmpty();

        if (m_codeRowWidget)
            m_codeRowWidget->setVisible(hasSelection);

        if (!hasSelection || !m_codeEdit || !m_codeLabel)
            return;

        const QString typeText = types.join(tr("、"));
        m_codeLabel->setText(tr("关联编码："));
        m_codeEdit->setPlaceholderText(tr("请输入%1编码").arg(typeText));
    }

private:
    QComboBox* m_productSnCombo{nullptr};
    QButtonGroup* m_typeGroup{nullptr};
    QList<QCheckBox*> m_typeCheckBoxes;
    QWidget* m_codeRowWidget{nullptr};
    QLabel* m_codeLabel{nullptr};
    QLineEdit* m_codeEdit{nullptr};
    QTextEdit* m_descriptionEdit{nullptr};
};

class ResumeReasonDialog : public BaseDialogWidget
{
public:
    explicit ResumeReasonDialog(const QString& productSn, QWidget* parent = nullptr)
        : BaseDialogWidget(parent)
    {
        setTitle(tr("解除暂停原因登记"));
        if (confirmButton())
            confirmButton()->setText(tr("确认解除"));
        if (cancelButton())
            cancelButton()->setText(tr("取消"));

        auto snRow = new QHBoxLayout;
        snRow->addWidget(new QLabel(tr("产品SN："), this));
        m_productSnCombo = new QComboBox(this);
        m_productSnCombo->addItem(productSn);
        m_productSnCombo->setMinimumWidth(220);
        snRow->addWidget(m_productSnCombo);
        snRow->addStretch();
        contentLayout()->addLayout(snRow);

        contentLayout()->addWidget(new QLabel(tr("解除暂停理由"), this));
        m_reasonEdit = new QTextEdit(this);
        m_reasonEdit->setMinimumHeight(90);
        m_reasonEdit->setPlaceholderText(tr("请填写解除暂停原因或现场处理说明"));
        contentLayout()->addWidget(m_reasonEdit);
    }

    QString reason() const
    {
        return m_reasonEdit ? m_reasonEdit->toPlainText().trimmed() : QString();
    }

protected:
    bool onConfirm() override
    {
        if (reason().isEmpty())
        {
            QMessageBox::warning(this, tr("解除暂停原因登记"), tr("请填写解除暂停理由。"));
            return false;
        }

        return true;
    }

private:
    QComboBox* m_productSnCombo{nullptr};
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

    m_taskInfo = new ProcessStationInfoSection(tr("任务单信息"), this);
    m_abnormalInfo = new ProcessStationInfoSection(tr("异常品信息"), this);
    m_taskStatus = new ProcessStationInfoSection(tr("任务单状态"), this);
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
        typeBtn->setProperty("buttonRole", "compact");

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

        QLabel* label = new QLabel(
            tr("<span style='color:#f56c6c;'>*</span>过站方式："),
            contentWidget);

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
        m_pauseBtn->setProperty("buttonRole", "compact");
        m_resumeBtn->setProperty("buttonRole", "compact");
        rollbackBtn->setProperty("buttonRole", "compact");

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

        QLabel* label = new QLabel(
            tr("<span style='color:#f56c6c;'>*</span>扫码操作："),
            contentWidget);

        m_scanEdit = new QLineEdit(contentWidget);
        m_scanEdit->setPlaceholderText(tr("请扫描产品条码..."));

        m_executeBtn = new QPushButton(tr("执行"), contentWidget);
        m_executeBtn->setProperty("buttonRole", "compact");

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
        QLabel* label = new QLabel(
            tr("<span style='color:#f56c6c;'>*</span>状态信息："),
            contentWidget);
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
    }
    else if (m_passRadio && m_passRadio->isChecked())
    {
        // PASS 针对输入框中的产品SN，再到状态信息表中查找对应行。
        const QString productSn = m_scanEdit ? m_scanEdit->text().trimmed() : QString();
        if (!validateProductSn(productSn))
            return;

        handlePassResult(productSn);
    }
    else if (m_ngRadio && m_ngRadio->isChecked())
    {
        // NG 针对输入框中的产品SN，再到状态信息表中查找对应行。
        const QString productSn = m_scanEdit ? m_scanEdit->text().trimmed() : QString();
        if (!validateProductSn(productSn))
            return;

        handleNgResult(productSn);
    }
}

void ProcessStationLeftPanel::onPauseBtnClicked()
{
    const QString productSn = productSnForPauseAction();
    if (!validateProductSn(productSn))
        return;
    if (!validateProductNotPassed(productSn, tr("暂停")))
        return;
    if (!validateProductNotNg(productSn, tr("暂停")))
        return;
    if (isProductPaused(productSn))
    {
        QMessageBox::warning(this, tr("扫码过站"), tr("该产品已暂停。"));
        return;
    }

    PauseReasonDialog dialog(productSn, this);
    if (dialog.exec() != QDialog::Accepted)
    {
        qDebug() << __FUNCTION__ << "pause canceled productSn:" << productSn;
        return;
    }

    qDebug() << __FUNCTION__
             << "productSn:" << productSn
             << "type:" << dialog.pauseType()
             << "code:" << dialog.relatedCode()
             << "description:" << dialog.description();

    handlePauseResult(true);
}

void ProcessStationLeftPanel::onResumeBtnClicked()
{
    const QString productSn = productSnForPauseAction();
    if (!validateProductSn(productSn))
        return;
    if (!validateProductNotPassed(productSn, tr("解除暂停")))
        return;
    if (!validateProductNotNg(productSn, tr("解除暂停")))
        return;

    const int row = statusRowForProductSn(productSn);
    if (row < 0)
    {
        QMessageBox::warning(this, tr("扫码过站"), tr("请先扫入该产品SN。"));
        qDebug() << __FUNCTION__ << "product not scanned:" << productSn;
        return;
    }

    if (!isProductPaused(productSn))
    {
        QMessageBox::warning(this, tr("扫码过站"), tr("该产品当前未暂停。"));
        return;
    }

    ResumeReasonDialog dialog(productSn, this);
    if (dialog.exec() != QDialog::Accepted)
    {
        qDebug() << __FUNCTION__ << "resume canceled productSn:" << productSn;
        return;
    }

    qDebug() << __FUNCTION__
             << "productSn:" << productSn
             << "reason:" << dialog.reason();

    handlePauseResult(false);
}

void ProcessStationLeftPanel::handleScanInAction(const QString& productSn)
{
    if (!validateProductSnForCurrentTask(productSn))
        return;

    int row = statusRowForProductSn(productSn);
    if (row >= 0 && isProductPassed(productSn))
    {
        if (m_statusTableView)
            m_statusTableView->selectRow(row);

        QMessageBox::warning(this, tr("扫码过站"), tr("该产品已PASS，不能重复操作。"));
        qDebug() << __FUNCTION__ << "product already passed:" << productSn;
        return;
    }

    if (row >= 0 && isProductNg(productSn))
    {
        if (m_statusTableView)
            m_statusTableView->selectRow(row);

        QMessageBox::warning(this, tr("扫码过站"), tr("该产品已NG，已转入维修站。"));
        qDebug() << __FUNCTION__ << "product already NG:" << productSn;
        return;
    }

    if (row >= 0)
    {
        if (m_statusTableView)
            m_statusTableView->selectRow(row);

        emit productSnScanned(productSn);
        QMessageBox::information(this, tr("扫码过站"), tr("此SN码已扫入，请确认当前状态。"));
        qDebug() << __FUNCTION__ << "product already scanned:" << productSn << "row:" << row;
        return;
    }

    if (row < 0)
        row = appendStatusRow(productSn);

    if (m_statusTableView)
        m_statusTableView->selectRow(row);

    startProductTiming(productSn);
    // 扫入只表示产品进入当前工序处理队列，不直接改变 PASS/NG 汇总数量。
    updateTaskStatusRealtime();
    emit productSnScanned(productSn);
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
    if (!validateProductNotPassed(productSn, tr("PASS")))
        return;
    if (!validateProductNotNg(productSn, tr("PASS")))
        return;
    if (!validateProductNotPaused(productSn, tr("PASS")))
        return;

    const int ret = QMessageBox::question(
        this,
        tr("扫码过站"),
        tr("确认该产品通过当前工序？"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (ret != QMessageBox::Yes)
    {
        qDebug() << __FUNCTION__ << "PASS canceled productSn:" << productSn;
        return;
    }

    setStatusMark(row, STATUS_COL_PASS, true, QColor(0, 150, 80));
    setStatusMark(row, STATUS_COL_NG, false, QColor());
    const qint64 elapsedSeconds = finishProductTiming(productSn);
    updateTaskStatusRealtime();

    qDebug() << __FUNCTION__
             << "PASS productSn:" << productSn
             << "row:" << row
             << "elapsed:" << formatDuration(elapsedSeconds);
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
    if (!validateProductNotPassed(productSn, tr("NG")))
        return;
    if (!validateProductNotNg(productSn, tr("NG")))
        return;
    if (!validateProductNotPaused(productSn, tr("NG")))
        return;

    NgAbnormalDialog dialog(productSn, this);
    if (dialog.exec() != QDialog::Accepted)
    {
        qDebug() << __FUNCTION__ << "NG canceled productSn:" << productSn;
        return;
    }

    setStatusMark(row, STATUS_COL_NG, true, QColor(210, 72, 72));
    setStatusMark(row, STATUS_COL_PASS, false, QColor());
    const qint64 elapsedSeconds = finishProductTiming(productSn);

    QVariantMap abnormalData;
    abnormalData.insert("productSN", productSn);
    abnormalData.insert("abnormalType", dialog.abnormalType());
    abnormalData.insert("abnormalPhenomenon", dialog.phenomenon());
    abnormalData.insert("abnormalImage", dialog.imagePath());
    abnormalData.insert("nextProcess", dialog.nextProcess());
    abnormalData.insert("reportTime", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    abnormalData.insert("status", tr("待处理"));
    abnormalData.insert("elapsedSeconds", elapsedSeconds);
    // TODO(backend): NG 确认后应调用后台接口生成异常处理单，并由后台推送/刷新维修站待处理列表。
    emit productNgSubmitted(abnormalData);
    updateTaskStatusRealtime();

    qDebug() << __FUNCTION__
             << "NG productSn:" << productSn
             << "abnormal:" << dialog.summary()
             << "row:" << row
             << "elapsed:" << formatDuration(elapsedSeconds);
}

void ProcessStationLeftPanel::handlePauseResult(bool paused)
{
    // 单件过站时优先使用当前选中行；没有选中行时再使用输入框 SN。
    // TODO(backend): 批量暂停/批量解除暂停应由后台返回批量 SN 状态并统一提交暂停记录。
    const QString productSn = productSnForPauseAction();
    if (!validateProductSn(productSn))
        return;

    const int row = statusRowForProductSn(productSn);
    if (row < 0)
    {
        QMessageBox::warning(this, tr("扫码过站"), tr("请先扫入该产品SN。"));
        qDebug() << __FUNCTION__ << "product not scanned:" << productSn;
        return;
    }
    if (!paused && !isProductPaused(productSn))
    {
        QMessageBox::warning(this, tr("扫码过站"), tr("该产品当前未暂停。"));
        return;
    }

    setStatusMark(row, STATUS_COL_PAUSE, paused, QColor(230, 145, 56));
    if (paused)
        pauseProductTiming(productSn);
    else
        resumeProductTiming(productSn);
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

bool ProcessStationLeftPanel::validateProductSnForCurrentTask(const QString& productSn) const
{
    if (!validateProductSn(productSn))
        return false;

    const QString reworkProductSn = m_abnormalInfoData.value("productSN").toString().trimmed();
    if (m_displayMode == DisplayMode::ReworkTask &&
        !reworkProductSn.isEmpty() &&
        productSn != reworkProductSn)
    {
        QMessageBox::warning(
            const_cast<ProcessStationLeftPanel*>(this),
            tr("扫码过站"),
            tr("当前返工任务单产品SN为%1，不能扫入%2。").arg(reworkProductSn, productSn));
        return false;
    }

    // TODO(backend): 正常生产任务单需要后台校验 SN 是否属于当前任务单、产品型号是否匹配、
    // 当前 SN 是否处于本工序；当前本地 JSON 数据没有任务单 SN 清单，只能先保留统一入口。
    return true;
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
    if (m_passConditionValidator)
    {
        QString message;
        const bool ok = m_passConditionValidator(productSn, &message);
        if (!ok)
        {
            QMessageBox::warning(
                const_cast<ProcessStationLeftPanel*>(this),
                tr("扫码过站"),
                message.isEmpty() ? tr("右侧工艺信息未完成，不能PASS。") : message);
            return false;
        }

        return true;
    }

    // TODO(backend): PASS 前应由后台返回当前工序需要校验的物料、工具设备、资料上传项；
    // 当前如果没有注入右侧校验器，则只保留入口并放行，避免单独使用左侧面板时无法过站。
    qDebug() << __FUNCTION__ << "right-side material/upload validation not injected";
    return true;
}

bool ProcessStationLeftPanel::isProductPassed(const QString& productSn) const
{
    return isStatusMarked(statusRowForProductSn(productSn), STATUS_COL_PASS);
}

bool ProcessStationLeftPanel::isProductNg(const QString& productSn) const
{
    return isStatusMarked(statusRowForProductSn(productSn), STATUS_COL_NG);
}

bool ProcessStationLeftPanel::isProductPaused(const QString& productSn) const
{
    return isStatusMarked(statusRowForProductSn(productSn), STATUS_COL_PAUSE);
}

bool ProcessStationLeftPanel::validateProductNotPassed(
    const QString& productSn,
    const QString& actionName) const
{
    if (!isProductPassed(productSn))
        return true;

    QMessageBox::warning(
        const_cast<ProcessStationLeftPanel*>(this),
        tr("扫码过站"),
        tr("该产品已PASS，不能%1。").arg(actionName));
    qDebug() << __FUNCTION__
             << "product already passed:" << productSn
             << "action:" << actionName;
    return false;
}

bool ProcessStationLeftPanel::validateProductNotNg(
    const QString& productSn,
    const QString& actionName) const
{
    if (!isProductNg(productSn))
        return true;

    QMessageBox::warning(
        const_cast<ProcessStationLeftPanel*>(this),
        tr("扫码过站"),
        tr("该产品已NG并转入维修站，不能%1。").arg(actionName));
    qDebug() << __FUNCTION__
             << "product already NG:" << productSn
             << "action:" << actionName;
    return false;
}

bool ProcessStationLeftPanel::validateProductNotPaused(
    const QString& productSn,
    const QString& actionName) const
{
    if (!isProductPaused(productSn))
        return true;

    QMessageBox::warning(
        const_cast<ProcessStationLeftPanel*>(this),
        tr("扫码过站"),
        tr("请先解除暂停。"));
    qDebug() << __FUNCTION__
             << "product paused:" << productSn
             << "action:" << actionName;
    return false;
}

QString ProcessStationLeftPanel::productSnForPauseAction() const
{
    const QString selectedSn = currentProductSnFromSelection();
    if (!selectedSn.trimmed().isEmpty())
        return selectedSn.trimmed();

    return m_scanEdit ? m_scanEdit->text().trimmed() : QString();
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

bool ProcessStationLeftPanel::isStatusMarked(int row, int column) const
{
    if (!m_statusModel || row < 0 || row >= m_statusModel->rowCount())
        return false;

    return !m_statusModel->index(row, column)
        .data(Qt::DisplayRole)
        .toString()
        .trimmed()
        .isEmpty();
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

    item->setText(checked ? tr("√") : QString());
    item->setTextAlignment(Qt::AlignCenter);
    item->setForeground(checked ? QBrush(color) : QBrush());

    QFont font = item->font();
    font.setBold(checked);
    item->setFont(font);
}

void ProcessStationLeftPanel::startProductTiming(const QString& productSn)
{
    const QString sn = productSn.trimmed();
    if (sn.isEmpty() || m_productStartTimes.contains(sn))
        return;

    m_productStartTimes.insert(sn, QDateTime::currentDateTime());
    m_productPausedSeconds.insert(sn, 0);

    // TODO(backend): 扫入成功后应把开始时间提交给后台，异常退出后由后台恢复当前工序状态。
    qDebug() << __FUNCTION__ << "productSn:" << sn << "start:" << m_productStartTimes.value(sn);
}

void ProcessStationLeftPanel::pauseProductTiming(const QString& productSn)
{
    const QString sn = productSn.trimmed();
    if (sn.isEmpty() || m_productPauseStartTimes.contains(sn))
        return;

    if (!m_productStartTimes.contains(sn))
    {
        // TODO(backend): 重新进入软件后，暂停/恢复应从后台读取历史开始时间；当前本地缺失时只能从当前时刻补起。
        m_productStartTimes.insert(sn, QDateTime::currentDateTime());
        m_productPausedSeconds.insert(sn, 0);
    }

    m_productPauseStartTimes.insert(sn, QDateTime::currentDateTime());
    qDebug() << __FUNCTION__ << "productSn:" << sn << "pauseStart:" << m_productPauseStartTimes.value(sn);
}

void ProcessStationLeftPanel::resumeProductTiming(const QString& productSn)
{
    const QString sn = productSn.trimmed();
    if (sn.isEmpty() || !m_productPauseStartTimes.contains(sn))
        return;

    const qint64 pausedSeconds =
        m_productPauseStartTimes.value(sn).secsTo(QDateTime::currentDateTime());
    m_productPausedSeconds[sn] = m_productPausedSeconds.value(sn, 0) + qMax<qint64>(0, pausedSeconds);
    m_productPauseStartTimes.remove(sn);

    // TODO(backend): 恢复生产时应提交解除暂停原因和暂停时长，后台负责累计工时。
    qDebug() << __FUNCTION__
             << "productSn:" << sn
             << "pausedSeconds:" << pausedSeconds
             << "totalPaused:" << m_productPausedSeconds.value(sn);
}

qint64 ProcessStationLeftPanel::finishProductTiming(const QString& productSn)
{
    const QString sn = productSn.trimmed();
    if (sn.isEmpty())
        return 0;

    if (m_productPauseStartTimes.contains(sn))
        resumeProductTiming(sn);

    const QDateTime startTime = m_productStartTimes.value(sn);
    if (!startTime.isValid())
        return 0;

    const qint64 elapsed =
        qMax<qint64>(0, startTime.secsTo(QDateTime::currentDateTime()) -
                            m_productPausedSeconds.value(sn, 0));
    m_productFinishedSeconds.insert(sn, elapsed);

    // TODO(backend): PASS/NG 完成时应提交结束时间、有效工时和暂停累计时长。
    qDebug() << __FUNCTION__ << "productSn:" << sn << "elapsed:" << formatDuration(elapsed);
    return elapsed;
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
        setTaskData(m_taskStatus, taskStatusFields(), m_taskStatusData);

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
    ProcessStationInfoSection* section,
    const QVector<QPair<QString, QString>>& fields,
    const QVariantMap& rowData,
    int fieldPairsPerRow)
{
    if (!section)
        return;

    section->setFieldsData(fields, rowData, fieldPairsPerRow);
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
        setTaskData(m_taskStatus, taskStatusFields(), m_reworkTaskStatusData);
}

void ProcessStationLeftPanel::setTaskInfoData(const QVariantMap &rowData)
{
    m_taskInfoData = rowData;
    setTaskData(m_taskInfo, taskInfoFields(), rowData);
}

void ProcessStationLeftPanel::setTaskStatusData(const QVariantMap &rowData)
{
    m_taskStatusData = rowData;
    if (m_displayMode == DisplayMode::NormalTask)
        setTaskData(m_taskStatus, taskStatusFields(), rowData);
}

void ProcessStationLeftPanel::setAbnormalInfoData(const QVariantMap &rowData)
{
    m_abnormalInfoData = rowData;
    setTaskData(m_abnormalInfo, abnormalInfoFields(), rowData);
}

void ProcessStationLeftPanel::setReworkTaskStatusData(const QVariantMap &rowData)
{
    m_reworkTaskStatusData = rowData;
    if (m_displayMode == DisplayMode::ReworkTask)
        setTaskData(m_taskStatus, taskStatusFields(), rowData);
}

void ProcessStationLeftPanel::setDisplayMode(DisplayMode mode)
{
    m_displayMode = mode;

    const bool isReworkTask = (mode == DisplayMode::ReworkTask);

    if (m_taskInfo)
        m_taskInfo->setVisible(!isReworkTask);

    if (m_abnormalInfo)
        m_abnormalInfo->setVisible(isReworkTask);

    if (m_taskStatus)
        m_taskStatus->setTitle(isReworkTask ? tr("返工维修任务单状态") : tr("任务单状态"));

    if (isReworkTask)
        setTaskData(m_taskStatus, taskStatusFields(), m_reworkTaskStatusData);
    else
        setTaskData(m_taskStatus, taskStatusFields(), m_taskStatusData);

    qDebug() << __FUNCTION__ << "mode:" << (isReworkTask ? "ReworkTask" : "NormalTask")
             << "taskInfoVisible:" << (m_taskInfo ? m_taskInfo->isVisible() : false)
             << "abnormalInfoVisible:" << (m_abnormalInfo ? m_abnormalInfo->isVisible() : false);
}

void ProcessStationLeftPanel::setPassConditionValidator(PassConditionValidator validator)
{
    m_passConditionValidator = validator;
}

void ProcessStationLeftPanel::clearPassStatus()
{
    // 切换任务或侧边栏直接进入时，清空当前工序的产品实时过站表。
    // 任务单状态中的完成数/NG数也同步归零，避免沿用上一个产品的实时状态。
    m_productStatusRows.clear();
    m_productStartTimes.clear();
    m_productPauseStartTimes.clear();
    m_productPausedSeconds.clear();
    m_productFinishedSeconds.clear();
    if (m_statusModel)
        m_statusModel->removeRows(0, m_statusModel->rowCount());

    m_taskStatusData.insert("finishedCount", 0);
    m_taskStatusData.insert("ngCount", 0);

    if (m_displayMode == DisplayMode::NormalTask)
        setTaskData(m_taskStatus, taskStatusFields(), m_taskStatusData);

    qDebug() << __FUNCTION__ << "pass status cleared";
}
