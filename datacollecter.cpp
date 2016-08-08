#include "datacollecter.hpp"
#include <QDebug>
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
                 << " SN: " << list[index].serviceName;
    }
}


