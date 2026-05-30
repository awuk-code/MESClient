#include "basedialogwidget.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

BaseDialogWidget::BaseDialogWidget(QWidget *parent)
    : QDialog(parent)
{
    setObjectName("BaseDialogWidget");
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setModal(true);
    resize(500, 360);

    initUI();
    initConnections();
}

void BaseDialogWidget::setTitle(const QString &title)
{
    if (m_titleLabel)
        m_titleLabel->setText(title);
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
    return true;
}

void BaseDialogWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isInTitleBar(event->position().toPoint()))
    {
        m_dragging = true;
        m_dragPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
        return;
    }

    QDialog::mousePressEvent(event);
}

void BaseDialogWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton))
    {
        move(event->globalPosition().toPoint() - m_dragPos);
        event->accept();
        return;
    }

    QDialog::mouseMoveEvent(event);
}

void BaseDialogWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragging = false;
    QDialog::mouseReleaseEvent(event);
}

void BaseDialogWidget::onConfirmButtonClicked()
{
    if (onConfirm())
        accept();
}

void BaseDialogWidget::onCancelButtonClicked()
{
    reject();
}

void BaseDialogWidget::onCloseButtonClicked()
{
    reject();
}

void BaseDialogWidget::initUI()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 自定义标题区：后续所有弹窗标题背景、标题字体、关闭按钮都在这里统一控制。
    m_titleBar = new QWidget(this);
    m_titleBar->setObjectName("DialogTitleBar");
    m_titleBar->setFixedHeight(44);

    auto titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(16, 0, 8, 0);
    titleLayout->setSpacing(8);

    m_titleLabel = new QLabel(m_titleBar);
    m_titleLabel->setProperty("labelRole", "dialogTitle");
    m_titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    m_closeButton = new QPushButton(m_titleBar);
    m_closeButton->setObjectName("DialogCloseButton");
    m_closeButton->setIcon(QIcon(":/res/common/close.svg"));
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setFlat(true);

    titleLayout->addWidget(m_titleLabel, 1);
    titleLayout->addWidget(m_closeButton);
    mainLayout->addWidget(m_titleBar);

    // 自定义内容区：子类通过 contentLayout() 添加具体内容。
    auto contentWidget = new QWidget(this);
    contentWidget->setObjectName("DialogContent");
    m_contentLayout = new QVBoxLayout(contentWidget);
    m_contentLayout->setContentsMargins(20, 18, 20, 18);
    m_contentLayout->setSpacing(10);
    mainLayout->addWidget(contentWidget, 1);

    // 自定义按钮区：默认提供取消/确认，单按钮弹窗可以隐藏 cancelButton()。
    auto buttonWidget = new QWidget(this);
    buttonWidget->setObjectName("DialogButtonBar");
    auto buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(20, 12, 20, 16);
    buttonLayout->setSpacing(12);
    buttonLayout->addStretch();

    m_cancelButton = new QPushButton(tr("取消"), buttonWidget);
    m_cancelButton->setFixedSize(90, 32);
    m_cancelButton->setProperty("buttonRole", "secondary");

    m_confirmButton = new QPushButton(tr("确定"), buttonWidget);
    m_confirmButton->setFixedSize(90, 32);
    m_confirmButton->setProperty("buttonRole", "primary");
    m_confirmButton->setDefault(true);
    m_confirmButton->setAutoDefault(true);

    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_confirmButton);
    mainLayout->addWidget(buttonWidget);
}

void BaseDialogWidget::initConnections()
{
    connect(m_confirmButton, &QPushButton::clicked,
            this, &BaseDialogWidget::onConfirmButtonClicked);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &BaseDialogWidget::onCancelButtonClicked);
    connect(m_closeButton, &QPushButton::clicked,
            this, &BaseDialogWidget::onCloseButtonClicked);
}

bool BaseDialogWidget::isInTitleBar(const QPoint& pos) const
{
    if (!m_titleBar || !m_titleBar->geometry().contains(pos))
        return false;

    if (m_closeButton && childAt(pos) == m_closeButton)
        return false;

    return true;
}
