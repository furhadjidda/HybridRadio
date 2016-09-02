TEMPLATE = app

QT += qml quick multimedia network core
CONFIG += c++11

SOURCES += main.cpp \
    player.cpp \
    xmlparser.cpp \
    datacollecter.cpp \
    qmlsignalhandler.cpp \
    dnslookup.cpp \
    downloadmanager.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    xmlparser.hpp \
    downloadmanager.h \
    player.hpp \
    datacollecter.hpp \
    dnslookup.hpp \
    qmlsignalhandler.hpp

DISTFILES += \
    ../build-HybridRadio-Desktop_Qt_5_7_0_MinGW_32bit-Debug/PlayStrip.PNG

