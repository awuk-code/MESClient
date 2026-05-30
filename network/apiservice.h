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

signals:
    void loginFinished(bool success, const QString& message, bool networkError);

private slots:
    void onHttpRequestFinished(const QString& requestId, const ApiResponse& response);

private:
    enum class RequestType
    {
        Unknown,
        Login
    };

    explicit ApiService(QObject* parent = nullptr);
    void handleLoginResponse(const ApiResponse& response);

    HttpClient m_httpClient;
    QHash<QString, RequestType> m_requestTypes;
};

#endif // APISERVICE_H
