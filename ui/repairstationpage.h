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
    void setupSearchLayout(QHBoxLayout* layout) override;

protected:
    bool isColumnVisibleForTab(
        const ColumnConfig& column,
        const QVariant& tabData) const override;

private:
    void updateRepairJudgeButton();
    QVariantMap rowDataFromProxyIndex(const QModelIndex& proxyIndex) const;
    void openRepairJudgePage(const QVariantMap& rowData);

private:
    QPushButton* m_repairJudgeBtn{nullptr};
};

#endif // REPAIRSTATIONPAGE_H
