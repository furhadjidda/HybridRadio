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
    DNSLookup()
    {
    }
    ~DNSLookup()
    {
    }
    void lookupCName( QString& val );
    void lookupService( QString& val );
    void lookupHttpVis();

    QString GetServiceName()
    {
        return mServiceName;
    }

    uint GetPortNumber()
    {
        return mServicePort;
    }

    QString GetHttpTargetName()
    {
        return mHttpTargetName;
    }

    uint GetHttpPortNumber()
    {
        return mHttpServicePort;
    }
signals:
    void sendSIAndEPGFileNames(QString,QString);
public slots:
    void onCNameResponse();
    void onServiceResponse();
    void onHttpVisResponse();


private:
    QDnsLookup *mCName;
    QDnsLookup *mService;
    QDnsLookup *mHttpVis;
    QString mCNAME;
    QString mServiceName;
    uint mServicePort;
    QString mHttpTargetName;
    uint mHttpServicePort;
};

#endif // DNSLOOKUP_HPP

