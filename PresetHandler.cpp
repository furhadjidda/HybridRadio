#include "PresetHandler.hpp"

static const QString CreateTableSqlQuery("CREATE TABLE IF NOT EXISTS presetData("
                                 "id int primary KEY,"
                                 "bearer TEXT NOT NULL,"
                                 "serviceId TEXT NOT NULL,"
                                 "audioStream TEXT NOT NULL,"
                                 "artWork TEXT NOT NULL"
                                 ");"
                                 );
static const qint16 PresetCount = 4;

PresetHandler::PresetHandler()
{
    mPresetDatabase = QSqlDatabase::addDatabase("QSQLITE");
    mPresetDatabase.setDatabaseName("/home/pi/qt-apps/presets.sqlite");
    mPresetDatabase.open();
    QSqlQuery query( mPresetDatabase );
    query.exec( CreateTableSqlQuery );

    QSqlQuery batchquery( mPresetDatabase );
    batchquery.prepare("INSERT INTO presetData values (?, ?, ?, ?, ?)");
    QVariantList keys;
    keys << 1 << 2 << 3 << 4;
    batchquery.addBindValue( keys, QSql::In );

    QVariantList bearer;
    bearer << "Preset1_bearer" << "Preset2_bearer" << "Preset3_bearer"<< "Preset4_bearer";
    batchquery.addBindValue( bearer, QSql::In );

    QVariantList serviceId;
    serviceId << "Preset1_serviceId" << "Preset2_serviceId" << "Preset3_serviceId"<< "Preset4_serviceId";
    batchquery.addBindValue( serviceId, QSql::In );

    QVariantList audioStream;
    audioStream << "audioStream" << "audioStream" << "audioStream" << "audioStream";
    batchquery.addBindValue( audioStream, QSql::In );

    QVariantList artWork;
    artWork << "Qml-PresetAdd.png" << "Qml-PresetAdd.png" << "Qml-PresetAdd.png" << "Qml-PresetAdd.png";
    batchquery.addBindValue( artWork, QSql::In );

    if(! batchquery.execBatch())
    {
        qDebug() << batchquery.lastError();
    }
}


void PresetHandler::SavePreset
    (
    const qint16 aBankId,
    const SiData& aSiData
    )
{
    qDebug() << "[PRESET HANDLER] Saving Preset at " << aBankId;
    QSqlQuery query( mPresetDatabase );
    query.prepare( "Update presetData SET bearer = ?, serviceId = ?,audioStream = ?, artWork = ? WHERE id = ?;" );
    query.addBindValue( aSiData.mBearerInfo[0].mId );
    query.addBindValue( aSiData.mServiceIdentifier );
    query.addBindValue( aSiData.mPlayableMedia );
    query.addBindValue( aSiData.mArtwork );
    query.addBindValue( aBankId );
    if(! query.exec())
    {
        qDebug() <<"[PRESET HANDLER] Error - " << query.lastError();
    }
}

void PresetHandler::RecallPreset
    (
    const qint16 aBankId,
    SiData& aSiData
    )
{
    QString queryText("select * from presetData where id = " + QString::number( aBankId, 10 ) );
    qDebug() << "[PRESET HANDLER]" << queryText;
    QSqlQuery q( queryText, mPresetDatabase );
    QSqlRecord rec = q.record();

    int bearer = rec.indexOf("bearer"); // index of the field "name"
    int serviceId = rec.indexOf("serviceId"); // index of the field "name"
    int audioStream = rec.indexOf("audioStream"); // index of the field "name"
    int artWork = rec.indexOf("artWork"); // index of the field "name"
    while ( q.next() )
    {
        BearerInfo data;
        data.mId = q.value(bearer).toString();
        aSiData.mBearerInfo.push_back( data );
        aSiData.mServiceIdentifier = q.value(serviceId).toString();
        aSiData.mPlayableMedia = q.value(audioStream).toString();
        aSiData.mArtwork = q.value(artWork).toString();
        qDebug() << aSiData.FormattedData();
    }
}

void PresetHandler::GetAllPresets
    (
    SiDataList& aSiData
    )
{
    for( int index = 1; index <= PresetCount; ++index )
    {
        SiData siData;
        QString queryText("select * from presetData where id = " + QString::number( index, 10 ) );
        qDebug() << queryText;
        QSqlQuery q( queryText, mPresetDatabase );
        QSqlRecord rec = q.record();

        int bearer = rec.indexOf("bearer"); // index of the field "name"
        int serviceId = rec.indexOf("serviceId"); // index of the field "name"
        int artWork = rec.indexOf("artWork"); // index of the field "name"
        while ( q.next() )
        {
            BearerInfo data;
            data.mId = q.value(bearer).toString();
            siData.mBearerInfo.push_back( data );
            siData.mServiceIdentifier = q.value(serviceId).toString();
            siData.mArtwork = q.value(artWork).toString();
            qDebug() << siData.FormattedData();
            aSiData.push_back( siData );
        }
    }
}

