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
#include <pop3set.h>
#include <cemailaccounts.h>
#include <xqconversions.h>

#include "nmipspop3settingsmanager.h"
#include "nmipsextendedsettingsmanager.h"

/*!
    \class NmIpsPop3SettingsManager
    \brief The class implements NmIpsPop3SettingsManager which is used for manipulating
    pop3 settings of a pop3 mailbox.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor
    Creates the CImPop4Settings instance for loading and saving the POP3 settings.
    Finds and loads the SMTP account and settings linked to the POP3 account.
    \param mailboxId Mailbox identifier.
    \param account CEmailAccounts created by the settings manager factory. Takes ownership.
    \param imapAccount TImapAccount of the mailbox.
*/
NmIpsPop3SettingsManager::NmIpsPop3SettingsManager(const NmId &mailboxId,
    CEmailAccounts *account, TPopAccount popAccount)
: NmIpsSettingsManagerBase(mailboxId, account, IpsServices::EMailPop),
  mPop3Account(popAccount)
{
    QT_TRAP_THROWING(mPop3Settings = new(ELeave) CImPop3Settings());

    TRAP_IGNORE(mAccount->LoadPopSettingsL(mPop3Account, *mPop3Settings));
    TRAP_IGNORE(mAccount->GetSmtpAccountL(mPop3Account.iSmtpService, mSmtpAccount));
    TRAP_IGNORE(mAccount->LoadSmtpSettingsL(mSmtpAccount, *mSmtpSettings));
}

/*!
    Destructor
*/
NmIpsPop3SettingsManager::~NmIpsPop3SettingsManager()
{
    delete mPop3Settings;
}

/*!
    Finds and returns settings for the account.
    \param settingItem SettingItem enum of the setting to return.
    \param QVariant SettingValue of the found setting value.
    \return <true> when the setting item was found otherwise <false>.
*/
bool NmIpsPop3SettingsManager::readSetting(IpsServices::SettingItem settingItem, QVariant &value)
{
	bool found(false);
	switch (settingItem) {
        case IpsServices::IncomingLoginName:
            value = XQConversions::s60Desc8ToQString(mPop3Settings->LoginName());
            found = true;
            break;
        case IpsServices::IncomingPassword:
            value = XQConversions::s60Desc8ToQString(mPop3Settings->Password());
            found = true;
            break;
        case IpsServices::MailboxName:
            value = XQConversions::s60DescToQString(mPop3Account.iPopAccountName);
            found = true;
            break;
        case IpsServices::IncomingMailServer:
            value = XQConversions::s60DescToQString(mPop3Settings->ServerAddress());
            found = true;
            break;   
        case IpsServices::IncomingPort:
            value = mPop3Settings->Port();
            found = true;
            break;  
        case IpsServices::IncomingSecureSockets:
            value = mPop3Settings->SecureSockets();
            found = true;
            break;  
        case IpsServices::IncomingSSLWrapper:
            value = mPop3Settings->SSLWrapper();
            found = true;
            break;  
        default:
            found = NmIpsSettingsManagerBase::readSetting(settingItem, value);
            break;
    }
	return found;
}

/*!
    Writes POP3 specific settings or passes SMTP and extended settings to the base class.
    \param settingItem SettingItem enum of the setting to replace.
    \param settingValue QVariant of the new setting value.
*/
bool NmIpsPop3SettingsManager::writeSetting(IpsServices::SettingItem settingItem, const QVariant &settingValue)
{
    HBufC *tmp = 0;
    HBufC8 *tmp8 = 0;

    bool ret(false);
    TInt err(KErrNone);

    switch (settingItem) {
        case IpsServices::IncomingLoginName:
            tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
            TRAP(err, mPop3Settings->SetLoginNameL(*tmp8));
            delete tmp8;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::IncomingPassword:
            tmp8 = XQConversions::qStringToS60Desc8(settingValue.toString());
            TRAP(err, mPop3Settings->SetPasswordL(*tmp8));
            delete tmp8;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::MailboxName:
            tmp = XQConversions::qStringToS60Desc(settingValue.toString());
            mPop3Account.iPopAccountName.Copy(*tmp);
            delete tmp;
            ret = saveSettings();
            break;
        case IpsServices::IncomingMailServer:
            tmp = XQConversions::qStringToS60Desc(settingValue.toString());
            TRAP(err, mPop3Settings->SetServerAddressL(*tmp));
            delete tmp;
            if (err==KErrNone) {
                ret = saveSettings();
            }
            break;
        case IpsServices::IncomingPort:            
            mPop3Settings->SetPort(settingValue.toInt());
            ret = saveSettings();
            break;
        case IpsServices::IncomingSecureSockets:
            mPop3Settings->SetSecureSockets(settingValue.toBool());
            ret = saveSettings();
            break;  
        case IpsServices::IncomingSSLWrapper:
            mPop3Settings->SetSSLWrapper(settingValue.toBool());
            ret = saveSettings();
            break;  
        default:
            ret = NmIpsSettingsManagerBase::writeSetting(settingItem, settingValue);
            break;
    }
    return ret;
}

/*!
    Deletes the POP3 mailbox.

    \return Error code <code>0</code> if mailbox deletion was successful, otherwise error
            code is returned.
*/
int NmIpsPop3SettingsManager::deleteMailbox()
{
    TRAPD(error, mAccount->DeletePopAccountL(mPop3Account));
    if (!error) {
    	NmIpsSettingsManagerBase::deleteMailbox();
    }

    NMLOG(QString("NmIpsPop3SettingsManager::deleteMailbox status %1").arg(error));
    return error;
}

/*!
    Stores the POP3 specific settings.
    \return bool <true> when the POP3 settings were succesfully written, otherwise <false>.
*/
bool NmIpsPop3SettingsManager::saveSettings()
{
    TRAPD(err, mAccount->SavePopSettingsL(mPop3Account, *mPop3Settings));
    NMLOG(QString("NmIpsPop3SettingsManager::saveSettings rval %1").arg(err));
    return (err==KErrNone);
}

/*!
     Determine the default port for the incoming mail server based on the security settings
     
     \return int the port number to use
 */
int NmIpsPop3SettingsManager::determineDefaultIncomingPort()
{
    int port = 0;    
    bool sslTls = mPop3Settings->SSLWrapper();    
    if (sslTls) {
        port = IpsServices::securePop3Port;
    } else {
        port = IpsServices::standardPop3Port;
    }        
    return port;
}
