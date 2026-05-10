#include "headeroverlaywidget.h"
#include "basetablemodel.h"
#include "qabstractproxymodel.h"
#include "qpainter.h"

HeaderOverlayWidget::HeaderOverlayWidget(QHeaderView *header, QWidget *parent)
    : QWidget{parent}, m_header(header)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

raise();
}

void HeaderOverlayWidget::setFilterFields(const QSet<QString> &fields)
{
    m_fields = fields;
    update();
}

QRect HeaderOverlayWidget::calcIconRect(int column) const
{
    int sectionPos = m_header->sectionPosition(column);
    int sectionWidth = m_header->sectionSize(column);

    int iconSize = 14;

    return QRect(
        sectionPos + sectionWidth - iconSize - 6,
        6,
        iconSize,
        iconSize
        );
}

void HeaderOverlayWidget::showDatePopup(const QRect &iconRect)
{
    qDebug() << __FUNCTION__ <<"🔥 showDatePopup triggered";
    auto *popup = new DateFilterPopup();

    connect(popup, &DateFilterPopup::dateSelected,
            this, [=](const QDate& date) {
                qDebug() << "selected date:" << date;
                // TODO: 这里后面接 proxy filter
            });

    popup->adjustSize();

    // ⭐ 核心：贴着 icon
    QPoint globalPos = mapToGlobal(iconRect.bottomLeft());

    globalPos.setY(globalPos.y() + 5); // 微调

    QPoint pos = QCursor::pos();
    popup->move(pos);
    popup->show();
}


void HeaderOverlayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    auto model = m_header->model();
    auto baseModel = qobject_cast<BaseTableModel*>(model);
    if (!baseModel)
    {
        auto proxy = qobject_cast<QAbstractProxyModel*>(model);
        if (proxy)
            baseModel = qobject_cast<BaseTableModel*>(proxy->sourceModel());
        if (!baseModel)
            return;
    }

    const auto &columns = baseModel->columns();

    QIcon icon(":/res/common/filter.svg");
    if (icon.isNull())
        return;

    const int iconSize = 14;

    for (int i = 0; i < m_header->count(); ++i)
    {
        QString field = columns[i].field;

        if (!m_fields.contains(field))
            continue;

        QRect rect = m_header->sectionViewportPosition(i) >= 0
                         ? QRect(
                               m_header->sectionViewportPosition(i),
                               0,
                               m_header->sectionSize(i),
                               m_header->height())
                         : QRect();

        if (!rect.isValid())
            continue;

        QRect iconRect(
            rect.right() - iconSize - 6,
            rect.center().y() - iconSize / 2,
            iconSize,
            iconSize
            );

        painter.drawRect(iconRect);   // debug 红框
        icon.paint(&painter, iconRect);
    }
    m_iconRects.clear();

    for (int col : m_columnFields.keys())
    {
        QRect r = calcIconRect(col); // 你已有逻辑

        m_iconRects[col] = r;
    }
}

void HeaderOverlayWidget::mousePressEvent(QMouseEvent *event)
{
    for (auto it = m_iconRects.begin(); it != m_iconRects.end(); ++it)
    {
        int column = it.key();

        if (!it.value().contains(event->pos()))
            continue;

        QString field = m_columnFields.value(column);

        if (field == "startTime" || field == "finishTime")
        {
            showDatePopup(it.value());
            return;
        }
    }

    QWidget::mousePressEvent(event);
}
