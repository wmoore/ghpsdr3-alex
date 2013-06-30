INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += widgets

SOURCES += \
    Plugins/MultimonDecoder/AudioBuffer.cpp     \
    Plugins/MultimonDecoder/Ssi.cpp             \
    Plugins/MultimonDecoder/multimon/costabf.c  \
    Plugins/MultimonDecoder/multimon/demod_dtmf.cpp \
    Plugins/MultimonDecoder/MultimonDecoder.cpp \
    Plugins/MultimonDecoder/multimon/demod_afsk12.cpp

HEADERS += \
    Plugins/MultimonDecoder/AudioBuffer.h       \
    Plugins/MultimonDecoder/Ssi.h               \
    Plugins/MultimonDecoder/multimon/filter.h   \
    Plugins/MultimonDecoder/multimon/filter-i386.h  \
    Plugins/MultimonDecoder/multimon/demod_dtmf.h \
    Plugins/MultimonDecoder/multimon/multimon.h \
    Plugins/MultimonDecoder/MultimonDecoder.h \
    Plugins/MultimonDecoder/multimon/demod_afsk12.h

FORMS += \
    Plugins/MultimonDecoder/MultimonDecoder.ui
