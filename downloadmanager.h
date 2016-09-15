#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtCore>
#include <QtNetwork>


class Downloader:public QNetworkAccessManager
{
  Q_OBJECT
public:
  Downloader();
  ~Downloader();
  void DownloadFile(QString urlPath,bool isSI = true);

signals:
  void sendDownloadComplete();

private slots:

  //If the reply is finished, save data in buffer to disk
  //as a png image: google_image_logo.png
  void slotFinished();

  void slotReadData();
private:
  QByteArray* m_pBuffer;
  QNetworkReply* reply;
  uint mIndex;

};
#endif // DOWNLOADMANAGER_H
