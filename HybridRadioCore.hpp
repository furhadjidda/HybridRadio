#ifndef HYBRIDRADIOCORE_HPP
#define HYBRIDRADIOCORE_HPP

#include <QObject>
#include <memory>
#include "HybridRadioCommonTypes.hpp"
#include "lookuphelper.hpp"
#include "player.hpp"
#include "dnslookup.hpp"
#include "XmlReader.h"
#include "downloadmanager.h"
#include "HttpTransport.hpp"
#include "StompTransport.hpp"
#include "GccHelper.hpp"

class HybridRadioCore
         : public QObject
{
    Q_OBJECT
public:
    HybridRadioCore();

    virtual ~HybridRadioCore();

    void InitializeCore();

    void GetServiceInformationList( SiDataList& aSiList );

    void GetProgramInformationData( EpgList& aEpgList );

    qint32 GetCurrentServiceIndex();

    qint32 PlayNextServiceIndex();

    qint32 PlayPreviousServiceIndex();

    void PlayMedia( const QString& aMediaLink );

    void PlayServiceAtIndex( qint32 aIndex );

    void StartPlayingCurrentAudio();

    void StopPlayingCurrentAudio();

    void LookForStation( const StationInformation& aStationData );

    bool SearchForStationInServiceInformationList
        (
        const StationInformation& aStationData,
        SiData& aData
        );

signals:
    void SignalStationNameChanged( const QString& aData );
    void SignalDescriptionChanged( const QString& aData );
    void SignalBitRateChanged( const QString& aData );
    void SignalTextChanged( const QString& aData );
    void SignalImageChanged( const QString& aData );

    void SignalMediaStateChanged( const QString& aData );
    void SignalServiceInformationDownloaded();
    void SignalProgramInformationDownloaded();
    void SignalGccValueNotFound();
    void SignalFqdnNotFound();
    void SignalAudioStreamAvailability( bool aStatus );
    void SignalHttpTransportSupport( bool aStatus );
    void SignalStompTransportSupport( bool aStatus );
    void SignalStationFound( const SiData& aData );

public slots:
    void OnServiceInformationAvailable( const QString& aFilePath );
    void OnServiceInformationDownloaded( const QString& aFilePath );
    void OnProgramInformationDownloaded( const QString& aFilePath );
    void OnTextChanged( const QString& aData );
    void OnImageChanged( const QString& aData );
    void MediaStateChanged( QMediaPlayer::State aState );
    void OnHttpVisSupported( bool aVal );
    void OnStompVisSupported( bool aVal );
    void OnStompConnectionReady();
    void OnStompDisconnected();

private:
    void ConnectSignals();

    void LookforBearerAtIndex
        (
        qint32 aIndex,
        BearerList& aBearerList
        );
    qint32 SearchForBearer( const QString& aBearerString );
    void SignalMetaDataAtIndex( qint32 aIndex );
    QString DownloadProgramInformation
        (
        QString aFqdn,
        QString aServiceIdentifier
        );

    std::unique_ptr<DownloadManager> mServiceInformationDownloader;
    std::unique_ptr<DownloadManager> mProgramInformationDownloader;
    std::unique_ptr<Player> mPlayer;
    std::unique_ptr<XmlReader> mReader;
    std::unique_ptr<DNSLookup> mDnsLookup;
    std::unique_ptr<Transport> mHttpTransport;
    std::unique_ptr<Transport> mStompTransport;

    // Metadata Handling parameters
    SiDataList mList;
    EpgList mProgramList;
    QString mCurrentPlayingUrl;
    QString mCurrentSelection;
    QString mCurrentBearer;
    qint32 mCurrentPlayingIndex{0};
    // Topic Handling
    QString mTextTopic;
    QString mImageTopic;
    GccHelper mGccHelper;
};

#endif // HYBRIDRADIOCORE_HPP
