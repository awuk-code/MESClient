#include "toggleswitchwidget.h"

#include <QPainter>
#include <QMouseEvent>

ToggleSwitchWidget::ToggleSwitchWidget(QWidget *parent)
    : QWidget(parent)
{
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

bool ToggleSwitchWidget::isChecked() const
{
    return m_checked;
}

void ToggleSwitchWidget::setChecked(bool checked)
{
    if (m_checked == checked)
        return;

    m_checked = checked;
    update();

    emit toggled(m_checked);
}

void ToggleSwitchWidget::setSideLength(int size)
{
    if (size <= 0)
        return;

    m_sideLength = size;
    updateGeometry();
    update();
}

QSize ToggleSwitchWidget::sizeHint() const
{
    return QSize(m_sideLength, m_sideLength);
}

void ToggleSwitchWidget::enterEvent(QEvent *event)
{
    m_hovered = true;
    update();
    QWidget::enterEvent(event);
}

void ToggleSwitchWidget::leaveEvent(QEvent *event)
{
    m_hovered = false;
    update();
    QWidget::leaveEvent(event);
}

void ToggleSwitchWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setChecked(!m_checked);
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void ToggleSwitchWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QRect r = rect().adjusted(0, 0, -1, -1);

    int half = r.width() / 2;

    QRect leftRect(
        r.left(),
        r.top(),
        half,
        r.height());

    QRect rightRect(
        r.left() + half,
        r.top(),
        r.width() - half,
        r.height());

    QColor borderColor(180, 180, 180);
    QColor grayColor(220, 220, 220);
    QColor hoverColor(128, 90, 213);   // 紫色

    QColor leftColor;
    QColor rightColor;

    if (!m_checked)
    {
        // 默认：左灰右白
        leftColor = grayColor;
        rightColor = Qt::white;

        // 悬停时左侧变紫
        if (m_hovered)
            leftColor = hoverColor;

        setToolTip(tr("收缩左侧区域"));
    }
    else
    {
        // 切换后：左白右灰
        leftColor = Qt::white;
        rightColor = grayColor;

        // 悬停时右侧变紫
        if (m_hovered)
            rightColor = hoverColor;

        setToolTip(tr("展开左侧区域"));
    }

    painter.fillRect(leftRect, leftColor);
    painter.fillRect(rightRect, rightColor);

    // 中间分隔线
    painter.setPen(borderColor);
    painter.drawLine(leftRect.topRight(), leftRect.bottomRight());

    // 外边框
    painter.drawRect(r);
}
