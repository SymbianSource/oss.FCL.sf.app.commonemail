/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements classes CIpsSetDataExtension, Extension. 
*
*/



#include "emailtrace.h"
#include <e32base.h>

#include "ipssetutilsconsts.h"
#include "ipssetdataextension.h"



// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::CIpsSetDataExtension()
// ----------------------------------------------------------------------------
//
CIpsSetDataExtension::CIpsSetDataExtension()
    :
    iMailboxId( KErrNotFound ),
    iAccountId( 0 ),
    iExtensionId( 0 ),
    iProtocol( TUid::Uid( 0 ) ),
    iOpenHtmlMail( KErrNotFound ),
    iVersion( KErrNotFound ),
    iAlwaysOnlineState( EMailAoOff ),
    iSelectedWeekDays( 0 ),
    iSelectedTimeStart( 0 ),
    iSelectedTimeStop( 0 ),
    iInboxRefreshTime( KErrNotFound ),
    iUpdateMode( EMailAoHeadersOnly ),
    iEmailAddress( NULL ),
    iEmnState( EMailEmnOff ),
    iHideMessages( EFalse ), 
    iNewMailIndicators( ETrue ),
    //<cmail>
    iHiddenData( EFalse )
    //</cmail>
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::~CIpsSetDataExtension()
// ----------------------------------------------------------------------------
//
CIpsSetDataExtension::~CIpsSetDataExtension()
    {
    FUNC_LOG;
    iEmailAddress.Close();    
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::NewL()
// ----------------------------------------------------------------------------
//
EXPORT_C CIpsSetDataExtension* CIpsSetDataExtension::NewL()
    {
    FUNC_LOG;
    CIpsSetDataExtension* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::NewLC()
// ----------------------------------------------------------------------------
//
EXPORT_C CIpsSetDataExtension* CIpsSetDataExtension::NewLC()
    {
    FUNC_LOG;
    CIpsSetDataExtension* self =
        new ( ELeave ) CIpsSetDataExtension();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataExtension::ConstructL()
    {
    FUNC_LOG;
    iEmailAddress.CreateL( KIpsSetUiMaxSettingsLongTextLength );
    PopulateDefaults();
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::CopyL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataExtension::CopyL(
    const CIpsSetDataExtension& aExtension )
    {
    FUNC_LOG;
    // remove trailing L, function can not leave
    iStatusFlags = aExtension.iStatusFlags;
    iMailboxId = aExtension.iMailboxId;
    iAccountId = aExtension.iAccountId;
    iExtensionId = aExtension.iExtensionId;
    iProtocol = aExtension.iProtocol;
    iHideMessages = aExtension.iHideMessages;
    iNewMailIndicators = aExtension.iNewMailIndicators;
    iOpenHtmlMail = aExtension.iOpenHtmlMail;
    iVersion = aExtension.iVersion;
    iAlwaysOnlineState = aExtension.iAlwaysOnlineState;
    iSelectedWeekDays = aExtension.iSelectedWeekDays;
    iSelectedTimeStart = aExtension.iSelectedTimeStart;
    iSelectedTimeStop = aExtension.iSelectedTimeStop;
    iInboxRefreshTime = aExtension.iInboxRefreshTime;
    iUpdateMode = aExtension.iUpdateMode;
    iLastUpdateInfo = aExtension.iLastUpdateInfo;
    iEmailAddress.Copy( aExtension.iEmailAddress );
    iEmnState = aExtension.iEmnState;
    iOutgoingLogin = aExtension.iOutgoingLogin;
    iLastModified = aExtension.iLastModified;
    iSyncStatus = aExtension.iSyncStatus;
    //<cmail>
    iHiddenData = aExtension.iHiddenData;
    //</cmail>
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::IsSettingsValid()
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CIpsSetDataExtension::IsSettingsValid() const
    {
    FUNC_LOG;
    
    // this function not work as described,
    // extension id seems to be > 255 in pop accounts
    // just comment out and return always true...
    
    // Verify that the account id is set, extension id is set, mailbox id
    // is set and the protocol is set to either imap4 or pop3

    // Make sure the mailbox id is valid
    /*if ( iMailboxId < 0 )
        {
        return EFalse;
        }

    // Make sure that the account id is between 1 <= id <= 255
    if ( !iAccountId || iAccountId > KIPSMaxAccounts )
        {
        return EFalse;
        }

    // Check the extension id's for each protocol
    IpsSetDataCtrlAccount accountControl;
    TUint32 extensionId = accountControl.ExtendedAccountIdToAccountId(
        iExtensionId, iProtocol );

    // Make sure that the extension id is between 1 <= id <= 255
    if ( !extensionId || extensionId > KIPSMaxAccounts )
        {
        return EFalse;
        }*/

    // Settings are valid
    return ETrue;
    }

/******************************************************************************

    Extended mail settings

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetStatusFlags()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetStatusFlags(
    const TUint64 aStatusFlags )
    {
    FUNC_LOG;
    iStatusFlags = aStatusFlags;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::StatusFlags()
// ----------------------------------------------------------------------------
//
EXPORT_C TUint64 CIpsSetDataExtension::StatusFlags() const
    {
    FUNC_LOG;
    return iStatusFlags;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetEmailAddress()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetEmailAddress(
    const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    iEmailAddress.Copy( aEmailAddress );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::EmailAddress()
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIpsSetDataExtension::EmailAddress() const
    {
    FUNC_LOG;
    return iEmailAddress;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetEmailNotificationState()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetEmailNotificationState(
    const TIpsSetDataEmnStates aEmnState )
    {
    FUNC_LOG;
    iEmnState = aEmnState;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::EmailNotificationState()
// ----------------------------------------------------------------------------
//
EXPORT_C TIpsSetDataEmnStates CIpsSetDataExtension::EmailNotificationState() 
    const
    {
    FUNC_LOG;
    return iEmnState;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetNewMailIndicators()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetNewMailIndicators(
    const TBool aNewState )
    {
    FUNC_LOG;
    iNewMailIndicators = aNewState;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::NewMailIndicators()
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CIpsSetDataExtension::NewMailIndicators() const
    {
    FUNC_LOG;
    return iNewMailIndicators;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetHideMsgs()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetHideMsgs(
    const TBool aNewState )
    {
    FUNC_LOG;
    iHideMessages = aNewState;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::HideMsgs()
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CIpsSetDataExtension::HideMsgs() const
    {
    FUNC_LOG;
    return iHideMessages;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetOpenHtmlMail()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetOpenHtmlMail(
    const TInt aOpenHtmlMail )
    {
    FUNC_LOG;
    iOpenHtmlMail = aOpenHtmlMail;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::OpenHtmlMail()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CIpsSetDataExtension::OpenHtmlMail() const
    {
    FUNC_LOG;
    return iOpenHtmlMail;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetOpenHtmlMail()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetMailboxId(
    const TMsvId aMailboxId )
    {
    FUNC_LOG;
    iMailboxId = aMailboxId;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::OpenHtmlMail()
// ----------------------------------------------------------------------------
//
EXPORT_C TMsvId CIpsSetDataExtension::MailboxId() const
    {
    FUNC_LOG;
    return iMailboxId;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetAccountId()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetAccountId(
    const TUint32 aAccountId )
    {
    FUNC_LOG;
    iAccountId = aAccountId;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::AccountId()
// ----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIpsSetDataExtension::AccountId() const
    {
    FUNC_LOG;
    return iAccountId;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetExtensionId()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetExtensionId(
    const TUint32 aExtensionId )
    {
    FUNC_LOG;
    iExtensionId = aExtensionId;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::ExtensionId()
// ----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIpsSetDataExtension::ExtensionId() const
    {
    FUNC_LOG;
    return iExtensionId;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetProtocol()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetProtocol(
    const TUid& aProtocol )
    {
    FUNC_LOG;
    iProtocol = aProtocol;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::Protocol()
// ----------------------------------------------------------------------------
//
EXPORT_C TUid CIpsSetDataExtension::Protocol() const
    {
    FUNC_LOG;
    return iProtocol;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetSyncStatus()
// ----------------------------------------------------------------------------    
EXPORT_C void CIpsSetDataExtension::SetSyncStatus( TInt aLastSyncStatus )
    {
    FUNC_LOG;
    iSyncStatus = aLastSyncStatus;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::LastSyncStatus()
// ----------------------------------------------------------------------------    
EXPORT_C TInt CIpsSetDataExtension::LastSyncStatus() const
    {
    FUNC_LOG;
    return iSyncStatus;
    }

//<cmail>
// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetDataHidden()
// ----------------------------------------------------------------------------    
EXPORT_C void CIpsSetDataExtension::SetDataHidden( const TBool aIsHidden )
    {
    iHiddenData = aIsHidden;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::DataHidden()
// ----------------------------------------------------------------------------    
EXPORT_C TBool CIpsSetDataExtension::DataHidden() const
    {
    return iHiddenData;
    }
//</cmail>    
    
/******************************************************************************

    Always Online settings

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::Version()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CIpsSetDataExtension::Version() const
    {
    FUNC_LOG;
    return iVersion;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetAlwaysOnlineState()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetAlwaysOnlineState(
    const TIpsSetDataAoStates aAlwaysOnlineState,
    TBool aIgnoreStateFlag )
    {
    FUNC_LOG;
    iAlwaysOnlineState = aAlwaysOnlineState;
    
    if ( aIgnoreStateFlag )
        {
        //a call from plugin's GoOnline will not change this flag.
        return;
        }
    //EMailAoOff will not cause any flag changes.
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::AlwaysOnlineState()
// ----------------------------------------------------------------------------
//
EXPORT_C TIpsSetDataAoStates CIpsSetDataExtension::AlwaysOnlineState() const
    {
    FUNC_LOG;
    return iAlwaysOnlineState;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetSelectedWeekDays()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetSelectedWeekDays(
    const TUint aSelectedWeekDays )
    {
    FUNC_LOG;
    iSelectedWeekDays = aSelectedWeekDays;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SelectedWeekDays()
// ----------------------------------------------------------------------------
//
EXPORT_C TUint CIpsSetDataExtension::SelectedWeekDays() const
    {
    FUNC_LOG;
    return iSelectedWeekDays;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetSelectedTimeStart()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetSelectedTimeStart(
    const TTime aSelectedTimeStart )
    {
    FUNC_LOG;
    iSelectedTimeStart = aSelectedTimeStart;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SelectedTimeStart()
// ----------------------------------------------------------------------------
//
EXPORT_C TTime CIpsSetDataExtension::SelectedTimeStart() const
    {
    FUNC_LOG;
    return iSelectedTimeStart;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetSelectedTimeStop()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetSelectedTimeStop(
    const TTime aSelectedTimeStop )
    {
    FUNC_LOG;
    iSelectedTimeStop = aSelectedTimeStop;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SelectedTimeStop()
// ----------------------------------------------------------------------------
//
EXPORT_C TTime CIpsSetDataExtension::SelectedTimeStop() const
    {
    FUNC_LOG;
    return iSelectedTimeStop;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetInboxRefreshTime()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetInboxRefreshTime(
    const TInt aInboxRefreshTime )
    {
    FUNC_LOG;
    iInboxRefreshTime = aInboxRefreshTime;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::InboxRefreshTime()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CIpsSetDataExtension::InboxRefreshTime() const
    {
    FUNC_LOG;
    return iInboxRefreshTime;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetUpdateMode()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetUpdateMode(
    const TIpsSetDataAoUpdateModes aUpdateMode )
    {
    FUNC_LOG;
    iUpdateMode = aUpdateMode;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::UpdateMode()
// ----------------------------------------------------------------------------
//
EXPORT_C TIpsSetDataAoUpdateModes CIpsSetDataExtension::UpdateMode() const
    {
    FUNC_LOG;
    return iUpdateMode;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::SetLastUpdateInfo()
// ----------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetLastUpdateInfo(
    const TAOInfo& aLastUpdateInfo )
    {
    FUNC_LOG;
    iLastUpdateInfo = aLastUpdateInfo;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataExtension::LastUpdateInfo()
// ----------------------------------------------------------------------------
//
EXPORT_C TAOInfo CIpsSetDataExtension::LastUpdateInfo() const
    {
    FUNC_LOG;
    return iLastUpdateInfo;
    }

// ---------------------------------------------------------------------------
// CIpsSetDataExtension::SetOutgoingLogin()
// ---------------------------------------------------------------------------
//
EXPORT_C void CIpsSetDataExtension::SetOutgoingLogin( 
        const TInt aOutgoingLogin )
    {
    FUNC_LOG;
    iOutgoingLogin = aOutgoingLogin;
    }

// ---------------------------------------------------------------------------
// CIpsSetDataExtension::OutgoingLogin()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIpsSetDataExtension::OutgoingLogin() const
    {
    FUNC_LOG;
    return iOutgoingLogin;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
EXPORT_C void CIpsSetDataExtension::SetLastModified( TTime aTime )
    {
    FUNC_LOG;
    iLastModified = aTime;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------    
EXPORT_C TTime CIpsSetDataExtension::LastModified() const
    {
    FUNC_LOG;
    return iLastModified;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------    
EXPORT_C void CIpsSetDataExtension::SetEmnReceivedButNotSyncedFlag( 
        TBool aFlag ) 
    {
    iEmnReceivedButNotSyncedFlag = aFlag;
    }

// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------- 
EXPORT_C TBool CIpsSetDataExtension::EmnReceivedButNotSyncedFlag() const
    {
    return iEmnReceivedButNotSyncedFlag;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------    
EXPORT_C void CIpsSetDataExtension::PopulateDefaults()
    {
    FUNC_LOG;
    iOpenHtmlMail = KIpsSetExtDefaultOpenHtmlMail;
    iVersion = KIpsSetExtDefaultAoVersion;
    iAlwaysOnlineState = static_cast<TIpsSetDataAoStates>(
            KIpsSetExtDefaultAlwaysOnlineState);
    iSelectedWeekDays = KIpsSetExtDefaultSelectedWeekDays;
    iSelectedTimeStart = KIpsSetExtDefaultSelectedTimeStart;
    iSelectedTimeStop = KIpsSetExtDefaultSelectedTimeStop;
    iInboxRefreshTime = KIpsSetExtDefaultInboxRefreshTime;
    iUpdateMode = static_cast<TIpsSetDataAoUpdateModes>(
            KIpsSetExtDefaultUpdateMode);
    iEmailAddress = KNullDesC16;
    iEmnState = static_cast<TIpsSetDataEmnStates>(
            KIpsSetExtDefaultEmailNotificationState);
    iHideMessages = KIpsSetExtDefaultHideMsgs;
    iNewMailIndicators = KIpsSetExtDefaultNewMailIndicators;
    iLastUpdateInfo.iLastUpdateFailed = 
        KIpsSetExtDefaultTAOInfoLastUpdateFailed;
    iLastUpdateInfo.iLastSuccessfulUpdate = 
        KIpsSetExtDefaultTAOInfoLastSuccessfulUpdate;
    iLastUpdateInfo.iUpdateSuccessfulWithCurSettings =
        KIpsSetExtDefaultTAOInfoUpdateSuccessfulWithCurSettings;
    iOutgoingLogin = KIpsSetExtDefaultOutgoingLogin;
    iLastModified = KIpsSetExtDefaultLastModified;
    iSyncStatus = KIpsSetExtDefaultSyncStatus;
    iStatusFlags = KIpsSetExtDefaultStatusFlags;
    //<cmail>
    iHiddenData = KIpsSetExtDefaultHiddenData;
    //</cmail>
    iEmnReceivedButNotSyncedFlag = 
        KIpsSetExtDefaultEmnReceivedButNotSyncedFlag;
	iFirstEMNReceived = KIpsSetExtDefaultFirstEMNReceived;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
EXPORT_C void CIpsSetDataExtension::SetFirstEmnReceived( TBool aValue ) 
    {
    iFirstEMNReceived = aValue;
    }

// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------- 
EXPORT_C TBool CIpsSetDataExtension::FirstEmnReceived() const
    {
    return iFirstEMNReceived;
    }
//  End of File

