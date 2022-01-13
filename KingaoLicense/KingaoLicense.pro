QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
DEFINES += _HAS_STD_BYTE=0
DEFINES += _WINDOWS
DEFINES += WIN32
DEFINES += _ENABLE_EXTENDED_ALIGNED_STORAGE
DEFINES += WIN32_LEAN_AND_MEAN

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../openssl-1.0.1g/include/openssl/applink.c \
    ../utility/TimeUtility.cpp \
    ../utility/Utility.cpp \
    main.cpp \
    register.cpp

HEADERS += \
    ../openssl-1.0.1g/include/openssl/__DECC_INCLUDE_EPILOGUE.H \
    ../openssl-1.0.1g/include/openssl/__DECC_INCLUDE_PROLOGUE.H \
    ../openssl-1.0.1g/include/openssl/aes.h \
    ../openssl-1.0.1g/include/openssl/asn1.h \
    ../openssl-1.0.1g/include/openssl/asn1_mac.h \
    ../openssl-1.0.1g/include/openssl/asn1err.h \
    ../openssl-1.0.1g/include/openssl/asn1t.h \
    ../openssl-1.0.1g/include/openssl/async.h \
    ../openssl-1.0.1g/include/openssl/asyncerr.h \
    ../openssl-1.0.1g/include/openssl/bio.h \
    ../openssl-1.0.1g/include/openssl/bioerr.h \
    ../openssl-1.0.1g/include/openssl/blowfish.h \
    ../openssl-1.0.1g/include/openssl/bn.h \
    ../openssl-1.0.1g/include/openssl/bnerr.h \
    ../openssl-1.0.1g/include/openssl/buffer.h \
    ../openssl-1.0.1g/include/openssl/buffererr.h \
    ../openssl-1.0.1g/include/openssl/camellia.h \
    ../openssl-1.0.1g/include/openssl/cast.h \
    ../openssl-1.0.1g/include/openssl/cmac.h \
    ../openssl-1.0.1g/include/openssl/cms.h \
    ../openssl-1.0.1g/include/openssl/cmserr.h \
    ../openssl-1.0.1g/include/openssl/comp.h \
    ../openssl-1.0.1g/include/openssl/comperr.h \
    ../openssl-1.0.1g/include/openssl/conf.h \
    ../openssl-1.0.1g/include/openssl/conf_api.h \
    ../openssl-1.0.1g/include/openssl/conferr.h \
    ../openssl-1.0.1g/include/openssl/crypto.h \
    ../openssl-1.0.1g/include/openssl/cryptoerr.h \
    ../openssl-1.0.1g/include/openssl/ct.h \
    ../openssl-1.0.1g/include/openssl/cterr.h \
    ../openssl-1.0.1g/include/openssl/des.h \
    ../openssl-1.0.1g/include/openssl/dh.h \
    ../openssl-1.0.1g/include/openssl/dherr.h \
    ../openssl-1.0.1g/include/openssl/dsa.h \
    ../openssl-1.0.1g/include/openssl/dsaerr.h \
    ../openssl-1.0.1g/include/openssl/dtls1.h \
    ../openssl-1.0.1g/include/openssl/e_os2.h \
    ../openssl-1.0.1g/include/openssl/ebcdic.h \
    ../openssl-1.0.1g/include/openssl/ec.h \
    ../openssl-1.0.1g/include/openssl/ecdh.h \
    ../openssl-1.0.1g/include/openssl/ecdsa.h \
    ../openssl-1.0.1g/include/openssl/ecerr.h \
    ../openssl-1.0.1g/include/openssl/engine.h \
    ../openssl-1.0.1g/include/openssl/engineerr.h \
    ../openssl-1.0.1g/include/openssl/err.h \
    ../openssl-1.0.1g/include/openssl/evp.h \
    ../openssl-1.0.1g/include/openssl/evperr.h \
    ../openssl-1.0.1g/include/openssl/hmac.h \
    ../openssl-1.0.1g/include/openssl/idea.h \
    ../openssl-1.0.1g/include/openssl/kdf.h \
    ../openssl-1.0.1g/include/openssl/kdferr.h \
    ../openssl-1.0.1g/include/openssl/lhash.h \
    ../openssl-1.0.1g/include/openssl/md2.h \
    ../openssl-1.0.1g/include/openssl/md4.h \
    ../openssl-1.0.1g/include/openssl/md5.h \
    ../openssl-1.0.1g/include/openssl/mdc2.h \
    ../openssl-1.0.1g/include/openssl/modes.h \
    ../openssl-1.0.1g/include/openssl/obj_mac.h \
    ../openssl-1.0.1g/include/openssl/objects.h \
    ../openssl-1.0.1g/include/openssl/objectserr.h \
    ../openssl-1.0.1g/include/openssl/ocsp.h \
    ../openssl-1.0.1g/include/openssl/ocsperr.h \
    ../openssl-1.0.1g/include/openssl/opensslconf.h \
    ../openssl-1.0.1g/include/openssl/opensslv.h \
    ../openssl-1.0.1g/include/openssl/ossl_typ.h \
    ../openssl-1.0.1g/include/openssl/pem.h \
    ../openssl-1.0.1g/include/openssl/pem2.h \
    ../openssl-1.0.1g/include/openssl/pemerr.h \
    ../openssl-1.0.1g/include/openssl/pkcs12.h \
    ../openssl-1.0.1g/include/openssl/pkcs12err.h \
    ../openssl-1.0.1g/include/openssl/pkcs7.h \
    ../openssl-1.0.1g/include/openssl/pkcs7err.h \
    ../openssl-1.0.1g/include/openssl/rand.h \
    ../openssl-1.0.1g/include/openssl/randerr.h \
    ../openssl-1.0.1g/include/openssl/rc2.h \
    ../openssl-1.0.1g/include/openssl/rc4.h \
    ../openssl-1.0.1g/include/openssl/rc5.h \
    ../openssl-1.0.1g/include/openssl/ripemd.h \
    ../openssl-1.0.1g/include/openssl/rsa.h \
    ../openssl-1.0.1g/include/openssl/rsaerr.h \
    ../openssl-1.0.1g/include/openssl/safestack.h \
    ../openssl-1.0.1g/include/openssl/seed.h \
    ../openssl-1.0.1g/include/openssl/sha.h \
    ../openssl-1.0.1g/include/openssl/srp.h \
    ../openssl-1.0.1g/include/openssl/srtp.h \
    ../openssl-1.0.1g/include/openssl/ssl.h \
    ../openssl-1.0.1g/include/openssl/ssl2.h \
    ../openssl-1.0.1g/include/openssl/ssl3.h \
    ../openssl-1.0.1g/include/openssl/sslerr.h \
    ../openssl-1.0.1g/include/openssl/stack.h \
    ../openssl-1.0.1g/include/openssl/store.h \
    ../openssl-1.0.1g/include/openssl/storeerr.h \
    ../openssl-1.0.1g/include/openssl/symhacks.h \
    ../openssl-1.0.1g/include/openssl/tls1.h \
    ../openssl-1.0.1g/include/openssl/ts.h \
    ../openssl-1.0.1g/include/openssl/tserr.h \
    ../openssl-1.0.1g/include/openssl/txt_db.h \
    ../openssl-1.0.1g/include/openssl/ui.h \
    ../openssl-1.0.1g/include/openssl/uierr.h \
    ../openssl-1.0.1g/include/openssl/whrlpool.h \
    ../openssl-1.0.1g/include/openssl/x509.h \
    ../openssl-1.0.1g/include/openssl/x509_vfy.h \
    ../openssl-1.0.1g/include/openssl/x509err.h \
    ../openssl-1.0.1g/include/openssl/x509v3.h \
    ../openssl-1.0.1g/include/openssl/x509v3err.h \
    ../utility/TimeUtility.h \
    ../utility/Utility.h \
    register.h

FORMS += \
    register.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../openssl-1.0.1g/lib/ -llibcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../openssl-1.0.1g/lib/ -llibcrypto
else:macx: LIBS += -L$$PWD/../openssl-1.0.1g/lib/ -llibcrypto

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../openssl-1.0.1g/lib/ -llibssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../openssl-1.0.1g/lib/ -llibssl

INCLUDEPATH += $$PWD/../openssl-1.0.1g/include
DEPENDPATH += $$PWD/../openssl-1.0.1g/include
