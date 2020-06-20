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

    // Find the XMPP servers for gmail.com
    mCName->setName( val );
    mCName->lookup();
}

void DNSLookup::lookupService( QString& val )
{
    QString lookUp("_radioepg._tcp.");

    lookUp = lookUp + val;
    qDebug() << "[DNS Look-up] Looking Service with Name " << lookUp;

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
    qDebug() << "[DNS Look-up] Looking HTTP VIS with Name " << lookUp;

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
     qDebug() << "[DNS Look-up] Looking VIS with Name " << lookUp;

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
        qDebug() << "[DNS Look-up] Service" <<record.name();
        qDebug() << "[DNS Look-up] Target" <<record.target();
        qDebug() << "[DNS Look-up] Port" <<record.port();

        QString HTTP("http://");
        QString SIFilePostFix("/radiodns/spi/3.1/SI.xml");
        QString XSIFilePostFix("/radiodns/epg/XSI.xml");
        QString SIFileName("");
        QString XSIFileName("");
        SIFileName = HTTP + GetServiceName() + SIFilePostFix;
        XSIFileName = HTTP + GetServiceName()+ XSIFilePostFix;
        emit sendSIAndEPGFileNames(SIFileName,XSIFileName);
    }
    mService->deleteLater();
}

void DNSLookup::onHttpVisResponse()
{
    // Check the lookup succeeded.
    if ( mHttpVis->error() != QDnsLookup::NoError )
    {
        qWarning() << "[DNS Look-up ERR] onHttpVisResponse::DNS lookup failed";
        qWarning() << "[DNS Look-up ERR] mHttpVis->error()" << mHttpVis->error();
        mHttpVis->deleteLater();
        return;
    }
    // Handle the results.
    const auto serviceRecords = mHttpVis->serviceRecords();
    for ( const QDnsServiceRecord &record : serviceRecords )
    {        
        mHttpTargetName = record.target();
        mHttpServicePort = record.port();
        qDebug() << "[DNS Look-up] HTTP Service" <<record.name();
        qDebug() << "[DNS Look-up] HTTP target" <<record.target();
        qDebug() << "[DNS Look-up] HTTP port" <<record.port();
    }
    mHttpVis->deleteLater();
}

void DNSLookup::onVisResponse()
{
    // Check the lookup succeeded.
    if ( mStompVis->error() != QDnsLookup::NoError )
    {
        qWarning() << "[DNS Look-up ERR] onVisResponse::DNS lookup failed";
        qWarning() << "[DNS Look-up ERR] mStompVis->error()" << mStompVis->error();
        mStompVis->deleteLater();
        return;
    }
    // Handle the results.
    const auto serviceRecords = mStompVis->serviceRecords();
    for ( const QDnsServiceRecord &record : serviceRecords )
    {
        mStompTargetName = record.target();
        mStompServicePort = record.port();
        qDebug() << "[DNS Look-up] STOMP Service" <<record.name();
        qDebug() << "[DNS Look-up] STOMP target" <<record.target();
        qDebug() << "[DNS Look-up] STOMP port" <<record.port();
    }
    mStompVis->deleteLater();
}
