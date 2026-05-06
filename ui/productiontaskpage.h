#ifndef PRODUCTIONTASKPAGE_H
#define PRODUCTIONTASKPAGE_H

#include "basepagewidget.h"

class ProductionTaskPage : public BasePageWidget
{
public:
    explicit ProductionTaskPage(QWidget *parent = nullptr);
protected:
    QAbstractItemModel* createModel() override;
    QStringList tabNames() const override;
    FieldFilterProxyModel* createProxy(int tabIndex) override;
    QString pageTitle() const override;
};

#endif // PRODUCTIONTASKPAGE_H
