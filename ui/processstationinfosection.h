#ifndef PROCESSSTATIONINFOSECTION_H
#define PROCESSSTATIONINFOSECTION_H

#include <QPair>
#include <QVariantMap>
#include <QVector>
#include <QWidget>

class QLabel;
class QGridLayout;

using InfoFieldList = QVector<QPair<QString, QString>>;

class ProcessStationInfoSection : public QWidget
{
    Q_OBJECT
public:
    explicit ProcessStationInfoSection(const QString& title, QWidget* parent = nullptr);

    void setTitle(const QString& title);
    void setFieldsData(const InfoFieldList& fields, const QVariantMap& rowData, int fieldPairsPerRow = 0);

private:
    QWidget* createTitleWidget(const QString& title);
    QGridLayout* createInfoGrid(QWidget* parentWidget);
    QLabel* createInfoTitleLabel(const QString& text, QWidget* parentWidget);
    QLabel* createInfoValueLabel(const QString& text, QWidget* parentWidget);

private:
    QLabel* m_titleLabel{nullptr};
    QWidget* m_infoWidget{nullptr};
    QGridLayout* m_infoLayout{nullptr};
};

#endif // PROCESSSTATIONINFOSECTION_H
