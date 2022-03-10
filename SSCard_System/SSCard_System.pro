QT       += core gui quickwidgets axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#用以避免C++17中std::byte与windows 定义的byte冲突
DEFINES += _HAS_STD_BYTE=0
DEFINES += _WINDOWS
DEFINES += WIN32
DEFINES += _ENABLE_EXTENDED_ALIGNED_STORAGE
DEFINES += WIN32_LEAN_AND_MEAN

TRANSLATIONS += SSCardSystem_zh_CN.ts

SOURCES += \
    ../utility/TimeUtility.cpp \
    ../utility/Utility.cpp \
    CheckPassword.cpp \
    Gloabal.cpp \
    MaskWidget.cpp \
    OperatorSucceed.cpp \
    Payment.cpp \
    Sys_DeviceManager.cpp \
    Sys_cardmanger.cpp \
    Sys_dialogcameratest.cpp \
    Sys_dialogidcardinfo.cpp \
    Sys_logmanager.cpp \
    Sys_otheroptions.cpp \
    Sys_pageconfigure.cpp \
    Sys_qwidgetreadidcard.cpp \
    Sys_regionconfigure.cpp \
    Sys_sscardservicet.cpp \
    SystemManager.cpp \
    WMIQuery.cpp \
    imageviewlabel.cpp \
    license.cpp \
    main.cpp \
    mainpage.cpp \
    mainwindow.cpp \
    nc_commitpersoninfo.cpp \
    nc_makephoto.cpp \
    newcard.cpp \
    qmainstackpage.cpp \
    qpinkeybroad.cpp \
    qstackpage.cpp \
    registerlost.cpp \
    showlicense.cpp \
    sys_manualmakecard.cpp \
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
    CheckPassword.h \
    ConfigurePage.h \
    DevBase.h \
    Gloabal.h \
    MaskWidget.h \
    OperatorSucceed.h \
    Payment.h \
    SDK/KT_Define.h \
    Sys_DeviceManager.h \
    Sys_cardmanger.h \
    Sys_dialogcameratest.h \
    Sys_dialogidcardinfo.h \
    Sys_logmanager.h \
    Sys_otheroptions.h \
    Sys_pageconfigure.h \
    Sys_qwidgetreadidcard.h \
    Sys_regionconfigure.h \
    Sys_sscardservicet.h \
    SystemManager.h \
    WMIQuery.h \
    imageviewlabel.h \
    license.h \
    mainpage.h \
    mainwindow.h \
    nc_commitpersoninfo.h \
    nc_makephoto.h \
    newcard.h \
    qmainstackpage.h \
    qpinkeybroad.h \
    qstackpage.h \
    registerlost.h \
    showlicense.h \
    sys_manualmakecard.h \
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
    ../utility/Utility.h\
    ./SDK/KT_Define.h

FORMS += \
    CheckPassword.ui \
    MaskWidget.ui \
    Newcard.ui \
    OperatorSucceed.ui \
    Sys_DeviceManager.ui \
    Sys_cardmanger.ui \
    Sys_dialogcameratest.ui \
    Sys_dialogidcardinfo.ui \
    Sys_logmanager.ui \
    Sys_otheroptions.ui \
    Sys_pageconfigure.ui \
    Sys_qwidgetreadidcard.ui \
    Sys_regionconfigure.ui \
    Sys_sscardservicet.ui \
    SystemManager.ui \
    mainpage.ui \
    mainwindow.ui \
    nc_commitpersoninfo.ui \
    nc_makephoto.ui \
    registerlost.ui \
    showlicense.ui \
    sys_manualmakecard.ui \
    uc_adforfinance.ui \
    uc_ensureinformation.ui \
    uc_facecapture.ui \
    uc_inputmobile.ui \
    uc_makecard.ui \
    uc_pay.ui \
    uc_readidcard.ui \
    up_Inputpwd.ui \
    up_changepwd.ui \
    up_readsscard.bak.ui \
    up_readsscard.ui \
    updatecard.ui \
    updatepassword.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Image.qrc

win32:CONFIG(release, debug|release): LIBS += -L ../SDK/IDCard/ -lIDCard_API
else:win32:CONFIG(debug, debug|release): LIBS += -L ../SDK/IDCard/ -lIDCard_API

INCLUDEPATH += ../SDK/IDCard
DEPENDPATH += ../SDK/IDCard

win32:CONFIG(release, debug|release): LIBS += -L ../glog/ -lglog
else:win32:CONFIG(debug, debug|release): LIBS += -L ../glog/ -lglogd


INCLUDEPATH += ../SDK/glog
DEPENDPATH += ../SDK/glog

win32:CONFIG(release, debug|release): LIBS += -L ../SDK/QREncode/ -lqrencode
else:win32:CONFIG(debug, debug|release): LIBS += -L ../SDK/QREncode/ -lqrencoded

INCLUDEPATH += ../SDK/QREncode
DEPENDPATH += ../SDK/QREncode

win32: LIBS += -L ../SDK/KT_Printer/ -lKT_Printer

INCLUDEPATH += ../SDK/KT_Printer
DEPENDPATH += ../SDK/KT_Printer

win32: LIBS += -L ../SDK/KT_Reader/ -lKT_Reader

INCLUDEPATH += ../SDK/KT_Reader
DEPENDPATH += ../SDK/KT_Reader

INCLUDEPATH += ../SDK
DEPENDPATH += ../SDK

INCLUDEPATH += ../utility
DEPENDPATH += ../utility


win32: LIBS += -L$$PWD/../SDK/bugtrap/ -lBugTrapU

INCLUDEPATH += $$PWD/../SDK/bugtrap
DEPENDPATH += $$PWD/../SDK/bugtrap

win32: LIBS += -L$$PWD/../openssl-1.0.1g/lib/ -llibssl

INCLUDEPATH += $$PWD/../openssl-1.0.1g/include
DEPENDPATH += $$PWD/../openssl-1.0.1g/include

win32: LIBS += -L$$PWD/../openssl-1.0.1g/lib/ -llibssl

INCLUDEPATH += $$PWD/../openssl-1.0.1g/include
DEPENDPATH += $$PWD/../openssl-1.0.1g/include
