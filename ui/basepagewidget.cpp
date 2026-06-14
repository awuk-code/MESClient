#include "basepagewidget.h"
#include "fieldfilterproxymodel.h"
#include "basetablemodel.h"
#include "headeroverlaywidget.h"
#include "imageviewwidget.h"
#include "lineeditdelegate.h"
#include "operationdelegate.h"
#include "paginationproxymodel.h"
#include "reportexporter.h"

#include <QAbstractItemView>
#include <QDebug>
#include <QEvent>
#include <QFontMetrics>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QIcon>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QScrollBar>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QTabBar>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

BasePageWidget::BasePageWidget(QWidget *parent)
    : QWidget{parent}
{
}

void BasePageWidget::setupPage()
{
    initUI();
    if(m_searchEdit)
        m_searchEdit->setPlaceholderText(searchInfo());

}

void BasePageWidget::onPageLinkClicked(const QVariantMap &rowData, const QString &pageID)
{
    Q_UNUSED(rowData)
    Q_UNUSED(pageID)
    qDebug() << __FUNCTION__ <<"clicked";
}

void BasePageWidget::onImageLinkClicked(const QString &NGNumber)
{
    qDebug() << __FUNCTION__ <<"clicked";
    qDebug() << tr("查看图片:") << NGNumber;

    // 1. 创建图片查看器

    auto viewer =
        new ImageViewWidget;

    viewer->resize(1200, 800);

    viewer->show();

    // 2. 发HTTP请求

    // emit sigRequestNGImage(
    //     ngNumber,
    //     viewer);
}

bool BasePageWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (event && event->type() == QEvent::Resize)
    {
        if (auto table = qobject_cast<QTableView*>(watched))
        {
            QTimer::singleShot(0, this, [this, table]() {
                applyAdaptiveColumnWidths(table);
            });
        }
    }

    return QWidget::eventFilter(watched, event);
}

void BasePageWidget::updateTableResizeMode()
{
    for (QWidget* page : qAsConst(m_pages))
    {
        if (auto table = qobject_cast<QTableView*>(page))
            applyAdaptiveColumnWidths(table);
    }
}

TabConfigs BasePageWidget::Tabs() const
{
    return m_tabs;
}

QHBoxLayout *BasePageWidget::createTitleLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;

    QLabel* icon = new QLabel(this);
    QLabel* text = new QLabel(this);

    icon->setPixmap(QPixmap(":res/common/rect.svg").scaled(24,24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    text->setText(pageTitle());
    // 标题等级样式入口：页面一级标题，后续修改字体时在 QSS 的 pageTitle 中统一设置。
    text->setProperty("labelRole", "pageTitle");
    layout->addWidget(icon);
    layout->addWidget(text);
    addWidgetToTitle(layout);
    return layout;
}

void BasePageWidget::setupSearchLayout(QHBoxLayout* layout)
{
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setMinimumWidth(180);
    m_searchEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_searchEdit->addAction(QIcon(":/res/common/search.svg"),
                            QLineEdit::LeadingPosition);


    layout->addWidget(m_searchEdit);

    layout->addStretch();
    setupPaginationLayout(layout);

    m_exportBtn = new QPushButton(tr("导出报表"), this);
    m_exportBtn->setProperty("buttonRole", "export");
    layout->addWidget(m_exportBtn);
}

void BasePageWidget::setupPaginationLayout(QHBoxLayout* layout)
{
    if (!layout)
        return;

    m_prevPageBtn = new QPushButton(tr("上一页"), this);
    m_prevPageBtn->setProperty("buttonRole", "pageNav");
    layout->addWidget(m_prevPageBtn);

    for (int i = 0; i < 3; ++i)
    {
        auto button = new QPushButton(QString::number(i + 1), this);
        button->setProperty("buttonRole", "pageNumber");
        button->setFixedWidth(34);
        m_pageBtns.append(button);
        layout->addWidget(button);
    }

    m_nextPageBtn = new QPushButton(tr("下一页"), this);
    m_nextPageBtn->setProperty("buttonRole", "pageNav");
    layout->addWidget(m_nextPageBtn);
}

void BasePageWidget::initUI()
{
    auto mainLayout = new QVBoxLayout(this);

    // ===== 1. 标题 =====

    auto titleLayout = createTitleLayout();
    mainLayout->addLayout(titleLayout);

    auto contentFrame = new QFrame(this);
    contentFrame->setProperty("panelRole", "pageContent");
    contentFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto contentLayout = new QVBoxLayout(contentFrame);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // ===== TabBar（只显示状态）=====
    m_tabBar = new QTabBar(contentFrame);
    m_tabBar->setObjectName("BasePageTabBar");
    m_tabBar->setExpanding(false);
    QFont tabFont = m_tabBar->font();
    tabFont.setPixelSize(13);
    m_tabBar->setFont(tabFont);
    contentLayout->addWidget(m_tabBar);


    // ===== 搜索栏 =====
    m_searchWidget = new QWidget(contentFrame);
    m_searchLayout = new QHBoxLayout(m_searchWidget);
    m_searchLayout->setContentsMargins(0, 0, 0, 0);
    setupSearchLayout(m_searchLayout);
    contentLayout->addWidget(m_searchWidget);

    // ===== Stack（表格区域）=====
    m_stack = new QStackedWidget(contentFrame);

    contentLayout->addWidget(m_stack);
    mainLayout->addWidget(contentFrame, 1);

    // ===== Model =====
    m_model = createModel();

    initTabs();
    initConnect();
}
void BasePageWidget::initTabs()
{
    TabConfigs tabs = this->Tabs();

    if (tabs.isEmpty())
    {
        m_tabBar->hide();
        return;
    }

    for (int i = 0; i < tabs.size(); ++i)
    {
        // 1. Tab UI
        m_tabBar->addTab(tabs[i].title);

        QWidget* page = nullptr;

        // 2. TABLE 页面（默认）
        if(tabs[i].PageDisplayType == PageDisplayType::TABLE){

            // 2. Proxy
            auto proxy = createProxy(tabs[i].data);
            // auto proxy = createProxy(tabs[i].data.toInt());
            if (!proxy || !m_model)
                continue;

            proxy->setSourceModel(m_model);

            // 3. Table（统一入口）
            QTableView* table = createTable(proxy, tabs[i].data);
            if (!table)
                continue;
            //表格
            page = table;

            m_proxies.append(proxy);
        }
        else{
            page = tabs[i].page;
        }
        if(page){
            m_pages.append(page);
            m_stack->addWidget(page);
        }
    }


    // 6. 默认选中第一个
    if (m_stack->count() > 0)
    {
        m_stack->setCurrentIndex(0);
        m_tabBar->setCurrentIndex(0);
    }

    refreshPaginationControls();

}

void BasePageWidget::initConnect()
{
    //绑定搜索过滤
    connect(m_searchEdit, &QLineEdit::textChanged, this,
            [=](const QString& text)
            {
                for (auto proxy : m_proxies)
                {
                    proxy->setKeyword(text);
                }

                resetPaginationToFirstPage();
            });
    //绑定页面切换
    connect(m_tabBar, &QTabBar::currentChanged, m_stack, &QStackedWidget::setCurrentIndex);
    connect(m_tabBar, &QTabBar::currentChanged, this,
            [=]() { refreshPaginationControls(); });

    if (m_prevPageBtn)
    {
        connect(m_prevPageBtn, &QPushButton::clicked, this, [=]() {
            if (auto pageProxy = currentPaginationProxy())
                pageProxy->previousPage();
        });
    }

    if (m_nextPageBtn)
    {
        connect(m_nextPageBtn, &QPushButton::clicked, this, [=]() {
            if (auto pageProxy = currentPaginationProxy())
                pageProxy->nextPage();
        });
    }

    for (auto button : qAsConst(m_pageBtns))
    {
        connect(button, &QPushButton::clicked, this,
                [=]() { goToPaginationButtonPage(button); });
    }

    //点击搜索
    if (m_searchBtn)
    {
        connect(m_searchBtn, &QPushButton::clicked, this,
                &BasePageWidget::onSearchBtnClicked);
    }
    if (m_exportBtn &&
        m_exportBtn->property("buttonRole").toString() == "export")
    {
        connect(m_exportBtn, &QPushButton::clicked, this,
                &BasePageWidget::onExportBtnClicked);
    }

    connect(this, &BasePageWidget::sigPageSwitching, this, &BasePageWidget::onPageLinkClicked);
    connect(this, &BasePageWidget::sigIMGView, this, &BasePageWidget::onImageLinkClicked);

}

void BasePageWidget::onSearchBtnClicked()
{
    QString text = m_searchEdit->text();

    for (auto proxy : qAsConst(m_proxies))
    {
        proxy->setKeyword(text);
    }

    resetPaginationToFirstPage();
}

void BasePageWidget::onExportBtnClicked()
{
    exportCurrentTableToExcel();
}

void BasePageWidget::exportCurrentTableToExcel()
{
    auto table =
        qobject_cast<QTableView*>(m_stack ? m_stack->currentWidget() : nullptr);
    ReportExporter::exportTable(this, table);
}

bool BasePageWidget::isColumnVisibleForTab(
    const ColumnConfig& column,
    const QVariant& tabData) const
{
    Q_UNUSED(tabData)
    return column.visible;
}

QSet<QString> BasePageWidget::collectFilterFields(const QVariant& tabData) const
{
    QSet<QString> filterFields;

    // 获取基础模型
    auto baseModel = qobject_cast<BaseTableModel*>(m_model);
    if (!baseModel)
        return filterFields;

    const auto& columns = baseModel->columns();

    for (const auto& column : columns)
    {
        if (!isColumnVisibleForTab(column, tabData))
            continue;

        // 只有设置了筛选类型的列，才显示表头筛选图标
        switch (column.filterType)
        {
        case FilterType::Date:
        case FilterType::Priority:
            // 只有这两种需要在表头绘制图标
            filterFields.insert(column.field);
            break;

        default:
            // None / Status / Keyword 等都不显示图标
            break;
        }
    }

    return filterFields;
}

QTableView *BasePageWidget::createTable(FieldFilterProxyModel *proxy, const QVariant& tabData)
{
    // 1. 创建表格
    QTableView* table = new QTableView(this);
    table->setProperty("tabData", tabData);

    // 2. 设置模型：先按状态/搜索过滤，再按当前页截取固定数量数据。
    auto pageProxy = new PaginationProxyModel(table);
    pageProxy->setPageSize(m_pageSize);
    pageProxy->setSourceModel(proxy);
    table->setModel(pageProxy);
    m_pageProxies.append(pageProxy);

    // 3. 创建表头
    auto header = new QHeaderView(Qt::Horizontal, table);
    header->setDefaultAlignment(Qt::AlignCenter);
    table->setHorizontalHeader(header);

    // 4. 设置表头筛选 Overlay
    setupHeaderOverlay(table, proxy, tabData);

    // 5. 设置列配置
    setupColumns(table, tabData);

    // 6. 绑定复选框点击逻辑
    setupCheckBoxSelection(table, tabData);

    // 7. 设置表格外观
    setupTableAppearance(table);

    table->installEventFilter(this);
    connect(proxy, &QAbstractItemModel::modelReset, pageProxy,
            &PaginationProxyModel::firstPage);
    connect(proxy, &QAbstractItemModel::layoutChanged, pageProxy,
            &PaginationProxyModel::firstPage);

    connect(pageProxy, &PaginationProxyModel::pageInfoChanged, this,
            [=]() {
                refreshPaginationControls();
                scheduleColumnResize(table);
            });

    auto refreshTable = [=]() {
        refreshPaginationControls();
        scheduleColumnResize(table);
    };
    connect(pageProxy, &QAbstractItemModel::modelReset, this, refreshTable);
    connect(pageProxy, &QAbstractItemModel::layoutChanged, this, refreshTable);
    connect(pageProxy, &QAbstractItemModel::rowsInserted, this, refreshTable);
    connect(pageProxy, &QAbstractItemModel::rowsRemoved, this, refreshTable);
    connect(pageProxy, &QAbstractItemModel::dataChanged, this,
            [=]() { scheduleColumnResize(table); });

    // 8. 按内容自适应列宽；空间足够时自动铺满表格区域。
    scheduleColumnResize(table);
    return table;
}

void BasePageWidget::setupHeaderOverlay(
    QTableView* table,
    FieldFilterProxyModel* proxy,
    const QVariant& tabData)
{
    if (!table || !proxy)
        return;

    // 获取表头
    auto header = table->horizontalHeader();
    if (!header)
        return;

    // 创建 Overlay（父对象设为 header->viewport()）
    auto overlay =
        new HeaderOverlayWidget(header, header->viewport());

    // 初始化位置
    overlay->setGeometry(header->viewport()->rect());
    overlay->show();
    overlay->raise();

    // 表头整体布局变化
    connect(header,
            &QHeaderView::geometriesChanged,
            this,
            [=]()
            {
                overlay->setGeometry(header->viewport()->rect());
                overlay->update();
            });

    // 列宽变化
    connect(header,
            &QHeaderView::sectionResized,
            this,
            [=]()
            {
                overlay->update();
            });

    // 列移动
    connect(header,
            &QHeaderView::sectionMoved,
            this,
            [=]()
            {
                overlay->update();
            });

    // 水平滚动
    connect(table->horizontalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            [=]()
            {
                overlay->setGeometry(header->viewport()->rect());
                overlay->update();
            });

    // 设置需要显示筛选图标的字段
    overlay->setFilterFields(collectFilterFields(tabData));

    // 将筛选结果传递给代理模型
    connect(overlay,
            &HeaderOverlayWidget::filterSelected,
            this,
            [=](const QString& field, const QVariant& value)
            {
                proxy->setFieldFilter(field, value);
            });
}

void BasePageWidget::setupColumns(QTableView *table, const QVariant& tabData)
{
    if (!table || !m_model)
        return;

    auto baseModel = qobject_cast<BaseTableModel*>(m_model);
    if (!baseModel)
        return;

    const auto& columns = baseModel->columns();

    auto header = table->horizontalHeader();
    if (!header)
        return;

    for (int col = 0; col < columns.size(); ++col)
    {
        const auto& cfg = columns[col];

        // =========================
        // 1. 列显示/隐藏
        // =========================
        const bool visible = isColumnVisibleForTab(cfg, tabData);

        table->setColumnHidden(col, !visible);

        if (!visible)
            continue;

        // =========================
        // 2. 列宽模式
        // =========================
        // 所有列统一由 applyAdaptiveColumnWidths() 计算实际宽度：
        // 先按内容完整显示，空间足够时再扩展铺满窗口。
        header->setSectionResizeMode(col, QHeaderView::Interactive);

        // =========================
        // 3. Delegate,输入框代理创建
        // =========================
        if (cfg.type == ColumnType::LineEdit)
        {
            table->setItemDelegateForColumn(
                col,
                new LineEditDelegate(table));   // 每个表格独立创建
        }
        else if (cfg.delegate)
        {
            table->setItemDelegateForColumn(col, cfg.delegate);
        }
        else
        {
            // 切换不同表格类型后，清掉当前列可能残留的旧输入框委托。
            table->setItemDelegateForColumn(col, nullptr);
        }

        // 所有表格单元格和表头默认居中，具体返回值在 BaseTableModel::data() 中统一处理。
    }

    applyAdaptiveColumnWidths(table);
}

void BasePageWidget::applyAdaptiveColumnWidths(QTableView* table)
{
    if (!table || !table->model() || !m_model)
        return;

    auto baseModel = qobject_cast<BaseTableModel*>(m_model);
    if (!baseModel)
        return;

    auto header = table->horizontalHeader();
    if (!header)
        return;

    const auto& columns = baseModel->columns();
    QVector<int> visibleColumns;
    QVector<int> widths(columns.size(), 0);
    int totalWidth = 0;
    int stretchableCount = 0;

    const QVariant tabData = table->property("tabData");
    for (int col = 0; col < columns.size(); ++col)
    {
        if (table->isColumnHidden(col) ||
            !isColumnVisibleForTab(columns[col], tabData))
            continue;

        const int width = contentWidthForColumn(table, baseModel, col);
        widths[col] = width;
        totalWidth += width;
        visibleColumns.append(col);

        if (columns[col].type != ColumnType::Operation)
            ++stretchableCount;
    }

    if (visibleColumns.isEmpty())
        return;

    const int availableWidth = table->viewport()->width();
    int extraWidth = qMax(0, availableWidth - totalWidth);

    for (int col : visibleColumns)
    {
        int finalWidth = widths[col];

        if (extraWidth > 0 &&
            columns[col].type != ColumnType::Operation &&
            stretchableCount > 0)
        {
            const int addWidth = extraWidth / stretchableCount;
            finalWidth += addWidth;
            extraWidth -= addWidth;
            --stretchableCount;
        }

        header->resizeSection(col, finalWidth);
    }
}

void BasePageWidget::setupTableAppearance(QTableView *table)
{
    if (!table)
        return;

    table->setEditTriggers(QAbstractItemView::CurrentChanged |
                           QAbstractItemView::SelectedClicked |
                           QAbstractItemView::DoubleClicked);
    table->setAlternatingRowColors(true);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    table->setWordWrap(false);
    table->setTextElideMode(Qt::ElideNone);
}

PaginationProxyModel* BasePageWidget::currentPaginationProxy() const
{
    auto table =
        qobject_cast<QTableView*>(m_stack ? m_stack->currentWidget() : nullptr);
    if (!table)
        return nullptr;

    return qobject_cast<PaginationProxyModel*>(table->model());
}

void BasePageWidget::refreshPaginationControls()
{
    auto pageProxy = currentPaginationProxy();
    const bool hasPagination = pageProxy != nullptr;

    if (m_prevPageBtn)
        m_prevPageBtn->setVisible(hasPagination);
    if (m_nextPageBtn)
        m_nextPageBtn->setVisible(hasPagination);
    for (auto button : qAsConst(m_pageBtns))
        button->setVisible(hasPagination);

    if (!pageProxy)
        return;

    const int currentPage = pageProxy->currentPage();
    const int pageCount = pageProxy->pageCount();

    if (m_prevPageBtn)
        m_prevPageBtn->setEnabled(currentPage > 1);
    if (m_nextPageBtn)
        m_nextPageBtn->setEnabled(currentPage < pageCount);

    int startPage = qMax(1, currentPage - 1);
    if (startPage + m_pageBtns.size() - 1 > pageCount)
        startPage = qMax(1, pageCount - m_pageBtns.size() + 1);

    for (int i = 0; i < m_pageBtns.size(); ++i)
    {
        QPushButton* button = m_pageBtns[i];
        const int page = startPage + i;
        const bool validPage = page <= pageCount;

        button->setVisible(validPage);
        if (!validPage)
            continue;

        button->setText(QString::number(page));
        button->setProperty("page", page);
        button->setEnabled(page != currentPage);
    }
}

void BasePageWidget::resetPaginationToFirstPage()
{
    for (auto pageProxy : qAsConst(m_pageProxies))
    {
        if (pageProxy)
            pageProxy->firstPage();
    }
}

void BasePageWidget::goToPaginationButtonPage(QPushButton* button)
{
    if (!button)
        return;

    auto pageProxy = currentPaginationProxy();
    if (!pageProxy)
        return;

    bool ok = false;
    const int page = button->property("page").toInt(&ok);
    if (ok)
        pageProxy->setCurrentPage(page);
}

void BasePageWidget::scheduleColumnResize(QTableView* table)
{
    if (!table)
        return;

    QTimer::singleShot(0, this, [=]() { applyAdaptiveColumnWidths(table); });
}

void BasePageWidget::setupCheckBoxSelection(QTableView* table, const QVariant& tabData)
{
    auto baseModel = qobject_cast<BaseTableModel*>(m_model);
    if (!table || !baseModel)
        return;

    const auto& columns = baseModel->columns();

    int checkCol = -1;
    for (int i = 0; i < columns.size(); ++i)
    {
        if (!isColumnVisibleForTab(columns[i], tabData))
            continue;

        if (columns[i].type == ColumnType::CheckBox)
        {
            checkCol = i;
            break;
        }
    }

    if (checkCol < 0)
        return;

    connect(table, &QTableView::clicked, this,
            [=](const QModelIndex& index)
            {
                if (!index.isValid() || index.column() != checkCol)
                    return;

                const auto state = index.data(Qt::CheckStateRole);

                if (state == Qt::Checked)
                {
                    table->selectRow(index.row());
                }
                else
                {
                    table->selectionModel()->select(
                        index,
                        QItemSelectionModel::Deselect |
                            QItemSelectionModel::Rows);
                }
        });
}

int BasePageWidget::contentWidthForColumn(
    QTableView* table,
    const BaseTableModel* model,
    int column) const
{
    if (!table || !table->model() || !model)
        return 0;

    const auto& columns = model->columns();
    if (column < 0 || column >= columns.size())
        return 0;

    const auto& cfg = columns[column];
    const int padding = 28;

    QFontMetrics headerFm(table->horizontalHeader()->font());
    const QString headerText =
        table->model()->headerData(column, Qt::Horizontal, Qt::DisplayRole)
            .toString();
    int width = headerFm.horizontalAdvance(headerText) + padding;

    if (cfg.filterType == FilterType::Date ||
        cfg.filterType == FilterType::Priority)
    {
        width += 18;
    }

    if (cfg.type == ColumnType::CheckBox)
    {
        width = qMax(width, qMax(18, QFontMetrics(table->font()).height() + 2) + 24);
    }
    else if (cfg.type == ColumnType::Operation)
    {
        width = qMax(width, OperationDelegate::minimumColumnWidth(table->font()));
    }

    QFontMetrics cellFm(table->font());
    for (int row = 0; row < table->model()->rowCount(); ++row)
    {
        const QModelIndex index = table->model()->index(row, column);
        QString text = index.data(Qt::DisplayRole).toString();

        if (text.isEmpty() && cfg.type == ColumnType::LineEdit)
        {
            text = headerText.isEmpty()
                       ? tr("请输入或扫入")
                       : tr("请输入或扫入%1").arg(headerText);
        }

        width = qMax(width, cellFm.horizontalAdvance(text) + padding);
    }

    return width;
}

void BasePageWidget::updateSearchInfo()
{
    if (m_searchEdit)
    {
        m_searchEdit->setPlaceholderText(searchInfo());
    }
}




