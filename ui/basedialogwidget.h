#ifndef BASEDIALOGWIDGET_H
#define BASEDIALOGWIDGET_H

#include <QDialog>
#include <QPoint>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QWidget;
class QMouseEvent;

class BaseDialogWidget : public QDialog
{
    Q_OBJECT

public:
    explicit BaseDialogWidget(QWidget *parent = nullptr);
    virtual ~BaseDialogWidget() = default;

    void setTitle(const QString &title);

    // 子类只需要向内容区添加自己的控件，标题区和按钮区由基类统一维护。
    QVBoxLayout* contentLayout() const;

    QPushButton* confirmButton() const;
    QPushButton* cancelButton() const;

protected:
    // 子类重写这个函数处理确认按钮逻辑，返回 true 表示关闭弹窗。
    virtual bool onConfirm();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onConfirmButtonClicked();
    void onCancelButtonClicked();
    void onCloseButtonClicked();

private:
    void initUI();
    void initConnections();
    bool isInTitleBar(const QPoint& pos) const;

private:
    QWidget* m_titleBar{nullptr};
    QLabel* m_titleLabel{nullptr};
    QPushButton* m_closeButton{nullptr};

    QVBoxLayout* m_contentLayout{nullptr};

    QPushButton* m_confirmButton{nullptr};
    QPushButton* m_cancelButton{nullptr};

    bool m_dragging{false};
    QPoint m_dragPos;
};

#endif // BASEDIALOGWIDGET_H
