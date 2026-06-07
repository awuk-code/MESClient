#ifndef DATAUPLOADWIDGET_H
#define DATAUPLOADWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
class DataUPloadWidget;
}

class DataUPloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataUPloadWidget(QWidget *parent = nullptr);
    ~DataUPloadWidget();
    QString productWeight()const;
    QString productSize()const;
    QString productPower()const;
    void setProductSn(const QString& productSn);
    bool isComplete(QString* message = nullptr) const;

private slots:
    void showTreeContextMenu(const QPoint& pos);

private:
    QTreeWidgetItem* rootItem(int index) const;
    void addFiles(QTreeWidgetItem* parentItem, const QString& title, const QString& filter);
    void addDocuments();
    void addPhotos();
    void clearChildren(QTreeWidgetItem* parentItem);
    void uploadItem(QTreeWidgetItem* item);
    void uploadChildren(QTreeWidgetItem* parentItem, const QString& emptyMessage);
    void removeItem(QTreeWidgetItem* item);
    void openCamera();
    bool hasChildren(QTreeWidgetItem* parentItem) const;

    Ui::DataUPloadWidget *ui;
    QString m_productSn;
};

#endif // DATAUPLOADWIDGET_H
