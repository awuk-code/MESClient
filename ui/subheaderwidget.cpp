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

    //m_onlineTime = new QLabel(QStringLiteral("在线时长：35min"), this);
    // m_onlineTimeBtn = new QPushButton(this);
    // m_onlineTimeBtn->setIcon(QIcon(":/res/common/time.svg"));
    // m_onlineTimeBtn->setStyleSheet("border: none; color: transparent");

    m_loginTime = new QComboBox(this);
    m_loginTime->addItem(QStringLiteral("在线时长：35min"));
    QString time = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm");
    m_currentTime = new QLabel(this);
    layout->addStretch();
    layout->addWidget(m_loginTime);
    layout->addWidget(m_onlineTime);
    layout->addWidget(m_onlineTimeBtn);
    layout->addWidget(m_currentTime);
}

void SubHeaderWidget::updateTime()
{
    ConfigManager& cfg = ConfigManager::instance();
    QString sessionTime = cfg.formatTime(cfg.getSessionSeconds());
    QString totalTime = cfg.formatTime(cfg.getTotalSeconds());
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy/MM/dd dddd HH:mm:ss");

    m_currentTime->setText(QStringLiteral("本次在线: %1 | 累计时长: %2 | %3")
                                 .arg(sessionTime)
                                 .arg(totalTime)
                                 .arg(currentDateTime));

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
