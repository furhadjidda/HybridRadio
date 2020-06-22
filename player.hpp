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
        delete mPlayer;
        mPlayer = nullptr;
    }

    bool Play(const QString &url);

    void Stop();

    bool playUrl(const QString &url);

    Player(QObject *parent = 0);

public slots:

     void volumeChanged(int val)
     {
         qDebug()<<"[PLAYER] Volume Changed "<<val;
     }

     void mutedChanged(bool val)
     {
         qDebug()<<"[PLAYER] Mute Changed "<<val;
     }
     void mediaStatusChanged(QMediaPlayer::MediaStatus val)
     {
         qDebug()<<"[PLAYER] Media status Changed "<<val;
     }

     void durationChanged(qint64 duration)
     {
         qDebug()<<"[PLAYER] Duration "<<duration;

     }

     void audioAvailableChanged(bool val)
     {
         qDebug()<<"[PLAYER] Audio avail Changed "<<val;
     }

     void mediaStateChanged( QMediaPlayer::State val )
     {
         emit signalMediaStatusChanged(val);
     }

signals:
     void signalMediaStatusChanged(QMediaPlayer::State val);


private:
    QMediaPlayer* mPlayer;
};

#endif // PLAYER_HPP

#endif
