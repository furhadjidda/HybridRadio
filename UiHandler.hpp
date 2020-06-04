#ifndef UIHANDLER_HPP
#define UIHANDLER_HPP

#include <QVariant>
#include "HybridRadioCommonTypes.hpp"

class UiHandler
         : public QObject
{
    Q_OBJECT
public:
    UiHandler( QObject* aUiObject );
    ~UiHandler();
    void SetStationNameValue( const QVariant& aValue );
    void SetArtworkValue( const QVariant& aValue );
    void SetStationDescriptionValue( const QVariant& aValue );
    void SetBitrateValue( const QVariant& aValue );
    void SetMoreInfoValue( const QVariant& aValue );
    void SetMediaStatusValue( const QVariant& aValue );
    void SetSongNameValue( const QVariant& aValue );

    void QmlMethodInvokeclearListElement();
    void QmlMethodInvokeaddListElement( const SiData& aValue );
    void QmlMethodInvokeMethoddisplayPopUp();
    void QmlMethodInvokeMethodhidePopUp();
    void QmlMethodInvokeMethodDisplayEpgPresentImage();
    void QmlMethodInvokeMethodHideEpgPresentImage();

    void QmlMethodInvokeclearProgramElement();
    void QmlMethodInvokeaddProgramElement( const EpgStruct& aValue );


private:
    QObject* mUiObject;

};

#endif // UIHANDLER_HPP
