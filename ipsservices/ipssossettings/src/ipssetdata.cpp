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
* Description: This file implements class CIpsSetData.
*
*/


#include "emailtrace.h"
#include <smtpset.h>
#include <pop3set.h>
#include <imapset.h>
#include <txtrich.h>            // CRichText
#include <SendUiConsts.h>
#include <StringLoader.h>
#include <ipssossettings.rsg>

#include "ipssetdataapi.h"
#include "ipssetdata.h"
#include "ipssetdatasignature.h"
#include "ipssetdataextension.h"
#include "ipssetdatamanager.h"
#include "ipssetutils.h"
#include "ipssetutilsconsts.h"
#include "ipssetutilsconsts.hrh"




// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIpsSetData::CIpsSetData()
// ----------------------------------------------------------------------------
//
CIpsSetData::CIpsSetData()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetData::~CIpsSetData()
// ----------------------------------------------------------------------------
//
CIpsSetData::~CIpsSetData()
    {
    FUNC_LOG;
    iName.Close();
    iTempStore.Close();
    delete iSignature;
    delete iImap4Settings;
    delete iPop3Settings;
    delete iSmtpSettings;
    delete iExtendedSettings;
    delete iIncomingIapPref;
    delete iOutgoingIapPref;    
    }

// ----------------------------------------------------------------------------
// CIpsSetData::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetData::ConstructL()
    {
    FUNC_LOG;
    iName.CreateL( KIpsSetUiMaxSettingsTextLength );
    iTempStore.CreateL( KIPsSetUiMaxSettingsUsrNameLength );
    iSignature = CIpsSetDataSignature::NewL();
    iSignature->CreateEmptyRichTextL();
    iSmtpSettings = new ( ELeave ) CImSmtpSettings;
    iPop3Settings = new ( ELeave ) CImPop3Settings;
    iImap4Settings = new ( ELeave ) CImImap4Settings;
    iIncomingIapPref = CImIAPPreferences::NewLC();
    CleanupStack::Pop( iIncomingIapPref );
    iOutgoingIapPref = CImIAPPreferences::NewLC();
    CleanupStack::Pop( iOutgoingIapPref );
    iExtendedSettings = CIpsSetDataExtension::NewL();
    SetDefaultDataL();
    }

// ----------------------------------------------------------------------------
// CIpsSetData::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetData* CIpsSetData::NewL()
    {
    FUNC_LOG;
    CIpsSetData* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetData::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetData* CIpsSetData::NewLC()
    {
    FUNC_LOG;
    CIpsSetData* self = new ( ELeave ) CIpsSetData();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }
    
// ===================== New functions ========================

// ---------------------------------------------------------------------------
// CIpsSetData::SetDefaultDataL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetDefaultDataL()
    {
    FUNC_LOG;
    
    CEmailAccounts* accounts = CEmailAccounts::NewL();
    CleanupStack::PushL( accounts );
    accounts->PopulateDefaultImapSettingsL( *iImap4Settings, *iIncomingIapPref );
    accounts->PopulateDefaultPopSettingsL( *iPop3Settings, *iIncomingIapPref );
    CleanupStack::PopAndDestroy( accounts );
    
    // POP3 preset settings
    iPop3Settings->SetAutoSendOnConnect( ETrue );
    iPop3Settings->SetDisconnectedUserMode( ETrue );
    iPop3Settings->SetDeleteEmailsWhenDisconnecting( EFalse );
    iPop3Settings->SetAcknowledgeReceipts( EFalse );
    iPop3Settings->SetGetMailOptions( EGetPop3EmailHeaders );
    iPop3Settings->SetInboxSynchronisationLimit( 
            KIpsSetFetchHeadersDefaultLimit );
    iPop3Settings->SetPopulationLimitL( KIpsSetDataFullBodyAndAttas );
    
    // IMAP4 preset settings
    iImap4Settings->SetAutoSendOnConnect( ETrue );
    iImap4Settings->SetDisconnectedUserMode( ETrue );
    iImap4Settings->SetDeleteEmailsWhenDisconnecting( EFalse );
    iImap4Settings->SetAcknowledgeReceipts( EFalse );
    iImap4Settings->SetSynchronise( EUseLocal );
    iImap4Settings->SetInboxSynchronisationLimit( 
            KIpsSetFetchHeadersDefaultLimit );
    iImap4Settings->SetBodyTextSizeLimitL( KIpsSetDataFullBodyAndAttas );
    
    // SMTP Preset parameters
    iSmtpSettings->SetBodyEncoding( EMsgOutboxMIME );
    iSmtpSettings->SetAddVCardToEmail( EFalse );
    iSmtpSettings->SetAddSignatureToEmail( EFalse );
    iSmtpSettings->SetRequestReceipts( EFalse );
    iSmtpSettings->SetSMTPAuth( ETrue );
    // <cmail> "To" an "Cc" needs to be included 
    // in header of reply/forward mail's body
    iSmtpSettings->SetToCcIncludeLimitL( KSmtpToCcIncludeLimitMax );
    // </cmail>
    
    // Extended Preset parameters
    iExtendedSettings->PopulateDefaults();
    }
    

// ---------------------------------------------------------------------------
// CIpsSetData::LoadL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::LoadL( const TMsvId aMailboxId, CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSetDataManager* manager = CIpsSetDataManager::NewLC( aSession );

    TMsvEntry mbox = IpsSetUtils::GetMailboxEntryL( aSession, aMailboxId );
    manager->LoadEmailSettingsL( mbox, *this );
    
    // Update signature if needed
    HBufC* body = NULL;
    body = HBufC::NewL( iSignature->iRichText->DocumentLength() );
    CleanupStack::PushL( body );
    TPtr ptr = body->Des();
    iSignature->iRichText->Extract( ptr );
    TInt len = ptr.Length();
    ptr.TrimLeft();
    len-= ptr.Length();
    if( len )
        {
        // Remove leading empty line from signature
        iSignature->iRichText->DeleteL( 0, len );
        }
    CleanupStack::PopAndDestroy( body );
    
    CleanupStack::PopAndDestroy( manager );
    manager = NULL;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SaveL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SaveL( CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSetDataManager* manager = CIpsSetDataManager::NewLC( aSession );

    manager->SaveEmailSettingsL( *this );
    
    CleanupStack::PopAndDestroy( manager );
    manager = NULL;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::IncomingIapPref()
// ---------------------------------------------------------------------------
//
CImIAPPreferences* CIpsSetData::IncomingIapPref() const
    {
    FUNC_LOG;
    return iIncomingIapPref;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::OutgoingIapPref()
// ---------------------------------------------------------------------------
//
CImIAPPreferences* CIpsSetData::OutgoingIapPref() const
    {
    FUNC_LOG;
    return iOutgoingIapPref;
    }


// ---------------------------------------------------------------------------
// CIpsSetData::Imap4Settings()
// ---------------------------------------------------------------------------
//
CImImap4Settings* CIpsSetData::Imap4Settings()
    {
    FUNC_LOG;
    return iImap4Settings;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::Pop3Settings()
// ---------------------------------------------------------------------------
//
CImPop3Settings* CIpsSetData::Pop3Settings()
    {
    FUNC_LOG;
    return iPop3Settings;
    }


// ---------------------------------------------------------------------------
// CIpsSetData::SmtpSettings()
// ---------------------------------------------------------------------------
//
CImSmtpSettings* CIpsSetData::SmtpSettings()
    {
    FUNC_LOG;
    return iSmtpSettings;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::ExtendedSettings()
// ---------------------------------------------------------------------------
//
CIpsSetDataExtension* CIpsSetData::ExtendedSettings()
    {
    FUNC_LOG;
    return iExtendedSettings;
    }


// ---------------------------------------------------------------------------
// CIpsSetData::IsOk()
// ---------------------------------------------------------------------------
//
TBool CIpsSetData::IsOk()
    {
    FUNC_LOG;
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::Protocol()
// ---------------------------------------------------------------------------
//
const TUid& CIpsSetData::Protocol() const
    {
    FUNC_LOG;
    return iProtocol;
    }


// ---------------------------------------------------------------------------
// CIpsSetData::ImapAccount()
// ---------------------------------------------------------------------------
//
TImapAccount& CIpsSetData::ImapAccount()
    {
    FUNC_LOG;
    return iImap4AccountId;
    }
    
// ---------------------------------------------------------------------------
// CIpsSetData::ImapAccount()
// ---------------------------------------------------------------------------
//
const TImapAccount& CIpsSetData::ImapAccount() const
    {
    FUNC_LOG;
    return iImap4AccountId;
    }
    

// ---------------------------------------------------------------------------
// CIpsSetData::PopAccount()
// ---------------------------------------------------------------------------
//
TPopAccount& CIpsSetData::PopAccount()
    {
    FUNC_LOG;
    return iPop3AccountId; 
    }
       
// ---------------------------------------------------------------------------
// CIpsSetData::PopAccount()
// ---------------------------------------------------------------------------
//
const TPopAccount& CIpsSetData::PopAccount() const
    {
    FUNC_LOG;
    return iPop3AccountId; 
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SmtpAccount()
// ---------------------------------------------------------------------------
//
TSmtpAccount& CIpsSetData::SmtpAccount()
    {
    FUNC_LOG;
    return iSmtpAccountId;
    }
    
// ---------------------------------------------------------------------------
// CIpsSetData::SmtpAccount()
// ---------------------------------------------------------------------------
//
const TSmtpAccount& CIpsSetData::SmtpAccount() const
    {
    FUNC_LOG;
    return iSmtpAccountId;
    }
            
// ---------------------------------------------------------------------------
// CIpsSetData::EmailAddress()
// ---------------------------------------------------------------------------
//
const TDesC& CIpsSetData::EmailAddress() const
    {
    FUNC_LOG;
    RBuf& temp = *const_cast<RBuf*>( &iTempStore );
    temp.Copy( iSmtpSettings->EmailAddress() );
    return iTempStore;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::ReplyToAddress()
// ---------------------------------------------------------------------------
//
const TDesC& CIpsSetData::ReplyToAddress() const
    {
    FUNC_LOG;
    RBuf& temp = *const_cast<RBuf*>( &iTempStore );
    temp.Copy( iSmtpSettings->ReplyToAddress() );
    return iTempStore;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::MailboxName()
// ---------------------------------------------------------------------------
//
const TDesC& CIpsSetData::MailboxName() const
    {
    FUNC_LOG;
    return iSmtpAccountId.iSmtpAccountName;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::MyName()
// ---------------------------------------------------------------------------
//
const TDesC& CIpsSetData::MyName() const
    {
    FUNC_LOG;
    RBuf& temp = *const_cast<RBuf*>( &iTempStore );
    temp.Copy( iSmtpSettings->EmailAlias() );
    return iTempStore;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::IncludeSignature()
// ---------------------------------------------------------------------------
//
TBool CIpsSetData::IncludeSignature() const
    {
    FUNC_LOG;
    return iSmtpSettings->AddSignatureToEmail() ? EIpsSetUiOn : EIpsSetUiOff;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::Signature()
// ---------------------------------------------------------------------------
//
CIpsSetDataSignature& CIpsSetData::Signature()
    {
    FUNC_LOG;
    return *iSignature;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::Signature()
// ---------------------------------------------------------------------------
//
const CIpsSetDataSignature& CIpsSetData::Signature() const
    {
    FUNC_LOG;
    return *iSignature;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::UserName()
// ---------------------------------------------------------------------------
//
const TDesC& CIpsSetData::UserName( const TBool aIncoming ) const
    {
    FUNC_LOG;
    // The method is const type, but we have to store the text into a
    // temporary storage, so cast the constness away from the variable.
    RBuf& temp = *const_cast<RBuf*>( &iTempStore );
    
    if ( aIncoming )
        {        
        if ( iProtocol == KSenduiMtmImap4Uid )
            {
            temp.Copy( iImap4Settings->LoginName() );
            return iTempStore;
            }
        else
            {            
            temp.Copy( iPop3Settings->LoginName() );
            return iTempStore;
            }
        }
    else
        {
        temp.Copy( iSmtpSettings->LoginName() );
        return iTempStore;
        }       
    }

// ---------------------------------------------------------------------------
// CIpsSetData::UserPwd()
// ---------------------------------------------------------------------------
//
const TDesC& CIpsSetData::UserPwd( const TBool aIncoming ) const
    {
    FUNC_LOG;
    // The method is const type, but we have to store the text into a
    // temporary storage, so cast the constness away from the variable.
    RBuf& temp = *const_cast<RBuf*>( &iTempStore );
    
    if ( aIncoming )
        {        
        if ( iProtocol == KSenduiMtmImap4Uid )
            {
            temp.Copy( iImap4Settings->Password() );
            return iTempStore;
            }
        else
            {            
            temp.Copy( iPop3Settings->Password() );
            return iTempStore;
            }
        }
    else
        {
        temp.Copy( iSmtpSettings->Password() );
        return iTempStore;
        }       
    }

// ---------------------------------------------------------------------------
// CIpsSetData::MailServer()
// ---------------------------------------------------------------------------
//
const TDesC& CIpsSetData::MailServer( const TBool aIncoming ) const
    {
    FUNC_LOG;
    RBuf& temp = *const_cast<RBuf*>( &iTempStore );
    
    if ( aIncoming )
        {        
        if ( iProtocol == KSenduiMtmImap4Uid )
            {
            temp.Copy( iImap4Settings->ServerAddress() );
            return iTempStore;
            }
        else
            {
            temp.Copy( iPop3Settings->ServerAddress() );
            return iTempStore;
            }
        }
    else
        {
        temp.Copy( iSmtpSettings->ServerAddress() );
        return iTempStore;
        }       
    }

// ---------------------------------------------------------------------------
// CIpsSetData::Iap()
// ---------------------------------------------------------------------------
//
TImIAPChoice CIpsSetData::Iap( const TBool aIncoming ) const
    {
    FUNC_LOG;
    
    TImIAPChoice ret;
    ret.iDialogPref = ECommDbDialogPrefPrompt;
    ret.iIAP = 0;
    if ( aIncoming )
        {
        if (iIncomingIapPref->SNAPDefined())
            {
            ret.iIAP = iIncomingIapPref->SNAPPreference();
            }
            
        if ( iIncomingIapPref->NumberOfIAPs() > 0 )
            {
            ret = iIncomingIapPref->IAPPreference(0);
            }
        }
    else
        {
        if ( iOutgoingIapPref->NumberOfIAPs() > 0 )
            {
            ret = iOutgoingIapPref->IAPPreference(0);   
            }
         
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::Security()
// ---------------------------------------------------------------------------
//
TInt CIpsSetData::Security( const TBool aIncoming ) const
    {
    FUNC_LOG;
    const CImBaseEmailSettings& settings = !aIncoming ? 
        *iSmtpSettings : 
        iProtocol == KSenduiMtmImap4Uid ? 
            *static_cast<const CImBaseEmailSettings*>( iImap4Settings ) : 
            *static_cast<const CImBaseEmailSettings*>( iPop3Settings );
    return settings.SecureSockets() ? 
        EStartTls : settings.SSLWrapper() ? ESslTls : ESecurityOff;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::Port()
// ---------------------------------------------------------------------------
//
TInt CIpsSetData::Port( const TBool aIncoming ) const
    {
    FUNC_LOG;
    if ( aIncoming )
        {        
        if ( iProtocol == KSenduiMtmImap4Uid )
            {
            return iImap4Settings->Port();
            }
        else
            {
            return iPop3Settings->Port();
            }
        }
    else
        {
        return iSmtpSettings->Port();
        }       
    }

// ---------------------------------------------------------------------------
// CIpsSetData::APop()
// ---------------------------------------------------------------------------
//
TInt CIpsSetData::APop() const
    {
    FUNC_LOG;
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        return KErrNotFound;
        }
    else
        {
        return iPop3Settings->Apop() ? EIpsSetUiOn : EIpsSetUiOff;
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetData::UserAuthentication()
// ---------------------------------------------------------------------------
//
TInt CIpsSetData::UserAuthentication() const
    {
    FUNC_LOG;
    return iExtendedSettings->OutgoingLogin();
    }

// ---------------------------------------------------------------------------
// CIpsSetData::ImapPath()
// ---------------------------------------------------------------------------
//
const TDesC& CIpsSetData::ImapPath() const
    {
    FUNC_LOG;
    // The method is const type, but we have to store the text into a
    // temporary storage, so cast the constness away from the variable.
    RBuf& temp = *const_cast<RBuf*>( &iTempStore );
    
    if ( iProtocol == KSenduiMtmImap4Uid )
        {        
        temp.Copy( iImap4Settings->FolderPath() );
        
        // If path is not set, use default string
        if ( !temp.Length() )
            {
            HBufC* text = NULL;
            TRAP_IGNORE( text = StringLoader::LoadL( R_FSE_SETTINGS_MAIL_FOLDER_PATH ) );
            temp.Copy( *text );
            delete text;
            text = NULL;
            }
        
        return iTempStore;
        }
    else
        {
        return KNullDesC;
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetData::DownloadSize()
// ---------------------------------------------------------------------------
//
TInt CIpsSetData::DownloadSize() const
    {
    FUNC_LOG;
    TInt ret(0);
    if ( iProtocol == KSenduiMtmImap4Uid ) 
        {
        ret = iImap4Settings->BodyTextSizeLimit();
        }
    else
        {
        ret = iPop3Settings->PopulationLimit();
        }
        
    return ret;                
    }

// ---------------------------------------------------------------------------
// CIpsSetData::RetrieveLimit()
// ---------------------------------------------------------------------------
//
TInt CIpsSetData::RetrieveLimit( const TRetrieveLimit aType ) const
    {
    FUNC_LOG;
    TInt ret = 0;
    switch ( aType )
        {
        case EPop3Limit: 
            ret = iPop3Settings->InboxSynchronisationLimit();
            break;
        case EImap4Inbox: 
            ret = iImap4Settings->InboxSynchronisationLimit();
            break;
        case EImap4Folders:
            ret = iImap4Settings->MailboxSynchronisationLimit();
            break;
        default:
            break;
        }
    return ret;        
    }

// ---------------------------------------------------------------------------
// CIpsSetData::Schedule()
// ---------------------------------------------------------------------------
//
TInt CIpsSetData::Schedule() const
    {
    FUNC_LOG;
    // First, check if notifications are on, one button off from the checks
    if ( iExtendedSettings->EmailNotificationState() != EMailEmnOff )
        {
        return ENotification;
        }
    // EMN is off, maybe its manual mode
    else if ( iExtendedSettings->AlwaysOnlineState() == EMailAoOff )
        {
        return EManual;
        }
    // Nope, its the interval    
    else            
        {            
        switch ( iExtendedSettings->InboxRefreshTime() )
            {
            case KIpsSetDataFiveMinutes:            return E5Mins;
            case KIpsSetDataTenMinutes:             return E10Mins;
            case KIpsSetDataFifteenMinutes:         return E15Mins;
            case KIpsSetDataThirtyMinutes:          return E30Mins;
            case KIpsSetDataMinutesInHour:          return E1Hour;
            case KIpsSetDataMinutesInFourHours:     return E4Hours;
            case KIpsSetDataMinutesInTwelveHours:   return E12Hours;
            case KIpsSetDataMinutesInOneDay:        return E1Day;
            default: break;
            }
        }
        
    return E1Hour;        
    }

// ---------------------------------------------------------------------------
// CIpsSetData::Days()
// ---------------------------------------------------------------------------
//
TInt CIpsSetData::Days() const
    {
    FUNC_LOG;
    return iExtendedSettings->SelectedWeekDays();
    }

// ---------------------------------------------------------------------------
// CIpsSetData::Hours()
// ---------------------------------------------------------------------------
//
TTime CIpsSetData::Hours( const TBool aStartTime ) const
    {
    FUNC_LOG;
    return aStartTime ? 
        iExtendedSettings->SelectedTimeStart() :
        iExtendedSettings->SelectedTimeStop();
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetProtocol()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetProtocol( const TUid& aProtocol )
    {
    FUNC_LOG;
    iProtocol = aProtocol;
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetEmailAddressL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetEmailAddressL( const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    iSmtpSettings->SetEmailAddressL( aEmailAddress );
    iExtendedSettings->SetEmailAddress( aEmailAddress );
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetReplyToAddressL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetReplyToAddressL( const TDesC& aReplyToAddress )
    {
    FUNC_LOG;
    iSmtpSettings->SetReplyToAddressL( aReplyToAddress );
    }
    
// ---------------------------------------------------------------------------
// CIpsSetData::SetMailboxName()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetMailboxName( const TDesC& aMailboxName )
    {
    FUNC_LOG;
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        iImap4AccountId.iImapAccountName.Copy( aMailboxName );
        }
    else
        {
        iPop3AccountId.iPopAccountName.Copy( aMailboxName );
        }            
        
    iSmtpAccountId.iSmtpAccountName.Copy( aMailboxName );
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetMyNameL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetMyNameL( const TDesC& aMyName )
    {
    FUNC_LOG;
    iSmtpSettings->SetEmailAliasL( aMyName );
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetIncludeSignature()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetIncludeSignature( TBool aIncludeSignature )
    {
    FUNC_LOG;
    iSmtpSettings->SetAddSignatureToEmail( aIncludeSignature == EIpsSetUiOn );
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetSignatureL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetSignatureL( const TDesC& aSignature )
    {
    FUNC_LOG;
    iSignature->iRichText->Reset();
    
    if ( aSignature.Length() )
        {
        TChar firstChar = aSignature[0];
        _LIT( KNewLines, "\r\n\x2028\x2029" );
        _LIT( KIMSLineFeed, "\r\n");
        if ( KNewLines().Locate( firstChar ) == KErrNotFound )
            {
            // First character is not a new line character. Insert one.
            HBufC* body = NULL;
            body = HBufC::NewL( aSignature.Length() + KIMSLineFeed().Length() );
            CleanupStack::PushL( body );
            TPtr ptr = body->Des();
            ptr.Append( KIMSLineFeed );
            ptr.Append( aSignature );
            iSignature->iRichText->InsertL( 0, ptr );
            CleanupStack::PopAndDestroy( body );
            }
        else
            {
            iSignature->iRichText->InsertL( 0, aSignature );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetUserNameL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetUserNameL( 
    const TDesC& aIncomingName, 
    const TDesC& aOutgoingName )
    {
    FUNC_LOG;
    TBuf8<KIPsSetUiMaxSettingsUsrNameLength> temp;
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        temp.Copy( aIncomingName );
        iImap4Settings->SetLoginNameL( temp );
        }
    else
        {
        temp.Copy( aIncomingName );
        iPop3Settings->SetLoginNameL( temp );
        }
    
    temp.Copy( aOutgoingName );
    iSmtpSettings->SetLoginNameL( temp );
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetUserPwdL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetUserPwdL( 
    const TDesC& aIncomingPassword, 
    const TDesC& aOutgoingPassword )
    {
    FUNC_LOG;
    TBuf8<KIpsSetUiMaxSettingsTextLength> temp;
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        temp.Copy( aIncomingPassword );
        iImap4Settings->SetPasswordL( temp );
        }
    else
        {
        temp.Copy( aIncomingPassword );
        iPop3Settings->SetPasswordL( temp );
        }
    
    temp.Copy( aOutgoingPassword );
    iSmtpSettings->SetPasswordL( temp );
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetMailServerL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetMailServerL( 
    const TDesC& aIncomingServer, 
    const TDesC& aOutgoingServer )
    {
    FUNC_LOG;
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        iImap4Settings->SetServerAddressL( aIncomingServer );
        }
    else
        {
        iPop3Settings->SetServerAddressL( aIncomingServer );
        }
        
    iSmtpSettings->SetServerAddressL( aOutgoingServer ); 
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetIapL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetIapL( 
    const TImIAPChoice aIncomingIap, 
    const TImIAPChoice aOutgoingIap )
    {
    FUNC_LOG;
    // This is not ideal choice, but current situation forces to do this,
    // until Reset() funcion is exported from class CImIAPPreferences
    delete iIncomingIapPref;
    iIncomingIapPref = NULL;
    delete iOutgoingIapPref;
    iOutgoingIapPref = NULL;

    iIncomingIapPref = CImIAPPreferences::NewLC();
    CleanupStack::Pop( iIncomingIapPref );
    iOutgoingIapPref = CImIAPPreferences::NewLC();
    CleanupStack::Pop( iOutgoingIapPref );
    iIncomingIapPref->AddIAPL( aIncomingIap );
    iOutgoingIapPref->AddIAPL( aOutgoingIap );
    }
// ---------------------------------------------------------------------------
// CIpsSetData::SetIapL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetIapL( 
    const CImIAPPreferences& aIncomingIap, 
    const CImIAPPreferences& aOutgoingIap )
    {
    FUNC_LOG;
    // This is not ideal choice, but current situation forces to do this,
    // until Reset() funcion is exported from class CImIAPPreferences
    delete iIncomingIapPref;
    iIncomingIapPref = NULL;
    delete iOutgoingIapPref;
    iOutgoingIapPref = NULL;

    iIncomingIapPref = CImIAPPreferences::NewLC();
    CleanupStack::Pop( iIncomingIapPref );
    iOutgoingIapPref = CImIAPPreferences::NewLC();
    CleanupStack::Pop( iOutgoingIapPref );
        
    if (aIncomingIap.SNAPDefined())
        {
        iIncomingIapPref->SetSNAPL(aIncomingIap.SNAPPreference());
        }
    else
        {
        iIncomingIapPref->AddIAPL( aIncomingIap.IAPPreference(0) );
        }
        
    if (aOutgoingIap.SNAPDefined())
        {
        iOutgoingIapPref->SetSNAPL(aOutgoingIap.SNAPPreference());
        }
    else
    	{
    	iOutgoingIapPref->AddIAPL( aOutgoingIap.IAPPreference(0) );
    	}
    
    }


// ---------------------------------------------------------------------------
// CIpsSetData::SetSecurity()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetSecurity( 
    const TInt aIncomingSecurity, 
    const TInt aOutgoingSecurity )
    {
    FUNC_LOG;
    CImBaseEmailSettings& settings = ( iProtocol == KSenduiMtmImap4Uid ) ?
        *static_cast<CImBaseEmailSettings*>( iImap4Settings ): 
        *static_cast<CImBaseEmailSettings*>( iPop3Settings );
    
    DoSetSecurity( aIncomingSecurity, settings );
    DoSetSecurity( aOutgoingSecurity, *iSmtpSettings );
    }

// ---------------------------------------------------------------------------
// CIpsSetData::DoSetSecurity()
// ---------------------------------------------------------------------------
//
void CIpsSetData::DoSetSecurity( 
    const TInt aSecurity,
    CImBaseEmailSettings& aSettings )
    {
    FUNC_LOG;
     switch( aSecurity )
        {
        case EStartTls:
            aSettings.SetSecureSockets( ETrue );
            aSettings.SetSSLWrapper( EFalse );
            break;

        case ESslTls:
            aSettings.SetSecureSockets( EFalse );
            aSettings.SetSSLWrapper( ETrue );
            break;

        case ESecurityOff:
            aSettings.SetSecureSockets( EFalse );
            aSettings.SetSSLWrapper( EFalse );
            break;

        default:
            break;
        }      
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetPort()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetPort( 
    const TInt aIncomingPort, 
    const TInt aOutgoingPort )
    {
    FUNC_LOG;
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        iImap4Settings->SetPort( aIncomingPort );
        }
    else
        {
        iPop3Settings->SetPort( aIncomingPort );
        }
    
    iSmtpSettings->SetPort( aOutgoingPort );
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetAPop()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetAPop( TInt aAPop )
    {
    FUNC_LOG;
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        // Do nothing
        }
    else
        {
        iPop3Settings->SetApop( aAPop == EIpsSetUiOn );
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetUserAuthentication()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetUserAuthentication( TInt aUserAuthentication )
    {
    FUNC_LOG;
    iExtendedSettings->SetOutgoingLogin( aUserAuthentication );
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetImapPathL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetImapPathL( const TDesC& aImapPath )
    {
    FUNC_LOG;
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        TBuf8<KIpsSetUiMaxSettingsTextLength> temp;
        
        HBufC* text = StringLoader::LoadL( R_FSE_SETTINGS_MAIL_FOLDER_PATH );
        
        if( !text->Compare( aImapPath ) )
            {
            //If the text matches with the default text, clean the text field
            temp.Zero();
            }
        else
            {
            temp.Copy( aImapPath );
            }
        
        delete text;
        text = NULL;
        iImap4Settings->SetFolderPathL( temp );
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetDownloadSizeL()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetDownloadSizeL( 
    const TInt aDownloadControl,
    const TInt aDownloadSize )
    {
    FUNC_LOG;
    TInt size = 0;
    if ( aDownloadControl == CIpsSetData::EHeaders ) 
        {
        size = KIpsSetDataHeadersOnly;
        }
    else if ( aDownloadControl == CIpsSetData::EWholeBody )
        {
        size = KIpsSetDataFullBodyAndAttas;
        }
   //<cmail>
    else if ( iProtocol == KSenduiMtmPop3Uid && aDownloadControl == CIpsSetData::EHeadersPlus )
        {
        // fix for POP that has only 2 radio (IMAP has 3) 
        // and the second should have value of EWholeBody instead of EHeadersPlus 
        size = KIpsSetDataFullBodyAndAttas; 
        }
    //</cmail>
    else
        {
        size = aDownloadSize;
        }
    
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        iImap4Settings->SetBodyTextSizeLimitL( size );
        }
    else
        {
        iPop3Settings->SetPopulationLimitL( size );
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetRetrieveLimit()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetRetrieveLimit( 
    TInt aRetrieveLimitPop,
    TInt aRetrieveLimitImapInbox,
    TInt aRetrieveLimitImapFolders )
    {
    FUNC_LOG;
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        iImap4Settings->SetInboxSynchronisationLimit( 
            aRetrieveLimitImapInbox );
        iImap4Settings->SetMailboxSynchronisationLimit( 
            aRetrieveLimitImapFolders );
        }
    else
        {
        iPop3Settings->SetInboxSynchronisationLimit( aRetrieveLimitPop );
        }            
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetSchedule()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetSchedule( 
    const TInt aSchedule,
    const TBool /*aRoaming*/ )
    {
    FUNC_LOG;
    TInt time = 0;
    switch ( aSchedule )
        {        
        case E5Mins:        time = KIpsSetDataFiveMinutes;          break;
        case E10Mins:       time = KIpsSetDataTenMinutes;           break;
        case E15Mins:       time = KIpsSetDataFifteenMinutes;       break;
        case E30Mins:       time = KIpsSetDataThirtyMinutes;        break;
        case E1Hour:        time = KIpsSetDataMinutesInHour;        break;
        case E4Hours:       time = KIpsSetDataMinutesInFourHours;   break;
        case E12Hours:      time = KIpsSetDataMinutesInTwelveHours; break;
        case E1Day:         time = KIpsSetDataMinutesInOneDay;      break;
        default:    break;
        };
        
    // Always Online in use        
    if ( aSchedule >= E5Mins && aSchedule <= E1Day )
        {
        iExtendedSettings->SetAlwaysOnlineState( EMailAoAlways );
        iExtendedSettings->SetInboxRefreshTime( time );
        iExtendedSettings->SetEmailNotificationState( EMailEmnOff );
        }
    // Set EMN        
    else if ( aSchedule == ENotification )
        {
        // set always online state off when emn is on
        iExtendedSettings->SetAlwaysOnlineState( EMailAoOff );
        iExtendedSettings->SetEmailNotificationState( EMailEmnAutomatic );
        iExtendedSettings->SetInboxRefreshTime( KErrNotFound );
        }
    // Manual connection mode        
    else
        {
        iExtendedSettings->SetAlwaysOnlineState( EMailAoOff );
        iExtendedSettings->SetEmailNotificationState( EMailEmnOff );
        iExtendedSettings->SetInboxRefreshTime( KErrNotFound );
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetDays()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetDays( TInt aDays )
    {
    FUNC_LOG;
    iExtendedSettings->SetSelectedWeekDays( aDays );
    }

// ---------------------------------------------------------------------------
// CIpsSetData::SetHours()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetHours( 
    const TInt64 aStartHour,
    const TInt64 aStopHour )
    {
    FUNC_LOG;
    iExtendedSettings->SetSelectedTimeStart( TTime( aStartHour ) );
    iExtendedSettings->SetSelectedTimeStop( TTime( aStopHour ) );
    }

//<cmail>
// ---------------------------------------------------------------------------
// CIpsSetData::SetDataHide()
// ---------------------------------------------------------------------------
//
void CIpsSetData::SetDataHide( TBool aHideData )
    {
    iExtendedSettings->SetDataHidden( aHideData );
    }
//</cmail>

// End of File

