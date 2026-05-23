#ifndef NAVIGATIONMANAGER_H
#define NAVIGATIONMANAGER_H

#include <QObject>
#include <QMap>

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

    bool contains(PageType type) const;

    void openPage(PageType type);

    PageType currentPage() const;
signals:
     void sigOpenPage(PageType type);

private:
     QMap<PageType, QWidget*> m_pageMap;

     PageType m_currentPage =
         PageType::Unknown;

};

#endif // NAVIGATIONMANAGER_H
