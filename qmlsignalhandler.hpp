#ifndef MAIN_H
#define MAIN_H
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>
#include <QObject>
#include <QDebug>
#include <QUrl>
#include <QTimer>
#include <QTimerEvent>
#include <QtNetwork>
#include <QTimer>
#include <QTimerEvent>
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
    void OnTimeout();
    void httpFinished();
    void httpImageFinished();


private:
   // Player *player;
    QObject *mObject;
    XmlReader *mReader;
    DNSLookup *mDnsLookup;
    DataCollector *mCollector;
    MyNetworkAccessManager *mDownloader;
    StationList mList;
    QString m_CurrentLyPlaying;
    QTimer *mTimer;

    //Network manager
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QNetworkReply *imageReply;
    QProcess *mProcess;
};
#endif // MAIN_H

