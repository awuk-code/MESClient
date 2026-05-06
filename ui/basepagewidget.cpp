#include "basepagewidget.h"
#include "fieldfilterproxymodel.h"

BasePageWidget::BasePageWidget(QWidget *parent)
    : QWidget{parent}
{

}

void BasePageWidget::setupPage()
{
    initUI();
    if (m_titleLabel)
        m_titleLabel->setText(pageTitle());
}

void BasePageWidget::initUI()
{
    qDebug() << "BasePageWidget initUI";
    auto mainLayout = new QVBoxLayout(this);

    // ===== 1. 标题 =====
    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("PageTitle");
    mainLayout->addWidget(m_titleLabel);



    // ===== 搜索栏 =====
    auto searchLayout = new QHBoxLayout;
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("输入单号搜索......");

    searchLayout->addWidget(m_searchEdit);
    mainLayout->addLayout(searchLayout);

    // ===== Tab =====
    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);


    // ===== Model =====
    m_model = createModel();

    initTabs();
    bindSearch();
}

void BasePageWidget::initTabs()
{
    QStringList tabs = tabNames();

    for (int i = 0; i < tabs.size(); ++i)
    {
        QTableView* table = new QTableView;

        auto proxy = createProxy(i);
        if(!proxy){
            qDebug() <<"proxy is null";
            continue;
        }
        if (!m_model)
        {
            qDebug() << "model is null!";
            continue;
        }
        proxy->setSourceModel(m_model);

        table->setModel(proxy);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->horizontalHeader()->setStretchLastSection(true);

        m_tabWidget->addTab(table, tabs[i]);
        m_proxies.append(proxy);
    }
}

void BasePageWidget::bindSearch()
{
    connect(m_searchEdit, &QLineEdit::textChanged, this,
            [=](const QString& text)
            {
                for (auto proxy : m_proxies)
                {
                    proxy->setKeyword(text);
                }
            });
}
