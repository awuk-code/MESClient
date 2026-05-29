#include "configmanager.h"
#include <QDebug>

ConfigManager::ConfigManager(){
    m_sessionStartTime = QDateTime::currentDateTime();
}

ConfigManager &ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

void ConfigManager::loadConfig(const QString &filePath)
{
    m_configPath = filePath;
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << __FUNCTION__"open Unsucess"<<file.errorString();
    return;
    }
    qDebug() << __FUNCTION__"config load success";
    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    m_config = doc.object();
    m_historySeconds = m_config.value("total_online_seconds").toVariant().toLongLong();
    qDebug() << __FUNCTION__"history time = "<<m_historySeconds;
}

void ConfigManager::saveConfig()
{
    if(m_configPath.isEmpty())  return;

    m_config["total_online_seconds"] = getTotalSeconds();
    QFile file(m_configPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonDocument doc(m_config);
     file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << __FUNCTION__ "Config saved. Total seconds:" << m_config["total_online_seconds"].toVariant().toLongLong();
    } else {
        qDebug() << __FUNCTION__ "Save Unsuccess:" << file.errorString();
        qDebug() << __FUNCTION__ "Save Failed Path:" << file.fileName();
    }
}

QString ConfigManager::softwareVersion() const
{
    return m_config.value("software_version").toString("1.0.0");
}

QString ConfigManager::protocolVersion() const
{
    return m_config.value("protocol_version").toString("1.0");
}

QString ConfigManager::buildTime() const
{
    return QString(__DATE__ " " __TIME__);
}

QString ConfigManager::serverIP() const
{
    return m_config.value("server_ip").toString("127.0.0.1");
}

int ConfigManager::serverPort() const
{
    return m_config.value("server_port").toInt(8080);
}

qint64 ConfigManager::getSessionSeconds() const
{
    return m_sessionStartTime.secsTo(QDateTime::currentDateTime());
}

qint64 ConfigManager::getTotalSeconds() const
{
    return m_historySeconds + getSessionSeconds();
}

QString ConfigManager::formatTime(qint64 sec) const
{
    int h = sec / 3600;
    int m = (sec % 3600) / 60;
    int s = sec % 60;
    return QString("%1:%2:%3")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'));
}


