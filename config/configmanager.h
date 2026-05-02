#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QJsonObject>

class ConfigManager
{
public:
    static ConfigManager& instance();
    void loadConfig(const QString &filePath);

    QString softwareVersion()const ;
    QString protocolVersion() const;
    QString buildTime() const;

    QString serverIP() const;
    int serverPort() const;

private:
    ConfigManager();
    QJsonObject m_config;
};

#endif // CONFIGMANAGER_H
