#ifndef HTTPTRANSPORT_HPP
#define HTTPTRANSPORT_HPP

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

    void RequestText( const QString& aTextTopic ) override;

    void RequestImage( const QString& aImageTopic ) override;

public slots:
    void OnTextResponse() override;
    void OnImageResponse() override;

signals:
    void SignalTextChanged( const QString& aText ) override;
    void SignalImageChanged( const QString& aImage ) override;
};

#endif // HTTPTRANSPORT_HPP
