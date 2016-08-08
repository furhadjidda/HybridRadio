#ifndef DATACOLLECTER_HPP
#define DATACOLLECTER_HPP

#include <QString>
#include <QVector>

struct StationData{
    QString serviceName;
    QString artwork;
    QString playableMedia;
};

typedef QVector<StationData> StationList;

class DataCollector{

public:
    DataCollector()
    {
        isListUpdating = false;
        index = 0;
    }
    ~DataCollector(){}
    void StartUpdate();
    void CollectNextElement();
    void SetServiceName( const QString& val );
    void SetArtwork( const QString& val );
    void SetPlayableMedia( const QString& val );
    void EndUpdate();
    void PrintCompleteList();
    void GetCompleteList( StationList& data )
    {
        data = list;
    }

private:
    StationList list;
    bool isListUpdating;
    uint index;
};



#endif // DATACOLLECTER_HPP

