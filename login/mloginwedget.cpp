#include "mloginwedget.h"
#include <QVBoxLayout>
#include <QEvent>
#include <QPixmap>

MLoginWedget::MLoginWedget(QWidget *parent): QDialog(parent)
{
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
    m_closeBtn->setStyleSheet(R"(
    QPushButton {
        background-color: none;
        color: #008;
        border: none;
        font-size: 18px;
    }
    QPushButton:hover {
        background-color: none;
        color: #008;
    }
)");
    m_closeBtn->setParent(this);
    m_closeBtn->move(width() - 40, 10);

    m_icon = new QLabel(this);
    m_icon->setPixmap(QPixmap(":/res/login/logo.svg").scaled(48,48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_icon->setAlignment(Qt::AlignCenter);

    m_title = new QLabel(QStringLiteral("生产系统"), this);
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setStyleSheet("font-size:20px; font-weight:bold;");

    mainLayout->addWidget(m_icon);
    mainLayout->addWidget(m_title);

    mainLayout->addSpacing(20);

    m_usrName = new QLabel("用户名：", this);
    m_usrNameEdit = new QLineEdit(this);
    m_usrNameEdit->setPlaceholderText("输入用户名");
    m_password = new QLabel("密码：", this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("输入密码");
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
    m_errString->setStyleSheet("color:red;");
    m_errString->setAlignment(Qt::AlignCenter);
    m_errString->setVisible(false);

    mainLayout->addWidget(m_errString);
    mainLayout->addSpacing(10);

    m_loginBtn = new QPushButton("登录", this);
    m_loginBtn->setFixedHeight(36);
    mainLayout->addWidget(m_loginBtn);

    mainLayout->setContentsMargins(40, 20, 40, 20);

    setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #ccc;
            border-radius: 5px;
            padding: 6px;
        }
        QLineEdit:hover {
            border: 1px solid #409EFF;
        }
        QPushButton {
            background-color: #409EFF;
            color: white;
            border-radius: 5px;
        }
        QPushButton:hover {
            background-color: #66b1ff;
        }
    )");

}

void MLoginWedget::InitConnect()
{
    connect(m_loginBtn, &QPushButton::clicked, this, [=](){
        if(!m_authService){
            m_errString->setText("未连接服务器，请重试！");
            m_errString->setVisible(true);
            errtimer->start(3000);
            return;
        }

        QString errorMsg;
        bool ok = m_authService->Login(m_usrNameEdit->text(),
                                       m_passwordEdit->text(),
                                       errorMsg);
        if(ok){
            accept();
        }
        else{
            m_errString->setText(errorMsg);
            m_errString->setVisible(true);
            errtimer->start(3000);
        }
    });

    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_passwordEdit, &QLineEdit::returnPressed, m_loginBtn, &QPushButton::click);
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
