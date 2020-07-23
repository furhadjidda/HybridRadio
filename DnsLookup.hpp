#ifndef DNSLOOKUP_HPP
#define DNSLOOKUP_HPP

#include <QObject>
#include <QDnsLookup>
#include <QDebug>
#include <QHostAddress>
#include <QDnsDomainNameRecord>
#include <QDnsMailExchangeRecord>
#include <QDnsHostAddressRecord>
#include <QDnsTextRecord>
#include <QDate>
#include <memory>

class DnsLookup
        : public QObject
{
    Q_OBJECT
public:
    DnsLookup();

    ~DnsLookup();

    void lookupCName
        (
        const QString& val
        );

    void lookupService
        (
        const QString& val
        );

    void lookupHttpVis();

    void lookupVis();

    inline void ConstructServiceInfoFileName
        (
        QString& aRef
        )
    {
        aRef = GetServiceName();
        aRef.replace(".","_");
        aRef.append("_SI.xml");
    }

    inline void ConstructProgramInfoFileName
        (
        const QString& aFqdn,
        const QString& aServiceIdentifier,
        QString& aUrl,
        QString& aFileName
        )
    {
        QString date = QDate::currentDate().toString("yyyyMMdd");
        aUrl = "http://" +
                GetServiceName() +
                "/radiodns/spi/3.1/id/" +
                aFqdn + "/" + aServiceIdentifier + "/" + date +"_PI.xml";

        qDebug() << "[HYBRID_CORE]  Program Inforamtion URL = " << aUrl;

        aFileName = aServiceIdentifier + "_" + date + ".xml";
    }


    inline QString GetServiceName()
    {
        return mServiceName;
    }

    inline uint GetPortNumber()
    {
        return mServicePort;
    }

    inline QString GetHttpTargetName()
    {
        return mHttpTargetName;
    }

    inline uint GetHttpPortNumber()
    {
        return mHttpServicePort;
    }

    inline QString GetStompTargetName()
    {
        return mStompTargetName;
    }

    inline uint GetStompPortNumber()
    {
        return mStompServicePort;
    }

signals:
    void SignalServiceInformationAvailable( const QString& );
    void SignalHttpVisSupported( bool aVal );
    void SignalStompVisSupported( bool aVal );

public slots:
    void onCNameResponse();
    void onServiceResponse();
    void onHttpVisResponse();
    void onVisResponse();


private:
    QDnsLookup* mCName{nullptr};
    QDnsLookup* mService{nullptr};
    QDnsLookup* mHttpVis{nullptr};
    QDnsLookup* mStompVis{nullptr};
    QString mCanonicalName;
    QString mServiceName;
    uint mServicePort;
    // For Http
    QString mHttpTargetName;
    uint mHttpServicePort;
    // For Stomp
    QString mStompTargetName;
    uint mStompServicePort;
};

#endif // DNSLOOKUP_HPP

