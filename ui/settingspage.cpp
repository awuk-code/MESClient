#include "settingspage.h"

#include "searchconfig.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QStringList>
#include <QVBoxLayout>

SettingsPage::SettingsPage(QWidget* parent)
    : QDialog(parent)
{
    initUI();
}

void SettingsPage::initUI()
{
    setWindowTitle(tr("搜索配置"));
    resize(760, 620);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);
    mainLayout->addWidget(createSearchConfigPage(), 1);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();

    auto saveButton = new QPushButton(tr("保存"), this);
saveButton->setProperty("buttonRole", "save");
    buttonLayout->addWidget(saveButton);

    mainLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, &SettingsPage::saveSettings);

}

QWidget* SettingsPage::createSearchConfigPage()
{
    auto page = new QWidget(this);
    auto layout = new QVBoxLayout(page);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    auto hint = new QLabel(
                tr("勾选后的字段会加入页面顶部搜索框的关键字过滤范围。"),
                page);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    auto scroll = new QScrollArea(page);
    scroll->setWidgetResizable(true);

    auto content = new QWidget(scroll);
    auto contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(12);
    contentLayout->addWidget(createModuleGroup(tr("生产任务"), SearchConfig::productionTaskModule()));
    contentLayout->addWidget(createModuleGroup(tr("工序站点"), SearchConfig::processStationModule()));
    contentLayout->addWidget(createModuleGroup(tr("维修站"), SearchConfig::repairStationModule()));
    contentLayout->addStretch();

    scroll->setWidget(content);
    layout->addWidget(scroll, 1);

    return page;
}

QWidget* SettingsPage::createModuleGroup(const QString& title, const QString& module)
{
    auto group = new QGroupBox(title, this);
    auto layout = new QGridLayout(group);
    layout->setContentsMargins(12, 18, 12, 12);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(8);

    const QStringList selected = SearchConfig::selectedFields(module);
    const QVector<SearchFieldItem> fields = SearchConfig::availableFields(module);

    QList<QCheckBox*> checks;
    for (int i = 0; i < fields.size(); ++i)
    {
        const SearchFieldItem item = fields[i];
        auto check = new QCheckBox(item.title, group);
        check->setProperty("field", item.field);
        check->setChecked(selected.contains(item.field));
        layout->addWidget(check, i / 3, i % 3);
        checks.append(check);
    }

    m_fieldChecks.insert(module, checks);
    return group;
}

void SettingsPage::saveSettings()
{
    for (auto it = m_fieldChecks.constBegin(); it != m_fieldChecks.constEnd(); ++it)
    {
        QStringList fields;
        for (QCheckBox* check : it.value())
        {
            if (check && check->isChecked())
                fields.append(check->property("field").toString());
        }
        SearchConfig::setSelectedFields(it.key(), fields);
    }

    QMessageBox::information(
                this,
                tr("搜索配置"),
                tr("搜索配置已保存，重新打开页面或重启客户端后生效。"));

    accept();
}
