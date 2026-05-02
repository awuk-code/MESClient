#include "mainwindow.h"
#include "mloginwedget.h"
#include "mlocalauthservice.h"
#include "configmanager.h"
#include "MDebug.h"

#include <QApplication>

typedef enum _LoginStatus{
    LoginFailed,
    UserExit,
    LoginSucess
}LoginStatus;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

/****
    ConfigManager::instance().loadConfig(":/etc/config.json");
    MLocalAuthService authService;
    QString text = QString("Version: %1 | Protocol: %2")
                       .arg(ConfigManager::instance().softwareVersion())
                       .arg(ConfigManager::instance().protocolVersion());
    funcDebug() << text;
    LoginStatus status;

    do{
        MLoginWedget login;
        login.setAuthService(&authService);
        int ret = login.exec();

        if(ret == QDialog::Accepted){
            status = LoginSucess;
        }else if(ret == QDialog::Rejected){
            status = UserExit;
            return 0;
        }else {
            status = LoginFailed;
        }
    }while (status == LoginFailed);

***/

    MainWindow w;
    w.show();

    return a.exec();

}
