#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QJsonObject>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QTimer>

class ConfigManager
{
public:
    static ConfigManager& instance();
    void loadConfig(const QString &filePath);
    void saveConfig();

    QString softwareVersion()const ;
    QString protocolVersion() const;
    QString buildTime() const;

    QString serverIP() const;
    int serverPort() const;

    qint64 getSessionSeconds() const;
    qint64 getTotalSeconds() const;
    QString formatTime(qint64 sec) const;

private:
    ConfigManager();
    QJsonObject m_config;
    QString m_configPath;
    QDateTime m_sessionStartTime;
    qint64 m_historySeconds = 0;
};

#endif // CONFIGMANAGER_H
