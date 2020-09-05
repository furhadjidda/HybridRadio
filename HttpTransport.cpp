#include "HttpTransport.hpp"
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonDocument>

static const QString RadioVISMessageID("RadioVIS-Message-ID");
static const QString RadioVISTriggerTime("RadioVIS-Trigger-Time");
const int sTimerValue = 10000;

HttpTransport::HttpTransport()
{
    mTextTopicTimer = new QTimer( this );
    mImageTopicTimer = new QTimer( this );

    QObject::connect
            (
            mTextTopicTimer,
            &QTimer::timeout,
            this,
            &HttpTransport::OnTextTopicTimeExpired
            );

    QObject::connect
            (
            mImageTopicTimer,
            &QTimer::timeout,
            this,
            &HttpTransport::OnImageTopicTimeExpired
            );
}


void HttpTransport::SetPortAndTarget
    (
    const QString& aPort,
    const QString& aTarget
    )
{
    mPort = aPort;
    mTarget = aTarget;
    if( !mPort.isEmpty() && !mTarget.isEmpty() )
    {
        mIsSupported = true;
    }
}

void HttpTransport::SubscribeTextTopic( const QString& aTextTopic )
{
    if( mIsDiabled )
        return;

    RequestTextTopic( aTextTopic );
    mCurrentTextTopic = aTextTopic;
    mTextTopicTimer->start( sTimerValue );
}

void HttpTransport::SubscribeImageTopic( const QString& aImageTopic )
{
    if( mIsDiabled )
        return;

    RequestImageTopic( aImageTopic );
    mCurrentImageTopic = aImageTopic;
    mImageTopicTimer->start( sTimerValue );
}

void HttpTransport::OnTextResponse()
{
    QString textJson( mTextReply->readAll().data() );
    mTextResponse.clear();
    qDebug() <<"[HTTP-TRANSPORT] Http Text Response : " << textJson << "\n\n";
    if( 0 == textJson.size() )
    {
        return;
    }
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( textJson.toUtf8(), &jsonError );
    if (jsonError.error != QJsonParseError::NoError)
    {
        qWarning() << "[HTTP-TRANSPORT] Error in Reading Json" << jsonError.errorString();
    }
    QJsonObject jsonObject = jsonDoc.object();
    QVariantMap jsonQVariantMap = jsonObject.toVariantMap();
    QVariantMap  jsonHeaderMap = jsonQVariantMap["headers"].toMap();
    if( !jsonHeaderMap.empty() )
    {
        mTextResponse = jsonHeaderMap;
    }

    QString value = jsonObject["body"].toString();
    QString SongName = value.remove(0,5);

    emit SignalTextChanged( SongName );
}

void HttpTransport::OnImageResponse()
{
    QString imageJson = QString( mImageReply->readAll().data() );
    qDebug() <<"[HTTP-TRANSPORT] Http Image Response : " << imageJson  << "\n\n";
    mImageResponse.clear();

    if( 0 == imageJson.size() )
    {
        return;
    }

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( imageJson.toUtf8(), &jsonError );
    if (jsonError.error != QJsonParseError::NoError)
    {
        qWarning() << "[HTTP-TRANSPORT] Error in Reading Json" << jsonError.errorString();
    }
    QJsonObject jsonObject = jsonDoc.object();
    QVariantMap jsonQVariantMap = jsonObject.toVariantMap();
    QVariantMap  jsonHeaderMap = jsonQVariantMap["headers"].toMap();
    if( !jsonHeaderMap.empty() )
    {
        mImageResponse = jsonHeaderMap;
    }
    QString value = jsonObject["body"].toString();
    QString artLink = value.remove(0,5);

    emit SignalImageChanged( artLink );
}

void HttpTransport::OnTextTopicTimeExpired()
{
    RequestTextTopic( mCurrentTextTopic );
}


void HttpTransport::OnImageTopicTimeExpired()
{
    RequestImageTopic( mCurrentImageTopic );
}

void HttpTransport::RequestTextTopic( const QString& aTextTopic )
{
    // Construct a url for text topic
    if( mIsDiabled )
        return;

    QVariantMap::const_iterator iter = mTextResponse.find( RadioVISMessageID );

    QString lastTextId("");
    if( iter.value().canConvert( QMetaType::QString ) && ( QVariant::String == iter.value().type() ) )
    {
        // potential place of crash ??
        qDebug() << "[HTTP-TRANSPORT] key = " << iter.key();
        qDebug() << "[HTTP-TRANSPORT] value = " << iter.value();
        lastTextId = iter.value().toString(); // This is mandatory if not initial request;
    }
    else
    {
        qWarning() << "[HTTP-TRANSPORT] Http Text Timeout";
    }
    if( 0 != lastTextId.size() && "ID:" == lastTextId.mid(0,3))
    {
        lastTextId = lastTextId.remove(0,3);
        lastTextId = "&last_id=" + lastTextId;
    }
    else
    {
        lastTextId = "";
    }

    QString httpTextReq = "http://" + mTarget + ":" + mPort +
                          "/radiodns/vis/vis.json?topic=/topic/"+ aTextTopic + lastTextId;

    QUrl httpTextUrlReq( httpTextReq );
    qDebug() << "[HTTP-TRANSPORT] Requesting Topic for text : " << httpTextReq;
    mTextReply = mNetworkManager.get( QNetworkRequest( httpTextUrlReq ) );

    QObject::connect
            (
            mTextReply,
            &QNetworkReply::finished,
            this,
            &HttpTransport::OnTextResponse
            );
}

void HttpTransport::RequestImageTopic( const QString& aImageTopic )
{
    if( mIsDiabled )
        return;

    // Construct a url for image topic
    QVariantMap::const_iterator iter = mImageResponse.find( RadioVISMessageID );

    QString lastImageId("");
    if( iter.value().canConvert(QMetaType::QString) && ( QVariant::String == iter.value().type() ) )
    {
        lastImageId = iter.value().toString(); // This is mandatory if not initial request;
    }
    else
    {
        qWarning() << ">> Http Image Timeout";
    }

    if( 0 != lastImageId.size() && "ID:" == lastImageId.mid(0,3) )
    {
        qDebug() << "lastImageId = " << lastImageId;
        lastImageId = lastImageId.remove(0,3);
        lastImageId = "&last_id=" + lastImageId;
    }
    else
    {
        lastImageId = "";
    }
    QString httpImageReq = "http://" + mTarget + ":" + mPort +
                           "/radiodns/vis/vis.json?topic=/topic/" + aImageTopic + lastImageId;

    QUrl httpImageUrlReq( httpImageReq );
    qDebug() << "[HTTP-TRANSPORT] Requesting Topic for image : " << httpImageReq;
    mImageReply = mNetworkManager.get( QNetworkRequest( httpImageUrlReq ) );
    QObject::connect
            (
            mImageReply,
            &QNetworkReply::finished,
            this,
            &HttpTransport::OnImageResponse
            );
}
