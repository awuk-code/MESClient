#include "basepagewidget.h"
#include "fieldfilterproxymodel.h"
#include "basetablemodel.h"

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
    {
        m_tabBar->hide();
    }
    for (int i = 0; i < tabs.size(); ++i)
    {
        // Tab按钮
        m_tabBar->addTab(tabs[i].title);

        QTableView* table = new QTableView;

        auto proxy = createProxy(tabs[i].status);
        if (!proxy || !m_model) continue;

        proxy->setSourceModel(m_model);
        table->setModel(proxy);

        auto baseModel = qobject_cast<BaseTableModel*>(m_model);

        if (baseModel)
        {
            const auto& columns = baseModel->columns();

            for (int col = 0; col < columns.size(); ++col)
            {
                const auto& cfg = columns[col];

                // 列宽
                table->setColumnWidth(col, cfg.width);

                // delegate
                if (cfg.delegate)
                {
                    table->setItemDelegateForColumn(col, cfg.delegate);
                }
            }
        }




        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        connect(table,
                &QTableView::clicked,
                this,
                [=](const QModelIndex& index)
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
                            table->selectionModel()->select(
                                index,
                                QItemSelectionModel::Deselect |
                                    QItemSelectionModel::Rows);
                        }
                    }
                });
        table->setSelectionMode(
            QAbstractItemView::SingleSelection);

        table->setEditTriggers(
            QAbstractItemView::NoEditTriggers);

        table->setAlternatingRowColors(true);

        table->setShowGrid(false);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        table->horizontalHeader()->setStretchLastSection(false);
        table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        table->verticalHeader()->hide();

        m_stack->addWidget(table);
        m_tables.append(table);
        m_proxies.append(proxy);
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

                for (auto proxy : m_proxies)
                {
                    proxy->setKeyword(text);
                }
            });



}


