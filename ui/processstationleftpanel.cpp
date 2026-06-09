#include "processstationleftpanel.h"
#include "processstationinfosection.h"
#include "processstationpasscontroller.h"
#include "processstationpasswidget.h"

#include <QDebug>
#include <QVBoxLayout>

ProcessStationLeftPanel::ProcessStationLeftPanel(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("ProcessStationLeftPanel");
    initUI();
    initConnect();

    // 工序站点默认可能由侧边栏直接进入，此时没有任务行数据：
    // 只初始化字段名称，不显示 JSON 虚拟数据；点击开工/返工任务单后再由行数据填充。
    setTaskInfoData(QVariantMap());
    setTaskStatusData(QVariantMap());
    setAbnormalInfoData(QVariantMap());
    setReworkTaskStatusData(QVariantMap());

    setDisplayMode(DisplayMode::NormalTask);
    qDebug() << "ProcessStationLeftPanel init, default mode NormalTask";
}

void ProcessStationLeftPanel::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0,0);
    mainLayout->setSpacing(8);

    m_taskInfo = new ProcessStationInfoSection(tr("任务单信息"), this);
    m_abnormalInfo = new ProcessStationInfoSection(tr("异常品信息"), this);
    m_taskStatus = new ProcessStationInfoSection(tr("任务单状态"), this);
    m_pass = new ProcessStationPassWidget(tr("扫码过站"), this);
    m_passController = new ProcessStationPassController(m_pass, this, this);

    mainLayout->addWidget(m_taskInfo);
    mainLayout->addWidget(m_abnormalInfo);
    mainLayout->addWidget(m_taskStatus);
    mainLayout->addWidget(m_pass);
    // 扫码区域占据剩余空间
    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 0);
    mainLayout->setStretch(2, 0);
    mainLayout->setStretch(3, 1);
}

void ProcessStationLeftPanel::initConnect()
{
    if (!m_passController)
        return;

    connect(m_passController, &ProcessStationPassController::productSnScanned,
            this, &ProcessStationLeftPanel::productSnScanned);
    connect(m_passController, &ProcessStationPassController::productNgSubmitted,
            this, &ProcessStationLeftPanel::productNgSubmitted);
    connect(m_passController, &ProcessStationPassController::passStatusChanged,
            this, &ProcessStationLeftPanel::updateTaskStatusRealtime);
}

void ProcessStationLeftPanel::updateTaskStatusRealtime(int passCount, int ngCount)
{
    m_taskStatusData.insert("finishedCount", passCount);
    m_taskStatusData.insert("ngCount", ngCount);

    if (m_displayMode == DisplayMode::NormalTask)
        setTaskData(m_taskStatus, taskStatusFields(), m_taskStatusData);

    qDebug() << __FUNCTION__
             << "finishedCount:" << passCount
             << "ngCount:" << ngCount;
}

QVector<QPair<QString, QString>> ProcessStationLeftPanel::taskInfoFields() const
{
    return {
        {tr("生产任务单号"), "taskNo"},
        {tr("产品型号"), "productModel"},
        {tr("产品EPR编码"), "erpCode"},
        {tr("生产任务工单数量"), "workCount"},
        {tr("生产任务单完成日期"), "finishTime"},
        {tr("配置项配套表版本"), "routeName"},
        {tr("生产线编号"), "lineNo"}
    };
}

QVector<QPair<QString, QString>> ProcessStationLeftPanel::taskStatusFields() const
{
    return {
        {tr("当前工序完成数量"), "finishedCount"},
        {tr("当前工序NG数量"), "ngCount"},
        {tr("上一工序"), "previousProcess"},
        {tr("当前工序"), "currentProcess"},
        {tr("下一工序"), "nextProcess"},
        {tr("物料核对状态"), "materialCheckStatus"},
        {tr("工站编号"), "stationNo"}
    };
}

QVector<QPair<QString, QString>> ProcessStationLeftPanel::abnormalInfoFields() const
{
    return {
        {tr("异常处理单号"), "exceptionHandleNo"},
        {tr("返工任务单号"), "reworkTaskNo"},
        {tr("生产任务单号"), "taskNo"},
        {tr("生产工艺流程"), "routeName"},
        {tr("产品型号"), "productModel"},
        {tr("产品SN"), "productSN"},
        {tr("异常数量"), "abnormalCount"},
        {tr("异常类型"), "abnormalType"},
        {tr("异常现象"), "abnormalPhenomenon"},
        {tr("异常图片"), "abnormalImage"},
        {tr("异常上报工序"), "reportProcess"},
        {tr("异常上报时间"), "reportTime"},
        {tr("产线编码"), "lineNo"},
        {tr("工作站编号"), "stationNo"},
        {tr("上报人员"), "reporter"},
        {tr("异常处理方式"), "handleMethod"},
        {tr("当前状态"), "status"}
    };
}

QVariantMap ProcessStationLeftPanel::valuesToRowData(
    const QVector<QPair<QString, QString>>& fields,
    const TaskList& values) const
{
    QVariantMap rowData;
    for (int i = 0; i < fields.size() && i < values.size(); ++i)
        rowData.insert(fields[i].second, values[i]);

    return rowData;
}

void ProcessStationLeftPanel::setTaskData(
    ProcessStationInfoSection* section,
    const QVector<QPair<QString, QString>>& fields,
    const QVariantMap& rowData,
    int fieldPairsPerRow)
{
    if (!section)
        return;

    section->setFieldsData(fields, rowData, fieldPairsPerRow);
}

void ProcessStationLeftPanel::setTaskInfoValue(TaskList &values)
{
    m_taskInfoValue = values;
    setTaskInfoData(valuesToRowData(taskInfoFields(), values));
}

void ProcessStationLeftPanel::setTaskStatusValue(TaskList &values)
{
    m_taskStatusValue = values;
    setTaskStatusData(valuesToRowData(taskStatusFields(), values));
}

void ProcessStationLeftPanel::setAbnormalInfoValue(TaskList &values)
{
    m_abnormalInfoValue = values;
    setAbnormalInfoData(valuesToRowData(abnormalInfoFields(), values));
    qDebug() << __FUNCTION__ << "rows:" << values.size();
}

void ProcessStationLeftPanel::setReworkTaskStatusValue(TaskList &values)
{
    m_reworkTaskStatusValue = values;
    m_reworkTaskStatusData = valuesToRowData(taskStatusFields(), values);
    qDebug() << __FUNCTION__ << "rows:" << values.size();

    if (m_displayMode == DisplayMode::ReworkTask)
        setTaskData(m_taskStatus, taskStatusFields(), m_reworkTaskStatusData);
}

void ProcessStationLeftPanel::setTaskInfoData(const QVariantMap &rowData)
{
    m_taskInfoData = rowData;
    setTaskData(m_taskInfo, taskInfoFields(), rowData);
}

void ProcessStationLeftPanel::setTaskStatusData(const QVariantMap &rowData)
{
    m_taskStatusData = rowData;
    if (m_displayMode == DisplayMode::NormalTask)
        setTaskData(m_taskStatus, taskStatusFields(), rowData);
}

void ProcessStationLeftPanel::setAbnormalInfoData(const QVariantMap &rowData)
{
    m_abnormalInfoData = rowData;
    if (m_passController)
        m_passController->setAbnormalInfoData(rowData);
    setTaskData(m_abnormalInfo, abnormalInfoFields(), rowData);
}

void ProcessStationLeftPanel::setReworkTaskStatusData(const QVariantMap &rowData)
{
    m_reworkTaskStatusData = rowData;
    if (m_displayMode == DisplayMode::ReworkTask)
        setTaskData(m_taskStatus, taskStatusFields(), rowData);
}

void ProcessStationLeftPanel::setDisplayMode(DisplayMode mode)
{
    m_displayMode = mode;

    const bool isReworkTask = (mode == DisplayMode::ReworkTask);
    if (m_passController)
        m_passController->setReworkMode(isReworkTask);

    if (m_taskInfo)
        m_taskInfo->setVisible(!isReworkTask);

    if (m_abnormalInfo)
        m_abnormalInfo->setVisible(isReworkTask);

    if (m_taskStatus)
        m_taskStatus->setTitle(isReworkTask ? tr("返工维修任务单状态") : tr("任务单状态"));

    if (isReworkTask)
        setTaskData(m_taskStatus, taskStatusFields(), m_reworkTaskStatusData);
    else
        setTaskData(m_taskStatus, taskStatusFields(), m_taskStatusData);

    qDebug() << __FUNCTION__ << "mode:" << (isReworkTask ? "ReworkTask" : "NormalTask")
             << "taskInfoVisible:" << (m_taskInfo ? m_taskInfo->isVisible() : false)
             << "abnormalInfoVisible:" << (m_abnormalInfo ? m_abnormalInfo->isVisible() : false);
}

void ProcessStationLeftPanel::setPassConditionValidator(PassConditionValidator validator)
{
    if (m_passController)
        m_passController->setPassConditionValidator(validator);
}

void ProcessStationLeftPanel::clearPassStatus()
{
    if (m_passController)
        m_passController->clearPassStatus();
}
