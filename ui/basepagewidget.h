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
#include <QStandardItemModel>
#include <QResizeEvent>

#include "fieldfilterproxymodel.h"
#include "commonfunc.h"
// 通用业务页面基类。
//
// 主要功能：
// 1. 页面标题区
// 2. 查询区（关键字搜索 + 导出）
// 3. Tab 页签区
// 4. 内容区（当前默认使用 QTableView）
// 5. 表头筛选（日期、优先级等）
// 6. 最大化时按比例调整列宽
//
// 子类通常只需要实现：
// 1. createModel()      -> 创建数据模型
// 2. tabs()             -> 定义所有 Tab
// 3. createProxy()      -> 为每个 Tab 创建过滤代理模型
// 4. pageTitle()        -> 页面标题
// 5. searchInfo()       -> 搜索框提示文字
//
// 典型子类：
// - ProductionTaskPage
// - RepairStationPage
// - ProcessStationPage
// -
class BasePageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BasePageWidget(QWidget *parent = nullptr);

public:
    void setupPage();

protected:
    //
    void resizeEvent(QResizeEvent *event)override;
    //
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

protected:
    // 创建界面控件和布局。
    void initUI();
    void updateSearchInfo();
    // 创建所有 Tabs 及其对应的表格和代理模型。
    void initTabs();

    // 建立内部信号连接。
    void initConnect();
    // 收集需要在表头显示筛选图标的字段
    QSet<QString> collectFilterFields() const;

    // 创建并初始化一个表格
    QTableView* createTable(FieldFilterProxyModel* proxy);

    // 设置表头筛选 Overlay
    void setupHeaderOverlay(
        QTableView* table,
        FieldFilterProxyModel* proxy);

    // 根据 BaseTableModel::columns() 设置列属性
    void setupColumns(QTableView* table);

    // 设置表格通用属性
    void setupTableAppearance(QTableView* table);


    // 绑定复选框点击时的整行选中逻辑
    void setupCheckBoxSelection(QTableView* table);
    //--------------------------------
protected:
    QTabBar* tabBar()const{return m_tabBar;};

protected:
    QLineEdit* m_searchEdit{nullptr};
    QPushButton* m_searchBtn{nullptr};
    QPushButton* m_exportBtn{nullptr};

    QTabBar* m_tabBar{nullptr};
    QStackedWidget* m_stack{nullptr};

    TabConfigs m_tabs;      //用来保存tabs
    // 当前默认每个 Tab 对应一个 QTableView。
    // 后续可扩展为 QWidget*，支持 PDF、表单等。
    QVector<QWidget*> m_pages;
    QVector<QTableView*> m_tables;

    QAbstractItemModel* m_model;    //基础模型
    //每个tab对于的代理模型
    QVector<FieldFilterProxyModel*> m_proxies;
    QString m_pageTitle, m_searchInfo;
};

#endif // BASEPAGEWIDGET_H
