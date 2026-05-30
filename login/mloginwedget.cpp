#include "mloginwedget.h"
#include "apiservice.h"

#include <QVBoxLayout>
#include <QEvent>
#include <QPixmap>

MLoginWedget::MLoginWedget(QWidget *parent): QDialog(parent)
{
    setObjectName("MLoginWidget");
    setFixedSize(410,380);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    errtimer = new QTimer(this);
    errtimer->setSingleShot(true);
    connect(errtimer, &QTimer::timeout, this, [=](){
        m_errString->setVisible(false);
    });

    InitLoginUI();
    InitConnect();
}

MLoginWedget::~MLoginWedget()
{}

void MLoginWedget::setAuthService(MAuthService *service)
{
    m_authService = service;
}

bool MLoginWedget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_usrNameEdit){
        if(event->type() == QEvent::HoverEnter){
            m_usrIcon->setIcon(QIcon(":/res/login/u2.svg"));
        }else if(event->type() == QEvent::HoverLeave){
            m_usrIcon->setIcon(QIcon(":/res/login/u1.svg"));
        }
    }
    else if(obj == m_passwordEdit){
        if (event->type() == QEvent::HoverEnter)
            m_passwordIcon->setIcon(QIcon(":/res/login/pwd2.svg"));
        else if (event->type() == QEvent::HoverLeave)
            m_passwordIcon->setIcon(QIcon(":/res/login/pwd1.svg"));
    }
    return QDialog::eventFilter(obj, event);
}


void MLoginWedget::InitLoginUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addSpacing(20);

    m_closeBtn = new QPushButton(this);
    m_closeBtn->setIcon(QIcon(":/res/common/close.svg"));
    m_closeBtn->setFixedSize(30,30);
    m_closeBtn->setObjectName("CloseBtn");

    m_closeBtn->setParent(this);
    m_closeBtn->move(width() - 40, 10);

    m_icon = new QLabel(this);
    m_icon->setPixmap(QPixmap(":/res/login/logo.svg").scaled(48,48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_icon->setAlignment(Qt::AlignCenter);

    m_title = new QLabel(tr("生产系统"), this);
    m_title->setAlignment(Qt::AlignCenter);


    mainLayout->addWidget(m_icon);
    mainLayout->addWidget(m_title);

    mainLayout->addSpacing(20);

    m_usrName = new QLabel(tr("用户名："), this);
    m_usrNameEdit = new QLineEdit(this);
    m_usrNameEdit->setPlaceholderText(tr("输入用户名"));
    m_password = new QLabel(tr("密码："), this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText(tr("输入密码"));
    m_passwordEdit->setEchoMode(QLineEdit::Password);


    m_usrIcon = new QAction(QIcon(":/res/login/u1.svg"), "", m_usrNameEdit);
    m_usrNameEdit->addAction(m_usrIcon, QLineEdit::LeadingPosition);
    m_usrNameEdit->installEventFilter(this);

    m_passwordIcon = new QAction(QIcon(":/res/login/pwd1.svg"), "", m_passwordEdit);
    m_passwordEdit->addAction(m_passwordIcon, QLineEdit::LeadingPosition);
    m_passwordEdit->installEventFilter(this);

    m_usrNameEdit->setAttribute(Qt::WA_Hover);
    m_passwordEdit->setAttribute(Qt::WA_Hover);

    mainLayout->addWidget(m_usrName);
    mainLayout->addWidget(m_usrNameEdit);
    mainLayout->addWidget(m_password);
    mainLayout->addWidget(m_passwordEdit);

    mainLayout->addSpacing(10);

    m_errString = new QLabel(this);
    m_errString->setObjectName("LoginErrorLabel");
    m_errString->setAlignment(Qt::AlignCenter);
    m_errString->setVisible(false);

    mainLayout->addWidget(m_errString);
    mainLayout->addSpacing(10);

    m_loginBtn = new QPushButton(tr("登录"), this);
    m_loginBtn->setFixedHeight(36);
    m_loginBtn->setObjectName("LoginBtn");
    mainLayout->addWidget(m_loginBtn);

    mainLayout->setContentsMargins(40, 20, 40, 20);

}

void MLoginWedget::InitConnect()
{
    connect(m_loginBtn, &QPushButton::clicked,
            this, &MLoginWedget::onLoginBtnClicked);
    connect(m_closeBtn, &QPushButton::clicked,
            this, &MLoginWedget::onCloseBtnClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, m_loginBtn, &QPushButton::click);

    // 登录结果统一从 ApiService 返回；后续真实后台字段变化时优先修改 network/apiservice.cpp。
    connect(ApiService::instance(), &ApiService::loginFinished,
            this, &MLoginWedget::onApiLoginFinished);
}

void MLoginWedget::onLoginBtnClicked()
{
    m_pendingUserName = m_usrNameEdit->text().trimmed();
    m_pendingPassword = m_passwordEdit->text();

    if (m_pendingUserName.isEmpty())
    {
        showLoginError(tr("请输入用户名。"));
        return;
    }

    if (m_pendingPassword.isEmpty())
    {
        showLoginError(tr("请输入密码。"));
        return;
    }

    setLoginWaiting(true);
    ApiService::instance()->login(m_pendingUserName, m_pendingPassword);
}

void MLoginWedget::onApiLoginFinished(bool success, const QString& message, bool networkError)
{
    setLoginWaiting(false);

    if (success)
    {
        accept();
        return;
    }

    // 临时兼容旧登录：只有网络错误时才回退本地校验，避免后台未接通阶段软件无法进入。
    // 后台登录接口稳定后，可以删除下面这段 m_authService 兜底逻辑。
    if (networkError && m_authService)
    {
        QString errorMsg;
        const bool ok = m_authService->Login(m_pendingUserName, m_pendingPassword, errorMsg);
        if (ok)
        {
            accept();
            return;
        }

        showLoginError(errorMsg);
        return;
    }

    showLoginError(message.isEmpty() ? tr("登录失败，请重试。") : message);
}

void MLoginWedget::setLoginWaiting(bool waiting)
{
    m_loginBtn->setEnabled(!waiting);
    m_usrNameEdit->setEnabled(!waiting);
    m_passwordEdit->setEnabled(!waiting);
    m_loginBtn->setText(waiting ? tr("登录中...") : tr("登录"));
}

void MLoginWedget::showLoginError(const QString& message)
{
    m_errString->setText(message);
    m_errString->setVisible(true);
    errtimer->start(3000);
}

void MLoginWedget::onCloseBtnClicked()
{
    reject();
}

void MLoginWedget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = true;
        m_dragPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
}

void MLoginWedget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton))
    {
        move(event->globalPosition().toPoint() - m_dragPos);
    }
}

void MLoginWedget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_dragging = false;
}

void MLoginWedget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        m_loginBtn->click();
        return;
    }
    if(event->key() == Qt::Key_Escape){
        m_closeBtn->click();
        return;
    }
    QDialog::keyPressEvent(event);
}
