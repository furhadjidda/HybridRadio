#include "GccHelper.hpp"

GccHelper::GccHelper()
{
    ReadLookUpTable();
}

QString GccHelper::FormGcc
    (
    const QString& aCountryCode,
    const  QString& aISOCountryCode
    )
{
    QMap<QString, MainTableLookupData>::const_iterator mapIter = mPrimaryTableLookup.find( aISOCountryCode );
    if( mapIter == mPrimaryTableLookup.end() )
    {
        qWarning() << "Nothing Found !! " << endl;
        return InvalidGcc;
    }

    QStringList countryCodeList = std::get<0>( mapIter.value() );
    QString ecc = std::get<1>( mapIter.value() );
    if( countryCodeList.contains( aCountryCode ) )
    {
        return ( aCountryCode + ecc ).toLower();
    }
    else
    {
        BoundaryCountriesLookupMap boundaries = std::get<2>( mapIter.value() );
        QMap<QString, QString>::const_iterator boundriesIter = boundaries.find( aCountryCode );
        if( boundriesIter == boundaries.end() )
        {
            qWarning() << "No Boundaries Found !! " << endl;
            return InvalidGcc;
        }
        return ( aCountryCode + ecc ).toLower();
    }

    return InvalidGcc;
}

void GccHelper::ReadLookUpTable()
{
    QFile file( "countries.csv" );

    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qDebug() << "[GCC-HELPER] Error" << file.errorString();
        return;
    }

    while (!file.atEnd())
    {
        //const qint8 CountryNameColumn = 0;
        const qint8 IsoCountryAplhaCodeColumn = 1;
        const qint8 EccColumn = 2;
        const qint8 CountryCodeColumn = 3;
        const qint8 BorderingCountriesColumn = 4;

        QStringList lineSplitDataList;
        CountyCodeList countryCodeList;
        BoundaryCountriesLookupMap boundaryCountries;
        // Reading the line
        QByteArray line = file.readLine();

        // Regular expression to split the lines into the data in columns
        QRegExp rx("(\\,)");

        // Data split into the data in columns
        lineSplitDataList = QString( line ).split( rx );

        // For boundaries countryCode List
        {
            QRegExp boundaryContriesRegEx("(\\;)");
            QStringList boundaryCountriesSplitData = lineSplitDataList[BorderingCountriesColumn].split( boundaryContriesRegEx );

            // Forming the QMap for Boundary countries
            for( auto boundaryContriesData : boundaryCountriesSplitData )
            {
                 QRegExp rx("(\\:)");
                 QStringList countryCode = boundaryContriesData.split(rx);
                 // If the data is read correctly there can only be one key value for example B:BW
                 // B -> Key , BW -> Value , which when stored in the string list the count is 2
                 if( 2 == countryCode.size() )
                 {
                     boundaryCountries.insert( countryCode.at(0), countryCode.at(1) );
                 }
            }
        }

        // For Country Code List
        {
            QRegExp ContryCodeRegEx("(\\;)");
            countryCodeList = lineSplitDataList[CountryCodeColumn].split( ContryCodeRegEx );
        }
        MainTableLookupData tuple = std::make_tuple
                                        (
                                        countryCodeList,
                                        lineSplitDataList[EccColumn],
                                        boundaryCountries
                                        );

        mPrimaryTableLookup.insert( lineSplitDataList[IsoCountryAplhaCodeColumn], tuple );
    }
}
