#include "httpclient.h"

#include "usersession.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUuid>
#include <QDebug>

HttpClient::HttpClient(QObject* parent)
    : QObject(parent)
{
    connect(&m_manager, &QNetworkAccessManager::finished,
            this, &HttpClient::onReplyFinished);
}

QString HttpClient::get(const QUrl& url)
{
    const QString requestId = createRequestId();
    QNetworkReply* reply = m_manager.get(createRequest(url));
    m_requestIds.insert(reply, requestId);

    qDebug() << __FUNCTION__ << "GET" << requestId << url;
    return requestId;
}

QString HttpClient::postJson(const QUrl& url, const QJsonObject& body)
{
    const QString requestId = createRequestId();
    const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
    QNetworkReply* reply = m_manager.post(createRequest(url), payload);
    m_requestIds.insert(reply, requestId);

    qDebug() << __FUNCTION__ << "POST" << requestId << url << payload;
    return requestId;
}

void HttpClient::onReplyFinished(QNetworkReply* reply)
{
    const QString requestId = m_requestIds.take(reply);
    const QByteArray rawData = reply->readAll();
    const ApiResponse response = parseReply(reply, rawData);

    qDebug() << __FUNCTION__
             << "requestId:" << requestId
             << "success:" << response.success
             << "networkError:" << response.networkError
             << "httpStatus:" << response.httpStatus
             << "message:" << response.message;

    emit requestFinished(requestId, response);
    reply->deleteLater();
}

QNetworkRequest HttpClient::createRequest(const QUrl& url) const
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    // 后台未响应时最多等待 5 秒，避免登录或按钮请求长时间卡在等待状态。
    request.setTransferTimeout(5000);

    // 后续所有需要登录态的接口都会从 UserSession 自动带 token，不需要页面自己处理请求头。
    const QString token = UserSession::instance()->token();
    if (!token.isEmpty())
        request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());

    return request;
}

QString HttpClient::createRequestId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

ApiResponse HttpClient::parseReply(QNetworkReply* reply, const QByteArray& rawData) const
{
    ApiResponse response;
    response.rawData = rawData;
    response.httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() != QNetworkReply::NoError)
    {
        response.networkError = true;
        response.message = reply->errorString();
        return response;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(rawData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject())
    {
        response.message = tr("后台返回数据不是有效 JSON。");
        qDebug() << __FUNCTION__ << "json parse error:" << parseError.errorString() << rawData;
        return response;
    }

    const QJsonObject obj = doc.object();
    // 这里兼容常见返回格式：success/code/message/data。后台格式确定后只需要集中调整解析规则。
    response.success = obj.value("success").toBool(obj.value("code").toInt(-1) == 0);
    response.message = obj.value("message").toString(obj.value("msg").toString());
    response.data = obj.value("data");

    if (response.message.isEmpty() && !response.success)
        response.message = tr("后台返回失败。");

    return response;
}
