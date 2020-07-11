#include "DownloadManager.hpp"
#include <QDateTime>
#include <QTime>
#include <QRegularExpression>
#include <QFileInfo>
#include <QDateTime>


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
    if( IsFilePresent( mFileName ) )
    {
        qDebug() << "[ DOWNLOADER ] File Present , no need for download";
        emit sendDownloadComplete( mFileName );
        return;
    }
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
     if ( mReply->error() == QNetworkReply::NoError )
     {
         int statusCode = mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
         if( statusCode == 301 )
         {
             QUrl redirectUrl = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
             qDebug() << "[ DOWNLOADER ] Redirection URL = " << redirectUrl;
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

bool DownloadManager::IsFilePresent
    (
    const QString& path
    )
{
    QFileInfo check_file( path );
    QDateTime birthTime = check_file.birthTime();
    QDateTime currDateAndTime = QDateTime::currentDateTime();

    if( birthTime.daysTo( currDateAndTime ) > 5 )
    {
        // Data could be out of date.
        //return false;
    }

    // check if file exists and if yes: Is it really a file and no directory?
    return ( check_file.exists() && check_file.isFile() );
}
