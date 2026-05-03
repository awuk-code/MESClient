#ifndef HEADERWIDGET_H
#define HEADERWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
#include <QHBoxLayout>

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

private:
    QLabel *m_title{nullptr};
    QLabel *m_userInfo{nullptr};
    QLabel *m_userIcon{nullptr};
    QLabel *m_time{nullptr};

    QPushButton* m_btnMin{nullptr};
    QPushButton* m_btnMax{nullptr};
    QPushButton* m_btnClose{nullptr};

    QPoint m_dragPos;

signals:
};

#endif // HEADERWIDGET_H
