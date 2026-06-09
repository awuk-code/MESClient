#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <QString>
#include <QVariantMap>

class ClientInfo
{
public:
    // 获取当前客户端真实 IPv4 地址，过滤回环、虚拟网卡、隧道网卡和链路本地地址。
    static QString ip();

    // 获取当前客户端真实 MAC 地址，优先返回和真实 IPv4 地址同一块网卡的 MAC。
    static QString mac();

    // 获取上传或登录时常用的客户端信息。
    // 返回字段：clientIP、clientMAC、hostName。
    static QVariantMap info();

private:
    struct NetCard;

    static NetCard primaryNetCard();
    static bool isRealNetCard(const NetCard& card);
    static bool isFallbackNetCard(const NetCard& card);
};

#endif // CLIENTINFO_H
