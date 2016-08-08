#include "player.hpp"
#include <QUrl>


bool Player::playUrl(const QString &url)
{
    _isPlaying = true;
    _player->setMedia(QMediaContent(url));
    _player->setVolume(50);
    _player->play();
    qDebug() << "audio = " << _player->isAudioAvailable();
    qDebug() << "mute = " << _player->isMuted();
    qDebug() << "player = " << _player->isAvailable();
    qDebug() << "audio = " << _player->isAudioAvailable();
    qDebug() << "State = "<< _player->state();
    qDebug() << "Duration = "<< _player->duration();

    return true;
}

void Player::Stop()
{
    _player->stop();
}

Player::Player(QObject *parent)
    : QObject(parent)
{
    _isPlaying = false;
    volume = 50;
    _player = new QMediaPlayer(this,QMediaPlayer::StreamPlayback);

    // Connect all the signals
    connect(_player, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
        [=](QMediaPlayer::Error error){ qDebug() << "Error = " << error; });

    connect(_player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));

    connect(_player, SIGNAL(volumeChanged(int)), this, SLOT(volumeChanged(int)));

    connect(_player, SIGNAL(audioAvailableChanged(bool)), this, SLOT(audioAvailableChanged(bool)));

    connect(_player, SIGNAL(mutedChanged(bool)), this, SLOT(mutedChanged(bool)));

    connect(_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    connect(_player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));

}
