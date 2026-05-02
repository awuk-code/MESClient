#include "mlocalauthservice.h"

MLocalAuthService::MLocalAuthService() {}

bool MLocalAuthService::Login(const QString &usr, const QString &pass,  QString &errMsg)
{
    if(usr != "admin"){
       errMsg = "用户名不存在";
        return false;
    }
    if (pass != "123456")
    {
       errMsg = "密码错误";
        return false;
    }

    return true;
}
