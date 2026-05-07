#ifndef PRODUCTIONTASKPAGE_H
#define PRODUCTIONTASKPAGE_H

#include "basepagewidget.h"

class ProductionTaskPage : public BasePageWidget
{
public:
    explicit ProductionTaskPage(QWidget *parent = nullptr);
protected:
    QAbstractItemModel* createModel() override;

    QVector<TabConfig> tabs() const override;
    FieldFilterProxyModel* createProxy(int tabIndex) override;
    QString pageTitle() const override;
    QString searchInfo()const override;
};

#endif // PRODUCTIONTASKPAGE_H
