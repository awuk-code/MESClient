#ifndef PROCESSSTATIONPASSCONTROLLER_H
#define PROCESSSTATIONPASSCONTROLLER_H

#include "processstationpassstatus.h"

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <functional>

class QColor;
class QStandardItemModel;
class QTableView;
class QWidget;
class ProcessStationPassWidget;

using PassConditionValidator = std::function<bool(const QString& productSn, QString* message)>;

class ProcessStationPassController : public QObject
{
    Q_OBJECT
public:
    explicit ProcessStationPassController(
        ProcessStationPassWidget* passWidget,
        QWidget* dialogParent,
        QObject* parent = nullptr);

    void setReworkMode(bool reworkMode);
    void setAbnormalInfoData(const QVariantMap& rowData);
    void setPassConditionValidator(PassConditionValidator validator);
    void clearPassStatus();

signals:
    void productSnScanned(const QString& productSn);
    void productNgSubmitted(const QVariantMap& abnormalData);
    void passStatusChanged(int passCount, int ngCount);

private:
    void initConnect();
    void onExecuteRequested();
    void onPauseRequested();
    void onResumeRequested();

    void handleScanInAction(const QString& productSn);
    void handlePassResult(const QString& productSn);
    void handleNgResult(const QString& productSn);
    void handlePauseResult(bool paused);

    bool validateProductSn(const QString& productSn) const;
    bool validateProductSnForCurrentTask(const QString& productSn) const;
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
    int appendStatusRow(const QString& productSn);
    void setStatusMark(int row, int column, bool checked, const QColor& color);
    void startProductTiming(const QString& productSn);
    void pauseProductTiming(const QString& productSn);
    void resumeProductTiming(const QString& productSn);
    qint64 finishProductTiming(const QString& productSn);
    void notifyPassStatusChanged();

private:
    ProcessStationPassWidget* m_passWidget{nullptr};
    QWidget* m_dialogParent{nullptr};
    QTableView* m_statusTableView{nullptr};
    QStandardItemModel* m_statusModel{nullptr};
    ProcessStationPassStatus m_passStatus;
    PassConditionValidator m_passConditionValidator;
    QVariantMap m_abnormalInfoData;
    bool m_reworkMode{false};
};

#endif // PROCESSSTATIONPASSCONTROLLER_H
