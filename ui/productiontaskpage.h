#ifndef PRODUCTIONTASKPAGE_H
#define PRODUCTIONTASKPAGE_H

#include "basepagewidget.h"

class ProductionTaskPage : public BasePageWidget
{
public:
    explicit ProductionTaskPage(QWidget *parent = nullptr);
protected:
    QAbstractItemModel* createModel() override;

   TabConfigs tabs() const override;
    FieldFilterProxyModel* createProxy(const QVariant& data) override;
    QString pageTitle() const override;
    QString searchInfo()const override;
};

#endif // PRODUCTIONTASKPAGE_H
