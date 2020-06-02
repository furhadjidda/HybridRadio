#include "player.hpp"
#include <QUrl>

#if 1
bool Player::playUrl(const QString &url)
{
    _isPlaying = true;
    mPlayer->setMedia(QMediaContent(url));
    mPlayer->setVolume(100);
    mPlayer->play();
    qDebug() << "audio = " << mPlayer->isAudioAvailable();
    qDebug() << "mute = " << mPlayer->isMuted();
    qDebug() << "player = " << mPlayer->isAvailable();
    qDebug() << "audio = " << mPlayer->isAudioAvailable();
    qDebug() << "State = "<< mPlayer->state();
    qDebug() << "Duration = "<< mPlayer->duration();

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
    volume = 50;
    mPlayer = new QMediaPlayer(this,QMediaPlayer::StreamPlayback);

    // Connect all the signals
    connect(mPlayer, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
        [=](QMediaPlayer::Error error){ qDebug() << "Error = " << error; });


    //connect(mPlayer, SIGNAL(volumeChanged(int)), this, SLOT(volumeChanged(int)));

    connect(mPlayer, SIGNAL(audioAvailableChanged(bool)), this, SLOT(audioAvailableChanged(bool)));

    //connect(mPlayer, SIGNAL(mutedChanged(bool)), this, SLOT(mutedChanged(bool)));

    //connect(mPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    //connect(mPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    //mediaStateChanged(QMediaPlayer::State val)
    connect(mPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State)));

}
#endif
