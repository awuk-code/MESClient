#ifndef PROCESSSTATIONPASSDIALOGS_H
#define PROCESSSTATIONPASSDIALOGS_H

#include "basedialogwidget.h"

#include <QList>
#include <QStringList>

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QTextEdit;

class NgAbnormalDialog : public BaseDialogWidget
{
public:
    explicit NgAbnormalDialog(const QString& productSn, QWidget* parent = nullptr);

    QString abnormalType() const;
    QString phenomenon() const;
    QString imagePath() const;
    QString nextProcess() const;
    QString summary() const;

protected:
    bool onConfirm() override;

private:
    QString m_productSn;
    QComboBox* m_productSnCombo{nullptr};
    QComboBox* m_typeCombo{nullptr};
    QLineEdit* m_imageEdit{nullptr};
    QTextEdit* m_phenomenonEdit{nullptr};
    QLabel* m_nextProcessLabel{nullptr};
};

class PauseReasonDialog : public BaseDialogWidget
{
public:
    explicit PauseReasonDialog(const QString& productSn, QWidget* parent = nullptr);

    QString pauseType() const;
    QString relatedCode() const;
    QString description() const;

protected:
    bool onConfirm() override;

private:
    QStringList selectedTypes() const;
    void updateCodePrompt();

private:
    QComboBox* m_productSnCombo{nullptr};
    QButtonGroup* m_typeGroup{nullptr};
    QList<QCheckBox*> m_typeCheckBoxes;
    QWidget* m_codeRowWidget{nullptr};
    QLabel* m_codeLabel{nullptr};
    QLineEdit* m_codeEdit{nullptr};
    QTextEdit* m_descriptionEdit{nullptr};
};

class ResumeReasonDialog : public BaseDialogWidget
{
public:
    explicit ResumeReasonDialog(const QString& productSn, QWidget* parent = nullptr);

    QString reason() const;

protected:
    bool onConfirm() override;

private:
    QComboBox* m_productSnCombo{nullptr};
    QTextEdit* m_reasonEdit{nullptr};
};

#endif // PROCESSSTATIONPASSDIALOGS_H
