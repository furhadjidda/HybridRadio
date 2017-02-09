#include "datacollecter.hpp"
#include <QDebug>
void DataCollector::StartEPGUpdate()
{
    list.clear();
    if( isEPGListUpdating != true )
    {
        isEPGListUpdating = true;
        EpgData data;
        epgList.push_back(data);
        epgIndex = epgList.size();
    }
}

void DataCollector::CollectNextEPGElement()
{
    EpgData data;
    epgList.push_back(data);
    epgIndex = epgList.size();

}

void DataCollector::SetEPGDescription(const QString& val)
{
    if( index > 0 )
    {
        epgList[epgIndex - 1].description = val;
    }
}

void DataCollector::SetEPGTimeAndDate(const QString& val)
{
    if( index > 0 )
    {
        epgList[epgIndex - 1].timeAndDate = val;
    }
}

void DataCollector::EndEPGUpdate()
{
    isEPGListUpdating = false;
}

void DataCollector::PrintEPGCompleteList()
{
    for(uint index = 0 ; index < epgList.size(); ++index)
    {
        qDebug() << " TAD:" << epgList[index].timeAndDate
                 << " DSC: " << epgList[index].description;
    }
}

void DataCollector::StartUpdate()
{
    list.clear();
    if( isListUpdating != true )
    {
        isListUpdating = true;
        StationData data;
        list.push_back(data);
        index = list.size();
    }
}

void DataCollector::CollectNextElement()
{
    StationData data;
    list.push_back(data);
    index = list.size();

}

void DataCollector::SetServiceName(const QString& val)
{
    if( index > 0 )
    {
        list[index - 1].serviceName = val;
    }
}

void DataCollector::SetArtwork(const QString& val)
{
    if( index > 0 )
    {
        list[index - 1].artwork = val;
    }
}

void DataCollector::SetPlayableMedia(const QString& val)
{
    if( index > 0 )
    {
        list[index - 1].playableMedia = val;
    }
}

void DataCollector::SetDescription( const QString& val )
{
    if( index > 0 )
    {
        list[index - 1].description = val;
    }
}

void DataCollector::SetGenre( const QString& val )
{
    if( index > 0 )
    {
        list[index - 1].genre = val;
    }
}

void DataCollector::SetBitrate( const QString& val )
{
    if( index > 0 )
    {
        list[index - 1].bitRate = val;
    }
}

void DataCollector::SetFqdn( const QString& val )
{
    if( index > 0 )
    {
        list[index - 1].fqdn = val;
    }
}

void DataCollector::SetServiceIdentifier( const QString& val )
{
    if( index > 0 )
    {
        list[index - 1].serviceIdentifier = val;
    }
}

void DataCollector::EndUpdate()
{
    isListUpdating = false;
}

void DataCollector::PrintCompleteList()
{
    for(uint index = 0 ; index < list.size(); ++index)
    {
        qDebug() << " AW:" << list[index].artwork
                 << " PM: " << list[index].playableMedia
                 << " SN: " << list[index].serviceName
                 << " FQDN: " << list[index].fqdn
                 << " SI: " << list[index].serviceIdentifier;
    }
}


