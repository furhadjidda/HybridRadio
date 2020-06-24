#include "player.hpp"
#include <QUrl>

#if 1
bool Player::playUrl
    (
    const QString &url
    )
{
    mPlayer->setMedia( QMediaContent( url ) );
    mPlayer->setVolume( 100 );
    mPlayer->play();
    qDebug() << "[PLAYER] Audio Available = " << mPlayer->isAudioAvailable()
             << " Mute = " << mPlayer->isMuted()
             << " Player Available = " << mPlayer->isAvailable()
             << " Audio Available= " << mPlayer->isAudioAvailable()
             << " State = "<< mPlayer->state();

    return true;
}

void Player::Stop()
{
    mPlayer->stop();
}

Player::Player(QObject *parent)
    : QObject(parent)
{
    mPlayer = new QMediaPlayer
                    (
                    this,
                    QMediaPlayer::StreamPlayback
                    );

    // Connect all the signals
    QObject::connect
        (
        mPlayer,
        static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
        [=]( QMediaPlayer::Error aError )
            {
                qDebug() << "[PLAYER] Error = " << aError;
            }
        );


    QObject::connect
        (
        mPlayer,
        &QMediaPlayer::audioAvailableChanged,
        this,
        &Player::audioAvailableChanged
        );

    QObject::connect
        (
        mPlayer,
        &QMediaPlayer::stateChanged,
        this,
        &Player::mediaStateChanged
        );

}
#endif
