QT       += core gui network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = MESClient
TEMPLATE = app

include(app/app.pri)
include(login/login.pri)
include(config/config.pri)
include(commFunc/common.pri)
include(ui/ui.pri)
include(core/core.pri)

INCLUDEPATH += $$PWD/thirdparty/poppler/Library/include
LIBS += -L$$PWD/thirdparty/poppler/Library/lib -lpoppler-cpp

INCLUDEPATH +=      \
    $$PWD/app       \
    $$PWD/login     \
    $$PWD/config    \
    $$PWD/commFunc  \
    $$PWD/ui        \
    $$PWD/core      \

RESOURCES += \
    res.qrc

DISTFILES += $$files(*/*.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target





