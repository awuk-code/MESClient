#ifndef PROCESSSTATIONLEFTPANEL_H
#define PROCESSSTATIONLEFTPANEL_H

#include <QList>
#include <QVariant>
#include <QVariantMap>
#include <QVector>
#include <QWidget>

#include "processstationpasscontroller.h"

class ProcessStationInfoSection;
class ProcessStationPassController;
class ProcessStationPassWidget;

using TaskList = QList<QVariant>;

class ProcessStationLeftPanel : public QWidget
{
    Q_OBJECT
public:
    enum class DisplayMode
    {
        NormalTask,
        ReworkTask
    };

    explicit ProcessStationLeftPanel(QWidget* parent = nullptr);
    // 填充数据
    void setTaskInfoValue(TaskList& values);
    void setTaskStatusValue(TaskList& values);

    void setAbnormalInfoValue(TaskList& values);
    void setReworkTaskStatusValue(TaskList& values);
    void setTaskInfoData(const QVariantMap& rowData);
    void setTaskStatusData(const QVariantMap& rowData);
    void setAbnormalInfoData(const QVariantMap& rowData);
    void setReworkTaskStatusData(const QVariantMap& rowData);
    void setDisplayMode(DisplayMode mode);
    void clearPassStatus();
    void setPassConditionValidator(PassConditionValidator validator);

signals:
    void productSnScanned(const QString& productSn);
    void productNgSubmitted(const QVariantMap& abnormalData);

private:
    void initUI();
    void initConnect();
    void setTaskData(ProcessStationInfoSection* section, const QVector<QPair<QString, QString>>& fields, const QVariantMap& rowData, int fieldPairsPerRow = 0);
    void updateTaskStatusRealtime(int passCount, int ngCount);

    QVector<QPair<QString, QString>> taskInfoFields() const;
    QVector<QPair<QString, QString>> taskStatusFields() const;
    QVector<QPair<QString, QString>> abnormalInfoFields() const;
    QVariantMap valuesToRowData(const QVector<QPair<QString, QString>>& fields, const TaskList& values) const;

private:
    ProcessStationInfoSection* m_taskInfo{nullptr};
    ProcessStationInfoSection* m_abnormalInfo{nullptr};
    ProcessStationInfoSection* m_taskStatus{nullptr};
    ProcessStationPassWidget* m_pass{nullptr};
    ProcessStationPassController* m_passController{nullptr};

    TaskList m_taskInfoValue;
    TaskList m_abnormalInfoValue;
    TaskList m_taskStatusValue;
    TaskList m_reworkTaskStatusValue;
    QVariantMap m_taskInfoData;
    QVariantMap m_abnormalInfoData;
    QVariantMap m_taskStatusData;
    QVariantMap m_reworkTaskStatusData;
    DisplayMode m_displayMode{DisplayMode::NormalTask};
};

#endif // PROCESSSTATIONLEFTPANEL_H
