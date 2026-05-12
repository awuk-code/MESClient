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
    auto displayBtn = new QPushButton("显示全部", this);
    auto okBtn = new QPushButton("确定", this);
    auto cancelBtn = new QPushButton("取消", this);

    btnLayout->addStretch();
    btnLayout->addWidget(displayBtn);
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    layout->addLayout(btnLayout);

    connect(displayBtn, &QPushButton::clicked, this, [=](){
        emit dateSelected(QDate()); //w无效日期表示取消过滤
        close();
    });
    connect(okBtn, &QPushButton::clicked, this, [=] {
        emit dateSelected(m_calendar->selectedDate());
        close();
    });

    connect(cancelBtn, &QPushButton::clicked, this, &QWidget::close);

}
