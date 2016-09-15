#include "downloadmanager.h"
#include <QDateTime>
#include <QTime>


Downloader::Downloader()
{
    mIndex = 0;
}

void Downloader::DownloadFile(QString urlPath, bool isSI)
{
    if(isSI){}
    m_pBuffer = new QByteArray();
    QUrl url = QUrl(urlPath);
    QNetworkRequest req(url);
    reply = get(req);
    connect(reply,SIGNAL(readyRead()),this,SLOT(slotReadData()));
    connect(reply,SIGNAL(finished()), this,SLOT(slotFinished()));
}

Downloader::~Downloader()
{
    delete m_pBuffer;
    delete reply;
}

//If the reply is finished, save data in buffer to disk
//as a png image: google_image_logo.png
void Downloader::slotFinished()
{
    QString fileName;
    fileName = "Downloaded_SI.xml";

    QFile file(fileName);

    if(file.open(QIODevice::WriteOnly))
    {
        file.write(*m_pBuffer);
        file.close();
        qDebug() << "Downloaded the File !!";
        emit sendDownloadComplete();
    }
    else
    {
        qDebug() << "Error Saving File";
    }
}

void Downloader::slotReadData()
{
    //append data to QByteArray buffer
    qDebug() << "Reading Data";
    *m_pBuffer += reply->readAll();
}
