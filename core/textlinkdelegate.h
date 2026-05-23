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

    void pageLinkClicked(const QString& pageId);

    void imageLinkClicked(const QString& imagePath);

private:
    QRect textRect(
        const QStyleOptionViewItem& option,
        const QString& text) const;
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
