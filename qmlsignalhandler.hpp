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
#include "dnslookup.hpp"
#include "XmlReader.h"
#include "downloadmanager.h"
#include "lookuphelper.hpp"

class SignalHandler : public QObject
{
Q_OBJECT

public:
    SignalHandler
        (
        QObject *object,
        XmlReader *reader,
        DNSLookup *dnsLookup
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
    void OnHttpTextTimeout();
    void OnHttpImageTimeout();
    void HttpTextResponseReceived();
    void HttpImageResponseReceived();
    void mediaStatusChanged(QMediaPlayer::State val);

private:
    QString FormPIString(QString fqdn, QString serviceIdentifier);
    void UpdateUIFromList( int aIndex );
    void ShowNoAudioStreamAvaialablePopup( bool val );
    Player* mPlayer;
    QObject* mUIObject;
    XmlReader* mReader;
    DNSLookup* mDnsLookup;
    DownloadManager* mServiceInformationDownloader;
    DownloadManager* mProgramInformationDownloader;
    SiDataList mList;
    EpgList mEpgList;
    QString m_CurrentLyPlaying;
    QTimer* mHttpTextTimer;
    QTimer* mHttpImageTimer;
    QString mCurrentSelection;
    QString mCurrentBearer;
    //Network manager
    QNetworkAccessManager mNetworkManager;
    QNetworkReply* mTextReply;
    QNetworkReply* mImageReply;
    //QProcess *mProcess;

    // Topic Handling
    QString mTextTopic;
    QString mImageTopic;
    QVariantMap mLastHttpTextResponse;
    QVariantMap mLastHttpImageResponse;

    typedef struct{
        QString mBand;
        QString mFrequency;
        QString mPi;
        QString mSid;
        QString mScids;
        QString mGcc;
        QString mEid;

        void SplitBearerString
            (
            const QString& data,
            StationInformation& aStationInfo,
            QString& aGcc
            )
        {
            DeconstructBearer(aStationInfo, aGcc, data);
            mBand = aStationInfo.mBand;
            mFrequency = aStationInfo.mFrequency;
            mPi = QString::number( aStationInfo.mPi, 16);
            mSid = QString::number( aStationInfo.mSid, 16);
            mScids = QString::number( aStationInfo.mScids, 16);
            mGcc = aGcc;
            mEid = QString::number( aStationInfo.mEid, 16);
        }

    }BearerSplit;

};
#endif // MAIN_H

