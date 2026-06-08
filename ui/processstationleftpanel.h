#ifndef PROCESSSTATIONLEFTPANEL_H
#define PROCESSSTATIONLEFTPANEL_H

#include <QList>
#include <QMap>
#include <QDateTime>
#include <QVariant>
#include <QVariantMap>
#include <QVector>
#include <QWidget>
#include <functional>

class QColor;
class QStandardItemModel;
class QTableView;
class ProcessStationInfoSection;
class ProcessStationPassWidget;

using TaskList = QList<QVariant>;
using PassConditionValidator = std::function<bool(const QString& productSn, QString* message)>;

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
    void onExecuteBtnClicked();
    void onPauseBtnClicked();
    void onResumeBtnClicked();
    void handleScanInAction(const QString& productSn);
    void handlePassResult(const QString& productSn);
    void handleNgResult(const QString& productSn);
    void handlePauseResult(bool paused);
    bool validateProductSn(const QString& productSn) const;
    bool validateProductSnForCurrentTask(const QString& productSn) const;
    bool validateSelectedProductSn(const QString& productSn) const;
    bool validatePassCondition(const QString& productSn) const;
    bool isProductPassed(const QString& productSn) const;
    bool isProductNg(const QString& productSn) const;
    bool isProductPaused(const QString& productSn) const;
    bool validateProductNotPassed(const QString& productSn, const QString& actionName) const;
    bool validateProductNotNg(const QString& productSn, const QString& actionName) const;
    bool validateProductNotPaused(const QString& productSn, const QString& actionName) const;
    QString productSnForPauseAction() const;
    QString currentProductSnFromSelection() const;
    int statusRowForProductSn(const QString& productSn) const;
    bool isStatusMarked(int row, int column) const;
    int appendStatusRow(const QString& productSn);
    void setStatusMark(int row, int column, bool checked, const QColor& color);
    void startProductTiming(const QString& productSn);
    void pauseProductTiming(const QString& productSn);
    void resumeProductTiming(const QString& productSn);
    qint64 finishProductTiming(const QString& productSn);
    void updateTaskStatusRealtime();

    QVector<QPair<QString, QString>> taskInfoFields() const;
    QVector<QPair<QString, QString>> taskStatusFields() const;
    QVector<QPair<QString, QString>> abnormalInfoFields() const;
    QVariantMap valuesToRowData(const QVector<QPair<QString, QString>>& fields, const TaskList& values) const;

private:
    ProcessStationInfoSection* m_taskInfo{nullptr};
    ProcessStationInfoSection* m_abnormalInfo{nullptr};
    ProcessStationInfoSection* m_taskStatus{nullptr};
    ProcessStationPassWidget* m_pass{nullptr};

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
    QMap<QString, QDateTime> m_productStartTimes;
    QMap<QString, QDateTime> m_productPauseStartTimes;
    QMap<QString, qint64> m_productPausedSeconds;
    QMap<QString, qint64> m_productFinishedSeconds;
    PassConditionValidator m_passConditionValidator;
    DisplayMode m_displayMode{DisplayMode::NormalTask};
};

#endif // PROCESSSTATIONLEFTPANEL_H
