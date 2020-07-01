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
static const QString Selection_DE_DAB("DAB(DEUTSCHLAND)");
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
    QObject *object // Object which can be used to set QML properties.
    )
    : mUIObject( object )
    , mHybridRadioCore( new HybridRadioCore() )
{    
    mHybridRadioCore->InitializeCore();
    ConnectSignals();
    OnSelectionChanged( SelectCoxMedia1 );
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
    mCurrentPlayingIndex = mHybridRadioCore->PlayNextServiceIndex();
}

void SignalHandler::OnPrevious()
{
    mCurrentPlayingIndex = mHybridRadioCore->PlayPreviousServiceIndex();
}

void SignalHandler::OnSelect( int aIndex )
{
    mHybridRadioCore->PlayServiceAtIndex( aIndex );
}

void SignalHandler::OnServiceInformationDownloaded()
{

    qDebug() << endl << endl << "OnServiceInformationDownloaded" << endl;
    mHybridRadioCore->GetServiceInformationList( mList );
    mUiHandler.QmlMethodInvokeclearListElement();
    foreach( SiData val, mList )
    {
        mUiHandler.QmlMethodInvokeaddListElement( val );
    }
}

void SignalHandler::OnProgramInformationDownloaded()
{
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
    qDebug() << temp.FormattedData();
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
    else
    {
        qWarning() << "InValid Selection";
        return;
    }

    mHybridRadioCore->LookForStation( data );
}
