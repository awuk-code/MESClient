#ifndef BASEDIALOGWIDGET_H
#define BASEDIALOGWIDGET_H

#include <QDialog>

class QLabel;
class QPushButton;
class QWidget;
class QVBoxLayout;

class BaseDialogWidget : public QDialog
{
    Q_OBJECT

public:
    explicit BaseDialogWidget(QWidget *parent = nullptr);
    virtual ~BaseDialogWidget() = default;

    // 设置标题
    void setTitle(const QString &title);

    // 获取中间内容布局，供子类添加控件
    QVBoxLayout* contentLayout() const;

    // 获取按钮
    QPushButton* confirmButton() const;
    QPushButton* cancelButton() const;

protected:
    // 子类可重写，返回 true 表示关闭对话框
    virtual bool onConfirm();

private slots:
    void onConfirmBtnClicked();
    void onCancelBtnClicked();

private:
    void initUI();
    void initConnections();

private:
    // 主布局
    // 标题栏
    QLabel* m_titleLabel = nullptr;

    // 内容区
    QVBoxLayout* m_contentLayout = nullptr;

    // 底部按钮区
    // 按钮
    QPushButton* m_confirmButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
};

#endif // BASEDIALOGWIDGET_H
