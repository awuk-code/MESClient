#ifndef TEXTLINKDELEGATE_H
#define TEXTLINKDELEGATE_H

#include "baseitemdelegate.h"
#include <QStyledItemDelegate>
// 可点击文本代理
//
// 功能：
// 1. 将单元格文本绘制为红色
// 2. 鼠标点击时发出 linkClicked(row, text)
// 3. 不创建编辑器
class TextLinkDelegate : public BaseItemDelegate
{
    Q_OBJECT
public:
    explicit TextLinkDelegate(QObject *parent = nullptr);

signals:
    // row  : 当前点击的行号（代理模型中的行号）
    // text : 当前单元格显示内容
    void linkClicked(int row, const QString &text) const;

protected:
    // 自定义绘制
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    // 捕获鼠标点击
    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;
};

#endif // TEXTLINKDELEGATE_H
