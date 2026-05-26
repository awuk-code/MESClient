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

signals:
private:
    void initUI();
    void updateTime();
    void initConnect();

private:
    QLabel *m_path{nullptr};
    QComboBox *m_loginTime{nullptr};
    QLabel* m_currentTime{nullptr};
    QTimer *timer{nullptr};
};

#endif // SUBHEADERWIDGET_H
