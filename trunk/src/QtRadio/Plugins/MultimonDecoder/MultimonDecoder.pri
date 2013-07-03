INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += widgets

SOURCES += \
    Plugins/MultimonDecoder/AudioBuffer.cpp         \
    Plugins/MultimonDecoder/Ssi.cpp                 \
    Plugins/MultimonDecoder/multimon/costabf.c      \
    Plugins/MultimonDecoder/multimon/costabi.c      \
    Plugins/MultimonDecoder/MultimonDecoder.cpp \
    Plugins/MultimonDecoder/multimon/DemodAFSK12.cpp \
    Plugins/MultimonDecoder/multimon/DemodDTMF.cpp \
    Plugins/MultimonDecoder/multimon/HDLC.cpp

HEADERS += \
    Plugins/MultimonDecoder/AudioBuffer.h       \
    Plugins/MultimonDecoder/Ssi.h               \
    Plugins/MultimonDecoder/multimon/filter.h   \
    Plugins/MultimonDecoder/multimon/filter-i386.h  \
    Plugins/MultimonDecoder/MultimonDecoder.h \
    Plugins/MultimonDecoder/multimon/DemodDTMF.h \
    Plugins/MultimonDecoder/multimon/DemodAFSK12.h \
    Plugins/MultimonDecoder/multimon/multimon.h \
    Plugins/MultimonDecoder/multimon/HDLC.h

FORMS += \
    Plugins/MultimonDecoder/MultimonDecoder.ui
