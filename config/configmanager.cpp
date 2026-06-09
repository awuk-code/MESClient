#include "configmanager.h"

#include "clientinfo.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QSysInfo>

ConfigManager::ConfigManager()
{
    m_sessionStartTime = QDateTime::currentDateTime();
}

ConfigManager &ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

void ConfigManager::loadConfig(const QString &filePath)
{
    // 不传路径时使用 exe 同目录 config/config.ini，避免 qrc 资源文件只读导致无法配置和保存。
    m_configPath = filePath.trimmed().isEmpty() ? defaultConfigPath() : filePath;

    ensureConfigFile();
    loadFromIni();
    refreshClientInfo();

    qDebug() << __FUNCTION__
             << "configPath:" << m_configPath
             << "softwareVersion:" << softwareVersion()
             << "buildTime:" << buildTime()
             << "protocolVersion:" << protocolVersion()
             << "server:" << serverIP() << serverPort()
             << "clientIP:" << clientIP()
             << "clientMAC:" << clientMAC()
             << "deviceId:" << deviceId();
}

void ConfigManager::saveConfig()
{
    if (m_configPath.isEmpty())
        return;

    if (m_configPath.startsWith(":/"))
    {
        qDebug() << __FUNCTION__ << "resource config is read only:" << m_configPath;
        return;
    }

    QSettings settings(m_configPath, QSettings::IniFormat);

    // 运行配置统一写入 ini；内网部署时可直接修改这个文件，不需要重新编译。
    settings.setValue("App/software_version", m_softwareVersion);
    settings.setValue("App/build_date", m_buildDate);
    settings.setValue("App/protocol_version", m_protocolVersion);
    settings.setValue("Server/ip", m_serverIP);
    settings.setValue("Server/port", m_serverPort);
    settings.setValue("Device/device_id", m_deviceId);
    settings.sync();

    qDebug() << __FUNCTION__ << "Config saved:" << m_configPath;
}

QString ConfigManager::softwareVersion() const
{
    return m_softwareVersion;
}

QString ConfigManager::protocolVersion() const
{
    return m_protocolVersion;
}

QString ConfigManager::buildTime() const
{
    return m_buildDate;
}

QString ConfigManager::deviceId() const
{
    // 单账号禁止多台电脑登录需要后台根据 deviceId 判断；客户端只负责提供稳定设备标识。
    return m_deviceId.isEmpty() ? localMachineId() : m_deviceId;
}

QString ConfigManager::clientIP() const
{
    if (m_clientIP.isEmpty())
        const_cast<ConfigManager*>(this)->refreshClientInfo();

    return m_clientIP;
}

QString ConfigManager::clientMAC() const
{
    if (m_clientMAC.isEmpty())
        const_cast<ConfigManager*>(this)->refreshClientInfo();

    return m_clientMAC;
}

QString ConfigManager::serverIP() const
{
    return m_serverIP;
}

int ConfigManager::serverPort() const
{
    return m_serverPort;
}

qint64 ConfigManager::getSessionSeconds() const
{
    return m_sessionStartTime.secsTo(QDateTime::currentDateTime());
}

qint64 ConfigManager::getTotalSeconds() const
{
    // 登录累计时长后续由服务器返回；当前客户端只维护本次登录时长。
    return getSessionSeconds();
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

QString ConfigManager::defaultConfigPath() const
{
    QDir appDir(QCoreApplication::applicationDirPath());
    return appDir.filePath("config/config.ini");
}

void ConfigManager::ensureConfigFile()
{
    if (m_configPath.startsWith(":/"))
        return;

    QFileInfo fileInfo(m_configPath);
    QDir dir = fileInfo.dir();
    if (!dir.exists() && !dir.mkpath("."))
        qDebug() << __FUNCTION__ << "create config dir failed:" << dir.absolutePath();

    if (!fileInfo.exists())
    {
        // 第一次运行自动生成默认 ini，方便部署后直接修改服务器地址和协议版本。
        writeDefaultConfig(m_configPath);
        qDebug() << __FUNCTION__ << "default config created:" << m_configPath;
    }
}

void ConfigManager::loadFromIni()
{
    QSettings settings(m_configPath, QSettings::IniFormat);

    m_softwareVersion =
        settings.value("App/software_version", m_softwareVersion).toString();
    m_buildDate =
        settings.value("App/build_date", m_buildDate).toString();
    m_protocolVersion =
        settings.value("App/protocol_version", m_protocolVersion).toString();
    m_serverIP =
        settings.value("Server/ip", m_serverIP).toString();
    m_serverPort =
        settings.value("Server/port", m_serverPort).toInt();
    m_deviceId =
        settings.value("Device/device_id", QString()).toString().trimmed();

    if (m_deviceId.isEmpty())
    {
        m_deviceId = localMachineId();
        saveConfig();
    }
}

void ConfigManager::writeDefaultConfig(const QString& filePath) const
{
    QSettings settings(filePath, QSettings::IniFormat);

    // 默认配置模板写在这里；后续需要新增配置项时优先在这里补默认值。
    settings.setValue("App/software_version", m_softwareVersion);
    settings.setValue("App/build_date", m_buildDate);
    settings.setValue("App/protocol_version", m_protocolVersion);
    settings.setValue("Server/ip", m_serverIP);
    settings.setValue("Server/port", m_serverPort);
    settings.setValue("Device/device_id", QString());
    settings.sync();
}

QString ConfigManager::localMachineId() const
{
    const QByteArray machineId = QSysInfo::machineUniqueId();
    if (!machineId.isEmpty())
        return QString::fromLatin1(machineId.toHex());

    const QByteArray hostName = QSysInfo::machineHostName().toUtf8();
    return QString::fromLatin1(
        QCryptographicHash::hash(hostName, QCryptographicHash::Sha256).toHex());
}

void ConfigManager::refreshClientInfo()
{
    m_clientIP = ClientInfo::ip();
    m_clientMAC = ClientInfo::mac();
}
