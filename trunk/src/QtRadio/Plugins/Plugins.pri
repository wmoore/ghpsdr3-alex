INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += sql
SOURCES += \
    Plugins/LogBook.cpp \
    Plugins/RBClient.cpp

HEADERS += \
    Plugins/LogBook.h \
    Plugins/RBClient.h

FORMS += \
    Plugins/LogBook.ui \
    Plugins/RBClient.ui
