#include "headeroverlaywidget.h"
#include "basetablemodel.h"
#include "qabstractproxymodel.h"
#include "qmenu.h"
#include "qpainter.h"

HeaderOverlayWidget::HeaderOverlayWidget(QHeaderView *header, QWidget *parent)
    : QWidget{parent}, m_header(header)
{
    //  setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

    raise();
}

void HeaderOverlayWidget::setFilterFields(const QSet<QString> &fields)
{
    // 1. 保存需要显示图标的字段集合
    m_fields = fields;

    // 2. 清空旧的列映射
    m_columnFields.clear();

    // 3. 获取当前模型
    QAbstractItemModel *model = m_header->model();
    if (!model)
    {
        update();
        return;
    }

    // 4. 找到 BaseTableModel
    BaseTableModel *baseModel = qobject_cast<BaseTableModel*>(model);
    // 5. 如果 Header 使用的是代理模型（FieldFilterProxyModel），
    //    则继续从 sourceModel() 中获取 BaseTableModel
    if (!baseModel)
    {
        QAbstractProxyModel *proxy =
            qobject_cast<QAbstractProxyModel*>(model);

        if (proxy)
            baseModel =
                qobject_cast<BaseTableModel*>(proxy->sourceModel());
    }
    // 6. 如果最终仍然没有拿到 BaseTableModel，
    //    则无法获取 columns() 配置，直接退出
    if (!baseModel)
    {
        update();
        qDebug() << __FUNCTION__ <<"无法获取 columns() 配置，直接退出";
        return;
    }

    // 7. 获取列配置列表
    //    每个 ColumnConfig 中都包含：
    //    - title
    //    - field
    //    - width
    //    - visible
    //    - resizeMode
    const QVector<ColumnConfig> &columns = baseModel->columns();
    // 8. 遍历所有列，找出需要显示筛选图标的字段
    for (int i = 0; i < columns.size(); ++i)
    {
        const QString &field = columns[i].field;

        if (m_fields.contains(field))
        {
            m_columnFields[i] = field;

            qDebug() << "filter field:"
                     << field
                     << "column =" << i;
        }
    }

    // . 触发重绘
    update();
}

QRect HeaderOverlayWidget::calcIconRect(int column) const
{
    // 列被隐藏
    if (m_header->isSectionHidden(column))
        return QRect();

    // 获取列在 viewport 中的位置
    int sectionPos = m_header->sectionViewportPosition(column);

    // 不可见时返回无效矩形
    if (sectionPos < 0)
        return QRect();

    // 获取列宽
    int sectionWidth = m_header->sectionSize(column);

    // 图标参数
    const int iconSize = 14;
    const int rightMargin = 6;

    // 图标放在列右侧，垂直居中
    return QRect(
        sectionPos + sectionWidth - iconSize - rightMargin,
        (height() - iconSize) / 2,
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
                emit filterSelected(m_currentField, date);
            });

    popup->adjustSize();

    // ⭐ 核心：贴着 icon
    QPoint globalPos = mapToGlobal(iconRect.bottomLeft());

    globalPos.setY(globalPos.y() + 5); // 微调

    // QPoint pos = QCursor::pos();
    // popup->move(pos);
    popup->move(globalPos);
    popup->show();
}

void HeaderOverlayWidget::showPriorityPopup(const QRect &iconRect)
{
    qDebug() << __FUNCTION__ << "show priority popup";

    QMenu *menu = new QMenu(this);

    QAction *highAction   = menu->addAction(QStringLiteral("高"));
    QAction *mediumAction = menu->addAction(QStringLiteral("中"));
    QAction *lowAction    = menu->addAction(QStringLiteral("低"));

    // 连接菜单选择信号
    connect(menu, &QMenu::triggered,
            this, [=](QAction *action)
            {
                QString priority = action->text();
                qDebug() << "selected priority:" << action->text();

                emit filterSelected(m_currentField, priority);
            });

    // 将图标区域坐标转换为全局坐标
    QPoint globalPos = mapToGlobal(iconRect.bottomLeft());
    globalPos.setY(globalPos.y() + 5);

    // 在图标下方弹出菜单
    menu->popup(globalPos);
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
    qDebug() << __FUNCTION__
             << "mouse clicked, pos =" << event->pos();

    for (auto it = m_columnFields.begin(); it != m_columnFields.end(); ++it)
    {
        int column = it.key();
        QString field = it.value();

        // 实时计算当前图标区域
        QRect iconRect = calcIconRect(column);

        qDebug() << "check column =" << column
                 << ", field =" << field
                 << ", iconRect =" << iconRect;

        // 无效区域直接跳过
        if (!iconRect.isValid() || iconRect.isEmpty())
            continue;

        // 判断点击是否落在图标区域
        if (!iconRect.contains(event->pos()))
            continue;

        qDebug() << "✅ icon clicked!"
                 << "column =" << column
                 << "field =" << field;

        if (field == "startTime" || field == "finishTime")
        {
            qDebug() << "📅 date field clicked";
            m_currentField = field;
            showDatePopup(iconRect);
            return;
        }

        if(field == "priority"){
            qDebug() << "⭐ priority field clicked";
            m_currentField = field;
            showPriorityPopup(iconRect);
            return;
        }
        qDebug() << "🔍 normal filter field clicked";
        return;
    }

    qDebug() << "❌ click not inside any icon rect";

    QWidget::mousePressEvent(event);
}
