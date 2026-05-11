#include "basepagewidget.h"
#include "fieldfilterproxymodel.h"
#include "basetablemodel.h"
#include "headeroverlaywidget.h"

BasePageWidget::BasePageWidget(QWidget *parent)
    : QWidget{parent}
{

}

void BasePageWidget::setupPage()
{
    initUI();
    if (m_titleLabel)
        m_titleLabel->setText(pageTitle());
    if(m_searchEdit)
        m_searchEdit->setPlaceholderText(searchInfo());

    m_exportBtn->setVisible(enableExport());

}



void BasePageWidget::initUI()
{
    auto mainLayout = new QVBoxLayout(this);

    // ===== 1. 标题 =====
    auto titleLayout = new QHBoxLayout;
    m_titleIcon = new QLabel(this);
    m_titleIcon->setPixmap(QPixmap(":res/common/rect.svg").scaled(24,24, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("PageTitle");

    titleLayout->addWidget(m_titleIcon);
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();

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


    // ===== Tab =====
    // m_tabWidget = new QTabWidget(this);
    // mainLayout->addWidget(m_tabWidget);


    // ===== 搜索栏 =====
    auto searchLayout = new QHBoxLayout;
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setFixedWidth(240);
    m_searchBtn = new QPushButton(this);
    m_searchBtn->setIcon(QIcon(":/res/common/search.svg"));

    // m_searchBtn->hide();
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_searchBtn);
    searchLayout->addStretch();

    m_exportBtn = new QPushButton("导出报表",this);
    searchLayout->addWidget(m_exportBtn);
    mainLayout->addLayout(searchLayout);

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
    QVector<TabConfig> tabs = this->tabs();

    if (tabs.isEmpty())
        m_tabBar->hide();

    for (int i = 0; i < tabs.size(); ++i)
    {
        // Tab按钮
        m_tabBar->addTab(tabs[i].title);
        //创建表格
        QTableView* table = new QTableView;
        //创建代理模型
        auto proxy = createProxy(tabs[i].status);
        if (!proxy || !m_model) continue;

        proxy->setSourceModel(m_model);
        table->setModel(proxy);

        auto header = new QHeaderView(Qt::Horizontal, table);
        header->setDefaultAlignment(Qt::AlignCenter);
        table->setHorizontalHeader(header);

        auto overlay = new HeaderOverlayWidget(header, header->viewport());

        overlay->resize(header->viewport()->size());
        overlay->show();
        overlay->raise();

        connect(header, &QHeaderView::geometriesChanged,
                this, [=]() {
                    overlay->resize(header->viewport()->size());
                    overlay->update();
                });

        connect(header, &QHeaderView::sectionResized,
                this, [=]() {
                    overlay->update();
                });

        connect(header, &QHeaderView::sectionMoved,
                this, [=]() {
                    overlay->update();
                });
     ///设置搜索区域,可以移除原先的自定义表头header，采用遮罩层方法
        QSet<QString> filterFields;
        filterFields << "startTime"
                     << "finishTime"
                     << "priority";
        overlay->setFilterFields(filterFields); // 用于筛选哪些需要自定义图标



        // ==============================
        // 5. 设置列配置
         // ===== 获取基础模型 =====
        auto baseModel = qobject_cast<BaseTableModel*>(m_model);

        if (baseModel)
        {
            const auto& columns = baseModel->columns();
            qDebug() << __FUNCTION__ << "column count =" << columns.size();
            auto header = table->horizontalHeader();    //--------
            for (int col = 0; col < columns.size(); ++col)
            {
                const auto& cfg = columns[col];

                //----设置列模式
                header->setSectionResizeMode(col, cfg.resizeMode);
                // ===== Fixed列设置宽度 =====
                if (cfg.resizeMode == QHeaderView::Fixed)
                {
                    table->setColumnWidth(col, cfg.width);
                }


                // delegate
                if (cfg.delegate)
                {
                    table->setItemDelegateForColumn(col, cfg.delegate);
                }
                // ===== 隐藏列 =====
                table->setColumnHidden(
                    col,
                    !cfg.visible);
            }
        }
        // ==============================
        // 6. 点击复选框时选中/取消整行
        // ==============================
        connect(table, &QTableView::clicked, this, [=](const QModelIndex& index)
                {
                    if (!index.isValid())
                        return;

                    // checkbox列
                    if (index.column() == 0)
                    {
                        auto state = index.data(Qt::CheckStateRole);

                        // 已勾选
                        if (state == Qt::Checked)
                        {
                            table->selectRow(index.row());
                        }
                        else
                        {
                            table->selectionModel()->select( index,
                                                            QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
                        }
                    }
                });


        // 整行选中（点击任意单元格时选中整行）
        //  table->setSelectionBehavior(QAbstractItemView::SelectRows);
        // 单选模式（一次只能选中一行）
        // table->setSelectionMode( QAbstractItemView::SingleSelection);//批量check--NoSelection，ExtendedSelection
        // 禁止编辑单元格
        // 表格变成纯展示模式
        table->setEditTriggers( QAbstractItemView::NoEditTriggers);
        // 开启斑马纹（隔行颜色）
        table->setAlternatingRowColors(true);
        // 最后一列自动拉伸填满剩余空间
        // table->horizontalHeader()->setStretchLastSection(true);
        // 横向滚动按像素平滑滚动
        table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        // 根据需要显示横向滚动条
        table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        // 隐藏左侧垂直表头（默认行号）
        table->verticalHeader()->hide();
        // 设置列最小宽度
        // 防止列太窄导致内容挤压
        table->horizontalHeader()->setMinimumSectionSize(60);
        // 表头文字居中
        table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

        // 禁止自动换行
        table->setWordWrap(false);
        // 文本过长时显示省略号 ...
        table->setTextElideMode(Qt::ElideRight);


        m_stack->addWidget(table);
        m_tables.append(table);
        m_proxies.append(proxy);
        qDebug() << __FUNCTION__ << __FUNCTION__ <<"Header class:"
                 << table->horizontalHeader()
                        ->metaObject()
                        ->className();
    }
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


