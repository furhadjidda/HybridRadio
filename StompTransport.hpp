#ifndef STOMPTRANSPORT_HPP
#define STOMPTRANSPORT_HPP

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QAbstractSocket>
#include "Transport.hpp"
#include <QStomp/qstomp.h>

class StompTransport : public Transport
{
    Q_OBJECT
public:
    StompTransport();

    void SetPortAndTarget
        (
        const QString& aPort,
        const QString& aTarget
        ) override;

    void ResetTransportResponses() override
    {
    }

    void SubscribeTextTopic( const QString& aTextTopic ) override;

    void SubscribeImageTopic( const QString& aImageTopic ) override;

    void UnSubscribeTextTopic( const QString& aTextTopic ) override;

    void UnSubscribeImageTopic( const QString& aImageTopic ) override;

    void DisableTransport() override
    {
        mIsDiabled = true;
    }

    void EnableTransport() override
    {
        mIsDiabled = false;
    }

public slots:
    void OnTextResponse() override;
    void OnImageResponse() override;

    void OnStompError( QAbstractSocket::SocketError error );
    void OnConnected();
    void OnDisConnected();
    void OnFrameReceived();
    void OnSocketStateChanged( QAbstractSocket::SocketState aState);

signals:
    void SignalTextChanged( const QString& aText ) override;
    void SignalImageChanged( const QString& aImage ) override;
    void SignalStompConnectionReady();

private:
    QWebSocket* m_webSocket;
    QStompClient mStompClient;
};

#endif // STOMPTRANSPORT_HPP
