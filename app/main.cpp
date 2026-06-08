#include "mainwindow.h"
// #include "mloginwedget.h"
#include "configmanager.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QRegularExpression>
#include <QScreen>
#include <QString>

typedef enum _LoginStatus{
    LoginFailed,
    UserExit,
    LoginSucess
}LoginStatus;

qreal uiScaleFactor()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen)
        return 1.0;

    const qreal scale = screen->logicalDotsPerInch() / 96.0;
    return qBound<qreal>(1.0, scale, 1.35);
}

QString scaleStyleSheetFonts(const QString& styleSheet, qreal scale)
{
    if (scale <= 1.0)
        return styleSheet;

    QString result;
    int lastPos = 0;
    QRegularExpression regex(QStringLiteral("font-size\\s*:\\s*(\\d+)px"));
    QRegularExpressionMatchIterator it = regex.globalMatch(styleSheet);

    while (it.hasNext())
    {
        const QRegularExpressionMatch match = it.next();
        result += styleSheet.mid(lastPos, match.capturedStart() - lastPos);

        const int fontSize = match.captured(1).toInt();
        const int scaledSize = qRound(fontSize * scale);
        result += QStringLiteral("font-size: %1px").arg(scaledSize);

        lastPos = match.capturedEnd();
    }

    result += styleSheet.mid(lastPos);
    return result;
}

void loadStylSheet(void)
{
    QFile file(":/res/style/style.qss");

    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << __FUNCTION__<< "style load failed";
        return;
    }

    QString styleSheet= QLatin1String(file.readAll());//读取样式表文件
    file.close();

    styleSheet = scaleStyleSheetFonts(styleSheet, uiScaleFactor());
    qApp->setStyleSheet(styleSheet);//把文件内容传参
}


int main(int argc, char *argv[])
{
   QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
   QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    loadStylSheet();

   // 运行配置从 exe 同目录 config/config.ini 读取；不存在时 ConfigManager 会自动生成默认配置。
   ConfigManager::instance().loadConfig();

   QString text = QString("Version: %1 | Build: %2 | Protocol: %3 | DeviceId: %4")
                      .arg(ConfigManager::instance().softwareVersion())
                      .arg(ConfigManager::instance().buildTime())
                      .arg(ConfigManager::instance().protocolVersion())
                      .arg(ConfigManager::instance().deviceId());
   qDebug() << __FUNCTION__<< text;

    // LoginStatus status;

    // do{
    //     MLoginWedget login;
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
