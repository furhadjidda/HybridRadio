#ifndef MAIN_H
#define MAIN_H
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>
#include <QObject>
#include <QDebug>
#include "player.hpp"
#include "datacollecter.hpp"
#include "dnslookup.hpp"
#include "xmlparser.hpp"
#include "downloadmanager.h"

class SignalHandler : public QObject
{
Q_OBJECT
public:
    SignalHandler
        (
        QObject *object,
        XmlReader *reader,
        DNSLookup *dnsLookup,
        DataCollector *collector
        );
    virtual ~SignalHandler(){}

signals:
    void sendPlay();
    void sendSelectionChanged( QString value );

public slots:
    void OnPlay();
    void OnSelectionChanged( QString value );
    void OnFileNameAvailable( QString si, QString xsi );
    void OnFileDownloaded();
    void OnStop();


private:
    Player *player;
    QObject *mObject;
    XmlReader *mReader;
    DNSLookup *mDnsLookup;
    DataCollector *mCollector;
    MyNetworkAccessManager *mDownloader;
    StationList mList;
    QString m_CurrentLyPlaying;
};
#endif // MAIN_H

