#ifndef REPAIRJUDGEPAGE_H
#define REPAIRJUDGEPAGE_H

#include <QVariantMap>
#include <QWidget>

class QLabel;
class QGridLayout;
class QButtonGroup;
class QPushButton;
class QTextEdit;

class RepairJudgePage : public QWidget
{
    Q_OBJECT
public:
    explicit RepairJudgePage(QWidget *parent = nullptr);

    void setRepairData(const QVariantMap& rowData);

private:
    void initUI();
    void rebuildInfo();
    void clearInfo();
    QLabel* createTitleLabel(const QString& text);
    QLabel* createValueLabel(const QString& text);
    QPushButton* createImageButton(const QString& text, QWidget* parent = nullptr);
    QLabel* createImageLinkLabel(QWidget* parent = nullptr);
    QString displayText(const QString& field) const;

private:
    QVariantMap m_rowData;
    QWidget* m_infoWidget{nullptr};
    QGridLayout* m_infoLayout{nullptr};
    QPushButton* m_saveBtn{nullptr};
    QPushButton* m_backBtn{nullptr};
    QButtonGroup* m_methodGroup{nullptr};
    QTextEdit* m_judgeTextEdit{nullptr};
    QPushButton* m_submitBtn{nullptr};
    QWidget* m_contentWidget{nullptr};
};

#endif // REPAIRJUDGEPAGE_H
