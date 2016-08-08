#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <QMultimedia>
#include <QMediaPlayer>
#include <QStringList>
#include <QSettings>
#include <QPair>
#include <QString>
#include <QObject>
#include <QMediaPlaylist>
class Player : public QObject{
Q_OBJECT
public:
    ~Player()
    {
        qDebug()<<"Destructor Called";
        delete _player;_player=0;
    }

    bool Play(const QString &url);

    void Stop();

    bool playUrl(const QString &url);

    Player(QObject *parent = 0);

public slots:
     void positionChanged(qint64 val)
     {
         qDebug()<<"position Changed"<<val;
     }
     void volumeChanged(int val)
     {
         qDebug()<<"volume Changed "<<val;
     }
     void audioAvailableChanged(bool val)
     {
         qDebug()<<"audio avail Changed "<<val;
     }
     void mutedChanged(bool val)
     {
         qDebug()<<"mute Changed "<<val;
     }
     void mediaStatusChanged(QMediaPlayer::MediaStatus val)
     {
         qDebug()<<"media status Changed "<<val;
     }
     void durationChanged(qint64 duration)
     {
         qDebug()<<"Duration "<<duration;

     }


private:
    QMediaPlayer *_player;
    QMediaPlaylist *playlist;
    bool _isPlaying;
    int volume;
};

#endif // PLAYER_HPP

