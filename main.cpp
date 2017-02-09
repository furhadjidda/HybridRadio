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
#include "xmlparser.hpp"
#include "player.hpp"
#include "datacollecter.hpp"
#include "dnslookup.hpp"


int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine   engine;
    DataCollector           collector;              // Instance which collects the metadata and
                                                    // services from the SI /XSI files.
    XmlReader               reader( &collector );   // Reads the SI/XSI xml files.
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

    QQmlComponent component2
            (
            &engine,
            QUrl(QStringLiteral("qrc:/child.qml"))
            );

    QObject *object2 = component2.create();
    if (component2.isError()) {
        qWarning() << component.errors();
    }

    SignalHandler handler
            (
            object,
            &reader,
            &dns,
            &collector
             ); // Handler that handles the events coming from QML.

    handler.SetChildQmlObject(object2);


    reader.ReadSiXmlData(QString("myxml.xml"));

    return app.exec();
}
