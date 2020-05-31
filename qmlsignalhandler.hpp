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
#include "UiHandler.hpp"

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
    void OnServiceInformationDownloaded();
    void OnProgramInformationDownloaded();
    void OnStop();
    void OnHttpTextTimeout();
    void OnHttpImageTimeout();
    void HttpTextResponseReceived();
    void HttpImageResponseReceived();
    void MediaStatusChanged(QMediaPlayer::State val);

private:
    QString DownloadProgramInformation(QString fqdn, QString serviceIdentifier);
    void UpdateUIFromList( int aIndex );
    void ShowNoAudioStreamAvaialablePopup( bool val );
    void ConnectSignals();

    Player* mPlayer;
    QObject* mUIObject;
    XmlReader* mReader;
    DNSLookup* mDnsLookup;
    DownloadManager* mServiceInformationDownloader;
    DownloadManager* mProgramInformationDownloader;
    QTimer* mHttpTextTimer;
    QTimer* mHttpImageTimer;
    //Network manager
    QNetworkAccessManager mNetworkManager;
    QNetworkReply* mTextReply;
    QNetworkReply* mImageReply;


    SiDataList mList;
    EpgList mEpgList;
    QString m_CurrentLyPlaying;
    QString mCurrentSelection;
    QString mCurrentBearer;
    // Topic Handling
    QString mTextTopic;
    QString mImageTopic;
    QVariantMap mLastHttpTextResponse;
    QVariantMap mLastHttpImageResponse;

    // UI Handler
    UiHandler mUiHandler{ mUIObject };

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

