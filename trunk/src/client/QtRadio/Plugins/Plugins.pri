INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += sql webkitwidgets

SOURCES += \
    Plugins/Servers.cpp \
    Plugins/Equalizer.cpp \
    Plugins/Keypad.cpp  \
    Plugins/LogBook.cpp \
    Plugins/RBClient.cpp \
    Plugins/RxAudio.cpp \
    Plugins/WebBrowser.cpp

HEADERS += \
    Plugins/Servers.h \
    Plugins/Equalizer.h \
    Plugins/Keypad.h  \
    Plugins/LogBook.h \
    Plugins/RBClient.h \
    Plugins/RxAudio.h \
    Plugins/WebBrowser.h

FORMS += \
    Plugins/Servers.ui \
    Plugins/Equalizer.ui \
    Plugins/Keypad.ui  \
    Plugins/LogBook.ui \
    Plugins/RBClient.ui \
    Plugins/RxAudio.ui \
    Plugins/WebBrowser.ui

include(MultimonDecoder/MultimonDecoder.pri)
