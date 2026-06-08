#include "processstationpassdialogs.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

NgAbnormalDialog::NgAbnormalDialog(const QString& productSn, QWidget* parent)
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

QString NgAbnormalDialog::abnormalType() const
{
    return m_typeCombo ? m_typeCombo->currentText() : QString();
}

QString NgAbnormalDialog::phenomenon() const
{
    return m_phenomenonEdit ? m_phenomenonEdit->toPlainText().trimmed() : QString();
}

QString NgAbnormalDialog::imagePath() const
{
    return m_imageEdit ? m_imageEdit->text().trimmed() : QString();
}

QString NgAbnormalDialog::nextProcess() const
{
    return m_nextProcessLabel ? m_nextProcessLabel->text() : QString();
}

QString NgAbnormalDialog::summary() const
{
    return tr("异常类型：%1；异常现象：%2；异常图片：%3；下一工序：%4")
        .arg(abnormalType(), phenomenon(), imagePath(), nextProcess());
}

bool NgAbnormalDialog::onConfirm()
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

PauseReasonDialog::PauseReasonDialog(const QString& productSn, QWidget* parent)
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

QString PauseReasonDialog::pauseType() const
{
    return selectedTypes().join(tr("、"));
}

QString PauseReasonDialog::relatedCode() const
{
    return m_codeEdit ? m_codeEdit->text().trimmed() : QString();
}

QString PauseReasonDialog::description() const
{
    return m_descriptionEdit ? m_descriptionEdit->toPlainText().trimmed() : QString();
}

bool PauseReasonDialog::onConfirm()
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

QStringList PauseReasonDialog::selectedTypes() const
{
    QStringList types;

    if (m_typeGroup && m_typeGroup->checkedButton())
        types.append(m_typeGroup->checkedButton()->text());

    return types;
}

void PauseReasonDialog::updateCodePrompt()
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

ResumeReasonDialog::ResumeReasonDialog(const QString& productSn, QWidget* parent)
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

QString ResumeReasonDialog::reason() const
{
    return m_reasonEdit ? m_reasonEdit->toPlainText().trimmed() : QString();
}

bool ResumeReasonDialog::onConfirm()
{
    if (reason().isEmpty())
    {
        QMessageBox::warning(this, tr("解除暂停原因登记"), tr("请填写解除暂停理由。"));
        return false;
    }

    return true;
}
