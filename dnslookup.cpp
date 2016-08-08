#include "dnslookup.hpp"

void DNSLookup::lookupCName( QString& val )
{
    // Create a DNS lookup.
    mCName = new QDnsLookup
            (
            QDnsLookup::ANY,
            QString(val),
            QHostAddress(val)
            );

    connect
        (
        mCName,
        SIGNAL(finished()),
        this,
        SLOT(onCNameResponse()));

    // Find the XMPP servers for gmail.com
    mCName->setName(val);
    mCName->lookup();
}

void DNSLookup::lookupService( QString& val )
{
    QString lookUp("_radioepg._tcp.");

    lookUp = lookUp + val;
    qDebug() << "Looking Service with Name " << lookUp;

    mService = new QDnsLookup(QDnsLookup::SRV,
                         lookUp,
                         QHostAddress(lookUp));

    connect
        (
        mService,
        SIGNAL(finished()),
        this,
        SLOT(onServiceResponse())
        );
    mService->lookup();
}

void DNSLookup::onCNameResponse()
{
    // Check the lookup succeeded.
    if (mCName->error() != QDnsLookup::NoError) {
        qWarning("DNS lookup failed");
        mCName->deleteLater();
        return;
    }
    qDebug() << "Handling DNS Lookup";
    // Handle the results.
    auto cName= mCName->canonicalNameRecords();
    for (const QDnsDomainNameRecord &Name : cName)
    {
        qDebug() << Name.value();
        mCNAME = Name.value();
        lookupService( mCNAME );
    }
    mCName->deleteLater();
}

void DNSLookup::onServiceResponse()
{
    // Check the lookup succeeded.
    if (mService->error() != QDnsLookup::NoError) {
        qWarning("DNS lookup failed");
        mService->deleteLater();
        qDebug() << mService->error();
        return;
    }
    // Handle the results.
    const auto records = mService->serviceRecords();
    for (const QDnsServiceRecord &record : records)
    {
        qDebug() << "Service" <<record.name();
        qDebug() << "target" <<record.target();
        mServiceName = record.target();
        qDebug() << "port" <<record.port();
        mServicePort = record.port();

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
