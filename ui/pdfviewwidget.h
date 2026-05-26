#ifndef PDFVIEWWIDGET_H
#define PDFVIEWWIDGET_H

#include <QWidget>
#include <QGraphicsPixmapItem>

class QPushButton;
class QLabel;
class QGraphicsView;
class QGraphicsScene;

namespace poppler
{
class document;
}

class PdfViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PdfViewWidget(QWidget *parent = nullptr);
    ~PdfViewWidget();

    bool loadPdf(const QString& filePath);

private:
    void initUI();

    void renderPage();

    void updatePageLabel();

    void fitPageToView();

private slots:
    void onPrevPage();

    void onNextPage();

    void onZoomIn();

    void onZoomOut();

protected:
    void resizeEvent(QResizeEvent* event)override;
private:
    // =========================
    // toolbar
    // =========================

    QPushButton* m_prevBtn = nullptr;

    QPushButton* m_nextBtn = nullptr;

    QPushButton* m_zoomInBtn = nullptr;

    QPushButton* m_zoomOutBtn = nullptr;

    QLabel* m_pageLabel = nullptr;

    // =========================
    // pdf view
    // =========================

    QGraphicsView* m_view = nullptr;

    QGraphicsScene* m_scene = nullptr;

    // =========================
    // pdf
    // =========================

    poppler::document* m_document = nullptr;

    int m_currentPage = 0;

    double m_scaleFactor = 1.0;

    bool m_autoFit = true;
    QGraphicsPixmapItem* m_pixmapItem{nullptr};
};

#endif // PDFVIEWWIDGET_H
