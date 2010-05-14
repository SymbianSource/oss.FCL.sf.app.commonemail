/*
 * Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  mailbox branding object
 *
 */

#include "emailmailboxinfo_p.h"
#include "nmutilitiescommonheaders.h"
#include "nmcenrepkeys.h"
#include <QRegExp>
#include <QStringList>

const unsigned long int partialKey = 0x0;
const unsigned long int bitMask = 0x0F000000;

EmailMailboxInfoPrivate* EmailMailboxInfoPrivate::mSelf = NULL;
qint32 EmailMailboxInfoPrivate::mReferenceCount = 0;

const QString KBrandNameGmail("Gmail");
const QString KBrandNameGoogleMail("Google Mail");

const QString KMCCGermany("262");
const QString KMCCUK1("234");
const QString KMCCUK2("235");

const quint8 KGermanyTzId = 36;
const quint8 KUKTzId = 104;

using namespace NmBrandingApi;
using namespace QtMobility;

/*!
     private constructor
 */
EmailMailboxInfoPrivate::EmailMailboxInfoPrivate() :
    QObject(NULL), 
	mIsResourceLoaded(false)
{
    XQSettingsManager manager;
    XQCentralRepositorySettingsKey rccKey(EMAIL_CENREP, RCC_PATH);

    XQCentralRepositorySettingsKey wlbKey(EMAIL_CENREP, WLB_BRAND_NAME);

    mWlbDomainName = manager.readItemValue(wlbKey, XQSettingsManager::TypeString).value<QString> ();

    QString pathToRcc =
        manager.readItemValue(rccKey, XQSettingsManager::TypeString).value<QString> ();
    if (!mIsResourceLoaded) {
        mIsResourceLoaded = QResource::registerResource(pathToRcc);
    }
}
/*!
     private destructor
 */
EmailMailboxInfoPrivate::~EmailMailboxInfoPrivate()
{

}

/*!
    gets instance of EmailMailboxInfoPrivate
    \return instance of EmailMailboxInfoPrivate
 */
EmailMailboxInfoPrivate* EmailMailboxInfoPrivate::getInstance()
{
    if (!mSelf) {
        mSelf = new EmailMailboxInfoPrivate();
    }
    mReferenceCount++;
    return mSelf;
}

/*!
    releases pointer to instance of EmailMailboxInfoPrivate
    \param pointer to instance of EmailMailboxInfoPrivate object
 */
void EmailMailboxInfoPrivate::releaseInstance(EmailMailboxInfoPrivate *&instance)
{
    if (instance) {
        mReferenceCount--;
        instance = NULL;
    }
    if (mReferenceCount < 1) {
        delete mSelf;
        mSelf = NULL;
    }
}

/*!
    returns branding name of given domain
    \param branding identifier
    \return branding name
 */
QString EmailMailboxInfoPrivate::name(const QVariant &identifier)
{
    QString returnValue = "";
    QString domainName = "";
    if (identifier.canConvert<QString> ()) {
        domainName = identifier.value<QString> ();
    }

    if (domainName.length() > 0) {
        processCenRepRecords(domainName);
        returnValue = mTempName;
    }

    verifyMailAccountName(returnValue);

    return returnValue;
}

/*!
    returns branding icon of given domain
    \param branding identifier
    \return branding icon
 */
QString EmailMailboxInfoPrivate::icon(const QVariant &identifier)
{
    QString returnValue = "";
    QString domainName = "";
    if (identifier.canConvert<QString> ()) {
        domainName = identifier.value<QString> ();
    }

    if (domainName.length() > 0){
        processCenRepRecords(domainName);
        returnValue = mTempIcon;
    }

    return returnValue;
}

/*!
    goes through cenrep to find matching branding details
    \param brandingId (i.e. domain name)
 */
void EmailMailboxInfoPrivate::processCenRepRecords(const QString &brandingId)
{
    bool found = false;
    QString name;
    QString icon;

    XQSettingsManager cenRepManager;

    if (brandingId == mWlbDomainName) {
        found = true;
        XQCentralRepositorySettingsKey wlbNameKey(EMAIL_CENREP, WLB_MAILBOX_NAME);
        XQCentralRepositorySettingsKey wlbIconKey(EMAIL_CENREP, WLB_ICON_PATH);

        icon = cenRepManager.readItemValue(wlbIconKey, XQSettingsManager::TypeString).value<QString> ();
        name = cenRepManager.readItemValue(wlbNameKey, XQSettingsManager::TypeString).value<QString> ();
    }
    else {
        XQCentralRepositorySearchCriteria sCriteria(EMAIL_CENREP, partialKey, bitMask);
        XQCentralRepositoryUtils cenrepUtils(cenRepManager);
        QList<XQCentralRepositorySettingsKey> foundKeys = cenrepUtils.findKeys(sCriteria);

        foreach(XQCentralRepositorySettingsKey key, foundKeys)
        {
            QString dataRow = "";
            if (mBrandingDataMap.contains(key.key())) {
                dataRow = mBrandingDataMap.value(key.key());
            }
            //first we put every spotted data row to map
            else {
                QVariant brandingDataRaw = cenRepManager.readItemValue(key,
					XQSettingsManager::TypeString);
                if (brandingDataRaw.canConvert<QString>()) {
                    dataRow = brandingDataRaw.value<QString>();
                    mBrandingDataMap.insert(key.key(), dataRow);
                }
            }

            // then we check if this row contains matching data
            QStringList cenRepRecord = dataRow.split(";");

            if (cenRepRecord.size() < 4) {
                continue;
            }

            if (!brandingId.contains(cenRepRecord.at(0), Qt::CaseInsensitive)) {
                continue;
            }

            QRegExp regExp(cenRepRecord.at(1));
            regExp.setCaseSensitivity(Qt::CaseInsensitive);

            if (regExp.exactMatch(brandingId)) { //match
                found = true;
                icon = ":/" + cenRepRecord.at(3);
                name = cenRepRecord.at(2);
                break;
            }
        }
    }
    if (!found || !mIsResourceLoaded) {
        //get default icon and name
        icon = "qtg_large_email";
        QStringList domain = brandingId.split(".");
        if (domain.size() > 0) {
            name = domain.at(0);
        }
    }
    mTempIcon = icon;
    mTempName = name;
}

/*!
    gets current country code
    \return current country id
 */
quint8 EmailMailboxInfoPrivate::getCurrentCountryL() const
{
    CTzLocalizer* localizer = CTzLocalizer::NewLC();

    CTzLocalizedCity* city = localizer->GetFrequentlyUsedZoneCityL(
        CTzLocalizedTimeZone::ECurrentZone);
    CleanupStack::PushL(city);

    CTzLocalizedCityGroup* cityGroup = localizer->GetCityGroupL(city->GroupId());

    TUint8 countryId = cityGroup->Id();

    delete cityGroup;
    cityGroup = NULL;

    CleanupStack::PopAndDestroy(2, localizer);

    return countryId;
}

/*!
    verifies if timezone is set for Germany or UK
    \return true if timezone is UK or Germany, false otherwise
 */
bool EmailMailboxInfoPrivate::verifyTimeZone() const
{
    quint8 timeZone = 0;
    bool retVal = false;
    TRAPD(err, timeZone = getCurrentCountryL());

    if (err == KErrNone && (timeZone == KGermanyTzId || timeZone == KUKTzId)) {
        retVal = true;
    }
    return retVal;
}

/*!
    modifies branding name "Gmail" according to UK and Germany law rules
    \param brandingName name of branding
 */
void EmailMailboxInfoPrivate::verifyMailAccountName(QString &brandingName) const
{
    QSystemNetworkInfo *networkInfo = new QSystemNetworkInfo();
    QString currentMCC = networkInfo->currentMobileCountryCode();

    if (brandingName == KBrandNameGmail) {
        if (currentMCC.size() > 0) {
            if ((currentMCC == KMCCGermany) || (currentMCC == KMCCUK1) || (currentMCC == KMCCUK2)) {
                brandingName = KBrandNameGoogleMail;
            }
        }
        else { //if there is information (no sim)
            if (verifyTimeZone()) {
                brandingName = KBrandNameGoogleMail;
            }
        }
    }
}

