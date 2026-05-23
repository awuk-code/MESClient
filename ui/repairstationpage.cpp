#include "repairstationpage.h"

RepairStationPage::RepairStationPage(QWidget *parent)
    : BasePageWidget(parent)
{

}

QAbstractItemModel *RepairStationPage::createModel()
{
    return {};
}

TabConfigs RepairStationPage::Tabs() const
{
    return {};
}

FieldFilterProxyModel *RepairStationPage::createProxy(const QVariant &data)
{
    return{};
}

QString RepairStationPage::pageTitle() const
{
  return "";
}

QString RepairStationPage::searchInfo() const
{
    return "";
}

void RepairStationPage::addWidgetToTitle(QHBoxLayout *layout)
{

}
