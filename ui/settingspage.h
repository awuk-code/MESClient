#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QMap>
#include <QList>
#include <QDialog>

class QCheckBox;
class QVBoxLayout;

class SettingsPage : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsPage(QWidget* parent = nullptr);

private slots:
    void saveSettings();

private:
    void initUI();
    QWidget* createSearchConfigPage();
    QWidget* createModuleGroup(const QString& title, const QString& module);

    QMap<QString, QList<QCheckBox*>> m_fieldChecks;
};

#endif // SETTINGSPAGE_H
