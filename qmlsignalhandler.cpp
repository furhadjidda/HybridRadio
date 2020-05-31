#include "qmlsignalhandler.hpp"
#include "downloadmanager.h"
#include <QDebug>
#include <stdlib.h>
#include <QProcess>
#include "lookuphelper.hpp"

static const QString Selecttion_UK_FM("UK");
static const QString Selecttion_UK_DAB("UK-DAB");
static const QString Selecttion_DE_FM("DE");
static const QString RadioVISMessageID("RadioVIS-Message-ID");
static const QString RadioVISTriggerTime("RadioVIS-Trigger-Time");
const int sTimerValue = 5000;

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
    qDebug() << "url Formation for PI xml = " << urlFormation;
    QString fileNameFormation = serviceIdentifier + "_" + date + ".xml";
    mProgramInformationDownloader->SetFileName( fileNameFormation );
    mProgramInformationDownloader->DownloadFile( urlFormation );
    return urlFormation;
}

void SignalHandler::OnHttpTextTimeout()
{
    // Construct a url for text topic
    mHttpTextTimer->stop();
    QString lastTextId = mLastHttpTextResponse.find( RadioVISMessageID ).value().toString(); // This is mandatory if not initial request;
    if( 0 != lastTextId.size() )
    {
        lastTextId = lastTextId.remove(0,3);
        lastTextId = "&last_id=" + lastTextId;
    }
    QString httpTextReq = "http://" +
                           mDnsLookup->GetHttpTargetName() + ":" +
                           QString::number(mDnsLookup->GetHttpPortNumber())
                           + "/radiodns/vis/vis.json?topic=/topic/"+ mTextTopic + lastTextId;

    mDnsLookup->GetHttpTargetName();
    QUrl httpTextUrlReq( httpTextReq );
    qDebug() << httpTextReq;
    mTextReply = mNetworkManager.get( QNetworkRequest( httpTextUrlReq ) );

    QObject::connect
            (
            mTextReply,
            &QNetworkReply::finished,
            this,
            &SignalHandler::HttpTextResponseReceived
            );
}

void SignalHandler::OnHttpImageTimeout()
{
    // Construct a url for image topic
    mHttpImageTimer->stop();

    // Construct a url
    QString lastImageId = mLastHttpImageResponse.find( RadioVISMessageID ).value().toString(); // This is mandatory if not initial request;
    if( 0 != lastImageId.size() )
    {
        lastImageId = lastImageId.remove(0,3);
        lastImageId = "&last_id=" + lastImageId;
    }
    QString httpImageReq = "http://" +
                            mDnsLookup->GetHttpTargetName() + ":" +
                            QString::number(mDnsLookup->GetHttpPortNumber())
                            + "/radiodns/vis/vis.json?topic=/topic/" + mImageTopic + lastImageId;

    mDnsLookup->GetHttpTargetName();
    QUrl httpImageUrlReq( httpImageReq );
    qDebug() << httpImageReq;
    mImageReply = mNetworkManager.get( QNetworkRequest( httpImageUrlReq ) );
    QObject::connect
            (
            mImageReply,
            &QNetworkReply::finished,
            this,
            &SignalHandler::HttpImageResponseReceived
            );
}

void SignalHandler::HttpImageResponseReceived()
{
    mHttpImageTimer->start( sTimerValue );
    QString imageJson = QString(mImageReply->readAll().data());
    qDebug() <<"Http Image Response : " << imageJson << endl;

    if( 0 == imageJson.size() )
    {
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson( imageJson.toUtf8() );
    QJsonObject jsonObject = jsonDoc.object();
    QVariantMap jsonQVariantMap = jsonObject.toVariantMap();
    QVariantMap  jsonHeaderMap = jsonQVariantMap["headers"].toMap();
    mLastHttpImageResponse = jsonHeaderMap;


    QString value = jsonObject["body"].toString();
    QString artLink = value.remove(0,5);
    mUiHandler.SetArtworkValue( artLink );
}


void SignalHandler::HttpTextResponseReceived()
{
    mHttpTextTimer->start( sTimerValue );
    QString textJson( mTextReply->readAll().data() );

    qDebug() <<"Http Text Response : " << textJson << endl;
    if( 0 == textJson.size() )
    {
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(textJson.toUtf8());
    QJsonObject jsonObject = jsonDoc.object();
    QVariantMap jsonQVariantMap = jsonObject.toVariantMap();
    QVariantMap  jsonHeaderMap = jsonQVariantMap["headers"].toMap();
    mLastHttpTextResponse = jsonHeaderMap;

    // Extracting body
    QString value = jsonObject["body"].toString();
    QString SongName = value.remove(0,5);

    mUiHandler.SetSongNameValue( SongName );

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

}

void SignalHandler::OnPrevious()
{

}

void SignalHandler::OnSelect( int aIndex )
{
    mLastHttpTextResponse.clear();
    mLastHttpImageResponse.clear();

    if ( mList[aIndex].mPlayableMedia.size() == 0 )
    {

        ShowNoAudioStreamAvaialablePopup( true );
    }
    else
    {
        ShowNoAudioStreamAvaialablePopup( false );
    }

    if( aIndex > mList.size() - 1 )
    {
        qWarning() << "List Size = " << mList.size() << "is not in range with Index = " << aIndex;
        return;
    }

    //if( mList[aIndex].mBearerInfo.size() > 0 )
    {
        mHttpTextTimer->stop();
        mHttpImageTimer->stop();
        mPlayer->Stop();
        m_CurrentLyPlaying = mList[aIndex].mPlayableMedia;
        qDebug() << "$$ Selecting " + m_CurrentLyPlaying + "@ index " << aIndex;
        qDebug() << "$$ mList[aIndex].mBearerInfo.size() = " << mList[aIndex].mBearerInfo.size();
        mPlayer->playUrl(m_CurrentLyPlaying.toUtf8().constData());
        // This updates the UI with the findings
        UpdateUIFromList( aIndex );
        DownloadProgramInformation( mList[aIndex].mFqdn, mList[aIndex].mServiceIdentifier );
        BearerSplit data;

        for(int index = 0; index < mList[aIndex].mBearerInfo.size(); ++index)
        {
            if( mList[aIndex].mBearerInfo[index].mId.length() > 0 )
            {
                qDebug() << mList[aIndex].mBearerInfo[index].mId;
                StationInformation station;
                QString gcc;
                data.SplitBearerString(mList[aIndex].mBearerInfo[index].mId,station,gcc);

                ConstructTopic( station, gcc, "text", mTextTopic );
                ConstructTopic( station, gcc, "image", mImageTopic );

                mCurrentBearer = mList[aIndex].mBearerInfo[index].mId;


                mHttpTextTimer->start( sTimerValue );
                mHttpImageTimer->start( sTimerValue );
                break;
            }
        }
    }
}

void SignalHandler::OnServiceInformationDownloaded()
{
    mLastHttpTextResponse.clear();
    mLastHttpImageResponse.clear();
    mList.clear();
    mReader->ReadSiXmlData(ServiceInformationFileName,mList);
    qDebug() << "List Size" << mList.size();
    m_CurrentLyPlaying = "";
    int dataIndex = 0;

    mPlayer->Stop();
    for( int index = 0; index < mList.size(); ++index )
    {
        if( mList[index].mBearerInfo.size() > 0 )
        {
            for( int bearerIndex = 0; bearerIndex < mList[index].mBearerInfo.size(); ++bearerIndex )
            {
                if( mCurrentBearer == mList[index].mBearerInfo[bearerIndex].mId )
                {
                    qDebug() << "[HYB_RADIO] BearerUri = " << mCurrentBearer;
                    qDebug() << "[HYB_RADIO] Media " << mList[index].mPlayableMedia;
                    mPlayer->playUrl( mList[index].mPlayableMedia.toUtf8().constData() );
                    m_CurrentLyPlaying = mList[index].mPlayableMedia;
                    dataIndex = index;
                    UpdateUIFromList( dataIndex );
                    //DownloadProgramInformation( mList[index].mFqdn, mList[index].mServiceIdentifier );
                    break;
                }
            }
        }
    }

    if ( m_CurrentLyPlaying.size() == 0 )
    {

        ShowNoAudioStreamAvaialablePopup( true );
    }
    else
    {
        ShowNoAudioStreamAvaialablePopup( false );
    }


    mUiHandler.QmlMethodInvokeclearListElement();
    foreach( SiData val,mList )
    {
        mUiHandler.QmlMethodInvokeaddListElement( val );
    }

    mHttpTextTimer->start(sTimerValue);
    mHttpImageTimer->start(sTimerValue);
}

void SignalHandler::OnProgramInformationDownloaded()
{
    qDebug() << "OnProgramInformationDownloaded\n";
    // To Implement
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
            SIGNAL(sendSIAndEPGFileNames(QString,QString)),
            this,
            SLOT(OnFileNameAvailable(QString,QString))
            );

    // Connect Downloader
    QObject::connect
            (
            mServiceInformationDownloader,
            SIGNAL(sendDownloadComplete()),
            this,
            SLOT(OnServiceInformationDownloaded())
            );

    QObject::connect
            (
            mProgramInformationDownloader,
            SIGNAL(sendDownloadComplete()),
            this,
            SLOT(OnProgramInformationDownloaded())
            );

}

void SignalHandler::UpdateUIFromList( int aIndex )
{
    qDebug() << "Updating UI";
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


void SignalHandler::OnFileNameAvailable( QString si, QString xsi )
{
    qDebug() << "FileName Received";
    qDebug() << "SI FileName=" << si;
    qDebug() << "XSI FileName=" << xsi;

    mServiceInformationDownloader->DownloadFile( si );
}


void SignalHandler::OnSelectionChanged(QString value)
{
    qDebug() <<"OnSelectionChanged - "<<value;
    mCurrentSelection = value;
    if( Selecttion_UK_FM == value )
    {
        StationInformation data;
        QString fqdn;
        data.PopulateFmFields(9580,0xc479);
        ConstructFqdn(data,"ce1",fqdn);
        mDnsLookup->lookupCName(fqdn);

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
        ConstructTopic(data,"ce1","text",mTextTopic);
    }
}
