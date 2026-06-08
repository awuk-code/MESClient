#include "processstationpassstatus.h"

#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QFont>
#include <QStandardItem>
#include <QStandardItemModel>

namespace
{
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

ProcessStationPassStatus::ProcessStationPassStatus(QStandardItemModel* model)
    : m_model(model)
{
}

void ProcessStationPassStatus::setModel(QStandardItemModel* model)
{
    m_model = model;
}

void ProcessStationPassStatus::clear()
{
    m_productStatusRows.clear();
    m_productStartTimes.clear();
    m_productPauseStartTimes.clear();
    m_productPausedSeconds.clear();
    m_productFinishedSeconds.clear();

    if (m_model)
        m_model->removeRows(0, m_model->rowCount());
}

int ProcessStationPassStatus::rowForProductSn(const QString& productSn) const
{
    return m_productStatusRows.value(normalizedSn(productSn), -1);
}

bool ProcessStationPassStatus::isMarked(int row, int column) const
{
    if (!m_model || row < 0 || row >= m_model->rowCount())
        return false;

    return !m_model->index(row, column)
        .data(Qt::DisplayRole)
        .toString()
        .trimmed()
        .isEmpty();
}

bool ProcessStationPassStatus::isPassed(const QString& productSn) const
{
    return isMarked(rowForProductSn(productSn), PassColumn);
}

bool ProcessStationPassStatus::isNg(const QString& productSn) const
{
    return isMarked(rowForProductSn(productSn), NgColumn);
}

bool ProcessStationPassStatus::isPaused(const QString& productSn) const
{
    return isMarked(rowForProductSn(productSn), PauseColumn);
}

int ProcessStationPassStatus::appendRow(const QString& productSn)
{
    if (!m_model)
        return -1;

    const QString sn = normalizedSn(productSn);
    if (sn.isEmpty())
        return -1;

    const int existingRow = rowForProductSn(sn);
    if (existingRow >= 0)
        return existingRow;

    const int row = m_model->rowCount();
    m_model->insertRow(row);
    m_model->setItem(row, NoColumn, new QStandardItem(QString::number(row + 1)));
    m_model->setItem(row, ProductSnColumn, new QStandardItem(sn));
    m_model->setItem(row, PassColumn, new QStandardItem);
    m_model->setItem(row, NgColumn, new QStandardItem);
    m_model->setItem(row, PauseColumn, new QStandardItem);

    m_productStatusRows.insert(sn, row);
    return row;
}

void ProcessStationPassStatus::setMark(int row, int column, bool checked, const QColor& color)
{
    if (!m_model || row < 0 || row >= m_model->rowCount())
        return;

    auto item = m_model->item(row, column);
    if (!item)
    {
        item = new QStandardItem;
        m_model->setItem(row, column, item);
    }

    item->setText(checked ? QString::fromUtf8("√") : QString());
    item->setTextAlignment(Qt::AlignCenter);
    item->setForeground(checked ? QBrush(color) : QBrush());

    QFont font = item->font();
    font.setBold(checked);
    item->setFont(font);
}

void ProcessStationPassStatus::startTiming(const QString& productSn)
{
    const QString sn = normalizedSn(productSn);
    if (sn.isEmpty() || m_productStartTimes.contains(sn))
        return;

    m_productStartTimes.insert(sn, QDateTime::currentDateTime());
    m_productPausedSeconds.insert(sn, 0);

    // TODO(backend): 扫入成功后应把开始时间提交给后台，异常退出后由后台恢复当前工序状态。
    qDebug() << __FUNCTION__ << "productSn:" << sn << "start:" << m_productStartTimes.value(sn);
}

void ProcessStationPassStatus::pauseTiming(const QString& productSn)
{
    const QString sn = normalizedSn(productSn);
    if (sn.isEmpty() || m_productPauseStartTimes.contains(sn))
        return;

    if (!m_productStartTimes.contains(sn))
    {
        // TODO(backend): 重新进入软件后，暂停/恢复应从后台读取历史开始时间；当前本地缺失时只能从当前时刻补起。
        m_productStartTimes.insert(sn, QDateTime::currentDateTime());
        m_productPausedSeconds.insert(sn, 0);
    }

    m_productPauseStartTimes.insert(sn, QDateTime::currentDateTime());
    qDebug() << __FUNCTION__ << "productSn:" << sn << "pauseStart:" << m_productPauseStartTimes.value(sn);
}

void ProcessStationPassStatus::resumeTiming(const QString& productSn)
{
    const QString sn = normalizedSn(productSn);
    if (sn.isEmpty() || !m_productPauseStartTimes.contains(sn))
        return;

    const qint64 pausedSeconds =
        m_productPauseStartTimes.value(sn).secsTo(QDateTime::currentDateTime());
    m_productPausedSeconds[sn] = m_productPausedSeconds.value(sn, 0) + qMax<qint64>(0, pausedSeconds);
    m_productPauseStartTimes.remove(sn);

    // TODO(backend): 恢复生产时应提交解除暂停原因和暂停时长，后台负责累计工时。
    qDebug() << __FUNCTION__
             << "productSn:" << sn
             << "pausedSeconds:" << pausedSeconds
             << "totalPaused:" << m_productPausedSeconds.value(sn);
}

qint64 ProcessStationPassStatus::finishTiming(const QString& productSn)
{
    const QString sn = normalizedSn(productSn);
    if (sn.isEmpty())
        return 0;

    if (m_productPauseStartTimes.contains(sn))
        resumeTiming(sn);

    const QDateTime startTime = m_productStartTimes.value(sn);
    if (!startTime.isValid())
        return 0;

    const qint64 elapsed =
        qMax<qint64>(0, startTime.secsTo(QDateTime::currentDateTime()) -
                            m_productPausedSeconds.value(sn, 0));
    m_productFinishedSeconds.insert(sn, elapsed);

    // TODO(backend): PASS/NG 完成时应提交结束时间、有效工时和暂停累计时长。
    qDebug() << __FUNCTION__ << "productSn:" << sn << "elapsed:" << formatDuration(elapsed);
    return elapsed;
}

int ProcessStationPassStatus::passCount() const
{
    if (!m_model)
        return 0;

    int count = 0;
    for (int row = 0; row < m_model->rowCount(); ++row)
    {
        if (isMarked(row, PassColumn))
            ++count;
    }

    return count;
}

int ProcessStationPassStatus::ngCount() const
{
    if (!m_model)
        return 0;

    int count = 0;
    for (int row = 0; row < m_model->rowCount(); ++row)
    {
        if (isMarked(row, NgColumn))
            ++count;
    }

    return count;
}

QString ProcessStationPassStatus::normalizedSn(const QString& productSn) const
{
    return productSn.trimmed();
}
