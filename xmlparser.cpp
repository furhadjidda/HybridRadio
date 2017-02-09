#include "xmlparser.hpp"

XmlReader::XmlReader( DataCollector* collector )
    : mCollector( collector )
{
    if( nullptr == mCollector )
    {
        qCritical() << "Null pointer";
    }
}

void XmlReader::ReadSiXmlData( const QString& fileName)
{
    {
        QFile downloadedSiXmlFile(fileName);
        QString siXml;
        if (downloadedSiXmlFile.open(QIODevice::ReadOnly))
        {
            QTextStream in(&downloadedSiXmlFile);
            siXml = in.readAll();
            qDebug() << "Reading XML file, Size = " << downloadedSiXmlFile.size() << "  Bytes";
            qDebug() << "Reading XML file, Name = " << fileName ;
        }

        QXmlStreamReader reader(siXml);

        while(!reader.atEnd() && !reader.hasError())
        {
            if(reader.readNext() == QXmlStreamReader::StartElement)
            {
                //This Indicates start of the service
                if( reader.name() == "services" )
                {
                    mCollector->StartUpdate();
                }

                if( reader.name() == "service" )
                {
                    mCollector->CollectNextElement();
                }

                if( reader.name() == "longName" )
                {
                    //qDebug() << reader.readElementText();
                    mCollector->SetServiceName( reader.readElementText() );
                }
                else if ( reader.name() == "mediaDescription" )
                {
                    if( reader.readNextStartElement() )
                    {
                        if( reader.name() == "multimedia" )
                        {
                            bool art_found  = false;
                            foreach(const QXmlStreamAttribute &attr, reader.attributes())
                            {
                                if (attr.name().toString() == QLatin1String("height"))
                                {
                                    QString attribute_value = attr.value().toString();
                                    if( attribute_value == "800" )
                                    {
                                        art_found = true;
                                    }
                                    else
                                    {
                                        art_found = false;
                                    }
                                }

                                if (attr.name().toString() == QLatin1String("type"))
                                {
                                    QString attribute_value = attr.value().toString();
                                    if( attribute_value == "logo_unrestricted" )
                                    {
                                        art_found = true;
                                    }
                                    else
                                    {
                                        art_found = false;
                                    }
                                }

                                if (attr.name().toString() == QLatin1String("url") && art_found == true)
                                {
                                    QString attribute_value = attr.value().toString();
                                    //qDebug() << attribute_value;
                                    mCollector->SetArtwork(attribute_value);
                                }
                            }
                        }
                        else if( "shortDescription" == reader.name() )
                        {
                            mCollector->SetDescription( reader.readElementText() );
                        }
                        else
                        {
                            // do nothing
                        }
                    }
                }
                else if ( reader.name() == "radiodns" )
                {
                    foreach(const QXmlStreamAttribute &attr, reader.attributes())
                    {
                        if (attr.name().toString() == QLatin1String("fqdn"))
                        {
                            QString attribute_value = attr.value().toString();
                            mCollector->SetFqdn(attribute_value);
                        }

                        if (attr.name().toString() == QLatin1String("serviceIdentifier"))
                        {
                            QString attribute_value = attr.value().toString();
                            mCollector->SetServiceIdentifier(attribute_value);
                        }
                    }

                }
                else if( reader.name() == "bearer" )
                {
                    bool id_found = false;
                    foreach(const QXmlStreamAttribute &attr, reader.attributes())
                    {
                        if (attr.name().toString() == QLatin1String("bitrate"))
                        {
                            QString attribute_value = attr.value().toString();
                            if( attribute_value == "128" )
                            {
                                //qDebug() << attribute_value;
                                mCollector->SetBitrate( attribute_value );
                                id_found = true;
                            }
                            else
                            {
                                id_found = false;
                            }
                        }

                        if (attr.name().toString() == QLatin1String("mimeValue"))
                        {
                            QString attribute_value = attr.value().toString();
                            if( attribute_value == "audio/mpeg" )
                            {
                                //qDebug() << attribute_value;
                                id_found = true;
                            }
                            else
                            {
                                id_found = false;
                            }
                        }

                        if( true == id_found )
                        {
                            if (attr.name().toString() == QLatin1String("id"))
                            {
                                QString attribute_value = attr.value().toString();
                                //qDebug() << attribute_value;
                                mCollector->SetPlayableMedia(attribute_value);
                            }
                        }
                    }
                }
            }
        }
        mCollector->EndUpdate();
        mCollector->PrintCompleteList();
    }
}
