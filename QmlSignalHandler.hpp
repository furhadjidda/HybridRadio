#ifndef MAIN_H
#define MAIN_H
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick>
#include <QObject>
#include <QDebug>
#include <memory>
#include <QUrl>
#include "HybridRadioCore.hpp"
#include "UiHandler.hpp"
#include "PresetHandler.hpp"

class SignalHandler : public QObject
{
Q_OBJECT

public:
    SignalHandler
        (
        QObject *object
        );
    virtual ~SignalHandler();

public slots:
    void OnPlay();
    void OnStop();
    void OnNext();
    void OnPrevious();
    void OnSelect( int aIndex );
    void OnSelectionChanged( QString value );
    void OnServiceInformationDownloaded();
    void OnProgramInformationDownloaded();
    void OnTextChanged( const QString& aText );
    void OnImageChanged( const QString& aImage );
    void OnHttpVisSupported( bool aVal );
    void OnStompVisSupported( bool aVal );
    void OnAudioStreamAvailability( bool val );
    void OnStationNameChanged( const QString& aData );
    void OnDescriptionChanged( const QString& aData );
    void OnBitRateChanged( const QString& aData );
    void OnMediaStateChanged( const QString& aData );
    void OnStationFound( const SiData& aData );
    void OnPresetSave(const QString& aData);
    void OnPresetRecall(const QString& aData);

private:
    void ConnectSignals();
    void ClearMetaData();
    void PlayAtIndex( const qint16 aIndex );
    void PopulateAdditionalInfo( const SiData& aData );
    void PopulatePresetFields();

    QObject* mUIObject;
    std::unique_ptr<HybridRadioCore> mHybridRadioCore;

    SiDataList mList;
    EpgList mProgramList;
    qint32 mCurrentPlayingIndex;
    // UI Handler
    UiHandler mUiHandler{ mUIObject };
    GccHelper mGccHelper;
    PresetHandler mPresets;
};
#endif // MAIN_H

