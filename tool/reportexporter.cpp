#include "reportexporter.h"

#include <QAbstractItemModel>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTableView>
#include <QTextStream>
#include <QVector>

ReportExporter::ReportExporter(QObject* parent)
    : QObject(parent)
{
}

namespace
{
bool isExportColumn(QTableView* table, int col)
{
    if (!table || !table->model() || table->isColumnHidden(col))
        return false;

    const QString header =
        table->model()->headerData(col, Qt::Horizontal, Qt::DisplayRole)
            .toString();

    if (header == ReportExporter::tr("选择"))
        return false;

    for (int row = 0; row < table->model()->rowCount(); ++row)
    {
        if (table->model()->index(row, col)
                .data(Qt::CheckStateRole)
                .isValid())
            return false;
    }

    return true;
}

QVector<int> exportColumns(QTableView* table)
{
    QVector<int> columns;
    if (!table || !table->model())
        return columns;

    for (int col = 0; col < table->model()->columnCount(); ++col)
    {
        if (isExportColumn(table, col))
            columns.append(col);
    }

    return columns;
}

QString csvText(const QString& text)
{
    QString value = text;
    value.replace("\"", "\"\"");
    if (value.contains(',') || value.contains('"') ||
        value.contains('\n') || value.contains('\r'))
        value = "\"" + value + "\"";
    return value;
}

bool writeCsv(QWidget* parent,
              const QString& fileName,
              QTableView* table,
              const QVector<int>& columns)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(
            parent,
            ReportExporter::tr("导出报表"),
            ReportExporter::tr("文件打开失败，无法导出。"));
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << QChar(0xFEFF);

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

    writeCsvRow(-1);
    for (int row = 0; row < table->model()->rowCount(); ++row)
        writeCsvRow(row);

    file.close();
    return true;
}
}

void ReportExporter::exportTable(QWidget* parent, QTableView* table)
{
    if (!table || !table->model())
    {
        QMessageBox::information(
            parent,
            tr("导出报表"),
            tr("当前页面没有可导出的表格。"));
        return;
    }

    const QString defaultDir =
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    const QString baseName =
        tr("%1/报表_%2")
            .arg(defaultDir)
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    QString fileName = QFileDialog::getSaveFileName(
        parent,
        tr("导出报表"),
        baseName + ".csv",
        tr("CSV 文件 (*.csv)"));
    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive))
        fileName += ".csv";

    const QVector<int> columns = exportColumns(table);
    const bool ok = writeCsv(parent, fileName, table, columns);

    if (ok)
        QMessageBox::information(parent, tr("导出报表"), tr("导出完成。"));
}
