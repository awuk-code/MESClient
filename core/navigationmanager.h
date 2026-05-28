#ifndef NAVIGATIONMANAGER_H
#define NAVIGATIONMANAGER_H

#include <QObject>
#include <QMap>
#include <QVariantMap>

#include "NavPageType.h"

class NavigationManager : public QObject
{
    Q_OBJECT
public:
    static NavigationManager* instance();

private:
    explicit NavigationManager(QObject *parent = nullptr);

public:
    void registerPage(PageType type,
        QWidget* page);
    QWidget* page(PageType type) const;
    void setPageData(PageType type, const QVariantMap& data);
    QVariantMap pageData(PageType type) const;

    bool contains(PageType type) const;

    void openPage(PageType type);
    void openPage(PageType type, const QVariantMap& data);

    PageType currentPage() const;
signals:
     void sigOpenPage(PageType type);

private:
     QMap<PageType, QWidget*> m_pageMap;
     QMap<PageType, QVariantMap> m_pageDataMap;

     PageType m_currentPage =
         PageType::Unknown;

};

#endif // NAVIGATIONMANAGER_H
