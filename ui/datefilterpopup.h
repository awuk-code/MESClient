#ifndef DATEFILTERPOPUP_H
#define DATEFILTERPOPUP_H

#include <QWidget>
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QPushButton>

class DateFilterPopup : public QWidget
{
    Q_OBJECT
public:
    explicit DateFilterPopup(QWidget *parent = nullptr);
signals:
    void dateSelected(const QDate& date);
private slots:
    void onDisplayBtnClicked();
    void onOkBtnClicked();
    void onCancelBtnClicked();
private:
    QCalendarWidget* m_calendar;
};

#endif // DATEFILTERPOPUP_H
