#ifndef PRODUCTIONTASKPAGE_H
#define PRODUCTIONTASKPAGE_H

#include "basepagewidget.h"


class ProductionTaskPage : public BasePageWidget
{
public:
    explicit ProductionTaskPage(QWidget *parent = nullptr);
protected:
    QAbstractItemModel* createModel() override;

    TabConfigs Tabs() const override;
    FieldFilterProxyModel* createProxy(const QVariant& data) override;
    QString pageTitle() const override;
    QString searchInfo()const override;
    void addWidgetToTitle(QHBoxLayout* layout) override;

};

#endif // PRODUCTIONTASKPAGE_H
