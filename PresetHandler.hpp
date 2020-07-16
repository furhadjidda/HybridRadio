#ifndef PRESETHANDLER_HPP
#define PRESETHANDLER_HPP
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include "HybridRadioCommonTypes.hpp"
#include <QVariantList>
#include <QSqlRecord>

class PresetHandler
{
public:
    PresetHandler();

    void SavePreset
        (
        const qint16 aBankId,
        const SiData& aSiData
        );

    void RecallPreset
        (
        const qint16 aBankId,
        SiData& aSiData
        );

    void GetAllPresets
        (
        SiDataList& aSiData
        );

private:
    QSqlDatabase mPresetDatabase;
};

#endif // PRESETHANDLER_HPP
