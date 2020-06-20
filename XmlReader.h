#ifndef XMLREADER_H
#define XMLREADER_H
#include "HybridRadioCommonTypes.hpp"
#include <QRegExp>
#include <QStringList>
#include <QDebug>

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
    void PrintSiData( const SiDataList& aSiList );

private:
    void TimeDateParser
        (
        const QString& aQString,
        QString& aTime,
        QString& aDate
        );

};

#endif // XMLREADER_H
