#include "pdfviewwidget.h"
#include "qtimer.h"

#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>

#include <QPushButton>
#include <QLabel>

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>
#include <QFile>
#include <QResizeEvent>


PdfViewWidget::PdfViewWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
}

PdfViewWidget::~PdfViewWidget()
{
    delete m_document;
}

void PdfViewWidget::initUI()
{
    auto mainLayout =
        new QVBoxLayout(this);

    // toolbar

    auto toolbarLayout =
        new QHBoxLayout;

    m_prevBtn =
        new QPushButton("上一页", this);

    m_nextBtn =
        new QPushButton("下一页", this);

    m_zoomInBtn =
        new QPushButton("+", this);

    m_zoomOutBtn =
        new QPushButton("-", this);

    m_pageLabel =
        new QLabel("0 / 0", this);

    toolbarLayout->addWidget(m_prevBtn);

    toolbarLayout->addWidget(m_nextBtn);

    toolbarLayout->addSpacing(20);

    toolbarLayout->addWidget(m_zoomOutBtn);

    toolbarLayout->addWidget(m_zoomInBtn);

    toolbarLayout->addSpacing(20);

    toolbarLayout->addWidget(m_pageLabel);

    toolbarLayout->addStretch();

    // =========================
    // graphics view
    // =========================

    m_scene =
        new QGraphicsScene(this);

    m_view =
        new QGraphicsView(m_scene, this);

    m_view->setRenderHints(
        QPainter::Antialiasing |
        QPainter::SmoothPixmapTransform);

    // =========================
    // layout
    // =========================

    mainLayout->addLayout(toolbarLayout);

    mainLayout->addWidget(m_view);


    m_view->setDragMode(
        QGraphicsView::ScrollHandDrag);

    m_view->setTransformationAnchor(
        QGraphicsView::AnchorUnderMouse);

    m_view->setResizeAnchor(
        QGraphicsView::AnchorUnderMouse);

    m_view->setViewportUpdateMode(
        QGraphicsView::SmartViewportUpdate);


    // =========================
    // connect
    // =========================

    connect(
        m_prevBtn,
        &QPushButton::clicked,
        this,
        &PdfViewWidget::onPrevPage);

    connect(
        m_nextBtn,
        &QPushButton::clicked,
        this,
        &PdfViewWidget::onNextPage);

    connect(
        m_zoomInBtn,
        &QPushButton::clicked,
        this,
        &PdfViewWidget::onZoomIn);

    connect(
        m_zoomOutBtn,
        &QPushButton::clicked,
        this,
        &PdfViewWidget::onZoomOut);
}


bool PdfViewWidget::loadPdf(
    const QString &filePath)
{
    qDebug() << "load pdf:" << filePath;

    delete m_document;

    m_document = nullptr;

    if (!QFile::exists(filePath))
    {
        qDebug() << "pdf file not exists";

        return false;
    }

    // Qt文件路径编码
    QByteArray path =
        QFile::encodeName(filePath);

    m_document =
        poppler::document::load_from_file(
            std::string(path.data()));

    if (!m_document)
    {
        qDebug() << "load pdf failed";

        return false;
    }

    qDebug() << "pdf loaded";

    m_currentPage = 0;
    m_scaleFactor = 1.0;
    m_autoFit = true;

    renderPage();

    updatePageLabel();

    return true;
}



void PdfViewWidget::renderPage()
{
    qDebug() << __FUNCTION__ <<"开始渲染第"<<m_currentPage<<"页";
    if (!m_document)   return;

    auto page =
        m_document->create_page(
            m_currentPage);

    if (!page)
        return;

    poppler::page_renderer renderer;

    renderer.set_render_hint(poppler::page_renderer::antialiasing, true);
    renderer.set_render_hint(poppler::page_renderer::text_antialiasing,true);
    renderer.set_render_hint(poppler::page_renderer::text_hinting, true);


    // 根据缩放比例提高DPI
    qreal dpr = devicePixelRatioF();
    int dpi = 144.0 * m_scaleFactor * dpr;
    qDebug() << __FUNCTION__ <<"渲染DPI："<<dpi<<"DPR:"<<dpr;

    auto image =renderer.render_page( page, dpi,dpi);

    if (!image.is_valid())
    {
        delete page;
        return;
    }
    qDebug() << __FUNCTION__ <<"渲染完成尺寸："<<image.width()<<image.height();
    // Poppler -> QImage
    QImage qimage(reinterpret_cast<const uchar*>(image.data()),
        image.width(),
                  image.height(),
                  QImage::Format_ARGB32);

    qimage.setDevicePixelRatio(dpr);


    m_scene->clear();

    m_pixmapItem =
        m_scene->addPixmap(
            QPixmap::fromImage(qimage.copy()));
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    m_pixmapItem->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

    m_view->resetTransform();
    m_scene->setSceneRect(m_pixmapItem->boundingRect());

    if (m_autoFit) {
        QTimer::singleShot(0, this, &PdfViewWidget::fitPageToView);
    }
    delete page;
}

void PdfViewWidget::updatePageLabel()
{
    if (!m_document)
    {
        m_pageLabel->setText("0 / 0");

        return;
    }

    m_pageLabel->setText(
        QString("%1 / %2")
            .arg(m_currentPage + 1)
            .arg(m_document->pages()));
}


void PdfViewWidget::onPrevPage()
{
    if (!m_document)
        return;

    if (m_currentPage <= 0)
        return;

    --m_currentPage;

    renderPage();

    updatePageLabel();
}

void PdfViewWidget::onNextPage()
{
    if (!m_document)
        return;

    if (m_currentPage >= m_document->pages() - 1)
        return;

    ++m_currentPage;

    renderPage();

    updatePageLabel();
}

void PdfViewWidget::onZoomIn()
{
    m_autoFit = false;
    m_scaleFactor *= 1.1;
    renderPage();
}

void PdfViewWidget::onZoomOut()
{
    m_autoFit = false;
    m_scaleFactor *= 0.9;
    renderPage();
}

void PdfViewWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (m_autoFit)
        fitPageToView();
}

void PdfViewWidget::fitPageToView()
{
    if (!m_pixmapItem || !m_view)
        return;

    m_view->resetTransform();
    m_view->fitInView(
        m_pixmapItem,
        Qt::KeepAspectRatio);
}

