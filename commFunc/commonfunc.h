#ifndef COMMONFUNC_H
#define COMMONFUNC_H

#include <QString>
#include <QVariant>
#include <QVector>
#include <QStyledItemDelegate>
#include <QHeaderView>

// ============================================================
// 常量字段定义
// ============================================================
// 这些字段不一定显示在表格中，但会用于业务逻辑判断。
// 例如 OperationDelegate 根据这些字段决定按钮文字和状态。

// 是否已打印标签（bool）
constexpr auto FIELD_LABEL_PRINTED = "labelPrinted";

// 是否已开工（bool）
constexpr auto FIELD_STARTED = "started";


// ============================================================
// Tab 配置
// ============================================================
// 用于描述一个页签（Tab）的基本信息。
// data 可存储任意附加数据，例如：
//   0 / 1 / 2                 -> 状态值
//   QString("pdf")            -> 页面类型
//   QVariantMap({...})        -> 更复杂的业务参数
//
// 示例：
// {
//     "未开工",
//     0
// }
struct TabConfig
{
    QString title;      // Tab 标题（显示给用户）
    QVariant data;      // 扩展数据（业务参数）
};


// ============================================================
// 筛选类型
// ============================================================
// 用于统一描述所有筛选方式。
// 包括：顶部查询区域、表头筛选按钮等。
enum class FilterType
{
    None,       // 无筛选

    // 顶部查询区域
    Status,     // 状态筛选（下拉框）
    Keyword,    // 关键字筛选（文本输入框）

    // 表头筛选
    Date,       // 日期筛选（点击表头按钮弹出日历）
    Priority    // 优先级筛选（点击表头按钮弹出优先级菜单）
};


// ============================================================
// 列类型
// ============================================================
// 描述单元格的显示方式和交互方式。
enum class ColumnType
{
    Normal,         // 普通文本
    CheckBox,       // 复选框
    RowNumber,      // 行号
    Operation,      // 操作按钮（打印、开工、编辑、删除等）
    LineEdit,       // 输入框
    ImageLink       // 显示“查看图片”，点击后弹出图片预览
};


// ============================================================
// 列配置
// ============================================================
// 用于描述表格中一列的完整属性。
//
// 初始化顺序：
// {
//     title,       // 列标题
//     field,       // 字段名
//     width,       // 列宽
//     visible,     // 是否显示
//     editable,    // 是否可编辑
//     fixedWidth,  // 是否固定宽度
//     alignment,   // 对齐方式
//     type,        // 列类型
//     resizeMode,  // 列宽调整模式
//     filterType,  // 筛选类型
//     delegate     // 自定义代理
// }
//
// 示例：
// {
//     "优先级",
//     "priority",
//     100,
//     true,
//     false,
//     true,
//     Qt::AlignCenter,
//     ColumnType::Normal,
//     QHeaderView::Fixed,
//     FilterType::Priority
// }
struct ColumnConfig
{
    // --------------------------------------------------------
    // 基础信息
    // --------------------------------------------------------
    QString title;      // 列标题（显示在表头上的文字）
    QString field;      // 字段名（程序内部唯一标识，如 taskNo、priority）

    // --------------------------------------------------------
    // 显示属性
    // --------------------------------------------------------
    int width = 120;    // 列宽（单位：像素）

    bool visible = true;        // 是否显示该列
    bool editable = false;      // 单元格是否允许编辑
    bool fixedWidth = false;    // 是否固定宽度（最大化时不参与比例缩放）

    // --------------------------------------------------------
    // 单元格显示方式
    // --------------------------------------------------------
    Qt::Alignment alignment = Qt::AlignCenter;   // 文本对齐方式
    ColumnType type = ColumnType::Normal;        // 列类型

    // --------------------------------------------------------
    // 列宽调整模式
    // --------------------------------------------------------
    // QHeaderView::Fixed
    //     固定宽度，不自动变化
    //
    // QHeaderView::ResizeToContents
    //     根据内容自动调整
    //
    // QHeaderView::Stretch
    //     自动拉伸填满剩余空间
    QHeaderView::ResizeMode resizeMode =
        QHeaderView::ResizeToContents;

    // --------------------------------------------------------
    // 表头筛选类型
    // --------------------------------------------------------
    // FilterType::None
    //     无筛选功能
    //
    // FilterType::Date
    //     点击表头按钮弹出日历
    //
    // FilterType::Priority
    //     点击表头按钮弹出优先级菜单
    FilterType filterType = FilterType::None;

    // --------------------------------------------------------
    // 自定义代理
    // --------------------------------------------------------
    // nullptr
    //     不使用代理
    //
    // m_checkBoxDelegate
    //     复选框代理
    //
    // opDelegate
    //     操作列代理
    //
    // imageDelegate
    //     图片查看代理
    QStyledItemDelegate* delegate = nullptr;
};


// ============================================================
// 类型别名
// ============================================================
// TabConfigs
//     等价于 QVector<TabConfig>
//
// ColumnConfigs
//     等价于 QVector<ColumnConfig>
using TabConfigs    = QVector<TabConfig>;
using ColumnConfigs = QVector<ColumnConfig>;

#endif // COMMONFUNC_H
