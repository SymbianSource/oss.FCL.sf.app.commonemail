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

#include <QVariant>
#include <QList>

#include <xqconversions.h>
#include <xqsettingsmanager.h>
#include <xqcentralrepositoryutils.h>
#include <xqcentralrepositorysearchcriteria.h>

#include <pop3set.h>
#include <imapset.h>
#include <smtpset.h>
#include <iapprefs.h>

#include "ipssettingkeys.h"
#include "nmapiheaders.h"
#include <nmapisettingsmanager.h>
#include "nmapisettingsmanager_p.h"

namespace EmailClientApi
{

NmApiSettingsManagerPrivate::NmApiSettingsManagerPrivate(const quint64 mailboxId)
    : mAccount(0),
    mPop3Settings(0),
    mImap4Settings(0),
    mSmtpSettings(0),
    mMailboxOffset(-1),
    mMailboxId(0)
{
    QT_TRAP_THROWING(mAccount = CEmailAccounts::NewL());
    mMailboxId = (quint32)mailboxId;

    mQSettingsManager = new XQSettingsManager();
    Q_CHECK_PTR(mQSettingsManager);

    checkAccountType();
    QT_TRAP_THROWING(initAccountL());
    calculateMailboxOffset();

}

NmApiSettingsManagerPrivate::~NmApiSettingsManagerPrivate()
{
    delete mPop3Settings;
    delete mImap4Settings;
    delete mSmtpSettings;
    delete mQSettingsManager;
    delete mAccount;
}

/*!
 Loads QMail specific settings.
 \param mailboxId which settings are returned
 \param data consists of keys and values of settings.
 \return bool <true> when the setting items were found otherwise <false>.
 */
bool NmApiSettingsManagerPrivate::load(quint64 mailboxId, NmApiMailboxSettingsData &data)
{
	mMailboxId = mailboxId;
    return (readSetting(data) && readCenRepSetting(data));
}

/*!
 Saves QMail specific settings.
 \param data consists of keys and values of settings.
 \return bool <true> when the setting items were successfully saved otherwise <false>.
 */
bool NmApiSettingsManagerPrivate::save(const NmApiMailboxSettingsData &data)
{
    TInt err(KErrNone);
    TRAP(err, writeSettingL(data));
    return (!err && writeSettingToCenRep(data));
}

/*!
 Finds and returns the SMTP specific setting.
 \param settingItem SettingItem enum of the setting to return
 \param QVariant SettingValue of the found setting value.
 \return bool <true> when the setting item was found otherwise <false>.
 */
bool NmApiSettingsManagerPrivate::readSetting(NmApiMailboxSettingsData &data)
{
    if (mMailboxType == NmApiMailboxTypePop) {
        data.setValue(IncomingLoginName, XQConversions::s60Desc8ToQString(mPop3Settings->LoginName()));
        data.setValue(IncomingPassword, XQConversions::s60Desc8ToQString(mPop3Settings->Password()));
        data.setValue(MailboxName, XQConversions::s60DescToQString(mPop3Account.iPopAccountName));
        data.setValue(IncomingMailServer, XQConversions::s60DescToQString(mPop3Settings->ServerAddress()));
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
        data.setValue(IncomingMailUsesAuthentication, mPop3Settings->POP3Auth());
#endif
        data.setValue(IncomingPort, mPop3Settings->Port());
    }
    else if (mMailboxType  == NmApiMailboxTypeImap) {
        data.setValue(IncomingLoginName, XQConversions::s60Desc8ToQString(mImap4Settings->LoginName()));
        data.setValue(IncomingPassword, XQConversions::s60Desc8ToQString(mImap4Settings->Password()));
        data.setValue(MailboxName, XQConversions::s60DescToQString(
                mImap4Account.iImapAccountName));
        data.setValue(IncomingMailServer, XQConversions::s60DescToQString(
                mImap4Settings->ServerAddress()));
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
        data.setValue(IncomingMailUsesAuthentication, mImap4Settings->IAMP4Auth());
#endif
        data.setValue(IncomingPort, mImap4Settings->Port());
        data.setValue(FolderPath, XQConversions::s60Desc8ToQString(mImap4Settings->FolderPath()));
    }

    data.setValue(IncomingMailSecurityType, security());
    data.setValue(MyName, XQConversions::s60DescToQString(mSmtpSettings->EmailAlias()));
    data.setValue(EmailAlias, XQConversions::s60DescToQString(mSmtpSettings->EmailAlias()));
    data.setValue(EmailAddress, XQConversions::s60DescToQString(mSmtpSettings->EmailAddress()));
    data.setValue(ReplyAddress, XQConversions::s60DescToQString(mSmtpSettings->ReplyToAddress()));
    data.setValue(OutgoingMailServer, XQConversions::s60DescToQString(mSmtpSettings->ServerAddress()));
    data.setValue(OutgoingPort, mSmtpSettings->Port());
    data.setValue(OutgoingLoginName, XQConversions::s60Desc8ToQString(mSmtpSettings->LoginName()));
    data.setValue(OutgoingPassword, XQConversions::s60Desc8ToQString(mSmtpSettings->Password()));
    data.setValue(UseOutgoingAuthentication, mSmtpSettings->SMTPAuth());
    data.setValue(OutgoingMailUsesAuthentication, mSmtpSettings->SMTPAuth());
    data.setValue(OutgoingSecureSockets, mSmtpSettings->SecureSockets());
    data.setValue(OutgoingSSLWrapper, mSmtpSettings->SSLWrapper());
    data.setValue(OutgoingMailSecurityType, XQConversions::s60Desc8ToQString(mSmtpSettings->TlsSslDomain()));
    return true;
}

bool NmApiSettingsManagerPrivate::readCenRepSetting(NmApiMailboxSettingsData &data)
{
    bool ret = true;
    QVariant tmp;
    data.setValue(DownloadPictures, readFromCenRep(IpsServices::EmailKeyPreferenceDownloadPictures));
    data.setValue(MessageDivider, readFromCenRep(IpsServices::EmailKeyPreferenceMessageDivider));

    tmp = readFromCenRep(IpsServices::EmailKeyReceptionActiveProfile);
    switch(tmp.toInt()){
        case 0:
            data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetKUTD);
            break;
        case 1:
            data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetSE);
            break;
        case 2:
            data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetMF);
            break;
        case 3:
            data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetUD);
            break;
        default:
            data.setValue(ReceptionActiveProfile, EmailClientApi::EmailProfileOffsetKUTD);
            break;
    };

    data.setValue(ReceptionUserDefinedProfile, readFromCenRep(IpsServices::EmailKeyReceptionUserDefinedProfile));
    data.setValue(ReceptionInboxSyncWindow, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionInboxSyncWindow));
    data.setValue(ReceptionGenericSyncWindowInMessages, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionGenericSyncWindowInMessages));
    data.setValue(ReceptionWeekDays, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionWeekDays));
    data.setValue(ReceptionDayStartTime, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionDayStartTime));
    data.setValue(ReceptionDayEndTime, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionDayEndTime));
    data.setValue(ReceptionRefreshPeriodDayTime, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionRefreshPeriodDayTime));
    data.setValue(ReceptionRefreshPeriodOther, readFromCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionRefreshPeriodOther));
    data.setValue(UserNameHidden, readFromCenRep(IpsServices::EmailKeyUserNameHidden));
    data.setValue(EmailNotificationState, readFromCenRep(IpsServices::EmailKeyEMNState));
    data.setValue(FirstEmnReceived, readFromCenRep(IpsServices::EmailKeyFirstEmnReceived));
    data.setValue(EmnReceivedNotSynced, readFromCenRep(IpsServices::EmailKeyEmnReceivedNotSynced));

    QString aolState = alwaysOnlineState();
    if(aolState.length()>0)
        data.setValue(AlwaysOnlineState, aolState);

    data.setValue(AoLastSuccessfulUpdate, readFromCenRep(IpsServices::EmailKeyAoLastSuccessfulUpdateL));
    data.setValue(AoLastUpdateFailed, readFromCenRep(IpsServices::EmailKeyAoLastUpdateFailed));
    data.setValue(AoUpdateSuccessfulWithCurSettings, readFromCenRep(IpsServices::EmailKeyAoUpdateSuccessfulWithCurSettings));
    return ret;
}

/*!
 Writes POP3/IMAP4 specific settings.
 \param data consists of keys and values of settings.
 */
bool NmApiSettingsManagerPrivate::writeSettingL(const NmApiMailboxSettingsData &data)
{
    HBufC *tmp = 0;
    HBufC8 *tmp8 = 0;
    bool ret(false);
    int key;
    QVariant settingValue;
    QList<int> keylist;
    bool aliasSet = false;

    keylist = data.listSettings();
    for (int i = 0; i < keylist.size(); i++) {
        key = keylist[i];
        data.getValue(key, settingValue);
        switch (key) {
            case IncomingLoginName: {
                tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
                if (mMailboxType == NmApiMailboxTypePop) {
                    mPop3Settings->SetLoginNameL(*tmp8);
                }
                else if (mMailboxType == NmApiMailboxTypeImap) {
                    mImap4Settings->SetLoginNameL(*tmp8);
                    }
                delete tmp8;
                break;
            }
            case IncomingPassword: {
                tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
                if (mMailboxType == NmApiMailboxTypePop) {
                    mPop3Settings->SetPasswordL(*tmp8);
                }
                else if (mMailboxType == NmApiMailboxTypeImap) {
                    mImap4Settings->SetPasswordL(*tmp8);
                    }
                delete tmp8;
                break;
            }
            case MailboxName: {
                tmp = XQConversions::qStringToS60Desc(settingValue.toString());
                if (mMailboxType == NmApiMailboxTypePop) {
                    mPop3Account.iPopAccountName.Copy(*tmp);
                }
                else if (mMailboxType == NmApiMailboxTypeImap) {
                    mImap4Account.iImapAccountName.Copy(*tmp);
                    }
                delete tmp;
                break;
            }
            case IncomingMailServer: {
                tmp = XQConversions::qStringToS60Desc(settingValue.toString());

                if (mMailboxType == NmApiMailboxTypePop) {
                    mPop3Settings->SetServerAddressL(*tmp);
                }
                else if (mMailboxType == NmApiMailboxTypeImap) {
                    mImap4Settings->SetServerAddressL(*tmp);
                    }
                delete tmp;
                break;
            }
            case IncomingMailUsesAuthentication: {
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)

                if (mMailboxType == NmApiMailboxTypePop) {
                    mPop3Settings->SetPOP3Auth(settingValue.toBool());
                }
                else if (mMailboxType == NmApiMailboxTypeImap) {
                    mImap4Settings->SetIAMP4Auth(settingValue.toBool());
                }
#endif
                break;
            }
            case IncomingMailSecurityType: {
                setSecurity(settingValue.toString());
                break;
            }
            case IncomingPort: {
                if (mMailboxType == NmApiMailboxTypePop) {
                mPop3Settings->SetPort(settingValue.toInt());
                }
                else if (mMailboxType == NmApiMailboxTypeImap) {
                    mImap4Settings->SetPort(settingValue.toInt());
                }
                break;
            }
            case OutgoingPort: {
                mSmtpSettings->SetPort(settingValue.toInt());
                break;
            }
            case IncomingSecureSockets: {
                if (mMailboxType == NmApiMailboxTypePop) {
                    mPop3Settings->SetSecureSockets(settingValue.toBool());
                 }
                 else if (mMailboxType == NmApiMailboxTypeImap) {
                    mImap4Settings->SetSecureSockets(settingValue.toBool());
                     }
                break;
            }
            case IncomingSSLWrapper: {
                if (mMailboxType == NmApiMailboxTypePop) {
                    mPop3Settings->SetSSLWrapper(settingValue.toBool());
                }
                else if (mMailboxType == NmApiMailboxTypeImap) {
                    mImap4Settings->SetSSLWrapper(settingValue.toBool());
                    }
                break;
            }
            case EmailAddress: {
                tmp = XQConversions::qStringToS60Desc(settingValue.toString());
                mSmtpSettings->SetEmailAddressL(*tmp);
                break;
            }
            case ReplyAddress: {
                tmp = XQConversions::qStringToS60Desc(settingValue.toString());
                mSmtpSettings->SetReplyToAddressL(*tmp);
                break;
            }
            case EmailAlias: {
                QVariant tmpName;
                if(data.getValue(MyName,tmpName)) {
                    if(tmpName.toString()
                        ==XQConversions::s60DescToQString(mSmtpSettings->EmailAlias())) {
                        aliasSet = true;
                    }
                }
                tmp = XQConversions::qStringToS60Desc(settingValue.toString());
                mSmtpSettings->SetEmailAliasL(*tmp);
                break;
            }
            case MyName: {
                if(!aliasSet) {
                tmp = XQConversions::qStringToS60Desc(settingValue.toString());
                mSmtpSettings->SetEmailAliasL(*tmp);
                }
                break;
            }
            case OutgoingMailServer: {
                tmp = XQConversions::qStringToS60Desc(settingValue.toString());
                mSmtpSettings->SetServerAddressL(*tmp);
                break;
            }
            case OutgoingLoginName: {
                tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
                mSmtpSettings->SetLoginNameL(*tmp8);
                break;
            }
            case OutgoingPassword: {
                tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
                mSmtpSettings->SetPasswordL(*tmp8);
                break;
            }
            case UseOutgoingAuthentication:
            case OutgoingMailUsesAuthentication: {
                mSmtpSettings->SetSMTPAuth(settingValue.toBool());
                break;
            }
            case OutgoingSecureSockets: {
                mSmtpSettings->SetSecureSockets(settingValue.toBool());
                break;
            }
            case OutgoingSSLWrapper: {
                mSmtpSettings->SetSSLWrapper(settingValue.toBool());
                break;
            }
            case OutgoingMailSecurityType: {
                tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
                mSmtpSettings->SetTlsSslDomainL(*tmp8);
                break;
            }
            case FolderPath: {
                tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
                mImap4Settings->SetFolderPathL(*tmp8);
                break;
            }
            default: {
				break;
            }
        };
    }
    ret = saveSettings();
    return ret;
}

/*!
Writes settings to central repository.
\param data consists of keys and values of settings.
 */
bool NmApiSettingsManagerPrivate::writeSettingToCenRep(
    const NmApiMailboxSettingsData &data)
{
    int key;
    QVariant settingValue;
    QList<int> keylist;

    keylist = data.listSettings();
    bool ret = false;
    if(keylist.contains(ReceptionActiveProfile)) {
        data.getValue(ReceptionActiveProfile,settingValue);
        QVariant profileOffset = 0;
        if(settingValue.toString() == EmailClientApi::EmailProfileOffsetKUTD) {
            profileOffset=0;
        } else if(settingValue.toString() == EmailClientApi::EmailProfileOffsetSE){
            profileOffset = 1;
        } else if (settingValue.toString() == EmailClientApi::EmailProfileOffsetMF){
                    profileOffset = 2;
        } else if (settingValue.toString() == EmailClientApi::EmailProfileOffsetUD) {
            profileOffset = 3;
        }
        ret = writeToCenRep(IpsServices::EmailKeyReceptionActiveProfile, profileOffset);
        if(ret) {
            calculateActiveProfileOffset();
        }
    }
    if (ret) {
        for (int i = 0; i < keylist.size(); i++) {
            key = keylist[i];
            data.getValue(key, settingValue);
            switch (key) {
                case DownloadPictures: {
                    ret = writeToCenRep(IpsServices::EmailKeyPreferenceDownloadPictures,
                        settingValue);
                    break;
                }
                case MessageDivider: {
                    ret = writeToCenRep(IpsServices::EmailKeyPreferenceMessageDivider, settingValue);
                    break;
                }
                case ReceptionUserDefinedProfile: {
                    ret = writeToCenRep(IpsServices::EmailKeyReceptionUserDefinedProfile,
                        settingValue);
                    break;
                }
                case ReceptionInboxSyncWindow: {
                    ret = writeToCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionInboxSyncWindow, settingValue);
                    break;
                }
                case ReceptionGenericSyncWindowInMessages: {
                    ret = writeToCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionGenericSyncWindowInMessages, settingValue);
                    break;
                }
                case ReceptionWeekDays: {
                    ret = writeToCenRep(mActiveProfileOffset + IpsServices::EmailKeyReceptionWeekDays,
                        settingValue);
                    break;
                }
                case ReceptionDayStartTime: {
                    ret = writeToCenRep(mActiveProfileOffset + IpsServices::EmailKeyReceptionDayStartTime,
                        settingValue);
                    break;
                }
                case ReceptionDayEndTime: {
                    ret = writeToCenRep(mActiveProfileOffset + IpsServices::EmailKeyReceptionDayEndTime,
                        settingValue);
                    break;
                }
                case ReceptionRefreshPeriodDayTime: {
                    ret = writeToCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionRefreshPeriodDayTime, settingValue);
                    break;
                }
                case ReceptionRefreshPeriodOther: {
                    ret = writeToCenRep(mActiveProfileOffset
                        + IpsServices::EmailKeyReceptionRefreshPeriodOther, settingValue);
                    break;
                }
                case UserNameHidden: {
                    ret = writeToCenRep(IpsServices::EmailKeyUserNameHidden, settingValue);
                    break;
                }
                case EmailNotificationState: {
                    ret = writeToCenRep(IpsServices::EmailKeyEMNState, settingValue);
                    break;
                }
                case FirstEmnReceived: {
                    ret = writeToCenRep(IpsServices::EmailKeyFirstEmnReceived, settingValue);
                    break;
                }
                case EmnReceivedNotSynced: {
                    ret = writeToCenRep(IpsServices::EmailKeyEmnReceivedNotSynced, settingValue);
                    break;
                }
                case AlwaysOnlineState: {
                    setAlwaysOnlineState(settingValue);
                    break;
                }
                case AoLastSuccessfulUpdate: {
                    ret = writeToCenRep(IpsServices::EmailKeyAoLastSuccessfulUpdateL, settingValue);
                    break;
                }
                case AoLastUpdateFailed: {
                    ret = writeToCenRep(IpsServices::EmailKeyAoLastUpdateFailed, settingValue);
                    break;
                }
                case AoUpdateSuccessfulWithCurSettings: {
                    ret = writeToCenRep(IpsServices::EmailKeyAoUpdateSuccessfulWithCurSettings,
                        settingValue);
                    break;
                }
                case ReceptionActiveProfile:
                default: {
                    break;
                }
            };
        }
    }
    return ret;
}
/*!
 Stores the POP3/IMAP4 specific settings.
 \return bool <true> when the settings were succesfully written, otherwise <false>.
 */
bool NmApiSettingsManagerPrivate::saveSettings()
{
    bool ret(false);
    if (mMailboxType == NmApiMailboxTypePop) {
        TRAPD(err, mAccount->SavePopSettingsL(mPop3Account, *mPop3Settings));
        if (err == KErrNone) {
            ret = true;
        }
    }
    else if (mMailboxType == NmApiMailboxTypeImap) {
        TRAPD(err, mAccount->SaveImapSettingsL(mImap4Account, *mImap4Settings));
        if (err == KErrNone) {
            ret = true;
        }
    }
    return ret;
}

/*!
 Reads a key value from the Central Repository.
 \param key Key identifier.
 \return the settings value for the given key.
 */
QVariant NmApiSettingsManagerPrivate::readFromCenRep(quint32 key) const
{
    XQCentralRepositorySettingsKey settingKey(IpsServices::EmailMailboxSettingRepository,
        mMailboxOffset + key);
    return mQSettingsManager->readItemValue(settingKey);
}

/*!
 Writes a key value to the Central Repository.
 \param key Key identifier.
 \param value The settings value for the given key.
 \return Returns <true> if the value was succesfully written, <false> if not.
 */
bool NmApiSettingsManagerPrivate::writeToCenRep(quint32 key, const QVariant &value) const
{
    XQCentralRepositorySettingsKey settingKey(IpsServices::EmailMailboxSettingRepository,
        mMailboxOffset + key);
    return mQSettingsManager->writeItemValue(settingKey, value);
}

void NmApiSettingsManagerPrivate::setSecurity(QString securityType)
{
    if (securityType == NmApiStartTls) {
        if (mMailboxType == NmApiMailboxTypePop) {
            mPop3Settings->SetSecureSockets(ETrue);
            mPop3Settings->SetSSLWrapper(EFalse);
        }
        else {
            mImap4Settings->SetSecureSockets(ETrue);
            mImap4Settings->SetSSLWrapper(EFalse);
        }
    }
    else if (securityType == NmApiSSLTls) {
        if (mMailboxType == NmApiMailboxTypePop) {
            mPop3Settings->SetSecureSockets(EFalse);
            mPop3Settings->SetSSLWrapper(ETrue);
        }
        else {
            mImap4Settings->SetSecureSockets(EFalse);
            mImap4Settings->SetSSLWrapper(ETrue);
        }
    }
    else {
        if (mMailboxType == NmApiMailboxTypePop) {
            mPop3Settings->SetSecureSockets(EFalse);
            mPop3Settings->SetSSLWrapper(EFalse);
        }
        else {
            mImap4Settings->SetSecureSockets(EFalse);
            mImap4Settings->SetSSLWrapper(EFalse);
        }
    }
}

QString NmApiSettingsManagerPrivate::security() const
{
    bool ss(false);
    bool sslw(false);
    QString securityType("");

    if (mMailboxType == NmApiMailboxTypePop) {
        ss = mPop3Settings->SecureSockets();
        sslw = mPop3Settings->SSLWrapper();
    }
    else {
        ss = mImap4Settings->SecureSockets();
        sslw = mImap4Settings->SSLWrapper();
    }

    if (ss == true && sslw == false) {
        securityType = NmApiStartTls;
    }
    else if (ss == false && sslw == true) {
        securityType = NmApiSSLTls;
    }
    else {
        securityType = NmApiSecurityOff;
    }
    return securityType;
}

/*!

 */
QString NmApiSettingsManagerPrivate::alwaysOnlineState() const
{
    TInt profile = IpsServices::EmailSyncProfileManualFetch;
    QString ret = NmApiOff;

    QVariant state;
    TInt settingValue = -1;
    state = readFromCenRep(IpsServices::EmailKeyReceptionActiveProfile);
    settingValue = state.toInt();

    if (settingValue >= 0) {
        profile = settingValue;
        if (profile != IpsServices::EmailSyncProfileManualFetch) {
            ret = NmApiAlways;
        }
    }
    return ret;
}

/*!

 */
void NmApiSettingsManagerPrivate::setAlwaysOnlineState(const QVariant state)
{
    //only allowed to switch state off, not on.
    if (state.toString() == NmApiOff) {
        writeToCenRep(IpsServices::EmailKeyReceptionActiveProfile,
            IpsServices::EmailSyncProfileManualFetch);
    }
}

void NmApiSettingsManagerPrivate::calculateMailboxOffset()
{
    // Find all the keys that match the criteria 0xZZZZZ000, where Z=don't care.
    // This will give us all the keys that hold the mailbox ids, e.g. 0x00001000, 0x00002000, etc.
    quint32 partialKey(0x00000000);
    quint32 bitMask(0x00000FFF);
    XQCentralRepositorySearchCriteria criteria(IpsServices::EmailMailboxSettingRepository,
        partialKey, bitMask);
    // Set the mailbox id is value criteria for the search.
    criteria.setValueCriteria((int) mMailboxId);

    // Find the keys.
    XQCentralRepositoryUtils utils(*mQSettingsManager);
    QList<XQCentralRepositorySettingsKey> foundKeys = utils.findKeys(criteria);

    // We should only get one key as a result.
    if (foundKeys.count() == 1) {
        mMailboxOffset = foundKeys[0].key();
        calculateActiveProfileOffset();
    }
    else {
        mActiveProfileOffset = -1;
        mMailboxOffset = -1;
    }
}

/*!
 Calculates the active reception schedule profile offset.
 */
void NmApiSettingsManagerPrivate::calculateActiveProfileOffset()
{
    QVariant activeProfile = readFromCenRep(IpsServices::EmailKeyReceptionActiveProfile);
    mActiveProfileOffset = convertToProfileOffset(activeProfile.toInt());
}

/*!

 */
qint32 NmApiSettingsManagerPrivate::convertToProfileOffset(int profile) const
{
    quint32 profileOffset(0);
    // Possible values are defined in ipssettingkeys.h
    switch (profile) {
        case 0: {
            profileOffset = IpsServices::EmailProfileOffsetKUTD;
            break;
        }
        case 1: {
            profileOffset = IpsServices::EmailProfileOffsetSE;
            break;
        }
        case 2: {
            profileOffset = IpsServices::EmailProfileOffsetMF;
            break;
        }
        case 3: {
            profileOffset = IpsServices::EmailProfileOffsetUD;
            break;
        }
        default: {
            break;
        }
    };

    return profileOffset;
}

bool NmApiSettingsManagerPrivate::checkAccountType()
{
    bool identified = false;

    RArray<TImapAccount> imapAccounts;
    CleanupClosePushL(imapAccounts);
    TRAPD(err, mAccount->GetImapAccountsL(imapAccounts));
    if (err == KErrNone) {
        for (int i=0; i < imapAccounts.Count(); ++i) {
            TImapAccount account = imapAccounts[i];
            if (account.iImapService == mMailboxId) {
                mImap4Account = account;
                mMailboxType = NmApiMailboxTypeImap;
                identified = true;
                break;
            }
        }
    }
    CleanupStack::PopAndDestroy(&imapAccounts);

    if (!identified) {
        RArray<TPopAccount> popAccounts;
        CleanupClosePushL(popAccounts);
        TRAPD(err, mAccount->GetPopAccountsL(popAccounts));
        if (err == KErrNone) {
            for (int i = 0; i < popAccounts.Count(); ++i) {
                TPopAccount account = popAccounts[i];
                if (popAccounts[i].iPopService == mMailboxId) {
                    mPop3Account = account;
                    mMailboxType = NmApiMailboxTypePop;
                    identified = true;
                    break;
                }
            }
        }
        CleanupStack::PopAndDestroy(&popAccounts);
    }

    return identified;
}

void NmApiSettingsManagerPrivate::initAccountL()
{
    if (mMailboxType == NmApiMailboxTypePop) {
        mPop3Settings = new(ELeave) CImPop3Settings();
        mAccount->LoadPopSettingsL(mPop3Account, *mPop3Settings);
    }
    else if (mMailboxType == NmApiMailboxTypeImap) {
        mImap4Settings = new(ELeave) CImImap4Settings();
        mAccount->LoadImapSettingsL(mImap4Account, *mImap4Settings);
    }

    mSmtpSettings = new(ELeave) CImSmtpSettings();

    if (mMailboxType == NmApiMailboxTypePop) {
        mAccount->GetSmtpAccountL(mPop3Account.iSmtpService, mSmtpAccount);
    }
    else if (mMailboxType == NmApiMailboxTypeImap) {
        mAccount->GetSmtpAccountL(mImap4Account.iSmtpService, mSmtpAccount);
    }
    mAccount->LoadSmtpSettingsL(mSmtpAccount, *mSmtpSettings);
}

} // end namespace
