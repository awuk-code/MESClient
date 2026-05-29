#include "subheaderwidget.h"
#include "MDebug.h"
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
    m_loginTime->addItem(tr("本次在线：00:00:00"));
    m_loginTime->addItem(tr("周：00:00:00"));
    m_loginTime->addItem(tr("月：00:00:00"));
    m_loginTime->addItem(tr("季：00:00:00"));
    m_loginTime->addItem(tr("半年：00:00:00"));
    m_loginTime->addItem(tr("年：00:00:00"));

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
    funcDebug() << tr("更新页面导航:") << m_pagePath.join(" -> ");
}

void SubHeaderWidget::setProcessNavigationVisible(bool visible)
{
    m_showProcessNavigation = visible;
    updateNavigationText();
    funcDebug() << tr("工序导航显示:") << visible;
}

void SubHeaderWidget::setCurrentProcessName(const QString &processName)
{
    m_currentProcessName = processName;
    updateNavigationText();
    funcDebug() << tr("当前工序:") << processName;
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
    QString totalTime = cfg.formatTime(cfg.getTotalSeconds());
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy/MM/dd dddd HH:mm:ss");

    m_loginTime->setItemText(0, tr("本次在线：%1").arg(sessionTime));
    m_loginTime->setItemText(1, tr("周：%1").arg(totalTime));
    m_loginTime->setItemText(2, tr("月：%1").arg(totalTime));
    m_loginTime->setItemText(3, tr("季：%1").arg(totalTime));
    m_loginTime->setItemText(4, tr("半年：%1").arg(totalTime));
    m_loginTime->setItemText(5, tr("年：%1").arg(totalTime));
    m_currentTime->setText(currentDateTime);

    static int saveCounter =0;
    saveCounter++;
    if(saveCounter >= 300){
        ConfigManager::instance().saveConfig();
        saveCounter = 0;
    }
}

void SubHeaderWidget::initConnect()
{
    connect(timer, &QTimer::timeout, this, &SubHeaderWidget::updateTime);
}
