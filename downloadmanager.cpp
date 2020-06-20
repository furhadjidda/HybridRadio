#include "downloadmanager.h"
#include <QDateTime>
#include <QTime>
#include <QRegularExpression>


DownloadManager::DownloadManager
    (
    QString fileName
    )
    : mIndex( 0 )
    , mFileName( fileName )
{
}

void DownloadManager::DownloadFile( QString urlPath )
{
    m_pBuffer = new QByteArray();
    QUrl url = QUrl( urlPath );
    QNetworkRequest req( url );
    mReply = get(req);
    connect
        (
        mReply,
        SIGNAL(readyRead()),
        this,
        SLOT(slotReadData())
        );

    connect
        (
        mReply,
        SIGNAL(finished()),
        this,
        SLOT(slotFinished())
        );
}

DownloadManager::~DownloadManager()
{
    delete m_pBuffer;
    delete mReply;
}

void DownloadManager::SetFileName( const QString& aFileName )
{
    mFileName = aFileName;
}

//If the reply is finished, save data in buffer to disk
//as a png image: google_image_logo.png
void DownloadManager::slotFinished()
{
    //Check status code
     if ( mReply->error() == QNetworkReply::NoError ) {
         int statusCode = mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
         if( statusCode == 301 )
         {
             QUrl redirectUrl = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
             qDebug() << "Redirection URL = " << redirectUrl;
             DownloadFile( redirectUrl.toString() );
             return;
         }
     }

    if( 0 == mFileName.length() )
    {
        qWarning() << "[ DOWNLOADER ] File Name empty ";
        return;
    }

    QFile file( mFileName );

    if( file.open( QIODevice::WriteOnly ) )
    {
        file.write(*m_pBuffer);
        file.close();
        qDebug() << "[ DOWNLOADER ] Downloaded the File " << mFileName;
        emit sendDownloadComplete( mFileName );
    }
    else
    {
        qDebug() << "[ DOWNLOADER ] Error Saving File";
    }
}

void DownloadManager::slotReadData()
{
    //append data to QByteArray buffer
    *m_pBuffer += mReply->readAll();
}
