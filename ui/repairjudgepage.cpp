#include "repairjudgepage.h"
#include "basedialogwidget.h"
#include "imageviewwidget.h"
#include "navigationmanager.h"

#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFormLayout>
#include <QFrame>
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QPixmap>
#include <QRadioButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStandardPaths>
#include <QTextEdit>
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

    auto title = new QLabel(tr("维修判定"), this);
    // 标题等级样式入口：页面一级标题，后续修改字体时在 QSS 的 pageTitle 中统一设置。
    title->setProperty("labelRole", "pageTitle");

    m_saveBtn = new QPushButton(tr("保存"), this);
    m_backBtn = new QPushButton(tr("返回"), this);
    m_saveBtn->setProperty("buttonRole", "save");
    m_backBtn->setProperty("buttonRole", "secondary");
    m_saveBtn->setFixedSize(84, 32);
    m_backBtn->setFixedSize(84, 32);

    titleLayout->addWidget(icon);
    titleLayout->addWidget(title);
    titleLayout->addStretch();
    titleLayout->addWidget(m_saveBtn);
    titleLayout->addWidget(m_backBtn);
    mainLayout->addLayout(titleLayout);

    auto contentFrame = new QFrame(this);
    contentFrame->setProperty("panelRole", "pageContent");
    contentFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto contentLayout = new QVBoxLayout(contentFrame);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(12);

    auto infoGroup = new QFrame(contentFrame);
    infoGroup->setProperty("panelRole", "section");

    auto infoGroupLayout = new QVBoxLayout(infoGroup);
    infoGroupLayout->setContentsMargins(0, 0, 0, 0);
    infoGroupLayout->setSpacing(0);

    auto infoTitle = new QLabel(tr("异常品信息"), infoGroup);
    infoTitle->setFixedHeight(36);
    infoTitle->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    // 标题等级样式入口：左侧/内容区二级标题，后续修改字体时在 QSS 的 sectionTitle 中统一设置。
    infoTitle->setProperty("labelRole", "sectionTitle");
    infoGroupLayout->addWidget(infoTitle);

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
    contentLayout->addWidget(infoGroup);

    m_contentWidget = new QFrame(contentFrame);
    m_contentWidget->setObjectName("judgePanel");
    m_contentWidget->setProperty("panelRole", "section");
    m_contentWidget->setMinimumHeight(260);
    m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto judgeLayout = new QVBoxLayout(m_contentWidget);
    judgeLayout->setContentsMargins(0, 0, 0, 0);
    judgeLayout->setSpacing(0);

    auto judgeTitle = new QLabel(tr("情况判定"), m_contentWidget);
    judgeTitle->setFixedHeight(36);
    judgeTitle->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    // 标题等级样式入口：左侧/内容区二级标题，后续修改字体时在 QSS 的 sectionTitle 中统一设置。
    judgeTitle->setProperty("labelRole", "sectionTitle");
    judgeLayout->addWidget(judgeTitle);

    auto judgeContent = new QWidget(m_contentWidget);
    auto judgeContentLayout = new QVBoxLayout(judgeContent);
    judgeContentLayout->setContentsMargins(12, 12, 12, 12);
    judgeContentLayout->setSpacing(12);

    auto methodLayout = new QHBoxLayout;
    methodLayout->setContentsMargins(0, 0, 0, 0);
    methodLayout->setSpacing(18);

    auto methodLabel = new QLabel(tr("异常处理方式："), judgeContent);
    methodLayout->addWidget(methodLabel);

    m_methodGroup = new QButtonGroup(this);
    const QStringList methods = {
        tr("让步"),
        tr("误判"),
        tr("返工"),
        tr("报废")
    };

    for (int i = 0; i < methods.size(); ++i)
    {
        auto radio = new QRadioButton(methods[i], judgeContent);
        radio->setChecked(i == 0);
        m_methodGroup->addButton(radio, i);
        methodLayout->addWidget(radio);
    }

    methodLayout->addStretch();
    judgeContentLayout->addLayout(methodLayout);

    m_reworkOptionsWidget = new QWidget(judgeContent);
    auto reworkLayout = new QHBoxLayout(m_reworkOptionsWidget);
    reworkLayout->setContentsMargins(0, 0, 0, 0);
    reworkLayout->setSpacing(12);

    m_reworkRouteCombo = new QComboBox(m_reworkOptionsWidget);
    m_reworkRouteCombo->setMinimumWidth(180);
    m_reworkRouteCombo->addItems({
        tr("返工工艺路线A"),
        tr("返工工艺路线B")
    });

    m_reworkFileCombo = new QComboBox(m_reworkOptionsWidget);
    m_reworkFileCombo->setMinimumWidth(180);
    m_reworkFileCombo->addItems({
        tr("返工工艺文件A"),
        tr("返工工艺文件B")
    });

    reworkLayout->addWidget(new QLabel(tr("返工工艺路线："), m_reworkOptionsWidget));
    reworkLayout->addWidget(m_reworkRouteCombo);
    reworkLayout->addWidget(new QLabel(tr("返工工艺文件："), m_reworkOptionsWidget));
    reworkLayout->addWidget(m_reworkFileCombo);
    reworkLayout->addStretch();
    judgeContentLayout->addWidget(m_reworkOptionsWidget);
    updateReworkOptionsVisible();

    m_judgeTextEdit = new QTextEdit(judgeContent);
    m_judgeTextEdit->setMinimumHeight(130);
    judgeContentLayout->addWidget(m_judgeTextEdit, 1);

    auto submitLayout = new QHBoxLayout;
    submitLayout->setContentsMargins(0, 0, 0, 0);
    submitLayout->addStretch();

    m_submitBtn = new QPushButton(tr("提交"), judgeContent);
    m_submitBtn->setProperty("buttonRole", "primary");
    m_submitBtn->setMinimumSize(84, 32);
    m_repairReworkBtn = new QPushButton(tr("维修返工"), judgeContent);
    m_repairReworkBtn->setProperty("buttonRole", "primary");
    m_repairReworkBtn->setMinimumSize(96, 32);
    m_productionReworkBtn = new QPushButton(tr("生产返工"), judgeContent);
    m_productionReworkBtn->setProperty("buttonRole", "primary");
    m_productionReworkBtn->setMinimumSize(96, 32);
    submitLayout->addWidget(m_submitBtn);
    submitLayout->addWidget(m_repairReworkBtn);
    submitLayout->addWidget(m_productionReworkBtn);
    judgeContentLayout->addLayout(submitLayout);

    judgeLayout->addWidget(judgeContent, 1);
    contentLayout->addWidget(m_contentWidget, 1);
    mainLayout->addWidget(contentFrame, 1);

    connect(m_saveBtn, &QPushButton::clicked,
            this, &RepairJudgePage::onSaveBtnClicked);
    connect(m_backBtn, &QPushButton::clicked,
            this, &RepairJudgePage::onBackBtnClicked);
    connect(m_submitBtn, &QPushButton::clicked,
            this, &RepairJudgePage::onSubmitBtnClicked);
    connect(m_repairReworkBtn, &QPushButton::clicked,
            this, [this]() { submitJudge(tr("维修返工")); });
    connect(m_productionReworkBtn, &QPushButton::clicked,
            this, [this]() { submitJudge(tr("生产返工")); });

    connect(m_methodGroup,
            static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this,
            [this](int) { updateReworkOptionsVisible(); });

    updateReworkOptionsVisible();
}

QVariantMap RepairJudgePage::judgeData(const QString& reworkType) const
{
    QVariantMap data = m_rowData;

    QString method;
    if (m_methodGroup && m_methodGroup->checkedButton())
        method = m_methodGroup->checkedButton()->text();

    data["judgeMethod"] = method;
    data["judgeContent"] =
        m_judgeTextEdit ? m_judgeTextEdit->toPlainText() : QString();
    data["reworkRoute"] =
        m_reworkRouteCombo ? m_reworkRouteCombo->currentText() : QString();
    data["reworkFile"] =
        m_reworkFileCombo ? m_reworkFileCombo->currentText() : QString();
    data["reworkType"] = reworkType;

    return data;
}

void RepairJudgePage::onSaveBtnClicked()
{
    const QVariantMap data = judgeData();
    const QString dirPath =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dirPath);
    if (!dir.exists())
        dir.mkpath(".");

    const QString key =
        data.value("exceptionHandleNo").toString().isEmpty()
            ? QStringLiteral("unknown")
            : data.value("exceptionHandleNo").toString();
    const QString filePath =
        dir.filePath(QStringLiteral("repair_judge_%1.json").arg(key));

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("保存"), tr("维修判定临时文件保存失败。"));
        return;
    }

    QJsonObject object = QJsonObject::fromVariantMap(data);
    file.write(QJsonDocument(object).toJson(QJsonDocument::Indented));
    file.close();

    // 当前先保存到本地临时文件；后续如果改为接口草稿保存，只需要替换这里的写文件逻辑。
    qDebug() << __FUNCTION__ << "draft saved:" << filePath;
    showInfoDialog(tr("保存"), tr("维修判定内容已临时保存。"));
}

void RepairJudgePage::onSubmitBtnClicked()
{
    submitJudge();
}

void RepairJudgePage::submitJudge(const QString& reworkType)
{
    const QVariantMap data = judgeData();
    const QString content =
        data.value("judgeContent").toString().trimmed();
    if (content.isEmpty())
    {
        showInfoDialog(tr("提交校验"), tr("请先填写说明记录，再提交维修判定。"));
        return;
    }

    const bool isRework =
        m_methodGroup && m_methodGroup->checkedId() == 2;
    QString message;
    if (isRework)
    {
        const QString selectedReworkType =
            reworkType.isEmpty() ? tr("维修返工") : reworkType;
        const QVariantMap reworkData = judgeData(selectedReworkType);
        message = tr("已选择“返工”。\n\n返工工艺路线：%1\n返工工艺文件：%2\n返工类型：%3\n\n按需求，提交后应进入审核流程；当前提交接口尚未接入，本次仅保留调试输出。")
            .arg(reworkData.value("reworkRoute").toString(),
                 reworkData.value("reworkFile").toString(),
                 reworkData.value("reworkType").toString());
    }
    else
    {
        message = tr("已提交“%1”判定。\n\n按需求，提交后应进入审核流程；当前提交接口尚未接入，本次仅保留调试输出。")
            .arg(data.value("judgeMethod").toString());
    }

    // TODO: 提交按钮后续在这里调用上传接口，将 judgeMethod 和 judgeContent 一并提交给后端。
    qDebug() << __FUNCTION__
             << "wait upload, exceptionHandleNo ="
             << data.value("exceptionHandleNo").toString()
             << "method =" << data.value("judgeMethod").toString()
             << "content =" << data.value("judgeContent").toString()
             << "reworkType =" << reworkType;

    showInfoDialog(tr("提交"), message);
}

void RepairJudgePage::onBackBtnClicked()
{
    // 返回按钮只负责回到维修站页面，不清空当前内容，方便用户再次进入时继续查看。
    NavigationManager::instance()->openPage(PageType::RepairStation);
}

void RepairJudgePage::updateReworkOptionsVisible()
{
    if (!m_reworkOptionsWidget || !m_methodGroup)
        return;

    const bool isRework = m_methodGroup->checkedId() == 2;
    m_reworkOptionsWidget->setVisible(isRework);

    if (m_submitBtn)
        m_submitBtn->setVisible(!isRework);
    if (m_repairReworkBtn)
        m_repairReworkBtn->setVisible(isRework);
    if (m_productionReworkBtn)
        m_productionReworkBtn->setVisible(isRework);
}

void RepairJudgePage::showInfoDialog(const QString& title, const QString& message)
{
    BaseDialogWidget dialog(this);
    dialog.setTitle(title);
    dialog.cancelButton()->hide();
    dialog.confirmButton()->setText(tr("确定"));

    auto label = new QLabel(message, &dialog);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setWordWrap(true);
    dialog.contentLayout()->addWidget(label);

    dialog.exec();
}

void RepairJudgePage::rebuildInfo()
{
    clearInfo();

    QVector<QPair<QString, QString>> fields;
    fields.append(QPair<QString, QString>(tr("异常处理单号"), "exceptionHandleNo"));

    const QString status = m_rowData.value("status").toString();
    if (status == tr("待返工"))
        fields.append(QPair<QString, QString>(tr("返工任务单号"), "reworkTaskNo"));
    else
        fields.append(QPair<QString, QString>(tr("生产任务单号"), "taskNo"));

    fields.append(QPair<QString, QString>(tr("生产工艺路线"), "routeName"));
    fields.append(QPair<QString, QString>(tr("产品型号"), "productModel"));
    fields.append(QPair<QString, QString>(tr("产品SN"), "productSN"));
    fields.append(QPair<QString, QString>(tr("异常数量"), "abnormalCount"));
    fields.append(QPair<QString, QString>(tr("异常类型"), "abnormalType"));
    fields.append(QPair<QString, QString>(tr("异常现象"), "abnormalPhenomenon"));
    fields.append(QPair<QString, QString>(tr("问题照片"), "abnormalImage"));
    fields.append(QPair<QString, QString>(tr("异常上报工序"), "reportProcess"));
    fields.append(QPair<QString, QString>(tr("异常上报时间"), "reportTime"));
    fields.append(QPair<QString, QString>(tr("产线编码"), "lineNo"));
    fields.append(QPair<QString, QString>(tr("工作站编号"), "stationNo"));
    fields.append(QPair<QString, QString>(tr("上报人员"), "reporter"));

    if (status == tr("审核中") || status == tr("已处理"))
        fields.append(QPair<QString, QString>(tr("异常处理方式"), "handleMethod"));

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
    label->setProperty("labelRole", "fieldName");
    return label;
}

QLabel* RepairJudgePage::createValueLabel(const QString& text)
{
    auto label = new QLabel(text, m_infoWidget);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setWordWrap(true);
    label->setProperty("labelRole", "fieldValue");
    return label;
}

QPushButton* RepairJudgePage::createImageButton(const QString& text, QWidget* parent)
{
    auto button = new QPushButton(text, parent ? parent : m_infoWidget);
    button->setCursor(Qt::PointingHandCursor);
    button->setFlat(true);
    button->setProperty("buttonRole", "link");
    connect(button, &QPushButton::clicked, this, [this]() {
        const QString imagePath =
            m_rowData.value("abnormalImage").toString();
        const QString exceptionNo =
            m_rowData.value("exceptionHandleNo").toString();

        if (QFile::exists(imagePath))
        {
            auto viewer = new ImageViewWidget;
            viewer->resize(1200, 800);
            viewer->show();
            viewer->loadImage(imagePath);
            return;
        }

        showInfoDialog(
            tr("异常图片"),
            tr("异常单号：%1\n\n当前异常图片尚未接入可预览的本地或后台地址。")
                .arg(exceptionNo.isEmpty() ? tr("-") : exceptionNo));
    });
    return button;
}

QString RepairJudgePage::displayText(const QString& field) const
{
    const QString text = m_rowData.value(field).toString();
    return text.isEmpty() ? "-" : text;
}
