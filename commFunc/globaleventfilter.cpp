#include "globaleventfilter.h"
#include "qabstractbutton.h"
#include <QWidget>
#include <QEvent>

GlobalEventFilter::GlobalEventFilter(QObject *parent) : QObject(parent)
{

}

bool GlobalEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    QWidget* w = qobject_cast<QWidget*>(obj);
    if (!w) return QObject::eventFilter(obj, event);


    if (w->property("hoverIcon").isValid()
        && w->property("normalIcon").isValid())
    {
        QString normal = w->property("normalIcon").toString();
        QString hover  = w->property("hoverIcon").toString();
        QString checked = w->property("checkedIcon").toString();


        if (event->type() == QEvent::Enter )
        {
            if (auto btn = qobject_cast<QAbstractButton*>(w))
                btn->setIcon(QIcon(hover));
        }
        else if (event->type() == QEvent::Leave)
        {
            if (auto btn = qobject_cast<QAbstractButton*>(w))
                btn->setIcon(QIcon(normal));
        }
    }

    return QObject::eventFilter(obj, event);
}
