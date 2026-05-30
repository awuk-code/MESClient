#include "apidefinition.h"

#include "configmanager.h"

QString ApiDefinition::baseUrl()
{
    const QString host = ConfigManager::instance().serverIP();
    const int port = ConfigManager::instance().serverPort();
    return QString("http://%1:%2").arg(host).arg(port);
}

QUrl ApiDefinition::loginUrl()
{
    // 后台登录接口地址集中写在这里；接口路径确定后只改这一处。
    return buildUrl("/api/login");
}

QUrl ApiDefinition::sessionCheckUrl()
{
    // 会话检查接口用于旧电脑定时确认当前 token 是否仍然有效。
    return buildUrl("/api/session/check");
}

QUrl ApiDefinition::buildUrl(const QString& path)
{
    QString normalizedPath = path;
    if (!normalizedPath.startsWith('/'))
        normalizedPath.prepend('/');

    return QUrl(baseUrl() + normalizedPath);
}
