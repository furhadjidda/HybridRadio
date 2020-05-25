#ifndef XMLREADER_H
#define XMLREADER_H
#include <QString>
#include <QVector>
#include <stdint.h>

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

//! Structure containing interactive link information.
typedef struct {
    QString tuneInLink;
    QString webpageLink;
}LinkInfo;

//! Structure to hold Service Information
typedef struct {
    QString mServiceName;
    QString mArtwork;
    QString mDescription;
    QString mGenre;
    QString mBitRate;
    QString mPlayableMedia;
    QString mMimeType;
    QString mFqdn;
    QString mServiceIdentifier;
    QVector<BearerInfo> mBearerInfo;
    LinkInfo mLinkeInfo;
}SiData;

//! List of each Service Information read from the xml file
typedef QVector<SiData> SiDataList;

/**
    @brief XmlReader

    This class Reads the xml file and reads and return the relevant data
    from the file depending on the type of API called.

    @ingroup HybridRadio
 */
class XmlReader
{
public:
    XmlReader();

    ~XmlReader();

    void ReadPiXmlData
        (
        const QString& fileName,
        EpgList& data
        );

    void ReadSiXmlData
        (
        const QString& fileName,
        SiDataList& epgList
        );
};

#endif // XMLREADER_H
