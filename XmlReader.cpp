

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

        qDebug() << "[XML_READER] Reading " << fileName << "\nSize = " << piXmlFile.size() << " Bytes";

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
        qDebug() << "[XML_READER] Reading " << fileName << " Size = " << downloadedSiXmlFile.size() << " Bytes";
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

            else if( "shortDescription" == reader.name()  && index >= 0 )
            {
                siList[index].mDescription = reader.readElementText();
            }
            else if ( reader.name() == "mediaDescription" && index >= 0 )
            {
                if( reader.readNextStartElement() )
                {
                    if( reader.name() == "multimedia" )
                    {
                        QString key;
                        QString value;
                        Q_FOREACH( const QXmlStreamAttribute &attr, reader.attributes() )
                        {
                            if ( attr.name().toString() == QLatin1String("height") )
                            {
                                QString attribute_value = attr.value().toString();
                                key = attribute_value;
                            }
                            else if ( attr.name().toString() == QLatin1String("url") )
                            {
                                QString attribute_value = attr.value().toString();
                                value = attribute_value;
                            }
                            else
                            {
                                // Do nothing
                            }

                            if( !key.isEmpty() && !value.isEmpty() )
                            {
                                siList[index].mArtworkAvailable.insert( key, value );
                                siList[index].mArtwork = value;
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
                Q_FOREACH( const QXmlStreamAttribute &attr, reader.attributes() )
                {
                    if ( attr.name().toString() == QLatin1String("bitrate") )
                    {                        
                        QString attribute_value = attr.value().toString();
                        siList[index].mBitRate = attribute_value;
                    }
                    else if ( attr.name().toString() == QLatin1String("id") )
                    {
                        QString attribute_value = attr.value().toString();
                        // This should add playable media
                        if( attribute_value.contains("http",Qt::CaseInsensitive) || attribute_value.contains("https",Qt::CaseInsensitive) )
                        {
                            siList[index].mPlayableMedia = attribute_value;
                            siList[index].mPlaylableMediaList.push_back( attribute_value );
                        }
                        //This should add the brearers
                        if( attribute_value.startsWith("fm:") || attribute_value.startsWith("dab:")|| attribute_value.startsWith("hd:") )
                        {
                            BearerInfo data;
                            siList[index].mBearerInfo.push_back( data );
                            int bearerIndex = siList[index].mBearerInfo.size() - 1;
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
    //PrintSiData( siList );
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
             for( auto artWork : data.mArtworkAvailable.keys() )
             {
                 stream << "\t [ ARTWORK ]" << artWork << " -> "<< data.mArtworkAvailable.value(artWork) << endl;
             }
             for( auto media : data.mPlaylableMediaList )
             {
                 stream << "\t [ PLAYABLEMEDIA ]" << media << endl;
             }
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


void XmlReader::TimeDateParser
    (
    const QString& aQString,
    QString& aTime,
    QString& aDate
    )
{
    QRegExp rx("(\\T)"); //RegEx for ' ' or ',' or '.' or ':' or '\t'
    QStringList  query = aQString.split(rx);
    if( 0 == query.size() )
    {
        return;
    }
    aTime = query.at(1);
    aDate = query.at(0);
}

