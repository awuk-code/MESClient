#include "navigationmanager.h"
#include <QWidget>

#include "MDebug.h"

NavigationManager *NavigationManager::instance()
{
    static NavigationManager manager;

    return &manager;
}
NavigationManager::NavigationManager(QObject *parent)
    : QObject{parent}
{}
void NavigationManager::registerPage(
    PageType type,
    QWidget *page)
{
    if (!page)
    {
        funcDebug()
        << "register page nullptr";

        return;
    }

    if (m_pageMap.contains(type))
    {
        funcDebug()
        << "page already registered";

        return;
    }

    m_pageMap.insert(type, page);

    funcDebug()
        << "register page success";
}

QWidget *NavigationManager::page(PageType type) const
{
    return m_pageMap.value(type, nullptr);
}

void NavigationManager::setPageData(PageType type, const QVariantMap &data)
{
    m_pageDataMap.insert(type, data);
    funcDebug() << "set page data";
}

QVariantMap NavigationManager::pageData(PageType type) const
{
    return m_pageDataMap.value(type);
}

bool NavigationManager::contains(PageType type) const
{
    return m_pageMap.contains(type);
}

void NavigationManager::openPage(PageType type)
{
    if (!m_pageMap.contains(type))
    {
        funcDebug()
        << "page not found";

        return;
    }

    if (m_currentPage == type)
    {
        return;
    }

    m_currentPage = type;

    emit sigOpenPage(type);

    funcDebug()
        << "open page";
}

void NavigationManager::openPage(PageType type, const QVariantMap &data)
{
    setPageData(type, data);

    if (m_currentPage == type)
    {
        emit sigOpenPage(type);
        funcDebug()
            << "refresh current page";
        return;
    }

    openPage(type);
}

PageType NavigationManager::currentPage() const
{
    return m_currentPage;
}
