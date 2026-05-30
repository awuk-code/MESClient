#ifndef PROCESSSTATIONPAGE_H
#define PROCESSSTATIONPAGE_H

#include <QVariantMap>
#include <QWidget>

class ProcessStationLeftPanel;
class ProcessStationRightPanel;

class ProcessStationPage : public QWidget
{
    Q_OBJECT
public:
    explicit ProcessStationPage(QWidget *parent = nullptr);
    void setReworkTaskMode(bool advancedPermission);
    void clearProductionTaskData();
    void setProductionTaskData(const QVariantMap& rowData);
    void setReworkTaskData(const QVariantMap& rowData);

private:
    void initUI();
    void initConnect();

private:
    ProcessStationRightPanel *m_rightPanel{nullptr};
    ProcessStationLeftPanel *m_leftPanel{nullptr};
};

#endif // PROCESSSTATIONPAGE_H
