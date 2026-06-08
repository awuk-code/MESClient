#ifndef DATAUPLOAD_H
#define DATAUPLOAD_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>


class DataUpload : public QWidget
{
    Q_OBJECT
public:
    explicit DataUpload(QWidget *parent = nullptr);

    void initUI();
    void initConnect();
    void setProductSn(const QString& productSn);

private slots:
    void onDocUploadBtnClicked();
    void onImgUploadBtnClicked();
signals:

private:
    QLabel* createTitleLabel(const QString& text);
    QLineEdit* createLineEdit(int minWidth);
    void chooseFile(QLineEdit* targetEdit, const QString& title);

private:
    QLabel* m_docLabel{nullptr};
    QLabel* m_imgLabel{nullptr};
    QLineEdit* m_docEdit{nullptr};
    QLineEdit* m_imgEdit{nullptr};
    QLineEdit* m_sizeEdit{nullptr};
    QLineEdit* m_dimensionEdit{nullptr};
    QLineEdit* m_weightEdit{nullptr};
    QPushButton* m_docUploadBtn{nullptr};
    QPushButton* m_imgUploadBtn{nullptr};

};

#endif // DATAUPLOAD_H
