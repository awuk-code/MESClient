#ifndef NAVPAGETYPE_H
#define NAVPAGETYPE_H

#include <QObject>

//用于页面跳转
enum class PageType
{
    Unknown = 0,

    ProductionTask,

    ProcessStation,

    RepairStation,

    RepairJudge,

    ReworkTask,
};
Q_DECLARE_METATYPE(PageType)

#endif // NAVPAGETYPE_H
