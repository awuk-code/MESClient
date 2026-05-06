#ifndef BASEPAGEWIDGET_H
#define BASEPAGEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTabWidget>
#include <QTableView>
#include <QAbstractItemModel>
#include <QTableView>
#include <QHeaderView>
#include "fieldfilterproxymodel.h"

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
    virtual QStringList tabNames() const = 0;
    virtual FieldFilterProxyModel* createProxy(int tabIndex)=0;
    virtual QString pageTitle()const=0;
protected:
    void initUI();
    void initTabs();
    void bindSearch();

protected:
    QLabel* m_titleLabel{nullptr};
    QLineEdit* m_searchEdit{nullptr};
    QTabWidget* m_tabWidget{nullptr};

    QAbstractItemModel* m_model;
    QVector<FieldFilterProxyModel*> m_proxies;

};

#endif // BASEPAGEWIDGET_H
