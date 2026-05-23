#include "pdfviewwidget.h"

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

    // =========================
    // toolbar
    // =========================

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

    renderPage();

    updatePageLabel();

    return true;
}



void PdfViewWidget::renderPage()
{
    if (!m_document)
        return;

    auto page =
        m_document->create_page(
            m_currentPage);

    if (!page)
        return;

    poppler::page_renderer renderer;

    auto image =
        renderer.render_page(
            page,
            96 * m_scaleFactor,
            96 * m_scaleFactor);

    if (!image.is_valid())
    {
        delete page;

        return;
    }

    QImage qimage(
        reinterpret_cast<const uchar*>(image.data()),
        image.width(),
        image.height(),
        image.bytes_per_row(),
        QImage::Format_RGBA8888);

    m_scene->clear();

    m_scene->addPixmap(
        QPixmap::fromImage(qimage.copy()));

    m_view->fitInView(
        m_scene->itemsBoundingRect(),
        Qt::KeepAspectRatio);

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
    m_scaleFactor += 0.2;

    renderPage();
}

void PdfViewWidget::onZoomOut()
{
    m_scaleFactor -= 0.2;

    if (m_scaleFactor < 0.4)
    {
        m_scaleFactor = 0.4;
    }

    renderPage();
}

