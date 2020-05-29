#include "lookuphelper.hpp"
#include <QTextStream>
#include <QDebug>
#include <QStringList>

static const QString FmBand("fm");
static const QString DabBand("dab");
static const QString HdBand("hd");

static const QString RadioDnsPostFix("radiodns.org");

void ConstructFqdn
    (
    const StationInformation& aStationInfo,
    const QString& aGcc,
    QString& returnValue
    )
    {
        if( FmBand == aStationInfo.mBand )
        {
            // <frequency>.<pi>.<gcc>.fm.radiodns.org
            returnValue = aStationInfo.mFrequency + "."
                   + QString::number( aStationInfo.mPi, 16 ) + "."
                   + aGcc + "."
                   + aStationInfo.mBand + "."
                   + RadioDnsPostFix;
            qDebug() << "FQDN = " << returnValue;
            return;
        }
        else if( DabBand == aStationInfo.mBand )
        {
            // [<uatype>.]<scids>.<sid>.<eid>.<gcc>.dab.radiodns.org
            returnValue = QString::number( aStationInfo.mScids, 16 ) + "."
                   + QString::number( aStationInfo.mSid, 16 ) + "."
                   + QString::number( aStationInfo.mEid, 16 ) + "."
                   + aGcc + "."
                   + aStationInfo.mBand + "."
                   + RadioDnsPostFix;
            qDebug() << "FQDN = " << returnValue;
            return;
        }
        else if( HdBand == aStationInfo.mBand )
        {
            //[<mId>.]<tx>.<cc>.hd.radiodns.org
            returnValue = QString::number( aStationInfo.mIbocHdId, 10 ) + "."
                   + QString::number( aStationInfo.mIbocTxId, 10 ) + "."
                   + aStationInfo.mIbocCountryCode + "."
                   + aStationInfo.mBand + "."
                   + RadioDnsPostFix;
            return;
        }
        else
        {
            returnValue = "Invalid Data";
        }
    }

void ConstructServiceIdentifier
    (
    const StationInformation& aStationInfo,
    const QString& aGcc,
    QString& returnValue
    )
    {
        if( FmBand == aStationInfo.mBand )
        {
            // fm/<gcc>/<pi>/<frequency>
            returnValue = aStationInfo.mBand + "/"
                   + aGcc + "/"
                   + QString::number( aStationInfo.mPi, 16 ) + "/"
                   + aStationInfo.mFrequency;
            return;
        }
        else if( DabBand == aStationInfo.mBand )
        {
            // dab/<gcc>/<eid>/<sid>/<scids>[/<uatype>]
            returnValue = aStationInfo.mBand + "/"
                   + aGcc + "/"
                   + QString::number( aStationInfo.mEid, 16 ) + "/"
                   + QString::number( aStationInfo.mSid, 16 ) + "/"
                   + QString::number( aStationInfo.mScids, 16 );
            return;

        }
        else if( HdBand == aStationInfo.mBand )
        {
            // hd/<cc>/<tx>[/<mId>]
            returnValue = aStationInfo.mBand + "/"
                   + aStationInfo.mIbocCountryCode + "/"
                   + QString::number( aStationInfo.mIbocTxId, 10 ) + "/"
                   + QString::number( aStationInfo.mIbocHdId, 10 );
            return;

        }
        else
        {

        }
    }

void ConstructBearerUri
    (
    const StationInformation& aStationInfo,
    const QString& aGcc,
    QString& returnValue
    )
    {
        if( FmBand == aStationInfo.mBand )
        {
            // fm:<gcc>.<pi>.<frequency>
            returnValue = aStationInfo.mBand + ":"
                   + aGcc + "."
                   + QString::number( aStationInfo.mPi, 16 ) + "."
                   + aStationInfo.mFrequency;
            qDebug() << "Bearer URI = " << returnValue;
            return;

        }
        else if( DabBand == aStationInfo.mBand )
        {
            // dab:<gcc>.<eid>.<sid>.<scids>[.<uatype>]
            returnValue = aStationInfo.mBand + ":"
                   + aGcc + "."
                   + QString::number( aStationInfo.mEid, 16 ) + "."
                   + QString::number( aStationInfo.mSid, 16 ) + "."
                   + QString::number( aStationInfo.mScids, 16 );
            qDebug() << "Bearer URI = " << returnValue;
            return;

        }
        else if( HdBand == aStationInfo.mBand )
        {
            // hd:<cc>.<tx>[.<mId>]
            returnValue = aStationInfo.mBand + ":"
                   + aStationInfo.mIbocCountryCode + "."
                   + QString::number( aStationInfo.mIbocTxId, 10 ) + "."
                   + QString::number( aStationInfo.mIbocHdId, 10 );
            return;

        }
        else
        {

        }
    }

void ConstructTopic
    (
    const StationInformation& aStationInfo,
    const QString& aGcc,
    const QString& aTopic,
    QString& returnValue
    )
{
    ConstructServiceIdentifier( aStationInfo, aGcc, returnValue );
    returnValue += "/" + aTopic;
}


void DeconstructBearer
    (
    StationInformation& aStationInfo,
    QString& aGcc,
    const QString& bearer
    )
{
    QRegExp rx("(\\.|\\:)"); //RegEx for ' ' or ',' or '.' or ':' or '\t'
    QStringList  query= bearer.split(rx);
    aStationInfo.mBand = query.at(0);

    if( aStationInfo.mBand == "fm" )
    {
        bool ok;
        int lastIndex = query.size() - 1;
        aStationInfo.mFrequency = query.at(lastIndex);
        aStationInfo.mPi = query.at(--lastIndex).toUInt(&ok,16);
        aGcc = query.at(--lastIndex);
    }
    else if( aStationInfo.mBand == "dab" )
    {
        bool ok;
        int lastIndex = query.size() - 1;
        aStationInfo.mScids = query.at(lastIndex).toUInt(&ok,16);
        aStationInfo.mSid = query.at(--lastIndex).toUInt(&ok,16);
        aStationInfo.mEid = query.at(--lastIndex).toUInt(&ok,16);
        aGcc = query.at(--lastIndex);
    }

    qDebug() << "@@ Splitting Data mBand " << aStationInfo.mBand;
    qDebug() << "@@ Splitting Data mPi " << aStationInfo.mPi;
    qDebug() << "@@ Splitting Data mGcc " << aGcc;
    qDebug() << "@@ Splitting Data mScids " << hex << aStationInfo.mScids;
    qDebug() << "@@ Splitting Data mSid " << hex << aStationInfo.mSid;
    qDebug() << "@@ Splitting Data mEid " << hex << aStationInfo.mEid;
}