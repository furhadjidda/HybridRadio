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
    void SetChildQmlObject(QObject* childObject)
    {
        mChildObject = childObject;
    }

private:
   // Player *player;
    QObject *mObject;
    QObject *mChildObject;
    XmlReader *mReader;
    DNSLookup *mDnsLookup;
    DataCollector *mCollector;
    MyNetworkAccessManager *mDownloader;
    MyNetworkAccessManager *mPIDownloader;
    StationList mList;
    EpgList mEpgList;
    QString m_CurrentLyPlaying;
    QTimer *mTimer;

    //Network manager
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QNetworkReply *imageReply;
    QProcess *mProcess;
    bool m_IsDownloadedPI;
    QString FormPIString(QString fqdn, QString serviceIdentifier);

};
#endif // MAIN_H

