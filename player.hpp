#ifndef PLAYER_HPP
#define PLAYER_HPP
#if 1
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
        delete mPlayer;mPlayer=0;
    }

    bool Play(const QString &url);

    void Stop();

    bool playUrl(const QString &url);

    Player(QObject *parent = 0);

public slots:

     void volumeChanged(int val)
     {
         qDebug()<<"volume Changed "<<val;
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

     void audioAvailableChanged(bool val)
     {
         qDebug()<<"audio avail Changed "<<val;
     }

     void mediaStateChanged(QMediaPlayer::State val)
     {
         //qDebug()<<"media status Changed "<<val;
         emit signalMediaStatusChanged(val);
     }

signals:
     void signalMediaStatusChanged(QMediaPlayer::State val);


private:
    QMediaPlayer *mPlayer;
    QMediaPlaylist *playlist;
    bool _isPlaying;
    int volume;
};

#endif // PLAYER_HPP

#endif
