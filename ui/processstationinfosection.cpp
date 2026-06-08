#include "processstationinfosection.h"

#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QToolButton>
#include <QVBoxLayout>

ProcessStationInfoSection::ProcessStationInfoSection(const QString& title, QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    auto titleWidget = createTitleWidget(title);
    layout->addWidget(titleWidget);

    m_infoWidget = new QWidget(this);
    m_infoLayout = createInfoGrid(m_infoWidget);

    auto tableLayout = new QHBoxLayout;
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(0);
    tableLayout->addSpacing(28);
    tableLayout->addWidget(m_infoWidget);

    layout->addLayout(tableLayout);

    auto toggleBtn = titleWidget->findChild<QToolButton*>();
    if (toggleBtn)
    {
        connect(toggleBtn, &QToolButton::toggled,
                this,
                [=](bool checked) {
                    if (m_infoWidget)
                        m_infoWidget->setVisible(checked);

                    toggleBtn->setIcon(QIcon(checked
                                             ? ":/res/common/expand.svg"
                                             : ":/res/common/collapse.svg"));
                    toggleBtn->setToolTip(checked ? tr("收起") : tr("展开"));
                });
    }
}

void ProcessStationInfoSection::setTitle(const QString& title)
{
    if (m_titleLabel)
        m_titleLabel->setText(title);
}

void ProcessStationInfoSection::setFieldsData(
    const InfoFieldList& fields,
    const QVariantMap& rowData,
    int fieldPairsPerRow)
{
    if (!m_infoLayout)
        return;

    if (fieldPairsPerRow <= 0)
        fieldPairsPerRow = fields.size() <= 7 ? 1 : 2;
    fieldPairsPerRow = qMax(1, fieldPairsPerRow);

    while (auto item = m_infoLayout->takeAt(0))
    {
        if (auto widget = item->widget())
            widget->deleteLater();

        delete item;
    }

    for (int i = 0; i < fields.size(); ++i)
    {
        const int row = i / fieldPairsPerRow;
        const int labelColumn = (i % fieldPairsPerRow) * 2;
        const QString title = fields[i].first;
        const QString field = fields[i].second;
        const QString value = rowData.value(field).toString();

        m_infoLayout->addWidget(createInfoTitleLabel(title, m_infoWidget), row, labelColumn);
        m_infoLayout->addWidget(createInfoValueLabel(value, m_infoWidget), row, labelColumn + 1);
    }

    qDebug() << __FUNCTION__ << "fields:" << fields.size() << "data keys:" << rowData.keys();
}

QWidget* ProcessStationInfoSection::createTitleWidget(const QString& title)
{
    auto widget = new QWidget(this);
    auto titleLayout = new QHBoxLayout(widget);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    auto icon = new QLabel(widget);
    auto text = new QLabel(title, widget);
    icon->setPixmap(QPixmap(":res/common/rect.svg").scaled(
        24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    text->setProperty("labelRole", "sectionTitle");
    m_titleLabel = text;

    auto toggleBtn = new QToolButton(widget);
    toggleBtn->setCheckable(true);
    toggleBtn->setChecked(true);
    toggleBtn->setAutoRaise(true);
    toggleBtn->setCursor(Qt::PointingHandCursor);
    toggleBtn->setFixedSize(20, 20);
    toggleBtn->setIconSize(QSize(16, 16));
    toggleBtn->setIcon(QIcon(":/res/common/expand.svg"));
    toggleBtn->setToolTip(tr("收起"));
    toggleBtn->setProperty("buttonRole", "panelToggle");

    titleLayout->addWidget(icon);
    titleLayout->addWidget(text);
    titleLayout->addWidget(toggleBtn);
    titleLayout->addStretch();

    return widget;
}

QGridLayout* ProcessStationInfoSection::createInfoGrid(QWidget* parentWidget)
{
    auto grid = new QGridLayout(parentWidget);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(8);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(3, 1);
    return grid;
}

QLabel* ProcessStationInfoSection::createInfoTitleLabel(const QString& text, QWidget* parentWidget)
{
    auto label = new QLabel(text, parentWidget);
    label->setMinimumWidth(110);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setProperty("labelRole", "fieldName");
    return label;
}

QLabel* ProcessStationInfoSection::createInfoValueLabel(const QString& text, QWidget* parentWidget)
{
    auto label = new QLabel(text, parentWidget);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setWordWrap(true);
    label->setProperty("labelRole", "fieldValue");
    return label;
}
