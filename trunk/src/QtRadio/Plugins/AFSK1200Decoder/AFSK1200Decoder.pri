INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += widgets

SOURCES += \
    Plugins/AFSK1200Decoder/AFSK1200Decoder.cpp	\
    Plugins/AFSK1200Decoder/AudioBuffer.cpp     \
    Plugins/AFSK1200Decoder/Ssi.cpp             \
    Plugins/AFSK1200Decoder/multimon/costabf.c  \
    Plugins/AFSK1200Decoder/multimon/cafsk12.cpp    \
    Plugins/AFSK1200Decoder/multimon/demod_dtmf.cpp

HEADERS += \
    Plugins/AFSK1200Decoder/AFSK1200Decoder.h	\
    Plugins/AFSK1200Decoder/AudioBuffer.h       \
    Plugins/AFSK1200Decoder/Ssi.h               \
    Plugins/AFSK1200Decoder/multimon/filter.h   \
    Plugins/AFSK1200Decoder/multimon/filter-i386.h  \
    Plugins/AFSK1200Decoder/multimon/cafsk12.h \
    Plugins/AFSK1200Decoder/multimon/demod_dtmf.h \
    Plugins/AFSK1200Decoder/multimon/multimon.h

FORMS += \
    Plugins/AFSK1200Decoder/AFSK1200Decoder.ui
