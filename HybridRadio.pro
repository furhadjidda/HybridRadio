TEMPLATE = app

QT += qml quick multimedia network core websockets
CONFIG += c++11

SOURCES += main.cpp \
    GccHelper.cpp \
    HttpTransport.cpp \
    HybridRadioCore.cpp \
    StompTransport.cpp \
    UiHandler.cpp \
    XmlReader.cpp \
    lookuphelper.cpp \
    player.cpp \
    qmlsignalhandler.cpp \
    dnslookup.cpp \
    downloadmanager.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    GccHelper.hpp \
    HttpTransport.hpp \
    HybridRadioCommonTypes.hpp \
    HybridRadioCore.hpp \
    StompTransport.hpp \
    Transport.hpp \
    UiHandler.hpp \
    XmlReader.h \
    downloadmanager.h \
    lookuphelper.hpp \
    player.hpp \
    dnslookup.hpp \
    qmlsignalhandler.hpp

DISTFILES += \
    ../build-HybridRadio-Desktop_Qt_5_7_0_MinGW_32bit-Debug/PlayStrip.PNG \
    countries.csv


unix:!macx: LIBS += -L$$PWD/../qt5pi/lib/ -lqstomp

INCLUDEPATH += $$PWD/../qt5pi/include
DEPENDPATH += $$PWD/../qt5pi/include
