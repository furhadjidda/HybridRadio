#include "player.hpp"
#include <QUrl>

#if 1
bool Player::playUrl(const QString &url)
{
    _isPlaying = true;
    mPlayer->setMedia( QMediaContent( url ) );
    mPlayer->setVolume( 100 );
    mPlayer->play();
    qDebug() << "[PLAYER] Audio = " << mPlayer->isAudioAvailable();
    qDebug() << "[PLAYER] Mute = " << mPlayer->isMuted();
    qDebug() << "[PLAYER] Player = " << mPlayer->isAvailable();
    qDebug() << "[PLAYER] Audio = " << mPlayer->isAudioAvailable();
    qDebug() << "[PLAYER] State = "<< mPlayer->state();
    qDebug() << "[PLAYER] Duration = "<< mPlayer->duration();

    return true;
}

void Player::Stop()
{
    mPlayer->stop();
}

Player::Player(QObject *parent)
    : QObject(parent)
{
    _isPlaying = false;

    mPlayer = new QMediaPlayer
                    (
                    this,
                    QMediaPlayer::StreamPlayback
                    );

    // Connect all the signals
    connect
        (
        mPlayer,
        static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
        [=]( QMediaPlayer::Error aError )
            {
                qDebug() << "[PLAYER] Error = " << aError;
            }
        );


    connect(mPlayer, SIGNAL(audioAvailableChanged(bool)), this, SLOT(audioAvailableChanged(bool)));

    //connect(mPlayer, SIGNAL(mutedChanged(bool)), this, SLOT(mutedChanged(bool)));

    //connect(mPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    //connect(mPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    //mediaStateChanged(QMediaPlayer::State val)
    connect(mPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State)));

}
#endif
