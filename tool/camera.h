#ifndef CAMERA_H
#define CAMERA_H

#include <QString>

class QWidget;

class Camera
{
public:
    // 打开高拍仪并拍照，返回拍摄照片路径。
    // 当前未接入高拍仪 SDK，先使用本地图片选择模拟拍照结果。
    static QString capture(QWidget* parent = nullptr);

    // 启动高拍仪厂家软件。
    static bool openApp(QWidget* parent = nullptr);

    // 获取高拍仪保存目录下最新的照片路径。
    static QString latestPhoto();

    // 当前是否已经接入真实高拍仪 SDK。
    static bool isSdkReady();

    // 获取当前实现模式说明。
    static QString modeText();
};

#endif // CAMERA_H
