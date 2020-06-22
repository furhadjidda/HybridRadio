#include "StompTransport.hpp"
#include <QUrl>
#include <QNetworkRequest>
#include <QStomp/qstomp.h>

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

void StompTransport::RequestText( const QString& aTextTopic )
{
    // TODO Clean this
    qDebug() << "\n[STOMP-TRANSPORT] RequestText " << aTextTopic;
    QString textTopic = "/topic/" + aTextTopic;
    QPair<QByteArray, QByteArray> receipt = {"receipt","newIdHybrid"};
    QPair<QByteArray, QByteArray> id = {"id","testHybridId1"};
    QStompHeaderList headers;
    headers.push_back(receipt);
    headers.push_back(id);
    mStompClient.subscribe( textTopic.toUtf8(), true, headers );
}

void StompTransport::RequestImage( const QString& aImageTopic )
{
    // TODO Clean this
    qDebug() << "\n[STOMP-TRANSPORT] RequestImage\n";
    QString imageTopic = "/topic/" + aImageTopic;
    QPair<QByteArray, QByteArray> receipt = {"receipt","newIdHybrid"};
    QPair<QByteArray, QByteArray> id = {"id","testHybridId2"};
    QStompHeaderList headers;
    headers.push_back(receipt);
    headers.push_back(id);
    mStompClient.subscribe( imageTopic.toUtf8(), true, headers );
}

void StompTransport::OnTextResponse()
{
}

void StompTransport::OnImageResponse()
{
}

void StompTransport::OnStompError( QAbstractSocket::SocketError error )
{
    qDebug() << "\n[STOMP-TRANSPORT] error = " << error;
}
void StompTransport::OnConnected()
{
    qDebug() << "\n[STOMP-TRANSPORT] connected !! ";
    mStompClient.login();
    emit SignalStompConnectionReady();
}
void StompTransport::OnDisConnected()
{
    qDebug() << "\n[STOMP-TRANSPORT] disconnected !! ";
}
void StompTransport::OnFrameReceived()
{
    qDebug() << "\n[STOMP-TRANSPORT] Frame Received ";
    QList<QStompResponseFrame> frames = mStompClient.fetchAllFrames();
    qDebug() << "\n [STOMP-TRANSPORT] Number of Frames Received = " << frames.size();
    for( QStompResponseFrame frame:frames )
    {
        QByteArray data = frame.toByteArray();
        qDebug() << "\n[STOMP-TRANSPORT] data = " << QString(data);
    }

    // TODO : Parse the data here to send to QMlSignalHandler

}
void StompTransport::OnSocketStateChanged( QAbstractSocket::SocketState aState)
{
    qDebug() << "\n[STOMP-TRANSPORT] State = " << aState;
}
