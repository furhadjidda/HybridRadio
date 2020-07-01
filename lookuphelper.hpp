#ifndef HYBRIDRADIO_LOOKUPHELPER_HPP_
#define HYBRIDRADIO_LOOKUPHELPER_HPP_

#include <stdint.h>
#include <QString>

static const QString BandFm("fm");
static const QString BandDab("dab");
static const QString BandIboc("hd");

typedef struct{
    QString mBand{""};
    QString mFrequency{""};
    qint32 mPi{0};
    qint32 mSid{0};
    qint32 mScids{0};
    qint32 mEid{0};
    QString mIbocTxId{""};
    QString mIbocCountryCode{""};
    qint32 mIbocHdId{0};
    QString mGcc{""};

    void PopulateDabFields
        (
        const qint32 aSid,
        const qint32 aScids,
        const qint32 aEid
        )
    {
        mBand = "dab";
        mScids = aScids;
        mSid = aSid;
        mEid = aEid;
    }

    void PopulateFmFields
        (
        const qint32 aFrequency,
        const qint32 aPi
        )
    {
        mBand = "fm";
        mPi = aPi;
        if(aFrequency < 10000 )
        {
            mFrequency = "0" + QString::number(aFrequency,10);
        }
        else
        {
            mFrequency = QString::number(aFrequency);
        }
    }

    void PopulateIbocFields
        (
        const qint32 aIbocTxId,
        const qint32 aIbocHdId,
        const QString aIbocCountryCode
        )
    {
        mBand = "hd";
        if(aIbocTxId <= 0xFFFF )
        {
            mIbocTxId = "0" + QString::number(aIbocTxId,16);
        }
        else
        {
            mIbocTxId = QString::number(aIbocTxId,16);
        }
        mIbocHdId = aIbocHdId;
        mIbocCountryCode = aIbocCountryCode;
    }

}StationInformation;

void ConstructFqdn
    (
    const StationInformation& aStationInfo,
    QString& returnValue
    );

void ConstructServiceIdentifier
    (
    const StationInformation& aStationInfo,
    QString& returnValue
    );

void ConstructBearerUri
    (
    const StationInformation& aStationInfo,
    QString& returnValue
    );

void ConstructTopic
    (
    const StationInformation& aStationInfo,
    const QString& aTopic,
    QString& returnValue
    );

void DeconstructBearer
    (
    StationInformation& aStationInfo,
    const QString& bearer
    );

#endif
