#ifndef MHTTPAUTHSERVICE_H
#define MHTTPAUTHSERVICE_H

#include "mauthservice.h"
#include <QNetworkAccessManager>

class MHttpAuthservice : public MAuthService
{
    Q_OBJECT
public:
    explicit MHttpAuthservice(QObject* parent = nullptr);
    //void bool Login(const QString& user,  const QString& password, QString &errMsg) override;

private:
    QNetworkAccessManager m_manager;
};

#endif // MHTTPAUTHSERVICE_H
