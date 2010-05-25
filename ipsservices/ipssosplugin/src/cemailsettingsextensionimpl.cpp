/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  implementation of settings extension for mailbox
*
*/

#include <cemailaccounts.h>
#include <imapset.h>
#include "ipssetdataapi.h"
#include "ipssetdataextension.h"

#include "emailtrace.h"
#include "cemailsettingsextensionimpl.h"          

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEmailSettingsExtension::CEmailSettingsExtension( const TUid& aUid ) : CEmailExtension( aUid )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEmailSettingsExtensionImpl::CEmailSettingsExtensionImpl() : 
    CEmailSettingsExtension( KEmailSettingExtensionUid )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEmailSettingsExtensionImpl::CEmailSettingsExtensionImpl(CMsvSession* aSession) : 
    CEmailSettingsExtension( KEmailSettingExtensionUid )
    {
    iSession = aSession;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEmailSettingsExtensionImpl::~CEmailSettingsExtensionImpl()
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmailSettingsExtensionImpl::GetSettingValue( const TDesC& /*aKey*/, TDesC8& /*aValue*/  ) const
    {
    FUNC_LOG

    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmailSettingsExtensionImpl::SetSettingValue( const TDesC& /*aKey*/, const RBuf8& /*aValue*/ )
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CEmailSettingsExtensionImpl::IsSetL( const TDesC& aKey )
    {
    FUNC_LOG
        
    TBool ret = EFalse;
    if(!aKey.Compare(EmailSyncInterval))
        {
        // create account api and setting objects
        CEmailAccounts* accounts = CEmailAccounts::NewL();
        CleanupStack::PushL( accounts );
        RArray<TImapAccount> imapBoxes;
        accounts->GetImapAccountsL(imapBoxes);
        TMsvId serviceId = iMailBoxId.Id();
        TBool found = EFalse;
        for(TInt i=0;i<imapBoxes.Count();i++)
        {
            if( serviceId == imapBoxes[i].iImapService)
            {
            found = ETrue;
            break;
            }
        }
        imapBoxes.Reset();
        if(!found)
            {
            RArray<TPopAccount> popBoxes;
            accounts->GetPopAccountsL(popBoxes);
            for(TInt i=0;i<popBoxes.Count();i++)
                {
                if( serviceId == popBoxes[i].iPopService)
                    {
                    found = ETrue;
                    break;
                    }
                }
            popBoxes.Reset();
            }
        if(found)
            {
            //Set new password and signal (possible) ongoing connect operation
            CIpsSetDataApi* api = CIpsSetDataApi::NewL( *iSession );
            CleanupStack::PushL( api );
            CIpsSetDataExtension* extentedSettings = CIpsSetDataExtension::NewL();
            CleanupStack::PushL( extentedSettings );
            api->LoadExtendedSettingsL(serviceId,*extentedSettings);
            TInt syncTime = extentedSettings->InboxRefreshTime();
            if(syncTime == KErrNotFound)
                {
                ret = ETrue;
                }
            else
                {
                ret = EFalse;
                }
            CleanupStack::PopAndDestroy( 2 );
            }
        CleanupStack::PopAndDestroy( );
        }
    return ret;
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TFSMailMsgId CEmailSettingsExtensionImpl::GetMailBoxId( )
    {
    FUNC_LOG
    return iMailBoxId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmailSettingsExtensionImpl::SetMailBoxId( const TFSMailMsgId& aMailBoxId )
    {
    FUNC_LOG
    iMailBoxId = aMailBoxId;
    }
