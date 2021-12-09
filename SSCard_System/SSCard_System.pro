QT       += core gui quickwidgets axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../utility/TimeUtility.cpp \
    ../utility/Utility.cpp \
    Gloabal.cpp \
    MaskWidget.cpp \
    OperatorSucceed.cpp \
    Payment.cpp \
    SDK/dvtldcamocx/dvtldcamocxlib.cpp \
    main.cpp \
    mainpage.cpp \
    mainwindow.cpp \
    qmainstackpage.cpp \
    qpinkeybroad.cpp \
    qstackpage.cpp \
    registerlost.cpp \
    rl_ensureinformation.cpp \
    uc_adforfinance.cpp \
    uc_ensureinformation.cpp \
    uc_facecapture.cpp \
    uc_inputmobile.cpp \
    uc_makecard.cpp \
    uc_pay.cpp \
    uc_readidcard.cpp \
    up_Inputpwd.cpp \
    up_changepwd.cpp \
    up_readsscard.cpp \
    updatecard.cpp \
    updatepassword.cpp \

HEADERS += \
    Gloabal.h \
    MaskWidget.h \
    OperatorSucceed.h \
    Payment.h \
    SDK/dvtldcamocx/dvtldcamocxlib.h \
    mainpage.h \
    mainwindow.h \
    qmainstackpage.h \
    qpinkeybroad.h \
    qstackpage.h \
    registerlost.h \
    rl_ensureinformation.h \
    uc_adforfinance.h \
    uc_ensureinformation.h \
    uc_facecapture.h \
    uc_inputmobile.h \
    uc_makecard.h \
    uc_pay.h \
    uc_readidcard.h \
    up_Inputpwd.h \
    up_changepwd.h \
    up_readsscard.h \
    updatecard.h \
    updatepassword.h \
    ../utility/AutoLock.h \
    ../utility/TimeUtility.h \
    ../utility/Utility.h

FORMS += \
    MaskWidget.ui \
    OperatorSucceed.ui \
    mainpage.ui \
    mainwindow.ui \
    registerlost.ui \
    rl_ensureinformation.ui \
    uc_adforfinance.ui \
    uc_ensureinformation.ui \
    uc_facecapture.ui \
    uc_inputmobile.ui \
    uc_makecard.ui \
    uc_pay.ui \
    uc_readidcard.ui \
    up_Inputpwd.ui \
    up_changepwd.ui \
    up_readsscard.ui \
    updatecard.ui \
    updatecard_copy.ui \
    updatepassword.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Image.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/SDK/IDCard/ -lIDCard_API
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/SDK/IDCard/ -lIDCard_API

INCLUDEPATH += $$PWD/SDK/IDCard
DEPENDPATH += $$PWD/SDK/IDCard

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/glog/ -lglog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/glog/ -lglogd

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/SDK/PinKeybroad -lXZ_F10_API
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/SDK/PinKeybroad -lXZ_F10_API

INCLUDEPATH += $$PWD/glog
DEPENDPATH += $$PWD/glog
#用以避免C++17中std::byte与windows 定义的byte冲突
DEFINES += _HAS_STD_BYTE=0

DISTFILES += \
    Image/Bkgrnd.png \
    qml/SliderPlayer.qml

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/SDK/QREncode/ -lqrencode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/SDK/QREncode/ -lqrencoded

INCLUDEPATH += $$PWD/SDK/QREncode
DEPENDPATH += $$PWD/SDK/QREncode

win32: LIBS += -L$$PWD/SDK/Printer/ -lKT_Printer

INCLUDEPATH += $$PWD/SDK/Printer
DEPENDPATH += $$PWD/SDK/Printer

win32: LIBS += -L$$PWD/SDK/Reader/ -lKT_Reader

INCLUDEPATH += $$PWD/SDK/Reader
DEPENDPATH += $$PWD/SDK/Reader
