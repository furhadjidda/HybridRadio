#include "qmlsignalhandler.hpp"
#include "downloadmanager.h"
#include <QDebug>
#include <stdlib.h>
#include <QProcess>
#include "lookuphelper.hpp"
#include <QtAlgorithms>
#include <QCollator>


static const QString Selecttion_UK_FM("95.8-c479-fm-uk");
static const QString Selecttion_UK_DAB("c7d8-c1ce-0-dab-uk");
static const QString Selecttion_DE_FM("104.2-d389-de");
// New
static const QString SelectRadio1("97.7-c201-radio1");
static const QString SelectBBC2("88.1-c202-uk-bbc2");
static const QString SelectBBC3("93.1-c203-bbc3");
static const QString SelectBBC4("93.2-c204-bbc4");
static const QString SelectCoxMedia1("73978-292-fm-usa");
static const QString SelectAustralia ("111a-f003-1f0-dab-au");
static const QString SelectNorway ("f204-f801-fe2-dab-norway");

static const QString RadioVISMessageID("RadioVIS-Message-ID");
static const QString RadioVISTriggerTime("RadioVIS-Trigger-Time");
const int sTimerValue = 10000;

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
    mHttpTextTimer = new QTimer( this );
    mHttpImageTimer = new QTimer( this );
    mPlayer = new Player();
    mServiceInformationDownloader = new DownloadManager( ServiceInformationFileName );
    mProgramInformationDownloader = new DownloadManager(ProgramInformationFileName);
    ConnectSignals();
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

void SignalHandler::OnHttpTextTimeout()
{
    mHttpTransport.RequestText( mTextTopic );
}

void SignalHandler::OnHttpImageTimeout()
{
    mHttpTransport.RequestImage( mImageTopic );
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
    //mProcess->kill();
    //mProcess->waitForFinished();
    //QString gstreamerCommand = "gst-launch-0.10 -v souphttpsrc location="
    //        + m_CurrentLyPlaying
    //        + " iradio-mode=true ! decodebin name=demux ! audioresample ! audioconvert ! autoaudiosink";
    //mProcess->start(gstreamerCommand);
}

void SignalHandler::OnStop()
{
     mPlayer->Stop();
     //mProcess->kill();
     //mProcess->waitForFinished();
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
    mHttpTransport.ResetTransportResponses();
    ClearMetaData();

    ShowNoAudioStreamAvaialablePopup( (mList[aIndex].mPlayableMedia.size() == 0) );

    if( aIndex > mList.size() - 1 )
    {
        qWarning() << "[HANDLER] List Size = " << mList.size() << "is not in range with Index = " << aIndex;
        return;
    }
    StopVisTimers();
    mPlayer->Stop();
    m_CurrentLyPlaying = mList[aIndex].mPlayableMedia;
    qDebug() << "[HANDLER] Selecting " + m_CurrentLyPlaying + "@ index " << aIndex;
    mPlayer->playUrl(m_CurrentLyPlaying.toUtf8().constData());
    // This updates the UI with the findings
    UpdateUIFromList( aIndex );
    DownloadProgramInformation( mList[aIndex].mFqdn, mList[aIndex].mServiceIdentifier );
    BearerSplit data;

    for(int index = 0; index < mList[aIndex].mBearerInfo.size(); ++index)
    {
        if( mList[aIndex].mBearerInfo[index].mId.length() > 0 )
        {
            StationInformation station;
            QString gcc;
            data.SplitBearerString(mList[aIndex].mBearerInfo[index].mId,station,gcc);
            // May be when selection get the service identifier from the list ??
            ConstructTopic( station, gcc, "text", mTextTopic );
            ConstructTopic( station, gcc, "image", mImageTopic );

            mCurrentBearer = mList[aIndex].mBearerInfo[index].mId;
            mCurrentPlayingIndex = aIndex;
            StartVisTimers();
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
    mHttpTransport.ResetTransportResponses();
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
                    qDebug() << "[HANDLER] BearerUri = " << mCurrentBearer;
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

    StartVisTimers();
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
            mHttpTextTimer,
            SIGNAL(timeout()),
            this,
            SLOT(OnHttpTextTimeout())
            );
    QObject::connect
            (
            mHttpImageTimer,
            SIGNAL(timeout()),
            this,
            SLOT(OnHttpImageTimeout())
            );

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
            &mHttpTransport,
            &HttpTransport::SignalTextChanged,
            this,
            &SignalHandler::OnTextChanged
            );

    QObject::connect
            (
            &mHttpTransport,
            &HttpTransport::SignalImageChanged,
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
    }

    mHttpTransport.SetPortAndTarget
            (
            QString::number( mDnsLookup->GetHttpPortNumber() ),
            mDnsLookup->GetHttpTargetName()
            );
    mHttpTransport.RequestText( mTextTopic );
    mHttpTransport.RequestImage( mImageTopic );

    isHttpVisSupported = aVal;
    StopVisTimers();
}

void SignalHandler::OnStompVisSupported( bool aVal )
{
    if( !aVal )
    {
        qDebug() << "[HANDLER] Stomp Vis NOT SUPPORTED !! ";
    }
    isStopVisSupported = aVal;

    mStompTransport.SetPortAndTarget
            (
            QString::number( mDnsLookup->GetStompPortNumber() ),
            mDnsLookup->GetStompTargetName()
            );
}

void SignalHandler::OnStompConnectionReady()
{
    mStompTransport.RequestText( mTextTopic );
    mStompTransport.RequestImage( mImageTopic );
}

void SignalHandler::StartVisTimers()
{
    if( isHttpVisSupported )
    {
        mHttpTextTimer->start(sTimerValue);
        mHttpImageTimer->start(sTimerValue);
    }
}

void SignalHandler::StopVisTimers()
{
    mHttpTextTimer->stop();
    mHttpImageTimer->stop();
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
    mUiHandler.SetMoreInfoValue( data );
}

void SignalHandler::ClearMetaData()
{
    mUiHandler.SetStationNameValue( "" );
    mUiHandler.SetStationDescriptionValue( "" );
    mUiHandler.SetBitrateValue( "" );

    QString data("Bearer Info: ");
    mUiHandler.SetMoreInfoValue( data );
}


void SignalHandler::OnServiceInformationAvailable( const QString& aFilePath )
{
    qDebug() << "[HANDLER] SI FileName=" << aFilePath;
    mServiceInformationDownloader->DownloadFile( aFilePath );
}


void SignalHandler::OnSelectionChanged(QString value)
{
    qDebug() <<"[HANDLER] OnSelectionChanged :"<<value;
    ClearMetaData();
    mCurrentSelection = value;
    if( Selecttion_UK_FM == value )
    {
        StationInformation data;
        QString fqdn;
        data.PopulateFmFields( 9580, 0xc479 );
        ConstructFqdn( data, "ce1", fqdn );
        mDnsLookup->lookupCName( fqdn );

        mCurrentBearer.clear();
        ConstructBearerUri(data,"ce1",mCurrentBearer);

        mImageTopic.clear();
        ConstructTopic(data,"ce1","image",mImageTopic);

        mTextTopic.clear();
        ConstructTopic(data,"ce1","text",mTextTopic);
    }
    else if( Selecttion_DE_FM == value )
    {
        StationInformation data;
        QString fqdn;
        data.PopulateFmFields(10420,0xd389);
        ConstructFqdn(data,"de0",fqdn);
        mDnsLookup->lookupCName(fqdn);

        mCurrentBearer.clear();
        ConstructBearerUri(data,"de0",mCurrentBearer);

        mImageTopic.clear();
        ConstructTopic(data,"de0","image",mImageTopic);

        mTextTopic.clear();
        ConstructTopic(data,"ce1","text",mTextTopic);
    }
    else if( Selecttion_UK_DAB == value )
    {
        // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
        // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
        //static QString DABDemo2_Lookup("0.c7d8.c1ce.ce1.dab.radiodns.org");
        StationInformation data;
        QString fqdn;
        data.PopulateDabFields(0xc7d8,0,0xc1ce);
        ConstructFqdn(data,"ce1",fqdn);
        mDnsLookup->lookupCName(fqdn);

        mCurrentBearer.clear();
        ConstructBearerUri(data,"de0",mCurrentBearer);

        mImageTopic.clear();
        ConstructTopic(data,"de0","image",mImageTopic);

        mTextTopic.clear();
        ConstructTopic(data,"de0","text",mTextTopic);
    }
    else if( SelectRadio1 == value )
    {
        StationInformation data;
        QString fqdn;
        data.PopulateFmFields(9770,0xc201);
        ConstructFqdn(data,"ce1",fqdn);
        mDnsLookup->lookupCName(fqdn);

        mCurrentBearer.clear();
        ConstructBearerUri(data,"ce1",mCurrentBearer);

        mImageTopic.clear();
        ConstructTopic(data,"ce1","image",mImageTopic);

        mTextTopic.clear();
        ConstructTopic(data,"ce1","text",mTextTopic);
    }
    else if( SelectBBC2 == value )
    {
        StationInformation data;
        QString fqdn;
        data.PopulateFmFields(8810,0xc202);
        ConstructFqdn(data,"ce1",fqdn);
        mDnsLookup->lookupCName(fqdn);

        mCurrentBearer.clear();
        ConstructBearerUri(data,"ce1",mCurrentBearer);

        mImageTopic.clear();
        ConstructTopic(data,"ce1","image",mImageTopic);

        mTextTopic.clear();
        ConstructTopic(data,"ce1","text",mTextTopic);
    }
    else if( SelectBBC3 == value )
    {
        StationInformation data;
        QString fqdn;
        data.PopulateFmFields(9310,0xc203);
        ConstructFqdn(data,"ce1",fqdn);
        mDnsLookup->lookupCName(fqdn);

        mCurrentBearer.clear();
        ConstructBearerUri(data,"ce1",mCurrentBearer);

        mImageTopic.clear();
        ConstructTopic(data,"ce1","image",mImageTopic);

        mTextTopic.clear();
        ConstructTopic(data,"ce1","text",mTextTopic);
    }
    else if( SelectBBC4 == value )
    {
        StationInformation data;
        QString fqdn;
        data.PopulateFmFields(9320,0xc204);
        ConstructFqdn(data,"ce1",fqdn);
        mDnsLookup->lookupCName(fqdn);

        mCurrentBearer.clear();
        ConstructBearerUri(data,"ce1",mCurrentBearer);

        mImageTopic.clear();
        ConstructTopic(data,"ce1","image",mImageTopic);

        mTextTopic.clear();
        ConstructTopic(data,"ce1","text",mTextTopic);
    }
    else if( SelectCoxMedia1 == value )
    {
        StationInformation data;
        QString fqdn;
        data.PopulateIbocFields(0x7426,0,"292");
        ConstructFqdn(data,"",fqdn);
        mDnsLookup->lookupCName(fqdn);

        mCurrentBearer.clear();
        ConstructBearerUri(data,"",mCurrentBearer);

        mImageTopic.clear();
        ConstructTopic(data,"","image",mImageTopic);

        mTextTopic.clear();
        ConstructTopic(data,"","text",mTextTopic);
    }
    else if( SelectAustralia == value )
    {
        // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
        // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
        //static QString DABDemo2_Lookup("0.c7d8.c1ce.ce1.dab.radiodns.org");
        StationInformation data;
        QString fqdn;
        data.PopulateDabFields(0x111a,0,0xf003);
        ConstructFqdn(data,"1f0",fqdn);
        mDnsLookup->lookupCName(fqdn);

        mCurrentBearer.clear();
        ConstructBearerUri(data,"1f0",mCurrentBearer);

        mImageTopic.clear();
        ConstructTopic(data,"1f0","image",mImageTopic);

        mTextTopic.clear();
        ConstructTopic(data,"1f0","text",mTextTopic);
    }
    else if( SelectNorway == value )
    {
        // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
        // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
        //static QString DABDemo2_Lookup("0.c7d8.c1ce.ce1.dab.radiodns.org");
        StationInformation data;
        QString fqdn;
        data.PopulateDabFields(0xf204,0,0xf801);
        ConstructFqdn(data,"fe2",fqdn);
        mDnsLookup->lookupCName(fqdn);
        qDebug() << fqdn;

        mCurrentBearer.clear();
        ConstructBearerUri(data,"fe2",mCurrentBearer);

        mImageTopic.clear();
        ConstructTopic(data,"fe2","image",mImageTopic);

        mTextTopic.clear();
        ConstructTopic(data,"fe2","text",mTextTopic);
    }
}
