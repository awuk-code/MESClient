#ifndef COMMONFUNC_H
#define COMMONFUNC_H

#include <QString>
#include <QStyledItemDelegate>
#include <QHeaderView>

struct TabConfig
{
    QString title;   // Tab标题
    int status;      // 对应状态值
};

enum class ColumnType
{
    Normal,
    CheckBox,
    RowNumber,
    Operation,
    Image
};

struct ColumnConfig
{
    QString title;      // 列标题
    QString field;      // 字段字段名（核心）

    int width = 120;    // 列宽

    bool visible = true;

    Qt::Alignment alignment = Qt::AlignCenter;
    ColumnType type = ColumnType::Normal;
    // ⭐ 每列独立Resize模式
    QHeaderView::ResizeMode resizeMode =
        QHeaderView::ResizeToContents;
    QStyledItemDelegate* delegate = nullptr;
};

class CommonFunc
{
public:
    CommonFunc();
};

#endif // COMMONFUNC_H
