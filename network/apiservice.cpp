#include "apiservice.h"

#include "apidefinition.h"
#include "configmanager.h"
#include "usersession.h"

#include <QJsonObject>
#include <QDebug>

ApiService* ApiService::instance()
{
    // 懒汉式单例：业务接口统一从这里调用，避免每个页面各自 new 网络对象。
    static ApiService service;
    return &service;
}

ApiService::ApiService(QObject* parent)
    : QObject(parent),
      m_httpClient(this)
{
    connect(&m_httpClient, &HttpClient::requestFinished,
            this, &ApiService::onHttpRequestFinished);
}

void ApiService::login(const QString& userName, const QString& password)
{
    QJsonObject body;
    // 登录入参集中写在这里；后台字段名确定后只改这些 key。
    // deviceId 用于后台判断“同一账号不能在多台电脑同时登录”；同一台电脑允许不同账号登录。
    body.insert("userName", userName);
    body.insert("password", password);
    body.insert("deviceId", ConfigManager::instance().deviceId());
    body.insert("clientVersion", ConfigManager::instance().softwareVersion());
    body.insert("buildDate", ConfigManager::instance().buildTime());
    body.insert("protocolVersion", ConfigManager::instance().protocolVersion());

    const QString requestId = m_httpClient.postJson(ApiDefinition::loginUrl(), body);
    m_requestTypes.insert(requestId, RequestType::Login);
    qDebug() << __FUNCTION__ << "login request sent:" << requestId << userName;
}

void ApiService::onHttpRequestFinished(const QString& requestId, const ApiResponse& response)
{
    const RequestType type = m_requestTypes.take(requestId);

    switch (type)
    {
    case RequestType::Login:
        handleLoginResponse(response);
        break;
    default:
        qDebug() << __FUNCTION__ << "unknown request:" << requestId;
        break;
    }
}

void ApiService::handleLoginResponse(const ApiResponse& response)
{
    if (!response.success)
    {
        UserSession::instance()->clear();
        emit loginFinished(false, response.message, response.networkError);
        return;
    }

    const QJsonObject data = response.data.toObject();
    // 登录返回字段集中写在这里；后台字段名确定后只改 token/userName/role 的读取方式。
    UserSession::instance()->setLoginInfo(
        data.value("token").toString(),
        data.value("userName").toString(),
        data.value("role").toString(),
        data);

    emit loginFinished(true, tr("登录成功。"), false);
}
