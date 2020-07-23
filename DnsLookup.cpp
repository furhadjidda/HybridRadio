#include "DnsLookup.hpp"
#include <QtNetwork>
#include <QUrl>
#include <QTimer>
#include <QTimerEvent>


DnsLookup::DnsLookup()
{
    mCName = new QDnsLookup( this );
    connect
        (
        mCName,
        &QDnsLookup::finished,
        this,
        &DnsLookup::onCNameResponse
        );

    mService = new QDnsLookup( this );
    connect
        (
        mService,
        &QDnsLookup::finished,
        this,
        &DnsLookup::onServiceResponse
        );

    mHttpVis = new QDnsLookup( this );
    connect
        (
        mHttpVis,
        &QDnsLookup::finished,
        this,
        &DnsLookup::onHttpVisResponse
        );

    mStompVis = new QDnsLookup( this );
    connect
        (
        mStompVis,
        &QDnsLookup::finished,
        this,
        &DnsLookup::onVisResponse
        );

}

DnsLookup::~DnsLookup()
{
    mCName->deleteLater();
    mService->deleteLater();
    mHttpVis->deleteLater();
    mStompVis->deleteLater();
}

void DnsLookup::lookupCName
    (
    const QString& val
    )
{
    // Create a DNS lookup.
    mCName->setType( QDnsLookup::CNAME );
    mCName->setName( val );
    mCName->lookup();
}

void DnsLookup::lookupService
    (
    const QString& val
    )
{
    QString lookUp("_radioepg._tcp.");
    lookUp = lookUp + val;
    qDebug() << "[DNS Look-up] Looking Service " << lookUp;

    mService->setName( lookUp );
    mService->setType( QDnsLookup::SRV );
    mService->lookup();
}

void DnsLookup::lookupHttpVis()
{
    // Formation of this is mentioned in ETSI TS 101 499 V3.1.1  Page 17
    /*
             Transport Used    			SRV Record Service Name    		Protocol
                Stomp                         radiovis                      tcp
                HTTP                        radiovis-http               	tcp

        For example, for the Authoritative FQDN rdns.musicradio.com,
        a DNS SRV record query is made to:
        _radiovis._tcp.rdns.musicradio.com Using the nslookup tool,
        this would yield the following
        DNS SRV record: service = 0 100 61613 vis.musicradio.com.

        This indicates that the SlideShow application over IP can be accessed using the Stomp transport
        on the host vis.musicradio.com, port 61613.
     */

    QString lookUp("_radiovis-http._tcp.");
    lookUp = lookUp + mCanonicalName;
    qDebug() << "[DNS Look-up] Checking HTTP VIS support" << lookUp;

    mHttpVis->setName( lookUp );
    mHttpVis->setType( QDnsLookup::ANY );
    mHttpVis->lookup();
}

 void DnsLookup::lookupVis()
 {
     // Formation of this is mentioned in ETSI TS 101 499 V3.1.1  Page 17
     /*
              Transport Used    			SRV Record Service Name    		Protocol
                 Stomp                         radiovis                      tcp
                 HTTP                        radiovis-http               	tcp

         For example, for the Authoritative FQDN rdns.musicradio.com,
         a DNS SRV record query is made to:
         _radiovis._tcp.rdns.musicradio.com Using the nslookup tool,
         this would yield the following
         DNS SRV record: service = 0 100 61613 vis.musicradio.com.

         This indicates that the SlideShow application over IP can be accessed using the Stomp transport
         on the host vis.musicradio.com, port 61613.
      */

     QString lookUp("_radiovis._tcp.");

     lookUp = lookUp + mCanonicalName;
     qDebug() << "[DNS Look-up] Checking Stomp support " << lookUp;

     mStompVis->setName( lookUp );
     mStompVis->setType( QDnsLookup::ANY );
     mStompVis->lookup();
 }

void DnsLookup::onCNameResponse()
{
    // Check the lookup succeeded.
    if( nullptr == mCName )
    {
        qWarning() << endl << "[DNS Look-up ERR] nullptr == mCName";
        return;
    }

    if ( mCName->error() != QDnsLookup::NoError )
    {
        qWarning() << "[DNS Look-up ERR] onCNameResponse::DNS lookup failed";
        qWarning() << "[DNS Look-up ERR] mCName->error()" << mCName->error();
        return;
    }

    // Handle the results.
    auto cName= mCName->canonicalNameRecords();
    for ( const QDnsDomainNameRecord &name : cName )
    {
        qDebug() << "[DNS Look-up] cName response Response " << name.value();
        mCanonicalName = name.value();
        lookupService( mCanonicalName );
        lookupHttpVis();
        lookupVis();
    }
}

void DnsLookup::onServiceResponse()
{
    // Check the lookup succeeded.
    if( nullptr == mService )
    {
        qWarning() << endl << "[DNS Look-up ERR] nullptr == mService";
        return;
    }
    if ( mService->error() != QDnsLookup::NoError )
    {
        qWarning() << "[DNS Look-up ERR] onServiceResponse::DNS lookup failed";
        qWarning() << "[DNS Look-up ERR] mService->error()" << mService->error();
        return;
    }
    // Handle the results.
    const auto serviceRecords = mService->serviceRecords();
    for ( const QDnsServiceRecord &record : serviceRecords )
    {
        mServiceName = record.target();
        mServicePort = record.port();
        qDebug() << "[DNS Look-up]->ServiceNameResponse Service" <<record.name()
                 << "Target" <<record.target()
                 << "Port" <<record.port();

        // See Section 10.2.2.2    Services not supporting client identification , of document
        // ETSI TS 102 818 V3.2.1 (2019-06) for document discovery of Service information.
        QString HTTP("http://");
        QString SIFilePostFix("/radiodns/spi/3.1/SI.xml");
        QString ServiceInformationLink = HTTP + GetServiceName() + SIFilePostFix;
        emit SignalServiceInformationAvailable( ServiceInformationLink );
    }
}

void DnsLookup::onHttpVisResponse()
{
    // Check the lookup succeeded.
    if( nullptr == mHttpVis )
    {
        qWarning() << endl << "[DNS Look-up ERR] nullptr == mHttpVis";
        return;
    }
    if ( mHttpVis->error() != QDnsLookup::NoError )
    {
        qWarning() << "[DNS Look-up ERR] -> HttpVisResponse::DNS lookup failed"
                   << " , mHttpVis->error()" << mHttpVis->error();
        emit SignalHttpVisSupported( false );
        return;
    }
    // Handle the results.
    const auto serviceRecords = mHttpVis->serviceRecords();
    for ( const QDnsServiceRecord &record : serviceRecords )
    {
        mHttpTargetName = record.target();
        mHttpServicePort = record.port();
        qDebug() << "[DNS Look-up] -> HttpVisResponse Service" <<record.name()
                 << " Target" <<record.target()
                 << " Port" <<record.port();
    }
    emit SignalHttpVisSupported( true );
}

void DnsLookup::onVisResponse()
{
    if( nullptr == mStompVis )
    {
        qWarning() << endl << "nullptr == mStompVis";
        return;
    }
    // Check the lookup succeeded.
    if ( mStompVis->error() != QDnsLookup::NoError )
    {
        qWarning() << "[DNS Look-up ERR] -> VisResponse::DNS lookup failed"
                   << " , mStompVis->error()" << mStompVis->error();
        emit SignalStompVisSupported( false );
        return;
    }
    // Handle the results.
    const auto serviceRecords = mStompVis->serviceRecords();
    for ( const QDnsServiceRecord &record : serviceRecords )
    {
        mStompTargetName = record.target();
        mStompServicePort = record.port();
        qDebug() << "[DNS Look-up] -> VisResponse Service"
                 << " Target" << mStompTargetName
                 << " Port" << mStompServicePort;
    }
    emit SignalStompVisSupported( true );
}
