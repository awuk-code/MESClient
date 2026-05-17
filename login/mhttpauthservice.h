#ifndef MHTTPAUTHSERVICE_H
#define MHTTPAUTHSERVICE_H

#include "mauthservice.h"
#include <QNetworkAccessManager>

class MHttpAuthservice : public MAuthService
{
    Q_OBJECT
public:
    explicit MHttpAuthservice(QObject* parent = nullptr);
    bool Login(const QString &usr, const QString &pass, QString &errMsg);

private:
    QNetworkAccessManager m_manager;
};

#endif // MHTTPAUTHSERVICE_H
