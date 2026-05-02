#include "configmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>
ConfigManager::ConfigManager(){}

ConfigManager &ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

void ConfigManager::loadConfig(const QString &filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() <<"open Unsucess"<<file.errorString();
    return;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    m_config = doc.object();
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


