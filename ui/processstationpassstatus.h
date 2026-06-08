#ifndef PROCESSSTATIONPASSSTATUS_H
#define PROCESSSTATIONPASSSTATUS_H

#include <QDateTime>
#include <QMap>
#include <QString>

class QColor;
class QStandardItemModel;

class ProcessStationPassStatus
{
public:
    enum Column
    {
        NoColumn = 0,
        ProductSnColumn = 1,
        PassColumn = 2,
        NgColumn = 3,
        PauseColumn = 4
    };

    explicit ProcessStationPassStatus(QStandardItemModel* model = nullptr);

    void setModel(QStandardItemModel* model);
    void clear();

    int rowForProductSn(const QString& productSn) const;
    bool isMarked(int row, int column) const;
    bool isPassed(const QString& productSn) const;
    bool isNg(const QString& productSn) const;
    bool isPaused(const QString& productSn) const;

    int appendRow(const QString& productSn);
    void setMark(int row, int column, bool checked, const QColor& color);

    void startTiming(const QString& productSn);
    void pauseTiming(const QString& productSn);
    void resumeTiming(const QString& productSn);
    qint64 finishTiming(const QString& productSn);

    int passCount() const;
    int ngCount() const;

private:
    QString normalizedSn(const QString& productSn) const;

private:
    QStandardItemModel* m_model{nullptr};
    QMap<QString, int> m_productStatusRows;
    QMap<QString, QDateTime> m_productStartTimes;
    QMap<QString, QDateTime> m_productPauseStartTimes;
    QMap<QString, qint64> m_productPausedSeconds;
    QMap<QString, qint64> m_productFinishedSeconds;
};

#endif // PROCESSSTATIONPASSSTATUS_H
