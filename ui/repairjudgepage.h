#ifndef REPAIRJUDGEPAGE_H
#define REPAIRJUDGEPAGE_H

#include <QVariantMap>
#include <QWidget>

class QLabel;
class QGridLayout;
class QButtonGroup;
class QComboBox;
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
    QVariantMap judgeData(const QString& reworkType = QString()) const;
    void onSaveBtnClicked();
    void onSubmitBtnClicked();
    void onBackBtnClicked();
    QLabel* createTitleLabel(const QString& text);
    QLabel* createValueLabel(const QString& text);
    QPushButton* createImageButton(const QString& text, QWidget* parent = nullptr);
    QString displayText(const QString& field) const;
    void updateReworkOptionsVisible();
    void submitJudge(const QString& reworkType = QString());
    void showInfoDialog(const QString& title, const QString& message);

private:
    QVariantMap m_rowData;
    QWidget* m_infoWidget{nullptr};
    QGridLayout* m_infoLayout{nullptr};
    QPushButton* m_saveBtn{nullptr};
    QPushButton* m_backBtn{nullptr};
    QButtonGroup* m_methodGroup{nullptr};
    QTextEdit* m_judgeTextEdit{nullptr};
    QPushButton* m_submitBtn{nullptr};
    QPushButton* m_repairReworkBtn{nullptr};
    QPushButton* m_productionReworkBtn{nullptr};
    QWidget* m_contentWidget{nullptr};
    QWidget* m_reworkOptionsWidget{nullptr};
    QComboBox* m_reworkRouteCombo{nullptr};
    QComboBox* m_reworkFileCombo{nullptr};
};

#endif // REPAIRJUDGEPAGE_H
