#include "StationLocaterHelper.hpp"
#include <algorithm>

StationLocaterHelper::StationLocaterHelper()
{
    ReadLookUpTable();
}

QString StationLocaterHelper::LookUpState
    (
    const QString& aFrequency,
    const QString& aPiCode,
    const QString& aFacilityId
    )
{
    qDebug() << "[StationLocaterHelper] Freq=" << aFrequency << " PiCode=" << aPiCode << " FacilityId=" << aFacilityId;
    for( auto value : mStationLookupTable )
    {
        if( !aFacilityId.isEmpty() && ( value.mFacilityId == aFacilityId ))
        {
            return ( value.mCityOfLicense + ", " + value.mState);
        }
        else if( ( value.mFrequency == aFrequency ) && ( value.mActivePiCode == aPiCode ) )
        {
            return ( value.mCityOfLicense + ", " + value.mState);
        }
        else
        {
            // Nothing found
        }
    }

    return "";
}


void StationLocaterHelper::ReadLookUpTable()
{
    const qint8 CallSign = 0;
    const qint8 ActivePiCode = 1;
    const qint8 Frequency = 2;
    const qint8 FacilityId = 3;
    const qint8 CityOfLicense = 4;
    const qint8 State = 5;
    const qint8 Location = 6;

    QStringList lineSplitDataList;

    QFile file( "/home/pi/qt-apps/HybridRadio/bin/pi-code-location-data-usa.csv" );

    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qDebug() << "[StationLocaterHelper] Error" << file.errorString();
        return;
    }


    while (!file.atEnd())
    {
        // Reading the line
        QByteArray line = file.readLine();

        // Regular expression to split the lines into the data in columns
        QRegExp rx("(\\,)");

        // Data split into the data in columns
        lineSplitDataList = QString( line ).split( rx );

        UsaStationData stationData;

        stationData.mCallSign = lineSplitDataList[CallSign];
        stationData.mActivePiCode = lineSplitDataList[ActivePiCode];
        stationData.mFrequency = lineSplitDataList[Frequency];
        stationData.mFacilityId = lineSplitDataList[FacilityId];
        stationData.mCityOfLicense = lineSplitDataList[CityOfLicense];
        stationData.mState = lineSplitDataList[State];
        stationData.mLocation = lineSplitDataList[Location];

        //qDebug() << stationData.FormattedData();

        mStationLookupTable.push_back( stationData );
    }
}
