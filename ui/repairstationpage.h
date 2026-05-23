#ifndef REPAIRSTATIONPAGE_H
#define REPAIRSTATIONPAGE_H

#include "basepagewidget.h"

class RepairStationPage : public BasePageWidget
{
    Q_OBJECT
public:
   explicit RepairStationPage(QWidget* parent = nullptr);
    QAbstractItemModel* createModel() override;

    TabConfigs Tabs() const override;
    FieldFilterProxyModel* createProxy(const QVariant& data) override;
    QString pageTitle() const override;
    QString searchInfo()const override;
    void addWidgetToTitle(QHBoxLayout* layout) override;
};

#endif // REPAIRSTATIONPAGE_H
