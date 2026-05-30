#include "usersession.h"

#include <QDebug>

UserSession* UserSession::instance()
{
    // 懒汉式单例：第一次使用时才创建，后续所有页面共用同一份登录态。
    static UserSession session;
    return &session;
}

UserSession::UserSession(QObject* parent)
    : QObject(parent)
{
}

void UserSession::setLoginInfo(const QString& token,
                               const QString& userName,
                               const QString& role,
                               const QJsonObject& userInfo)
{
    m_token = token;
    m_userName = userName;
    m_role = role;
    m_userInfo = userInfo;

    qDebug() << __FUNCTION__
             << "userName:" << m_userName
             << "role:" << m_role
             << "hasToken:" << !m_token.isEmpty();
}

void UserSession::clear()
{
    m_token.clear();
    m_userName.clear();
    m_role.clear();
    m_userInfo = QJsonObject();
    qDebug() << __FUNCTION__ << "session cleared";
}

QString UserSession::token() const
{
    return m_token;
}

QString UserSession::userName() const
{
    return m_userName;
}

QString UserSession::role() const
{
    return m_role;
}

bool UserSession::isLoggedIn() const
{
    return !m_token.isEmpty() || !m_userName.isEmpty();
}

QJsonObject UserSession::userInfo() const
{
    return m_userInfo;
}
