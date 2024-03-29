#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtCore>
#include <QtNetwork>


class DownloadManager
        :public QNetworkAccessManager
{
  Q_OBJECT
public:
  DownloadManager(QString fileName);
  virtual ~DownloadManager();
  void DownloadFile(QString urlPath);
  void SetFileName( const QString& aFileName );

signals:
  void sendDownloadComplete( const QString& aFilePath );

private slots:

  //If the reply is finished, save data in buffer to disk
  //as a png image: google_image_logo.png
  void slotFinished();

  void slotReadData();

private:
  bool IsFilePresent
      (
      const QString& path
      );
  QByteArray* m_pBuffer;
  QNetworkReply* mReply;
  uint mIndex;
  QString mFileName;

};
#endif // DOWNLOADMANAGER_H
