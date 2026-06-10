#include "camera.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QWidget>

QString Camera::capture(QWidget* parent)
{
    QMessageBox::information(
        parent,
        QObject::tr("高拍仪"),
        QObject::tr("高拍仪 SDK 尚未接入，请先选择一张本地图片作为拍照结果。"));

    return QFileDialog::getOpenFileName(
        parent,
        QObject::tr("选择高拍仪照片"),
        QString(),
        QObject::tr("图片文件 (*.png *.jpg *.jpeg *.bmp);;所有文件 (*.*)"));
}

bool Camera::openApp(QWidget* parent)
{
    Q_UNUSED(parent)

    // TODO(camera): 后续在这里启动高拍仪厂家软件。
    // 需要确认软件 exe 路径、启动参数，以及是否需要等待软件完成初始化。
    return false;
}

QString Camera::latestPhoto()
{
    // TODO(camera): 后续在这里读取高拍仪保存目录下最新照片。
    // 需要确认保存目录、图片格式、文件命名规则，以及是否需要过滤旧照片。
    return QString();
}

bool Camera::isSdkReady()
{
    return false;
}

QString Camera::modeText()
{
    return QObject::tr("本地图片模拟高拍仪拍照");
}
