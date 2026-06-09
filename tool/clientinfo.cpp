#include "clientinfo.h"

#include <QAbstractSocket>
#include <QHostAddress>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QStringList>
#include <QSysInfo>

namespace
{
bool isInvalidMac(const QString& mac)
{
    const QString normalized = mac.trimmed().toUpper();
    return normalized.isEmpty() ||
           normalized == QStringLiteral("00:00:00:00:00:00") ||
           normalized == QStringLiteral("FF:FF:FF:FF:FF:FF");
}

bool isInvalidIPv4(const QHostAddress& address)
{
    if (address.protocol() != QAbstractSocket::IPv4Protocol ||
        address.isLoopback() ||
        address.isNull())
        return true;

    const quint32 value = address.toIPv4Address();
    const quint8 first = static_cast<quint8>((value >> 24) & 0xff);
    const quint8 second = static_cast<quint8>((value >> 16) & 0xff);

    return first == 0 ||
           first == 127 ||
           first >= 224 ||
           (first == 169 && second == 254);
}

bool isVirtualInterfaceName(const QString& text)
{
    const QString lower = text.toLower();
    const QStringList keywords = {
        QStringLiteral("virtual"),
        QStringLiteral("vmware"),
        QStringLiteral("virtualbox"),
        QStringLiteral("vbox"),
        QStringLiteral("hyper-v"),
        QStringLiteral("vethernet"),
        QStringLiteral("docker"),
        QStringLiteral("wsl"),
        QStringLiteral("tap"),
        QStringLiteral("tun"),
        QStringLiteral("vpn"),
        QStringLiteral("loopback"),
        QStringLiteral("bluetooth"),
        QStringLiteral("teredo"),
        QStringLiteral("isatap"),
        QStringLiteral("6to4"),
        QStringLiteral("npcap")
    };

    for (const QString& keyword : keywords)
    {
        if (lower.contains(keyword))
            return true;
    }

    return false;
}
}

struct ClientInfo::NetCard
{
    QString ip;
    QString mac;
    QString name;
    QString humanName;
    QNetworkInterface::InterfaceFlags flags;
};

QString ClientInfo::ip()
{
    return primaryNetCard().ip;
}

QString ClientInfo::mac()
{
    return primaryNetCard().mac;
}

QVariantMap ClientInfo::info()
{
    QVariantMap data;
    const NetCard card = primaryNetCard();
    data.insert(QStringLiteral("clientIP"), card.ip);
    data.insert(QStringLiteral("clientMAC"), card.mac);
    data.insert(QStringLiteral("hostName"), QSysInfo::machineHostName());
    return data;
}

ClientInfo::NetCard ClientInfo::primaryNetCard()
{
    NetCard fallback;

    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& netInterface : interfaces)
    {
        NetCard card;
        card.mac = netInterface.hardwareAddress().trimmed();
        card.name = netInterface.name();
        card.humanName = netInterface.humanReadableName();
        card.flags = netInterface.flags();

        const QList<QNetworkAddressEntry> entries = netInterface.addressEntries();
        for (const QNetworkAddressEntry& entry : entries)
        {
            const QHostAddress address = entry.ip();
            if (isInvalidIPv4(address))
                continue;

            card.ip = address.toString();
            break;
        }

        if (isRealNetCard(card))
            return card;

        if (card.ip.isEmpty())
        {
            if (fallback.mac.isEmpty() && isFallbackNetCard(card))
                fallback = card;
            continue;
        }

        if (fallback.ip.isEmpty() && isFallbackNetCard(card))
            fallback = card;
    }

    return fallback;
}

bool ClientInfo::isRealNetCard(const NetCard& card)
{
    return isFallbackNetCard(card) &&
           !card.ip.isEmpty() &&
           !isVirtualInterfaceName(card.name) &&
           !isVirtualInterfaceName(card.humanName);
}

bool ClientInfo::isFallbackNetCard(const NetCard& card)
{
    const bool isUp = card.flags.testFlag(QNetworkInterface::IsUp);
    const bool isRunning = card.flags.testFlag(QNetworkInterface::IsRunning);
    const bool isLoopBack = card.flags.testFlag(QNetworkInterface::IsLoopBack);
    const bool isPointToPoint = card.flags.testFlag(QNetworkInterface::IsPointToPoint);
    const bool isVirtual = isVirtualInterfaceName(card.name) ||
                           isVirtualInterfaceName(card.humanName);

    return isUp &&
           isRunning &&
           !isLoopBack &&
           !isPointToPoint &&
           !isVirtual &&
           !isInvalidMac(card.mac);
}
