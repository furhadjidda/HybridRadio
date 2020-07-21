#ifndef STATIONLOCATERHELPER_HPP
#define STATIONLOCATERHELPER_HPP

#include <QFile>
#include <QDebug>
#include <QMap>
#include <QMultiMap>
#include <QStringList>
#include <QVector>

typedef struct {
    QString mCallSign;
    QString mActivePiCode;
    QString mFrequency;
    QString mFacilityId;
    QString mCityOfLicense;
    QString mState;
    QString mLocation;

    QString FormattedData()
    {
        return( "[Station Location Helper]CallSign = " + mCallSign + " mActivePiCode= " + mActivePiCode
                + " Location = " + mState + " mFacilityId=" + mFacilityId );
    }

} UsaStationData;

class StationLocaterHelper
{
public:
    StationLocaterHelper();
    QString LookUpState
        (
        const QString& aFrequency,
        const QString& aPiCode,
        const QString& aFacilityId
        );

private:
    void ReadLookUpTable();
    QVector<UsaStationData> mStationLookupTable;

};

#endif // STATIONLOCATERHELPER_HPP
