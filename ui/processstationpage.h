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

class QComboBox;
class QLabel;

class ProcessStationLeftPanel;
class ProcessStationRightPanel;
class PdfViewWidget;
class ProcessStationPage : public QWidget
{
    Q_OBJECT
public:
    explicit ProcessStationPage(QWidget *parent = nullptr);
    void setReworkTaskMode(bool advancedPermission);

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
    enum class DisplayMode
    {
        NormalTask,
        ReworkTask
    };

    explicit ProcessStationLeftPanel(QWidget* parent = nullptr);
    // 填充数据
    void setTaskData(QTableWidget* table, const TaskList& keys, const TaskList& values);
    void setTaskInfoValue(TaskList& values);
    void setTaskStatusValue(TaskList& values);

    void setAbnormalInfoValue(TaskList& values);
    void setReworkTaskStatusValue(TaskList& values);
    void setDisplayMode(DisplayMode mode);


private:
    void initUI();
    void initConnect();
    // 将区域标题和表格拆开创建，后续权限切换时可以直接替换标题或显示指定区域。
    QWidget* createTaskWidget(const QString &title, QLabel* &titleLabel, QTableWidget* &tableout);
    QWidget* createTaskTitleWidget(const QString &title, QLabel* &titleLabel, QWidget* parentWidget);
    QTableWidget* createTaskTable(QWidget* parentWidget);
    //创建扫码过站小页面
    QWidget* createPassWidget(const QString &title);

    TaskList taskInfoKeys() const;
    TaskList taskStatusKeys() const;
    TaskList abnormalInfoKeys() const;

private:
    QWidget* m_taskInfo{nullptr};
    QWidget* m_abnormalInfo{nullptr};
    QWidget* m_taskStatus{nullptr};
    QWidget* m_pass{nullptr};

    QLabel* m_taskInfoTitleLabel{nullptr};
    QLabel* m_abnormalInfoTitleLabel{nullptr};
    QLabel* m_taskStatusTitleLabel{nullptr};

    QTableWidget* m_taskInfoTable{nullptr};
    QTableWidget* m_abnormalInfoTable{nullptr};
    QTableWidget* m_taskStatusTable{nullptr};

    TaskList m_taskInfoValue;
    TaskList m_abnormalInfoValue;
    TaskList m_taskStatusValue;
    TaskList m_reworkTaskStatusValue;
    DisplayMode m_displayMode{DisplayMode::NormalTask};
};

class ProcessStationRightPanel : public BasePageWidget
{
    Q_OBJECT
public:
    explicit ProcessStationRightPanel(QWidget* parent = nullptr);
    void setReplacementMaterialVisible(bool visible);

signals:
    void toggleRequested(bool isChecked);
protected:
    TabConfigs Tabs() const override;
    QString pageTitle() const override;
    QString searchInfo() const override;
    QAbstractItemModel* createModel() override;
    FieldFilterProxyModel* createProxy(const QVariant &data) override;
    void addWidgetToTitle(QHBoxLayout *layout) override;
    void setupSearchLayout(QHBoxLayout* layout) override;

    void setCurrentSearchInfo(const QString& info);

private:
    void updateTableModelByTab(int index);
    void updateSearchBarByTab(int index);
    int replacementMaterialTabIndex() const;

private:
    ToggleSwitchWidget* m_toggle{nullptr};
    QString m_currentSearchInfo;
    QLabel* m_productSnLabel{nullptr};
    QComboBox* m_productSnCombo{nullptr};
    //pdf
    // m_processPdfPage
    //m_referencePdfPage
    //normal
    PdfViewWidget* m_processPdfPage{nullptr};
    PdfViewWidget* m_referencePdfPage{nullptr};

    QWidget* m_uploadPage{nullptr};
    bool m_replacementMaterialVisible{true};
};

#endif // PROCESSSTATIONPAGE_H
