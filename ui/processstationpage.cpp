#include "processstationpage.h"
#include "processstationleftpanel.h"
#include "processstationrightpanel.h"

#include <QDebug>
#include <QHBoxLayout>

ProcessStationPage::ProcessStationPage(QWidget *parent)
    : QWidget{parent}
{
    initUI();
    initConnect();
}

void ProcessStationPage::initUI()
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    m_leftPanel = new ProcessStationLeftPanel(this);

    m_rightPanel = new ProcessStationRightPanel(this);

    layout->addWidget(m_leftPanel);
    layout->addWidget(m_rightPanel);
    layout->setStretch(0, 2);
    layout->setStretch(1,3);
}

void ProcessStationPage::initConnect()
{
    connect(m_rightPanel, &ProcessStationRightPanel::toggleRequested,
            this, [=](bool isChecked){
                m_leftPanel->setVisible(!isChecked);
            });

    connect(m_leftPanel, &ProcessStationLeftPanel::productSnScanned,
            m_rightPanel, &ProcessStationRightPanel::addScannedProductSn);

    m_leftPanel->setPassConditionValidator(
        [this](const QString& productSn, QString* message) {
            if (!m_rightPanel)
            {
                if (message)
                    *message = tr("右侧工艺信息页面未初始化，不能PASS。");
                return false;
            }

            return m_rightPanel->validatePassReady(productSn, message);
        });

    connect(m_leftPanel, &ProcessStationLeftPanel::productNgSubmitted,
            this, [](const QVariantMap& abnormalData) {
                // TODO(backend): 这里应调用“提交NG异常/创建异常处理单”接口，
                // 成功后由后台刷新维修站待处理列表；当前阶段先保留本地事件和日志。
                // TODO(backend-request): request backend to submit NG and create repair exception.
                // Suggested API: /api/process-station/ng
                // Suggested request: taskNo, productSN, processCode, abnormalType,
                // abnormalPhenomenon, abnormalImage/fileId, reportTime, stationNo.
                // Suggested response: exceptionHandleNo, status, repairStationRow.
                qDebug() << "ProcessStationPage receive NG abnormal data:" << abnormalData;
            });
}

void ProcessStationPage::setReworkTaskMode(bool advancedPermission)
{
    if (m_leftPanel)
    {
        m_leftPanel->setDisplayMode(advancedPermission
                                    ? ProcessStationLeftPanel::DisplayMode::ReworkTask
                                    : ProcessStationLeftPanel::DisplayMode::NormalTask);
    }

    if (m_rightPanel)
    {
        // 普通工序站点不显示更换物料信息；维修站返工任务单入口才显示。
        m_rightPanel->setReplacementMaterialVisible(advancedPermission);
    }

    qDebug() << __FUNCTION__ << "advanced permission:" << advancedPermission;
}

void ProcessStationPage::clearProductionTaskData()
{
    if (!m_leftPanel)
        return;

    // 侧边栏直接进入工序站点时没有任务行数据，只显示字段名称，具体数据保持为空。
    m_leftPanel->setDisplayMode(ProcessStationLeftPanel::DisplayMode::NormalTask);
    m_leftPanel->setTaskInfoData(QVariantMap());
    m_leftPanel->clearPassStatus();
    m_leftPanel->setTaskStatusData(QVariantMap());

    if (m_rightPanel)
    {
        // 普通工序站点入口不显示返工任务单专用的更换物料信息。
        m_rightPanel->clearProductSnList();
        m_rightPanel->setReplacementMaterialVisible(false);
    }

    qDebug() << __FUNCTION__ << "clear process station task data";
}

void ProcessStationPage::setProductionTaskData(const QVariantMap &rowData)
{
    if (!m_leftPanel)
        return;

    if (rowData.isEmpty())
    {
        clearProductionTaskData();
        return;
    }

    // 生产任务点击“开工”后带入当前任务数据；左侧信息区按字段名从 rowData 中取值。
    m_leftPanel->setDisplayMode(ProcessStationLeftPanel::DisplayMode::NormalTask);
    m_leftPanel->setTaskInfoData(rowData);
    m_leftPanel->clearPassStatus();
    if (m_rightPanel)
    {
        // 开工进入的是普通工序站点，不显示返工专用的更换物料信息页签。
        m_rightPanel->clearProductSnList();
        m_rightPanel->setReplacementMaterialVisible(false);
    }

    qDebug() << __FUNCTION__
             << "taskNo =" << rowData.value("taskNo").toString()
             << "productModel =" << rowData.value("productModel").toString();
}

void ProcessStationPage::setReworkTaskData(const QVariantMap &rowData)
{
    if (!m_leftPanel || rowData.isEmpty())
        return;

    // 返工任务单从维修站的返工任务单号进入，异常品信息直接显示该行产品的所有异常字段。
    m_leftPanel->setDisplayMode(ProcessStationLeftPanel::DisplayMode::ReworkTask);
    m_leftPanel->setAbnormalInfoData(rowData);
    if (m_rightPanel)
    {
        // 只有维修站点击返工任务单号进入时，右侧才显示更换物料信息页签。
        m_rightPanel->clearProductSnList();
        m_rightPanel->setReplacementMaterialVisible(true);
    }

    qDebug() << __FUNCTION__
             << "reworkTaskNo =" << rowData.value("reworkTaskNo").toString()
             << "exceptionHandleNo =" << rowData.value("exceptionHandleNo").toString()
             << "productSN =" << rowData.value("productSN").toString();
}
