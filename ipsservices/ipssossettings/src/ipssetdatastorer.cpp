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
* Description: This file implements classes TIpsSetDataStorerParams, CIpsSetDataStorer. 
*
*/



#include "emailtrace.h"
#include <centralrepository.h>          // CRepository
#include <SendUiConsts.h>
#include <smtpset.h>
#include <imapset.h>
#include <pop3set.h>
#include <cemailaccounts.h>

#include "ipssetutilsconsts.hrh"
#include "ipssetdatastorer.h"
#include "ipssetdataextension.h"
#include "ipssetdatactrlcenrep.h"
#include "ipssetutils.h"
#include "ipssetutilsconsts.h"
#include "ipssetdatactrlaccount.h"

#include "ipsplgcommon.h"

#ifdef _DEBUG
_LIT( KIpsSetDataStorer, "settings - storer");
_LIT( KIpsSetDataCenRepKeyNotFound, "IpsSettings - cenrep key not found");
#endif

const TUint KIpsSetStorerAtChar = '@';

// ============================ TIpsSetDataStorerParams =======================

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TIpsSetDataStorerParams::TIpsSetDataStorerParams(
    const TUint32& aAccountId,
    const TUid& aMtmId )
    :
    iAccountId( aAccountId ),
    iMtmId( aMtmId )
    {
    FUNC_LOG;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TUint32 TIpsSetDataStorerParams::AccountId() const
    {
    FUNC_LOG;
    return iAccountId;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TUid TIpsSetDataStorerParams::MtmId() const
    {
    FUNC_LOG;
    return iMtmId;
    }

// ============================ CIpsSetDataStorer =============================

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSetDataStorer::CIpsSetDataStorer()
    :
    iCenRepSmtp( NULL ),
    iCenRepExtMail( NULL ),
    iCenRepAlwaysOnline( NULL ),
    iCenRepDefaultData( NULL ),
    iAccounts( NULL ),
    iCenRepControl( NULL )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSetDataStorer::ConstructL()
    {
    FUNC_LOG;
    iCenRepControl = CIpsSetDataCtrlCenrep::NewL();
    iCenRepExtMail = CRepository::NewL( KCRUidExtendedSettingsUid );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSetDataStorer::~CIpsSetDataStorer()
    {
    FUNC_LOG;
    delete iCenRepSmtp;
    delete iCenRepExtMail;
    delete iCenRepAlwaysOnline;
    delete iCenRepDefaultData;
    delete iAccounts;
    delete iCenRepControl;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSetDataStorer* CIpsSetDataStorer::NewL()
    {
    FUNC_LOG;
    CIpsSetDataStorer* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSetDataStorer* CIpsSetDataStorer::NewLC()
    {
    FUNC_LOG;
    CIpsSetDataStorer* self =
        new ( ELeave ) CIpsSetDataStorer();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::CreateExtendedSettings(
    const TMsvId aMailboxId,
    const TUint32 aAccountId,
    const TUid& aProtocol,
    CIpsSetDataExtension& aExtendedMailSettings,
    const TBool /*aPopulateDefaults*/ )
    {
    FUNC_LOG;
    // Initialize the settings
    aExtendedMailSettings.SetMailboxId( aMailboxId );
    aExtendedMailSettings.SetAccountId( aAccountId );
    aExtendedMailSettings.SetProtocol( aProtocol );
    aExtendedMailSettings.SetExtensionId(
        IpsSetDataCtrlAccount::AccountIdToExtendedAccountId(
            aAccountId, aProtocol ) );

    TInt error = KErrNone;

    // If populating has not failed, continue with creating the settings
    if ( error == KErrNone )
        {
        // Create the settings
        TRAP( error, CreateExtendedSettingsL( aExtendedMailSettings ) );
        
        if ( error != KErrNone )
            {
            __ASSERT_DEBUG(EFalse, User::Panic( 
                KIpsSetDataStorer , KErrNotFound ) );
            }
        
        }

    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::DeleteExtendedSettingsL(
    const TMsvEntry& aMailbox )
    {
    FUNC_LOG;
    TMsvId accountId;
    
    if ( aMailbox.iMtm == KSenduiMtmPop3Uid )
        {
        TPopAccount popAccount;
        AccountsL()->GetPopAccountL( aMailbox.iServiceId, popAccount );
        accountId = popAccount.iPopAccountId;
        }
    else
        {
        TImapAccount imapAccount;
        AccountsL()->GetImapAccountL( aMailbox.iServiceId, imapAccount );
        accountId = imapAccount.iImapAccountId;
        }

    TInt error = DeleteExtendedSettingsL(
        accountId, aMailbox.iMtm );
        
    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::SaveExtendedSettings(
    const CIpsSetDataExtension& aExtendedMailSettings )
    {
    FUNC_LOG;
    // Here it is good idea to make sure, that the settings are valid, so
    // no corrupted data is going in.
    if ( aExtendedMailSettings.IsSettingsValid() )
        {
        TRAPD( error, SaveExtendedSettingsL( aExtendedMailSettings ) );
        
        if ( error != KErrNone )
            {
            __ASSERT_DEBUG(EFalse, User::Panic( 
                KIpsSetDataCenRepKeyNotFound , KErrNotFound ) );
            }
        
        return error;
        }
    else
        {
        return KErrNotReady;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::SaveImap4Settings(
    const TUint32 aAccountId,
    const CImImap4Settings& aImap4Settings )
    {
    FUNC_LOG;
    TImapAccount imapAccountId;
    imapAccountId.iImapAccountId = aAccountId;

    TRAPD( error, 
        AccountsL()->SaveImapSettingsL( imapAccountId, aImap4Settings ) );

    if ( error != KErrNone )
        {
        __ASSERT_DEBUG(EFalse, User::Panic( 
            KIpsSetDataCenRepKeyNotFound , KErrNotFound ) );
        }

    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::SavePop3Settings(
    const TUint32 aAccountId,
    const CImPop3Settings& aPop3Settings )
    {
    FUNC_LOG;
    TPopAccount popAccountId;
    popAccountId.iPopAccountId = aAccountId;

    TRAPD( error,
        AccountsL()->SavePopSettingsL( popAccountId, aPop3Settings ) );
    
    if ( error != KErrNone )
        {
        __ASSERT_DEBUG(EFalse, User::Panic( 
            KIpsSetDataCenRepKeyNotFound , KErrNotFound ) );
        }

    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::SaveSmtpSettings(
    const TUint32 aAccountId,
    const CImSmtpSettings& aSmtpSettings )
    {
    FUNC_LOG;
    TSmtpAccount smtpAccountId;
    smtpAccountId.iSmtpAccountId = aAccountId;

    TRAPD( error,
        AccountsL()->SaveSmtpSettingsL( smtpAccountId, aSmtpSettings ) );

    if ( error != KErrNone )
        {
        __ASSERT_DEBUG(EFalse, User::Panic( 
            KIpsSetDataCenRepKeyNotFound , KErrNotFound ) );
        }
        
    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::LoadExtendedSettings(
    const TIpsSetDataStorerParams& aParams,
    CIpsSetDataExtension& aExtendedMailSettings )
    {
    FUNC_LOG;
    // Make sure the mtm is valid email mtm
    TBool ok = IpsSetUtils::IsMailMtm( aParams.MtmId() );

    // Choose the right way to Load the settings
    if ( ok && aParams.AccountId() != KErrNone )
        {
        TRAPD( error, LoadExtendedSettingsL(
            aParams.AccountId(), aParams.MtmId(), aExtendedMailSettings ) );
        
        if ( error != KErrNone )
            {
            // This happens in case when extended settings are not created.
            // If mailbox is created via "creator" or there was been failure
            // in mailbox creation...
            //__ASSERT_DEBUG(EFalse, User::Panic( 
            //    KIpsSetDataCenRepKeyNotFound , KErrNotFound ) );
            }

        return error;
        }
    else
        {
        return KErrNotSupported;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::LoadImap4Settings(
    const TUint32 aAccountId,
    CImImap4Settings& aImap4Settings )
    {
    FUNC_LOG;
    TImapAccount imapAccountId;
    imapAccountId.iImapAccountId = aAccountId;
    
    TRAPD( error,
        AccountsL()->LoadImapSettingsL( imapAccountId, aImap4Settings ) );

    if ( error != KErrNone )
        {
        __ASSERT_DEBUG(EFalse, User::Panic( 
            KIpsSetDataCenRepKeyNotFound , KErrNotFound ) );
        }


    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::LoadPop3Settings(
    const TUint32 aAccountId,
    CImPop3Settings& aPop3Settings )
    {
    FUNC_LOG;
    TPopAccount popAccountId;
    popAccountId.iPopAccountId = aAccountId;
    
    TRAPD( error,
        AccountsL()->LoadPopSettingsL( popAccountId, aPop3Settings ) );
    
    if ( error != KErrNone )
        {
        __ASSERT_DEBUG(EFalse, User::Panic( 
            KIpsSetDataCenRepKeyNotFound , KErrNotFound ) );
        }

    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::LoadSmtpSettings(
    const TUint32 aAccountId,
    CImSmtpSettings& aSmtpSettings )
    {
    FUNC_LOG;
    
    TSmtpAccount smtpAccountId;
    smtpAccountId.iSmtpAccountId = aAccountId;

    TRAPD( error,
        AccountsL()->LoadSmtpSettingsL( smtpAccountId, aSmtpSettings ) );
 
    if ( error != KErrNone )
        {
        __ASSERT_DEBUG(EFalse, User::Panic( 
            KIpsSetDataCenRepKeyNotFound , KErrNotFound ) );
        }

    return error;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSetDataStorer::CreateExtendedSettingsL(
    const CIpsSetDataExtension& aExtendedMailSettings )
    {
    FUNC_LOG;
    // Prepare account control
    iCenRepControl->SetActiveCentralRepository( *iCenRepExtMail );
    iCenRepControl->SetActiveBaseKey( aExtendedMailSettings.ExtensionId() );

    iCenRepControl->CreateToAccountL( EIpsSetDataCommonSetStatusFlagsH,
        EIpsSetDataCommonSetStatusFlagsL, 
        aExtendedMailSettings.StatusFlags() );
    
    iCenRepControl->CreateToAccountL( EIpsSetDataCommonSetMailboxId,
        aExtendedMailSettings.MailboxId() );
    iCenRepControl->CreateToAccountL( EIpsSetDataCommonSetAccountId,
        static_cast<TInt>( aExtendedMailSettings.AccountId() ) );
    iCenRepControl->CreateToAccountL( EIpsSetDataCommonSetExtensionId,
        static_cast<TInt>( aExtendedMailSettings.ExtensionId() ) );
    iCenRepControl->CreateToAccountL( EIpsSetDataCommonSetProtocol,
        aExtendedMailSettings.Protocol().iUid );
    iCenRepControl->CreateToAccountL( EIpsSetDataCommonSetEmailAddress,
        aExtendedMailSettings.EmailAddress() );

    // Extended mailbox settings
    iCenRepControl->CreateToAccountL( EIpsSetDataExtSetEmailNotif,
        static_cast<TInt>( aExtendedMailSettings.EmailNotificationState() ) );
    iCenRepControl->CreateToAccountL( EIpsSetDataExtSetIndicators,
        static_cast<TBool>( aExtendedMailSettings.NewMailIndicators() ) );
    iCenRepControl->CreateToAccountL( EIpsSetDataExtSetHideMsgs,
        static_cast<TInt>( aExtendedMailSettings.HideMsgs() ) );
    iCenRepControl->CreateToAccountL( EIpsSetDataExtSetOpenHtmlMail,
        static_cast<TInt>( aExtendedMailSettings.OpenHtmlMail() ) );

    // Always Online Settings
    iCenRepControl->CreateToAccountL( EIpsSetDataAoVersion,
        static_cast<TInt>( aExtendedMailSettings.Version() ) );
    iCenRepControl->CreateToAccountL( EIpsSetDataAoOnlineState,
        aExtendedMailSettings.AlwaysOnlineState() );
    iCenRepControl->CreateToAccountL( EIpsSetDataAoWeekDays,
        aExtendedMailSettings.SelectedWeekDays() );
    iCenRepControl->CreateToAccountL( EIpsSetDataAoTimeStartH, EIpsSetDataAoTimeStartL,
        aExtendedMailSettings.SelectedTimeStart().Int64() );
    iCenRepControl->CreateToAccountL( EIpsSetDataAoTimeStopH, EIpsSetDataAoTimeStopL,
        aExtendedMailSettings.SelectedTimeStop().Int64() );
    iCenRepControl->CreateToAccountL( EIpsSetDataAoInboxRefreshTime,
        aExtendedMailSettings.InboxRefreshTime() );
    iCenRepControl->CreateToAccountL( EIpsSetDataAoUpdateMode,
        aExtendedMailSettings.UpdateMode() );
    // Create the last update AO info
    TAOInfo aoInfo = aExtendedMailSettings.LastUpdateInfo();
    iCenRepControl->CreateToAccountL(
        EIpsSetDataAoLastSuccessfulUpdateH, EIpsSetDataAoLastSuccessfulUpdateL,
        aoInfo.iLastSuccessfulUpdate.Int64() );
    iCenRepControl->CreateToAccountL(
        EIpsSetDataAoLastUpdateFailed, aoInfo.iLastUpdateFailed );
    iCenRepControl->CreateToAccountL( EIpsSetDataAoUpdateSuccessfulWithCurSettings, 
        aoInfo.iUpdateSuccessfulWithCurSettings );

    // Extra items
    iCenRepControl->CreateToAccountL( EIpsSetDataFsOutgoingLogin,
        aExtendedMailSettings.OutgoingLogin() );
    
    iCenRepControl->CreateToAccountL(
        EIpsSetDataLastModifiedH, EIpsSetDataLastModifiedL,
        aExtendedMailSettings.LastModified().Int64() );
    
    iCenRepControl->CreateToAccountL( EIpsSetDataLastSyncStatus,
        aExtendedMailSettings.LastSyncStatus() );
        
    //<cmail>
    iCenRepControl->CreateToAccountL( EIpsSetDataHideUserNameAndAddress,
        aExtendedMailSettings.DataHidden() );
    //</cmail>
    iCenRepControl->CreateToAccountL( 
        EIpsSetDateEmnReceivedButNotSyncedFlag,
        aExtendedMailSettings.EmailNotificationState() );
		
	iCenRepControl->CreateToAccountL( 
            EIpsSetDataFirstEMNReceivedFlag,
            aExtendedMailSettings.FirstEmnReceived() );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::DeleteExtendedSettingsL(
    const TUint32 aAccountId,
    const TUid& aMtmId )
    {
    FUNC_LOG;
    TInt error = KErrNone;
    // Prepare account control
    iCenRepControl->SetActiveCentralRepository( *iCenRepExtMail );
    iCenRepControl->SetActiveBaseKey( aAccountId, aMtmId );

    // We don't care about errors in case of deletion
    // seems that KErrNotFound appears often
    iCenRepControl->DeleteFromAccount( EIpsSetDataCommonSetStatusFlagsH, 
        EIpsSetDataCommonSetStatusFlagsL );
    iCenRepControl->DeleteFromAccount( EIpsSetDataCommonSetMailboxId );
    iCenRepControl->DeleteFromAccount( EIpsSetDataCommonSetAccountId );
    iCenRepControl->DeleteFromAccount( EIpsSetDataCommonSetExtensionId );
    iCenRepControl->DeleteFromAccount( EIpsSetDataCommonSetProtocol );
    iCenRepControl->DeleteFromAccount( EIpsSetDataCommonSetEmailAddress );

    // Extended mailbox settings
    iCenRepControl->DeleteFromAccount( EIpsSetDataExtSetEmailNotif );
    iCenRepControl->DeleteFromAccount( EIpsSetDataExtSetIndicators );
    iCenRepControl->DeleteFromAccount( EIpsSetDataExtSetHideMsgs );
    iCenRepControl->DeleteFromAccount( EIpsSetDataExtSetOpenHtmlMail );

    // Always Online Settings
    iCenRepControl->DeleteFromAccount( EIpsSetDataAoVersion );
    iCenRepControl->DeleteFromAccount( EIpsSetDataAoOnlineState );
    iCenRepControl->DeleteFromAccount( EIpsSetDataAoWeekDays );
    iCenRepControl->DeleteFromAccount( EIpsSetDataAoTimeStartH, 
        EIpsSetDataAoTimeStartL );
    iCenRepControl->DeleteFromAccount( EIpsSetDataAoTimeStopH, 
        EIpsSetDataAoTimeStopL );
    iCenRepControl->DeleteFromAccount( EIpsSetDataAoInboxRefreshTime );
    iCenRepControl->DeleteFromAccount( EIpsSetDataAoUpdateMode );
    iCenRepControl->DeleteFromAccount( EIpsSetDataAoLastSuccessfulUpdateH, 
        EIpsSetDataAoLastSuccessfulUpdateL );
    iCenRepControl->DeleteFromAccount( EIpsSetDataAoLastUpdateFailed ); 
    iCenRepControl->DeleteFromAccount( 
        EIpsSetDataAoUpdateSuccessfulWithCurSettings );

    // Extra items
    iCenRepControl->DeleteFromAccount( EIpsSetDataFsOutgoingLogin );
    iCenRepControl->DeleteFromAccount( EIpsSetDataLastModifiedH, 
        EIpsSetDataLastModifiedL );
    iCenRepControl->DeleteFromAccount( EIpsSetDataLastSyncStatus );
    iCenRepControl->DeleteFromAccount( 
        EIpsSetDateEmnReceivedButNotSyncedFlag );

    return error;
    }
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSetDataStorer::SaveExtendedSettingsL(
    const CIpsSetDataExtension& aExtendedMailSettings )
    {
    FUNC_LOG;
    // Prepare account control
    iCenRepControl->SetActiveCentralRepository( *iCenRepExtMail );
    iCenRepControl->SetActiveBaseKey( aExtendedMailSettings.ExtensionId() );

    // Store the settings
    // Common Settings
    
    iCenRepControl->SetToAccountL( EIpsSetDataCommonSetStatusFlagsH,
        EIpsSetDataCommonSetStatusFlagsL, 
        aExtendedMailSettings.StatusFlags() );
    
    iCenRepControl->SetToAccountL( EIpsSetDataCommonSetMailboxId,
        aExtendedMailSettings.MailboxId() );
    iCenRepControl->SetToAccountL( EIpsSetDataCommonSetAccountId,
        static_cast<TInt>( aExtendedMailSettings.AccountId() ) );
    iCenRepControl->SetToAccountL( EIpsSetDataCommonSetExtensionId,
        static_cast<TInt>( aExtendedMailSettings.ExtensionId() ) );
    iCenRepControl->SetToAccountL( EIpsSetDataCommonSetProtocol,
        aExtendedMailSettings.Protocol().iUid );
    iCenRepControl->SetToAccountL( EIpsSetDataCommonSetEmailAddress,
        aExtendedMailSettings.EmailAddress() );

    // Extended mailbox settings
    iCenRepControl->SetToAccountL( EIpsSetDataExtSetEmailNotif,
        static_cast<TInt>( aExtendedMailSettings.EmailNotificationState() ) );
    iCenRepControl->SetToAccountL( EIpsSetDataExtSetIndicators,
        static_cast<TBool>( aExtendedMailSettings.NewMailIndicators() ) );
    iCenRepControl->SetToAccountL( EIpsSetDataExtSetHideMsgs,
        static_cast<TInt>( aExtendedMailSettings.HideMsgs() ) );
    iCenRepControl->SetToAccountL( EIpsSetDataExtSetOpenHtmlMail,
        static_cast<TBool>( aExtendedMailSettings.OpenHtmlMail() ) );

    // Always Online Settings
    iCenRepControl->SetToAccountL( EIpsSetDataAoVersion,
         static_cast<TInt>( aExtendedMailSettings.Version() ) );
    iCenRepControl->SetToAccountL( EIpsSetDataAoOnlineState,
        aExtendedMailSettings.AlwaysOnlineState() );
    iCenRepControl->SetToAccountL( EIpsSetDataAoWeekDays,
        aExtendedMailSettings.SelectedWeekDays() );
    iCenRepControl->SetToAccountL( EIpsSetDataAoTimeStartH, 
        EIpsSetDataAoTimeStartL,
        aExtendedMailSettings.SelectedTimeStart().Int64() );
    iCenRepControl->SetToAccountL( EIpsSetDataAoTimeStopH, 
        EIpsSetDataAoTimeStopL,
        aExtendedMailSettings.SelectedTimeStop().Int64() );
    iCenRepControl->SetToAccountL( EIpsSetDataAoInboxRefreshTime,
        aExtendedMailSettings.InboxRefreshTime() );
    iCenRepControl->SetToAccountL( EIpsSetDataAoUpdateMode,
        aExtendedMailSettings.UpdateMode() );
        
    TAOInfo aoInfo = aExtendedMailSettings.LastUpdateInfo();
    iCenRepControl->SetToAccountL(
        EIpsSetDataAoLastSuccessfulUpdateH, 
        EIpsSetDataAoLastSuccessfulUpdateL,
        aoInfo.iLastSuccessfulUpdate.Int64() );
    iCenRepControl->SetToAccountL(
        EIpsSetDataAoLastUpdateFailed, aoInfo.iLastUpdateFailed  );
    iCenRepControl->SetToAccountL( 
        EIpsSetDataAoUpdateSuccessfulWithCurSettings, 
        aoInfo.iUpdateSuccessfulWithCurSettings  );

    iCenRepControl->SetToAccountL( EIpsSetDataFsOutgoingLogin,
        aExtendedMailSettings.OutgoingLogin() );
        
    iCenRepControl->SetToAccountL(
        EIpsSetDataLastModifiedH, EIpsSetDataLastModifiedL,
        aExtendedMailSettings.LastModified().Int64() );
    iCenRepControl->SetToAccountL( EIpsSetDataLastSyncStatus,
        aExtendedMailSettings.LastSyncStatus() );
    
    iCenRepControl->SetToAccountL( 
            EIpsSetDateEmnReceivedButNotSyncedFlag,
            aExtendedMailSettings.EmnReceivedButNotSyncedFlag() );
    
    iCenRepControl->SetToAccountL( 
            EIpsSetDataFirstEMNReceivedFlag,
            aExtendedMailSettings.FirstEmnReceived() );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSetDataStorer::LoadExtendedSettingsL(
    const TUint32 aAccountId,
    const TUid& aMtmId,
    CIpsSetDataExtension& aExtendedMailSettings )
    {
    FUNC_LOG;
    
    // Prepare account control
    iCenRepControl->SetActiveCentralRepository( *iCenRepExtMail );
    iCenRepControl->SetActiveBaseKey( aAccountId, aMtmId );

    // Load the settings
    // Common Settings
    TInt error = KErrNone;
    TInt value = 0;
    
    // EIpsSetDataCommonSetAccountId
    //
    error = iCenRepControl->GetFromAccount( 
            EIpsSetDataCommonSetAccountId, value );
    if ( error == KErrNone )
        {
    aExtendedMailSettings.SetAccountId( value );
        }
    else if ( error == KErrNotFound )
        {
        // extended settings are not created at all
        // create now
        TMsvId service = 0;
        TMsvId smtpServ = 0;
        IpsSetUtils::GetServiceIdFromAccountIdL(
            aAccountId, aMtmId, service, smtpServ );
        
        aExtendedMailSettings.PopulateDefaults();
        CreateExtendedSettings(
            service, aAccountId, aMtmId,
            aExtendedMailSettings, EFalse );
        }
    else
        {
        User::Leave( error );
        }
    
    // EIpsSetDataCommonSetMailboxId
    //
    error = iCenRepControl->GetFromAccount( EIpsSetDataCommonSetMailboxId, value );
    if ( error == KErrNone )
        {
        aExtendedMailSettings.SetMailboxId( static_cast<TMsvId>( value ) );
        }
    else if ( error == KErrNotFound )
        {
        // creating mailbox id to account settings if not found
        TMsvId service = 0;
        TMsvId smtpServ = 0;
        IpsSetUtils::GetServiceIdFromAccountIdL(
            aAccountId, aMtmId, service, smtpServ );
        aExtendedMailSettings.SetMailboxId( static_cast<TMsvId>( service ) );
        
        TRAP_IGNORE( iCenRepControl->CreateToAccountL( 
                EIpsSetDataCommonSetMailboxId, service ) );
        }
    else
        {
        User::Leave( error );
        }
    
    // EIpsSetDataCommonSetExtensionId
    //
    error = iCenRepControl->GetFromAccount( 
            EIpsSetDataCommonSetExtensionId, value );
    if ( error == KErrNone )
        {
    aExtendedMailSettings.SetExtensionId( value );
        }
    else if ( error == KErrNotFound )
        {
        // create extension id
        TUint32 extId = IpsSetDataCtrlAccount::AccountIdToExtendedAccountId(
                    aAccountId, aMtmId );
        
        TRAP_IGNORE( iCenRepControl->CreateToAccountL( 
                EIpsSetDataCommonSetExtensionId, extId ) );
        }
    else
        {
        User::Leave( error );
        }
    
    // EIpsSetDataCommonSetProtocol
    //
    error = iCenRepControl->GetFromAccount( EIpsSetDataCommonSetProtocol, value );
    if ( error == KErrNone )
        {
    aExtendedMailSettings.SetProtocol( TUid::Uid( value ) );
        }
    else if ( error == KErrNotFound && 
            ( aMtmId == KSenduiMtmPop3Uid || 
                    aMtmId == KSenduiMtmImap4Uid ) )
        {
        TRAP_IGNORE( iCenRepControl->CreateToAccountL( 
                EIpsSetDataCommonSetProtocol, aMtmId.iUid ) );
        }
    else
        {
        aExtendedMailSettings.SetProtocol( aMtmId );
        }
    
    // EIpsSetDataCommonSetStatusFlagsH, EIpsSetDataCommonSetStatusFlagsL
    //
    TInt64 flags = 0;
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound(
         EIpsSetDataCommonSetStatusFlagsH, EIpsSetDataCommonSetStatusFlagsL, 
         KIpsSetExtDefaultStatusFlags, flags );
    aExtendedMailSettings.SetStatusFlags( flags );
    
    // EIpsSetDataCommonSetEmailAddress
    //
    TIpsSetUtilsTextPlain text;
    error = iCenRepControl->GetTextFromAccount( 
            EIpsSetDataCommonSetEmailAddress, text );
    TInt atPos = text.Locate( KIpsSetStorerAtChar );
    if ( error == KErrNone && atPos > 0 )
        {
        aExtendedMailSettings.SetEmailAddress( text );
        }
    else
        {
        CImSmtpSettings* smtpSet = IpsSetUtils::GetSmtpSettingsFromAccountIdL(
            aAccountId, aMtmId );
        text.Copy( smtpSet->EmailAddress() );
        delete smtpSet;
        smtpSet = NULL;
    aExtendedMailSettings.SetEmailAddress( text );

        if ( error == KErrNotFound )
            {
            TRAP_IGNORE( iCenRepControl->CreateToAccountL( 
                    EIpsSetDataCommonSetEmailAddress, text ) );
            }
        else if ( atPos == KErrNotFound )
            {
            TRAP_IGNORE( iCenRepControl->SetToAccountL( 
                    EIpsSetDataCommonSetEmailAddress, text ) );
            }
        }

    // EIpsSetDataExtSetEmailNotif
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound( 
            EIpsSetDataExtSetEmailNotif, 
            KIpsSetExtDefaultEmailNotificationState, value );
    aExtendedMailSettings.SetEmailNotificationState(
        static_cast<TIpsSetDataEmnStates>( value ) );
    
    // EIpsSetDataExtSetIndicators
    //
    TBool result = ETrue;
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound( 
            EIpsSetDataExtSetIndicators, KIpsSetExtDefaultNewMailIndicators,
            result );
    aExtendedMailSettings.SetNewMailIndicators( result );
    
    // EIpsSetDataExtSetHideMsgs
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound( 
            EIpsSetDataExtSetHideMsgs, KIpsSetExtDefaultHideMsgs,
            result );
    aExtendedMailSettings.SetHideMsgs( result );
    
    // EIpsSetDataExtSetOpenHtmlMail
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound(
            EIpsSetDataExtSetOpenHtmlMail, 
            KIpsSetExtDefaultOpenHtmlMail, value);
    aExtendedMailSettings.SetOpenHtmlMail( value );

    // EIpsSetDataAoOnlineState
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound( 
            EIpsSetDataAoOnlineState, KIpsSetExtDefaultAlwaysOnlineState
            ,value );
    aExtendedMailSettings.SetAlwaysOnlineState(
        static_cast<TIpsSetDataAoStates>( value ) );
    
    // EIpsSetDataAoWeekDays
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound( 
            EIpsSetDataAoWeekDays, KIpsSetExtDefaultSelectedWeekDays
            ,value );
    aExtendedMailSettings.SetSelectedWeekDays( value );
    
    // EIpsSetDataAoTimeStartH, EIpsSetDataAoTimeStartL
    //
    TInt64 value64 = 0;
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound(
        EIpsSetDataAoTimeStartH, EIpsSetDataAoTimeStartL,
        KIpsSetExtDefaultSelectedTimeStart, value64 );
    aExtendedMailSettings.SetSelectedTimeStart( value64 );
    
    // EIpsSetDataAoTimeStopH, EIpsSetDataAoTimeStopL
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound(
        EIpsSetDataAoTimeStopH, EIpsSetDataAoTimeStopL,
        KIpsSetExtDefaultSelectedTimeStop, value64 );
    aExtendedMailSettings.SetSelectedTimeStop( value64 );
    
    // EIpsSetDataAoInboxRefreshTime
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound( 
            EIpsSetDataAoInboxRefreshTime, 
            KIpsSetExtDefaultInboxRefreshTime ,value );
    aExtendedMailSettings.SetInboxRefreshTime( value );
    
    // EIpsSetDataAoUpdateMode
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound(
            EIpsSetDataAoUpdateMode, 
            KIpsSetExtDefaultUpdateMode, value );
    aExtendedMailSettings.SetUpdateMode(
        static_cast<TIpsSetDataAoUpdateModes>( value ) );

    TAOInfo aoInfo;
    // Load the last update info
    
    // EIpsSetDataAoLastSuccessfulUpdateH, EIpsSetDataAoLastSuccessfulUpdateL
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound(
        EIpsSetDataAoLastSuccessfulUpdateH, EIpsSetDataAoLastSuccessfulUpdateL,
        KIpsSetExtDefaultTAOInfoLastSuccessfulUpdate , value64 );
    aoInfo.iLastSuccessfulUpdate = value64;
    
    // EIpsSetDataAoLastUpdateFailed
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound(
        EIpsSetDataAoLastUpdateFailed, 
        KIpsSetExtDefaultTAOInfoLastUpdateFailed, result ); 
    aoInfo.iLastUpdateFailed = result;
    
    // EIpsSetDataAoUpdateSuccessfulWithCurSettings
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound(
        EIpsSetDataAoUpdateSuccessfulWithCurSettings, 
        KIpsSetExtDefaultTAOInfoUpdateSuccessfulWithCurSettings, result );
    aoInfo.iUpdateSuccessfulWithCurSettings = result;
   
    aExtendedMailSettings.SetLastUpdateInfo( aoInfo );

    // EIpsSetDataFsOutgoingLogin
    //
    value = KIpsSetExtDefaultOutgoingLogin;
    error = iCenRepControl->GetFromAccount( 
            EIpsSetDataFsOutgoingLogin, value );
    if ( error == KErrNotFound )
        {
        TRAP_IGNORE( value = SolveOutgoingLoginMethodL(
                aAccountId, aMtmId ) );
        TRAP_IGNORE( iCenRepControl->CreateToAccountL( 
                EIpsSetDataFsOutgoingLogin, value ) );
        }
    aExtendedMailSettings.SetOutgoingLogin( value );
    
    // EIpsSetDataLastModifiedH, EIpsSetDataLastModifiedL
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound(
        EIpsSetDataLastModifiedH, EIpsSetDataLastModifiedL,
        KIpsSetExtDefaultLastModified, value64 );
    aExtendedMailSettings.SetLastModified( value64 );
    
    // EIpsSetDataLastSyncStatus
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound( 
            EIpsSetDataLastSyncStatus,
            KIpsSetExtDefaultSyncStatus, value );
    aExtendedMailSettings.SetSyncStatus( value );
    
    //<cmail>
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound( 
        EIpsSetDataHideUserNameAndAddress,
        KIpsSetExtDefaultHiddenData, result );
    aExtendedMailSettings.SetDataHidden( result );
    //</cmail>

    // EIpsSetDateEmnReceivedButNotSyncedFlag
    //
    error = iCenRepControl->GetFromAccountAndCreateIfNotFound( 
        EIpsSetDateEmnReceivedButNotSyncedFlag, 
        KIpsSetExtDefaultEmnReceivedButNotSyncedFlag, value );
    aExtendedMailSettings.SetEmnReceivedButNotSyncedFlag( value );
	
	error = iCenRepControl->GetFromAccountAndCreateIfNotFound( 
            EIpsSetDataFirstEMNReceivedFlag, 
            KIpsSetExtDefaultFirstEMNReceived ,value );
    aExtendedMailSettings.SetFirstEmnReceived( value );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CEmailAccounts* CIpsSetDataStorer::AccountsL()
    {
    FUNC_LOG;
    if ( !iAccounts )
        {
        iAccounts = CEmailAccounts::NewL();
        }

    return iAccounts;
    }

// ----------------------------------------------------------------------------
// function sets EIpsSetDataFsOutgoingLogin setting to correct value if
// it not set in cenrep (situation with old imum mailboxes)
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataStorer::SolveOutgoingLoginMethodL(
    const TUint32 aAccountId,
    const TUid& aMtmId )
    {
    FUNC_LOG;
    TInt ret = CIpsSetData::EOwnOutgoing;
    TMsvId serviceId = 0;
    TMsvId smtpId = 0;
    IpsSetUtils::GetServiceIdFromAccountIdL(
        aAccountId, aMtmId, serviceId, smtpId );
    
    TBuf8<KIpsSetUiMaxPasswordLength> incPass;
    TBuf8<KIpsSetUiMaxPasswordLength> incUser;
    TBuf8<KIpsSetUiMaxPasswordLength> outPass;
    TBuf8<KIpsSetUiMaxPasswordLength> outUser;
    
    CEmailAccounts* accounts = CEmailAccounts::NewLC( );
    if ( aMtmId == KSenduiMtmPop3Uid )
        {
        TPopAccount popAccount;
        accounts->GetPopAccountL( serviceId, popAccount );
        CImPop3Settings* popSettings = new ( ELeave ) CImPop3Settings;
        CleanupStack::PushL( popSettings );
        accounts->LoadPopSettingsL( popAccount, *popSettings );
        incUser.Copy( popSettings->LoginName() );
        incPass.Copy( popSettings->Password() );
        CleanupStack::PopAndDestroy( popSettings );
        popSettings = NULL;
        }
    else if ( aMtmId == KSenduiMtmImap4Uid )
        {
        TImapAccount imapAccount;
        accounts->GetImapAccountL( serviceId, imapAccount );
        CImImap4Settings* imapSettings = new ( ELeave ) CImImap4Settings;
        CleanupStack::PushL( imapSettings );
        accounts->LoadImapSettingsL( imapAccount, *imapSettings );
        incUser.Copy( imapSettings->LoginName() );
        incPass.Copy( imapSettings->Password() );
        CleanupStack::PopAndDestroy( imapSettings );
        imapSettings = NULL;
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    TSmtpAccount smtpAccount;
    accounts->GetSmtpAccountL( smtpId, smtpAccount );
    CImSmtpSettings* smtpSettings = new ( ELeave ) CImSmtpSettings;
    CleanupStack::PushL( smtpSettings );
    accounts->LoadSmtpSettingsL( smtpAccount, *smtpSettings );
    outUser.Copy( smtpSettings->LoginName() );
    outPass.Copy( smtpSettings->Password() );
    CleanupStack::PopAndDestroy( 2, accounts );
    smtpSettings = NULL;
    accounts = NULL;
    
    if ( outUser.Length() == 0 && outPass.Length() == 0 )
        {
        // if outgoing pass and username missing,
        // set value to CIpsSetData::ENoAuth
        ret = CIpsSetData::ENoAuth;
        }
    else if ( outUser.Compare(incUser) == 0 && outPass.Compare(incPass) == 0 )
        {
        // if outgoing loging are same as incoming
        // from smtp settings,
        // then use value CIpsSetData::EUseIncoming
        ret = CIpsSetData::EUseIncoming;
        }
    else
        {
        // if outgoing pass and username are same as incoming,
        // use value CIpsSetData::EOwnOutgoing
        ret = CIpsSetData::EOwnOutgoing;
        }
    return ret;
    }

//  End of File

