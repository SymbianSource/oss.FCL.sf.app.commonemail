/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <xqsettingsmanager.h>
#include <xqcentralrepositoryutils.h>
#include <xqcentralrepositorysearchcriteria.h>
#include <qlist.h>

#include "nmipsextendedsettingsmanager.h"
#include "nmipssettingitems.h"
#include "ipssettingkeys.h"
#include "nmcommon.h"

/*!
    \class NmIpsExtendedSettingsManager
    \brief The class is used for manipulating extended POP3 and IMAP4 account settings stored
    in the Central Repository.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor    
    Creates the NmIpsExtendedSettingsManagerfor loading and saving extended mailbox settings.
    \param mailboxId Mailbox identifier.
*/
NmIpsExtendedSettingsManager::NmIpsExtendedSettingsManager(const NmId &mailboxId)
    : mMailboxId(mailboxId), mMailboxOffset(-1), mActiveProfileOffset(-1)
{
    mSettingsManager = new XQSettingsManager();

    calculateMailboxOffset();
}

/*!
    Destructor
*/
NmIpsExtendedSettingsManager::~NmIpsExtendedSettingsManager()
{
    delete mSettingsManager;
}

/*!     
    Reads extended mailbox setting.
    \param settingItem SettingItem enum of the setting to return.
    \param QVariant SettingValue of the found setting value.
    \return bool <true> when the setting item was read, otherwise <false>.
*/
bool NmIpsExtendedSettingsManager::readSetting(IpsServices::SettingItem settingItem,
                                               QVariant &settingValue) const
{
    bool ret(mMailboxOffset>=0);
    if(ret) {
        switch(settingItem) {
            case IpsServices::DownloadPictures:
                settingValue = readFromCenRep(IpsServices::EmailKeyPreferenceDownloadPictures);
                break;
            case IpsServices::MessageDivider:
                settingValue = readFromCenRep(IpsServices::EmailKeyPreferenceMessageDivider);
                break;
            case IpsServices::ReceptionActiveProfile:
                settingValue = readFromCenRep(IpsServices::EmailKeyReceptionActiveProfile);
                break;
            case IpsServices::ReceptionUserDefinedProfile:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionUserDefinedProfile);
                break;
            case IpsServices::ReceptionInboxSyncWindow:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionInboxSyncWindow);
                break;
            case IpsServices::ReceptionGenericSyncWindowInMessages:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                      IpsServices::EmailKeyReceptionGenericSyncWindowInMessages);
                break;
            case IpsServices::ReceptionWeekDays:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionWeekDays);
                break;
            case IpsServices::ReceptionDayStartTime:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionDayStartTime);
                break;
            case IpsServices::ReceptionDayEndTime:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionDayEndTime);
                break;
            case IpsServices::ReceptionRefreshPeriodDayTime:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionRefreshPeriodDayTime);
                break;
            case IpsServices::ReceptionRefreshPeriodOther:
                settingValue = readFromCenRep(mActiveProfileOffset +
                                              IpsServices::EmailKeyReceptionRefreshPeriodOther);
                break;
            case IpsServices::UserNameHidden:
                settingValue = readFromCenRep(IpsServices::EmailKeyUserNameHidden);
                break;
            default:
                ret = false;
                break;
            }
    }
    return ret;
}
   
/*!     
    Writes extended mailbox setting.
    \param settingItem SettingItem enum of the setting to replace.
    \param settingValue QVariant of the new setting value.
    \return bool <true> when the setting item was written, otherwise <false>.
*/
bool NmIpsExtendedSettingsManager::writeSetting(IpsServices::SettingItem settingItem,
                                                const QVariant &settingValue)
{
    bool ret(mMailboxOffset>=0);
    if(ret) {
        switch(settingItem) {
            case IpsServices::DownloadPictures:
                ret = writeToCenRep(IpsServices::EmailKeyPreferenceDownloadPictures, settingValue);
                break;
            case IpsServices::MessageDivider:
                ret = writeToCenRep(IpsServices::EmailKeyPreferenceMessageDivider, settingValue);
                break;
            case IpsServices::ReceptionActiveProfile:
                ret = writeToCenRep(IpsServices::EmailKeyReceptionActiveProfile, settingValue);
                calculateActiveProfileOffset();
                break;
            case IpsServices::ReceptionUserDefinedProfile:
                ret = writeToCenRep(mActiveProfileOffset +
                                    IpsServices::EmailKeyReceptionUserDefinedProfile,
                                    settingValue);
                break;
            case IpsServices::ReceptionInboxSyncWindow:
                ret = writeToCenRep(mActiveProfileOffset +
                                    IpsServices::EmailKeyReceptionInboxSyncWindow,
                                    settingValue);
                break;
            case IpsServices::ReceptionGenericSyncWindowInMessages:
                ret = writeToCenRep(mActiveProfileOffset +
                                    IpsServices::EmailKeyReceptionGenericSyncWindowInMessages,
                                    settingValue);
                break;
            case IpsServices::ReceptionWeekDays:
                ret = writeToCenRep(mActiveProfileOffset + IpsServices::EmailKeyReceptionWeekDays,
                                    settingValue);
                break;
            case IpsServices::ReceptionDayStartTime:
                ret = writeToCenRep(mActiveProfileOffset +
                                    IpsServices::EmailKeyReceptionDayStartTime,
                                    settingValue);
                break;
            case IpsServices::ReceptionDayEndTime:
                ret = writeToCenRep(mActiveProfileOffset + IpsServices::EmailKeyReceptionDayEndTime,
                                    settingValue);
                break;
            case IpsServices::ReceptionRefreshPeriodDayTime:
                ret = writeToCenRep(mActiveProfileOffset +
                                    IpsServices::EmailKeyReceptionRefreshPeriodDayTime,
                                    settingValue);
                break;
            case IpsServices::ReceptionRefreshPeriodOther:
                ret = writeToCenRep(mActiveProfileOffset +
                                    IpsServices::EmailKeyReceptionRefreshPeriodOther,
                                    settingValue);
                break;
            case IpsServices::UserNameHidden:
                ret = writeToCenRep(IpsServices::EmailKeyUserNameHidden, settingValue);
                break;
            default:
                ret = false;
                break;
            }
    }
    return ret;
}

/*!     
    Deletes all the extended settings of the mailbox.
*/
void NmIpsExtendedSettingsManager::deleteSettings()
{
    if(mMailboxOffset>=0) {

        // Find all the keys that match the criteria 0xXXXXXZZZ, where X=part of mailbox offset
        // and Z=don't care. This will give us all the keys for the particular mailbox.
        quint32 partialKey(mMailboxOffset);
        quint32 bitMask(0xFFFFF000);
        XQCentralRepositorySearchCriteria criteria(IpsServices::EmailMailboxSettingRepository,
                                                   partialKey, bitMask);
        // Find the keys.
        XQCentralRepositoryUtils utils(*mSettingsManager);
        QList<XQCentralRepositorySettingsKey> foundKeys = utils.findKeys(criteria);

        // Delete the keys.
        foreach(XQCentralRepositorySettingsKey key, foundKeys) {
            utils.deleteKey(key);
            }
    }
}

/*!
    Reads a key value from the Central Repository.
    \param key Key identifier.
    \return The settings value for the given key.
*/
QVariant NmIpsExtendedSettingsManager::readFromCenRep(quint32 key) const
{
    XQCentralRepositorySettingsKey settingKey(IpsServices::EmailMailboxSettingRepository,
                                              mMailboxOffset + key);
    return mSettingsManager->readItemValue(settingKey);
}

/*!
    Writes a key value to the Central Repository.
    \param key Key identifier.
    \param value The settings value for the given key.
    \return Returns <true> if the value was succesfully written, <false> if not.
*/
bool NmIpsExtendedSettingsManager::writeToCenRep(quint32 key, const QVariant &value) const
{
    XQCentralRepositorySettingsKey settingKey(IpsServices::EmailMailboxSettingRepository,
                                              mMailboxOffset + key);
    return mSettingsManager->writeItemValue(settingKey, value);
}

void NmIpsExtendedSettingsManager::calculateMailboxOffset()
{
    // Find all the keys that match the criteria 0xZZZZZ000, where Z=don't care.
    // This will give us all the keys that hold the mailbox ids, e.g. 0x00001000, 0x00002000, etc.
    quint32 partialKey(0x00000000);
    quint32 bitMask(0x00000FFF);
    XQCentralRepositorySearchCriteria criteria(IpsServices::EmailMailboxSettingRepository,
                                               partialKey, bitMask);
    // Set the mailbox id is value criteria for the search.
    criteria.setValueCriteria((int)mMailboxId.id32());

    // Find the keys.
    XQCentralRepositoryUtils utils(*mSettingsManager);
    QList<XQCentralRepositorySettingsKey> foundKeys = utils.findKeys(criteria);

    // We should only get one key as a result.
    if (foundKeys.count() == 1) {
        mMailboxOffset = foundKeys[0].key();
        calculateActiveProfileOffset();
    }
}

/*!
    Calculates the active reception schedule profile offset.
*/
void NmIpsExtendedSettingsManager::calculateActiveProfileOffset()
{
    quint32 profileOffset(0);

    QVariant activeProfile = readFromCenRep(IpsServices::EmailKeyReceptionActiveProfile);

    // Possible values are defined in ipssettingkeys.h
    switch(activeProfile.toInt()) {
        case 0:
            profileOffset = IpsServices::EmailProfileOffsetKUTD;
            break;
        case 1:
            profileOffset = IpsServices::EmailProfileOffsetSE;
            break;
        case 2:
            profileOffset = IpsServices::EmailProfileOffsetMF;
            break;
        case 3:
            profileOffset = IpsServices::EmailProfileOffsetUD;
            break;
        default:
            break;
    }
    mActiveProfileOffset = profileOffset;
}
