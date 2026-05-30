#ifndef APISERVICE_H
#define APISERVICE_H

#include "httpclient.h"

#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QString>

class ApiService : public QObject
{
    Q_OBJECT
public:
    static ApiService* instance();

    void login(const QString& userName, const QString& password);
    void checkSession();

signals:
    void loginFinished(bool success, const QString& message, bool networkError);
    void forcedLoggedOut(const QString& message);

private slots:
    void onHttpRequestFinished(const QString& requestId, const ApiResponse& response);

private:
    enum class RequestType
    {
        Unknown,
        Login,
        CheckSession
    };

    explicit ApiService(QObject* parent = nullptr);
    void handleLoginResponse(const ApiResponse& response);
    bool isForcedLogoutResponse(const ApiResponse& response) const;

    HttpClient m_httpClient;
    QHash<QString, RequestType> m_requestTypes;
};

#endif // APISERVICE_H
