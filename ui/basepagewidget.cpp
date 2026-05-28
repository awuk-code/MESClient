#include "basepagewidget.h"
#include "fieldfilterproxymodel.h"
#include "basetablemodel.h"
#include "headeroverlaywidget.h"
#include "imageviewwidget.h"

#include <QScrollBar>

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
    qDebug() << __FUNCTION__ <<"clicked";
}

void BasePageWidget::onImageLinkClicked(const QString &NGNumber)
{
    qDebug() << __FUNCTION__ <<"clicked";
    qDebug() << "查看图片:" << NGNumber;

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

void BasePageWidget::updateTableResizeMode()
{
    return;
/*
    //获取基础模型
    auto baseModel = qobject_cast<BaseTableModel*>(m_model);
    if(!baseModel) return;

    const auto& columns = baseModel->columns();

    //判段window最大化
    bool windowIsMax = window()->isMaximized();
    //遍历表格
    for(QTableView* table : std::as_const(m_tables)){
        if(!table) continue;

        auto header = table->horizontalHeader();
        const QVariant tabData = table->property("tabData");

        for(int i=0; i< columns.size();++i){
            const auto &cfg = columns[i];

            if(!isColumnVisibleForTab(cfg, tabData)) continue;

            if(windowIsMax){

                if(cfg.resizeMode == QHeaderView::Fixed){
                    header->setSectionResizeMode(i,QHeaderView::Fixed);
                    table->setColumnWidth(i, cfg.width);
                }      else{
                    header->setSectionResizeMode(i,QHeaderView::Stretch);
                }
            }else{
                //普通窗口
                header->setSectionResizeMode(i,cfg.resizeMode);
                if(cfg.resizeMode == QHeaderView::Fixed){
                    table->setColumnWidth(i, cfg.width);
                }
            }

        }
    }
*/
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
    m_searchBtn = new QPushButton(this);
    m_searchBtn->setIcon(QIcon(":/res/common/search.svg"));

    layout->addWidget(m_searchEdit);
    layout->addWidget(m_searchBtn);
    layout->addStretch();

    m_exportBtn = new QPushButton("导出报表",this);
    layout->addWidget(m_exportBtn);
}

void BasePageWidget::initUI()
{
    auto mainLayout = new QVBoxLayout(this);

    // ===== 1. 标题 =====

    auto titleLayout = createTitleLayout();
    mainLayout->addLayout(titleLayout);

    // ===== TabBar（只显示状态）=====
    m_tabBar = new QTabBar(this);
    m_tabBar->setExpanding(false);
    m_tabBar->setStyleSheet(R"(
    QTabBar {
        border-top: 1px solid #dcdfe6;
    }

    QTabBar::tab {
        background: transparent;
        padding: 8px 20px;
        margin-top: 1px;
        color: #606266;
        border: none;
    }

    QTabBar::tab:selected {
        color: #409EFF;
        border-top: 2px solid #409EFF;
    }

    QTabBar::tab:hover {
        color: #409EFF;
    }
)");
    mainLayout->addWidget(m_tabBar);


    // ===== 搜索栏 =====
    m_searchWidget = new QWidget(this);
    m_searchLayout = new QHBoxLayout(m_searchWidget);
    m_searchLayout->setContentsMargins(0, 0, 0, 0);
    setupSearchLayout(m_searchLayout);
    mainLayout->addWidget(m_searchWidget);

    // ===== Stack（表格区域）=====
    m_stack = new QStackedWidget(this);

    mainLayout->addWidget(m_stack);

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

            m_tables.append(table);
            m_proxies.append(proxy);
        }
        else{
            page = tabs[i].page;
        }
        if(page){
            m_stack->addWidget(page);
        }
    }


    // 6. 默认选中第一个
    if (m_stack->count() > 0)
    {
        m_stack->setCurrentIndex(0);
        m_tabBar->setCurrentIndex(0);
    }

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
            });
    //绑定页面切换
    connect(m_tabBar, &QTabBar::currentChanged, m_stack, &QStackedWidget::setCurrentIndex);

    //点击搜索
    if (m_searchBtn)
    {
        connect(m_searchBtn, &QPushButton::clicked, this,
                [=]()
                {
                    QString text = m_searchEdit->text();

                    for (auto proxy : std::as_const(m_proxies))
                    {
                        proxy->setKeyword(text);
                    }
                });
    }
    connect(this, &BasePageWidget::sigPageSwitching, this, &BasePageWidget::onPageLinkClicked);
    connect(this, &BasePageWidget::sigIMGView, this, &BasePageWidget::onImageLinkClicked);

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

    // 2. 设置模型
    table->setModel(proxy);

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

    // 8. 根据窗口状态调整列宽
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
        const bool contentSizedColumn =
            cfg.type == ColumnType::RowNumber ||
            cfg.type == ColumnType::CheckBox ||
            cfg.type == ColumnType::Operation ||
            cfg.filterType == FilterType::Date ||
            cfg.filterType == FilterType::Priority;

        header->setSectionResizeMode(
            col,
            contentSizedColumn
                ? QHeaderView::ResizeToContents
                : QHeaderView::Stretch);

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

        // =========================
        // 4. 对齐方式（补强项，建议保留）
        // =========================
        table->model()->setHeaderData(
            col,
            Qt::Horizontal,
            // QVariant::fromValue(cfg.alignment),
            static_cast<int>(cfg.alignment),
            Qt::TextAlignmentRole
            );
    }

}

void BasePageWidget::setupTableAppearance(QTableView *table)
{
    table->setEditTriggers( QAbstractItemView::CurrentChanged |
                           QAbstractItemView::SelectedClicked |
                           QAbstractItemView::DoubleClicked);
    // 开启斑马纹（隔行颜色）
    table->setAlternatingRowColors(true);


    // 横向滚动按像素平滑滚动
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    // 根据需要显示横向滚动条
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 表头文字居中
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    // 禁止自动换行，列宽按内容自适应，放不下时使用横向滚动条完整查看。
    table->setWordWrap(false);
    table->setTextElideMode(Qt::ElideNone);

}

void BasePageWidget::setupCheckBoxSelection(QTableView* table, const QVariant& tabData)
{
    if (!table)
        return;

    auto baseModel = qobject_cast<BaseTableModel*>(m_model);
    if (!baseModel)
        return;

    const auto& columns = baseModel->columns();

    // 找到 checkbox 列
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
                if (!index.isValid())
                    return;

                if (index.column() != checkCol)
                    return;

                auto state = index.data(Qt::CheckStateRole);

                if (state == Qt::Checked)
                {
                    table->selectRow(index.row());
                }
                else
                {
                    table->selectionModel()->select(
                        index,
                        QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
                }
            });
}

void BasePageWidget::updateSearchInfo()
{
    if (m_searchEdit)
    {
        m_searchEdit->setPlaceholderText(searchInfo());
    }
}



