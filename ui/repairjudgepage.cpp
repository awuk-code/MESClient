#include "repairjudgepage.h"

#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QScrollArea>
#include <QSizePolicy>
#include <QVBoxLayout>

RepairJudgePage::RepairJudgePage(QWidget *parent)
    : QWidget{parent}
{
    setObjectName("RepairJudgePage");
    initUI();
    rebuildInfo();
}

void RepairJudgePage::setRepairData(const QVariantMap& rowData)
{
    m_rowData = rowData;
    rebuildInfo();
}

void RepairJudgePage::initUI()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    auto titleLayout = new QHBoxLayout;
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(10);

    auto icon = new QLabel(this);
    icon->setPixmap(QPixmap(":/res/common/rect.svg").scaled(
        24,
        24,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));

    auto title = new QLabel("维修判定", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);

    m_saveBtn = new QPushButton("保存", this);
    m_backBtn = new QPushButton("返回", this);
    m_saveBtn->setFixedSize(84, 32);
    m_backBtn->setFixedSize(84, 32);

    titleLayout->addWidget(icon);
    titleLayout->addWidget(title);
    titleLayout->addStretch();
    titleLayout->addWidget(m_saveBtn);
    titleLayout->addWidget(m_backBtn);
    mainLayout->addLayout(titleLayout);

    auto infoGroup = new QGroupBox(this);
    infoGroup->setTitle("");
    infoGroup->setStyleSheet(
        "QGroupBox{"
        "background:#ffffff;"
        "border:1px solid #dcdfe6;"
        "margin-top:0;"
        "}");

    auto infoGroupLayout = new QVBoxLayout(infoGroup);
    infoGroupLayout->setContentsMargins(12, 12, 12, 12);
    infoGroupLayout->setSpacing(0);

    auto scrollArea = new QScrollArea(infoGroup);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setMinimumHeight(220);
    scrollArea->setMaximumHeight(300);

    m_infoWidget = new QWidget(scrollArea);
    m_infoLayout = new QGridLayout(m_infoWidget);
    m_infoLayout->setContentsMargins(0, 0, 0, 0);
    m_infoLayout->setHorizontalSpacing(14);
    m_infoLayout->setVerticalSpacing(10);
    m_infoLayout->setColumnStretch(1, 1);
    m_infoLayout->setColumnStretch(3, 1);

    scrollArea->setWidget(m_infoWidget);
    infoGroupLayout->addWidget(scrollArea);
    mainLayout->addWidget(infoGroup);

    m_contentWidget = new QFrame(this);
    m_contentWidget->setMinimumHeight(260);
    m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_contentWidget->setStyleSheet(
        "QFrame{"
        "background:#ffffff;"
        "border:1px solid #dcdfe6;"
        "}");
    mainLayout->addWidget(m_contentWidget, 1);
}

void RepairJudgePage::rebuildInfo()
{
    clearInfo();

    QVector<QPair<QString, QString>> fields;
    fields.append(QPair<QString, QString>("异常处理单号", "exceptionHandleNo"));

    const QString status = m_rowData.value("status").toString();
    if (status == "待返工")
        fields.append(QPair<QString, QString>("返工任务单号", "reworkTaskNo"));
    else
        fields.append(QPair<QString, QString>("生产任务单号", "taskNo"));

    fields.append(QPair<QString, QString>("生产工艺路线", "routeName"));
    fields.append(QPair<QString, QString>("产品型号", "productModel"));
    fields.append(QPair<QString, QString>("产品SN", "productSN"));
    fields.append(QPair<QString, QString>("异常数量", "abnormalCount"));
    fields.append(QPair<QString, QString>("异常类型", "abnormalType"));
    fields.append(QPair<QString, QString>("异常现象", "abnormalPhenomenon"));
    fields.append(QPair<QString, QString>("问题照片", "abnormalImage"));
    fields.append(QPair<QString, QString>("异常上报工序", "reportProcess"));
    fields.append(QPair<QString, QString>("异常上报时间", "reportTime"));
    fields.append(QPair<QString, QString>("产线编码", "lineNo"));
    fields.append(QPair<QString, QString>("工作站编号", "stationNo"));
    fields.append(QPair<QString, QString>("上报人员", "reporter"));

    if (status == "审核中" || status == "已处理")
        fields.append(QPair<QString, QString>("异常处理方式", "handleMethod"));

    for (int i = 0; i < fields.size(); ++i)
    {
        const int row = i / 2;
        const int labelColumn = (i % 2) * 2;
        const QString title = fields[i].first;
        const QString field = fields[i].second;

        m_infoLayout->addWidget(
            createTitleLabel(title),
            row,
            labelColumn);

        if (field == "abnormalImage")
        {
            m_infoLayout->addWidget(
                createImageButton(displayText(field)),
                row,
                labelColumn + 1);
        }
        else
        {
            m_infoLayout->addWidget(
                createValueLabel(displayText(field)),
                row,
                labelColumn + 1);
        }
    }
}

void RepairJudgePage::clearInfo()
{
    while (auto item = m_infoLayout->takeAt(0))
    {
        if (auto widget = item->widget())
            widget->deleteLater();

        delete item;
    }
}

QLabel* RepairJudgePage::createTitleLabel(const QString& text)
{
    auto label = new QLabel(text, m_infoWidget);
    label->setMinimumWidth(110);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setStyleSheet("QLabel{color:#606266;}");
    return label;
}

QLabel* RepairJudgePage::createValueLabel(const QString& text)
{
    auto label = new QLabel(text, m_infoWidget);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setWordWrap(true);
    label->setStyleSheet("QLabel{color:#303133;}");
    return label;
}

QPushButton* RepairJudgePage::createImageButton(const QString& text)
{
    auto button = new QPushButton(text, m_infoWidget);
    button->setCursor(Qt::PointingHandCursor);
    button->setFlat(true);
    button->setStyleSheet(
        "QPushButton{"
        "color:#409EFF;"
        "text-align:left;"
        "border:none;"
        "background:transparent;"
        "padding:0;"
        "}");
    return button;
}

QString RepairJudgePage::displayText(const QString& field) const
{
    const QString text = m_rowData.value(field).toString();
    return text.isEmpty() ? "-" : text;
}
