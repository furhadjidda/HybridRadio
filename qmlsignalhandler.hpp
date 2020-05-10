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
//#include "datacollecter.hpp"
#include "dnslookup.hpp"
#include "XmlReader.h"
#include "downloadmanager.h"
#include <vector>
#include <bits/stdc++.h>

class SignalHandler : public QObject
{
Q_OBJECT

public:
    SignalHandler
        (
        QObject *object,
        XmlReader *reader,
        DNSLookup *dnsLookup
        //DataCollector *collector
        );
    virtual ~SignalHandler(){}

signals:
    void sendPlay();
    void sendSelectionChanged( QString value );

public slots:
    void OnPlay();
    void OnNext();
    void OnPrevious();
    void OnSelect( int aIndex );
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
    void mediaStatusChanged(QMediaPlayer::State val);

private:
    Player *player;
    QObject *mObject;
    QObject *mChildObject;
    XmlReader *mReader;
    DNSLookup *mDnsLookup;
    //DataCollector *mCollector;
    MyNetworkAccessManager *mDownloader;
    MyNetworkAccessManager *mPIDownloader;
    SiDataList mList;
    EpgList mEpgList;
    QString m_CurrentLyPlaying;
    QTimer *mTimer;
    QString mCurrentSelection;
    QString mCurrentBearer;
    //Network manager
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QNetworkReply *imageReply;
    QProcess *mProcess;
    bool m_IsDownloadedPI;
    QString FormPIString(QString fqdn, QString serviceIdentifier);
    // Topic Handling
    QString mTextTopic;
    QString mImageTopic;

    typedef struct{
        QString mBand;
        QString mFrequency;
        QString mPi;
        QString mSid;
        QString mScids;
        QString mGcc;

        void SplitBearerString( QString& data)
        {
            QRegExp rx("(\\.|\\:)"); //RegEx for ' ' or ',' or '.' or ':' or '\t'
            QStringList  query= data.split(rx);
            if(query.size() == 4)
            {
                int lastIndex = query.size() - 1;
                mFrequency = query.at(lastIndex);
                mPi = query.at(--lastIndex);
                mGcc = query.at(--lastIndex);
                mBand = query.at(--lastIndex);
            }
            qDebug() << "@@ Splitting Data mBand " << mBand;
            qDebug() << "@@ Splitting Data mPi " << mPi;
            qDebug() << "@@ Splitting Data mGcc " << mGcc;
            qDebug() << "@@ Splitting Data mFrequency " << mFrequency;

        }

    }BearerSplit;

};
#endif // MAIN_H

