QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += _HAS_STD_BYTE=0
DEFINES += _WINDOWS
DEFINES += WIN32
DEFINES += _ENABLE_EXTENDED_ALIGNED_STORAGE
DEFINES += WIN32_LEAN_AND_MEAN

SOURCES += \
    ../utility/Utility.cpp \
    main.cpp \
    dialog.cpp

HEADERS += \
    ../utility/Utility.h \
    dialog.h

FORMS += \
    dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/sdk/ -lIDCard_API

INCLUDEPATH += $$PWD/sdk
DEPENDPATH += $$PWD/sdk
