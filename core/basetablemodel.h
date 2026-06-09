#ifndef BASETABLEMODEL_H
#define BASETABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QVariantMap>

#include "commonfunc.h"

class BaseTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BaseTableModel(QObject *parent = nullptr);

    /**
 * @brief 获取指定行的完整数据。
 *
 * 返回一个 QVariantMap：
 * - key：字段名（如 "taskNo"、"status"、"priority"）
 * - value：该字段对应的数据
 *
 * 使用示例：
 * @code
 * QVariantMap row = model->rowData(0);
 * QString status = row.value("status").toString();
 * QString priority = row.value("priority").toString();
 * @endcode
 *
 * @param row 行号（从 0 开始）
 * @return 指定行的数据映射
 */
    QVariantMap rowData(int row) const;

    // 设置数据
    void setRows(const QVector<QVariantMap>& rows);

    // 获取当前表格完整数据，提交前用于组合上传 JSON。
    QVector<QVariantMap> rows() const;

    // 获取列配置
    const QVector<ColumnConfig>& columns() const;


protected:
    // =========================================================
    // 单元格编辑后的扩展点
    //
    // 作用：
    // 子类可重写该函数，在某个字段被编辑后，
    // 自动修改同一行中的其他字段。
    //
    // 典型场景：
    // 1. 输入物料标签码 -> 自动带出 EPR 编码、批次号
    // 2. 输入物料 SN   -> 自动带出批次号
    // 3. 输入数量      -> 自动计算金额
    //
    // 参数：
    // row   : 当前编辑的行号
    // field : 当前编辑的字段名（如 "materialLabelCode"）
    // value : 用户输入的新值
virtual void afterCellEdited(int row,
        const QString& field,
        const QVariant& value);

    QVector<ColumnConfig> m_columns;
    QVector<QVariantMap> m_rows;

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;

    QSet<int> m_checkedRows;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role) override;

    QString columnField(int column) const;

};

#endif // BASETABLEMODEL_H
