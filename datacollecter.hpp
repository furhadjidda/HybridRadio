#ifndef DATACOLLECTER_HPP
#define DATACOLLECTER_HPP

#include <QString>
#include <QVector>

struct StationData{
    QString serviceName;
    QString artwork;
    QString description;
    QString genre;
    QString bitRate;
    QString playableMedia;
    QString fqdn;
    QString serviceIdentifier;
};

struct EpgData{
    QString description;
    QString longName;
    QString timeAndDate;
    QString duration;
};

typedef QVector<StationData> StationList;
typedef QVector<EpgData> EpgList;

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
    void SetDescription( const QString& val );
    void SetGenre( const QString& val );
    void SetBitrate( const QString& val );
    void SetFqdn( const QString& val );
    void SetServiceIdentifier( const QString& val );
    void EndUpdate();
    void PrintCompleteList();
    void GetCompleteList( StationList& data )
    {
        data = list;
    }

    void StartEPGUpdate();
    void CollectNextEPGElement();
    void SetEPGDescription( const QString& val );
    void SetEPGTimeAndDate( const QString& val );
    void SetEPGLongName( const QString& val );
    void SetEPGDuration( const QString& val );
    void EndEPGUpdate();
    void PrintEPGCompleteList();
    void GetEPGCompleteList( EpgList& data )
    {
        data = epgList;
    }
private:
    StationList list;
    bool isListUpdating;
    uint index;

    EpgList epgList;
    bool isEPGListUpdating;
    uint epgIndex;
};



#endif // DATACOLLECTER_HPP

