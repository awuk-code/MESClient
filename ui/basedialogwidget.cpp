// basedialogwidget.cpp

#include "basedialogwidget.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

BaseDialogWidget::BaseDialogWidget(QWidget *parent)
    : QDialog(parent)
{
    // 去掉帮助按钮
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    // 模态对话框（默认行为，可按需修改）
    setModal(true);

    // 默认大小
    resize(500, 400);

    initUI();
    initConnections();
}

void BaseDialogWidget::setTitle(const QString &title)
{
    if (m_titleLabel)
    {
        m_titleLabel->setText(title);
    }
}

QVBoxLayout *BaseDialogWidget::contentLayout() const
{
    return m_contentLayout;
}

QPushButton *BaseDialogWidget::confirmButton() const
{
    return m_confirmButton;
}

QPushButton *BaseDialogWidget::cancelButton() const
{
    return m_cancelButton;
}

bool BaseDialogWidget::onConfirm()
{
    // 基类默认允许关闭
    return true;
}

void BaseDialogWidget::onConfirmBtnClicked()
{
    // 先执行子类逻辑
    if (onConfirm())
    {
        accept();
    }
}

void BaseDialogWidget::onCancelBtnClicked()
{
    reject();
}

void BaseDialogWidget::initUI()
{
    // =========================
    // 主布局
    // =========================
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // =========================
    // 标题栏
    // =========================
    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setMinimumHeight(30);

    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);

    mainLayout->addWidget(m_titleLabel);

    // =========================
    // 中间内容区
    // =========================
    auto contentWidget = new QWidget(this);
    m_contentLayout = new QVBoxLayout(contentWidget);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(10);

    // 让中间区域自动拉伸，占据主要空间
    mainLayout->addWidget(contentWidget, 1);

    // =========================
    // 底部按钮区
    // =========================
    auto buttonWidget = new QWidget(this);
    auto buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(12);

    // 左侧弹簧
    buttonLayout->addStretch();

    // 取消按钮
    m_cancelButton = new QPushButton(tr("取消"), this);
    m_cancelButton->setFixedSize(90, 32);

    // 确认按钮
    m_confirmButton = new QPushButton(tr("确认"), this);
    m_confirmButton->setFixedSize(90, 32);
    m_confirmButton->setDefault(true);   // 回车触发
    m_confirmButton->setAutoDefault(true);

    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_confirmButton);

    // 右侧弹簧
    buttonLayout->addStretch();

    mainLayout->addWidget(buttonWidget);
}

void BaseDialogWidget::initConnections()
{
    connect(m_confirmButton, &QPushButton::clicked,
            this, &BaseDialogWidget::onConfirmBtnClicked);

    connect(m_cancelButton, &QPushButton::clicked,
            this, &BaseDialogWidget::onCancelBtnClicked);
}
