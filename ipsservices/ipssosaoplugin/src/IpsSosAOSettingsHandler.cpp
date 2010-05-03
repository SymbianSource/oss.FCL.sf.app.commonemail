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
*
*/
#include "IpsSosAOSettingsHandler.h"
#include <SendUiConsts.h>


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSettingsHandler* CIpsSosAOSettingsHandler::NewL( TUid aProtocol )
    {
    CIpsSosAOSettingsHandler* self = new(ELeave)CIpsSosAOSettingsHandler();
    CleanupStack::PushL(self);
    self->ConstructL( aProtocol );
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSettingsHandler::CIpsSosAOSettingsHandler() 
    {
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOSettingsHandler::~CIpsSosAOSettingsHandler()
    {
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::ConstructL( TUid aProtocol )
    {
    iMtmType = aProtocol;
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsSosAOSettingsHandler::GetFSPluginId() const
    {
    TInt pluginId = 0;
    if ( iMtmType.iUid == KSenduiMtmImap4UidValue )
        {
        pluginId = IPSSOSIMAP4PLUGIN_IMPLEMENTATION_UID;
        }
    else if ( iMtmType.iUid == KSenduiMtmPop3UidValue )
        {
        pluginId = IPSSOSPOP3PLUGIN_IMPLEMENTATION_UID;
        }
    return pluginId;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::GetSubscribedImapFoldersL( 
            TMsvId /*aServiceId*/, 
            RArray<TMsvId>& /*aFoldersArray*/ )
    {
    
    }
    
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::ConstructImapPartialFetchInfo( 
        TImImap4GetPartialMailInfo& /*aInfo*/, 
        CImImap4Settings& /*aImap4Settings*/ )
    {
    
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
IpsServices::TIpsSetDataAoStates 
    CIpsSosAOSettingsHandler::AlwaysOnlineState() const
    {
    return IpsServices::EMailAoOff;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::SetAlwaysOnlineState(
    const IpsServices::TIpsSetDataAoStates /*aAlwaysOnlineState*/,
    TBool /*aIgnoreStateFlag*/ )
    {
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
IpsServices::TIpsSetDataEmnStates 
    CIpsSosAOSettingsHandler::EmailNotificationState() const
    {
    return IpsServices::EMailEmnOff;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::SetEmailNotificationState(
    const IpsServices::TIpsSetDataEmnStates /*aEmnState*/ )
    {
    
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOSettingsHandler::FirstEmnReceived() const
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::SetFirstEmnReceived( TBool /*aValue*/ )
    {
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOSettingsHandler::EmnReceivedButNotSyncedFlag() const
    {
    return EFalse;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::SetEmnReceivedButNotSyncedFlag( TBool /*aFlag*/ )
    {
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TUint CIpsSosAOSettingsHandler::SelectedWeekDays() const
    {
    return 0;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TTime CIpsSosAOSettingsHandler::SelectedTimeStart() const
    {
    TTime t = TTime(); 
    t.HomeTime();
    return t;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TTime CIpsSosAOSettingsHandler::SelectedTimeStop() const
    {
    TTime t = TTime();
    t.HomeTime();
    return t;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOSettingsHandler::SetLastUpdateInfo( 
        const IpsServices::TAOInfo& /*aLastUpdateInfo*/ )
    {
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
IpsServices::TAOInfo CIpsSosAOSettingsHandler::LastUpdateInfo() const
    {
    IpsServices::TAOInfo dummy;
    return dummy;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsSosAOSettingsHandler::InboxRefreshTime() const
    {
    return 0;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOSettingsHandler::RoamHomeOnlyFlag()
    {
    return EFalse;
    }
// End of file

