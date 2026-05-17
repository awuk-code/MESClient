#include "mhttpauthservice.h"


#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>


MHttpAuthservice::MHttpAuthservice(QObject *parent)
    : MAuthService(parent)
{

}

bool MHttpAuthservice::Login(const QString &usr, const QString &pass, QString &errMsg)
{
    return true;
}
