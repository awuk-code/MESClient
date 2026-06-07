#ifndef HEADERWIDGET_H
#define HEADERWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
#include <QHBoxLayout>

class QSystemTrayIcon;

class HeaderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeaderWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;


private:
    void initUI();
    void initConnect();
    void onBtnCloseClicked();
    void onBtnMinClicked();
    void onBtnMaxClicked();

private:
    QPoint m_dragPos;
    QLabel *m_title{nullptr};
    QLabel *m_userInfo{nullptr};
    QLabel *m_userIcon{nullptr};
    QLabel *m_time{nullptr};

    QPushButton* m_btnMin{nullptr};
    QPushButton* m_btnMax{nullptr};
    QPushButton* m_btnClose{nullptr};
    QSystemTrayIcon* m_trayIcon{nullptr};

signals:
};

#endif // HEADERWIDGET_H
