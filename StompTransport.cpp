#include "StompTransport.hpp"
#include <QUrl>
#include <QNetworkRequest>
#include <QStomp/qstomp.h>
#include <QThread>
#include <QTcpSocket>

const static QString TopicPrefix("/topic/");
const static QPair<QByteArray, QByteArray> ReceiptHeaderText = {"receipt","1-1-text"};
const static QPair<QByteArray, QByteArray> ReceiptHeaderImage = {"receipt","1-1-image"};
QPair<QByteArray, QByteArray> IdHeaderText = {"id","id-text"};
QPair<QByteArray, QByteArray> IdHeaderImage = {"id","id-image"};

StompTransport::StompTransport()
{
    mStompClient = new QStompClient();
    mIsDiabled = true;
}

StompTransport::~StompTransport()
{
    delete mStompClient;
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
        qWarning() << "[STOMP-TRANSPORT] Nothing to be done Same Target";
        return;
    }

    // For some reason everytime I disconnected the client and connected it back again it created another socket
    // which was causing a lot of issues where multiple socket would be created and multiple subscriptions could happen
    // on a socket to which we would not have a reference to. In order to avoid that issue I am deleting the stompclient
    // and creating it back again explicitly. This does not cause multiple sockets and thus multiple connections to be
    // created.
    if( nullptr != mStompClient )
    {
        delete mStompClient;
        mStompClient = new QStompClient();
    }

    mPort = aPort;
    mTarget = aTarget;

    mStompClient->connectToHost( mTarget, mPort.toUShort() );
    QObject::connect(mStompClient,SIGNAL(socketError(QAbstractSocket::SocketError )),
                     this, SLOT(OnStompError( QAbstractSocket::SocketError  )));

    QObject::connect(mStompClient,SIGNAL(socketConnected()),
                     this, SLOT(OnConnected()));

    QObject::connect(mStompClient,SIGNAL(socketDisconnected()),
                     this, SLOT(OnDisConnected()));

    QObject::connect(mStompClient,SIGNAL(socketStateChanged(QAbstractSocket::SocketState)),
                     this, SLOT(OnSocketStateChanged(QAbstractSocket::SocketState)));

    QObject::connect(mStompClient,SIGNAL(frameReceived()),
                     this, SLOT(OnFrameReceived()));
}

void StompTransport::SubscribeTextTopic( const QString& aTextTopic )
{
    if( mIsDiabled )
        return;

    mCurrentTextTopic.clear();
    mCurrentTextTopic = TopicPrefix + aTextTopic;
    QStompHeaderList headers;
    headers.push_back( ReceiptHeaderText );
    headers.push_back( IdHeaderText );
    qDebug() << "[STOMP-TRANSPORT] Subscribing to " << mCurrentTextTopic.toUtf8();
    mStompClient->subscribe( mCurrentTextTopic.toUtf8(), true, headers );
}

void StompTransport::SubscribeImageTopic( const QString& aImageTopic )
{
    if( mIsDiabled )
        return;

    mCurrentImageTopic.clear();
    mCurrentImageTopic = TopicPrefix + aImageTopic;
    QStompHeaderList headers;
    headers.push_back( ReceiptHeaderImage );
    headers.push_back( IdHeaderImage );
    qDebug() << "[STOMP-TRANSPORT] Subscribing to " << mCurrentImageTopic.toUtf8();
    mStompClient->subscribe( mCurrentImageTopic.toUtf8(), true, headers );
}

void StompTransport::UnSubscribeTextTopic( const QString& aTextTopic )
{
    if( mIsDiabled )
        return;

    QString textTopic = TopicPrefix + aTextTopic;
    QStompHeaderList headers;
    headers.push_back( IdHeaderText );
    qDebug() << "[STOMP-TRANSPORT] UnSubscribing from " << textTopic;
    mStompClient->unsubscribe( textTopic.toUtf8(), headers );
    mCurrentTextTopic.clear();
}

void StompTransport::UnSubscribeImageTopic( const QString& aImageTopic )
{
    if( mIsDiabled )
        return;

    QString imageTopic = TopicPrefix + aImageTopic;
    QStompHeaderList headers;
    headers.push_back( IdHeaderImage );
    qDebug() << "[STOMP-TRANSPORT] UnSubscribing from " << imageTopic;
    mStompClient->unsubscribe( imageTopic.toUtf8(), headers );
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
    qWarning() << "[STOMP-TRANSPORT] Error = " << error;
}

void StompTransport::OnConnected()
{
    qDebug() << "[STOMP-TRANSPORT] CONNECTED !! ";
    mStompClient->login();
    EnableTransport();
    // Do not emit connected signal from here because this is just socket connection and not connection to STOMP server
}

void StompTransport::OnDisConnected()
{
    DisableTransport();
    qWarning() << "[STOMP-TRANSPORT] DISCONNECTED !! ";
    emit SignalStompConnectionBroken();
}

void StompTransport::OnFrameReceived()
{
    QList<QStompResponseFrame> responseFrames = mStompClient->fetchAllFrames();
    QString frameDestination;

    for( QStompResponseFrame frame : responseFrames )
    {
        QByteArray data = frame.toByteArray();
        //qDebug() << "[STOMP-TRANSPORT] Data Received = " << data << endl << endl;

        if( QStompResponseFrame::ResponseError == frame.type() )
        {
            qDebug() << "[STOMP-TRANSPORT] Data Received = " << data << endl << endl;
            if( frame.hasMessage() )
            {
                qDebug() << "[STOMP-TRANSPORT] ResponseError message = " << frame.message();
            }
        }
        else if( QStompResponseFrame::ResponseReceipt == frame.type() )
        {
            if( frame.hasReceiptId() )
            {
                qDebug() << "[STOMP-TRANSPORT] ResponseReceipt receiptId = " << frame.receiptId();
            }
        }
        else if( QStompResponseFrame::ResponseConnected == frame.type() )
        {
            QString receivedFrame( data );
            QStringList parts = receivedFrame.split(QLatin1Char('\n'));
            for( QString part : parts )
            {
                qDebug() << "[STOMP-TRANSPORT] ResponseConnected " << part;
            }
            emit SignalStompConnectionReady();
        }
        else if( QStompResponseFrame::ResponseMessage == frame.type() )
        {
            QString receivedFrame( data );

            if( frame.hasDestination() )
            {
                frameDestination = frame.destination();
                qDebug() << "[STOMP-TRANSPORT] ResponseMessage Destination = " << frame.destination();
            }

            QStringList parts = receivedFrame.split(QLatin1Char('\n'));
            for( QString part : parts )
            {
                if( part.contains( "SHOW" ) && frameDestination == mCurrentImageTopic )
                {
                    // 5 is the size of "TEXT" + the space
                    // SHOW http://slideshow.musicradio.com/slides/GCap_Media/One_Network/Capital_Radio/NowPlaying_973027.jpg
                    QString ImagePath = part.right( part.size() - 5 );
                    qDebug() << "[STOMP-TRANSPORT] ResponseMessage[IMAGE] " << ImagePath;
                    emit SignalImageChanged( ImagePath );
                }
                else if( part.contains( "TEXT" ) && frameDestination == mCurrentTextTopic )
                {
                    // 5 is the size of "TEXT" + the space
                    // TEXT Now on Capital: Powfu Feat. Beabadoobee with coffee for your head
                    QString Text = part.right( part.size() - 5 );
                    qDebug() << "[STOMP-TRANSPORT] ResponseMessage[TEXT] " << Text;
                    emit SignalTextChanged( Text );
                }
                else if( part.contains( "link" ) && frameDestination == mCurrentImageTopic )
                {
                    QString link = part.right( part.size() - sizeof ("link:") );
                    qDebug() << "[STOMP-TRANSPORT] ResponseMessage[link] " << link;
                    emit SignalLinkInformation( link );
                }
                else if( part.contains( "SlideID" ) && frameDestination == mCurrentImageTopic )
                {
                    QString slideId = part.right( part.size() - sizeof ("SlideID:") );
                    qDebug() << "[STOMP-TRANSPORT] ResponseMessage[SlideID] " << slideId;
                    emit SignalSlideId( slideId );
                }
                else if( part.contains( "trigger-time" ) && frameDestination == mCurrentImageTopic )
                {
                    QString triggerTime = part.right( part.size() - sizeof ("trigger-time:") );
                    qDebug() << "[STOMP-TRANSPORT] ResponseMessage[triggerTime] " << triggerTime;
                    emit SignalSlideId( triggerTime );
                }
                else if( part.contains( "timestamp" ) )
                {
                    qDebug() << "[STOMP-TRANSPORT] ResponseMessage[timestamp] " << part;
                }
            }
        }
    }
}


void StompTransport::OnSocketStateChanged( QAbstractSocket::SocketState aState)
{
    qDebug() << "[STOMP-TRANSPORT] State = " << aState;
}


void StompTransport::SendStompFrame
    (
    const QString& aTopic,
    const QString& aReceipt,
    QStompRequestFrame::RequestType aType
    )
{
    QStompRequestFrame sendFrame;

    sendFrame.setType( aType );
    if( !aTopic.isEmpty() )
    {
        sendFrame.setDestination( aTopic.toUtf8() );
    }
    if( !aTopic.isEmpty() )
    {
        sendFrame.setReceiptId( aReceipt.toUtf8() );
    }
    mStompClient->sendFrame( sendFrame );
}
