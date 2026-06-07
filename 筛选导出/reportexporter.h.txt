#ifndef REPORTEXPORTER_H
#define REPORTEXPORTER_H

#include <QObject>

class QTableView;
class QWidget;

class ReportExporter : public QObject
{
    Q_OBJECT

public:
    explicit ReportExporter(QObject* parent = nullptr);

    static void exportTable(QWidget* parent, QTableView* table);
};

#endif // REPORTEXPORTER_H
