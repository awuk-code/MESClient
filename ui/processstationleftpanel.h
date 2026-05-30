#ifndef PROCESSSTATIONLEFTPANEL_H
#define PROCESSSTATIONLEFTPANEL_H

#include <QList>
#include <QMap>
#include <QVariant>
#include <QVariantMap>
#include <QVector>
#include <QWidget>

class QColor;
class QLabel;
class QGridLayout;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QStandardItemModel;
class QTableView;

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
    void setTaskData(QGridLayout* layout, const QVector<QPair<QString, QString>>& fields, const QVariantMap& rowData, int fieldPairsPerRow = 0);
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

private:
    void initUI();
    void initConnect();
    // 将区域标题和表格拆开创建，后续权限切换时可以直接替换标题或显示指定区域。
    QWidget* createTaskWidget(const QString &title, QLabel* &titleLabel, QGridLayout* &layoutout);
    QWidget* createTaskTitleWidget(const QString &title, QLabel* &titleLabel, QWidget* parentWidget);
    QGridLayout* createInfoGrid(QWidget* parentWidget);
    QLabel* createInfoTitleLabel(const QString& text, QWidget* parentWidget);
    QLabel* createInfoValueLabel(const QString& text, QWidget* parentWidget);
    // 创建扫码过站小页面
    QWidget* createPassWidget(const QString &title);
    void bindPassWidgetActions();
    void handlePassAction();
    void handleScanInAction(const QString& productSn);
    void handlePassResult(const QString& productSn);
    void handleNgResult(const QString& productSn);
    void handlePauseResult(bool paused);
    bool validateProductSn(const QString& productSn) const;
    bool validatePassCondition(const QString& productSn) const;
    QString currentProductSnFromInputOrSelection() const;
    int statusRowForProductSn(const QString& productSn) const;
    int appendStatusRow(const QString& productSn);
    void setStatusMark(int row, int column, bool checked, const QColor& color);
    void updateTaskStatusRealtime();

    QVector<QPair<QString, QString>> taskInfoFields() const;
    QVector<QPair<QString, QString>> taskStatusFields() const;
    QVector<QPair<QString, QString>> abnormalInfoFields() const;
    QVariantMap valuesToRowData(const QVector<QPair<QString, QString>>& fields, const TaskList& values) const;

private:
    QWidget* m_taskInfo{nullptr};
    QWidget* m_abnormalInfo{nullptr};
    QWidget* m_taskStatus{nullptr};
    QWidget* m_pass{nullptr};

    QLabel* m_taskInfoTitleLabel{nullptr};
    QLabel* m_abnormalInfoTitleLabel{nullptr};
    QLabel* m_taskStatusTitleLabel{nullptr};

    QGridLayout* m_taskInfoLayout{nullptr};
    QGridLayout* m_abnormalInfoLayout{nullptr};
    QGridLayout* m_taskStatusLayout{nullptr};

    QRadioButton* m_scanInRadio{nullptr};
    QRadioButton* m_passRadio{nullptr};
    QRadioButton* m_ngRadio{nullptr};
    QLineEdit* m_scanEdit{nullptr};
    QPushButton* m_executeBtn{nullptr};
    QPushButton* m_pauseBtn{nullptr};
    QPushButton* m_resumeBtn{nullptr};
    QTableView* m_statusTableView{nullptr};
    QStandardItemModel* m_statusModel{nullptr};

    TaskList m_taskInfoValue;
    TaskList m_abnormalInfoValue;
    TaskList m_taskStatusValue;
    TaskList m_reworkTaskStatusValue;
    QVariantMap m_taskInfoData;
    QVariantMap m_abnormalInfoData;
    QVariantMap m_taskStatusData;
    QVariantMap m_reworkTaskStatusData;
    QMap<QString, int> m_productStatusRows;
    DisplayMode m_displayMode{DisplayMode::NormalTask};
};

#endif // PROCESSSTATIONLEFTPANEL_H
