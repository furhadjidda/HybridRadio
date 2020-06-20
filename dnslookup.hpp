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
    void lookupVis();


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
    void sendSIAndEPGFileNames( QString, QString );
public slots:
    void onCNameResponse();
    void onServiceResponse();
    void onHttpVisResponse();
    void onVisResponse();


private:
    QDnsLookup* mCName;
    QDnsLookup* mService;
    QDnsLookup* mHttpVis;
    QDnsLookup* mStompVis;
    QString mCNAME;
    QString mServiceName;
    uint mServicePort;
    QString mHttpTargetName;
    uint mHttpServicePort;
    QString mStompTargetName;
    uint mStompServicePort;
};

#endif // DNSLOOKUP_HPP

