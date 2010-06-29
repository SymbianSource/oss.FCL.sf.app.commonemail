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

#include "nmapiheaders.h"




static const int DefaultPopPort = 110;
static const int DefaultImapPort = 993;

using namespace EmailClientApi;
    
NmApiPopImapSettingsManager::NmApiPopImapSettingsManager()
{    
}

NmApiPopImapSettingsManager::~NmApiPopImapSettingsManager()
{   
}

bool NmApiPopImapSettingsManager::populateDefaults(const QString &mailboxType, NmApiMailboxSettingsData &data)
{
    bool ret = false;
    
    QT_TRY {
        if (mailboxType==NmApiMailboxTypePop) {
            populatePopDefs(data);
            ret = true;
        }
        else if (mailboxType==NmApiMailboxTypeImap) {
            populateImapDefs(data);
            ret = true;
        }
        else {
            ret = false;
        }
    }
    QT_CATCH(...){
        ret = false;
    }
    
    return ret;
}

void NmApiPopImapSettingsManager::populateImapDefs(NmApiMailboxSettingsData &data)
{
    QScopedPointer<CImImap4Settings> imapSettings(new CImImap4Settings());
    
    QScopedPointer<CImIAPPreferences> iapPref;
    QT_TRAP_THROWING(iapPref.reset(CImIAPPreferences::NewLC());
            CleanupStack::Pop(iapPref.data()));  
        
    QScopedPointer<CEmailAccounts> mailAccounts;
    QT_TRAP_THROWING(mailAccounts.reset(CEmailAccounts::NewL()));
        
    QT_TRAP_THROWING( mailAccounts->PopulateDefaultImapSettingsL(*imapSettings,*iapPref));
    
    data.setValue(IncomingLoginName, 
                           XQConversions::s60Desc8ToQString(imapSettings->LoginName()));

    data.setValue(IncomingPassword, 
                           XQConversions::s60Desc8ToQString(imapSettings->Password()));
       
    data.setValue(IncomingMailServer, 
                           XQConversions::s60DescToQString(imapSettings->ServerAddress()));
         
   if (imapSettings->Port()) {
       data.setValue(IncomingPort, imapSettings->Port());
       }
   else {
       data.setValue(IncomingPort, DefaultImapPort);
   }
   
   data.setValue(OutgoingSecureSockets, imapSettings->SecureSockets());
   data.setValue(OutgoingSSLWrapper, imapSettings->SSLWrapper());  
}

void NmApiPopImapSettingsManager::populatePopDefs(NmApiMailboxSettingsData &data)
{
    QScopedPointer<CImPop3Settings> popSettings(new CImPop3Settings());
   
    QScopedPointer<CImIAPPreferences> iapPref;
    QT_TRAP_THROWING(iapPref.reset(CImIAPPreferences::NewLC());
        CleanupStack::Pop(iapPref.data()));
 
    QScopedPointer<CEmailAccounts> mailAccounts;
    QT_TRAP_THROWING(mailAccounts.reset(CEmailAccounts::NewL()));
    
    QT_TRAP_THROWING( mailAccounts->PopulateDefaultPopSettingsL(*popSettings,*iapPref));
    
    data.setValue(IncomingLoginName, 
                        XQConversions::s60Desc8ToQString(popSettings->LoginName()));
   
    data.setValue(IncomingPassword, 
                        XQConversions::s60Desc8ToQString(popSettings->Password()));
     
    data.setValue(IncomingMailServer, 
                        XQConversions::s60DescToQString(popSettings->ServerAddress()));
    
    if (popSettings->Port()) {
        data.setValue(IncomingPort, popSettings->Port());
        }
    else {
        data.setValue(IncomingPort, DefaultPopPort);
    }
    
    data.setValue(OutgoingSecureSockets, popSettings->SecureSockets());
    data.setValue(OutgoingSSLWrapper, popSettings->SSLWrapper());
}


