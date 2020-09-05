#ifndef STOMPTRANSPORT_HPP
#define STOMPTRANSPORT_HPP

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QAbstractSocket>
#include "Transport.hpp"
#include <QStomp/qstomp.h>
#include <tuple>

class StompTransport : public Transport
{
    Q_OBJECT
public:
    StompTransport();
    virtual ~StompTransport();

    void SetPortAndTarget
        (
        const QString& aPort,
        const QString& aTarget
        ) override;

    void ResetTransport() override
    {
        mTarget.clear();
        DisableTransport();
    }

    void SubscribeTextTopic( const QString& aTextTopic ) override;

    void SubscribeImageTopic( const QString& aImageTopic ) override;

    void UnSubscribeTextTopic( const QString& aTextTopic ) override;

    void UnSubscribeImageTopic( const QString& aImageTopic ) override;

    void DisableTransport() override
    {
        qWarning() << "[STOMP-TRANSPORT] Disabling the Transport";
        mIsDiabled = true;

        QStompRequestFrame disconnectFrame;

        disconnectFrame.setType( QStompRequestFrame::RequestDisconnect );
        disconnectFrame.setReceiptId("hybrid-radio-id-disconnect");
        mStompClient->sendFrame( disconnectFrame );

        mStompClient->disconnectFromHost();
    }

    void EnableTransport() override
    {
        qWarning() << "[STOMP-TRANSPORT] Enabling the Transport";
        mIsDiabled = false;
    }

    bool IsSupported() override
    {
        return mIsSupported;
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
    void SignalStompConnectionReady();
    void SignalStompConnectionBroken();

private:
    void SendStompFrame
        (
        const QString& aTopic,
        const QString& aReceipt,
        QStompRequestFrame::RequestType aType
        );
    QStompClient* mStompClient{nullptr};

    // tuple = RequestType , topic, receipt-id
    QList<std::tuple<QStompRequestFrame::RequestType, QByteArray, QByteArray> > mPendingResponse;
};

#endif // STOMPTRANSPORT_HPP
