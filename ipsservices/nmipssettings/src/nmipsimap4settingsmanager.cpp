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
#include <imapset.h>
#include <cemailaccounts.h>
#include <xqconversions.h>

#include "nmipsimap4settingsmanager.h"
#include "nmipsextendedsettingsmanager.h"

/*!
    \class NmIpsImap4SettingsManager
    \brief The class implements NmIpsImap4SettingsManager which is used for manipulating
    IMAP4 specific settings of an IMAP4 mailbox.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor
    Creates the CImImap4Settings instance for loading and saving the IMAP4 settings.
    Finds and loads the SMTP account and settings linked to the IMAP4 account.
    \param mailboxId Mailbox identifier.
    \param account CEmailAccounts created by the settings manager factory. Takes ownership.
    \param imapAccount TImapAccount of the mailbox.
*/
NmIpsImap4SettingsManager::NmIpsImap4SettingsManager(const NmId &mailboxId,
    CEmailAccounts *account,
    TImapAccount imapAccount)
: NmIpsSettingsManagerBase(mailboxId, account),
  mImap4Account(imapAccount)
{
    QT_TRAP_THROWING(mImap4Settings = new(ELeave) CImImap4Settings());

    TRAP_IGNORE(mAccount->LoadImapSettingsL(mImap4Account, *mImap4Settings));
    TRAP_IGNORE(mAccount->GetSmtpAccountL(mImap4Account.iSmtpService, mSmtpAccount));
    TRAP_IGNORE(mAccount->LoadSmtpSettingsL(mSmtpAccount, *mSmtpSettings));
}

/*!
    Destructor
*/
NmIpsImap4SettingsManager::~NmIpsImap4SettingsManager()
{
    delete mImap4Settings;
}

/*!
    Finds and returns settings for the account.
    \param settingItem SettingItem enum of the setting to return.
    \param QVariant SettingValue of the found setting value.
    \return <true> when the setting item was found otherwise <false>.
*/
bool NmIpsImap4SettingsManager::readSetting(IpsServices::SettingItem settingItem, QVariant &settingValue)
{
    bool found(true);
    switch (settingItem) {
        case IpsServices::LoginName:
            settingValue = XQConversions::s60Desc8ToQString(mImap4Settings->LoginName());
            break;
        case IpsServices::Password:
            settingValue = XQConversions::s60Desc8ToQString(mImap4Settings->Password());
            break;
        case IpsServices::MailboxName:
            settingValue = XQConversions::s60DescToQString(mImap4Account.iImapAccountName);
            break;
        default:
            found = NmIpsSettingsManagerBase::readSetting(settingItem, settingValue);
            break;
    }
    return found;
}

/*!
    Writes IMAP4 specific settings or passes SMTP and extended settings to the base class.
    \param settingItem SettingItem enum of the setting to replace.
    \param settingValue QVariant of the new setting value.
    \return bool <true> when the setting item was succesfully written, otherwise <false>.
*/
bool NmIpsImap4SettingsManager::writeSetting(IpsServices::SettingItem settingItem, const QVariant &settingValue)
{
    HBufC *tmp = 0;
    HBufC8 *tmp8 = 0;

    bool ret(false);
    TInt err(KErrNone);

    switch (settingItem) {
        case IpsServices::LoginName:
            tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
            TRAP(err, mImap4Settings->SetLoginNameL(*tmp8));
            delete tmp8;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::Password:
            tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
            TRAP(err, mImap4Settings->SetPasswordL(*tmp8));
            delete tmp8;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::MailboxName:
            tmp = XQConversions::qStringToS60Desc(settingValue.toString());
            mImap4Account.iImapAccountName.Copy( *tmp );
            delete tmp;
            ret = saveSettings();
            break;
        default:
            ret = NmIpsSettingsManagerBase::writeSetting(settingItem, settingValue);
            break;
    }
    return ret;
}

/*!
    Deletes the IMAP4 mailbox.

    \return Error code <code>0</code> if mailbox deletion was successful, otherwise error
            code is returned.
*/
int NmIpsImap4SettingsManager::deleteMailbox()
{
    int error(NmIpsSettingsManagerBase::deleteMailbox());
    if (!error) {
        TRAP(error, mAccount->DeleteImapAccountL(mImap4Account));
    }

    NMLOG(QString("NmIpsImap4SettingsManager::deleteMailbox status %1").arg(error));
    return error;
}

/*!
    Stores the IMAP4 specific settings.
    \return bool <true> when the IMAP4 settings were succesfully written, otherwise <false>.
*/
bool NmIpsImap4SettingsManager::saveSettings()
{
    TRAPD(err, mAccount->SaveImapSettingsL(mImap4Account, *mImap4Settings));
    NMLOG(QString("NmIpsImap4SettingsManager::saveSettings rval %1").arg(err));
    return (err==KErrNone);
}
