#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "networktypes.h"

#include <QHash>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QUrl>

class QNetworkReply;

class HttpClient : public QObject
{
    Q_OBJECT
public:
    explicit HttpClient(QObject* parent = nullptr);

    QString get(const QUrl& url);
    QString postJson(const QUrl& url, const QJsonObject& body);

signals:
    void requestFinished(const QString& requestId, const ApiResponse& response);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    QNetworkRequest createRequest(const QUrl& url) const;
    QString createRequestId() const;
    ApiResponse parseReply(QNetworkReply* reply, const QByteArray& rawData) const;

    QNetworkAccessManager m_manager;
    QHash<QNetworkReply*, QString> m_requestIds;
};

#endif // HTTPCLIENT_H
