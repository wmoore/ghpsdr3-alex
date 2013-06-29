INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
SOURCES += Hardware/Hardware.cpp \
    Hardware/HardwareSdriq.cpp \
    Hardware/HardwareSDR1000.cpp \
    Hardware/HardwareRtlsdr.cpp \
    Hardware/HardwarePerseus.cpp \
    Hardware/HardwareHiqsdr.cpp \
    Hardware/HardwareHermes.cpp \

HEADERS += Hardware/Hardware.h \
    Hardware/HardwareSdriq.h \
    Hardware/HardwareSDR1000.h \
    Hardware/HardwareRtlsdr.h \
    Hardware/HardwarePerseus.h \
    Hardware/HardwareHiqsdr.h \
    Hardware/HardwareHermes.h
