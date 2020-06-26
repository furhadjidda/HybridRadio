#include "qmlsignalhandler.hpp"
#include "downloadmanager.h"
#include <QDebug>
#include <stdlib.h>
#include <QProcess>
#include "lookuphelper.hpp"
#include <QtAlgorithms>
#include <QCollator>


static const QString Selecttion_UK_FM("FM(UK)");
static const QString Selecttion_UK_DAB("DAB(UK)");
static const QString Selecttion_DE_FM("FM(DEUTSCHLAND)");
// New
static const QString SelectRadio1("FM(RADIO1 UK)");
static const QString SelectBBC2("FM(BBC-2 UK)");
static const QString SelectBBC4("FM(BBC-4 UK)");
static const QString SelectCoxMedia1("FM(USA)-73978");
static const QString SelectAustralia ("DAB(AUS)");
static const QString SelectNorway ("DAB(NORWAY)");

static const QString ServiceInformationFileName("RadioDns_ServiceInformation.xml");
static const QString ProgramInformationFileName("RadioDns_ProgramInformation.xml");

// PI document link formation http://epg.musicradio.com/radiodns/spi/3.1/fm/ce1/c36b/09630/20160904_PI.xml
// PI document link formation http://epg.musicradio.com/radiodns/spi/3.1/fm/ce1/c479/09580/20160904_PI.xml
// http://epg.musicradio.com/radiodns/spi/3.1/id/www.capitalfm.com/london/20160906_PI.xml
// http://epg.musicradio.com/radiodns/spi/3.1/id/london/20160906_PI.xml
// http://radio-service-information.api.bbci.co.uk/radiodns/spi/3.1/p00fzl86/20170202_PI.xml
// http://<host>:<port>/radiodns/spi/3.1/<ServiceIdentifier>/<date>_PI.xml
// http://radio-service-information.api.bbci.co.uk/radiodns/spi/3.1/id/radiodns.api.bbci.co.uk/p00fzl86/20170202_PI.xml
// http://epg.musicradio.com/radiodns/spi/3.1/id/www.helpmechill.com/chill/20170202_PI.xml

SignalHandler::SignalHandler
    (
    QObject *object, // Object which can be used to set QML properties.
    XmlReader *reader,
    DNSLookup *dnsLookup
    )
    : mUIObject( object )
    , mReader( reader )
    , mDnsLookup( dnsLookup )
{

    // Creating a Timer
    mPlayer = new Player();
    mServiceInformationDownloader = new DownloadManager( ServiceInformationFileName );
    mProgramInformationDownloader = new DownloadManager(ProgramInformationFileName);
    ConnectSignals();
    OnSelectionChanged( SelectCoxMedia1 );
}

void SignalHandler::MediaStatusChanged(QMediaPlayer::State val)
{
    QString status("");

    switch (val)
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
    mUiHandler.SetMediaStatusValue( status );
}



QString SignalHandler::DownloadProgramInformation(QString fqdn, QString serviceIdentifier)
{
    //http://epg.musicradio.com/radiodns/spi/3.1/id/www.helpmechill.com/chill/20170202_PI.xml
    QString date = QDate::currentDate().toString("yyyyMMdd");
    QString urlFormation = "http://" +
            mDnsLookup->GetServiceName() +
            "/radiodns/spi/3.1/id/" +
            fqdn + "/" + serviceIdentifier + "/" + date +"_PI.xml";

    qDebug() << "[HANDLER]  Program Inforamtion URL = " << urlFormation;

    QString fileNameFormation = serviceIdentifier + "_" + date + ".xml";
    // Create a file Name
    mProgramInformationDownloader->SetFileName( fileNameFormation );
    // Download a file
    mProgramInformationDownloader->DownloadFile( urlFormation );
    mUiHandler.QmlMethodInvokeMethodHideEpgPresentImage();
    return urlFormation;
}


void SignalHandler::OnTextChanged( const QString& aText )
{
    mUiHandler.SetSongNameValue( aText );
}

void SignalHandler::OnImageChanged( const QString& aImage )
{
    mUiHandler.SetArtworkValue( aImage );
}


void SignalHandler::OnPlay()
{
    mPlayer->playUrl(m_CurrentLyPlaying);
}

void SignalHandler::OnStop()
{
     mPlayer->Stop();
}


void SignalHandler::OnNext()
{
    ++mCurrentPlayingIndex;
    if( mCurrentPlayingIndex > mList.size() - 1 )
    {
        mCurrentPlayingIndex = 0;
    }
    PlayAtIndex( mCurrentPlayingIndex );
}

void SignalHandler::OnPrevious()
{
    --mCurrentPlayingIndex;
    if( mCurrentPlayingIndex < 0 )
    {
        mCurrentPlayingIndex = mList.size() - 1;
    }
    PlayAtIndex( mCurrentPlayingIndex );
}

void SignalHandler::PlayAtIndex( const qint16 aIndex )
{
    mHttpTransport.ResetTransport();
    ClearMetaData();

    ShowNoAudioStreamAvaialablePopup( (mList[aIndex].mPlayableMedia.size() == 0) );

    if( aIndex > mList.size() - 1 )
    {
        qWarning() << "[HANDLER] List Size = " << mList.size() << "is not in range with Index = " << aIndex;
        return;
    }
    //StopVisTimers();
    mPlayer->Stop();
    m_CurrentLyPlaying = mList[aIndex].mPlayableMedia;
    qDebug() << "\n\n\n[HANDLER] >>> Selecting " + m_CurrentLyPlaying + " @ index " << aIndex << "\n\n";
    mPlayer->playUrl(m_CurrentLyPlaying.toUtf8().constData());
    // This updates the UI with the findings
    UpdateUIFromList( aIndex );
    DownloadProgramInformation( mList[aIndex].mFqdn, mList[aIndex].mServiceIdentifier );
    BearerSplit data;

    // First UnSubscribe from previous topics
    mHttpTransport.UnSubscribeTextTopic( mTextTopic );
    mHttpTransport.UnSubscribeImageTopic( mImageTopic );
    mStompTransport.UnSubscribeTextTopic( mTextTopic );
    mStompTransport.UnSubscribeImageTopic( mImageTopic );

    for(int index = 0; index < mList[aIndex].mBearerInfo.size(); ++index)
    {
        if( mList[aIndex].mBearerInfo[index].mId.length() > 0 )
        {
            StationInformation station;
            data.SplitBearerString(mList[aIndex].mBearerInfo[index].mId,station);
            // May be when selection get the service identifier from the list ??
            // Create New Topics
            ConstructTopic( station, "text", mTextTopic );
            ConstructTopic( station, "image", mImageTopic );
            // Subscribe to New Topics
            mHttpTransport.SubscribeTextTopic( mTextTopic );
            mHttpTransport.SubscribeImageTopic( mImageTopic );
            mStompTransport.SubscribeTextTopic( mTextTopic );
            mStompTransport.SubscribeImageTopic( mImageTopic );
            mCurrentBearer = mList[aIndex].mBearerInfo[index].mId;
            mCurrentPlayingIndex = aIndex;
            break;
        }
    }
}

void SignalHandler::OnSelect( int aIndex )
{
    PlayAtIndex( aIndex );
}

void SignalHandler::OnServiceInformationDownloaded( const QString& aFilePath )
{
    mHttpTransport.ResetTransport();
    mList.clear();
    mReader->ReadSiXmlData( ServiceInformationFileName, mList );
    qDebug() << "[HANDLER] List Size = " << mList.size() << " FilePath = " << aFilePath;
    m_CurrentLyPlaying = "";

    mPlayer->Stop();
    for( int index = 0; index < mList.size(); ++index )
    {
        if( mList[index].mBearerInfo.size() > 0 )
        {
            for( int bearerIndex = 0; bearerIndex < mList[index].mBearerInfo.size(); ++bearerIndex )
            {
                if( mCurrentBearer == mList[index].mBearerInfo[bearerIndex].mId )
                {
                    //qDebug() << "[HANDLER] BearerUri = " << mCurrentBearer;
                    qDebug() << "[HANDLER] Media " << mList[index].mPlayableMedia;
                    mPlayer->playUrl( mList[index].mPlayableMedia.toUtf8().constData() );
                    m_CurrentLyPlaying = mList[index].mPlayableMedia;
                    mCurrentPlayingIndex = index;
                    UpdateUIFromList( index );
                    DownloadProgramInformation( mList[index].mFqdn, mList[index].mServiceIdentifier );
                    break;
                }
            }
        }
    }

    ShowNoAudioStreamAvaialablePopup( ( m_CurrentLyPlaying.size() == 0 ) );
    QCollator collator;
    collator.setNumericMode(true);
    qSort( mList.begin(),mList.end(), [&collator]( const SiData& aArg1, const SiData& aArg2 ){
        return ( collator.compare( aArg1.mServiceName, aArg2.mServiceName ) < 0 );
    });
    mUiHandler.QmlMethodInvokeclearListElement();
    foreach( SiData val,mList )
    {
        mUiHandler.QmlMethodInvokeaddListElement( val );
    }

    //StartVisTimers();
}

void SignalHandler::OnProgramInformationDownloaded( const QString& aFilePath )
{
    if( 0 == aFilePath.size() )
    {
        qWarning() << "[HANDLER] Program information file size is empty";
    }
    mUiHandler.QmlMethodInvokeclearProgramElement();
    EpgList data;
    mReader->ReadPiXmlData( aFilePath, data );

    for( EpgStruct val : data )
    {
        mUiHandler.QmlMethodInvokeaddProgramElement( val );
    }

    if( data.size() > 0 )
    {
       mUiHandler.QmlMethodInvokeMethodDisplayEpgPresentImage();
    }
}

void SignalHandler::ShowNoAudioStreamAvaialablePopup( bool val )
{
    if ( val )
    {
        mUiHandler.QmlMethodInvokeMethoddisplayPopUp();
    }
    else
    {
        mUiHandler.QmlMethodInvokeMethodhidePopUp();
    }
}

void SignalHandler::ConnectSignals()
{

    QObject::connect
            (
            mPlayer,
            SIGNAL(signalMediaStatusChanged(QMediaPlayer::State)),
            this,
            SLOT(MediaStatusChanged(QMediaPlayer::State))
            );

    // Connect Play Button
    QObject *playButton_Object = mUIObject->findChild<QObject*>("Play");
    QObject::connect
            (
            playButton_Object,
            SIGNAL(sendPlay()),
            this,
            SLOT(OnPlay())
            );

    QObject *stopButton_Object = mUIObject->findChild<QObject*>("Stop");
    QObject::connect
            (
            stopButton_Object,
            SIGNAL(sendStop()),
            this,
            SLOT(OnStop())
            );

    QObject *nextButton_Object = mUIObject->findChild<QObject*>("Next");
    QObject::connect
            (
            nextButton_Object,
            SIGNAL(sendNext()),
            this,
            SLOT(OnNext())
            );

    QObject *previousButton_Object = mUIObject->findChild<QObject*>("Previous");
    QObject::connect
            (
            previousButton_Object,
            SIGNAL(sendPrevious()),
            this,
            SLOT(OnPrevious())
            );

    QObject *onSelect_Object = mUIObject->findChild<QObject*>("ServiceListTableObject");
    QObject::connect
            (
            onSelect_Object,
            SIGNAL(selectIndex(int)),
            this,
            SLOT(OnSelect(int))
            );

    // Connect Selection Combo Box
    QObject *comboBox_Object = mUIObject->findChild<QObject*>("selection");
    QObject::connect
            (
            comboBox_Object,
            SIGNAL(sendSelectionChanged(QString)),
            this,
            SLOT(OnSelectionChanged(QString))
            );

    QObject::connect
            (
            mDnsLookup,
            &DNSLookup::SignalServiceInformationAvailable,
            this,
            &SignalHandler::OnServiceInformationAvailable
            );

    // Connect Downloader
    QObject::connect
            (
            mServiceInformationDownloader,
            SIGNAL(sendDownloadComplete( const QString& )),
            this,
            SLOT(OnServiceInformationDownloaded( const QString& ))
            );

    QObject::connect
            (
            mProgramInformationDownloader,
            SIGNAL(sendDownloadComplete( const QString& )),
            this,
            SLOT(OnProgramInformationDownloaded( const QString& ))
            );

    QObject::connect
            (
            &mStompTransport,
            &Transport::SignalTextChanged,
            this,
            &SignalHandler::OnTextChanged
            );

    QObject::connect
            (
            &mStompTransport,
            &Transport::SignalImageChanged,
            this,
            &SignalHandler::OnImageChanged
            );

    QObject::connect
            (
            &mHttpTransport,
            &Transport::SignalTextChanged,
            this,
            &SignalHandler::OnTextChanged
            );

    QObject::connect
            (
            &mHttpTransport,
            &Transport::SignalImageChanged,
            this,
            &SignalHandler::OnImageChanged
            );

    QObject::connect
            (
            mDnsLookup,
            &DNSLookup::SignalHttpVisSupported,
            this,
            &SignalHandler::OnHttpVisSupported
            );

    QObject::connect
            (
            mDnsLookup,
            &DNSLookup::SignalStompVisSupported,
            this,
            &SignalHandler::OnStompVisSupported
            );

    QObject::connect
            (
            mDnsLookup,
            &DNSLookup::SignalStompVisSupported,
            this,
            &SignalHandler::OnStompVisSupported
            );

    QObject::connect
            (
            &mStompTransport,
            &StompTransport::SignalStompConnectionReady,
            this,
            &SignalHandler::OnStompConnectionReady
            );

}


void SignalHandler::OnHttpVisSupported( bool aVal )
{
    if( !aVal )
    {
        qDebug() << "[HANDLER] Http Vis NOT SUPPORTED !! ";
        mUiHandler.QmlMethodInvokeMethodDisplayHttpProtocolSupport(false);
        return;
    }

    mHttpTransport.SetPortAndTarget
            (
            QString::number( mDnsLookup->GetHttpPortNumber() ),
            mDnsLookup->GetHttpTargetName()
            );
    mHttpTransport.SubscribeTextTopic( mTextTopic );
    mHttpTransport.SubscribeImageTopic( mImageTopic );

    isHttpVisSupported = aVal;
    mUiHandler.QmlMethodInvokeMethodDisplayHttpProtocolSupport(true);
}



void SignalHandler::OnStompVisSupported( bool aVal )
{
    if( !aVal )
    {
        qDebug() << "[HANDLER] Stomp Vis NOT SUPPORTED !! ";        
        mUiHandler.QmlMethodInvokeMethodDisplayStompProtocolSupport(false);
        return;
    }
    isStopVisSupported = aVal;

    // TODO Once STOMP is stable - disable Http transport when STOMP is supported !
    mHttpTransport.DisableTransport(); // Disables Http Transport

    mStompTransport.SetPortAndTarget
            (
            QString::number( mDnsLookup->GetStompPortNumber() ),
            mDnsLookup->GetStompTargetName()
            );
    mUiHandler.QmlMethodInvokeMethodDisplayStompProtocolSupport(true);
}



void SignalHandler::OnStompConnectionReady()
{
    mStompTransport.SubscribeTextTopic( mTextTopic );
    mStompTransport.SubscribeImageTopic( mImageTopic );
}


void SignalHandler::UpdateUIFromList( int aIndex )
{
    mUiHandler.SetArtworkValue( mList[aIndex].mArtwork );
    mUiHandler.SetStationNameValue( mList[aIndex].mServiceName );
    mUiHandler.SetStationDescriptionValue( mList[aIndex].mDescription );
    mUiHandler.SetBitrateValue( mList[aIndex].mBitRate );

    QString data("Bearer Info: ");
    for (auto val : mList[aIndex].mBearerInfo) {
        data.append(val.mId);
        data.append(" ; ");
    }
    mUiHandler.QmlMethodInvokeAddMoreInfo( data );
}


void SignalHandler::ClearMetaData()
{
    mUiHandler.SetStationNameValue( "" );
    mUiHandler.SetStationDescriptionValue( "" );
    mUiHandler.SetBitrateValue( "" );
    mUiHandler.SetSongNameValue( "" );
    mUiHandler.SetArtworkValue("");

    QString data("Bearer Info: ");
    mUiHandler.QmlMethodInvokeAddMoreInfo( data );
}


void SignalHandler::OnServiceInformationAvailable( const QString& aFilePath )
{
    qDebug() << "[HANDLER] SI FileName=" << aFilePath;
    mServiceInformationDownloader->DownloadFile( aFilePath );
}


void SignalHandler::OnSelectionChanged(QString value)
{
    StationInformation data;
    QString fqdn;

    qDebug() <<"\n\n\n[HANDLER] OnSelectionChanged :"<<value << "\n\n";
    ClearMetaData();
    mCurrentSelection = value;
    mHttpTransport.UnSubscribeTextTopic( mTextTopic );
    mHttpTransport.UnSubscribeImageTopic( mImageTopic );
    mStompTransport.UnSubscribeTextTopic( mTextTopic );
    mStompTransport.UnSubscribeImageTopic( mImageTopic );
    mStompTransport.ResetTransport();
    if( Selecttion_UK_FM == value )
    {
        data.PopulateFmFields( 9580, 0xc479 );
        data.mGcc = "ce1";
    }
    else if( Selecttion_DE_FM == value )
    {
        data.PopulateFmFields( 10420, 0xd389 );
        data.mGcc = "de0";
    }
    else if( Selecttion_UK_DAB == value )
    {
        // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
        // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
        //static QString DABDemo2_Lookup("0.c7d8.c1ce.ce1.dab.radiodns.org");
        data.PopulateDabFields( 0xc7d8, 0, 0xc1ce) ;
        data.mGcc = "ce1";
    }
    else if( SelectRadio1 == value )
    {
        data.PopulateFmFields( 9770, 0xc201 );
        data.mGcc = "ce1";
    }
    else if( SelectBBC2 == value )
    {
        data.PopulateFmFields( 8810, 0xc202 );
        data.mGcc = "ce1";
    }
    else if( SelectBBC4 == value )
    {
        data.PopulateFmFields( 9320, 0xc204 );
        data.mGcc = "ce1";
    }
    else if( SelectCoxMedia1 == value )
    {
        data.PopulateIbocFields( 0x7426, 0, "292" );
    }
    else if( SelectAustralia == value )
    {
        // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
        // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
        //static QString DABDemo2_Lookup("0.c7d8.c1ce.ce1.dab.radiodns.org");
        data.PopulateDabFields(0x111a,0,0xf003);
        data.mGcc = "1f0";
    }
    else if( SelectNorway == value )
    {
        // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
        // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
        //static QString DABDemo2_Lookup("0.c7d8.c1ce.ce1.dab.radiodns.org");
        data.PopulateDabFields( 0xf204, 0, 0xf801 );
        data.mGcc = "fe2";
    }
    else
    {
        qWarning() << "InValid Selection";
        return;
    }

    ConstructFqdn( data, fqdn );
    mDnsLookup->lookupCName(fqdn);

    mCurrentBearer.clear();
    ConstructBearerUri( data , mCurrentBearer );

    mImageTopic.clear();
    ConstructTopic( data, "image", mImageTopic );

    mTextTopic.clear();
    ConstructTopic( data, "text", mTextTopic );

    mHttpTransport.SubscribeTextTopic( mTextTopic );
    mHttpTransport.SubscribeImageTopic( mImageTopic );
}
