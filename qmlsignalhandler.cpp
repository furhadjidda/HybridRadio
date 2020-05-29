#include "qmlsignalhandler.hpp"
#include "downloadmanager.h"
#include <QDebug>
#include <stdlib.h>
#include <QProcess>
#include "lookuphelper.hpp"

static const QString Selecttion_UK_FM("UK");
static const QString Selecttion_UK_DAB("UK-DAB");
static const QString Selecttion_DE_FM("DE");

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
    //DataCollector *collector
    )
    : mUIObject( object )
    , mReader( reader )
    , mDnsLookup( dnsLookup )
{
    // Creating a Timer
    mTimer = new QTimer(this);
    QObject::connect
            (
            mTimer,
            SIGNAL(timeout()),
            this,
            SLOT(OnTimeout())
            );

    // Player which plays te URL.
    mPlayer = new Player();
    //mProcess = new QProcess();
    mServiceInformationDownloader = new DownloadManager( ServiceInformationFileName );
    mProgramInformationDownloader = new DownloadManager(ProgramInformationFileName);
    // Connect Play Button
    QObject::connect
            (
            mPlayer,
            SIGNAL(signalMediaStatusChanged(QMediaPlayer::State)),
            this,
            SLOT(mediaStatusChanged(QMediaPlayer::State))
            );

    QObject *playButton_Object = object->findChild<QObject*>("Play");
    QObject::connect
            (
            playButton_Object,
            SIGNAL(sendPlay()),
            this,
            SLOT(OnPlay())
            );

    QObject *stopButton_Object = object->findChild<QObject*>("Stop");
    QObject::connect
            (
            stopButton_Object,
            SIGNAL(sendStop()),
            this,
            SLOT(OnStop())
            );

    QObject *nextButton_Object = object->findChild<QObject*>("Next");
    QObject::connect
            (
            nextButton_Object,
            SIGNAL(sendNext()),
            this,
            SLOT(OnNext())
            );

    QObject *previousButton_Object = object->findChild<QObject*>("Previous");
    QObject::connect
            (
            previousButton_Object,
            SIGNAL(sendPrevious()),
            this,
            SLOT(OnPrevious())
            );

    QObject *onSelect_Object = object->findChild<QObject*>("ServiceListTableObject");
    QObject::connect
            (
            onSelect_Object,
            SIGNAL(selectIndex(int)),
            this,
            SLOT(OnSelect(int))
            );

    // Connect Selection Combo Box
    QObject *comboBox_Object = object->findChild<QObject*>("selection");
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

    QObject::connect
            (
            mServiceInformationDownloader,
            SIGNAL(sendDownloadComplete()),
            this,
            SLOT(OnFileDownloaded())
            );

    QObject::connect
            (
            mProgramInformationDownloader,
            SIGNAL(sendDownloadComplete()),
            this,
            SLOT(OnFileDownloaded())
            );
}

void SignalHandler::mediaStatusChanged(QMediaPlayer::State val)
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

    QObject *mediaStatus = mUIObject->findChild<QObject*>("MediaStatus");
    if( mediaStatus )
    {
        mediaStatus->setProperty
                (
                "text",
                status
                );

        QQmlProperty
                (
                mediaStatus,
                "text"
                )
                .write
                (
                status
                );
    }
}

QString SignalHandler::FormPIString(QString fqdn, QString serviceIdentifier)
{
    //http://epg.musicradio.com/radiodns/spi/3.1/id/www.helpmechill.com/chill/20170202_PI.xml
    QString date = QDate::currentDate().toString("yyyyMMdd");
    QString urlFormation = "http://" +
            mDnsLookup->GetServiceName() +
            "/radiodns/spi/3.1/id/" +
            fqdn + "/" + serviceIdentifier + "/" + date +"_PI.xml";
    qDebug() << "url Formation for PI xml = " << urlFormation;
    mProgramInformationDownloader->DownloadFile(urlFormation,false);
    return urlFormation;
}

void SignalHandler::OnTimeout()
{
    QString text = mTextTopic;
    QString image = mImageTopic;

    // Construct a url
    QString urlFormation = "http://" +
            mDnsLookup->GetHttpTargetName() +
            ":" +
            QString::number(mDnsLookup->GetHttpPortNumber())
            + "/radiodns/vis/vis.json?topic=/topic/"
            + text;
    mDnsLookup->GetHttpTargetName();
    QUrl url(urlFormation);
    qDebug() << urlFormation;
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &SignalHandler::httpFinished);

    // Construct a url
    QString urlFormation2 = "http://" +
            mDnsLookup->GetHttpTargetName() +
            ":" +
            QString::number(mDnsLookup->GetHttpPortNumber())
            + "/radiodns/vis/vis.json?topic=/topic/"
            + image;
    mDnsLookup->GetHttpTargetName();
    QUrl url2(urlFormation2);
    qDebug() << urlFormation2;
    imageReply = qnam.get(QNetworkRequest(url2));
    connect(imageReply, &QNetworkReply::finished, this, &SignalHandler::httpImageFinished);
}

void SignalHandler::httpImageFinished()
{
    qDebug() << "http finished";
    QString json = QString(imageReply->readAll().data());
    qDebug() <<"HTTP RESPONSE : " <<json;
    QJsonDocument d = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject bodyVal = d.object();
    QString value = bodyVal["body"].toString();
    QString artLink = value.remove(0,5);
    qDebug() << "ART : "<< artLink;
    if( 0 != artLink.size() )
    {
        QObject *artWork = mUIObject->findChild<QObject*>("artWork");
        if( artWork )
        {
            artWork->setProperty
                    (
                    "source",
                    artLink
                    );

            QQmlProperty
                    (
                    artWork,
                    "source"
                    )
                    .write
                    (
                    artLink
                    );
        }
    }
}


void SignalHandler::httpFinished()
{
    qDebug() << "http finished";
    QString json(reply->readAll().data());
    qDebug() <<"HTTP RESPONSE : " <<json;
    QJsonDocument d = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject bodyVal = d.object();
    QString value = bodyVal["body"].toString();
    QString SongName = value.remove(0,5);
    qDebug() << "SONG : "<< SongName;
    QObject *songName = mUIObject->findChild<QObject*>("SongObject");
    if( songName )
    {
        songName->setProperty
                (
                "text",
                QVariant(SongName)
                );

        QQmlProperty
                (
                songName,
                "text"
                )
                .write
                (
                SongName
                );
    }
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

    if ( mList[aIndex].mPlayableMedia.size() == 0 )
    {

        ShowNoAudioStreamAvaialablePopup( true );
    }
    else
    {
        ShowNoAudioStreamAvaialablePopup( false );
    }

    //if( mList[aIndex].mBearerInfo.size() > 0 )
    {
        mTimer->stop();
        mPlayer->Stop();
        m_CurrentLyPlaying = mList[aIndex].mPlayableMedia;
        qDebug() << "$$ Selecting " + m_CurrentLyPlaying + "@ index " << aIndex;
        qDebug() << "$$ mList[aIndex].mBearerInfo.size() = " << mList[aIndex].mBearerInfo.size();
        mPlayer->playUrl(m_CurrentLyPlaying.toUtf8().constData());
        // This updates the UI with the findings
        UpdateUIFromList( aIndex );

        BearerSplit data;

        for(int index = 0; index < mList[aIndex].mBearerInfo.size(); ++index)
        {
            if( mList[aIndex].mBearerInfo[index].mId.length() > 0 )
            {
                qDebug() << mList[aIndex].mBearerInfo[index].mId;
                StationInformation station;
                QString gcc;
                data.SplitBearerString(mList[aIndex].mBearerInfo[index].mId,station,gcc);

                QString textQuery;
                ConstructTopic( station, gcc, "text", textQuery );

                QString imageQuery;
                ConstructTopic( station, gcc, "image", imageQuery );

                mCurrentBearer = mList[aIndex].mBearerInfo[index].mId;
                mTextTopic = textQuery;
                mImageTopic = imageQuery;

                mTimer->start(sTimerValue);
                break;
            }
        }
    }
}

void SignalHandler::OnFileDownloaded()
{
    mList.clear();
    mReader->ReadSiXmlData(ServiceInformationFileName,mList);
    qDebug() << "List Size" << mList.size();
    QVariant retValue=0;
    m_CurrentLyPlaying = "";
    int dataIndex = 0;
    bool isIndexValid = false;
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
                    isIndexValid = true;
                    dataIndex = index;
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


    bool succeeded = false;
    QObject *RectBoxObj = mUIObject->findChild<QObject*>("RectBox");

    succeeded = QMetaObject::invokeMethod(
        RectBoxObj, "clearListElement",
                Q_RETURN_ARG(QVariant, retValue));

    foreach( SiData val,mList )
    {
        //if ( val.mBearerInfo.size() > 0 )
        {
            succeeded = QMetaObject::invokeMethod(
                RectBoxObj, "addListElement",
                        Q_RETURN_ARG(QVariant, retValue),
                        Q_ARG( QVariant, val.mServiceName ),
                        Q_ARG( QVariant, val.mGenre ),
                        Q_ARG( QVariant, val.mArtwork ));

            if(!succeeded)
            {
                qDebug() << "Invokation Failed";
            }
        }
    }

    if( isIndexValid )
    {
        UpdateUIFromList( dataIndex );
    }

    mTimer->start(sTimerValue);
}

void SignalHandler::ShowNoAudioStreamAvaialablePopup( bool val )
{
    QVariant retValue=0;
    QObject *RectBoxObj= mUIObject->findChild<QObject*>("RectBox");
    if ( val )
    {

        bool succeeded = QMetaObject::invokeMethod(
            RectBoxObj, "displayPopUp",
                    Q_RETURN_ARG(QVariant, retValue));

        if(!succeeded)
        {
            qDebug() << "Invokation Failed";
        }
    }
    else
    {
        bool succeeded = QMetaObject::invokeMethod(
            RectBoxObj, "hidePopUp",
                    Q_RETURN_ARG(QVariant, retValue));

        if(!succeeded)
        {
            qDebug() << "Invokation Failed";
        }
    }
}

void SignalHandler::UpdateUIFromList( int aIndex )
{
    qDebug() << "Updating UI";
    QObject *artWork = mUIObject->findChild<QObject*>("artWork");
    QObject *StationName = mUIObject->findChild<QObject*>("StationNameObj");
    QObject *description = mUIObject->findChild<QObject*>("DescriptionObject");
    QObject *bitRate = mUIObject->findChild<QObject*>("BitRateObject");
    QObject *moreInfo = mUIObject->findChild<QObject*>("additionalInfo");

    if( artWork )
    {
        artWork->setProperty
                (
                "source",
                mList[aIndex].mArtwork
                );

        QQmlProperty
                (
                artWork,
                "source"
                )
                .write
                (
                mList[aIndex].mArtwork
                );
    }

    if( StationName )
    {
        StationName->setProperty
                (
                "text",
                QVariant(mList[aIndex].mServiceName)
                );

        QQmlProperty
                (
                StationName,
                "text"
                )
                .write
                (
                mList[aIndex].mServiceName
                );
    }

    if( description )
    {
        description->setProperty
                (
                "text",
                QVariant(mList[aIndex].mDescription)
                );

        QQmlProperty
                (
                description,
                "text"
                )
                .write
                (
                mList[aIndex].mDescription
                );
    }


    if( bitRate )
    {
        QString bitRateValue("BitRate:");
        bitRateValue.append(mList[aIndex].mBitRate);
        bitRate->setProperty
                (
                "text",
                QVariant(bitRateValue)
                );

        QQmlProperty
                (
                bitRate,
                "text"
                )
                .write
                (
                bitRateValue
                );
    }

    if( moreInfo )
    {
        QString data("Bearer Info: ");
        for (auto val : mList[aIndex].mBearerInfo) {
            data.append(val.mId);
            data.append(" ; ");
        }

        moreInfo->setProperty
                (
                "text",
                QVariant(data)
                );

        QQmlProperty
                (
                moreInfo,
                "text"
                )
                .write
                (
                data
                );
    }
}


void SignalHandler::OnFileNameAvailable( QString si, QString xsi )
{
    qDebug() << "Received";
    qDebug() << "FileName=" << si;
    qDebug() << "FileName=" << xsi;

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
