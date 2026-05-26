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

    m_path = new QLabel(QStringLiteral("索引 (生产任务 -> 生产任务列表)"), this);
    layout->addWidget(m_path);
    layout->addStretch();

    m_loginTime = new QComboBox(this);
    m_loginTime->setMinimumWidth(170);
    m_loginTime->addItem(QStringLiteral("本次在线：00:00:00"));
    m_loginTime->addItem(QStringLiteral("周：00:00:00"));
    m_loginTime->addItem(QStringLiteral("月：00:00:00"));
    m_loginTime->addItem(QStringLiteral("季：00:00:00"));
    m_loginTime->addItem(QStringLiteral("半年：00:00:00"));
    m_loginTime->addItem(QStringLiteral("年：00:00:00"));

    m_currentTime = new QLabel(this);
    layout->addStretch();
    layout->addWidget(m_loginTime);
    layout->addWidget(m_currentTime);
}

void SubHeaderWidget::updateTime()
{
    ConfigManager& cfg = ConfigManager::instance();
    QString sessionTime = cfg.formatTime(cfg.getSessionSeconds());
    QString totalTime = cfg.formatTime(cfg.getTotalSeconds());
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy/MM/dd dddd HH:mm:ss");

    m_loginTime->setItemText(0, QStringLiteral("本次在线：%1").arg(sessionTime));
    m_loginTime->setItemText(1, QStringLiteral("周：%1").arg(totalTime));
    m_loginTime->setItemText(2, QStringLiteral("月：%1").arg(totalTime));
    m_loginTime->setItemText(3, QStringLiteral("季：%1").arg(totalTime));
    m_loginTime->setItemText(4, QStringLiteral("半年：%1").arg(totalTime));
    m_loginTime->setItemText(5, QStringLiteral("年：%1").arg(totalTime));
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
