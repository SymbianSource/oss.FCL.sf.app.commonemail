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
*  Description: This file implements class CIpsSetDataApi.
*
*/

#include "emailtrace.h"
#include <e32base.h>
#include <e32cmn.h>
#include <SendUiConsts.h>
#include <pop3set.h>
#include <imapset.h>
#include <smtpset.h>
#include <centralrepository.h>      // CRepository
#include <iapprefs.h>
// <cmail> #include <ApUtils.h> // CApUtils
#include <commdb.h>
#include <ipssossettings.rsg>
#include <etel.h>                       // RTelServer
#include <etelmm.h>                     // RMobilePhone
#include <cemailaccounts.h>
#include <txtrich.h>

#include "ipssetdatastorer.h"
#include "ipssetdata.h"
#include "ipssetdatamanager.h"
#include "ipssetdataextension.h"
#include "ipssetdatastorer.h"
#include "ipssetwizardadapter.h"
#include "ipssetui.h"
#include "ipssetdatactrlaccount.h"
#include "ipssetutilsconsts.h"
#include "ipsplgcommon.h"
#include "ipssetdataapi.h"
#include "ipssetwizardadapter.h"

#include "ipssetwizardsettingscenrepkeys.h"
#include "freestyleemailcenrepkeys.h"
#include "ipssetdatasignature.h"

const TInt KIpsDataApiMaxPassLen = 256;
const TInt KWizardDataPopIndicator = 0;
const TInt KWizardDataImapIndicator = 1;

#ifdef __WINS__
_LIT( KIMAEmulatorImei, "123456789012345" );
#endif // __WINS__


#ifdef _DEBUG
_LIT( KIpsSetApi, "IpsSetApi");
#endif
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C CIpsSetDataApi* CIpsSetDataApi::NewL( CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSetDataApi* self = new(ELeave)CIpsSetDataApi( aSession );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSetDataApi::CIpsSetDataApi( CMsvSession& aSession )
    : iSession( aSession ),
    iServerAddress( NULL )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSetDataApi::ConstructL()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
CIpsSetDataApi::~CIpsSetDataApi()
    {
    FUNC_LOG;
    delete iServerAddress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
EXPORT_C TInt CIpsSetDataApi::LaunchSettingsL( const TMsvId aMailboxId )
    {
    FUNC_LOG;
    CIpsSetUi* ui = CIpsSetUi::NewL( aMailboxId );
    TInt ret = ui->ExecuteLD( R_IPS_SET_SETTINGS_DIALOG );

    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
EXPORT_C TDesC& CIpsSetDataApi::GetServerAddressL( const CMsvEntry& aEntry )
    {
    FUNC_LOG;
    delete iServerAddress;
    iServerAddress = NULL;
    
    if( aEntry.Entry().iMtm.iUid == KSenduiMtmImap4UidValue )
        {
        CImImap4Settings* settings = new(ELeave) CImImap4Settings();
        CleanupStack::PushL( settings );
        
        TImapAccount imapAccountId;
        CEmailAccounts* accounts = CEmailAccounts::NewLC();
        accounts->GetImapAccountL( aEntry.Entry().Id(), imapAccountId );
        accounts->LoadImapSettingsL( imapAccountId, *settings );
        
        iServerAddress = HBufC::NewL( settings->ServerAddress().Length() );
        iServerAddress->Des().Copy( settings->ServerAddress() );
        
        CleanupStack::PopAndDestroy( 2, settings );
        }
    else if( aEntry.Entry().iMtm.iUid == KSenduiMtmPop3UidValue )
        {
        CImPop3Settings* settings = new(ELeave) CImPop3Settings();
        CleanupStack::PushL( settings );
        
        TPopAccount popAccountId;
        CEmailAccounts* accounts = CEmailAccounts::NewLC();
        accounts->GetPopAccountL( aEntry.Entry().Id(), popAccountId );
        accounts->LoadPopSettingsL( popAccountId, *settings );
        
        iServerAddress = HBufC::NewL( settings->ServerAddress().Length() );
        iServerAddress->Des().Copy( settings->ServerAddress() );
        
        CleanupStack::PopAndDestroy( 2, settings );
        }
    return *iServerAddress;
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
EXPORT_C void CIpsSetDataApi::SetNewPasswordL( 
    CMsvEntry& aService, 
    const TDesC& aPassword )
    {
    FUNC_LOG;
    if ( aService.Entry().iMtm.iUid == KSenduiMtmPop3UidValue )
        {
        SetNewPop3PasswordL( aService, aPassword );
        }
    else if ( aService.Entry().iMtm.iUid == KSenduiMtmImap4UidValue )
        {
        SetNewImap4PasswordL( aService, aPassword );
        }
    else if ( aService.Entry().iMtm.iUid == KSenduiMtmSmtpUidValue )
        {
        CEmailAccounts* accounts = CEmailAccounts::NewLC();

        TBuf8<KIpsDataApiMaxPassLen> temp;
        temp.Copy( aPassword );

        SetNewSmtpPasswordL( aService.EntryId(), temp, *accounts );
        CleanupStack::PopAndDestroy( 1, accounts );
        }
    }

// ----------------------------------------------------------------------------
// function returns KErrNotSupported if wizard data is not 
// not match for the uid parameter. All leaving function calls are
// trapped to prevent KErrNotSupported error code to return
// in case of real error situation
// ----------------------------------------------------------------------------        
EXPORT_C TInt CIpsSetDataApi::HandleMailboxCreation( 
    const TUid aUid, 
    CMsvSession& aSession )
    {
    FUNC_LOG;
    // NOTE: this function should not leave, CleanupStack not used
    
    _LIT( KFsBasicPluginStr, "fsp" );
    
    TInt error = KErrNone;
    CRepository* repository = NULL;
    CIpsSetWizardAdapter* adapter = NULL;
    TInt protocol = 0; // <cmail>
    TIpsSetUtilsTextPlain accountType;
    
    TRAP( error, repository = CRepository::NewL( KCRUidWizardSettings ) );
    if ( error != KErrNone )
        {
        error = KErrGeneral;
        }
    else
        {
        // protocol 0= POP, 1= IMAP
        error = repository->Get( 
            ECRKSetupWizAccountType, accountType );
        if ( error != KErrNone )
            {
            error = KErrNotSupported;
            }
        }
    
    // check that protocol match until go further...    
    TInt diff = accountType.Compare( KFsBasicPluginStr );
    if ( error == KErrNone && diff != 0 )
        {
        error = KErrNotSupported;
        }
    
    if ( error == KErrNone )
        {
        error = repository->Get( 
            ECRKPopImapProtocolIndicatorId, protocol );
        }
    
    if ( error != KErrNone )
        {
        error = KErrNotSupported;
        }
    else if ( error == KErrNone 
            && protocol == KWizardDataPopIndicator 
            && aUid.iUid == KSenduiMtmImap4UidValue )
        {
        error = KErrNotSupported;
        }
    else if ( error == KErrNone 
            && protocol == KWizardDataImapIndicator 
            && aUid.iUid == KSenduiMtmPop3UidValue )
        {
        error = KErrNotSupported;
        }
    
    if ( error == KErrNone )
        {
        TRAP( error, adapter = CIpsSetWizardAdapter::NewL(
            *repository, aSession ) );
        if ( error != KErrNone )
            {
            error = KErrGeneral;
            }
        }
    if ( error == KErrNone )
        {
        // before creating mailbox clear common wizard fields to prevent another 
        // ips plugin instances to create (dublicate) mailboxes
        TInt setErr = repository->Set( 
                    ECRKSetupWizAccountType, KNullDesC );
        TRAP( error, adapter->HandleWizardDataL() );
        if ( error != KErrNone )
            {
            error = KErrGeneral;
            }
        }

    delete repository;
    delete adapter;
    return error;
    }


    
// ----------------------------------------------------------------------------
// maybe these could be combined...SetNewImap4PasswordL & SetNewPop3PasswordL
// ----------------------------------------------------------------------------        
void CIpsSetDataApi::SetNewImap4PasswordL( 
    CMsvEntry& aService, 
    const TDesC& aPassword )
    {
    FUNC_LOG;
    CImImap4Settings* settings = new(ELeave) CImImap4Settings();
    CleanupStack::PushL( settings );

    TImapAccount imapAccountId;
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    accounts->GetImapAccountL( aService.Entry().Id(), imapAccountId );
    accounts->LoadImapSettingsL( imapAccountId, *settings );
    
    TBuf8<KIpsDataApiMaxPassLen> temp;
    temp.Copy( aPassword );
    settings->SetPasswordL( temp );
    accounts->SaveImapSettingsL( imapAccountId, *settings );
    
    // Possibly update also outgoing service password
    if ( IsOutgoingLoginSameAsIncomingL( 
         TIpsSetDataStorerParams( imapAccountId.iImapAccountId, KSenduiMtmImap4Uid ) ) )
        {
        SetNewSmtpPasswordL( imapAccountId.iSmtpService, temp, *accounts );
        }
    
    CleanupStack::PopAndDestroy( 2, settings );
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
void CIpsSetDataApi::SetNewPop3PasswordL( 
    CMsvEntry& aService, 
    const TDesC& aPassword )
    {
    FUNC_LOG;
    CImPop3Settings* settings = new(ELeave) CImPop3Settings();
    CleanupStack::PushL( settings );
    
    TPopAccount popAccountId;
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    accounts->GetPopAccountL( aService.Entry().Id(), popAccountId );
    
    accounts->LoadPopSettingsL( popAccountId, *settings );
    
    TBuf8<KIpsDataApiMaxPassLen> temp;
    temp.Copy( aPassword );
    
    settings->SetPasswordL( temp );
    
    accounts->SavePopSettingsL( popAccountId, *settings );
    
    // Possibly update also outgoing service password
    if ( IsOutgoingLoginSameAsIncomingL(
         TIpsSetDataStorerParams( popAccountId.iPopAccountId, KSenduiMtmPop3Uid ) ) )
        {
        SetNewSmtpPasswordL( popAccountId.iSmtpService, temp, *accounts );
        }

    CleanupStack::PopAndDestroy( 2, settings );
    }

//<cmail>
//This method is not used anywhere
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------   
//  
/*EXPORT_C TBool CIpsSetDataApi::IsSmtpAllowedToConnectL( TMsvId aSmtpService, TBool aIsRoaming,
                                                        CMsvSession& aSession )
    {
	FUNC_LOG;
    // get correct imap or pop entry from smtp service
    TMsvId serv;
    TMsvEntry entry; 
    TInt error = KErrNone;
    error = aSession.GetEntry( aSmtpService, serv, entry );
    
    if( error == KErrNone )
        {
        error = aSession.GetEntry( entry.iRelatedId, serv, entry );
        }
    // service might be deleted
    if( error != KErrNone )
        {
        return EFalse;
        }
    
    CIpsSetData* setData = CIpsSetData::NewLC();    // << setData
    CIpsSetDataManager* dataMan = CIpsSetDataManager::NewLC( aSession );// << dataMan
    TRAP( error, dataMan->LoadEmailSettingsL( entry, *setData ) );
            
    if( error != KErrNone )
        {
        CleanupStack::PopAndDestroy( 2 ); // >>> setData, dataMan
        return EFalse;
        }
   
    TBool isAllowed( ETrue );
    CIpsSetDataExtension* extented = setData->ExtendedSettings();
    
    if( extented->AlwaysOnlineState() == EMailAoOff )
        {
        //isAllowed = EFalse;
        }
    else if ( aIsRoaming && extented->AlwaysOnlineState() == EMailAoHomeOnly )
        {
        //isAllowed = EFalse;
        }
    
    // Always ask check, do not check in emulator because
    // lan connection works only with always ask access point
#ifndef __WINS__
    CImIAPPreferences* iapOut = setData->OutgoingIapPref();
    TImIAPChoice iapChoice = iapOut->IAPPreference( 0 );
    isAllowed = ( !iapChoice.iIAP == KIpsServiceIdNoServiceSpecified );
#endif
    CleanupStack::PopAndDestroy( 2 );  //>>> setData, dataMan
    return isAllowed;
    }*/
//</cmail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
EXPORT_C void CIpsSetDataApi::GoOnlineL( TMsvEntry /*aMailbox*/ )
    {
    FUNC_LOG;
    // remove this
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
EXPORT_C void CIpsSetDataApi::GoOfflineL( TMsvEntry /*aMailbox*/ )
    {
    FUNC_LOG;
    // remove this
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
EXPORT_C void CIpsSetDataApi::RemoveAccountL( 
    const TMsvEntry& aMailbox, 
    CMsvSession& aSession )
    {
    FUNC_LOG;
    
    // First let the data manager to delete the account.
    CIpsSetDataManager* manager = CIpsSetDataManager::NewL( aSession );
    
    TRAPD( err, manager->RemoveAccountL( aMailbox ) );
    
    delete manager;
    manager = NULL;

    if ( err != KErrNone )
        {
        __ASSERT_DEBUG(EFalse, User::Panic( 
            KIpsSetApi , KErrNotFound ) );
        }
    // Then update the max account number flag.
    CRepository* repository = NULL;

    TRAPD( err2, repository = CRepository::NewL( KCRUidWizardSettings ) );

    // Do not assume repository is always available and working correctly.
    if ( err2 == KErrNone )
        {
        //always reset the max account flag when a mailbox is deleted
        TInt err3 = repository->Set(
            ECRKMaxAccountsReached, EFalse );

       
        delete repository;
        repository = NULL;
        }
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C TUint32 CIpsSetDataApi::CreateCenRepKeyL( 
        const TUint32 aAccountId,
        const TUid& aProtocol,
        const TUint32 aSettingKey )
    {
    FUNC_LOG;
    return IpsSetDataCtrlAccount::CreateSettingKey( 
        aAccountId, 
        aSettingKey, 
        aProtocol );    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C void CIpsSetDataApi::GetMailboxAddressL(
    const TMsvEntry& aMailboxEntry,
    HBufC*& aMailboxAddress )
    {
    FUNC_LOG;
    /*CIpsSetData* settingsData = CIpsSetData::NewLC();
    CIpsSetDataManager* dataManager = CIpsSetDataManager::NewLC( iSession );
    TRAP_IGNORE( 
        dataManager->LoadEmailSettingsL( aMailboxEntry, *settingsData ) );
      
    CIpsSetDataExtension* extentedSettings = settingsData->ExtendedSettings();
    
    aMailboxAddress = extentedSettings->EmailAddress().Alloc();
    CleanupStack::PopAndDestroy( 2, settingsData ); // dataManager   */
    
    CEmailAccounts* acc = CEmailAccounts::NewLC();
    TSmtpAccount smtpAcc;
    acc->GetSmtpAccountL( aMailboxEntry.iRelatedId , smtpAcc );
    CImSmtpSettings* smtpSet = new (ELeave) CImSmtpSettings();
    CleanupStack::PushL( smtpSet );
    acc->LoadSmtpSettingsL( smtpAcc, *smtpSet );
    aMailboxAddress = smtpSet->EmailAddress().AllocL();
    CleanupStack::PopAndDestroy( 2, acc );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C void CIpsSetDataApi::SaveSyncStatusL( TMsvEntry aMailbox, TInt aState )
    {
    FUNC_LOG;
    CIpsSetDataStorer* storer = CIpsSetDataStorer::NewLC();
    CIpsSetData* settingsData = CIpsSetData::NewLC();
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TUint32 accountId;
    
    if( aMailbox.iMtm.iUid == KSenduiMtmImap4UidValue )
    	{
    	accounts->GetImapAccountL( aMailbox.Id(), settingsData->ImapAccount() );
    	accountId = settingsData->ImapAccount().iImapAccountId;
    	}
    else
    	{
    	accounts->GetPopAccountL( aMailbox.Id(), settingsData->PopAccount() );
    	accountId = settingsData->PopAccount().iPopAccountId;
    	}
    
    TInt error = storer->LoadExtendedSettings( 
    		TIpsSetDataStorerParams( accountId, aMailbox.iMtm ),
    		*settingsData->ExtendedSettings() );

    settingsData->ExtendedSettings()->SetSyncStatus( aState );
    
    error = storer->SaveExtendedSettings( *settingsData->ExtendedSettings() );
    
    CleanupStack::PopAndDestroy( 3, storer );
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
EXPORT_C TInt CIpsSetDataApi::GetLastSyncStatusL( TMsvEntry aMailbox )
    {
    FUNC_LOG;
    CIpsSetDataStorer* storer = CIpsSetDataStorer::NewLC();
    CIpsSetData* settingsData = CIpsSetData::NewLC();
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TUint32 accountId;
    
    if( aMailbox.iMtm.iUid == KSenduiMtmImap4UidValue )
    	{
    	accounts->GetImapAccountL( aMailbox.Id(), settingsData->ImapAccount() );
    	accountId = settingsData->ImapAccount().iImapAccountId;
    	}
    else
    	{
    	accounts->GetPopAccountL( aMailbox.Id(), settingsData->PopAccount() );
    	accountId = settingsData->PopAccount().iPopAccountId;
    	}
    
    TInt error = storer->LoadExtendedSettings( 
    		TIpsSetDataStorerParams( accountId, aMailbox.iMtm ),
    		*settingsData->ExtendedSettings() );
    
    TInt lastSyncStatus = settingsData->ExtendedSettings()->LastSyncStatus( );
    
    CleanupStack::PopAndDestroy( 3, storer );
    
    return lastSyncStatus;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C void CIpsSetDataApi::GetIMEIFromThePhoneL( TBuf<KIpsSetDataMaxPhoneIdLength> &aIMEI )
    {
    FUNC_LOG;
    // In Emulator version, the imei cannot be fetched, so the basic number
    // is used instead
    #ifndef __WINS__

        RTelServer telServer;
        User::LeaveIfError( telServer.Connect() );
        CleanupClosePushL(telServer);

        TInt numPhones = 0;
        User::LeaveIfError( telServer.EnumeratePhones( numPhones ) );
        if ( numPhones < 1 )
            {
            User::Leave( KErrNotFound );
            }
        RTelServer::TPhoneInfo info;
        User::LeaveIfError( telServer.GetPhoneInfo( 0, info ) );
        RMobilePhone mobilePhone;
        User::LeaveIfError( mobilePhone.Open( telServer, info.iName ) );
        CleanupClosePushL( mobilePhone );

        TUint32 identityCaps;
        User::LeaveIfError( mobilePhone.GetIdentityCaps( identityCaps ) );

        if ( identityCaps & RMobilePhone::KCapsGetSerialNumber )
            {
            TRequestStatus status;
            RMobilePhone::TMobilePhoneIdentityV1 mobilePhoneIdentity;

            mobilePhone.GetPhoneId( status, mobilePhoneIdentity );

            User::WaitForRequest( status );
            User::LeaveIfError( status.Int() );

            aIMEI.Copy( mobilePhoneIdentity.iSerialNumber );
            }
        else
            {
            User::Leave( KErrNotSupported );
            }

        CleanupStack::PopAndDestroy( &mobilePhone );
        CleanupStack::PopAndDestroy( &telServer );
    #else
        aIMEI.Copy( KIMAEmulatorImei );
    #endif // __WINS__
    }
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C void CIpsSetDataApi::LoadExtendedSettingsL(
        TMsvId aServiceId,
        CIpsSetDataExtension& aExtendedSettings )
    {
    FUNC_LOG;
    TMsvEntry mboxEntry;
    TMsvId dummy;
    User::LeaveIfError( iSession.GetEntry( aServiceId, dummy, mboxEntry ) );
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TInt accountId( KErrNotFound );
    
    if ( mboxEntry.iMtm.iUid == KSenduiMtmImap4UidValue )
        {
        TImapAccount imapAccount;
        accounts->GetImapAccountL( mboxEntry.Id(), imapAccount );
        accountId = imapAccount.iImapAccountId;
        }
    else if ( mboxEntry.iMtm.iUid == KSenduiMtmPop3UidValue )
        {
        TPopAccount popAccount;
        accounts->GetPopAccountL( mboxEntry.Id(), popAccount );
        accountId = popAccount.iPopAccountId;
        }
    else
        {
        __ASSERT_DEBUG(EFalse, User::Panic( 
            KIpsSetApi , KErrNotFound ) );
        }
    
    CIpsSetDataStorer* storer = CIpsSetDataStorer::NewLC();
    storer->LoadExtendedSettingsL(
            accountId, mboxEntry.iMtm, aExtendedSettings );
    
    CleanupStack::PopAndDestroy( 2, accounts );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C void CIpsSetDataApi::SaveExtendedSettingsL(
        const CIpsSetDataExtension& aExtendedSettings )
    {
    FUNC_LOG;
    CIpsSetDataStorer* storer = CIpsSetDataStorer::NewLC();
    storer->SaveExtendedSettingsL( aExtendedSettings );
    
    CleanupStack::PopAndDestroy( storer );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C void CIpsSetDataApi::ConstructImapPartialFetchInfo( 
        TImImap4GetPartialMailInfo& aInfo, CImImap4Settings& aImap4Settings )

    {
    FUNC_LOG;
    TInt sizeLimit = aImap4Settings.BodyTextSizeLimit();
    
    TInt limit(KMaxTInt);
    CRepository* centRep = NULL;
    TRAP_IGNORE( centRep = CRepository::NewL( KFreestyleEmailCenRep ) );
    if (NULL != centRep)
        {
        //not necessary to push centRep as no leave function is called
        TInt err = centRep->Get(KFreestyleMaxBodySize, limit);
        limit *= KKilo;
        if (limit == 0 || err)
            {
            limit = KMaxTInt;
            }
        //free
        delete centRep;
        centRep = NULL;		
        }//end if (NULL != centRep)

    if ( sizeLimit == KIpsSetDataHeadersOnly )
        {
        aInfo.iTotalSizeLimit = KIpsSetDataHeadersOnly;
        }
    else if ( sizeLimit == KIpsSetDataFullBodyAndAttas )
        {        
        aInfo.iTotalSizeLimit = KMaxTInt;
        aInfo.iAttachmentSizeLimit = KMaxTInt;
        aInfo.iBodyTextSizeLimit = limit;
        aInfo.iMaxEmailSize = KMaxTInt;
        aInfo.iPartialMailOptions = ENoSizeLimits;
        aInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
        }
    else if ( sizeLimit == KIpsSetDataFullBodyOnly )
        {
        aInfo.iTotalSizeLimit = KMaxTInt; 
        aInfo.iAttachmentSizeLimit = 0;
        aInfo.iBodyTextSizeLimit = limit;
        aInfo.iMaxEmailSize = KMaxTInt;
        aInfo.iPartialMailOptions = EBodyAlternativeText;
        aInfo.iGetMailBodyParts = EGetImap4EmailBodyAlternativeText;
        }
    else
        {
        aInfo.iTotalSizeLimit = sizeLimit*1024; 
        // set zero when it not documentated does total size overrides these 
        aInfo.iAttachmentSizeLimit = 0;
        aInfo.iMaxEmailSize = sizeLimit*1024;
        aInfo.iBodyTextSizeLimit = Min(sizeLimit*1024, limit);
        aInfo.iPartialMailOptions = EBodyAlternativeText;
        aInfo.iGetMailBodyParts = EGetImap4EmailBodyAlternativeText;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C void CIpsSetDataApi::GetSubscribedImapFoldersL( 
        TMsvId aServiceId, RArray<TMsvId>& aFoldersArray )
    {
    FUNC_LOG;
    CMsvEntry* cEntry = iSession.GetEntryL( aServiceId );
    CleanupStack::PushL( cEntry );
    CMsvEntrySelection* foldersUnderService = cEntry->ChildrenWithTypeL( 
            KUidMsvFolderEntry );
    CleanupStack::PushL( foldersUnderService );
    CIpsSetDataApi::GetImapChildFoldersL( 
            iSession, *foldersUnderService, aFoldersArray );
    CleanupStack::PopAndDestroy( foldersUnderService );
    CleanupStack::PopAndDestroy( cEntry );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C void CIpsSetDataApi::SetMailboxNameL( 
        TMsvEntry& aService, 
        const TDesC& aMailboxName)
    {
    FUNC_LOG;
    CIpsSetData* settingsData = CIpsSetData::NewLC();
    CIpsSetDataManager* dataManager = CIpsSetDataManager::NewLC( iSession );
    TRAP_IGNORE( 
        dataManager->LoadEmailSettingsL( aService, *settingsData ) );
    
    settingsData->SetMailboxName( aMailboxName );
    
    TRAP_IGNORE( dataManager->SaveEmailSettingsL( *settingsData ) );
    
    CleanupStack::PopAndDestroy( 2, settingsData ); // dataManager
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
EXPORT_C HBufC* CIpsSetDataApi::SignatureTextL( const TMsvEntry& aService )
    {
    FUNC_LOG;
    HBufC* signatureText( NULL );
    CIpsSetData* settingsData = CIpsSetData::NewLC();
    CIpsSetDataManager* dataManager = CIpsSetDataManager::NewLC( iSession );
    dataManager->LoadEmailSettingsL( aService, *settingsData );
    // the IncludeSignature returns zero in case that inclusion is ON..
    if ( !settingsData->IncludeSignature() )
        {
        CRichText* rText = settingsData->Signature().iRichText;
        if ( rText )
            {
            signatureText = HBufC::NewL( KIpsSetUiMaxSettingsSignatureLength );
            TPtr sPtr = signatureText->Des();
            rText->Extract( sPtr, 0, KIpsSetUiMaxSettingsSignatureLength );
            }
        }
    CleanupStack::PopAndDestroy( 2, settingsData ); // dataManager
    return signatureText;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSetDataApi::GetImapChildFoldersL(
        CMsvSession& aSession,
        const CMsvEntrySelection& aFolders, 
        RArray<TMsvId>& aFillArray )
    {
    FUNC_LOG;
    for ( TInt folderLoop=0; folderLoop<aFolders.Count(); folderLoop++ )
        {
        TBool match( EFalse );
        TMsvEntry tEntry;
        TMsvId service;
        aSession.GetEntry( aFolders.At(folderLoop), service, tEntry );
        TMsvEmailEntry emailEntry( tEntry );

        if( emailEntry.LocalSubscription() )
            {
            for( TInt arrayLoop=0; arrayLoop<aFillArray.Count(); arrayLoop++ )
                {
                if( aFillArray[arrayLoop] == emailEntry.Id() )
                    {
                    match = ETrue;
                    }
                }
            
            if( !aFillArray.Count() || !match )
                {
                aFillArray.AppendL( aFolders[folderLoop] );
                }
            }
        }
    
    for ( TInt i = 0; i < aFolders.Count(); i++ )
        {
        CMsvEntry* cEntry = aSession.GetEntryL( aFolders[i] );
        CleanupStack::PushL( cEntry );
        CMsvEntrySelection* folders = cEntry->ChildrenWithTypeL( 
                KUidMsvFolderEntry );
        CleanupStack::PopAndDestroy( cEntry );
        CleanupStack::PushL( folders );
        // recursive
        GetImapChildFoldersL( aSession, *folders, aFillArray );
        CleanupStack::PopAndDestroy( folders );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSetDataApi::IsOutgoingLoginSameAsIncomingL(
    const TIpsSetDataStorerParams& aParams )
    {
    FUNC_LOG;
    CIpsSetDataStorer* storer = CIpsSetDataStorer::NewLC();
    CIpsSetDataExtension* extSettings = CIpsSetDataExtension::NewLC();
    User::LeaveIfError( storer->LoadExtendedSettings(
        TIpsSetDataStorerParams( aParams.AccountId(), aParams.MtmId() ),
        *extSettings ) );
    TInt outgoingLogin = extSettings->OutgoingLogin();
    CleanupStack::PopAndDestroy( 2 ); // extSettings, storer
    return outgoingLogin == 1; // == Same as incoming
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSetDataApi::SetNewSmtpPasswordL(
    const TMsvId aSmtpServiceId,
    const TDesC8& aPassword,
    CEmailAccounts& aEmailAccounts )
    {
    FUNC_LOG;
    CImSmtpSettings* smtpSettings = new(ELeave) CImSmtpSettings();
    CleanupStack::PushL( smtpSettings );
    TSmtpAccount smtpAccount;
    aEmailAccounts.GetSmtpAccountL( aSmtpServiceId, smtpAccount );
    aEmailAccounts.LoadSmtpSettingsL( smtpAccount, *smtpSettings );
    smtpSettings->SetPasswordL( aPassword );
    aEmailAccounts.SaveSmtpSettingsL( smtpAccount, *smtpSettings );
    CleanupStack::PopAndDestroy( smtpSettings );
    }

// End of File

