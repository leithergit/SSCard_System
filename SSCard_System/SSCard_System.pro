QT       += core gui sql xlsx

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
DEFINES += MARKUP_STL
DEFINES += _CRT_SECURE_NO_WARNINGS

TRANSLATIONS += SSCardSystem_zh_CN.ts

SOURCES += \
    ../utility/Markup.cpp \
    ../utility/TimeUtility.cpp \
    ../utility/Utility.cpp \
    CheckPassword.cpp \
    Gloabal.cpp \
    MaskWidget.cpp \
    OperatorSucceed.cpp \
    Payment.cpp \
    Sys_DeviceManager.cpp \
    Sys_batchmakecard.cpp \
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
    clickablelabel.cpp \
    dialogconfigbank.cpp \
    imageviewlabel.cpp \
    license.cpp \
    main.cpp \
    mainpage.cpp \
    mainwindow.cpp \
    nc_commitpersoninfo.cpp \
    newcard.cpp \
    qclickablelineedit.cpp \
    qi_information.cpp \
    qmainstackpage.cpp \
    qnationwidget.cpp \
    qpinkeybroad.cpp \
    qstackpage.cpp \
    queryinfo.cpp \
    registerlost.cpp \
    showlicense.cpp \
    sys_dialogreadidcard.cpp \
    sys_manualmakecard.cpp \
    sys_readidcard.cpp \
    uc_adforfinance.cpp \
    uc_ensureinformation.cpp \
    uc_facecapture.cpp \
    uc_inputidcardinfo.cpp \
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
    ../utility/Markup.h \
    CheckPassword.h \
    ConfigurePage.h \
    DevBase.h \
    Gloabal.h \
    MaskWidget.h \
    OperatorSucceed.h \
    Payment.h \
    SDK/KT_Define.h \
    Sys_DeviceManager.h \
    Sys_batchmakecard.h \
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
    clickablelabel.h \
    dialogconfigbank.h \
    imageviewlabel.h \
    license.h \
    mainpage.h \
    mainwindow.h \
    nc_commitpersoninfo.h \
    newcard.h \
    qclickablelineedit.h \
    qi_information.h \
    qmainstackpage.h \
    qnationwidget.h \
    qpinkeybroad.h \
    qstackpage.h \
    queryinfo.h \
    registerlost.h \
    showlicense.h \
    sys_dialogreadidcard.h \
    sys_manualmakecard.h \
    sys_readidcard.h \
    uc_adforfinance.h \
    uc_ensureinformation.h \
    uc_facecapture.h \
    uc_inputidcardinfo.h \
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
    Sys_batchmakecard.ui \
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
    dialogconfigbank.ui \
    mainpage.ui \
    mainwindow.ui \
    nc_commitpersoninfo.ui \
    qi_information.ui \
    qnationwidget.ui \
    queryinfo.ui \
    registerlost.ui \
    showlicense.ui \
    sys_dialogreadidcard.ui \
    sys_manualmakecard.ui \
    sys_readidcard.ui \
    uc_adforfinance.ui \
    uc_ensureinformation.ui \
    uc_facecapture.ui \
    uc_inputidcardinfo.ui \
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

INCLUDEPATH += ../SDK/IDCard
DEPENDPATH += ../SDK/IDCard

INCLUDEPATH += ../SDK/glog
DEPENDPATH += ../SDK/glog

INCLUDEPATH += ../SDK/QREncode
DEPENDPATH += ../SDK/QREncode

INCLUDEPATH += ../SDK/KT_Printer
DEPENDPATH += ../SDK/KT_Printer

INCLUDEPATH += ../SDK/KT_Reader
DEPENDPATH += ../SDK/KT_Reader

INCLUDEPATH += ../SDK
DEPENDPATH += ../SDK

INCLUDEPATH += ../utility
DEPENDPATH += ../utility

INCLUDEPATH += ../QtSingleton
DEPENDPATH += ../QtSingleton

INCLUDEPATH += $$PWD/../SDK/bugtrap
DEPENDPATH += $$PWD/../SDK/bugtrap

INCLUDEPATH += $$PWD/../openssl-1.0.1g/include
DEPENDPATH += $$PWD/../openssl-1.0.1g/include

INCLUDEPATH += $$PWD/../openssl-1.0.1g/include
DEPENDPATH += $$PWD/../openssl-1.0.1g/include

INCLUDEPATH += $$PWD/../SDK/IDCard
DEPENDPATH += $$PWD/../SDK/IDCard


DEPENDPATH += $$PWD/../SDK/SSCardDriver
DEPENDPATH += $$PWD/../SDK/SSCardHSM
DEPENDPATH += $$PWD/../SDK/SSCardInfo_Henan
DEPENDPATH += $$PWD/../SDK/libcurl
DEPENDPATH += $$PWD/../SDK/QREncode
DEPENDPATH += $$PWD/../SDK/IDCard
DEPENDPATH += $$PWD/../SDK/glog
DEPENDPATH += $$PWD/../SDK/PinKeybroad
DEPENDPATH += $$PWD/../SDK/7Z/lib



DISTFILES +=
