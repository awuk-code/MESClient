#ifndef NETWORKTYPES_H
#define NETWORKTYPES_H

#include <QByteArray>
#include <QJsonValue>
#include <QString>

struct ApiResponse
{
    bool success = false;
    bool networkError = false;
    int httpStatus = 0;
    int businessCode = -1;
    QString businessCodeText;
    QString message;
    QJsonValue data;
    QByteArray rawData;
};

#endif // NETWORKTYPES_H
