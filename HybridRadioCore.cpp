#include "HybridRadioCore.hpp"

static const QString ServiceInformationFileName("RadioDns_ServiceInformation.xml");
static const QString ProgramInformationFileName("RadioDns_ProgramInformation.xml");

HybridRadioCore::HybridRadioCore()
    : mServiceInformationDownloader( new DownloadManager( ServiceInformationFileName ) )
    , mProgramInformationDownloader( new DownloadManager( ProgramInformationFileName ) )
    , mPlayer( new Player() )
    , mReader( new XmlReader() )
    , mDnsLookup( new DNSLookup() )
    , mHttpTransport( static_cast<HttpTransport*>( new HttpTransport() ) )
    , mStompTransport( static_cast<StompTransport*>(new StompTransport() ) )
{
}

HybridRadioCore::~HybridRadioCore()
{
}

void HybridRadioCore::InitializeCore()
{
    ConnectSignals();
}

void HybridRadioCore::GetServiceInformationList( SiDataList& aSiList )
{
    aSiList = mList;
}

void HybridRadioCore::GetProgramInformationData( EpgList& aEpgList )
{
    aEpgList = mProgramList;
}

qint32 HybridRadioCore::GetCurrentServiceIndex()
{
    return mCurrentPlayingIndex;
}

qint32 HybridRadioCore::PlayNextServiceIndex()
{
    ++mCurrentPlayingIndex;
    if( mCurrentPlayingIndex > mList.size() - 1 )
    {
        mCurrentPlayingIndex = 0;
    }
    PlayServiceAtIndex( mCurrentPlayingIndex );
    return mCurrentPlayingIndex;
}

qint32 HybridRadioCore::PlayPreviousServiceIndex()
{
    --mCurrentPlayingIndex;
    if( mCurrentPlayingIndex < 0 )
    {
        mCurrentPlayingIndex = mList.size() - 1;
    }
    PlayServiceAtIndex( mCurrentPlayingIndex );
    return mCurrentPlayingIndex;
}

void HybridRadioCore::PlayServiceAtIndex( qint32 aIndex )
{
    mHttpTransport->ResetTransport();

    emit SignalAudioStreamAvailability( (mList[aIndex].mPlayableMedia.size() == 0) );

    if( aIndex > mList.size() - 1 )
    {
        qWarning() << "[HYBRID_CORE] List Size = " << mList.size() << "is not in range with Index = " << aIndex;
        return;
    }
    //StopVisTimers();
    mPlayer->Stop();
    mCurrentPlayingUrl = mList[aIndex].mPlayableMedia;
    qDebug() << "[HYBRID_CORE] >>> Selecting " + mCurrentPlayingUrl + " @ index " << aIndex << "\n\n";
    qDebug() << "[HYBRID_CORE] >>> station = " << mList[aIndex].FormattedData();
    mPlayer->playUrl( mCurrentPlayingUrl.toUtf8().constData() );

    // This updates the UI with the findings
    SignalMetaDataAtIndex( aIndex );
    DownloadProgramInformation( mList[aIndex].mFqdn, mList[aIndex].mServiceIdentifier );

    // First UnSubscribe from previous topics
    mHttpTransport->UnSubscribeTextTopic( mTextTopic );
    mHttpTransport->UnSubscribeImageTopic( mImageTopic );
    mStompTransport->UnSubscribeTextTopic( mTextTopic );
    mStompTransport->UnSubscribeImageTopic( mImageTopic );

    for(int index = 0; index < mList[aIndex].mBearerInfo.size(); ++index)
    {
        if( mList[aIndex].mBearerInfo[index].mId.length() > 0 )
        {
            BearerSplit data;
            StationInformation station;
            data.SplitBearerString( mList[aIndex].mBearerInfo[index].mId,station );
            // May be when selection get the service identifier from the list ??
            // Create New Topics
            ConstructTopic( station, "text", mTextTopic );
            ConstructTopic( station, "image", mImageTopic );
            // Subscribe to New Topics
            mHttpTransport->SubscribeTextTopic( mTextTopic );
            mHttpTransport->SubscribeImageTopic( mImageTopic );
            mStompTransport->SubscribeTextTopic( mTextTopic );
            mStompTransport->SubscribeImageTopic( mImageTopic );
            mCurrentBearer = mList[aIndex].mBearerInfo[index].mId;
            mCurrentPlayingIndex = aIndex;
            break;
        }
    }
}

void HybridRadioCore::StartPlayingCurrentAudio()
{
    mPlayer->playUrl( mCurrentPlayingUrl );
}

void HybridRadioCore::StopPlayingCurrentAudio()
{
    mPlayer->Stop();
}

void HybridRadioCore::LookForStation( const StationInformation& aStationData )
{
    QString fqdn;
    mTextTopic.clear();
    mImageTopic.clear();
    mCurrentBearer.clear();
    mHttpTransport->UnSubscribeTextTopic( mTextTopic );
    mHttpTransport->UnSubscribeImageTopic( mImageTopic );
    mStompTransport->UnSubscribeTextTopic( mTextTopic );
    mStompTransport->UnSubscribeImageTopic( mImageTopic );
    mStompTransport->ResetTransport();

    ConstructFqdn( aStationData, fqdn );
    mDnsLookup->lookupCName(fqdn);
    qDebug() << " [LookForStation] -> " << aStationData.mBand;
    ConstructBearerUri( aStationData, mCurrentBearer );    
    qDebug() << " [LookForStation] -> " << mCurrentBearer;
    ConstructTopic( aStationData, "image", mImageTopic );
    ConstructTopic( aStationData, "text", mTextTopic );
}

void HybridRadioCore::OnServiceInformationAvailable( const QString& aFilePath )
{
    qDebug() << "[HYBRID_CORE] SI FileName=" << aFilePath;
    mServiceInformationDownloader->DownloadFile( aFilePath );
}
void HybridRadioCore::OnServiceInformationDownloaded( const QString& aFilePath )
{
    mHttpTransport->ResetTransport();
    mList.clear();
    mReader->ReadSiXmlData( ServiceInformationFileName, mList );
    qDebug() << "[HYBRID_CORE] List Size = " << mList.size() << " FilePath = " << aFilePath;
    mCurrentPlayingUrl = "";

    mPlayer->Stop();

    // This part looks for current Bearer
    qDebug() << endl << " >> Looking for bearer" << mCurrentBearer;
    for( int index = 0; index < mList.size(); ++index )
    {
        if( mList[index].mBearerInfo.size() > 0 )
        {
            for( int bearerIndex = 0; bearerIndex < mList[index].mBearerInfo.size(); ++bearerIndex )
            {
                if( mCurrentBearer == mList[index].mBearerInfo[bearerIndex].mId )
                {
                    //qDebug() << "[HYBRID_CORE] BearerUri = " << mCurrentBearer;
                    qDebug() << "[HYBRID_CORE] Media " << mList[index].mPlayableMedia;
                    mPlayer->playUrl( mList[index].mPlayableMedia.toUtf8().constData() );
                    mCurrentPlayingUrl = mList[index].mPlayableMedia;
                    mCurrentPlayingIndex = index;
                    SignalMetaDataAtIndex( index );
                    emit SignalStationFound( mList[index] );
                    DownloadProgramInformation( mList[index].mFqdn, mList[index].mServiceIdentifier );
                    break;
                }
            }
        }
    }

    emit SignalAudioStreamAvailability( ( mCurrentPlayingUrl.size() == 0 ) );

    // This is for sorting
    QCollator collator;
    collator.setNumericMode(true);
    qSort( mList.begin(),mList.end(), [&collator]( const SiData& aArg1, const SiData& aArg2 ){
        return ( collator.compare( aArg1.mServiceName, aArg2.mServiceName ) < 0 );
    });

    emit SignalServiceInformationDownloaded();
}

void HybridRadioCore::OnProgramInformationDownloaded( const QString& aFilePath )
{
    if( 0 == aFilePath.size() )
    {
        qWarning() << "[HYBRID_CORE] Program information file size is empty";
    }

    mReader->ReadPiXmlData( aFilePath, mProgramList );

    emit SignalProgramInformationDownloaded();
}

void HybridRadioCore::OnTextChanged( const QString& aData )
{
    emit SignalTextChanged( aData );
}

void HybridRadioCore::OnImageChanged( const QString& aData )
{
    emit SignalImageChanged( aData );
}

void HybridRadioCore::MediaStateChanged(QMediaPlayer::State val)
{
    QString status("");

    switch ( val )
    {
        case QMediaPlayer::State::PlayingState :
            status = "Playing";
        break;

        case QMediaPlayer::State::StoppedState :
            status = "Stopped";
        break;

        case QMediaPlayer::State::PausedState :
            status = "Paused";
        break;

        default:
            break;

    }
    emit SignalMediaStateChanged( status );
}

void HybridRadioCore::OnHttpVisSupported( bool aVal )
{
    if( !aVal )
    {
        qDebug() << "[HYBRID_CORE] Http Vis NOT SUPPORTED !! ";
        emit SignalHttpTransportSupport( false );
        return;
    }

    mHttpTransport->SetPortAndTarget
            (
            QString::number( mDnsLookup->GetHttpPortNumber() ),
            mDnsLookup->GetHttpTargetName()
            );

    mHttpTransport->SubscribeTextTopic( mTextTopic );
    mHttpTransport->SubscribeImageTopic( mImageTopic );

    emit SignalHttpTransportSupport( true );
}

void HybridRadioCore::OnStompVisSupported( bool aVal )
{
    if( !aVal )
    {
        qDebug() << "[HYBRID_CORE] Stomp Vis NOT SUPPORTED !! ";
        emit SignalStompTransportSupport( false );
        return;
    }

    mHttpTransport->DisableTransport(); // Disables Http Transport if STOMP is supported

    mStompTransport->SetPortAndTarget
            (
            QString::number( mDnsLookup->GetStompPortNumber() ),
            mDnsLookup->GetStompTargetName()
            );

    emit SignalStompTransportSupport( true );
}

void HybridRadioCore::OnStompConnectionReady()
{
    qDebug() << "[HYBRID_CORE] Stomp Ready";
    mStompTransport->SubscribeTextTopic( mTextTopic );
    mStompTransport->SubscribeImageTopic( mImageTopic );
}

void HybridRadioCore::ConnectSignals()
{
    QObject::connect
            (
            mPlayer.get(),
            &Player::signalMediaStatusChanged,
            this,
            &HybridRadioCore::MediaStateChanged
            );

    QObject::connect
            (
            mDnsLookup.get(),
            &DNSLookup::SignalServiceInformationAvailable,
            this,
            &HybridRadioCore::OnServiceInformationAvailable
            );

    // Connect Downloader
    QObject::connect
            (
            mServiceInformationDownloader.get(),
            &DownloadManager::sendDownloadComplete,
            this,
            &HybridRadioCore::OnServiceInformationDownloaded
            );

    QObject::connect
            (
            mProgramInformationDownloader.get(),
            &DownloadManager::sendDownloadComplete,
            this,
            &HybridRadioCore::OnProgramInformationDownloaded
            );

    QObject::connect
            (
            mStompTransport.get(),
            &Transport::SignalTextChanged,
            this,
            &HybridRadioCore::OnTextChanged
            );

    QObject::connect
            (
            mStompTransport.get(),
            &Transport::SignalImageChanged,
            this,
            &HybridRadioCore::OnImageChanged
            );

    QObject::connect
            (
            static_cast<StompTransport*>( mStompTransport.get() ),
            &StompTransport::SignalStompConnectionReady,
            this,
            &HybridRadioCore::OnStompConnectionReady
            );

    QObject::connect
            (
            mHttpTransport.get(),
            &Transport::SignalTextChanged,
            this,
            &HybridRadioCore::OnTextChanged
            );

    QObject::connect
            (
            mHttpTransport.get(),
            &Transport::SignalImageChanged,
            this,
            &HybridRadioCore::OnImageChanged
            );

    QObject::connect
            (
            mDnsLookup.get(),
            &DNSLookup::SignalHttpVisSupported,
            this,
            &HybridRadioCore::OnHttpVisSupported
            );

    QObject::connect
            (
            mDnsLookup.get(),
            &DNSLookup::SignalStompVisSupported,
            this,
            &HybridRadioCore::OnStompVisSupported
            );
}

void HybridRadioCore::LookforBearerAtIndex
    (
    qint32 aIndex,
    BearerList& aBearerList
    )
{
    for( auto bearerData : mList[aIndex].mBearerInfo )
    {
        if( bearerData.mId.length() > 0 )
        {
            aBearerList.push_back( bearerData );
        }
    }
}


void HybridRadioCore::SignalMetaDataAtIndex( qint32 aIndex )
{
    emit SignalImageChanged( mList[aIndex].mArtwork );
    emit SignalStationNameChanged( mList[aIndex].mServiceName );
    emit SignalDescriptionChanged( mList[aIndex].mDescription );
    emit SignalBitRateChanged( mList[aIndex].mBitRate );
    emit SignalTextChanged( "" );
}


qint32 HybridRadioCore::SearchForBearer( const QString& aBearerString )
{

    for( int index = 0; index < mList.size(); ++index )
    {
        if( mList[index].mBearerInfo.size() > 0 )
        {
            for( auto bearerData : mList[index].mBearerInfo )
            {
                if( aBearerString == bearerData.mId )
                {
                    qDebug() << "[HYBRID_CORE] BearerUri = " << aBearerString;
                    return index;
                    break;
                }
            }
        }
    }
    // This means nothing was found
    return -1;
}

QString HybridRadioCore::DownloadProgramInformation
    (
    QString aFqdn,
    QString aServiceIdentifier
    )
{
    //http://epg.musicradio.com/radiodns/spi/3.1/id/www.helpmechill.com/chill/20170202_PI.xml
    QString date = QDate::currentDate().toString("yyyyMMdd");
    QString urlFormation = "http://" +
            mDnsLookup->GetServiceName() +
            "/radiodns/spi/3.1/id/" +
            aFqdn + "/" + aServiceIdentifier + "/" + date +"_PI.xml";

    qDebug() << "[HYBRID_CORE]  Program Inforamtion URL = " << urlFormation;

    QString fileNameFormation = aServiceIdentifier + "_" + date + ".xml";
    // Create a file Name
    mProgramInformationDownloader->SetFileName( fileNameFormation );
    // Download a file
    mProgramInformationDownloader->DownloadFile( urlFormation );
    return urlFormation;
}

bool HybridRadioCore::SearchForStationInServiceInformationList
    (
    const StationInformation& aStationData,
    SiData& aData
    )
{
    QString bearerUri;
    ConstructBearerUri( aStationData, bearerUri );
    for( auto siData : mList )
    {
        for( auto bearerData : siData.mBearerInfo )
        {
            if( bearerUri == bearerData.mId )
            {
                aData = siData;
            }
        }
    }
}