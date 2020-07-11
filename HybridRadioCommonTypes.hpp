#ifndef HYBRIDRADIOCOMMONTYPES_HPP
#define HYBRIDRADIOCOMMONTYPES_HPP
#include <QString>
#include <QVector>
#include <QMap>
#include <stdint.h>
#include "LookupHelper.hpp"

//! Structure to hold Information about EPG
typedef struct {
    QString mDescription;
    QString mLongName;
    QString mTimeAndDate;
    QString mDuration;
    QString mArt;
}EpgStruct;

//! List that hold EPG data read from the xml file
typedef QVector<EpgStruct> EpgList;

//! Structure having bearer information
typedef struct {
    QString mId{""};
    QString mCost{""};
    QString mMimeValue{""};
    std::uint32_t mBitRate{0};
    std::uint32_t mOffset{0};
}BearerInfo;

typedef  QVector<BearerInfo> BearerList;

//! Structure containing interactive link information.
typedef struct {
    QString tuneInLink;
    QString webpageLink;
}LinkInfo;

//! Structure to hold Service Information
struct SiData{
    QString mServiceName;
    QString mArtwork;
    QMap<QString,QString> mArtworkAvailable;
    QString mDescription;
    QString mGenre;
    QString mBitRate;
    QString mPlayableMedia;
    QStringList mPlaylableMediaList;
    QString mMimeType;
    QString mFqdn;
    QString mServiceIdentifier;
    QVector<BearerInfo> mBearerInfo;
    LinkInfo mLinkeInfo;

    static bool SortingComparison( const SiData& aArg1, const SiData& aArg2 )
    {
        return QString::compare(aArg2.mServiceName, aArg1.mServiceName);
    }

    const QString FormattedData()
    {
        QString formattedData;

        formattedData = "Service Name = " + mServiceName + "\n" +
                        " PlayableMedia = " + mPlayableMedia + "\n" +
                        " FQDN = " + mFqdn + "\n" +
                        " ServiceIdentifier = " + mServiceIdentifier + "\n";

        for( auto bearer : mBearerInfo )
        {
            formattedData.append( ("   Bearer: " + bearer.mId + "\n") ) ;
        }

        return formattedData;
    }
};

//! List of each Service Information read from the xml file
typedef QVector<SiData> SiDataList;



typedef struct{
    QString mBand;
    QString mFrequency;
    QString mPi;
    QString mSid;
    QString mScids;
    QString mGcc;
    QString mEid;

    void SplitBearerString
        (
        const QString& data,
        StationInformation& aStationInfo
        )
    {
        DeconstructBearer(aStationInfo, data);
        mBand = aStationInfo.mBand;
        mFrequency = aStationInfo.mFrequency;
        mPi = QString::number( aStationInfo.mPi, 16);
        mSid = QString::number( aStationInfo.mSid, 16);
        mScids = QString::number( aStationInfo.mScids, 16);
        mGcc = aStationInfo.mGcc;
        mEid = QString::number( aStationInfo.mEid, 16);
    }

}BearerSplit;

#endif // HYBRIDRADIOCOMMONTYPES_HPP
