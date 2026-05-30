#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QDateTime>

class ConfigManager
{
public:
    static ConfigManager& instance();
    void loadConfig(const QString &filePath = QString());
    void saveConfig();

    QString softwareVersion()const ;
    QString protocolVersion() const;
    QString buildTime() const;
    QString deviceId() const;

    QString serverIP() const;
    int serverPort() const;

    qint64 getSessionSeconds() const;
    qint64 getTotalSeconds() const;
    QString formatTime(qint64 sec) const;
private:
    ConfigManager();
    QString defaultConfigPath() const;
    void ensureConfigFile();
    void loadFromIni();
    void writeDefaultConfig(const QString& filePath) const;
    QString localMachineId() const;

    QString m_configPath;
    QDateTime m_sessionStartTime;
    QString m_softwareVersion = "1.0.0";
    QString m_protocolVersion = "1.0";
    QString m_buildDate = QString(__DATE__ " " __TIME__);
    QString m_deviceId;
    QString m_serverIP = "127.0.0.1";
    int m_serverPort = 8080;
};

#endif // CONFIGMANAGER_H
