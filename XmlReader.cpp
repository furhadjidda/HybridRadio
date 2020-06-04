

#include "XmlReader.h"
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QFile>
#include <QDebug>
#include <QtAlgorithms>

/**
  @brief Constructor
*/
XmlReader::XmlReader()
{
}

/**
  @brief Destructor
*/
XmlReader::~XmlReader()
{
}

/**
  @brief ReadPiXmlData

  This method reads the PI xml file and reads and return the relevant data
  in EpgList.
*/
void XmlReader::ReadPiXmlData
        (
        const QString& fileName, //!<[in] File Name to read
        EpgList& epgList //!<[out] epg list
        )
{
    QFile   piXmlFile( fileName );
    QString piXmlString("");
    int    index = -1;

    if ( piXmlFile.open( QIODevice::ReadOnly ) )
    {
        QTextStream in( &piXmlFile );
        piXmlString = in.readAll();

        qDebug() << "[HYB_RADIO] Reading PI XML file, Size = " << piXmlFile.size() << "  Bytes";
        qDebug() << "[HYB_RADIO] Reading PI XML file, Name = " << fileName ;
    }

    QXmlStreamReader reader( piXmlString );

    while( !reader.atEnd() && !reader.hasError() )
    {
        if( reader.readNext() == QXmlStreamReader::StartElement )
        {
            //This Indicates start of the service
            if( reader.name() == "schedule" )
            {
                epgList.clear();
            }
            else if( reader.name() == "programme" )
            {
                EpgStruct data;
                epgList.push_back(data);
                index = epgList.size() - 1;
            }
            else if( reader.name() == "longName" && index >= 0 )
            {
                epgList[index].mLongName = reader.readElementText();
            }
            else if( reader.name() == "mediaDescription" && index >= 0 )
            {
                bool artCapture = false;
                if( reader.readNextStartElement() )
                {
                    Q_FOREACH( const QXmlStreamAttribute &attr, reader.attributes() )
                    {
                        if ( attr.name().toString() == QLatin1String("url") )
                        {
                            QString attribute_value = attr.value().toString();

                            if( artCapture )
                            {
                                epgList[index].mArt = attribute_value;
                            }
                        }
                        else if( attr.name().toString() == QLatin1String("height") )
                        {
                            QString attribute_value = attr.value().toString();
                            if( attribute_value == "320" )
                            {
                                artCapture = true;
                            }
                        }
                        else if ( attr.name().toString() == QLatin1String("longDescription") )
                         {
                             QString attribute_value = attr.value().toString();
                             epgList[index].mDescription = reader.readElementText();
                         }
                        else
                        {
                            // Do nothing
                        }
                    }
                }
            }
            else if( reader.name() == "location" && index >= 0 )
            {
                if( reader.readNextStartElement() )
                {
                    Q_FOREACH( const QXmlStreamAttribute &attr, reader.attributes() )
                    {
                        if ( attr.name().toString() == QLatin1String("actualTime") )
                        {
                            QString attribute_value = attr.value().toString();
                            epgList[index].mTimeAndDate = attribute_value;
                            QString time;
                            QString date;
                            TimeDateParser( epgList[index].mTimeAndDate, time, date );
                            epgList[index].mTimeAndDate = time;
                        }
                        else if ( attr.name().toString() == QLatin1String("actualDuration") )
                        {
                            QString attribute_value = attr.value().toString();
                            epgList[index].mDuration = attribute_value;
                        }
                        else
                        {
                            // Do nothing
                        }
                    }
                }
            }
            else
            {
                // Do ntohing
            }
        }
    }
}

/**
  @brief ReadSiXmlData

  This method reads the SI xml file and reads and return the relevant data
  in EpgList.
*/
void XmlReader::ReadSiXmlData
    (
    const QString& fileName, //!<[in] File Name to read
    SiDataList& siList //!<[out] Si List
    )
{
    QFile downloadedSiXmlFile( fileName );
    QString siXml("");
    int index = -1;

    if ( downloadedSiXmlFile.open( QIODevice::ReadOnly ) )
    {
        QTextStream in( &downloadedSiXmlFile );
        siXml = in.readAll();
        qDebug() << "[HYB_RADIO] Reading SI XML file, Size = " << downloadedSiXmlFile.size() << "  Bytes";
        qDebug() << "[HYB_RADIO] Reading SI XML file, Name = " << fileName ;
    }

    QXmlStreamReader reader( siXml );

    while( !reader.atEnd() && !reader.hasError() )
    {
        if( reader.readNext() == QXmlStreamReader::StartElement )
        {
            //This Indicates start of the service
            if( reader.name() == "services" )
            {
                siList.clear();
            }
            else if( reader.name() == "service" )
            {
                SiData data;
                siList.push_back( data );
                index = siList.size() - 1;
            }
            else if( reader.name() == "longName" && index >= 0 )
            {
                siList[index].mServiceName = reader.readElementText();
            }
            else if ( reader.name() == "mediaDescription" && index >= 0 )
            {
                if( reader.readNextStartElement() )
                {
                    if( reader.name() == "multimedia" )
                    {
                        bool art_found  = false;
                        QString artCache("");
                        Q_FOREACH( const QXmlStreamAttribute &attr, reader.attributes() )
                        {
                            if ( attr.name().toString() == QLatin1String("height") )
                            {
                                QString attribute_value = attr.value().toString();
                                // This select the image with resolution of
                                // 800x800 for station logo.
                                // Eventually we will have options to select
                                // from different resolutions available.
                                if( attribute_value == "800" || attribute_value == "600" || attribute_value == "128")
                                {
                                    art_found = true;
                                    if( !artCache.isEmpty() )
                                    {
                                        siList[index].mArtwork = artCache;
                                        art_found = false;
                                        artCache = "";
                                    }
                                }
                                else
                                {
                                    art_found = false;
                                }
                            }
                            else if ( attr.name().toString() == QLatin1String("type") )
                            {
                                QString attribute_value = attr.value().toString();
                                if( attribute_value == "logo_unrestricted" )
                                {
                                    art_found = true;
                                    if( !artCache.isEmpty() )
                                    {
                                        siList[index].mArtwork = artCache;
                                        art_found = false;
                                        artCache = "";
                                    }
                                }
                                else
                                {
                                    art_found = false;
                                }
                            }
                            else if ( attr.name().toString() == QLatin1String("url") )
                            {                                
                                QString attribute_value = attr.value().toString();
                                artCache = attribute_value;
                                if( art_found == true )
                                {
                                    siList[index].mArtwork = attribute_value;
                                }
                            }
                            else
                            {
                                // Do nothing
                            }
                        }
                    }
                    else if( "shortDescription" == reader.name() )
                    {
                        siList[index].mDescription = reader.readElementText();
                    }
                    else
                    {
                        // Do nothing
                    }
                }
            }
            else if ( reader.name() == "radiodns" && index >= 0 )
            {
                Q_FOREACH( const QXmlStreamAttribute &attr, reader.attributes() )
                {
                    if ( attr.name().toString() == QLatin1String("fqdn") )
                    {
                        QString attribute_value = attr.value().toString();
                        siList[index].mFqdn = attribute_value;
                    }
                    else if ( attr.name().toString() == QLatin1String("serviceIdentifier") )
                    {
                        QString attribute_value = attr.value().toString();
                        siList[index].mServiceIdentifier = attribute_value;
                    }
                    else
                    {
                        // Do nothing
                    }
                }

            }
            else if( reader.name() == "bearer" && index >= 0 )
            {
                bool id_found = false;
                bool bearer_found = false;
                QString idCache("");
                int bearerIndex = -1;

                Q_FOREACH( const QXmlStreamAttribute &attr, reader.attributes() )
                {
                    if ( attr.name().toString() == QLatin1String("bitrate") )
                    {                        
                        QString attribute_value = attr.value().toString();
                        if( attribute_value == "128" || attribute_value == "112" || attribute_value == "48" )
                        {
                            siList[index].mBitRate = attribute_value;
                            id_found = true;
                        }
                        else
                        {
                            id_found = false;
                        }
                    }
                    else if ( attr.name().toString() == QLatin1String("cost") )
                    {
                        QString attribute_value = attr.value().toString();                        

                        if( attribute_value == "73" || attribute_value == "80" || attribute_value == "70" || attribute_value == "110" || attribute_value == "120")
                        {

                            id_found = true;
                            if(!idCache.isEmpty())
                            {
                                siList[index].mPlayableMedia = idCache;
                                idCache = "";
                                id_found = false;
                            }
                        }
                        else if( attribute_value == "30" || attribute_value == "40" || attribute_value == "100" || attribute_value == "101")
                        {
                            bearer_found = true;
                            if(!idCache.isEmpty())
                            {

                                BearerInfo data;
                                siList[index].mBearerInfo.push_back( data );
                                bearerIndex = siList[index].mBearerInfo.size() - 1;

                                siList[index].mBearerInfo[bearerIndex].mId = idCache;
                                idCache = "";
                                bearer_found = false;
                            }
                        }
                        else
                        {
                            id_found = false;
                            bearer_found = false;
                        }
                    }
                    else if ( attr.name().toString() == QLatin1String("id") )
                    {
                        QString attribute_value = attr.value().toString();
                        idCache = attribute_value;
                        if( true == id_found )
                        {
                            siList[index].mPlayableMedia = attribute_value;
                        }
                        if( bearer_found )
                        {

                            BearerInfo data;
                            siList[index].mBearerInfo.push_back( data );
                            bearerIndex = siList[index].mBearerInfo.size() - 1;

                            siList[index].mBearerInfo[bearerIndex].mId = attr.value().toString();
                        }
                    }
                    else
                    {
                        // Do nothing
                    }
                }
            }
            else if( reader.name() == "genre" && index >= 0 )
            {
                siList[index].mGenre = reader.readElementText();
            }
            else
            {
                // Do nothing
            }
        }
    }
    //qSort( siList.begin(),siList.end(), SiData::SortingComparison );
}

void XmlReader::PrintSiData( const SiDataList& aSiList )
{
    QString filename="SiData.txt";
    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );
        int index = 0;
        for(SiData data: aSiList)
        {
             stream << "[" << index << "]"
                    << " mServiceName = " << data.mServiceName
                    << " mPlayableMedia = " << data.mPlayableMedia
                    << " mFqdn = " << data.mFqdn
                    << " mServiceIdentifier = " << data.mServiceIdentifier
                    << " mArtwork = " << data.mArtwork
                    << endl;
            int bearerIndex = 0;
            for( BearerInfo bearerData: data.mBearerInfo )
            {
                stream << "\t[" << bearerIndex << "]"
                       << "\t mId = " << bearerData.mId
                       << "\t mCost = " << bearerData.mCost
                       << "\t mBitRate = " << bearerData.mBitRate
                       << endl;
                ++bearerIndex;
            }
             ++index;
            stream << endl << endl;
        }
    }

    file.close();
}

