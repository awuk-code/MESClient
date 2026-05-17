#ifndef PROCESSSTATIONPAGE_H
#define PROCESSSTATIONPAGE_H
// 工序站点页面
//ProcessStationPage分为两部分
//左边ProcessStationLeftPannel  : QWidget
//右边ProcessStationRightPannel : BasePageWidget
#include <QWidget>

class ProcessStationPage : public QWidget
{
    Q_OBJECT
public:
    explicit ProcessStationPage(QWidget *parent = nullptr);

signals:
};

#endif // PROCESSSTATIONPAGE_H
