#include "repairstationpage.h"
#include "repairstationmodel.h"

RepairStationPage::RepairStationPage(QWidget *parent)
    : BasePageWidget(parent)
{
    setObjectName("RepairStationPage");
    setupPage();
    auto model = qobject_cast<RepairStationModel*>(m_model);
    if(model)
    {
        auto delegate = model->textLinkDelegate();
        if(delegate)
        {
            connect(delegate, &TextLinkDelegate::linkClicked,
                    this, [=](const QPersistentModelIndex& proxyIndex, const QString &text){
                        auto proxy =
                            qobject_cast<const FieldFilterProxyModel*>(
                                proxyIndex.model());

                        if (!proxy)
                            return;
                        // 映射回 source model
                        QModelIndex sourceIndex =
                            proxy->mapToSource(proxyIndex);
                        if (!sourceIndex.isValid())
                            return;

                        int row = sourceIndex.row();

                        QVariantMap rowData =
                            model->rowData(row);

                        QString NGnumber =
                            rowData.value("NGNumber").toString();

                        QString NGImg =
                            rowData.value("NGImg").toString();

                        if(text == NGnumber)
                        {
                            //跳转到维修判定页面
                            QString pageid("judge");
                            emit sigPageSwitching(rowData, pageid);
                        }else if(text == NGImg)
                        {
                            emit sigIMGView(row);
                        }
                    });
        }
    }
}

QAbstractItemModel *RepairStationPage::createModel()
{
    return new RepairStationModel(this);
}

TabConfigs RepairStationPage::Tabs() const
{
    return
        {
         {"待处理", PageDisplayType::TABLE, QVariant::fromValue(FilterStatus::PENDING)},
         {"审核中", PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::REVIEW)},
         {"待返工", PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::WAITRETURN)},
         {"已处理", PageDisplayType::TABLE,QVariant::fromValue(FilterStatus::PROCESSED)},
         };
}

FieldFilterProxyModel *RepairStationPage::createProxy(const QVariant &data)
{
    auto proxy = new FieldFilterProxyModel(this);
    proxy->setStatus(data); // 👉 Tab控制状态

    return proxy;
}

QString RepairStationPage::pageTitle() const
{
    return "维修站";
}

QString RepairStationPage::searchInfo() const
{
    return "请输入表单号";
}

void RepairStationPage::addWidgetToTitle(QHBoxLayout *layout)
{
    layout->addStretch();
}
