#ifndef PROCESSSTATIONPAGE_H
#define PROCESSSTATIONPAGE_H
// 工序站点页面
//ProcessStationPage分为两部分
//左边ProcessStationLeftPannel  : QWidget
//右边ProcessStationRightPannel : BasePageWidget

#include <QWidget>
#include <QTableWidget>
#include <QList>
#include <QVariant>

#include "basepagewidget.h"
#include "toggleswitchwidget.h"


class ProcessStationLeftPanel;
class ProcessStationRightPanel;
class ProcessStationPage : public QWidget
{
    Q_OBJECT
public:
    explicit ProcessStationPage(QWidget *parent = nullptr);

private:
    void initUI();
    void initConnect();

private:
    ProcessStationRightPanel *m_rightPanel{nullptr};
    ProcessStationLeftPanel *m_leftPanel{nullptr};
};

using TaskList = QList<QVariant>;
class ProcessStationLeftPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ProcessStationLeftPanel(QWidget* parent = nullptr);
    // 填充数据
    void setTaskData(QTableWidget* table, const TaskList& keys, const TaskList& values);
    void setTaskInfoValue(TaskList& values);
    void setTaskStatusValue(TaskList& values);


private:
    void initUI();
    void initConnect();
    //创建一个小页面,使用两次（任务单信息和任务单状态）
    QWidget* createTaskWidget(const QString &title, QTableWidget* &tableout);
    //创建扫码过站小页面
    QWidget* createPassWidget(const QString &title);

    TaskList taskInfoKeys() const;
    TaskList taskStatusKeys() const;

private:
    QWidget* m_taskInfo{nullptr};
    QWidget* m_taskStatus{nullptr};
    QWidget* m_pass{nullptr};

    QTableWidget* m_taskInfoTable{nullptr};
    QTableWidget* m_taskStatusTable{nullptr};

    TaskList m_taskInfoValue;
    TaskList m_taskStatusValue;
};

class ProcessStationRightPanel : public BasePageWidget
{
    Q_OBJECT
public:
    explicit ProcessStationRightPanel(QWidget* parent = nullptr);

signals:
    void toggleRequested(bool isChecked);

protected:
    TabConfigs Tabs() const override;
    QString pageTitle() const override;
    QString searchInfo() const override;
    QAbstractItemModel* createModel() override;
    FieldFilterProxyModel* createProxy(const QVariant &data) override;
    void addWidgetToTitle(QHBoxLayout *layout) override;

    void setCurrentSearchInfo(const QString& info);

private:
    void updateTableModelByTab(int index);

private:
    ToggleSwitchWidget* m_toggle{nullptr};
    QString m_currentSearchInfo;


};

#endif // PROCESSSTATIONPAGE_H
