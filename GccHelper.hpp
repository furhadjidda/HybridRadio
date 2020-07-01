#ifndef GCCHELPER_HPP
#define GCCHELPER_HPP

#include <QFile>
#include <QDebug>
#include <QMap>
#include <QMultiMap>
#include <QStringList>
#include <tuple>

typedef QMap<QString, QString>  BoundaryCountriesLookupMap;
typedef QStringList CountyCodeList;
typedef std::tuple<CountyCodeList, QString, BoundaryCountriesLookupMap> MainTableLookupData;

const QString InvalidGcc("InvalidGcc");
const QString InvalidPi("InvalidPi");

class GccHelper
{
public:
    GccHelper();

    QString FormGcc
        (
        const QString& aCountryCode,
        const  QString& aISOCountryCode
        );

    QString CountryIdentifier( const qint32 aUniqueCodeHex )
    {
        // For FM RDS/RBDS or DAB/DAB+ audio services, the Country Code of the
        // service is given by the first nibble of the RDS/RBDS PI code or DAB
        // SId respectively.
        if( 0 == aUniqueCodeHex )
        {
           return InvalidPi;
        }

        return QString(  QString::number( aUniqueCodeHex, 16 ).toUtf8().at( 0 ) ).toUpper();
    }

private:
    void ReadLookUpTable();
    QMap<QString, MainTableLookupData> mPrimaryTableLookup;
};

#endif // GCCHELPER_HPP
