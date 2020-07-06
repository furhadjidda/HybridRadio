#include "dnslookup.hpp"
#include <QtNetwork>
#include <QUrl>
#include <QTimer>
#include <QTimerEvent>

void DNSLookup::lookupCName( QString& val )
{
    // Create a DNS lookup.
    mCName = new QDnsLookup
            (
            QDnsLookup::ANY,
            QString( val )
            );

    connect
        (
        mCName,
        SIGNAL(finished()),
        this,
        SLOT(onCNameResponse()));

    mCName->setName( val );
    mCName->lookup();
}

void DNSLookup::lookupService( QString& val )
{
    QString lookUp("_radioepg._tcp.");

    lookUp = lookUp + val;
    qDebug() << "[DNS Look-up] Looking Service " << lookUp;

    mService = new QDnsLookup
                        (
                        QDnsLookup::SRV,
                        lookUp
                        );

    connect
        (
        mService,
        SIGNAL(finished()),
        this,
        SLOT(onServiceResponse())
        );
    mService->lookup();
}

void DNSLookup::lookupHttpVis()
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

    lookUp = lookUp + mCNAME;
    qDebug() << "[DNS Look-up] Checking HTTP VIS support" << lookUp;

    mHttpVis = new QDnsLookup
                        (
                        QDnsLookup::ANY,
                        lookUp
                        );

    connect
        (
        mHttpVis,
        SIGNAL(finished()),
        this,
        SLOT(onHttpVisResponse())
        );
    mHttpVis->lookup();
}

 void DNSLookup::lookupVis()
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

     lookUp = lookUp + mCNAME;
     qDebug() << "[DNS Look-up] Checking Stomp support " << lookUp;

     mStompVis = new QDnsLookup
                         (
                         QDnsLookup::ANY,
                         lookUp
                         );

     connect
         (
         mStompVis,
         SIGNAL(finished()),
         this,
         SLOT(onVisResponse())
         );
     mStompVis->lookup();
 }

void DNSLookup::onCNameResponse()
{
    // Check the lookup succeeded.
    if ( mCName->error() != QDnsLookup::NoError )
    {
        qWarning() << "[DNS Look-up ERR] onCNameResponse::DNS lookup failed";
        qWarning() << "[DNS Look-up ERR] mCName->error()" << mCName->error();
        mCName->deleteLater();
        return;
    }

    // Handle the results.
    auto cName= mCName->canonicalNameRecords();
    for ( const QDnsDomainNameRecord &name : cName )
    {
        qDebug() << "[DNS Look-up] cName response Response " << name.value();
        mCNAME = name.value();
        lookupService( mCNAME );
        lookupHttpVis();
        lookupVis();
    }
    mCName->deleteLater();
}

void DNSLookup::onServiceResponse()
{
    // Check the lookup succeeded.
    if ( mService->error() != QDnsLookup::NoError )
    {
        qWarning() << "[DNS Look-up ERR] onServiceResponse::DNS lookup failed";
        qWarning() << "[DNS Look-up ERR] mService->error()" << mService->error();
        mService->deleteLater();
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
    mService->deleteLater();
}

void DNSLookup::onHttpVisResponse()
{
    // Check the lookup succeeded.
    if ( mHttpVis->error() != QDnsLookup::NoError )
    {
        qWarning() << "[DNS Look-up ERR] -> HttpVisResponse::DNS lookup failed"
                   << " , mHttpVis->error()" << mHttpVis->error();
        emit SignalHttpVisSupported( false );
        mHttpVis->deleteLater();
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
    mHttpVis->deleteLater();
}

void DNSLookup::onVisResponse()
{
    // Check the lookup succeeded.
    if ( mStompVis->error() != QDnsLookup::NoError )
    {
        qWarning() << "[DNS Look-up ERR] -> VisResponse::DNS lookup failed"
                   << " , mStompVis->error()" << mStompVis->error();
        emit SignalStompVisSupported( false );
        mStompVis->deleteLater();
        return;
    }
    // Handle the results.
    const auto serviceRecords = mStompVis->serviceRecords();
    for ( const QDnsServiceRecord &record : serviceRecords )
    {
        mStompTargetName = record.target();
        mStompServicePort = record.port();
        qDebug() << "[DNS Look-up] -> VisResponse Service" <<record.name()
                 << " Target" <<record.target()
                 << " Port" <<record.port();
    }
    emit SignalStompVisSupported( true );
    mStompVis->deleteLater();
}
