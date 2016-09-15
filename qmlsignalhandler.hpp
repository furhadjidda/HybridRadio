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
    QObject*        mObject; //! Holds Acces to QML objects
    XmlReader*      mReader; //! XML Reader to Read/ Parse all the xml file
    DNSLookup*      mDnsLookup; //! Hold the instance of Dns Lookup
    DataCollector*  mCollector; //! Holds the instance of Data Collector
    Downloader*     mDownloader; //! Holds the instance of Downloader
    StationList     mList; //! Holds the instance of station list
    QString         m_CurrentLyPlaying; //! Holds Currently playing audio link
    QTimer*         mTimer; //! Holds the instance of timer.
    QProcess*       mProcess; //! Holds the Process object.

    // Members for http requests
    QNetworkAccessManager mHttpRequest;
    QNetworkReply *reply;
    QNetworkReply *imageReply;
};
#endif // MAIN_H

