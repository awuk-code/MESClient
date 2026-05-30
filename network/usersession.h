#ifndef USERSESSION_H
#define USERSESSION_H

#include <QJsonObject>
#include <QObject>
#include <QString>

class UserSession : public QObject
{
    Q_OBJECT
public:
    static UserSession* instance();

    void setLoginInfo(const QString& token,
                      const QString& userName,
                      const QString& role,
                      const QJsonObject& userInfo);
    void clear();

    QString token() const;
    QString userName() const;
    QString role() const;
    bool isLoggedIn() const;
    QJsonObject userInfo() const;

private:
    explicit UserSession(QObject* parent = nullptr);

    QString m_token;
    QString m_userName;
    QString m_role;
    QJsonObject m_userInfo;
};

#endif // USERSESSION_H
