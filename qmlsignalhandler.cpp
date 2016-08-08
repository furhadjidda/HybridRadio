#include "qmlsignalhandler.hpp"
#include "downloadmanager.h"
#include <QDebug>
#include <windows.h> // for Sleep
#include <stdlib.h>

static const QString FMDemo1("FM Demo - 1");
static const QString FMDemo2("FM Demo - 2 ");
static const QString DABDemo1("DAB Demo - 1");
static const QString DABDemo2("DAB Demo - 2");
static QString FMDemo1_LookUp("09580.c479.ce1.fm.radiodns.org");
static QString FMDemo2_Lookup("10570.c372.ce1.fm.radiodns.org");
static QString DABDemo1_LookUp("0.c221.ce15.ce1.dab.radiodns.org");
//0.d220.100c.de0.dab.radiodns.org
//29A9 103.3   10330.29a9.2a0.fm.radiodns.org

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
{
    // Player which plays te URL.
    player = new Player();

    mDownloader = new MyNetworkAccessManager();
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
}

void SignalHandler::OnPlay()
{
    player->playUrl(m_CurrentLyPlaying);
}

void SignalHandler::OnStop()
{
    player->Stop();
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


    uint index = 0;
    index = rand()% mList.size() + 1;
    bool isIndexValid = false;
    while( index < mList.size() )
    {
        if( mList[index].playableMedia.size() > 0 )
        {
            player->playUrl(mList[index].playableMedia);
            m_CurrentLyPlaying = mList[index].playableMedia;
            qDebug() << mList[index].playableMedia;
            qDebug() << mList[index].artwork;
            qDebug() << mList[index].serviceName;
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

    }
}
