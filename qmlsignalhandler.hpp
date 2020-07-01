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
    void OnServiceInformationAvailable( const QString& aFilePath );
    void OnServiceInformationDownloaded( const QString& aFilePath );
    void OnProgramInformationDownloaded( const QString& aFilePath );
    void OnTextChanged( const QString& aText );
    void OnImageChanged( const QString& aImage );
    void MediaStatusChanged(QMediaPlayer::State val);
    void OnHttpVisSupported( bool aVal );
    void OnStompVisSupported( bool aVal );
    void OnStompConnectionReady();

private:
    QString DownloadProgramInformation(QString fqdn, QString serviceIdentifier);
    void UpdateUIFromList( int aIndex );
    void ShowNoAudioStreamAvaialablePopup( bool val );
    void ConnectSignals();
    void ClearMetaData();
    void PlayAtIndex( const qint16 aIndex );

    QObject* mUIObject;
    std::unique_ptr<Player> mPlayer;
    std::unique_ptr<XmlReader> mReader;
    std::unique_ptr<DNSLookup> mDnsLookup;
    DownloadManager* mServiceInformationDownloader;
    DownloadManager* mProgramInformationDownloader;

    SiDataList mList;
    QString m_CurrentLyPlaying;
    QString mCurrentSelection;
    QString mCurrentBearer;
    // Topic Handling
    QString mTextTopic;
    QString mImageTopic;
    qint16 mCurrentPlayingIndex{0};

    // UI Handler
    UiHandler mUiHandler{ mUIObject };

    HttpTransport mHttpTransport;
    StompTransport mStompTransport;

    GccHelper mGccHelper;

    std::unique_ptr<HybridRadioCore> mHybridRadioCore;
};
#endif // MAIN_H

