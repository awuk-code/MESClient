#include "processstationpasswidget.h"

#include <QAbstractItemView>
#include <QButtonGroup>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QStandardItemModel>
#include <QStringList>
#include <QTableView>
#include <QVBoxLayout>

ProcessStationPassWidget::ProcessStationPassWidget(const QString& title, QWidget* parent)
    : QWidget(parent)
{
    initUI(title);
    initConnect();
}

bool ProcessStationPassWidget::isScanInMode() const
{
    return m_scanInRadio && m_scanInRadio->isChecked();
}

bool ProcessStationPassWidget::isPassMode() const
{
    return m_passRadio && m_passRadio->isChecked();
}

bool ProcessStationPassWidget::isNgMode() const
{
    return m_ngRadio && m_ngRadio->isChecked();
}

QString ProcessStationPassWidget::scanText() const
{
    return m_scanEdit ? m_scanEdit->text().trimmed() : QString();
}

QTableView* ProcessStationPassWidget::statusTableView() const
{
    return m_statusTableView;
}

QStandardItemModel* ProcessStationPassWidget::statusModel() const
{
    return m_statusModel;
}

void ProcessStationPassWidget::initUI(const QString& title)
{
    auto layout = new QVBoxLayout(this);

    auto titleLayout = new QHBoxLayout;
    titleLayout->setContentsMargins(0, 0, 0, 0);

    auto icon = new QLabel(this);
    auto text = new QLabel(title, this);
    icon->setPixmap(QPixmap(":res/common/rect.svg").scaled(
        24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    text->setProperty("labelRole", "sectionTitle");

    titleLayout->addWidget(icon);
    titleLayout->addWidget(text);
    titleLayout->addStretch();
    layout->addLayout(titleLayout);

    auto contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->addSpacing(28);

    auto contentWidget = new QWidget(this);
    auto contentVBox = new QVBoxLayout(contentWidget);
    contentVBox->setContentsMargins(0, 0, 0, 0);
    contentVBox->setSpacing(8);

    {
        auto row = new QHBoxLayout;
        row->setContentsMargins(0, 0, 0, 0);

        auto label = new QLabel(tr("过站类别："), contentWidget);
        auto typeBtn = new QPushButton(tr("正常过站"), contentWidget);
        typeBtn->setProperty("buttonRole", "compact");

        row->addWidget(label);
        row->addWidget(typeBtn);
        row->addStretch();
        contentVBox->addLayout(row);
    }

    {
        auto row = new QHBoxLayout;
        row->setContentsMargins(0, 0, 0, 0);

        auto label = new QLabel(
            tr("<span style='color:#f56c6c;'>*</span>过站方式："),
            contentWidget);

        m_scanInRadio = new QRadioButton(tr("扫入"), contentWidget);
        m_passRadio = new QRadioButton(tr("PASS"), contentWidget);
        m_ngRadio = new QRadioButton(tr("NG"), contentWidget);

        auto passModeGroup = new QButtonGroup(contentWidget);
        passModeGroup->addButton(m_scanInRadio);
        passModeGroup->addButton(m_passRadio);
        passModeGroup->addButton(m_ngRadio);
        m_scanInRadio->setChecked(true);

        m_pauseBtn = new QPushButton(tr("暂停"), contentWidget);
        m_resumeBtn = new QPushButton(tr("解除暂停"), contentWidget);
        auto rollbackBtn = new QPushButton(tr("工序回退"), contentWidget);
        m_pauseBtn->setProperty("buttonRole", "compact");
        m_resumeBtn->setProperty("buttonRole", "compact");
        rollbackBtn->setProperty("buttonRole", "compact");

        row->addWidget(label);
        row->addWidget(m_scanInRadio);
        row->addWidget(m_passRadio);
        row->addWidget(m_ngRadio);
        row->addSpacing(12);
        row->addWidget(m_pauseBtn);
        row->addWidget(m_resumeBtn);
        row->addWidget(rollbackBtn);
        row->addStretch();
        contentVBox->addLayout(row);
    }

    {
        auto row = new QHBoxLayout;
        row->setContentsMargins(0, 0, 0, 0);

        auto label = new QLabel(
            tr("<span style='color:#f56c6c;'>*</span>扫码操作："),
            contentWidget);

        m_scanEdit = new QLineEdit(contentWidget);
        m_scanEdit->setPlaceholderText(tr("请扫描产品条码..."));

        m_executeBtn = new QPushButton(tr("执行"), contentWidget);
        m_executeBtn->setProperty("buttonRole", "compact");

        row->addWidget(label);
        row->addWidget(m_scanEdit);
        row->addWidget(m_executeBtn);
        row->addStretch();
        contentVBox->addLayout(row);
    }

    {
        auto row = new QHBoxLayout;
        row->setContentsMargins(0, 0, 0, 0);
        row->setSpacing(8);

        auto label = new QLabel(
            tr("<span style='color:#f56c6c;'>*</span>状态信息："),
            contentWidget);
        row->addWidget(label, 0, Qt::AlignTop);

        m_statusTableView = new QTableView(contentWidget);
        m_statusModel = new QStandardItemModel(0, 5, m_statusTableView);
        m_statusModel->setHorizontalHeaderLabels(QStringList()
                                                 << tr("序号")
                                                 << tr("产品SN")
                                                 << tr("PASS")
                                                 << tr("NG")
                                                 << tr("暂停"));
        m_statusTableView->setModel(m_statusModel);
        m_statusTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_statusTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_statusTableView->setSelectionMode(QAbstractItemView::SingleSelection);
        m_statusTableView->setAlternatingRowColors(true);
        m_statusTableView->verticalHeader()->hide();
        m_statusTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_statusTableView->setMinimumHeight(180);

        row->addWidget(m_statusTableView, 1);
        contentVBox->addLayout(row);
    }

    contentLayout->addWidget(contentWidget);
    layout->addLayout(contentLayout);
}

void ProcessStationPassWidget::initConnect()
{
    if (m_executeBtn)
        connect(m_executeBtn, &QPushButton::clicked,
                this, &ProcessStationPassWidget::executeRequested);

    if (m_scanEdit)
        connect(m_scanEdit, &QLineEdit::returnPressed,
                this, &ProcessStationPassWidget::executeRequested);

    if (m_pauseBtn)
        connect(m_pauseBtn, &QPushButton::clicked,
                this, &ProcessStationPassWidget::pauseRequested);

    if (m_resumeBtn)
        connect(m_resumeBtn, &QPushButton::clicked,
                this, &ProcessStationPassWidget::resumeRequested);
}
