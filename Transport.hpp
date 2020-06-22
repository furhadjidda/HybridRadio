#ifndef TRANSPORT_HPP
#define TRANSPORT_HPP

#include <QString>
#include <QObject>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Transport : public QObject{
Q_OBJECT
public:
    Transport();

    virtual void SetPortAndTarget
        (
        const QString& aPort,
        const QString& aTarget
        ) = 0;

    virtual void ResetTransportResponses() = 0;

    virtual void SubscribeTextTopic( const QString& aTextTopic ) = 0;

    virtual void SubscribeImageTopic( const QString& aImageTopic ) = 0;

    virtual void UnSubscribeTextTopic( const QString& aTextTopic ) = 0;

    virtual void UnSubscribeImageTopic( const QString& aImageTopic ) = 0;

signals:
    virtual void SignalTextChanged( const QString& aText ) = 0;
    virtual void SignalImageChanged( const QString& aImage ) = 0;

public slots:
    virtual void OnTextResponse() = 0;
    virtual void OnImageResponse() = 0;

protected:
    QString mCurrentTextTopic;
    QString mCurrentImageTopic;
    QString mPort;
    QString mTarget;

};

#endif // TRANSPORT_HPP
