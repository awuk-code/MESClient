#include "basepagewidget.h"
#include "fieldfilterproxymodel.h"
#include "basetablemodel.h"
#include "headeroverlaywidget.h"
#include "imageviewwidget.h"
#include "operationdelegate.h"

#include <QDateTime>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontMetrics>
#include <QMessageBox>
#include <QFrame>
#include <QStandardPaths>
#include <QScrollBar>
#include <QTimer>
#include <QTextStream>

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
    for (QWidget* page : std::as_const(m_pages))
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

    m_exportBtn = new QPushButton(tr("导出报表"), this);
    m_exportBtn->setProperty("buttonRole", "export");
    layout->addWidget(m_exportBtn);
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
    if (m_exportBtn &&
        m_exportBtn->property("buttonRole").toString() == "export")
    {
        connect(m_exportBtn, &QPushButton::clicked, this,
                &BasePageWidget::exportCurrentTableToExcel);
    }

    connect(this, &BasePageWidget::sigPageSwitching, this, &BasePageWidget::onPageLinkClicked);
    connect(this, &BasePageWidget::sigIMGView, this, &BasePageWidget::onImageLinkClicked);

}

void BasePageWidget::exportCurrentTableToExcel()
{
    auto table =
        qobject_cast<QTableView*>(m_stack ? m_stack->currentWidget() : nullptr);
    if (!table || !table->model())
    {
        QMessageBox::information(this, tr("导出报表"), tr("当前页面没有可导出的表格。"));
        return;
    }

    const QString defaultDir =
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    const QString baseName =
        tr("%1/报表_%2")
            .arg(defaultDir)
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    QString selectedFilter;
    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("导出报表"),
        baseName + ".xls",
        tr("Excel 97-2003 文件 (*.xls);;CSV 文件 (*.csv);;Excel 工作簿 (*.xlsx)"),
        &selectedFilter);
    if (fileName.isEmpty())
        return;

    QString exportFileName = fileName;
    QString exportType = QFileInfo(exportFileName).suffix().toLower();
    if (exportType != "xls" && exportType != "csv" && exportType != "xlsx")
    {
        if (selectedFilter.contains("*.csv"))
            exportType = "csv";
        else if (selectedFilter.contains("*.xlsx"))
            exportType = "xlsx";
        else
            exportType = "xls";
    }

    if (!exportFileName.endsWith("." + exportType, Qt::CaseInsensitive))
        exportFileName += "." + exportType;

    auto isExportColumn = [table](int col) {
        if (table->isColumnHidden(col))
            return false;

        const QString header =
            table->model()->headerData(col, Qt::Horizontal, Qt::DisplayRole)
                .toString();

        if (header == QObject::tr("选择"))
            return false;

        for (int row = 0; row < table->model()->rowCount(); ++row)
        {
            if (table->model()->index(row, col)
                    .data(Qt::CheckStateRole)
                    .isValid())
                return false;
        }

        return true;
    };

    auto exportColumns = [&]() {
        QVector<int> columns;
        for (int col = 0; col < table->model()->columnCount(); ++col)
        {
            if (isExportColumn(col))
                columns.append(col);
        }
        return columns;
    };

    const QVector<int> columns = exportColumns();

    auto xmlText = [](const QString& text) {
        QString value = text;
        value.replace("&", "&amp;");
        value.replace("<", "&lt;");
        value.replace(">", "&gt;");
        value.replace("\"", "&quot;");
        value.replace("'", "&apos;");
        return value;
    };

    auto columnName = [](int index) {
        QString name;
        int value = index + 1;
        while (value > 0)
        {
            const int remainder = (value - 1) % 26;
            name.prepend(QChar('A' + remainder));
            value = (value - 1) / 26;
        }
        return name;
    };

    auto crc32 = [](const QByteArray& data) {
        quint32 crc = 0xFFFFFFFFu;
        for (uchar byte : data)
        {
            crc ^= byte;
            for (int i = 0; i < 8; ++i)
                crc = (crc >> 1) ^ (0xEDB88320u & (-(int)(crc & 1)));
        }
        return crc ^ 0xFFFFFFFFu;
    };

    auto append16 = [](QByteArray& data, quint16 value) {
        data.append(char(value & 0xFF));
        data.append(char((value >> 8) & 0xFF));
    };

    auto append32 = [](QByteArray& data, quint32 value) {
        data.append(char(value & 0xFF));
        data.append(char((value >> 8) & 0xFF));
        data.append(char((value >> 16) & 0xFF));
        data.append(char((value >> 24) & 0xFF));
    };

    auto makeXlsx = [&]() {
        struct ZipEntry
        {
            QString name;
            QByteArray data;
            quint32 crc = 0;
            quint32 offset = 0;
        };

        QVector<ZipEntry> entries;

        auto addEntry = [&](const QString& name, const QByteArray& data) {
            ZipEntry entry;
            entry.name = name;
            entry.data = data;
            entry.crc = crc32(data);
            entries.append(entry);
        };

        QByteArray worksheet;
        QTextStream sheetOut(&worksheet);
        sheetOut.setEncoding(QStringConverter::Utf8);
        sheetOut << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
        sheetOut << "<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">";
        sheetOut << "<sheetData>";

        auto writeXlsxRow = [&](int excelRow, int modelRow) {
            sheetOut << "<row r=\"" << excelRow << "\">";
            for (int exportCol = 0; exportCol < columns.size(); ++exportCol)
            {
                const int modelCol = columns[exportCol];
                const QString cellRef = columnName(exportCol) + QString::number(excelRow);
                const QString text =
                    modelRow < 0
                        ? table->model()->headerData(modelCol, Qt::Horizontal, Qt::DisplayRole).toString()
                        : table->model()->index(modelRow, modelCol).data(Qt::DisplayRole).toString();
                sheetOut << "<c r=\"" << cellRef << "\" t=\"inlineStr\"><is><t>"
                         << xmlText(text)
                         << "</t></is></c>";
            }
            sheetOut << "</row>";
        };

        writeXlsxRow(1, -1);
        for (int row = 0; row < table->model()->rowCount(); ++row)
            writeXlsxRow(row + 2, row);

        sheetOut << "</sheetData></worksheet>";
        sheetOut.flush();

        addEntry("[Content_Types].xml",
                 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                 "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
                 "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
                 "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
                 "<Override PartName=\"/xl/workbook.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>"
                 "<Override PartName=\"/xl/worksheets/sheet1.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>"
                 "</Types>");
        addEntry("_rels/.rels",
                 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                 "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
                 "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"xl/workbook.xml\"/>"
                 "</Relationships>");
        addEntry("xl/workbook.xml",
                 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                 "<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" "
                 "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\">"
                 "<sheets><sheet name=\"Sheet1\" sheetId=\"1\" r:id=\"rId1\"/></sheets>"
                 "</workbook>");
        addEntry("xl/_rels/workbook.xml.rels",
                 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
                 "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
                 "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/sheet1.xml\"/>"
                 "</Relationships>");
        addEntry("xl/worksheets/sheet1.xml", worksheet);

        QByteArray zip;
        for (ZipEntry& entry : entries)
        {
            const QByteArray name = entry.name.toUtf8();
            entry.offset = zip.size();

            append32(zip, 0x04034b50);
            append16(zip, 20);
            append16(zip, 0);
            append16(zip, 0);
            append16(zip, 0);
            append16(zip, 0);
            append32(zip, entry.crc);
            append32(zip, entry.data.size());
            append32(zip, entry.data.size());
            append16(zip, name.size());
            append16(zip, 0);
            zip.append(name);
            zip.append(entry.data);
        }

        const quint32 centralOffset = zip.size();
        for (const ZipEntry& entry : entries)
        {
            const QByteArray name = entry.name.toUtf8();
            append32(zip, 0x02014b50);
            append16(zip, 20);
            append16(zip, 20);
            append16(zip, 0);
            append16(zip, 0);
            append16(zip, 0);
            append16(zip, 0);
            append32(zip, entry.crc);
            append32(zip, entry.data.size());
            append32(zip, entry.data.size());
            append16(zip, name.size());
            append16(zip, 0);
            append16(zip, 0);
            append16(zip, 0);
            append16(zip, 0);
            append32(zip, 0);
            append32(zip, entry.offset);
            zip.append(name);
        }

        const quint32 centralSize = zip.size() - centralOffset;
        append32(zip, 0x06054b50);
        append16(zip, 0);
        append16(zip, 0);
        append16(zip, entries.size());
        append16(zip, entries.size());
        append32(zip, centralSize);
        append32(zip, centralOffset);
        append16(zip, 0);

        return zip;
    };

    if (exportType == "xlsx")
    {
        QFile file(exportFileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this, tr("导出报表"), tr("文件打开失败，无法导出。"));
            return;
        }

        file.write(makeXlsx());
        file.close();
        QMessageBox::information(this, tr("导出报表"), tr("导出完成。"));
        return;
    }

    QFile file(exportFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("导出报表"), tr("文件打开失败，无法导出。"));
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    auto csvText = [](const QString& text) {
        QString value = text;
        value.replace("\"", "\"\"");
        if (value.contains(',') || value.contains('"') ||
            value.contains('\n') || value.contains('\r'))
            value = "\"" + value + "\"";
        return value;
    };

    auto writeCsvRow = [&](int row) {
        QStringList values;
        for (int col : columns)
        {
            const QString text =
                row < 0
                    ? table->model()->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString()
                    : table->model()->index(row, col).data(Qt::DisplayRole).toString();
            values << csvText(text);
        }
        out << values.join(",") << "\n";
    };

    if (exportType == "csv")
    {
        out << QChar(0xFEFF);
        writeCsvRow(-1);
        for (int row = 0; row < table->model()->rowCount(); ++row)
            writeCsvRow(row);

        file.close();
        QMessageBox::information(this, tr("导出报表"), tr("导出完成。"));
        return;
    }

    // xls 使用 Excel 可直接打开的 HTML 表格格式。
    out << "<html><head><meta charset=\"UTF-8\"></head><body><table border=\"1\">\n";
    out << "<tr>";
    for (int col : columns)
    {
        const QString header =
            table->model()->headerData(col, Qt::Horizontal, Qt::DisplayRole)
                .toString()
                .toHtmlEscaped();
        out << "<th>" << header << "</th>";
    }
    out << "</tr>\n";

    for (int row = 0; row < table->model()->rowCount(); ++row)
    {
        out << "<tr>";
        for (int col : columns)
        {
            const QString text =
                table->model()->index(row, col)
                    .data(Qt::DisplayRole)
                    .toString()
                    .toHtmlEscaped();
            out << "<td>" << text << "</td>";
        }
        out << "</tr>\n";
    }

    out << "</table></body></html>\n";
    file.close();

    QMessageBox::information(this, tr("导出报表"), tr("导出完成。"));
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

    table->installEventFilter(this);
    connect(proxy, &QAbstractItemModel::modelReset, this, [=]() {
        QTimer::singleShot(0, this, [=]() { applyAdaptiveColumnWidths(table); });
    });
    connect(proxy, &QAbstractItemModel::layoutChanged, this, [=]() {
        QTimer::singleShot(0, this, [=]() { applyAdaptiveColumnWidths(table); });
    });
    connect(proxy, &QAbstractItemModel::rowsInserted, this, [=]() {
        QTimer::singleShot(0, this, [=]() { applyAdaptiveColumnWidths(table); });
    });
    connect(proxy, &QAbstractItemModel::rowsRemoved, this, [=]() {
        QTimer::singleShot(0, this, [=]() { applyAdaptiveColumnWidths(table); });
    });
    connect(proxy, &QAbstractItemModel::dataChanged, this, [=]() {
        QTimer::singleShot(0, this, [=]() { applyAdaptiveColumnWidths(table); });
    });

    // 8. 按内容自适应列宽；空间足够时自动铺满表格区域。
    QTimer::singleShot(0, this, [=]() { applyAdaptiveColumnWidths(table); });
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

int BasePageWidget::contentWidthForColumn(QTableView* table, int column) const
{
    if (!table || !table->model() || !m_model)
        return 0;

    auto baseModel = qobject_cast<BaseTableModel*>(m_model);
    if (!baseModel)
        return 0;

    const auto& columns = baseModel->columns();
    if (column < 0 || column >= columns.size())
        return 0;

    const auto& cfg = columns[column];
    const int padding = 28;

    QFontMetrics headerFm(table->horizontalHeader()->font());
    const QString headerText =
        table->model()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
    int width = headerFm.horizontalAdvance(headerText) + padding;

    if (cfg.filterType == FilterType::Date ||
        cfg.filterType == FilterType::Priority)
    {
        width += 18;
    }

    if (cfg.type == ColumnType::CheckBox)
    {
        width = qMax(width, 44);
    }
    else if (cfg.type == ColumnType::Operation)
    {
        width = qMax(width, OperationDelegate::minimumColumnWidth());
    }

    QFontMetrics cellFm(table->font());
    for (int row = 0; row < table->model()->rowCount(); ++row)
    {
        const QModelIndex index = table->model()->index(row, column);
        QString text = index.data(Qt::DisplayRole).toString();

        if (text.isEmpty() && cfg.type == ColumnType::LineEdit)
            text = headerText.isEmpty() ? tr("请输入或扫入") : tr("请输入或扫入%1").arg(headerText);

        width = qMax(width, cellFm.horizontalAdvance(text) + padding);
    }

    return width;
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
        if (table->isColumnHidden(col) || !isColumnVisibleForTab(columns[col], tabData))
            continue;

        const int width = contentWidthForColumn(table, col);
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

        // 操作列只保证按钮完整显示，不参与额外空间分配，避免按钮区域被拉得过宽。
        if (extraWidth > 0 && columns[col].type != ColumnType::Operation && stretchableCount > 0)
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



