#include "processstationpasscontroller.h"
#include "processstationpassdialogs.h"
#include "processstationpasswidget.h"

#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QTableView>

namespace
{
constexpr int STATUS_COL_PRODUCT_SN = ProcessStationPassStatus::ProductSnColumn;
constexpr int STATUS_COL_PASS = ProcessStationPassStatus::PassColumn;
constexpr int STATUS_COL_NG = ProcessStationPassStatus::NgColumn;
constexpr int STATUS_COL_PAUSE = ProcessStationPassStatus::PauseColumn;

QString formatDuration(qint64 seconds)
{
    const qint64 safeSeconds = qMax<qint64>(0, seconds);
    const qint64 h = safeSeconds / 3600;
    const qint64 m = (safeSeconds % 3600) / 60;
    const qint64 s = safeSeconds % 60;
    return QStringLiteral("%1:%2:%3")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'));
}
}

ProcessStationPassController::ProcessStationPassController(
    ProcessStationPassWidget* passWidget,
    QWidget* dialogParent,
    QObject* parent)
    : QObject(parent),
      m_passWidget(passWidget),
      m_dialogParent(dialogParent)
{
    if (m_passWidget)
    {
        m_statusTableView = m_passWidget->statusTableView();
        m_statusModel = m_passWidget->statusModel();
        m_passStatus.setModel(m_statusModel);
    }

    initConnect();
}

void ProcessStationPassController::setReworkMode(bool reworkMode)
{
    m_reworkMode = reworkMode;
}

void ProcessStationPassController::setAbnormalInfoData(const QVariantMap& rowData)
{
    m_abnormalInfoData = rowData;
}

void ProcessStationPassController::setPassConditionValidator(PassConditionValidator validator)
{
    m_passConditionValidator = validator;
}

void ProcessStationPassController::clearPassStatus()
{
    m_passStatus.clear();
    emit passStatusChanged(0, 0);
    qDebug() << __FUNCTION__ << "pass status cleared";
}

void ProcessStationPassController::initConnect()
{
    if (!m_passWidget)
        return;

    connect(m_passWidget, &ProcessStationPassWidget::executeRequested,
            this, &ProcessStationPassController::onExecuteRequested);
    connect(m_passWidget, &ProcessStationPassWidget::pauseRequested,
            this, &ProcessStationPassController::onPauseRequested);
    connect(m_passWidget, &ProcessStationPassWidget::resumeRequested,
            this, &ProcessStationPassController::onResumeRequested);
}

void ProcessStationPassController::onExecuteRequested()
{
    if (!m_passWidget)
        return;

    const QString productSn = m_passWidget->scanText();
    if (!validateProductSn(productSn))
        return;

    if (m_passWidget->isScanInMode())
        handleScanInAction(productSn);
    else if (m_passWidget->isPassMode())
        handlePassResult(productSn);
    else if (m_passWidget->isNgMode())
        handleNgResult(productSn);
}

void ProcessStationPassController::onPauseRequested()
{
    const QString productSn = productSnForPauseAction();
    if (!validateProductSn(productSn))
        return;
    if (!validateProductNotPassed(productSn, tr("暂停")))
        return;
    if (!validateProductNotNg(productSn, tr("暂停")))
        return;
    if (isProductPaused(productSn))
    {
        QMessageBox::warning(m_dialogParent, tr("扫码过站"), tr("该产品已暂停。"));
        return;
    }

    PauseReasonDialog dialog(productSn, m_dialogParent);
    if (dialog.exec() != QDialog::Accepted)
    {
        qDebug() << __FUNCTION__ << "pause canceled productSn:" << productSn;
        return;
    }

    qDebug() << __FUNCTION__
             << "productSn:" << productSn
             << "type:" << dialog.pauseType()
             << "code:" << dialog.relatedCode()
             << "description:" << dialog.description();

    handlePauseResult(true);
}

void ProcessStationPassController::onResumeRequested()
{
    const QString productSn = productSnForPauseAction();
    if (!validateProductSn(productSn))
        return;
    if (!validateProductNotPassed(productSn, tr("解除暂停")))
        return;
    if (!validateProductNotNg(productSn, tr("解除暂停")))
        return;

    const int row = statusRowForProductSn(productSn);
    if (row < 0)
    {
        QMessageBox::warning(m_dialogParent, tr("扫码过站"), tr("请先扫入该产品SN。"));
        qDebug() << __FUNCTION__ << "product not scanned:" << productSn;
        return;
    }

    if (!isProductPaused(productSn))
    {
        QMessageBox::warning(m_dialogParent, tr("扫码过站"), tr("该产品当前未暂停。"));
        return;
    }

    ResumeReasonDialog dialog(productSn, m_dialogParent);
    if (dialog.exec() != QDialog::Accepted)
    {
        qDebug() << __FUNCTION__ << "resume canceled productSn:" << productSn;
        return;
    }

    qDebug() << __FUNCTION__
             << "productSn:" << productSn
             << "reason:" << dialog.reason();

    handlePauseResult(false);
}

void ProcessStationPassController::handleScanInAction(const QString& productSn)
{
    if (!validateProductSnForCurrentTask(productSn))
        return;

    int row = statusRowForProductSn(productSn);
    if (row >= 0 && isProductPassed(productSn))
    {
        if (m_statusTableView)
            m_statusTableView->selectRow(row);

        QMessageBox::warning(m_dialogParent, tr("扫码过站"), tr("该产品已PASS，不能重复操作。"));
        qDebug() << __FUNCTION__ << "product already passed:" << productSn;
        return;
    }

    if (row >= 0 && isProductNg(productSn))
    {
        if (m_statusTableView)
            m_statusTableView->selectRow(row);

        QMessageBox::warning(m_dialogParent, tr("扫码过站"), tr("该产品已NG，已转入维修站。"));
        qDebug() << __FUNCTION__ << "product already NG:" << productSn;
        return;
    }

    if (row >= 0)
    {
        if (m_statusTableView)
            m_statusTableView->selectRow(row);

        emit productSnScanned(productSn);
        QMessageBox::information(m_dialogParent, tr("扫码过站"), tr("此SN码已扫入，请确认当前状态。"));
        qDebug() << __FUNCTION__ << "product already scanned:" << productSn << "row:" << row;
        return;
    }

    row = appendStatusRow(productSn);
    if (m_statusTableView)
        m_statusTableView->selectRow(row);

    startProductTiming(productSn);
    notifyPassStatusChanged();
    emit productSnScanned(productSn);
    qDebug() << __FUNCTION__ << "scan in productSn:" << productSn << "row:" << row;
}

void ProcessStationPassController::handlePassResult(const QString& productSn)
{
    const int row = statusRowForProductSn(productSn);
    if (row < 0)
    {
        QMessageBox::warning(m_dialogParent, tr("扫码过站"), tr("请先扫入该产品SN。"));
        qDebug() << __FUNCTION__ << "product not scanned:" << productSn;
        return;
    }

    if (!validatePassCondition(productSn))
        return;
    if (!validateProductNotPassed(productSn, tr("PASS")))
        return;
    if (!validateProductNotNg(productSn, tr("PASS")))
        return;
    if (!validateProductNotPaused(productSn, tr("PASS")))
        return;

    const int ret = QMessageBox::question(
        m_dialogParent,
        tr("扫码过站"),
        tr("确认该产品通过当前工序？"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (ret != QMessageBox::Yes)
    {
        qDebug() << __FUNCTION__ << "PASS canceled productSn:" << productSn;
        return;
    }

    setStatusMark(row, STATUS_COL_PASS, true, QColor(0, 150, 80));
    setStatusMark(row, STATUS_COL_NG, false, QColor());
    const qint64 elapsedSeconds = finishProductTiming(productSn);
    notifyPassStatusChanged();

    qDebug() << __FUNCTION__
             << "PASS productSn:" << productSn
             << "row:" << row
             << "elapsed:" << formatDuration(elapsedSeconds);
}

void ProcessStationPassController::handleNgResult(const QString& productSn)
{
    const int row = statusRowForProductSn(productSn);
    if (row < 0)
    {
        QMessageBox::warning(m_dialogParent, tr("扫码过站"), tr("请先扫入该产品SN。"));
        qDebug() << __FUNCTION__ << "product not scanned:" << productSn;
        return;
    }
    if (!validateProductNotPassed(productSn, tr("NG")))
        return;
    if (!validateProductNotNg(productSn, tr("NG")))
        return;
    if (!validateProductNotPaused(productSn, tr("NG")))
        return;

    NgAbnormalDialog dialog(productSn, m_dialogParent);
    if (dialog.exec() != QDialog::Accepted)
    {
        qDebug() << __FUNCTION__ << "NG canceled productSn:" << productSn;
        return;
    }

    setStatusMark(row, STATUS_COL_NG, true, QColor(210, 72, 72));
    setStatusMark(row, STATUS_COL_PASS, false, QColor());
    const qint64 elapsedSeconds = finishProductTiming(productSn);

    QVariantMap abnormalData;
    abnormalData.insert("productSN", productSn);
    abnormalData.insert("abnormalType", dialog.abnormalType());
    abnormalData.insert("abnormalPhenomenon", dialog.phenomenon());
    abnormalData.insert("abnormalImage", dialog.imagePath());
    abnormalData.insert("nextProcess", dialog.nextProcess());
    abnormalData.insert("reportTime", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    abnormalData.insert("status", tr("待处理"));
    abnormalData.insert("elapsedSeconds", elapsedSeconds);
    emit productNgSubmitted(abnormalData);
    notifyPassStatusChanged();

    qDebug() << __FUNCTION__
             << "NG productSn:" << productSn
             << "abnormal:" << dialog.summary()
             << "row:" << row
             << "elapsed:" << formatDuration(elapsedSeconds);
}

void ProcessStationPassController::handlePauseResult(bool paused)
{
    const QString productSn = productSnForPauseAction();
    if (!validateProductSn(productSn))
        return;

    const int row = statusRowForProductSn(productSn);
    if (row < 0)
    {
        QMessageBox::warning(m_dialogParent, tr("扫码过站"), tr("请先扫入该产品SN。"));
        qDebug() << __FUNCTION__ << "product not scanned:" << productSn;
        return;
    }
    if (!paused && !isProductPaused(productSn))
    {
        QMessageBox::warning(m_dialogParent, tr("扫码过站"), tr("该产品当前未暂停。"));
        return;
    }

    setStatusMark(row, STATUS_COL_PAUSE, paused, QColor(230, 145, 56));
    if (paused)
        pauseProductTiming(productSn);
    else
        resumeProductTiming(productSn);
    notifyPassStatusChanged();

    qDebug() << __FUNCTION__
             << "productSn:" << productSn
             << "paused:" << paused
             << "row:" << row;
}

bool ProcessStationPassController::validateProductSn(const QString& productSn) const
{
    if (!productSn.isEmpty())
        return true;

    QMessageBox::warning(
        m_dialogParent,
        tr("扫码过站"),
        tr("请先扫描或输入产品SN。"));
    qDebug() << __FUNCTION__ << "empty productSn";
    return false;
}

bool ProcessStationPassController::validateProductSnForCurrentTask(const QString& productSn) const
{
    if (!validateProductSn(productSn))
        return false;

    const QString reworkProductSn = m_abnormalInfoData.value("productSN").toString().trimmed();
    if (m_reworkMode && !reworkProductSn.isEmpty() && productSn != reworkProductSn)
    {
        QMessageBox::warning(
            m_dialogParent,
            tr("扫码过站"),
            tr("当前返工任务单产品SN为%1，不能扫入%2。").arg(reworkProductSn, productSn));
        return false;
    }

    return true;
}

bool ProcessStationPassController::validatePassCondition(const QString& productSn) const
{
    if (m_passConditionValidator)
    {
        QString message;
        const bool ok = m_passConditionValidator(productSn, &message);
        if (!ok)
        {
            QMessageBox::warning(
                m_dialogParent,
                tr("扫码过站"),
                message.isEmpty() ? tr("右侧工艺信息未完成，不能PASS。") : message);
            return false;
        }

        return true;
    }

    qDebug() << __FUNCTION__ << "right-side material/upload validation not injected";
    return true;
}

bool ProcessStationPassController::isProductPassed(const QString& productSn) const
{
    return m_passStatus.isPassed(productSn);
}

bool ProcessStationPassController::isProductNg(const QString& productSn) const
{
    return m_passStatus.isNg(productSn);
}

bool ProcessStationPassController::isProductPaused(const QString& productSn) const
{
    return m_passStatus.isPaused(productSn);
}

bool ProcessStationPassController::validateProductNotPassed(
    const QString& productSn,
    const QString& actionName) const
{
    if (!isProductPassed(productSn))
        return true;

    QMessageBox::warning(
        m_dialogParent,
        tr("扫码过站"),
        tr("该产品已PASS，不能%1。").arg(actionName));
    qDebug() << __FUNCTION__
             << "product already passed:" << productSn
             << "action:" << actionName;
    return false;
}

bool ProcessStationPassController::validateProductNotNg(
    const QString& productSn,
    const QString& actionName) const
{
    if (!isProductNg(productSn))
        return true;

    QMessageBox::warning(
        m_dialogParent,
        tr("扫码过站"),
        tr("该产品已NG并转入维修站，不能%1。").arg(actionName));
    qDebug() << __FUNCTION__
             << "product already NG:" << productSn
             << "action:" << actionName;
    return false;
}

bool ProcessStationPassController::validateProductNotPaused(
    const QString& productSn,
    const QString& actionName) const
{
    if (!isProductPaused(productSn))
        return true;

    QMessageBox::warning(
        m_dialogParent,
        tr("扫码过站"),
        tr("请先解除暂停。"));
    qDebug() << __FUNCTION__
             << "product paused:" << productSn
             << "action:" << actionName;
    return false;
}

QString ProcessStationPassController::productSnForPauseAction() const
{
    const QString selectedSn = currentProductSnFromSelection();
    if (!selectedSn.trimmed().isEmpty())
        return selectedSn.trimmed();

    return m_passWidget ? m_passWidget->scanText() : QString();
}

QString ProcessStationPassController::currentProductSnFromSelection() const
{
    if (!m_statusTableView || !m_statusModel)
        return QString();

    const QModelIndex currentIndex = m_statusTableView->currentIndex();
    if (!currentIndex.isValid())
        return QString();

    return m_statusModel->index(currentIndex.row(), STATUS_COL_PRODUCT_SN)
        .data(Qt::DisplayRole)
        .toString();
}

int ProcessStationPassController::statusRowForProductSn(const QString& productSn) const
{
    return m_passStatus.rowForProductSn(productSn);
}

int ProcessStationPassController::appendStatusRow(const QString& productSn)
{
    return m_passStatus.appendRow(productSn);
}

void ProcessStationPassController::setStatusMark(
    int row,
    int column,
    bool checked,
    const QColor& color)
{
    m_passStatus.setMark(row, column, checked, color);
}

void ProcessStationPassController::startProductTiming(const QString& productSn)
{
    m_passStatus.startTiming(productSn);
}

void ProcessStationPassController::pauseProductTiming(const QString& productSn)
{
    m_passStatus.pauseTiming(productSn);
}

void ProcessStationPassController::resumeProductTiming(const QString& productSn)
{
    m_passStatus.resumeTiming(productSn);
}

qint64 ProcessStationPassController::finishProductTiming(const QString& productSn)
{
    return m_passStatus.finishTiming(productSn);
}

void ProcessStationPassController::notifyPassStatusChanged()
{
    emit passStatusChanged(m_passStatus.passCount(), m_passStatus.ngCount());
}
