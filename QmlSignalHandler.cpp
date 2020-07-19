#include "QmlSignalHandler.hpp"
#include "DownloadManager.hpp"
#include <QDebug>
#include <stdlib.h>
#include <QProcess>
#include "LookupHelper.hpp"
#include <QtAlgorithms>
#include <QCollator>
#include <QThread>


static const QString Selecttion_UK_FM("FM(UK)");
static const QString Selecttion_UK_DAB("DAB(UK)");
static const QString Selecttion_DE_FM("FM(DEUTSCHLAND)");
static const QString Selection_DE_DAB("DAB(DEUTSCHLAND)");
static const QString SelectRadio1("FM(RADIO1 UK)");
static const QString SelectBBC2("FM(BBC-2 UK)");
static const QString SelectBBC4("FM(BBC-4 UK)");
static const QString SelectCoxMedia1("FM(USA)-73978");
static const QString SelectAustralia ("DAB(AUS)");
static const QString SelectNorway ("DAB(NORWAY)");
static const QString SelectNorway2 ("DAB(NORWAY)2");
static const QString SelectSpain ("FM(SPAIN)");
static const QString SelectNetherLands ("FM(NETHERLANDS)");
static const QString SelectIreland ("FM(IRELAND)");
static const QString SelectAustria ("FM(AUSTRIA)");


SignalHandler::SignalHandler
    (
    QObject *object // Object which can be used to set QML properties.
    )
    : mUIObject( object )
    , mHybridRadioCore( new HybridRadioCore() )
{    
    mHybridRadioCore->InitializeCore();
    ConnectSignals();
    OnSelectionChanged( SelectCoxMedia1 );    
    QThread::sleep( 5 ) ;
    PopulatePresetFields();
}

SignalHandler::~SignalHandler()
{
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
    mHybridRadioCore->StartPlayingCurrentAudio();
}

void SignalHandler::OnStop()
{
    mHybridRadioCore->StopPlayingCurrentAudio();
}


void SignalHandler::OnNext()
{
    mUiHandler.QmlMethodInvokeMethodHideEpgPresentImage();
    mCurrentPlayingIndex = mHybridRadioCore->PlayNextServiceIndex();
}

void SignalHandler::OnPrevious()
{
    mUiHandler.QmlMethodInvokeMethodHideEpgPresentImage();
    mCurrentPlayingIndex = mHybridRadioCore->PlayPreviousServiceIndex();
}

void SignalHandler::OnSelect( int aIndex )
{
    mUiHandler.QmlMethodInvokeMethodHideEpgPresentImage();
    mHybridRadioCore->PlayServiceAtIndex( aIndex );
    PopulateAdditionalInfo( mList[aIndex] );
    mCurrentPlayingIndex = aIndex;
}

void SignalHandler::OnServiceInformationDownloaded()
{
    mHybridRadioCore->GetServiceInformationList( mList );
    mUiHandler.QmlMethodInvokeclearListElement();
    foreach( SiData val, mList )
    {
        mUiHandler.QmlMethodInvokeaddListElement( val );
    }
}

void SignalHandler::OnProgramInformationDownloaded()
{
    mProgramList.clear();
    mHybridRadioCore->GetProgramInformationData( mProgramList );
    mUiHandler.QmlMethodInvokeclearProgramElement();
    for( EpgStruct val : mProgramList )
    {
        mUiHandler.QmlMethodInvokeaddProgramElement( val );
    }

    if( mProgramList.size() > 0 )
    {
       mUiHandler.QmlMethodInvokeMethodDisplayEpgPresentImage();
    }
}

void SignalHandler::OnAudioStreamAvailability( bool val )
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

    QObject *presetId = mUIObject->findChild<QObject*>("presetid1_MouseArea");
    QObject::connect
            (
            presetId,
            SIGNAL(sendPresetSave(QString)),
            this,
            SLOT(OnPresetSave(QString))
            );
    QObject::connect
            (
            presetId,
            SIGNAL(sendPresetRecall(QString)),
            this,
            SLOT(OnPresetRecall(QString))
            );

    QObject *presetId2 = mUIObject->findChild<QObject*>("presetid2_MouseArea");
    QObject::connect
            (
            presetId2,
            SIGNAL(sendPresetSave(QString)),
            this,
            SLOT(OnPresetSave(QString))
            );
    QObject::connect
            (
            presetId2,
            SIGNAL(sendPresetRecall(QString)),
            this,
            SLOT(OnPresetRecall(QString))
            );

    QObject *presetId3 = mUIObject->findChild<QObject*>("presetid3_MouseArea");
    QObject::connect
            (
            presetId3,
            SIGNAL(sendPresetSave(QString)),
            this,
            SLOT(OnPresetSave(QString))
            );
    QObject::connect
            (
            presetId3,
            SIGNAL(sendPresetRecall(QString)),
            this,
            SLOT(OnPresetRecall(QString))
            );

    QObject *presetId4 = mUIObject->findChild<QObject*>("presetid4_MouseArea");
    QObject::connect
            (
            presetId4,
            SIGNAL(sendPresetSave(QString)),
            this,
            SLOT(OnPresetSave(QString))
            );
    QObject::connect
            (
            presetId4,
            SIGNAL(sendPresetRecall(QString)),
            this,
            SLOT(OnPresetRecall(QString))
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalTextChanged,
            this,
            &SignalHandler::OnTextChanged
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalImageChanged,
            this,
            &SignalHandler::OnImageChanged
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalServiceInformationDownloaded,
            this,
            &SignalHandler::OnServiceInformationDownloaded
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalProgramInformationDownloaded,
            this,
            &SignalHandler::OnProgramInformationDownloaded
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalAudioStreamAvailability,
            this,
            &SignalHandler::OnAudioStreamAvailability
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalAudioStreamAvailability,
            this,
            &SignalHandler::OnAudioStreamAvailability
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalHttpTransportSupport,
            this,
            &SignalHandler::OnHttpVisSupported
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalStompTransportSupport,
            this,
            &SignalHandler::OnStompVisSupported
            );
    // Data

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalStationNameChanged,
            this,
            &SignalHandler::OnStationNameChanged
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalDescriptionChanged,
            this,
            &SignalHandler::OnDescriptionChanged
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalBitRateChanged,
            this,
            &SignalHandler::OnBitRateChanged
            );

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalMediaStateChanged,
            this,
            &SignalHandler::OnMediaStateChanged
            );    

    QObject::connect
            (
            mHybridRadioCore.get(),
            &HybridRadioCore::SignalStationFound,
            this,
            &SignalHandler::OnStationFound
            );
}

void SignalHandler::OnStationFound( const SiData& aData )
{
    SiData temp( aData );
    qDebug() << "[HANDLER] OnStationFound "<< temp.FormattedData();
    qDebug() << "[HANDLER] OnStationFound " << aData.mPlayableMedia;
    mHybridRadioCore->PlayMedia( aData.mPlayableMedia );
    PopulateAdditionalInfo( aData );
    mCurrentPlayingIndex = mHybridRadioCore->GetCurrentServiceIndex();
}

void SignalHandler::OnStationNameChanged( const QString& aData )
{
    mUiHandler.SetStationNameValue( aData );
}

void SignalHandler::OnDescriptionChanged( const QString& aData )
{
    mUiHandler.SetStationDescriptionValue( aData );
}

void SignalHandler::OnBitRateChanged( const QString& aData )
{
    mUiHandler.SetBitrateValue( aData );
}

void SignalHandler::OnMediaStateChanged( const QString& aData )
{
    mUiHandler.SetMediaStatusValue( aData );
}

void SignalHandler::OnHttpVisSupported( bool aVal )
{
    mUiHandler.QmlMethodInvokeMethodDisplayHttpProtocolSupport( aVal );
}

void SignalHandler::OnStompVisSupported( bool aVal )
{
    mUiHandler.QmlMethodInvokeMethodDisplayStompProtocolSupport( aVal );
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


void SignalHandler::OnPresetSave(const QString& aData)
{
    qDebug() << "PresetSaved" << aData;
    if ( mPresets.SavePreset( aData.toInt(), mList[mCurrentPlayingIndex] ) )
    {
        mUiHandler.UpdatePresetBox( aData, mList[mCurrentPlayingIndex] );
    }
}

void SignalHandler::OnPresetRecall(const QString& aData)
{
    qDebug() << "PresetRecalled" << aData;
    mUiHandler.QmlMethodInvokeMethodHideEpgPresentImage();
    SiData data;
    mPresets.RecallPreset( aData.toInt(), data );
    if( 0 == data.mBearerInfo.size() )
    {
        qWarning() << "[HANDLER] No Bearer info preset";
        return;
    }
    StationInformation stationInfo;
    DeconstructBearer( stationInfo, data.mBearerInfo[0].mId );
    mHybridRadioCore->LookForStation( stationInfo );

}

void SignalHandler::OnSelectionChanged(QString value)
{
    StationInformation data;
    QString fqdn;

    qDebug() <<"[HANDLER] OnSelectionChanged :"<<value << "\n\n";
    ClearMetaData();
    if( Selecttion_UK_FM == value )
    {
        data.PopulateFmFields( 9580, 0xc479 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xc479 ),
                        "GB"
                        );
    }
    else if( Selecttion_DE_FM == value )
    {
        data.PopulateFmFields( 10420, 0xd389 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xd318 ),
                        "DE"
                        );
    }
    else if( Selection_DE_DAB == value )
    {
        data.PopulateDabFields( 0xd318, 0, 0x10a5) ;
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xd318 ),
                        "DE"
                        );
    }
    else if( Selecttion_UK_DAB == value )
    {
        // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
        // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
        //static QString DABDemo2_Lookup("0.c7d8.c1ce.ce1.dab.radiodns.org");
        data.PopulateDabFields( 0xc7d8, 0, 0xc1ce) ;
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xc7d8 ),
                        "GB"
                        );
    }
    else if( SelectRadio1 == value )
    {
        data.PopulateFmFields( 9770, 0xc201 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xc201 ),
                        "GB"
                        );
    }
    else if( SelectBBC2 == value )
    {
        data.PopulateFmFields( 8810, 0xc202 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xc202 ),
                        "GB"
                        );
    }
    else if( SelectBBC4 == value )
    {
        data.PopulateFmFields( 9320, 0xc204 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xc204 ),
                        "GB"
                        );
    }
    else if( SelectCoxMedia1 == value )
    {
        // For more information on how we got 292 and 7426 , refer to section 4.1
        // 4.1 Station ID Number (MSG ID= 0000) of the document below
        // https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-5-d/reference-docs/1020s.pdf
        // 7426 is the fcc facility id
        // 292 is the country code
        data.PopulateIbocFields( 0x7426, 0, "292" );
    }
    else if( SelectAustralia == value )
    {
        // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
        // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
        //static QString DABDemo2_Lookup("0.c7d8.c1ce.ce1.dab.radiodns.org");
        data.PopulateDabFields(0x111a,0,0xf003);
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0x111a ),
                        "AU"
                        );
    }
    else if( SelectNorway == value )
    {
        // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
        // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
        //static QString DABDemo2_Lookup("0.c7d8.c1ce.ce1.dab.radiodns.org");
        data.PopulateDabFields( 0xf204, 0, 0xf801 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xf204 ),
                        "NO"
                        );
    }
    else if( SelectSpain == value )
    {
        // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
        // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
        //static QString DABDemo2_Lookup("0.c7d8.c1ce.ce1.dab.radiodns.org");
        data.PopulateFmFields( 9320, 0xe213 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xe213 ),
                        "ES"
                        );
    }
    else if( SelectNetherLands == value )
    {
        data.PopulateFmFields( 8860, 0x8203 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0x8203 ),
                        "NL"
                        );
    }
    else if( SelectNorway2 == value )
    {
        data.PopulateDabFields( 0xf214, 0, 0xf201 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xf214 ),
                        "NO"
                        );
    }
    else if( SelectIreland == value )
    {
        data.PopulateFmFields( 10060, 0x2205 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0x2205 ),
                        "IE"
                        );
    }
    else if( SelectAustria == value )
    {
        data.PopulateFmFields( 9200, 0xA943 );
        data.mGcc = mGccHelper.FormGcc
                        (
                        mGccHelper.CountryIdentifier( 0xA943 ),
                        "AT"
                        );
    }
    else
    {
        qWarning() << "InValid Selection";
        return;
    }
    mUiHandler.QmlMethodInvokeMethodHideEpgPresentImage();
    mHybridRadioCore->LookForStation( data );
}


void SignalHandler::PopulateAdditionalInfo( const SiData& aData )
{
    QString data("Bearer Info: ");
    for( auto bearer : aData.mBearerInfo )
    {
        data.append( bearer.mId );
        data.append( " ; ");
    }

    data.append("Additional Streams: ");

    for( auto media : aData.mPlaylableMediaList )
    {
        data.append( media );
        data.append( " ; ");
    }

    mUiHandler.QmlMethodInvokeAddMoreInfo( data );
}



void SignalHandler::PopulatePresetFields()
{
    SiDataList presets;
    mPresets.GetAllPresets( presets );

    // preset id starts with 1
    for( qint16 index =0; index < presets.size(); ++index )
    {
        mUiHandler.UpdatePresetBox( QString::number( index+1, 10 ), presets[index] );
    }
}
