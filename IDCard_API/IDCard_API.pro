CONFIG -= qt

TEMPLATE = lib
DEFINES += IDCARD_API_LIBRARY

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
    idcard_api.cpp \
    idcardsdk.cpp

HEADERS += \
    ../utility/Utility.h \
    IDCard_API_global.h \
    idcard_api.h \
    idcardsdk.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
