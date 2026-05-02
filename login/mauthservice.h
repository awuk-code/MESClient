#ifndef MAUTHSERVICE_H
#define MAUTHSERVICE_H

#include <QObject>
#include <QString>

class MAuthService
{

public:
    MAuthService();
    //virtual ~MAuthService() = default;
    virtual bool Login(const QString &usr, const QString &pass, QString &errMsg) = 0;
// signals:
//     void loginResult(bool success, const QString &errMsg);
};

#endif // MAUTHSERVICE_H
