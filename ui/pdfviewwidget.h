#ifndef PDFVIEWWIDGET_H
#define PDFVIEWWIDGET_H

#include <QWidget>

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

private slots:
    void onPrevPage();

    void onNextPage();

    void onZoomIn();

    void onZoomOut();

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
};

#endif // PDFVIEWWIDGET_H
