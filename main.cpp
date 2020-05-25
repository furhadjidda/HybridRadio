#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>
#include <QObject>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include "downloadmanager.h"
#include "qmlsignalhandler.hpp"
#include "XmlReader.h"
#include "player.hpp"
#include "dnslookup.hpp"


int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine   engine;
    XmlReader               reader;   // Reads the SI/XSI xml files.
    DNSLookup dns;
    QQmlComponent component
            (
            &engine,
            QUrl(QStringLiteral("qrc:/main.qml"))
            );

    QObject *object = component.create();
    if (component.isError()) {
        qWarning() << component.errors();
    }

    SignalHandler handler
            (
            object,
            &reader,
            &dns
             ); // Handler that handles the events coming from QML.

    return app.exec();
}
