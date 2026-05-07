#ifndef BASEPAGEWIDGET_H
#define BASEPAGEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTabBar>
#include <QStackedWidget>
#include <QTableView>
#include <QAbstractItemModel>
#include <QHeaderView>

#include "fieldfilterproxymodel.h"
#include "commonfunc.h"
#include "basetablemodel.h"

class BasePageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BasePageWidget(QWidget *parent = nullptr);

public:
    void setupPage();

protected:
    // 子类实现
    virtual QAbstractItemModel* createModel() = 0;

    virtual QVector<TabConfig> tabs() const=0;
    virtual FieldFilterProxyModel* createProxy(int tabIndex)=0;
    virtual QString pageTitle()const=0;
    virtual QString searchInfo()const=0;
    virtual void setPageTitle(const QString &title){m_pageTitle = title;}
    virtual void setSearchInfo(const QString &info){m_searchInfo = info;}
    virtual bool enableExport() const{return true;}
protected:
    void initUI();
    void initTabs();
    void initConnect();

protected:
    QLabel* m_titleIcon{nullptr};
    QLabel* m_titleLabel{nullptr};
    QLineEdit* m_searchEdit{nullptr};
    QPushButton* m_searchBtn{nullptr};
    QPushButton* m_exportBtn{nullptr};

    QTabBar* m_tabBar{nullptr};
    QStackedWidget* m_stack{nullptr};
    QVector<QTableView*> m_tables;

    QAbstractItemModel* m_model;
    QVector<FieldFilterProxyModel*> m_proxies;
    QString m_pageTitle, m_searchInfo;
};

#endif // BASEPAGEWIDGET_H
