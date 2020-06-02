#include "downloadmanager.h"
#include <QDateTime>
#include <QTime>


DownloadManager::DownloadManager(QString fileName)
{
    mFileName = fileName;
    mIndex = 0;
}

void DownloadManager::DownloadFile(QString urlPath)
{
    m_pBuffer = new QByteArray();
    QUrl url = QUrl(urlPath);
    QNetworkRequest req(url);
    reply = get(req);
    connect(reply,SIGNAL(readyRead()),this,SLOT(slotReadData()));
    connect(reply,SIGNAL(finished()), this,SLOT(slotFinished()));
}

DownloadManager::~DownloadManager()
{
    delete m_pBuffer;
    delete reply;
}

void DownloadManager::SetFileName( const QString& aFileName )
{
    mFileName = aFileName;
}

//If the reply is finished, save data in buffer to disk
//as a png image: google_image_logo.png
void DownloadManager::slotFinished()
{
    QString fileName;
    if(mFileName.length() == 0)
    {
        fileName = "Downloaded_SI.xml";
    }
    else
    {
        fileName = mFileName;
    }

    QFile file( fileName );

    if(file.open(QIODevice::WriteOnly))
    {
        file.write(*m_pBuffer);
        file.close();
        qDebug() << "Downloaded the File !!";
        emit sendDownloadComplete( fileName );
    }
    else
    {
        qDebug() << "Error Saving File";
    }
}

void DownloadManager::slotReadData()
{
    //append data to QByteArray buffer
    *m_pBuffer += reply->readAll();
}
