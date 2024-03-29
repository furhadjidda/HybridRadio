TEMPLATE = app

QT += qml quick multimedia network core websockets sql
CONFIG += c++11
CONFIG += sql

SOURCES += main.cpp \
    DnsLookup.cpp \
    DownloadManager.cpp \
    GccHelper.cpp \
    HttpTransport.cpp \
    HybridRadioCore.cpp \
    LookupHelper.cpp \
    Player.cpp \
    PresetHandler.cpp \
    QmlSignalHandler.cpp \
    StationLocaterHelper.cpp \
    StompTransport.cpp \
    UiHandler.cpp \
    XmlReader.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    DnsLookup.hpp \
    DownloadManager.hpp \
    GccHelper.hpp \
    HttpTransport.hpp \
    HybridRadioCommonTypes.hpp \
    HybridRadioCore.hpp \
    LookupHelper.hpp \
    Player.hpp \
    PresetHandler.hpp \
    QmlSignalHandler.hpp \
    StationLocaterHelper.hpp \
    StompTransport.hpp \
    Transport.hpp \
    UiHandler.hpp \
    XmlReader.h

DISTFILES += \
    ../build-HybridRadio-Desktop_Qt_5_7_0_MinGW_32bit-Debug/PlayStrip.PNG \
    countries.csv \
    hybridRadio.desktop \
    hybridRadio.service \
    pi-code-location-data-usa.csv \
    volumeControl.py \
    volumeControl.service

HOME = /home/furhad/raspi

unix:!macx: LIBS += -L$$HOME/qt5pi/lib -lqstomp

INCLUDEPATH += $$HOME/qt5pi/include
DEPENDPATH += $$HOME/qt5pi/include
