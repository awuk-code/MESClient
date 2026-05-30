#ifndef MLOGINWEDGET_H
#define MLOGINWEDGET_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QAction>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>

#include "mauthservice.h"

class MLoginWedget : public QDialog
{
    Q_OBJECT
public:
    explicit MLoginWedget(QWidget *parent = nullptr);
    ~MLoginWedget();
    void setAuthService(MAuthService* service);

protected:
    bool eventFilter(QObject *obj, QEvent *event)override;

private slots:
    void onLoginBtnClicked();
    void onCloseBtnClicked();
    void onApiLoginFinished(bool success, const QString& message, bool networkError);

private:
    void  InitLoginUI();
    void  InitConnect();
    void  setLoginWaiting(bool waiting);
    void  showLoginError(const QString& message);

    MAuthService* m_authService = nullptr;
    QTimer *errtimer;
    QString m_pendingUserName;
    QString m_pendingPassword;

    QLabel *m_title;
    QLabel *m_icon;
    QLabel *m_usrName;
    QLabel *m_password;
    QLabel *m_errString;
    QLineEdit *m_usrNameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginBtn;
    QPushButton *m_closeBtn;

    QAction *m_usrIcon;
    QAction *m_passwordIcon;

    QPoint m_dragPos;
    bool m_dragging = false;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event)override;
};

#endif // MLOGINWEDGET_H
