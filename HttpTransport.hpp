#ifndef HTTPTRANSPORT_HPP
#define HTTPTRANSPORT_HPP

#include <QTimer>
#include "Transport.hpp"

class HttpTransport : public Transport
{
    Q_OBJECT
public:
    HttpTransport();

    void SetPortAndTarget
        (
        const QString& aPort,
        const QString& aTarget
        ) override;

    void ResetTransportResponses() override
    {
        mImageResponse.clear();
        mTextResponse.clear();
    }

    void SubscribeTextTopic( const QString& aTextTopic ) override;

    void SubscribeImageTopic( const QString& aImageTopic ) override;


    void UnSubscribeTextTopic( const QString& aTextTopic ) override
    {
        qDebug() << "[HTTP-TRANSPORT] Unsubscribe from Topic" << aTextTopic;
        mCurrentTextTopic.clear();
        mTextTopicTimer->stop();
    };

    void UnSubscribeImageTopic( const QString& aImageTopic ) override
    {
        qDebug() << "[HTTP-TRANSPORT] Unsubscribe from Topic" << aImageTopic;
        mCurrentImageTopic.clear();
        mImageTopicTimer->stop();
    }

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
    void OnTextTopicTimeExpired();
    void OnImageTopicTimeExpired();

signals:
    void SignalTextChanged( const QString& aText ) override;
    void SignalImageChanged( const QString& aImage ) override;

private:
    void RequestTextTopic( const QString& aTextTopic );
    void RequestImageTopic( const QString& aImageTopic );
    QVariantMap mTextResponse;
    QVariantMap mImageResponse;
    QNetworkReply* mTextReply;
    QNetworkReply* mImageReply;
    QTimer* mTextTopicTimer;
    QTimer* mImageTopicTimer;
    QNetworkAccessManager mNetworkManager;
};

#endif // HTTPTRANSPORT_HPP
