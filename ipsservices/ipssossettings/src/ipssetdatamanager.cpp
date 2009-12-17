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
*  Description:
*       Encapsulates saving and removing email accounts
*  Version     : %version: 12.1.2 %
*
*/

#include "emailtrace.h"
#include <cemailaccounts.h>             // CEmailAccounts
#include <imapset.h>                    // CImImap4Settings
#include <SendUiConsts.h>
#include <e32property.h>

#include "ipssetdatamanager.h"
#include "ipssetdata.h"
// <cmail>
#include "ipssetutilsconsts.h"
// </cmail>
#include "ipssetdatastorer.h"
#include "ipssetutils.h"
#include "ipssetdatasignature.h"
#include "ipssetdataapi.h"
#include "ipsplgpropertywatcher.h"
#include "ipsplgcommon.h"
#include "CFSMailCommon.h"


const TInt KIpsSetManagerMaxCreTry = 10;

/* #ifdef _DEBUG <cmail>
_LIT( KMailboxCreationFailed, "IpsSettings - mbox creation failed");
#endif </cmail>*/

// <cmail>
const TInt KMaxBodyFetchSize = 999;
// </cmail>

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetDataManager::CIpsSetDataManager()
// ----------------------------------------------------------------------------
//
CIpsSetDataManager::CIpsSetDataManager(
    CMsvSession& aSession )
    :
    iSession( aSession ),
    iStorer( NULL ),
    iAccounts( NULL )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::ConstructL()
    {
    FUNC_LOG;
    iAccounts = CEmailAccounts::NewL();
    iStorer = CIpsSetDataStorer::NewL();
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::~CIpsSetDataManager()
// ----------------------------------------------------------------------------
//
CIpsSetDataManager::~CIpsSetDataManager()
    {
    FUNC_LOG;
    delete iStorer;
    delete iAccounts;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetDataManager* CIpsSetDataManager::NewL(
    CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSetDataManager* self = NewLC( aSession );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetDataManager* CIpsSetDataManager::NewLC(
    CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSetDataManager* self =
        new ( ELeave ) CIpsSetDataManager( aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/******************************************************************************

    Manager tools

******************************************************************************/


// ----------------------------------------------------------------------------
// CIpsSetDataManager::RemoveAccountL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::RemoveAccountL(
    const TMsvEntry& aMailbox )
    {
    FUNC_LOG;
    // First remove the extended settings, as this cannot leave
    TInt error = iStorer->DeleteExtendedSettingsL( aMailbox );

    // Cleanup all global folders that might have entries belonging to the 
    CleanGlobalFoldersL( aMailbox );
    
    // Determine the account type
    if ( aMailbox.iMtm == KSenduiMtmImap4Uid )
        {
        // Remove the Imap4 account
        TImapAccount imap;
        iAccounts->GetImapAccountL( aMailbox.iServiceId, imap);
        iAccounts->DeleteImapAccountL( imap );
        }
    else
        {
        // Remove the Pop3 account
        TPopAccount pop;
        iAccounts->GetPopAccountL( aMailbox.iServiceId, pop);
        iAccounts->DeletePopAccountL( pop );
        }
    TSmtpAccount smtp;
    iAccounts->GetSmtpAccountL( aMailbox.iRelatedId, smtp);
    iAccounts->DeleteSmtpAccountL( smtp );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::CleanGlobalFoldersL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::CleanGlobalFoldersL( const TMsvEntry& aMailbox )
    {
    FUNC_LOG;
    CleanGlobalFolderL( KMsvGlobalOutBoxIndexEntryId, aMailbox );
    CleanGlobalFolderL( KMsvDraftEntryId, aMailbox );
    CleanGlobalFolderL( KMsvSentEntryId, aMailbox );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::CleanGlobalFolderL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::CleanGlobalFolderL( const TMsvId& aFolderId, const TMsvEntry& aMailbox )
    {
    FUNC_LOG;
    // delete possible entries from this folder that are related to the given mailbox
    CMsvEntry* cEntry = iSession.GetEntryL( aFolderId );
    CleanupStack::PushL( cEntry );
    CMsvEntrySelection* selection = cEntry->ChildrenL();
    CleanupStack::PushL( selection );
    for ( TInt ii = 0; ii < selection->Count(); ii++ )
        {
        TMsvEntry tEntry;
        TMsvId service;
        User::LeaveIfError( iSession.GetEntry( selection->At(ii), service, tEntry ) );
        if( tEntry.iServiceId == aMailbox.iRelatedId )
            {
            iSession.RemoveEntry( selection->At(ii) );
            }
        }
    CleanupStack::PopAndDestroy( selection );
    CleanupStack::PopAndDestroy( cEntry );
    }

/******************************************************************************

    Account creation

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetDataManager::CreateEmailAccount()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataManager::CreateEmailAccount(
    CIpsSetData&  aSettings ,
    const TBool aPopulateDefaults )
    {
    FUNC_LOG;
    TInt error = KErrNone;
    // If settings not ready, don't try to create
    if ( aSettings.IsOk() )
        {
        TBool creReady = EFalse;
        for ( TInt i = 0; i < KIpsSetManagerMaxCreTry && !creReady; i++ )
            {
        // Attempt to create the mailaccount
            error = KErrNone;
            TRAP( error, DoCreateEmailAccountL( 
                    aSettings, aPopulateDefaults ) );

            if ( error == KErrNone )
                {
                creReady = VerifyMailboxCreation( aSettings );
                }
            else
                {
                CleanInvalidMailbox( aSettings );
                }
            }
        if ( error == KErrNone && !creReady )
            {
            // mailbox not good, set return error;
            error = KErrGeneral;
            }
        // Creating mailbox has failed, delete entries and cenrep settings
        if ( error != KErrNone )
            {
            CleanInvalidMailbox( aSettings );
            }
        }
    return error;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::CleanInvalidMailbox()
// ----------------------------------------------------------------------------
//
TBool CIpsSetDataManager::VerifyMailboxCreation( const CIpsSetData& aSettings )
                {
    FUNC_LOG;
    TInt error = KErrNotFound;

    TMsvId mboxId = 0;
    TMsvId smtpId = 0;
    TUid mtmId = aSettings.Protocol();
    if ( mtmId == KSenduiMtmImap4Uid )
        {
        mboxId = aSettings.ImapAccount().iImapService;
        smtpId = aSettings.ImapAccount().iSmtpService;
                }
            else
                {
        mboxId = aSettings.PopAccount().iPopService;
        smtpId = aSettings.PopAccount().iSmtpService;
        }
    
    
                TMsvEntry mboxEntry;
    TMsvEntry smtpEntry;
    TMsvId dummy;
    
    error = iSession.GetEntry( mboxId, dummy, mboxEntry );
    
    if ( error == KErrNone )
        {
        error = iSession.GetEntry( smtpId, dummy, smtpEntry );
                }
    
    if ( error == KErrNone )
        {
        // check that related id is set
        if ( mboxEntry.iRelatedId != smtpEntry.Id() )
            {
            TRAP( error, SetRelatedIdL( mboxEntry.Id(), smtpEntry.Id() ) );
            }
        if ( error == KErrNone && 
                smtpEntry.iRelatedId != mboxEntry.Id() )
            {
            TRAP( error, SetRelatedIdL( smtpEntry.Id(), mboxEntry.Id() ) );
        }
        }

    TBool ret = EFalse;
    if ( error == KErrNone )
        {
        ret = ETrue;
        }
    else
        {
        CleanInvalidMailbox( aSettings );
        ret = EFalse;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::SetRelatedIdL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::SetRelatedIdL( TMsvId aEntryId, TMsvId aRelatedId )
    {
    FUNC_LOG;
    CMsvEntry* centry = iSession.GetEntryL( aEntryId );
    CleanupStack::PushL( centry );
    
    TMsvEntry tentry = centry->Entry();
    tentry.iRelatedId = aRelatedId;
    centry->ChangeL( tentry );
    
    CleanupStack::PopAndDestroy( centry );
    centry = NULL;
    
    // make sure that id is really changed
    TMsvId dummy;
    TInt error = iSession.GetEntry( aEntryId, dummy, tentry );
    if ( error != KErrNone || tentry.iRelatedId != aRelatedId )
        {
        User::Leave( KErrNotFound );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::CleanInvalidMailbox()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::CleanInvalidMailbox( const CIpsSetData& aSettings )
    {
    FUNC_LOG;
    // Determine the account type
    TMsvId mboxId = 0;
    TUid mtmId = aSettings.Protocol();
    // <cmail>TUint32 accountId = 0;</cmail>
    if ( mtmId == KSenduiMtmImap4Uid )
        {
        // Remove the Imap4 account
        TImapAccount imap = aSettings.ImapAccount();
        mboxId = imap.iImapService;
        // <cmail>accountId = imap.iImapAccountId;</cmail>
        TRAP_IGNORE( iAccounts->DeleteImapAccountL( imap ) );
        }
    else
        {
        // Remove the Pop3 account
        TPopAccount pop = aSettings.PopAccount();
        mboxId = pop.iPopService;
        // <cmail>accountId = pop.iPopAccountId;</cmail>
        TRAP_IGNORE( iAccounts->DeletePopAccountL( pop ) );
        }
    TRAP_IGNORE( TInt error = iStorer->DeleteExtendedSettingsL(
        mboxId, mtmId ) );
    
    TRAP_IGNORE( iAccounts->DeleteSmtpAccountL( aSettings.SmtpAccount() ) ); 
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::DoCreateEmailAccountL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::DoCreateEmailAccountL(
    CIpsSetData& aSettings,
    const TBool aPopulateDefaults )
    {
    FUNC_LOG;
    TUint32 id = 0;
    TUid mtmId;
    TMsvId mailboxId;

    // Check for imap4
    if ( aSettings.Protocol() == KSenduiMtmImap4Uid )
        {
        // Catch all the problems with account creation
        id = CreateImap4AccountL( aSettings, aPopulateDefaults );
        mtmId = KSenduiMtmImap4Uid;
        mailboxId = aSettings.ImapAccount().iImapService;
        }
    else
        {
        id = CreatePop3AccountL( aSettings, aPopulateDefaults );
        mtmId = KSenduiMtmPop3Uid;
        mailboxId = aSettings.PopAccount().iPopService;
        }
    CreateExtendedSettingsL( mailboxId, id, mtmId, aSettings );
        
    TRAPD( signerr, StoreSignatureL( aSettings ) );
    if ( signerr != KErrNone )
        {
        }
    StoreIMEIToMailbox( mailboxId );
    
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::CreateImap4AccountL()
// ----------------------------------------------------------------------------
//
TUint32 CIpsSetDataManager::CreateImap4AccountL(
    CIpsSetData& aSettings,
    const TBool aPopulateDefaults,
    const TBool aReadOnly )
    {
    FUNC_LOG;
    // Populate defaults
    if ( aPopulateDefaults )
        {
        iAccounts->PopulateDefaultImapSettingsL(
            *aSettings.Imap4Settings(), *aSettings.IncomingIapPref() );
        iAccounts->PopulateDefaultSmtpSettingsL(
            *aSettings.SmtpSettings(), *aSettings.OutgoingIapPref() );
        }

    aSettings.Imap4Settings()->SetPathSeparator( '/' );
    aSettings.Imap4Settings()->SetUpdatingSeenFlags(ETrue);

    // Create the account
    aSettings.ImapAccount() = iAccounts->CreateImapAccountL(
        aSettings.MailboxName(), 
        *aSettings.Imap4Settings(), *aSettings.IncomingIapPref(), aReadOnly );
    aSettings.SmtpAccount() = iAccounts->CreateSmtpAccountL( 
        aSettings.ImapAccount(),
        *aSettings.SmtpSettings(), *aSettings.OutgoingIapPref(), aReadOnly );

    return aSettings.ImapAccount().iImapAccountId;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::CreatePop3AccountL()
// ----------------------------------------------------------------------------
//
TUint32 CIpsSetDataManager::CreatePop3AccountL(
    CIpsSetData& aSettings,
    const TBool aPopulateDefaults,
    const TBool aReadOnly )
    {
    FUNC_LOG;
    // Populate defaults
    if ( aPopulateDefaults )
        {
        iAccounts->PopulateDefaultPopSettingsL(
            *aSettings.Pop3Settings(), *aSettings.IncomingIapPref() );
        iAccounts->PopulateDefaultSmtpSettingsL(
            *aSettings.SmtpSettings(), *aSettings.OutgoingIapPref() );
        }

    // Create the account
    aSettings.PopAccount() = iAccounts->CreatePopAccountL(
        aSettings.MailboxName(),
        *aSettings.Pop3Settings(), *aSettings.IncomingIapPref(), aReadOnly );
    aSettings.SmtpAccount() = iAccounts->CreateSmtpAccountL( 
        aSettings.PopAccount(),
        *aSettings.SmtpSettings(), *aSettings.OutgoingIapPref(), aReadOnly );
    return aSettings.PopAccount().iPopAccountId;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::CreateExtendedSettingsL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::CreateExtendedSettingsL(
    const TMsvId aMailboxId,
    const TUint32 aAccountId,
    const TUid& aProtocol,
    CIpsSetData& aSettings,
    const TBool aPopulateDefaults )
    {
    FUNC_LOG;
    // If populating has been successful
    User::LeaveIfError( iStorer->CreateExtendedSettings(
        aMailboxId, aAccountId, aProtocol,
        *aSettings.ExtendedSettings(), aPopulateDefaults ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::StoreIMEIToMailbox()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataManager::StoreIMEIToMailbox(
    const TMsvId aMailboxId ) const
    {
    FUNC_LOG;
    // Get available mailbox entries
    TMsvEntry entry;    
    TInt result = IpsSetUtils::GetMailboxEntry( iSession, aMailboxId, entry );
    
    // Imap4 & Pop3 & Smtp protocols contain two entries
    if ( result == KErrNone )
        {
        // Attempt to store the IMEI code to the entry
        result = StoreIMEIToEntry( entry.Id() );       
        }

    // Imap4 / Pop3 protocols have two different entries
    if ( IpsSetUtils::IsMailMtm( entry.iMtm ) )
        {
        TMsvEntry secondaryEntry;
        result = IpsSetUtils::GetMailboxEntry( iSession, entry.iRelatedId, entry );
            
        if ( result == KErrNone ) 
            {
            // Attempt to store the IMEI code to the entry
            result = StoreIMEIToEntry( entry.Id() );
            }            
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::StoreIMEIToEntry()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataManager::StoreIMEIToEntry( const TMsvId aMailboxId ) const
    {
    FUNC_LOG;
    TRAPD( error, GetAndStoreIMEIToEntryL( aMailboxId ) );
    return ( error == KErrNone ) ? KErrNone : KErrGeneral;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::GetAndStoreIMEIToEntryL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::GetAndStoreIMEIToEntryL(
    const TMsvId aMailboxId ) const
    {
    FUNC_LOG;
    TBuf<KIMAMaxPhoneIdLength> imei;
    
    CIpsSetDataApi* setApi = CIpsSetDataApi::NewL( iSession );
    CleanupStack::PushL( setApi );
    
    setApi->GetIMEIFromThePhoneL( imei );
    
    CMsvEntry* centry = iSession.GetEntryL( aMailboxId );
    CleanupStack::PushL( centry );

    TMsvEntry tentry = centry->Entry();
    tentry.iDescription.Set( imei );
    centry->ChangeL( tentry );

    CleanupStack::PopAndDestroy( 2, setApi );
    centry = NULL;
    }

/******************************************************************************

    Account Saving

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetDataManager::SaveEmailSettingsL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetDataManager::SaveEmailSettingsL(
    CIpsSetData& aSettings )
    {
    FUNC_LOG;
    // If settings not ready, don't try to create
    if ( !aSettings.IsOk() )
        {
        return KErrNotFound;
        }

    TInt error = KErrNone;
    TBool isImap = ( aSettings.Protocol() == KSenduiMtmImap4Uid ); 
    // Check for imap4
    if ( isImap )
        {
        // Catch all the problems with account creation
        TRAP( error, SaveImap4SettingsL( aSettings ) );
        }
    // It's a pop
    else
        {
        // Catch all the problems with account creation
        TRAP( error, SavePop3SettingsL( aSettings ) );
        }

    // Attempt to save the extended settings, even if other settings have
    // failed to save
    error = iStorer->SaveExtendedSettings( *aSettings.ExtendedSettings() );
    TRAP_IGNORE( StoreSignatureL( aSettings ) );

    //settings have been saved, signal email plugins
    SendPropertyEventL( (isImap ? 
        aSettings.ImapAccount().iImapService : 
        aSettings.PopAccount().iPopService), isImap );
    return error;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::SavePop3SettingsL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::SavePop3SettingsL(
    CIpsSetData& aSettings )
    {
    FUNC_LOG;
    // Store the mailbox name
    aSettings.PopAccount().iPopAccountName.Copy(
        aSettings.MailboxName() );
    aSettings.SmtpAccount().iSmtpAccountName.Copy(
        aSettings.MailboxName() );

    // Load mailbox settings
    iAccounts->SavePopSettingsL(
        aSettings.PopAccount(), *aSettings.Pop3Settings() );
    iAccounts->SavePopIapSettingsL(
        aSettings.PopAccount(), *aSettings.IncomingIapPref() );
    iAccounts->SaveSmtpSettingsL(
        aSettings.SmtpAccount(), *aSettings.SmtpSettings() );
    iAccounts->SaveSmtpIapSettingsL(
        aSettings.SmtpAccount(), *aSettings.OutgoingIapPref() );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::SaveImap4SettingsL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::SaveImap4SettingsL(
    CIpsSetData& aSettings )
    {
    FUNC_LOG;
    // Store the mailbox name
    aSettings.ImapAccount().iImapAccountName.Copy(
        aSettings.MailboxName() );
    aSettings.SmtpAccount().iSmtpAccountName.Copy(
        aSettings.MailboxName() );

    // Load mailbox settings
    iAccounts->SaveImapSettingsL(
        aSettings.ImapAccount(), *aSettings.Imap4Settings() );
    iAccounts->SaveImapIapSettingsL(
        aSettings.ImapAccount(), *aSettings.IncomingIapPref() );
    iAccounts->SaveSmtpSettingsL(
        aSettings.SmtpAccount(), *aSettings.SmtpSettings() );
    iAccounts->SaveSmtpIapSettingsL(
        aSettings.SmtpAccount(), *aSettings.OutgoingIapPref() );
    }

/******************************************************************************

    Account Loading

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetDataManager::LoadEmailSettingsL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::LoadEmailSettingsL(
    const TMsvEntry& aEntry,
    CIpsSetData& aSettings )
    {
    FUNC_LOG;
    // Get both of the entries
    TMsvEntry smtpEntry;
    TMsvEntry relatedEntry;
    TUid mtmId = IpsSetUtils::GetMailboxEntriesL(
        aEntry, iSession, smtpEntry, relatedEntry );

    // Store the id and protocol
    aSettings.SetProtocol( mtmId );

    // Load settings based on the protocol
    if ( aSettings.Protocol() == KSenduiMtmImap4Uid )
        {
        iAccounts->GetImapAccountL(
            relatedEntry.Id(), aSettings.ImapAccount() );
        LoadImap4SettingsL( aSettings );

        // Load extended settings
        TUint32 accountId = aSettings.ImapAccount().iImapAccountId;
        TInt error = iStorer->LoadExtendedSettings(
            TIpsSetDataStorerParams( accountId, mtmId ),
            *aSettings.ExtendedSettings() );
        }
    else
        {
        iAccounts->GetPopAccountL(
            relatedEntry.Id(), aSettings.PopAccount() );
        LoadPop3SettingsL( aSettings );

        // Load extended settings
        TUint32 accountId = aSettings.PopAccount().iPopAccountId;
        TInt error = iStorer->LoadExtendedSettings(
            TIpsSetDataStorerParams( accountId, mtmId ),
            *aSettings.ExtendedSettings() );
        }

    // Attempt to read the signature
    TRAP_IGNORE( RestoreSignatureL( aSettings ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::LoadPop3SettingsL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::LoadPop3SettingsL(
    CIpsSetData& aSettings )
    {
    FUNC_LOG;
    iAccounts->GetSmtpAccountL(
        aSettings.PopAccount().iSmtpService, aSettings.SmtpAccount() );

    // Load mailbox settings
    iAccounts->LoadPopSettingsL(
        aSettings.PopAccount(), *aSettings.Pop3Settings() );
    iAccounts->LoadPopIapSettingsL(
        aSettings.PopAccount(), *aSettings.IncomingIapPref() );
    iAccounts->LoadSmtpSettingsL(
        aSettings.SmtpAccount(), *aSettings.SmtpSettings() );
    iAccounts->LoadSmtpIapSettingsL(
        aSettings.SmtpAccount(), *aSettings.OutgoingIapPref() );

    aSettings.SetMailboxName( aSettings.PopAccount().iPopAccountName );
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::LoadImap4SettingsL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::LoadImap4SettingsL(
    CIpsSetData& aSettings )
    {
    FUNC_LOG;
    iAccounts->GetSmtpAccountL(
        aSettings.ImapAccount().iSmtpService, aSettings.SmtpAccount() );

    // Load mailbox settings
    iAccounts->LoadImapSettingsL(
        aSettings.ImapAccount(), *aSettings.Imap4Settings() );

    // <cmail>
    TInt szLimit = aSettings.Imap4Settings()->BodyTextSizeLimit();
    
    if( szLimit < KIpsSetDataFullBodyOnly ||
        szLimit > KMaxBodyFetchSize )
        {
        //body size limit value not set properly, so setting a default value.
        aSettings.Imap4Settings()->SetBodyTextSizeLimitL(
                KIpsSetDataDefaultDownloadSizeKb );
        
        iAccounts->SaveImapSettingsL( 
                aSettings.ImapAccount(), *aSettings.Imap4Settings() );
        }
    // </cmail>        

    iAccounts->LoadImapIapSettingsL(
        aSettings.ImapAccount(), *aSettings.IncomingIapPref() );
    iAccounts->LoadSmtpSettingsL(
        aSettings.SmtpAccount(), *aSettings.SmtpSettings() );
    iAccounts->LoadSmtpIapSettingsL(
        aSettings.SmtpAccount(), *aSettings.OutgoingIapPref() );

    aSettings.SetMailboxName( aSettings.ImapAccount().iImapAccountName );
    }

/******************************************************************************

    MISC TOOLS

******************************************************************************/

// ----------------------------------------------------------------------------
// CIpsSetDataManager::StoreSignatureL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::StoreSignatureL(
    const CIpsSetData& aSettings )
    {
    FUNC_LOG;
    // Get Signature
    CMsvEntry* entry = 
        iSession.GetEntryL( aSettings.SmtpAccount().iSmtpService );
    CleanupStack::PushL( entry );
    CMsvStore* msvStore = entry->EditStoreL();
    CleanupStack::PushL( msvStore );

    msvStore->StoreBodyTextL( *( aSettings.Signature().iRichText ) );
    msvStore->CommitL();

    CleanupStack::PopAndDestroy( msvStore );
    msvStore = NULL;
    CleanupStack::PopAndDestroy( entry );
    entry = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataManager::RestoreSignatureL()
// ----------------------------------------------------------------------------
//
void CIpsSetDataManager::RestoreSignatureL(
    CIpsSetData& aSettings )
    {
    FUNC_LOG;
    // Get Signature
    CMsvEntry* entry = 
        iSession.GetEntryL( aSettings.SmtpAccount().iSmtpService );
    CleanupStack::PushL( entry );
    CMsvStore* msvStore = entry->ReadStoreL();
    CleanupStack::PushL( msvStore );

    aSettings.Signature().CreateEmptyRichTextL();
    msvStore->RestoreBodyTextL( *( aSettings.Signature().iRichText ) );

    CleanupStack::PopAndDestroy( msvStore );
    msvStore = NULL;
    CleanupStack::PopAndDestroy( entry );
    entry = NULL;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsSetDataManager::SendPropertyEventL( TInt aMailbox, TBool aIsImap )
    {
    FUNC_LOG;
    TInt error = RProperty::Define( 
            KIpsPlgPropertyCatUid, 
        KIPSSosEventPropertyKey, RProperty::EByteArray,
        KAllowAllPolicy,
        KAllowWriteDeviceDataPolicy );
    
    TUid plugin = (aIsImap ? KIpsPlgImap4PluginUid : KIpsPlgPop3PluginUid);
    TIpsPlgPropertyEvent pEvent(  
            EIPSSosSettingsChanged, 
            aMailbox , 
            plugin.iUid,
            KErrNone );
    
    TPckgBuf<TIpsPlgPropertyEvent> propertyBuf = pEvent;
    
    // if definition was success
    if ( error == KErrNone || error == KErrAlreadyExists || 
	error == KErrPermissionDenied )
        {
        error = RProperty::Set(
                KIpsPlgPropertyCatUid, 
                KIPSSosEventPropertyKey, propertyBuf );
    
        }
    return error;
    }

// End Of File

