#ifndef MLOCALAUTHSERVICE_H
#define MLOCALAUTHSERVICE_H

#include "mauthservice.h"

class MLocalAuthService : public MAuthService
{
    Q_OBJECT
public:
    MLocalAuthService();
    bool Login(const QString &usr, const QString &pass,  QString &errMsg) override;
};

#endif // MLOCALAUTHSERVICE_H
