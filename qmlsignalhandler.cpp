#include "qmlsignalhandler.hpp"
#include "downloadmanager.h"
#include <QDebug>
#include <stdlib.h>
#include <QProcess>
static const QString FMDemo1("FM Demo - 1");
static const QString FMDemo2("FM Demo - 2 ");
static const QString DABDemo1("DAB Demo - 1");
static const QString DABDemo2("DAB Demo - 2");
static QString FMDemo1_LookUp("09580.c479.ce1.fm.radiodns.org");
static QString FMDemo1_TEXT("fm/ce1/c479/09580/text");
static QString FMDemo1_IMAGE("fm/ce1/c479/09580/image");
static QString FMDemo2_Lookup("10570.c372.ce1.fm.radiodns.org");
static QString DABDemo1_LookUp("0.c221.ce15.ce1.dab.radiodns.org");
static QString DABDemo2_LookUp("0.c2A1.c181.ce1.dab.radiodns.org");
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
    DNSLookup *dnsLookup,
    DataCollector *collector
    )
    : mObject( object )
    , mReader( reader )
    , mDnsLookup( dnsLookup )
    , mCollector( collector )
    , m_IsDownloadedPI( false )
{
    // Creating a Timer
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(OnTimeout()));

    // Player which plays te URL.
    //player = new Player();
    mProcess = new QProcess();
    mDownloader = new MyNetworkAccessManager("");
    mPIDownloader = new MyNetworkAccessManager("DownloadPI.xml");
    // Connect Play Button
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

QString SignalHandler::FormPIString(QString fqdn, QString serviceIdentifier)
{
    //http://epg.musicradio.com/radiodns/spi/3.1/id/www.helpmechill.com/chill/20170202_PI.xml
    QString urlFormation = "http://" +
            mDnsLookup->GetServiceName() +
            "/radiodns/spi/3.1/id/" +
            fqdn + "/" + serviceIdentifier + "/20170202_PI.xml";
    qDebug() << "url Formation for PI xml = " << urlFormation;
    mPIDownloader->DownloadFile(urlFormation,false);
    return urlFormation;
}

void SignalHandler::OnTimeout()
{
    // Construct a url
    QString urlFormation = "http://" +
            mDnsLookup->GetHttpTargetName() +
            ":" +
            QString::number(mDnsLookup->GetHttpPortNumber())
            + "/radiodns/vis/vis.json?topic=/topic/"
            + FMDemo1_TEXT;
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
            + FMDemo1_IMAGE;
    mDnsLookup->GetHttpTargetName();
    QUrl url2(urlFormation2);
    qDebug() << urlFormation2;
    imageReply = qnam.get(QNetworkRequest(url2));
    connect(imageReply, &QNetworkReply::finished, this, &SignalHandler::httpImageFinished);
}

void SignalHandler::httpImageFinished()
{
    qDebug() << "http finished";
   // QString json =(reply->readAll());// QString::fromStdString(imageReply->readAll().toStdString());
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
    //player->playUrl(m_CurrentLyPlaying);
    //mProcess->kill();
    //qDebug() << "mList[index].playableMedia " << mList[index].playableMedia;
    QString gstreamerCommand = "gst-launch-0.10 -v souphttpsrc location="
            + m_CurrentLyPlaying
            + " iradio-mode=true ! decodebin name=demux ! audioresample ! audioconvert ! autoaudiosink";
    mProcess->start(gstreamerCommand);
}

void SignalHandler::OnStop()
{
    //player->Stop();
     mProcess->kill();
}

void SignalHandler::OnFileDownloaded()
{
    mReader->ReadSiXmlData("Downloaded_SI.xml");
    mList.clear();

    mCollector->GetCompleteList( mList );

    qDebug() << "List Size" << mList.size();

    QObject *artWork = mObject->findChild<QObject*>("artWork");
    QObject *StationName = mObject->findChild<QObject*>("StationNameObj");
    QObject *description = mObject->findChild<QObject*>("DescriptionObject");
    QObject *bitRate = mObject->findChild<QObject*>("BitRateObject");
    QVariant retValue=0;

    int index = 0;
    index = rand()% mList.size() + 1;
    bool isIndexValid = false;
    while( index < mList.size() )
    {
        if( mList[index].playableMedia.size() > 0 )
        {
            //player->playUrl(mList[index].playableMedia);
            mProcess->kill();
            qDebug() << "mList[index].playableMedia " << mList[index].playableMedia;
            QString gstreamerCommand = "gst-launch-0.10 -v souphttpsrc location="
                    + mList[index].playableMedia
                    + " iradio-mode=true ! decodebin name=demux ! audioresample ! audioconvert ! autoaudiosink";
            mProcess->start(gstreamerCommand);
            m_CurrentLyPlaying = mList[index].playableMedia;
            qDebug() << mList[index].playableMedia;
            qDebug() << mList[index].artwork;
            qDebug() << mList[index].serviceName;
            if(!m_IsDownloadedPI)
            {
                m_IsDownloadedPI = true;
                FormPIString(mList[index].fqdn,mList[index].serviceIdentifier);
            }
            else
            {
                mEpgList.clear();
                bool succeeded = QMetaObject::invokeMethod(
                    mChildObject, "clearListElement",
                            Q_RETURN_ARG(QVariant, retValue));

                mReader->ReadPiXmlData("DownloadPI.xml");
                mCollector->GetEPGCompleteList( mEpgList );

                uint count = 0;
                foreach( EpgData epgData,mEpgList )
                {
                    succeeded = QMetaObject::invokeMethod(
                        mChildObject, "addListElement",
                                Q_RETURN_ARG(QVariant, retValue),
                                Q_ARG( QVariant,epgData.longName ),
                                Q_ARG( QVariant, epgData.description ),
                                Q_ARG( QVariant, epgData.timeAndDate ),
                                Q_ARG( QVariant, epgData.duration ));
                    if(!succeeded)
                    {
                        qDebug() << "Invokation Failed";
                    }
                    else
                    {
                        qDebug() << "count" << ++count;
                    }
                }


            }
            isIndexValid = true;
            break;
        }
        index++;
    }

    if( isIndexValid )
    {
        artWork->setProperty
                (
                "source",
                mList[index].artwork
                );

        QQmlProperty
                (
                artWork,
                "source"
                )
                .write
                (
                mList[index].artwork
                );

        if(StationName)
        {
            StationName->setProperty
                    (
                    "text",
                    QVariant(mList[index].serviceName)
                    );

            QQmlProperty
                    (
                    StationName,
                    "text"
                    )
                    .write
                    (
                    mList[index].serviceName
                    );
        }

        if( description )
        {
            description->setProperty
                    (
                    "text",
                    QVariant(mList[index].description)
                    );

            QQmlProperty
                    (
                    description,
                    "text"
                    )
                    .write
                    (
                    mList[index].description
                    );
        }


        if( bitRate )
        {
            QString bitRateValue("BitRate:");
            bitRateValue.append(mList[index].bitRate);
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
    mTimer->start(2000);
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
    if( FMDemo1 == value )
    {
        mDnsLookup->lookupCName(FMDemo1_LookUp);
    }
    else if( FMDemo2 == value )
    {
        mDnsLookup->lookupCName(FMDemo2_Lookup);
    }
    else if( DABDemo1 == value )
    {
        mDnsLookup->lookupCName(DABDemo1_LookUp);
    }
    else if( DABDemo2 == value )
    {
        mDnsLookup->lookupCName(DABDemo2_LookUp);
    }
}
