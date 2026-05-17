#ifndef TOGGLESWITCHWIDGET_H
#define TOGGLESWITCHWIDGET_H

#include <QWidget>
#include <QEvent>

class ToggleSwitchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToggleSwitchWidget(QWidget *parent = nullptr);
    // 状态
    bool isChecked() const;
    void setChecked(bool checked);

    // 设置固定边长（正方形）
    void setSideLength(int size);

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    QSize sizeHint() const override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
private:
    bool m_checked = false;
    bool m_hovered = false;
    int  m_sideLength = 24;
};

#endif // TOGGLESWITCHWIDGET_H
