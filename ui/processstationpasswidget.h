#ifndef PROCESSSTATIONPASSWIDGET_H
#define PROCESSSTATIONPASSWIDGET_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QRadioButton;
class QStandardItemModel;
class QTableView;

class ProcessStationPassWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProcessStationPassWidget(const QString& title, QWidget* parent = nullptr);

    bool isScanInMode() const;
    bool isPassMode() const;
    bool isNgMode() const;
    QString scanText() const;
    QTableView* statusTableView() const;
    QStandardItemModel* statusModel() const;

signals:
    void executeRequested();
    void pauseRequested();
    void resumeRequested();

private:
    void initUI(const QString& title);
    void initConnect();

private:
    QRadioButton* m_scanInRadio{nullptr};
    QRadioButton* m_passRadio{nullptr};
    QRadioButton* m_ngRadio{nullptr};
    QLineEdit* m_scanEdit{nullptr};
    QPushButton* m_executeBtn{nullptr};
    QPushButton* m_pauseBtn{nullptr};
    QPushButton* m_resumeBtn{nullptr};
    QTableView* m_statusTableView{nullptr};
    QStandardItemModel* m_statusModel{nullptr};
};

#endif // PROCESSSTATIONPASSWIDGET_H
