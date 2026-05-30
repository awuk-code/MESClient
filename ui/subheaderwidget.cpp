#include "subheaderwidget.h"
#include <QDebug>
#include "configmanager.h"

#include <QHBoxLayout>

SubHeaderWidget::SubHeaderWidget(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("SubHeaderWidget");
    timer = new QTimer(this);

    initUI();
    initConnect();
    timer->start(1000);
    updateTime();
}

void SubHeaderWidget::initUI()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(15,0,15,0);

    m_pagePath = {tr("生产任务"), tr("生产任务列表")};
    m_path = new QLabel(this);
    layout->addWidget(m_path);
    layout->addStretch();

    m_loginTime = new QComboBox(this);
    m_loginTime->setMinimumWidth(170);
    // 登录时间后续由服务器返回；当前只显示本次登录时长。
    m_loginTime->addItem(tr("本次登录：00:00:00"));

    m_currentTime = new QLabel(this);
    layout->addStretch();
    layout->addWidget(m_loginTime);
    layout->addWidget(m_currentTime);

    updateNavigationText();
}

void SubHeaderWidget::setPageNavigation(const QStringList &pagePath)
{
    m_pagePath = pagePath;
    updateNavigationText();
    qDebug() << __FUNCTION__ << tr("更新页面导航:") << m_pagePath.join(" -> ");
}

void SubHeaderWidget::setProcessNavigationVisible(bool visible)
{
    m_showProcessNavigation = visible;
    updateNavigationText();
    qDebug() << __FUNCTION__<< tr("工序导航显示:") << visible;
}

void SubHeaderWidget::setCurrentProcessName(const QString &processName)
{
    m_currentProcessName = processName;
    updateNavigationText();
    qDebug() << __FUNCTION__ <<tr("当前工序:") << processName;
}

void SubHeaderWidget::updateNavigationText()
{
    QString text = tr("索引 (%1)").arg(m_pagePath.join(QStringLiteral(" -> ")));

    if (m_showProcessNavigation)
    {
        // TODO: 当前工序名称后续由接口返回后调用 setCurrentProcessName() 更新。
        // 这里先保留工序导航占位，避免后面接接口时再调整页面结构。
        const QString processName =
            m_currentProcessName.isEmpty() ? QStringLiteral("--") : m_currentProcessName;
        text += tr("    当前工序：%1").arg(processName);
    }

    if (m_path)
        m_path->setText(text);
}

void SubHeaderWidget::updateTime()
{
    ConfigManager& cfg = ConfigManager::instance();
    QString sessionTime = cfg.formatTime(cfg.getSessionSeconds());
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy/MM/dd dddd HH:mm:ss");

    m_loginTime->setItemText(0, tr("本次登录：%1").arg(sessionTime));
    m_currentTime->setText(currentDateTime);
}

void SubHeaderWidget::initConnect()
{
    connect(timer, &QTimer::timeout, this, &SubHeaderWidget::updateTime);
}
