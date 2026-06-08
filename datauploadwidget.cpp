#include "datauploadwidget.h"
#include "ui_datauploadwidget.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>

namespace
{
constexpr int DocumentRootIndex = 0;
constexpr int PhotoRootIndex = 1;
}

DataUPloadWidget::DataUPloadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataUPloadWidget)
{
    ui->setupUi(this);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QTreeWidgetItem* itemDoc = new QTreeWidgetItem(QStringList() << tr("文档"));
    QTreeWidgetItem* itemImg = new QTreeWidgetItem(QStringList() << tr("图片"));
    itemDoc->setData(0, Qt::UserRole, DocumentRootIndex);
    itemImg->setData(0, Qt::UserRole, PhotoRootIndex);

    ui->treeWidget->addTopLevelItem(itemDoc);
    ui->treeWidget->addTopLevelItem(itemImg);

    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested,
            this, &DataUPloadWidget::showTreeContextMenu);
}

DataUPloadWidget::~DataUPloadWidget()
{
    delete ui;
}

QString DataUPloadWidget::productWeight() const
{
    return ui->product_Weight->text();
}

QString DataUPloadWidget::productSize() const
{
    return ui->product_Size->text();
}

QString DataUPloadWidget::productPower() const
{
    return ui->product_Power->text();
}

void DataUPloadWidget::setProductSn(const QString& productSn)
{
    m_productSn = productSn.trimmed();
}

bool DataUPloadWidget::isComplete(QString* message) const
{
    if (!hasChildren(rootItem(DocumentRootIndex)))
    {
        if (message)
            *message = tr("资料上传未完成：请先添加文档。");
        return false;
    }

    if (!hasChildren(rootItem(PhotoRootIndex)))
    {
        if (message)
            *message = tr("资料上传未完成：请先添加照片。");
        return false;
    }

    return true;
}

void DataUPloadWidget::showTreeContextMenu(const QPoint& pos)
{
    QTreeWidgetItem* item = ui->treeWidget->itemAt(pos);
    if (!item)
        return;

    QMenu menu(this);
    if (!item->parent())
    {
        const int index = item->data(0, Qt::UserRole).toInt();
        if (index == DocumentRootIndex)
        {
            menu.addAction(tr("添加文件"), this, &DataUPloadWidget::addDocuments);
            menu.addAction(tr("清空文件"), this, [this, item]() { clearChildren(item); });
            menu.addAction(tr("上传全部文档"), this, [this, item]() {
                uploadChildren(item, tr("当前没有可上传的文档。"));
            });
        }
        else if (index == PhotoRootIndex)
        {
            menu.addAction(tr("添加照片"), this, &DataUPloadWidget::addPhotos);
            menu.addAction(tr("打开相机"), this, &DataUPloadWidget::openCamera);
            menu.addAction(tr("清空照片"), this, [this, item]() { clearChildren(item); });
            menu.addAction(tr("上传全部照片"), this, [this, item]() {
                uploadChildren(item, tr("当前没有可上传的照片。"));
            });
        }
    }
    else
    {
        menu.addAction(tr("上传"), this, [this, item]() { uploadItem(item); });
        menu.addAction(tr("移除"), this, [this, item]() { removeItem(item); });
    }

    if (!menu.actions().isEmpty())
        menu.exec(ui->treeWidget->viewport()->mapToGlobal(pos));
}

QTreeWidgetItem* DataUPloadWidget::rootItem(int index) const
{
    return ui->treeWidget->topLevelItem(index);
}

void DataUPloadWidget::addFiles(
    QTreeWidgetItem* parentItem,
    const QString& title,
    const QString& filter)
{
    if (!parentItem)
        return;

    const QStringList files = QFileDialog::getOpenFileNames(this, title, QString(), filter);
    for (const QString& file : files)
    {
        const QString path = QFileInfo(file).absoluteFilePath();
        if (path.isEmpty())
            continue;

        bool exists = false;
        for (int i = 0; i < parentItem->childCount(); ++i)
            exists = exists || parentItem->child(i)->text(0) == path;

        if (!exists)
            parentItem->addChild(new QTreeWidgetItem(QStringList() << path));
    }

    parentItem->setExpanded(true);
}

void DataUPloadWidget::addDocuments()
{
    addFiles(rootItem(DocumentRootIndex),
             tr("添加文件"),
             tr("文档文件 (*.doc *.docx *.xls *.xlsx *.pdf *.txt);;所有文件 (*.*)"));
}

void DataUPloadWidget::addPhotos()
{
    addFiles(rootItem(PhotoRootIndex),
             tr("添加照片"),
             tr("图片文件 (*.png *.jpg *.jpeg *.bmp);;所有文件 (*.*)"));
}

void DataUPloadWidget::clearChildren(QTreeWidgetItem* parentItem)
{
    if (parentItem)
        qDeleteAll(parentItem->takeChildren());
}

void DataUPloadWidget::uploadItem(QTreeWidgetItem* item)
{
    if (!item || !item->parent() || item->text(0).isEmpty())
        return;

    qDebug() << __FUNCTION__ << "productSN:" << m_productSn << "file:" << item->text(0);
    QMessageBox::information(this, tr("资料上传"), tr("已加入上传队列：%1").arg(item->text(0)));
}

void DataUPloadWidget::uploadChildren(QTreeWidgetItem* parentItem, const QString& emptyMessage)
{
    if (!parentItem || parentItem->childCount() == 0)
    {
        QMessageBox::information(this, tr("资料上传"), emptyMessage);
        return;
    }

    for (int i = 0; i < parentItem->childCount(); ++i)
        uploadItem(parentItem->child(i));
}

void DataUPloadWidget::removeItem(QTreeWidgetItem* item)
{
    if (item && item->parent())
        delete item->parent()->takeChild(item->parent()->indexOfChild(item));
}

void DataUPloadWidget::openCamera()
{
    QMessageBox::information(this, tr("打开相机"), tr("相机功能待接入。"));
}

bool DataUPloadWidget::hasChildren(QTreeWidgetItem* parentItem) const
{
    return parentItem && parentItem->childCount() > 0;
}
