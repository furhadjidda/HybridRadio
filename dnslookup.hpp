#ifndef DNSLOOKUP_HPP
#define DNSLOOKUP_HPP

#include<QObject>
#include<QDnsLookup>
#include<QDebug>
#include <QHostAddress>
#include <QDnsDomainNameRecord>
#include <QDnsMailExchangeRecord>
#include <QDnsHostAddressRecord>
#include <QDnsTextRecord>

class DNSLookup : public QObject
{
    Q_OBJECT
public:
    DNSLookup(){}
    ~DNSLookup()
    {
    }
    void lookupCName( QString& val );
    void lookupService( QString& val );

    QString GetServiceName()
    {
        return mServiceName;
    }

    uint GetPortNumber()
    {
        return mServicePort;
    }
signals:
    void sendSIAndEPGFileNames(QString,QString);
public slots:
    void onCNameResponse();
    void onServiceResponse();


private:
    QDnsLookup *mCName;
    QDnsLookup *mService;
    QString mCNAME;
    QString mServiceName;
    uint mServicePort;
};

#endif // DNSLOOKUP_HPP

