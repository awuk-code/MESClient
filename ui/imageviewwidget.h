#ifndef IMAGEVIEWWIDGET_H
#define IMAGEVIEWWIDGET_H

#include <QWidget>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;
class ImageViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageViewWidget(QWidget *parent = nullptr);
    bool loadImage(const QString& filePath);

    void clear();
private:
    void initUI();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QGraphicsView* m_view{nullptr};

    QGraphicsScene* m_scene{nullptr};

    QGraphicsPixmapItem* m_pixmapItem{nullptr};

    double m_scaleFactor{1.0};

    QPoint m_lastMousePos;

    bool m_dragging{false};
};

#endif // IMAGEVIEWWIDGET_H
