#include "UiHandler.hpp"
#include <QQmlProperty>
#include <QDebug>

UiHandler::UiHandler
    (
    QObject* aUiObject
    )
    : mUiObject( aUiObject )
{

}

UiHandler::~UiHandler()
{

}
void UiHandler::SetStationNameValue( const QVariant& aValue )
{
    QObject *StationName = mUiObject->findChild<QObject*>("StationNameObj");

    if( StationName )
    {
        StationName->setProperty
                (
                "text",
                aValue
                );

        QQmlProperty
                (
                StationName,
                "text"
                )
                .write
                (
                aValue
                );
    }

}

void UiHandler::SetArtworkValue( const QVariant& aValue )
{
    QString artWorkString = aValue.toString();
    if( 0 == artWorkString.size() )
    {
        artWorkString = "default-art.jpg";
    }
    QObject *artWork = mUiObject->findChild<QObject*>("artWork");

    if( artWork )
    {
        artWork->setProperty
                (
                "source",
                artWorkString
                );

        QQmlProperty
                (
                artWork,
                "source"
                )
                .write
                (
                artWorkString
                );
    }

}

void UiHandler::SetStationDescriptionValue( const QVariant& aValue )
{
    QObject *description = mUiObject->findChild<QObject*>("DescriptionObject");

    if( description )
    {
        description->setProperty
                (
                "text",
                aValue
                );

        QQmlProperty
                (
                description,
                "text"
                )
                .write
                (
                aValue
                );
    }

}

void UiHandler::SetBitrateValue( const QVariant& aValue )
{
    QObject *bitRate = mUiObject->findChild<QObject*>("BitRateObject");

    if( bitRate )
    {
        QString bitRateValue("BitRate:");
        bitRateValue.append(aValue.toString());
        bitRate->setProperty
                (
                "text",
                QVariant(bitRateValue)
                );

        QQmlProperty
                (
                bitRate,
                "text"
                )
                .write
                (
                bitRateValue
                );
    }
}

void UiHandler::SetMoreInfoValue( const QVariant& aValue )
{
    QObject *moreInfo = mUiObject->findChild<QObject*>("additionalInfo");
    {
        moreInfo->setProperty
                (
                "text",
                aValue
                );

        QQmlProperty
                (
                moreInfo,
                "text"
                )
                .write
                (
                aValue
                );
    }
}

void UiHandler::SetMediaStatusValue( const QVariant& aValue )
{
    QObject *mediaStatus = mUiObject->findChild<QObject*>("MediaStatus");

    if( mediaStatus )
    {
        mediaStatus->setProperty
                (
                "text",
                aValue
                );

        QQmlProperty
                (
                mediaStatus,
                "text"
                )
                .write
                (
                aValue
                );
    }
}


void UiHandler::SetSongNameValue( const QVariant& aValue )
{
    QObject *songName = mUiObject->findChild<QObject*>("SongObject");
    if( songName )
    {
        songName->setProperty
                (
                "text",
                aValue
                );

        QQmlProperty
                (
                songName,
                "text"
                )
                .write
                (
                aValue
                );
    }
}

void UiHandler::QmlMethodInvokeclearListElement()
{
    QVariant retValue = 0;
    QObject *RectBoxObj = mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj,
                        "clearListElement",
                        Q_RETURN_ARG(QVariant, retValue)
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}

void UiHandler::QmlMethodInvokeaddListElement( const SiData& aValue )
{
    QVariant retValue = 0;
    QObject *RectBoxObj = mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj, "addListElement",
                        Q_RETURN_ARG(QVariant, retValue),
                        Q_ARG( QVariant, aValue.mServiceName ),
                        Q_ARG( QVariant, aValue.mGenre ),
                        Q_ARG( QVariant, aValue.mArtwork )
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}


void UiHandler::QmlMethodInvokeMethoddisplayPopUp()
{
    QVariant retValue=0;
    QObject *RectBoxObj= mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj, "displayPopUp",
                        Q_RETURN_ARG(QVariant, retValue)
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}


void UiHandler::QmlMethodInvokeMethodhidePopUp()
{
    QVariant retValue=0;
    QObject *RectBoxObj= mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj, "hidePopUp",
                        Q_RETURN_ARG(QVariant, retValue)
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}

void UiHandler::QmlMethodInvokeMethodDisplayEpgPresentImage()
{
    QVariant retValue=0;
    QObject *RectBoxObj= mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj, "displayEpgImage",
                        Q_RETURN_ARG(QVariant, retValue)
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}

void UiHandler::QmlMethodInvokeMethodDisplayHttpProtocolSupport( bool aStatus )
{
    QVariant retValue=0;
    QObject *RectBoxObj= mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj, "displayHttpProtocolStatus",
                        Q_RETURN_ARG(QVariant, retValue),
                        Q_ARG( QVariant, aStatus )
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}

void UiHandler::QmlMethodInvokeMethodDisplayStompProtocolSupport( bool aStatus )
{
    QVariant retValue=0;
    QObject *RectBoxObj= mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj, "displayStompProtocolStatus",
                        Q_RETURN_ARG(QVariant, retValue),
                        Q_ARG( QVariant, aStatus )
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}

void UiHandler::QmlMethodInvokeMethodHideEpgPresentImage()
{
    QVariant retValue=0;
    QObject *RectBoxObj= mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj, "hideEpgImage",
                        Q_RETURN_ARG(QVariant, retValue)
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}

void UiHandler::QmlMethodInvokeclearProgramElement()
{
    QVariant retValue = 0;
    QObject *RectBoxObj = mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj,
                        "clearProgramElementsElement",
                        Q_RETURN_ARG(QVariant, retValue)
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}


void UiHandler::QmlMethodInvokeaddProgramElement( const EpgStruct& aValue )
{
    QVariant retValue = 0;
    QObject *RectBoxObj = mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj, "addProgramElement",
                        Q_RETURN_ARG(QVariant, retValue),
                        Q_ARG( QVariant, aValue.mLongName ),
                        Q_ARG( QVariant, aValue.mArt ),
                        Q_ARG( QVariant, aValue.mTimeAndDate )
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}

void UiHandler::QmlMethodInvokeAddMoreInfo( const QVariant& aValue )
{
    QVariant retValue = 0;
    QObject *RectBoxObj = mUiObject->findChild<QObject*>("RectBox");
    bool succeeded = QMetaObject::invokeMethod
                        (
                        RectBoxObj, "addBearerInformation",
                        Q_RETURN_ARG(QVariant, retValue),
                        Q_ARG( QVariant, aValue )
                        );

    if(!succeeded)
    {
        qWarning() << "Invokation Failed";
    }
}

void UiHandler::UpdatePresetBox( const QString aIndex, const SiData& aValue )
{
    QString id("presetid");
    id.append( aIndex );

    QObject *prsesetImage = mUiObject->findChild<QObject*>(id);
    qDebug() << "aValue.mArtwork = " << aValue.mArtwork;
    if( prsesetImage )
    {
        prsesetImage->setProperty
                (
                "source",
                aValue.mArtwork
                );

        QQmlProperty
                (
                prsesetImage,
                "source"
                )
                .write
                (
                aValue.mArtwork
                );
    }
}


