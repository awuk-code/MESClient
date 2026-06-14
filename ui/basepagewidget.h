#ifndef BASEPAGEWIDGET_H
#define BASEPAGEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QSet>
#include <QVariantMap>

#include "commonfunc.h"

class BaseTableModel;
class FieldFilterProxyModel;
class PaginationProxyModel;
class QAbstractItemModel;
class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTabBar;
class QTableView;
class QEvent;


class BasePageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BasePageWidget(QWidget *parent = nullptr);

public:
    void setupPage();

signals:
    void sigPageSwitching(const QVariantMap& rowData,
                          const QString& pageID);
    void sigIMGView(const QString& NGNumber);

protected slots:
    void onPageLinkClicked(const QVariantMap& rowData,
                          const  QString& pageID);
    void onImageLinkClicked(const QString& NGNumber);
    void onSearchBtnClicked();
    void onExportBtnClicked();
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

    void updateTableResizeMode();
protected:
    // 创建基础数据模型（通常继承自 BaseTableModel）。
    virtual QAbstractItemModel* createModel() = 0;

    virtual TabConfigs  Tabs() const;
    virtual void setTabs(TabConfigs  tabs){ m_tabs = tabs;}

    virtual FieldFilterProxyModel* createProxy(const QVariant& data)=0;
    virtual QString pageTitle()const=0;
    virtual QString searchInfo()const=0;
    virtual void setPageTitle(const QString &title){m_pageTitle = title;}
    virtual void setSearchInfo(const QString &info){m_searchInfo = info;}

    virtual QHBoxLayout* createTitleLayout();
    virtual void addWidgetToTitle(QHBoxLayout* layout) = 0;
    virtual void setupSearchLayout(QHBoxLayout* layout);
    void setupPaginationLayout(QHBoxLayout* layout);

protected:
    // 创建界面控件和布局。
    void initUI();
    void updateSearchInfo();
    // 创建所有 Tabs 及其对应的表格和代理模型。
    void initTabs();

    // 建立内部信号连接。
    void initConnect();
    // 判断指定 Tab 下某列是否显示。子类可按状态控制不同表头。
    virtual bool isColumnVisibleForTab(
        const ColumnConfig& column,
        const QVariant& tabData) const;

    // 收集需要在表头显示筛选图标的字段
    QSet<QString> collectFilterFields(const QVariant& tabData) const;

    // 创建并初始化一个表格
    QTableView* createTable(FieldFilterProxyModel* proxy, const QVariant& tabData);

    // 设置表头筛选 Overlay
    void setupHeaderOverlay(
        QTableView* table,
        FieldFilterProxyModel* proxy,
        const QVariant& tabData);

    // 根据 BaseTableModel::columns() 设置列属性
    void setupColumns(QTableView* table, const QVariant& tabData);
    void applyAdaptiveColumnWidths(QTableView* table);

    // 设置表格通用属性
    void setupTableAppearance(QTableView* table);
    PaginationProxyModel* currentPaginationProxy() const;
    void refreshPaginationControls();
    void resetPaginationToFirstPage();


    // 绑定复选框点击时的整行选中逻辑
    void setupCheckBoxSelection(QTableView* table, const QVariant& tabData);
    int contentWidthForColumn(
        QTableView* table,
        const BaseTableModel* model,
        int column) const;
    void exportCurrentTableToExcel();
    void goToPaginationButtonPage(QPushButton* button);
    void scheduleColumnResize(QTableView* table);
    //--------------------------------
protected:
    QTabBar* tabBar()const{return m_tabBar;};

protected:
    QLineEdit* m_searchEdit{nullptr};
    QPushButton* m_searchBtn{nullptr};
    QPushButton* m_exportBtn{nullptr};
    QPushButton* m_prevPageBtn{nullptr};
    QPushButton* m_nextPageBtn{nullptr};
    QVector<QPushButton*> m_pageBtns;
    QWidget* m_searchWidget{nullptr};
    QHBoxLayout* m_searchLayout{nullptr};

    QTabBar* m_tabBar{nullptr};
    QStackedWidget* m_stack{nullptr};

    TabConfigs m_tabs;      //用来保存tabs
    // 保存每个 Tab 对应的页面控件，可能是 QTableView、PDF 页面或普通表单页面。
    QVector<QWidget*> m_pages;

    QAbstractItemModel* m_model;    //基础模型
    //每个tab对于的代理模型
    QVector<FieldFilterProxyModel*> m_proxies;
    QVector<PaginationProxyModel*> m_pageProxies;
    int m_pageSize{50};
    QString m_pageTitle, m_searchInfo;
};

#endif // BASEPAGEWIDGET_H
