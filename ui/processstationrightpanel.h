#ifndef PROCESSSTATIONRIGHTPANEL_H
#define PROCESSSTATIONRIGHTPANEL_H

#include "basepagewidget.h"
#include "datauploadwidget.h"
#include "pdfviewwidget.h"
#include "toggleswitchwidget.h"

class QLabel;
class QComboBox;


class ProcessStationRightPanel : public BasePageWidget
{
    Q_OBJECT
public:
    explicit ProcessStationRightPanel(QWidget* parent = nullptr);
    void setReplacementMaterialVisible(bool visible);
    void clearProductSnList();
    bool validatePassReady(const QString& productSn, QString* message = nullptr) const;

public slots:
    void addScannedProductSn(const QString& productSn);

signals:
    void toggleRequested(bool isChecked);

protected:
    TabConfigs Tabs() const override;
    QString pageTitle() const override;
    QString searchInfo() const override;
    QAbstractItemModel* createModel() override;
    FieldFilterProxyModel* createProxy(const QVariant &data) override;
    void addWidgetToTitle(QHBoxLayout *layout) override;
    void setupSearchLayout(QHBoxLayout* layout) override;

    void setCurrentSearchInfo(const QString& info);

private:
    void onSaveBtnClicked();
    void updateTableModelByTab(int index);
    void updateSearchBarByTab(int index);
    void applyCurrentProductSn();
    QString productSnFieldForTab(int index) const;
    int replacementMaterialTabIndex() const;

private:
    ToggleSwitchWidget* m_toggle{nullptr};
    QString m_currentSearchInfo;
    QLabel* m_productSnLabel{nullptr};
    QComboBox* m_productSnCombo{nullptr};
    QString m_currentProductSn;

    PdfViewWidget* m_processPdfPage{nullptr};
    PdfViewWidget* m_referencePdfPage{nullptr};
    DataUPloadWidget* m_uploadPage{nullptr};

    bool m_replacementMaterialVisible{true};
};

#endif // PROCESSSTATIONRIGHTPANEL_H
