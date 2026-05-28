#ifndef SUBHEADERWIDGET_H
#define SUBHEADERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QDate>
#include <QDateTime>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>

class SubHeaderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SubHeaderWidget(QWidget *parent = nullptr);
    void setPageNavigation(const QStringList& pagePath);
    void setProcessNavigationVisible(bool visible);
    void setCurrentProcessName(const QString& processName);

signals:
private:
    void initUI();
    void updateTime();
    void initConnect();
    void updateNavigationText();

private:
    QLabel *m_path{nullptr};
    QComboBox *m_loginTime{nullptr};
    QLabel* m_currentTime{nullptr};
    QTimer *timer{nullptr};
    QStringList m_pagePath;
    QString m_currentProcessName;
    bool m_showProcessNavigation{false};
};

#endif // SUBHEADERWIDGET_H
