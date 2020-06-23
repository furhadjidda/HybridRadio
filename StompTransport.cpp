#include "StompTransport.hpp"
#include <QUrl>
#include <QNetworkRequest>
#include <QStomp/qstomp.h>

const static QString TopicPrefix("/topic/");
const static QPair<QByteArray, QByteArray> ReceiptHeader = {"receipt","hybrid-radio-receipt-2020"};
QPair<QByteArray, QByteArray> IdHeader = {"id","hybrid-radio-id-2020"};

StompTransport::StompTransport()
    : m_webSocket( nullptr )
{
    m_webSocket = new QWebSocket();
}


void StompTransport::SetPortAndTarget
    (
    const QString& aPort,
    const QString& aTarget
    )
{
    qDebug() << "[STOMP-TRANSPORT] Port = " << aPort << " Target = " << aTarget;
    if( mTarget == aTarget )
    {
        return;
    }

    mPort = aPort;
    mTarget = aTarget;
    qDebug() << "[STOMP-TRANSPORT] Port = " << mPort << " Target = " << mTarget;
    mStompClient.connectToHost( mTarget );
    QObject::connect(&mStompClient,SIGNAL(socketError(QAbstractSocket::SocketError )),
                     this, SLOT(OnStompError( QAbstractSocket::SocketError  )));

    QObject::connect(&mStompClient,SIGNAL(socketConnected()),
                     this, SLOT(OnConnected()));

    QObject::connect(&mStompClient,SIGNAL(socketDisconnected()),
                     this, SLOT(OnDisConnected()));

    QObject::connect(&mStompClient,SIGNAL(socketStateChanged(QAbstractSocket::SocketState)),
                     this, SLOT(OnSocketStateChanged(QAbstractSocket::SocketState)));

    QObject::connect(&mStompClient,SIGNAL(frameReceived()),
                     this, SLOT(OnFrameReceived()));
}

void StompTransport::SubscribeTextTopic( const QString& aTextTopic )
{
    QString textTopic = TopicPrefix + aTextTopic;
    QStompHeaderList headers;
    headers.push_back( ReceiptHeader );
    headers.push_back( IdHeader );
    qDebug() << "[STOMP-TRANSPORT] Subscribing to " << textTopic;
    mStompClient.subscribe( textTopic.toUtf8(), true, headers );
}

void StompTransport::SubscribeImageTopic( const QString& aImageTopic )
{
    QString imageTopic = TopicPrefix + aImageTopic;
    QStompHeaderList headers;
    headers.push_back( ReceiptHeader );
    headers.push_back( IdHeader );
    qDebug() << "[STOMP-TRANSPORT] Subscribing to " << imageTopic;
    mStompClient.subscribe( imageTopic.toUtf8(), true, headers );
}

void StompTransport::UnSubscribeTextTopic( const QString& aTextTopic )
{
    QString textTopic = TopicPrefix + aTextTopic;
    QStompHeaderList headers;
    qDebug() << "[STOMP-TRANSPORT] UnSubscribing from " << textTopic;
    mStompClient.unsubscribe( textTopic.toUtf8(), headers );
    mCurrentTextTopic.clear();
}

void StompTransport::UnSubscribeImageTopic( const QString& aImageTopic )
{
    QString imageTopic = TopicPrefix + aImageTopic;
    QStompHeaderList headers;
    qDebug() << "[STOMP-TRANSPORT] UnSubscribing from " << imageTopic;
    mStompClient.unsubscribe( imageTopic.toUtf8(), headers );
    mCurrentImageTopic.clear();
}

void StompTransport::OnTextResponse()
{
}

void StompTransport::OnImageResponse()
{
}

void StompTransport::OnStompError( QAbstractSocket::SocketError error )
{
    qDebug() << "[STOMP-TRANSPORT] error = " << error;
}

void StompTransport::OnConnected()
{
    qDebug() << "[STOMP-TRANSPORT] CONNECTED !! ";
    mStompClient.login();
    emit SignalStompConnectionReady();
}

void StompTransport::OnDisConnected()
{
    qDebug() << "[STOMP-TRANSPORT] DISCONNECTED !! ";
}

void StompTransport::OnFrameReceived()
{
    QList<QStompResponseFrame> responseFrames = mStompClient.fetchAllFrames();
    qDebug() << "[STOMP-TRANSPORT] Number of Frames Received = " << responseFrames.size();
    for( QStompResponseFrame frame:responseFrames )
    {
        QByteArray data = frame.toByteArray();
        qDebug() << "[STOMP-TRANSPORT] Data Received = " << data;

        QString receivedFrame( data );
        QStringList parts = receivedFrame.split(QLatin1Char('\n'));
        for( QString part : parts )
        {
            if( part.contains( "SHOW" ) )
            {
                // 5 is the size of "TEXT" + the space
                // SHOW http://slideshow.musicradio.com/slides/GCap_Media/One_Network/Capital_Radio/NowPlaying_973027.jpg
                QString ImagePath = part.right( part.size() - 5 );
                emit SignalImageChanged( ImagePath );
            }
            else if( part.contains( "TEXT" ) )
            {
                // 5 is the size of "TEXT" + the space
                // TEXT Now on Capital: Powfu Feat. Beabadoobee with coffee for your head
                QString Text = part.right( part.size() - 5 );
                emit SignalTextChanged( Text );
            }
        }

        qDebug() << "[STOMP-TRANSPORT] Response Type = " << frame.type();

        if( frame.hasMessage() )
        {
            qDebug() << "[STOMP-TRANSPORT] Response message = " << frame.message();
        }

        if( frame.hasDestination() )
        {
            qDebug() << "[STOMP-TRANSPORT] Response Destination = " << frame.destination();
        }

        if( frame.hasSubscriptionId() )
        {
            qDebug() << "[STOMP-TRANSPORT] Response subscriptionId = " << frame.subscriptionId();
        }

        if( frame.hasMessageId() )
        {
            qDebug() << "[STOMP-TRANSPORT] Response messageId = " << frame.messageId();
        }

        if( frame.hasReceiptId() )
        {
            qDebug() << "[STOMP-TRANSPORT] Response receiptId = " << frame.receiptId();
        }

    }

    // TODO : Parse the data here to send to QMlSignalHandler

}
void StompTransport::OnSocketStateChanged( QAbstractSocket::SocketState aState)
{
    qDebug() << "[STOMP-TRANSPORT] State = " << aState;
}
