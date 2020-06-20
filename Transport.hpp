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

    virtual void RequestText( const QString& aTextTopic ) = 0;

    virtual void RequestImage( const QString& aImageTopic ) = 0;

signals:
    virtual void SignalTextChanged( const QString& aText ) = 0;
    virtual void SignalImageChanged( const QString& aImage ) = 0;

public slots:
    virtual void OnTextResponse() = 0;
    virtual void OnImageResponse() = 0;

protected:
    QVariantMap mTextResponse;
    QVariantMap mImageResponse;
    QNetworkAccessManager mNetworkManager;
    QString mPort;
    QString mTarget;
    QNetworkReply* mTextReply;
    QNetworkReply* mImageReply;

};

#endif // TRANSPORT_HPP
