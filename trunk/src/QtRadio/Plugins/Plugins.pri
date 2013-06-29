INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += sql
SOURCES += \
    Plugins/Servers.cpp \
    Plugins/Equalizer.cpp \
    Plugins/Keypad.cpp  \
    Plugins/LogBook.cpp \
    Plugins/RBClient.cpp

HEADERS += \
    Plugins/Servers.h \
    Plugins/Equalizer.h \
    Plugins/Keypad.h  \
    Plugins/LogBook.h \
    Plugins/RBClient.h

FORMS += \
    Plugins/Servers.ui \
    Plugins/Equalizer. ui \
    Plugins/Keypad.ui  \
    Plugins/LogBook.ui \
    Plugins/RBClient.ui
