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
    SDK/dvtldcamocx/dvtldcamocxlib.cpp \
    Sys_DeviceManager.cpp \
    Sys_cardmanger.cpp \
    Sys_dialogcameratest.cpp \
    Sys_dialogidcardinfo.cpp \
    Sys_logmanager.cpp \
    Sys_otheroptions.cpp \
    Sys_pageconfigure.cpp \
    Sys_qwidgetreadidcard.cpp \
    Sys_regionconfigure.cpp \
    SystemManager.cpp \
    WMIQuery.cpp \
    administer.cpp \
    imageviewlabel.cpp \
    license.cpp \
    main.cpp \
    mainpage.cpp \
    mainwindow.cpp \
    nc_commitPersonInfo.cpp \
    newcard.cpp \
    openssl-1.0.1g/include/openssl/applink.c \
    qmainstackpage.cpp \
    qpinkeybroad.cpp \
    qstackpage.cpp \
    qwidgetreadidcard.cpp \
    registerlost.cpp \
    showlicense.cpp \
    sys_dialogreadidcard.cpp \
    sys_manualmakecard.cpp \
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
    waitingprogress.cpp

HEADERS += \
    CheckPassword.h \
    ConfigurePage.h \
    DevBase.h \
    Gloabal.h \
    MaskWidget.h \
    OperatorSucceed.h \
    Payment.h \
    SDK/KT_Define.h \
    SDK/dvtldcamocx/dvtldcamocxlib.h \
    Sys_DeviceManager.h \
    Sys_cardmanger.h \
    Sys_dialogcameratest.h \
    Sys_dialogidcardinfo.h \
    Sys_logmanager.h \
    Sys_otheroptions.h \
    Sys_pageconfigure.h \
    Sys_qwidgetreadidcard.h \
    Sys_regionconfigure.h \
    SystemManager.h \
    WMIQuery.h \
    administer.h \
    imageviewlabel.h \
    license.h \
    mainpage.h \
    mainwindow.h \
    nc_commitPersonInfo.h \
    newcard.h \
    openssl-1.0.1g/include/openssl/aes.h \
    openssl-1.0.1g/include/openssl/asn1.h \
    openssl-1.0.1g/include/openssl/asn1_mac.h \
    openssl-1.0.1g/include/openssl/asn1t.h \
    openssl-1.0.1g/include/openssl/bio.h \
    openssl-1.0.1g/include/openssl/blowfish.h \
    openssl-1.0.1g/include/openssl/bn.h \
    openssl-1.0.1g/include/openssl/buffer.h \
    openssl-1.0.1g/include/openssl/camellia.h \
    openssl-1.0.1g/include/openssl/cast.h \
    openssl-1.0.1g/include/openssl/cmac.h \
    openssl-1.0.1g/include/openssl/cms.h \
    openssl-1.0.1g/include/openssl/comp.h \
    openssl-1.0.1g/include/openssl/conf.h \
    openssl-1.0.1g/include/openssl/conf_api.h \
    openssl-1.0.1g/include/openssl/crypto.h \
    openssl-1.0.1g/include/openssl/des.h \
    openssl-1.0.1g/include/openssl/des_old.h \
    openssl-1.0.1g/include/openssl/dh.h \
    openssl-1.0.1g/include/openssl/dsa.h \
    openssl-1.0.1g/include/openssl/dso.h \
    openssl-1.0.1g/include/openssl/dtls1.h \
    openssl-1.0.1g/include/openssl/e_os2.h \
    openssl-1.0.1g/include/openssl/ebcdic.h \
    openssl-1.0.1g/include/openssl/ec.h \
    openssl-1.0.1g/include/openssl/ecdh.h \
    openssl-1.0.1g/include/openssl/ecdsa.h \
    openssl-1.0.1g/include/openssl/engine.h \
    openssl-1.0.1g/include/openssl/err.h \
    openssl-1.0.1g/include/openssl/evp.h \
    openssl-1.0.1g/include/openssl/hmac.h \
    openssl-1.0.1g/include/openssl/idea.h \
    openssl-1.0.1g/include/openssl/krb5_asn.h \
    openssl-1.0.1g/include/openssl/kssl.h \
    openssl-1.0.1g/include/openssl/lhash.h \
    openssl-1.0.1g/include/openssl/md4.h \
    openssl-1.0.1g/include/openssl/md5.h \
    openssl-1.0.1g/include/openssl/mdc2.h \
    openssl-1.0.1g/include/openssl/modes.h \
    openssl-1.0.1g/include/openssl/obj_mac.h \
    openssl-1.0.1g/include/openssl/objects.h \
    openssl-1.0.1g/include/openssl/ocsp.h \
    openssl-1.0.1g/include/openssl/opensslconf.h \
    openssl-1.0.1g/include/openssl/opensslv.h \
    openssl-1.0.1g/include/openssl/ossl_typ.h \
    openssl-1.0.1g/include/openssl/pem.h \
    openssl-1.0.1g/include/openssl/pem2.h \
    openssl-1.0.1g/include/openssl/pkcs12.h \
    openssl-1.0.1g/include/openssl/pkcs7.h \
    openssl-1.0.1g/include/openssl/pqueue.h \
    openssl-1.0.1g/include/openssl/rand.h \
    openssl-1.0.1g/include/openssl/rc2.h \
    openssl-1.0.1g/include/openssl/rc4.h \
    openssl-1.0.1g/include/openssl/ripemd.h \
    openssl-1.0.1g/include/openssl/rsa.h \
    openssl-1.0.1g/include/openssl/safestack.h \
    openssl-1.0.1g/include/openssl/seed.h \
    openssl-1.0.1g/include/openssl/sha.h \
    openssl-1.0.1g/include/openssl/srp.h \
    openssl-1.0.1g/include/openssl/srtp.h \
    openssl-1.0.1g/include/openssl/ssl.h \
    openssl-1.0.1g/include/openssl/ssl2.h \
    openssl-1.0.1g/include/openssl/ssl23.h \
    openssl-1.0.1g/include/openssl/ssl3.h \
    openssl-1.0.1g/include/openssl/stack.h \
    openssl-1.0.1g/include/openssl/symhacks.h \
    openssl-1.0.1g/include/openssl/tls1.h \
    openssl-1.0.1g/include/openssl/ts.h \
    openssl-1.0.1g/include/openssl/txt_db.h \
    openssl-1.0.1g/include/openssl/ui.h \
    openssl-1.0.1g/include/openssl/ui_compat.h \
    openssl-1.0.1g/include/openssl/whrlpool.h \
    openssl-1.0.1g/include/openssl/x509.h \
    openssl-1.0.1g/include/openssl/x509_vfy.h \
    openssl-1.0.1g/include/openssl/x509v3.h \
    qmainstackpage.h \
    qpinkeybroad.h \
    qstackpage.h \
    qwidgetreadidcard.h \
    registerlost.h \
    showlicense.h \
    sys_dialogreadidcard.h \
    sys_manualmakecard.h \
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
    ./SDK/KT_Define.h \
    waitingprogress.h

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
    SystemManager.ui \
    administer.ui \
    license.ui \
    mainpage.ui \
    mainwindow.ui \
    nc_commitpersoninfo.ui \
    registerlost.ui \
    showlicense.ui \
    sys_dialogreadidcard.ui \
    sys_manualmakecard.ui \
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
    updatepassword.ui \
    waitingprogress.ui

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


INCLUDEPATH += $$PWD/glog
DEPENDPATH += $$PWD/glog

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/SDK/QREncode/ -lqrencode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/SDK/QREncode/ -lqrencoded

INCLUDEPATH += $$PWD/SDK/QREncode
DEPENDPATH += $$PWD/SDK/QREncode

win32: LIBS += -L$$PWD/SDK/KT_Printer/ -lKT_Printer

INCLUDEPATH += $$PWD/SDK/KT_Printer
DEPENDPATH += $$PWD/SDK/KT_Printer

win32: LIBS += -L$$PWD/SDK/KT_Reader/ -lKT_Reader

INCLUDEPATH += $$PWD/SDK/KT_Reader
DEPENDPATH += $$PWD/SDK/KT_Reader

INCLUDEPATH += $$PWD/SDK
DEPENDPATH += $$PWD/SDK
