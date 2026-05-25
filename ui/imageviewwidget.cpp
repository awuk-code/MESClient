#include "imageviewwidget.h"
#include "qscrollbar.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include <QVBoxLayout>

#include <QWheelEvent>
#include <QMouseEvent>

#include <QImageReader>
#include <QDebug>

ImageViewWidget::ImageViewWidget(QWidget *parent)
    : QWidget{parent}
{
     initUI();
}
void ImageViewWidget::initUI()
{
    auto layout =
        new QVBoxLayout(this);

    layout->setContentsMargins(0,0,0,0);

    m_scene =
        new QGraphicsScene(this);

    m_view =
        new QGraphicsView(m_scene, this);

    // 平滑渲染
    m_view->setRenderHints(
        QPainter::Antialiasing |
        QPainter::SmoothPixmapTransform);

    // 去边框
    m_view->setFrameShape(QFrame::NoFrame);

    // 背景
    m_view->setBackgroundBrush(QColor(30,30,30));

    // 关闭滚动条
    m_view->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);

    m_view->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);

    // 锚点
    m_view->setTransformationAnchor(
        QGraphicsView::AnchorUnderMouse);

    m_view->setResizeAnchor(
        QGraphicsView::AnchorUnderMouse);

    // 安装事件过滤器
    m_view->viewport()->installEventFilter(this);

    layout->addWidget(m_view);
}

bool ImageViewWidget::loadImage(
    const QString &filePath)
{
    QImageReader reader(filePath);

    reader.setAutoTransform(true);

    QImage image = reader.read();

    if (image.isNull())
    {
        qDebug() << "load image failed";

        return false;
    }

    clear();

    QPixmap pixmap =
        QPixmap::fromImage(image);

    m_pixmapItem =
        m_scene->addPixmap(pixmap);

    m_scene->setSceneRect(
        pixmap.rect());

    // 自适应铺满
    m_view->fitInView(
        m_scene->sceneRect(),
        Qt::KeepAspectRatio);

    m_scaleFactor = 1.0;

    return true;
}

void ImageViewWidget::clear()
{
    m_scene->clear();

    m_pixmapItem = nullptr;

    m_scaleFactor = 1.0;
}

bool ImageViewWidget::eventFilter(
    QObject *obj,
    QEvent *event)
{
    if (obj == m_view->viewport())
    {
        // =========================
        // 滚轮缩放
        // =========================

        if (event->type() == QEvent::Wheel)
        {
            auto wheelEvent =
                static_cast<QWheelEvent*>(event);

            constexpr double scaleStep = 1.15;

            if (wheelEvent->angleDelta().y() > 0)
            {
                m_view->scale(scaleStep, scaleStep);

                m_scaleFactor *= scaleStep;
            }
            else
            {
                m_view->scale(
                    1.0 / scaleStep,
                    1.0 / scaleStep);

                m_scaleFactor /= scaleStep;
            }

            return true;
        }

        // =========================
        // 鼠标按下
        // =========================

        else if (event->type() ==
                 QEvent::MouseButtonPress)
        {
            auto mouseEvent =
                static_cast<QMouseEvent*>(event);

            if (mouseEvent->button() ==
                Qt::LeftButton)
            {
                m_dragging = true;

                m_lastMousePos =
                    mouseEvent->pos();

                m_view->setCursor(
                    Qt::ClosedHandCursor);

                return true;
            }
        }

        // =========================
        // 鼠标移动
        // =========================

        else if (event->type() ==
                 QEvent::MouseMove)
        {
            if (m_dragging)
            {
                auto mouseEvent =
                    static_cast<QMouseEvent*>(event);

                QPoint delta =
                    mouseEvent->pos() -
                    m_lastMousePos;

                m_lastMousePos =
                    mouseEvent->pos();

                m_view->horizontalScrollBar()
                    ->setValue(
                        m_view->horizontalScrollBar()
                            ->value() - delta.x());

                m_view->verticalScrollBar()
                    ->setValue(
                        m_view->verticalScrollBar()
                            ->value() - delta.y());

                return true;
            }
        }

        // =========================
        // 鼠标释放
        // =========================

        else if (event->type() ==
                 QEvent::MouseButtonRelease)
        {
            auto mouseEvent =
                static_cast<QMouseEvent*>(event);

            if (mouseEvent->button() ==
                Qt::LeftButton)
            {
                m_dragging = false;

                m_view->setCursor(
                    Qt::ArrowCursor);

                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}
