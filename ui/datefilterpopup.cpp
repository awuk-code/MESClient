#include "datefilterpopup.h"

DateFilterPopup::DateFilterPopup(QWidget *parent)
    : QWidget{parent}
{
    setWindowFlags(Qt::Popup);

    auto layout = new QVBoxLayout(this);

    m_calendar = new QCalendarWidget(this);
    m_calendar->setGridVisible(true);

    layout->addWidget(m_calendar);

    auto btnLayout = new QHBoxLayout;
    auto displayBtn = new QPushButton(tr("显示全部"), this);
    auto okBtn = new QPushButton(tr("确定"), this);
    auto cancelBtn = new QPushButton(tr("取消"), this);

    btnLayout->addStretch();
    btnLayout->addWidget(displayBtn);
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    layout->addLayout(btnLayout);

    connect(displayBtn, &QPushButton::clicked,
            this, &DateFilterPopup::onDisplayBtnClicked);
    connect(okBtn, &QPushButton::clicked,
            this, &DateFilterPopup::onOkBtnClicked);
    connect(cancelBtn, &QPushButton::clicked,
            this, &DateFilterPopup::onCancelBtnClicked);

}

void DateFilterPopup::onDisplayBtnClicked()
{
    emit dateSelected(QDate()); //无效日期表示取消过滤
    close();
}

void DateFilterPopup::onOkBtnClicked()
{
    emit dateSelected(m_calendar->selectedDate());
    close();
}

void DateFilterPopup::onCancelBtnClicked()
{
    close();
}
