#include "mlocalauthservice.h"

MLocalAuthService::MLocalAuthService() {}

bool MLocalAuthService::Login(const QString &usr, const QString &pass,  QString &errMsg)
{
    if(usr != "admin"){
       errMsg = tr("用户名不存在");
        return false;
    }
    if (pass != "123456")
    {
       errMsg = tr("密码错误");
        return false;
    }

    return true;
}
