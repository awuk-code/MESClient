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
enum class PageDisplayType{
    TABLE,
    PDF,
    NORMAL
};
Q_DECLARE_METATYPE(PageDisplayType)



struct TabConfig
{
    QString title;      // Tab 标题（显示给用户）
    PageDisplayType PageDisplayType = PageDisplayType::TABLE;
    QVariant data;      // 扩展数据（业务参数）
    QWidget* page = nullptr;
};

//筛选状态：
enum class FilterStatus{
    UNKNOWN = -1, //未知状态
    WAITWORK = 0, //待开工
    ALREADY = 1,    //已开工
    FINISH = 2,     //已完工

    //维修站
    PENDING = 3,        //待处理
    REVIEW = 4,         //审核中
    WAITRETURN = 5,     //待返工
    PROCESSED = 6       //已处理
};
Q_DECLARE_METATYPE(FilterStatus)

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
    ImageLink,       // 显示“查看图片”，点击后弹出图片预览
    TextLink        //文本链接
};

struct ColumnConfig
{
    QString title;      // 列标题（显示在表头上的文字）
    QString field;      // 字段名（程序内部唯一标识，如 taskNo、priority）

    int width = 120;    // 列宽（单位：像素）

    bool visible = true;        // 是否显示该列
    bool editable = false;      // 单元格是否允许编辑
    bool fixedWidth = false;    // 是否固定宽度（最大化时不参与比例缩放）

    Qt::Alignment alignment = Qt::AlignCenter;   // 文本对齐方式
    ColumnType type = ColumnType::Normal;        // 列类型

    QHeaderView::ResizeMode resizeMode =
        QHeaderView::ResizeToContents;

    FilterType filterType = FilterType::None;

    QStyledItemDelegate* delegate = nullptr;
};

using TabConfigs    = QVector<TabConfig>;
using ColumnConfigs = QVector<ColumnConfig>;

enum class LinkType{
    Page,
    Image
};

struct LinkData
{
    LinkType type;
    QString text;   //点击的文本
    QString target; //跳转的目标
};
Q_DECLARE_METATYPE(LinkData)

#endif // COMMONFUNC_H
