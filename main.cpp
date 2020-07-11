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
#include "DownloadManager.hpp"
#include "QmlSignalHandler.hpp"
#include "XmlReader.h"
#include "Player.hpp"
#include "DnsLookup.hpp"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine   engine;
    QQmlComponent component
            (
            &engine,
            QUrl(QStringLiteral("qrc:/main.qml"))
            );

    QObject *object = component.create();
    if (component.isError())
    {
        qWarning() << component.errors();
    }

    SignalHandler handler
            (
            object
             ); // Handler that handles the events coming from QML.

    return app.exec();
}
