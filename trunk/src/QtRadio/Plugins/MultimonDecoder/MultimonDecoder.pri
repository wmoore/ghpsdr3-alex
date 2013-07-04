INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += widgets

SOURCES += \
    Plugins/MultimonDecoder/AudioBuffer.cpp         \
    Plugins/MultimonDecoder/multimon/costabf.c      \
    Plugins/MultimonDecoder/multimon/costabi.c      \
    Plugins/MultimonDecoder/MultimonDecoder.cpp \
    Plugins/MultimonDecoder/multimon/DemodAFSK12.cpp \
    Plugins/MultimonDecoder/multimon/DemodDTMF.cpp \
    Plugins/MultimonDecoder/multimon/DemodFSK96.cpp \
    Plugins/MultimonDecoder/multimon/HDLC.cpp \
#    Plugins/MultimonDecoder/multimon/DemodClipFSK.cpp \
#    Plugins/MultimonDecoder/multimon/DemodAFSK24.cpp \
#    Plugins/MultimonDecoder/multimon/DemodAFSK24_3.cpp \
#    Plugins/MultimonDecoder/multimon/DemodAFSK24_2.cpp \
    Plugins/MultimonDecoder/multimon/DemodHAPN48.cpp \
    Plugins/MultimonDecoder/multimon/DemodUFSK12.cpp \
    Plugins/MultimonDecoder/multimon/DemodCW.cpp

HEADERS += \
    Plugins/MultimonDecoder/AudioBuffer.h       \
    Plugins/MultimonDecoder/multimon/filter.h   \
    Plugins/MultimonDecoder/multimon/filter-i386.h  \
    Plugins/MultimonDecoder/MultimonDecoder.h \
    Plugins/MultimonDecoder/multimon/DemodDTMF.h \
    Plugins/MultimonDecoder/multimon/DemodAFSK12.h \
    Plugins/MultimonDecoder/multimon/DemodFSK96.h \
    Plugins/MultimonDecoder/multimon/multimon.h \
    Plugins/MultimonDecoder/multimon/HDLC.h \
    Plugins/MultimonDecoder/multimon/DemodUFSK12.h \
    Plugins/MultimonDecoder/multimon/DemodHAPN48.h \
    Plugins/MultimonDecoder/multimon/DemodCW.h
#    Plugins/MultimonDecoder/multimon/DemodClipFSK.h \
#    Plugins/MultimonDecoder/multimon/DemodAFSK24.h \
#    Plugins/MultimonDecoder/multimon/DemodAFSK24_3.h \
#    Plugins/MultimonDecoder/multimon/DemodAFSK24_2.h

FORMS += \
    Plugins/MultimonDecoder/MultimonDecoder.ui
