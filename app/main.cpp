#include "mainwindow.h"
#include "mloginwedget.h"
#include "mlocalauthservice.h"
#include "configmanager.h"
#include <QApplication>
#include <QFile>
#include <QString>

#include "repairjudgepage.h"

typedef enum _LoginStatus{
    LoginFailed,
    UserExit,
    LoginSucess
}LoginStatus;

void loadStylSheet(void)
{
    QFile file(":/res/style/style.qss");

    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << __FUNCTION__ "style load failed";
        return;
    }

    QString styleSheet= QLatin1String(file.readAll());//读取样式表文件
    file.close();

    qDebug() << __FUNCTION__ "style load ok...."<<styleSheet;
    qApp->setStyleSheet(styleSheet);//把文件内容传参
    //        if(0 == ms)
    //        {
    //            qDebug() << "style load ok....";
    //            qApp->setStyleSheet(styleSheet);//把文件内容传参
    //        }
    //        else
    //        {
    //            QTimer::singleShot(ms, [&,styleSheet](){
    //                qDebug() << "style load ok....";
    //                qApp->setStyleSheet(styleSheet);//把文件内容传参
    //            });
    //        }
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loadStylSheet();

    //GlobalEventFilter* filter = new GlobalEventFilter(&a);
    // a.installEventFilter(filter);
    ConfigManager::instance().loadConfig(":/etc/config.json");
    // MLocalAuthService authService;
    // QString text = QString("Version: %1 | Protocol: %2")
    //                    .arg(ConfigManager::instance().softwareVersion())
    //                    .arg(ConfigManager::instance().protocolVersion());
    // qDebug() << __FUNCTION__ text;
    // LoginStatus status;

    // do{
    //     MLoginWedget login;
    //     login.setAuthService(&authService);
    //     int ret = login.exec();

    //     if(ret == QDialog::Accepted){
    //         status = LoginSucess;
    //     }else if(ret == QDialog::Rejected){
    //         status = UserExit;
    //         return 0;
    //     }else {
    //         status = LoginFailed;
    //     }
    // }while (status == LoginFailed);

    MainWindow w;

    w.showMaximized();
    return a.exec();
}
