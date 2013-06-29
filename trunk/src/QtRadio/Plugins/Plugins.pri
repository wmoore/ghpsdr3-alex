INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += sql
SOURCES += \
    Plugins/Servers.cpp \
    Plugins/Keypad.cpp  \
    Plugins/LogBook.cpp \
    Plugins/RBClient.cpp

HEADERS += \
    Plugins/Servers.h \
    Plugins/Keypad.h  \
    Plugins/LogBook.h \
    Plugins/RBClient.h

FORMS += \
    Plugins/Servers.ui \
    Plugins/Keypad.ui  \
    Plugins/LogBook.ui \
    Plugins/RBClient.ui
