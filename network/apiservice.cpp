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

void ApiService::checkSession()
{
    if (!UserSession::instance()->isLoggedIn())
        return;

    const QString requestId = m_httpClient.get(ApiDefinition::sessionCheckUrl());
    m_requestTypes.insert(requestId, RequestType::CheckSession);
    qDebug() << __FUNCTION__ << "session check request sent:" << requestId;
}

void ApiService::onHttpRequestFinished(const QString& requestId, const ApiResponse& response)
{
    const RequestType type = m_requestTypes.take(requestId);

    // 新电脑登录后，后台应让旧电脑的 token/session 失效。
    // 旧电脑下一次请求收到这些状态码/业务码时，统一从这里退出登录。
    if (type != RequestType::Login && isForcedLogoutResponse(response))
    {
        UserSession::instance()->clear();
        emit forcedLoggedOut(response.message.isEmpty()
                                 ? tr("账号已在其他电脑登录，本机已退出登录。")
                                 : response.message);
        return;
    }

    switch (type)
    {
    case RequestType::Login:
        handleLoginResponse(response);
        break;
    case RequestType::CheckSession:
        qDebug() << __FUNCTION__ << "session check success:" << response.success;
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


bool ApiService::isForcedLogoutResponse(const ApiResponse& response) const
{
    if (response.networkError)
        return false;

    // HTTP 401/403 通常表示当前 token/session 已失效，旧电脑需要退出登录。
    if (response.httpStatus == 401 || response.httpStatus == 403)
        return true;

    // 下面这些业务码先作为客户端约定，后台最终确定后只需要改这里。
    if (response.businessCode == 40101 ||
        response.businessCode == 40901)
    {
        return true;
    }

    const QString code = response.businessCodeText.trimmed().toUpper();
    return code == "ACCOUNT_LOGIN_ELSEWHERE" ||
           code == "LOGIN_ELSEWHERE" ||
           code == "SESSION_REPLACED" ||
           code == "TOKEN_INVALID";
}
