#include "qmlsignalhandler.hpp"
#include "downloadmanager.h"
#include <QDebug>
#include <stdlib.h>
#include <QProcess>
static const QString FMDemo1("UK");
static const QString FMDemo2("DE");

static QString FMDemo1_LookUp("09580.c479.ce1.fm.radiodns.org");
static QString FMDemo1_TEXT("fm/ce1/c479/09580/text");
static QString FMDemo1_IMAGE("fm/ce1/c479/09580/image");
static QString FMDemo1_bearer("fm:ce1.c479.09580");

static QString FMDemo2_Lookup("10420.d389.de0.fm.radiodns.org");
static QString FMDemo2_TEXT("fm/de0/d389/10420/text");
static QString FMDemo2_IMAGE("fm/de0/d389/10420/text");
static QString FMDemo2_bearer("fm:de0.d389.10420");

const int sTimerValue = 5000;


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
    : mObject( object )
    , mReader( reader )
    , mDnsLookup( dnsLookup )
    //, mCollector( collector )
    , m_IsDownloadedPI( false )
{
    // Creating a Timer
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(OnTimeout()));

    // Player which plays te URL.
    player = new Player();
    mProcess = new QProcess();
    mDownloader = new MyNetworkAccessManager("");
    mPIDownloader = new MyNetworkAccessManager("DownloadPI.xml");
    // Connect Play Button
    QObject::connect
            (
            player,
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
            mDownloader,
            SIGNAL(sendDownloadComplete()),
            this,
            SLOT(OnFileDownloaded())
            );

    QObject::connect
            (
            mPIDownloader,
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

    QObject *mediaStatus = mObject->findChild<QObject*>("MediaStatus");
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
    mPIDownloader->DownloadFile(urlFormation,false);
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
    QObject *artWork = mObject->findChild<QObject*>("artWork");
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
    QObject *songName = mObject->findChild<QObject*>("SongObject");
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
    player->playUrl(m_CurrentLyPlaying);
    mProcess->kill();
    mProcess->waitForFinished();
    //qDebug() << "mList[index].playableMedia " << mList[index].playableMedia;
    QString gstreamerCommand = "gst-launch-0.10 -v souphttpsrc location="
            + m_CurrentLyPlaying
            + " iradio-mode=true ! decodebin name=demux ! audioresample ! audioconvert ! autoaudiosink";
    //mProcess->start(gstreamerCommand);
}

void SignalHandler::OnStop()
{
     player->Stop();
     mProcess->kill();
     mProcess->waitForFinished();
}


void SignalHandler::OnNext()
{

}

void SignalHandler::OnPrevious()
{

}

void SignalHandler::OnSelect( int aIndex )
{
    if( mList[aIndex].mBearerInfo.size() > 0 )
    {
        player->Stop();
        m_CurrentLyPlaying = mList[aIndex].mPlayableMedia;
        qDebug() << "$$ Selecting " + m_CurrentLyPlaying + "@ index " + aIndex;
        player->playUrl(m_CurrentLyPlaying.toUtf8().constData());
        BearerSplit data;
        for(int index =0; index < mList[aIndex].mBearerInfo.size(); ++index)
        {
            if( mList[aIndex].mBearerInfo[index].mId.length() > 0 )
            {
                qDebug() << mList[aIndex].mBearerInfo[index].mId;
                data.SplitBearerString(mList[aIndex].mBearerInfo[index].mId);
                QString textQuery( data.mBand +
                                   '/' +
                                   data.mGcc +
                                   '/' +
                                   data.mPi +
                                   '/' +
                                   data.mFrequency +
                                   "/text");
                QString imageQuery( data.mBand +
                                   '/' +
                                   data.mGcc +
                                   '/' +
                                   data.mPi +
                                   '/' +
                                   data.mFrequency +
                                   "/image");
                mCurrentBearer = mList[aIndex].mBearerInfo[index].mId;
                mTextTopic = textQuery;
                mImageTopic = imageQuery;

                QObject *artWork = mObject->findChild<QObject*>("artWork");
                QObject *StationName = mObject->findChild<QObject*>("StationNameObj");
                QObject *description = mObject->findChild<QObject*>("DescriptionObject");
                QObject *bitRate = mObject->findChild<QObject*>("BitRateObject");
                QVariant retValue=0;
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

                if(StationName)
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
                break;
            }
        }
    }
}

void SignalHandler::OnFileDownloaded()
{
    mList.clear();
    mReader->ReadSiXmlData("Downloaded_SI.xml",mList);
    qDebug() << "List Size" << mList.size();

    QObject *artWork = mObject->findChild<QObject*>("artWork");
    QObject *StationName = mObject->findChild<QObject*>("StationNameObj");
    QObject *description = mObject->findChild<QObject*>("DescriptionObject");
    QObject *bitRate = mObject->findChild<QObject*>("BitRateObject");
    QVariant retValue=0;

    int dataIndex = 0;
    bool isIndexValid = false;
    player->Stop();
    for( int index = 0; index < mList.size(); ++index )
    {
        if( mList[index].mBearerInfo.size() > 0 )
        {
            for( int bearerIndex = 0; bearerIndex < mList[index].mBearerInfo.size(); ++bearerIndex )
            {
                if( mCurrentBearer == mList[index].mBearerInfo[bearerIndex].mId )
                {
                    qDebug() << "[HYB_RADIO] BearerUri = " << FMDemo1_bearer;
                    qDebug() << "[HYB_RADIO] Media " << mList[index].mPlayableMedia;
                    player->playUrl( mList[index].mPlayableMedia.toUtf8().constData() );
                    m_CurrentLyPlaying = mList[index].mPlayableMedia;
                    isIndexValid = true;
                    dataIndex = index;
                    break;
                }
            }
        }
    }

    bool succeeded = false;
    QObject *RectBoxObj= mObject->findChild<QObject*>("RectBox");

    succeeded = QMetaObject::invokeMethod(
        RectBoxObj, "clearListElement",
                Q_RETURN_ARG(QVariant, retValue));

    foreach( SiData val,mList )
    {
        succeeded = QMetaObject::invokeMethod(
            RectBoxObj, "addListElement",
                    Q_RETURN_ARG(QVariant, retValue),
                    Q_ARG( QVariant, val.mServiceName ),
                    Q_ARG( QVariant, val.mGenre ));

        if(!succeeded)
        {
            qDebug() << "Invokation Failed";
        }
    }

    if( isIndexValid )
    {
        artWork->setProperty
                (
                "source",
                mList[dataIndex].mArtwork
                );

        QQmlProperty
                (
                artWork,
                "source"
                )
                .write
                (
                mList[dataIndex].mArtwork
                );

        if(StationName)
        {
            StationName->setProperty
                    (
                    "text",
                    QVariant(mList[dataIndex].mServiceName)
                    );

            QQmlProperty
                    (
                    StationName,
                    "text"
                    )
                    .write
                    (
                    mList[dataIndex].mServiceName
                    );
        }

        if( description )
        {
            description->setProperty
                    (
                    "text",
                    QVariant(mList[dataIndex].mDescription)
                    );

            QQmlProperty
                    (
                    description,
                    "text"
                    )
                    .write
                    (
                    mList[dataIndex].mDescription
                    );
        }


        if( bitRate )
        {
            QString bitRateValue("BitRate:");
            bitRateValue.append(mList[dataIndex].mBitRate);
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
    }
    mTimer->start(sTimerValue);
}


void SignalHandler::OnFileNameAvailable( QString si, QString xsi )
{
    qDebug() << "Received";
    qDebug() << "FileName=" << si;
    qDebug() << "FileName=" << xsi;

    mDownloader->DownloadFile( si );
}


void SignalHandler::OnSelectionChanged(QString value)
{
    qDebug() <<"OnSelectionChanged - "<<value;
    m_IsDownloadedPI = false;
    mCurrentSelection = value;
    if( FMDemo1 == value )
    {
        mDnsLookup->lookupCName(FMDemo1_LookUp);
        mCurrentBearer = FMDemo1_bearer;
        mImageTopic = FMDemo1_IMAGE;
        mTextTopic = FMDemo1_TEXT;
    }
    else if( FMDemo2 == value )
    {
        mCurrentBearer = FMDemo2_bearer;
        mDnsLookup->lookupCName(FMDemo2_Lookup);
        mImageTopic = FMDemo2_IMAGE;
        mTextTopic = FMDemo2_TEXT;
    }
}
