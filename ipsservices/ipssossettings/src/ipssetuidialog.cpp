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
*  Description: This file implements class CIpsSetUiDialogCtrl.
*
*/


#include "emailtrace.h"
#include <e32base.h>
//<cmail>
//#include <aputils.h> not used in cmail
//</cmail>
#include <ipssossettings.rsg>
#include <akntitle.h>
#include <txtrich.h>    // crichtext
#include <SendUiConsts.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <layoutmetadata.cdl.h>

#include <miut_err.h>

#include <featmgr.h>
//</cmail>

//<cmail>
#include "cfsmailcommon.h"
//</cmail>

#include "ipssetui.h"
#include "ipssetuiitemaccesspoint.h"
#include "ipssetutilspageids.hrh"
#include "ipssetdata.h"
#include "ipssetdatasignature.h"
#include "ipssetutilsexception.h"
#include "ipssetutilsconsts.h"
#include "ipssetuifoldersubscription.h"
#include "ipssetutils.h"
#include "ipsplgsossettings.hrh"
#include "ipssetuiitemlink.h"
#include "ipssetuiitemextlink.h"
#include "ipssetuidialog.h"
//<cmail>
#include "ipsplgconnectandrefreshfolderlist.h"
#include "ipssetutilsoperationwait.h"

class MFSMailRequestObserver;
class CIpsPlgTimerOperation;
//</cmail>

//<cmail>
#include "ipssetdataextension.h"
//</cmail>

#include "FreestyleEmailUiConstants.h"
#include <csxhelp/cmail.hlp.hrh>

// All day mask
const TUint KIpsSetUiAllDays = 0x7F;
const TInt KIpsSetDefaultLimit = 30;
const TInt KIpsSetDefaultSizeLimit = 1;

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::NewL()
// ----------------------------------------------------------------------------
//
CIpsSetUiDialogCtrl* CIpsSetUiDialogCtrl::NewL(
    CIpsSetUi& aDialog,
    CEikFormattedCellListBox& aListBox,
    CAknTitlePane& aTitlePane,
    CEikButtonGroupContainer& aButtons,
    CMsvSession& aSession,
    TUint64& aFlags )
    {
    FUNC_LOG;
    CIpsSetUiDialogCtrl* self = NewLC(
        aDialog, aListBox, aTitlePane, aButtons, aSession ,aFlags );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::NewLC()
// ----------------------------------------------------------------------------
//
CIpsSetUiDialogCtrl* CIpsSetUiDialogCtrl::NewLC(
    CIpsSetUi& aDialog,
    CEikFormattedCellListBox& aListBox,
    CAknTitlePane& aTitlePane,
    CEikButtonGroupContainer& aButtons,
    CMsvSession& aSession,
    TUint64& aFlags )
    {
    FUNC_LOG;
    CIpsSetUiDialogCtrl* self = new ( ELeave ) CIpsSetUiDialogCtrl(
            aDialog, aListBox, aTitlePane, aSession ,aFlags );
    CleanupStack::PushL( self );
    self->ConstructL( aButtons );
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::ConstructL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::ConstructL(
    CEikButtonGroupContainer& aButtons )
    {
    FUNC_LOG;
    iData = CIpsSetData::NewL();
#ifndef _DEBUG
    iData->LoadL( iDialog.MailboxId(), iSession );
#else
    TRAPD( dataErr, iData->LoadL( iDialog.MailboxId(), iSession ) );
    if ( dataErr != KErrNone )
        {
        }
#endif
   
    //<cmail>
    const CIpsSetDataExtension* extSet = iData->ExtendedSettings();
    if ( extSet->DataHidden() )
        {
        iFlags |= EIpsSetUiHideAccountData;
        }
    //</cmail>   
   
    TBool mskEnabled( AknLayoutUtils::MSKEnabled() && 
                      Layout_Meta_Data::IsMSKEnabled() );
                      
    CIpsSetUiBaseDialog::ConstructL( aButtons, R_IPS_SET_MAIN_MENU, mskEnabled );
    iFlags |= EIpsSetUiMainSettingsMenu;

    SetMainMBoxMenuTitlePaneTextL( iData->MailboxName(), ETrue );
      
#ifdef _DEBUG // to prevent compiler warning in urel
    TInt error = KErrNone;
#endif // _DEBUG
    __ASSERT_DEBUG( error == KErrNone, User::Panic( KIpsSetGenPanicLit, error ) );
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::CIpsSetUiDialogCtrl()
// ----------------------------------------------------------------------------
//
CIpsSetUiDialogCtrl::CIpsSetUiDialogCtrl(
    CIpsSetUi& aDialog,
    CEikFormattedCellListBox& aListBox,
    CAknTitlePane& aTitlePane,
    CMsvSession& aSession,
    TUint64& aFlags )
    :
    CIpsSetUiBaseDialog( aListBox, aTitlePane ),
    iDialog( aDialog ),
    iSession( aSession ),
    iFlags( aFlags )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::~CIpsSetUiDialogCtrl()
// ----------------------------------------------------------------------------
//
CIpsSetUiDialogCtrl::~CIpsSetUiDialogCtrl()
    {
    FUNC_LOG;
    delete iData;
    iData = NULL;
    iSignature.Close();
    }
//<cmail>
// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitUserData()
// ----------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitUserData( CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    switch ( aBaseItem.iItemId.iUid )
        {
        case EIpsSetUiMainMailboxSettings:
        case EIpsSetUiMainWhatToSync:
        case EIpsSetUiMainWhenToSync:
        case EIpsSetUiAdvancedIncomingSettings:
        case EIpsSetUiAdvancedOutgoingSettings:
            // Menu items do not contain any user data
            break;

        case EIpsSetUiMailboxEmailAddress:
            InitAnyItem( aBaseItem, 0, iData->EmailAddress() );
            break;

        case EIpsSetUiMailboxReplyToAddress:
            InitAnyItem( aBaseItem, 0, iData->ReplyToAddress() );
            break;

        case EIpsSetUiMailboxMailboxName:
            InitAnyItem( aBaseItem, 0, iData->MailboxName() );
            break;

        case EIpsSetUiMailboxMyName:
            InitAnyItem( aBaseItem, 0, iData->MyName() );
            break;

        case EIpsSetUiMailboxIncludeSignature:
            InitAnyItem( aBaseItem, iData->IncludeSignature(), KNullDesC );
            break;

        case EIpsSetUiIncomingUserName:
            InitAnyItem( aBaseItem, 0, iData->UserName( ETrue ) );        
            if ( iFlags & EIpsSetUiHideAccountData )
                {
                SetHideItem( ETrue, aBaseItem, EFalse );        
                }
            break;

        case EIpsSetUiOutgoingUserName:
            InitAnyItem( aBaseItem, 0, iData->UserName( EFalse ) );        
            if ( iFlags & EIpsSetUiHideAccountData )
                {
                SetHideItem( ETrue, aBaseItem, EFalse );        
                }
            break;

        case EIpsSetUiIncomingUserPwd:
            InitAnyItem( aBaseItem, 0, iData->UserPwd( ETrue ) );
            break;

        case EIpsSetUiOutgoingUserPwd:
            InitLogin( aBaseItem, iData->UserPwd( EFalse ) );
            break;

        case EIpsSetUiIncomingMailServer:
            InitAnyItem( aBaseItem, 0, iData->MailServer( ETrue ) );
            if ( iFlags & EIpsSetUiHideAccountData )
                {
                SetHideItem( ETrue, aBaseItem, EFalse );
                }                            
            break;

        case EIpsSetUiOutgoingMailServer:
            InitAnyItem( aBaseItem, 0, iData->MailServer( EFalse ) );
            if ( iFlags & EIpsSetUiHideAccountData )
                {
                SetHideItem( ETrue, aBaseItem, EFalse );
                }                                        
            break;

        case EIpsSetUiIncomingSecurity:
            InitSecuritySetting( aBaseItem, iData->Security( ETrue ) );
            break;

        case EIpsSetUiOutgoingSecurity:
            InitSecuritySetting( aBaseItem, iData->Security( EFalse ) );
            break;

        case EIpsSetUiIncomingPort:
            InitPort( aBaseItem );
            if ( iFlags & EIpsSetUiHideAccountData )
                {
                SetHideItem( ETrue, aBaseItem, EFalse );
                }                            
            break;

        case EIpsSetUiOutgoingPort:
            InitPort( aBaseItem );
            if ( iFlags & EIpsSetUiHideAccountData )
                {
                SetHideItem( ETrue, aBaseItem, EFalse ); 
                }                                        
            break;

        case EIpsSetUiIncomingAPop:
            InitAnyItem( aBaseItem, iData->APop(), KNullDesC );
            break;

        case EIpsSetUiOutgoingUserAuthentication:
            if ( iFlags & EIpsSetUiHideAccountData )
                {
                InitAnyItem( aBaseItem, CIpsSetData::EUseIncoming, KNullDesC );
                SetHideItem( ETrue, aBaseItem, EFalse );
                }
            else
                {
                InitAnyItem( aBaseItem, iData->UserAuthentication(), KNullDesC );
                }    
            break;

        case EIpsSetUiWhatImapPath:
            InitAnyItem( aBaseItem, 0, iData->ImapPath() );
            break;

        case EIpsSetUiWhatRetrievePop3:
        case EIpsSetUiWhatRetrievePop3EditCustom:
            InitRetrieve(
                aBaseItem,
                iData->RetrieveLimit( CIpsSetData::EPop3Limit ) );
            break;

        case EIpsSetUiWhatRetrieveImap4Inbox:
        case EIpsSetUiWhatRetrieveImap4InboxEditCustom:
            InitRetrieve(
                aBaseItem,
                iData->RetrieveLimit( CIpsSetData::EImap4Inbox ) );
            break;

        case EIpsSetUiWhatRetrieveImap4Folders:
        case EIpsSetUiWhatRetrieveImap4FolderEditCustom:
            InitRetrieve(
                aBaseItem,
                iData->RetrieveLimit( CIpsSetData::EImap4Folders ) );
            break;

        case EIpsSetUiWhenSchedule:
            InitAnyItem( aBaseItem, iData->Schedule(), KNullDesC );
            break;

        // these have leaving init functions
        case EIpsSetUiMailboxSignature:
        case EIpsSetUiOutgoingIap:
        case EIpsSetUiIncomingIap:
        case EIpsSetUiWhatDownloadSize:
        case EIpsSetUiWhatDownloadSizeEditPlus:
        case EIpsSetUiWhenDays:
        case EIpsSetUiWhenHours:
        case EIpsSetUiWhenHoursEditCustomizeFrom:
        case EIpsSetUiWhenHoursEditCustomizeTo:
            {
            TRAP_IGNORE( InitUserDataL( aBaseItem ) );
            }
            break;

        default:
            break;
        }
    }
//</cmail>
// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitUserDataL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitUserDataL( CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    switch ( aBaseItem.iItemId.iUid )
        {
        case EIpsSetUiMailboxSignature:
            InitSignatureL( aBaseItem );
            break;

        case EIpsSetUiOutgoingUserPwd:
            InitLogin( aBaseItem, iData->UserPwd( EFalse ) );
            break;

        case EIpsSetUiOutgoingIap:
        case EIpsSetUiIncomingIap:
            InitIapL( aBaseItem );
            break;

        case EIpsSetUiWhatDownloadSize:
        case EIpsSetUiWhatDownloadSizeEditPlus:
            InitDownloadSizeL( aBaseItem, iData->DownloadSize() );
            break;

        case EIpsSetUiWhenDays:
            InitAnyItem( aBaseItem, iData->Days(), KNullDesC );
            HandleEventDaysL( aBaseItem );
            break;

        case EIpsSetUiWhenHours:
        case EIpsSetUiWhenHoursEditCustomizeFrom:
        case EIpsSetUiWhenHoursEditCustomizeTo:
            InitHoursL(
                aBaseItem,
                iData->Hours( ETrue ).Int64(),
                iData->Hours( EFalse ).Int64() );
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitDownloadSizeL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitDownloadSizeL(
    CIpsSetUiItem& aBaseItem,
    const TInt aValue )
    {
    FUNC_LOG;
    if ( aBaseItem.iItemId.iUid == EIpsSetUiWhatDownloadSizeEditPlus )
        {
        InitAnyItem( aBaseItem, aValue, KNullDesC );
        }
    else // EIpsSetUiWhatDownloadSize
        {
        CIpsSetUiItem& item =
            *GetItem( TUid::Uid( EIpsSetUiWhatDownloadSizeEditPlus ) );
        if ( aValue == KErrNotFound )
            {
            InitAnyItem( aBaseItem, CIpsSetData::EWholeBody, KNullDesC );
            InitAnyItem( item, KIpsSetDefaultSizeLimit, KNullDesC );
            }
        else if ( aValue >= 0 )
            {
            InitAnyItem( aBaseItem, CIpsSetData::EHeadersPlus, KNullDesC );
            HandleDownloadSizeL( aBaseItem );
            }
        else
            {
            InitAnyItem( aBaseItem, CIpsSetData::EHeaders, KNullDesC );
            InitAnyItem( item, KIpsSetDefaultSizeLimit, KNullDesC );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::GetHelpContext()
// ----------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    FUNC_LOG;
    aContext.iMajor = KFSEmailUiUid;
    aContext.iContext = KFSE_HLP_LAUNCHER_GRID;
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::HandleSettingPageEventL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::HandleSettingPageEventL(
    CAknSettingPage* /* aSettingPage */,
    TAknSettingPageEvent /* aEventType */ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::OkToExitL()
// ---------------------------------------------------------------------------
//
TBool CIpsSetUiDialogCtrl::OkToExitL( const TInt /* aButtonId */ )
    {
    FUNC_LOG;
    // Save settings only when closing (back key pressed)
    if ( iFlags & EIpsSetUiShouldClose )
        {
        iFlags |= EIpsSetUiAllowSave;
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::EventHandlerL()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiDialogCtrl::EventHandlerL( const TInt aCommandId )
    {
    FUNC_LOG;
    TBool ok = ETrue;

    switch ( aCommandId )
        {
        // Show help dialog
        case EAknCmdHelp:
            iDialog.LaunchHelpL();
            break;

        // User has pressed ok key
        case EAknSoftkeyOpen:
        case EAknSoftkeySelect:
        case EAknSoftkeyOk:
            KeyPressOKButtonL( aCommandId );
            break;

        case EAknSoftkeyOptions:
            iFlags |= EIpsSetUiOpenOptionsMenu;
            iDialog.LaunchMenuL();
            break;

        case EAknSoftkeyShow:
        case EAknSoftkeyDone:
        case EAknSoftkeyBack:
            KeyPressSoftkeyL( aCommandId );
            break;

        case EAknSoftkeyCancel:
		
//<cmail>
            iFlags |= EIpsSetUiShouldClose;
            break;
//</cmail>

        // Emergency exit           

        case EAknCmdExit:
            iFlags |= EIpsSetUiShouldExit;
            iDialog.DoQuitL();
            break;

        default:
            ok = EFalse;
            break;
        }

    return ok;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::KeyPressSoftkeyL()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiDialogCtrl::KeyPressSoftkeyL(
    const TInt aButton )
    {
    FUNC_LOG;
    if ( aButton == EAknSoftkeyBack )
        {
        if ( iFlags & EIpsSetUiMainSettingsMenu )
            {
            iFlags |= EIpsSetUiShouldClose;
            SetSettingsMenuTitlePaneText( EFalse );
            iDialog.DoQuitL();
            }
        else
            {
            // Go backwards to previous setting page
            // Panic if any error
            TInt error( KErrNone );
            TRAP( error, HandleStackBackwardL() );
            IPS_ASSERT_DEBUG( error == KErrNone, error, EDialogCtrl );
            }
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::KeyPressOKButtonL()
// ----------------------------------------------------------------------------
//
TBool CIpsSetUiDialogCtrl::KeyPressOKButtonL( const TInt aButton )
    {
    FUNC_LOG;
    // Get the currently selected item
    CIpsSetUiItem* base = CurrentItem();
    IPS_ASSERT_DEBUG( error == KErrNone, KErrNotFound, EDialogCtrl );

    // When Options->Change is selected, then the menu should
    // be opened, no matter what
    if ( aButton == EAknSoftkeySelect )
        {
        base->iItemFlags.SetFlag32( KIpsSetUiFlagOneTimeForceViewOpen );
        }
    else
        {
        base->iItemFlags.ClearFlag32( KIpsSetUiFlagOneTimeForceViewOpen );
        }

    TIpsSetUiPageResult result = OpenSettingPageL( *base );

    if( base->iItemId == TUid::Uid( EIpsSetUiWhatImapPath ) )
        {
        //save folder path immediately
        iData->SetImapPathL( CurrentItem()->Text() );
        iData->SaveL( iSession );
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::EventSubArrayChangeL()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiDialogCtrl::EventSubArrayChangeL(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    if ( aBaseItem.iItemResourceId == R_IPS_SET_MAIN_MENU )
        {
        iFlags |= EIpsSetUiMainSettingsMenu;
        iFlags &= ~EIpsSetUiSettingsMenuItem;
        }
    else if ( aBaseItem.iItemResourceId == R_IPS_SET_MENU_ADVANCED_MAILBOX_SETTINGS )
        {
        iFlags &= ~EIpsSetUiSettingsMenuItem;
        }
    else
        {
        iFlags &= ~EIpsSetUiMainSettingsMenu;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::ProcessCommandL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::ProcessCommandL( TInt aCommandId )
    {
    FUNC_LOG;
    EventHandlerL( aCommandId );
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::DynInitMenuPaneL(
        /*TInt aResourceId,*/
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
	if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
		{
		// remove help support in pf5250
		aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
		}
    }


// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::StoreSettingsToAccountL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::StoreSettingsToAccountL()
    {
    FUNC_LOG;
    // Save all settings at once.
    iData->SetEmailAddressL(
        GetItem( TUid::Uid( EIpsSetUiMailboxEmailAddress ) )->Text() );
    iData->SetReplyToAddressL(
        GetItem( TUid::Uid( EIpsSetUiMailboxReplyToAddress ) )->Text() );
    iData->SetMailboxName(
        GetItem( TUid::Uid( EIpsSetUiMailboxMailboxName ) )->Text() );
    iData->SetMyNameL(
        GetItem( TUid::Uid( EIpsSetUiMailboxMyName ) )->Text() );
    iData->SetIncludeSignature(
        GetItem( TUid::Uid( EIpsSetUiMailboxIncludeSignature ) )->Value() );
    iData->SetSignatureL(
        GetItem( TUid::Uid( EIpsSetUiMailboxSignature ) )->Text() );
    iData->SetUserNameL(
        GetItem( TUid::Uid( EIpsSetUiIncomingUserName ) )->Text(),
        GetItem( TUid::Uid( EIpsSetUiOutgoingUserName ) )->Text() );
    iData->SetUserPwdL(
        GetItem( TUid::Uid( EIpsSetUiIncomingUserPwd ) )->Text(),
        GetItem( TUid::Uid( EIpsSetUiOutgoingUserPwd ) )->Text() );
    iData->SetMailServerL(
        GetItem( TUid::Uid( EIpsSetUiIncomingMailServer ) )->Text(),
        GetItem( TUid::Uid( EIpsSetUiOutgoingMailServer ) )->Text() );
	iData->SetIapL(
    	GetIapPrefsL(  TUid::Uid( EIpsSetUiIncomingIap ) ),
    	GetIapPrefsL(  TUid::Uid( EIpsSetUiOutgoingIap ) ) ); 
    iData->SetSecurity(
        GetItem( TUid::Uid( EIpsSetUiIncomingSecurity ) )->Value(),
        GetItem( TUid::Uid( EIpsSetUiOutgoingSecurity ) )->Value() );
    iData->SetPort(
        GetItem( TUid::Uid( EIpsSetUiIncomingPortBtnEditor ) )->Value(),
        GetItem( TUid::Uid( EIpsSetUiOutgoingPortBtnEditor ) )->Value() );
    iData->SetAPop(
        GetItem( TUid::Uid( EIpsSetUiIncomingAPop ) )->Value() );
    iData->SetUserAuthentication(
        GetItem( TUid::Uid( EIpsSetUiOutgoingUserAuthentication ) )->Value() );
    iData->SetImapPathL(
        GetItem( TUid::Uid( EIpsSetUiWhatImapPath ) )->Text() );
    iData->SetDownloadSizeL(
        GetItem( TUid::Uid( EIpsSetUiWhatDownloadSize ) )->Value(),
        GetItem( TUid::Uid( EIpsSetUiWhatDownloadSizeEditPlus ) )->Value() );
    StoreRetrievalLimit();

    iData->SetSchedule(
        GetItem( TUid::Uid( EIpsSetUiWhenSchedule ) )->Value(), EFalse );
    iData->SetDays(
        GetItem( TUid::Uid( EIpsSetUiWhenDays ) )->Value() );
    iData->SetHours(
        GetItem( TUid::Uid(
            EIpsSetUiWhenHoursEditCustomizeFrom ) )->Value(),
        GetItem( TUid::Uid(
            EIpsSetUiWhenHoursEditCustomizeTo ) )->Value() );

    iData->SaveL( iSession );
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::StoreRetrievalLimit()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::StoreRetrievalLimit()
    {
    FUNC_LOG;
    // Retrieve radio button values
    TInt popBtn = GetItem( TUid::Uid( EIpsSetUiWhatRetrievePop3 ) )->Value();
    TInt inboxBtn = GetItem( TUid::Uid(
        EIpsSetUiWhatRetrieveImap4Inbox ) )->Value();
    TInt folderBtn = GetItem( TUid::Uid(
        EIpsSetUiWhatRetrieveImap4Folders ) )->Value();

    // Retrieve editor values
    TInt popValue = GetItem(
        TUid::Uid( EIpsSetUiWhatRetrievePop3EditCustom ) )->Value();
    TInt inboxValue = GetItem( TUid::Uid(
            EIpsSetUiWhatRetrieveImap4InboxEditCustom ) )->Value();
    TInt folderValue = GetItem( TUid::Uid(
            EIpsSetUiWhatRetrieveImap4FolderEditCustom ) )->Value();

    iData->SetRetrieveLimit(
        popBtn == CIpsSetData::EAll ? KIpsSetMaxFetchHeadersDefaultLimit : popValue,
        inboxBtn == CIpsSetData::EAll ? KIpsSetMaxFetchHeadersDefaultLimit : inboxValue,
        folderBtn == CIpsSetData::EAll ? KIpsSetMaxFetchHeadersDefaultLimit : folderValue );
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::GetIapPref()
// ---------------------------------------------------------------------------
//
TImIAPChoice CIpsSetUiDialogCtrl::GetIapPref( const TInt aItemId )
    {
    FUNC_LOG;
    CIpsSetUiItemAccessPoint* ap =
        static_cast<CIpsSetUiItemAccessPoint*>( GetItem( aItemId ) );
    TImIAPChoice choice;
    //<cmail> different from FS since CIpsSetUiItemAccessPoint has differnet implementation
    //choice.iIAP = ap->iIapWapId; //In FS
    choice.iIAP = ap->iIapId;    // In cmail
    //</cmail>
    choice.iDialogPref = ap->iIapPref;
    return choice;
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::GetIapPrefsL()
// ---------------------------------------------------------------------------
//
CImIAPPreferences& CIpsSetUiDialogCtrl::GetIapPrefsL(const TUid& aId)
	{
	FUNC_LOG;
	
	CIpsSetUiItemAccessPoint* ap =
		static_cast<CIpsSetUiItemAccessPoint*>( GetItem( aId ) );
	
	CImIAPPreferences& prefs = ap->GetExtendedIapPreferencesL(); 
	
	return prefs;
	}

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::OfferKeyEventL()
// ---------------------------------------------------------------------------
//
TKeyResponse CIpsSetUiDialogCtrl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;
    TBool ok = EFalse;

    // Handle the key events for wizard
    if ( aType == EEventKey )
        {
        switch ( aKeyEvent.iCode )
            {
            // Ignore left and right keys
            case EKeyLeftArrow:
            case EKeyRightArrow:
                ok = ETrue;
                break;

            // Selection key
            case EKeyOK:
            case EKeyEnter:
                ok = EventHandlerL( EAknSoftkeyOk );
                break;

            // Red button
            case EKeyPhoneEnd:
                ok = EventHandlerL( EAknSoftkeyCancel );
                break;

            default:
                break;
            }
        }

    TKeyResponse ret = ok ? EKeyWasConsumed : EKeyWasNotConsumed;
    ret = ( ret == EKeyWasNotConsumed ) ?
        CIpsSetUiBaseDialog::OfferKeyEventL( aKeyEvent, aType ) :
        EKeyWasNotConsumed;

    if ( aType == EEventKeyDown )
        {
        ret = EKeyWasConsumed;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// From class CIpsSetUiBaseArray.
// CIpsSetUiDialogCtrl::CreateCustomItemToArrayLC()
// ---------------------------------------------------------------------------
//
CIpsSetUiItem* CIpsSetUiDialogCtrl::CreateCustomItemToArrayLC(
    const TUid& aId )
    {
    FUNC_LOG;
    CIpsSetUiItem* uiItem = NULL;
    switch ( aId.iUid )
        {
        case EIpsSetUiIncomingIap:
        case EIpsSetUiOutgoingIap:
            {
            uiItem = CIpsSetUiItemAccessPoint::NewLC();
            }
            break;
        case EIpsSetUiWhatFolderSync:
            {
            uiItem = CIpsSetUiItemLinkExt::NewLC();
            }
            break;
        default:
            break;
        }
    return uiItem;
    }

// ---------------------------------------------------------------------------
// From class CIpsSetUiBaseDialog.
// CIpsSetUiDialogCtrl::EventItemEditStartsL()
// ---------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiDialogCtrl::EventItemEditStartsL( CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    switch ( aBaseItem.iItemId.iUid )
        {
        case EIpsSetUiIncomingIap:
        case EIpsSetUiOutgoingIap:
            return HandleAccessPointOpeningL( aBaseItem );

        case EIpsSetUiIncomingPort:
        case EIpsSetUiOutgoingPort:
            return HandlePort();

        case EIpsSetUiWhatFolderSync:
            return HandleEventSubscribeFoldersL();

        default: break;
        }

    return EIpsSetUiPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::HandleEventSubscribeFoldersL()
// ----------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiDialogCtrl::HandleEventSubscribeFoldersL()
    {
    FUNC_LOG;
    // <cmail>
    // Run the dialog
    CAknQueryDialog* confDialog = CAknQueryDialog::NewL();
    if( confDialog->ExecuteLD( R_IPS_SET_CONNECT_FOR_UPDATE ) )
    	{
    	CMsvEntrySelection* mySelection=new (ELeave) CMsvEntrySelection;
    	CleanupStack::PushL(mySelection);

    	MFSMailRequestObserver* dummyFSObserver = NULL;
    	CIpsPlgTimerOperation* timer = NULL;
    	TFSMailMsgId mailboxId;
    	CIpsPlgConnectAndRefreshFolderList* op = NULL;
    	
    	// Create a waiter, which shows wait dialog
    	CIpsSetUtilsOperationWait* wait = CIpsSetUtilsOperationWait::NewLC();

    	
    	mailboxId.SetId( iDialog.MailboxId() );
    	mySelection->AppendL( iDialog.MailboxId() );
    	
    	op = CIpsPlgConnectAndRefreshFolderList::NewL( 
    			iSession, 
    			CActive::EPriorityStandard, 
    			wait->iStatus,//status,
    			iDialog.MailboxId(), 
    			mailboxId, 
    			*mySelection, 
    			*dummyFSObserver, 
    			*timer );
    	CleanupStack::PushL(op);

    	wait->StartAndShowWaitDialogL();
        TInt status = wait->iStatus.Int();
    	CleanupStack::PopAndDestroy(3, mySelection); // op, wait, myselection

        // Don't open the folder subscription list if we couldn't log on.
        if ( status == KErrImapBadLogon )
            {
            return EIpsSetUiPageEventResultCancel;
            }
    	}
    iDialog.SetIgnoreOneBackKey(EFalse); //<cmail>
    // </cmail>
    
    CIpsSetUiSubscriptionDialog* subDlg =
    CIpsSetUiSubscriptionDialog::NewL(
    		iFlags, iDialog.MailboxId(), iSession, iTitlePane,
    		*iMainMBoxMenuTitleText );

    subDlg->PrepareLC( R_IPS_SET_FOLDER_SUBSRCIPTION_DIALOG );
    subDlg->RunLD();
    	    
    return EIpsSetUiPageEventResultDisapproved;
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::HandleAccessPointOpeningL()
// ---------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiDialogCtrl::HandleAccessPointOpeningL(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    // OMG, here we go...
    CIpsSetUiItemAccessPoint& apItem =
        *static_cast<CIpsSetUiItemAccessPoint*>( &aBaseItem );

    // Continue at your own risk... think twice!
    apItem.LaunchL();

    return EIpsSetUiPageEventResultDisapproved;
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::HandlePort()
// ---------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiDialogCtrl::HandlePort()
    {
    FUNC_LOG;
    SetSettingPageResource(
        EIpsSetUiNumber, R_IPS_SETUI_SETTINGS_DIALOG_FIVE_DIGITS );

    return EIpsSetUiPageEventResultApproved;
    }

// ---------------------------------------------------------------------------
// From class CIpsSetUiBaseDialog.
// CIpsSetUiDialogCtrl::EventItemEditEndsL()
// ---------------------------------------------------------------------------
//
TInt CIpsSetUiDialogCtrl::EventItemEditEndsL( CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    switch ( aBaseItem.iItemId.iUid )
        {
        case EIpsSetUiMailboxIncludeSignature:
            SetHideItem(
                aBaseItem.Value() == EIpsSetUiOff,
                TUid::Uid( EIpsSetUiMailboxSignature ),
                EFalse );
            break;
        case EIpsSetUiIncomingSecurity:
            HandleEventPort( *GetItem( TUid::Uid( EIpsSetUiIncomingPort ) ) );
            break;
        case EIpsSetUiOutgoingSecurity:
            HandleEventPort( *GetItem( TUid::Uid( EIpsSetUiOutgoingPort ) ) );
            break;
        case EIpsSetUiIncomingPort:
        case EIpsSetUiOutgoingPort:
            HandleEventPort( aBaseItem );
            break;

        case EIpsSetUiIncomingUserName:
        case EIpsSetUiIncomingUserPwd:
            HandleEventAuthentication( *GetItem(
                TUid::Uid( EIpsSetUiOutgoingUserAuthentication ) ), EFalse );
            break;

        case EIpsSetUiOutgoingUserAuthentication:
            HandleEventAuthentication( aBaseItem, EFalse );
            break;

        case EIpsSetUiWhenDays:
            HandleEventDaysL( aBaseItem );
            break;

        case EIpsSetUiWhatDownloadSize:
            HandleDownloadSizeL( aBaseItem );
            break;

        case EIpsSetUiWhatRetrievePop3:
        case EIpsSetUiWhatRetrieveImap4Inbox:
        case EIpsSetUiWhatRetrieveImap4Folders:
            HandleEmailsToRetrieve( aBaseItem );
            break;

        default: break;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::HandleEmailsToRetrieve()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::HandleEmailsToRetrieve( CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    CIpsSetUiItemLink* linkArray = static_cast<CIpsSetUiItemLink*>( &aBaseItem );

    CIpsSetUiItem* subItem =
        GetSubItem( *linkArray, linkArray->Value() );

    TInt retlimit = 0;

    switch( subItem->iItemId.iUid )
        {
        case EIpsSetUiWhatRetrieveImap4InboxBtnCustom:
            {
            retlimit = GetItem( TUid::Uid(
                EIpsSetUiWhatRetrieveImap4InboxEditCustom ) )->Value();
            break;
            }
        case EIpsSetUiWhatRetrieveImap4FolderBtnCustom:
            {
            retlimit = GetItem( TUid::Uid(
                EIpsSetUiWhatRetrieveImap4FolderEditCustom ) )->Value();
            break;
            }
        case EIpsSetUiWhatRetrievePop3BtnCustom:
            {
            retlimit = GetItem( TUid::Uid(
                EIpsSetUiWhatRetrievePop3EditCustom ) )->Value();
            break;
            }
        default:
            {
            retlimit = KErrNotFound;
            break;
            }
        }

    if( retlimit >= 0)
        {
        CIpsSetUiItemLink& linkItem =
            *static_cast<CIpsSetUiItemLink*>( &aBaseItem );
        linkItem.iItemSettingText->Num( retlimit );
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::HandleDownloadSizeL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::HandleDownloadSizeL( CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    CIpsSetUiItemLink* linkArray = static_cast<CIpsSetUiItemLink*>( &aBaseItem );

    CIpsSetUiItem* subItem =
        GetSubItem( *linkArray, linkArray->Value() );

    if ( subItem->iItemId.iUid == EIpsSetUiWhatDownloadSizeBtnPlus )
        {
        RBuf resourceText;
        resourceText.CreateL( KIpsSetUiMaxSettingsLongTextLength );
        RBuf finalText;
        finalText.CreateL( KIpsSetUiMaxSettingsLongTextLength );

        TInt size = GetItem( TUid::Uid(
            EIpsSetUiWhatDownloadSizeEditPlus ) )->Value();

        // <cmail> ipssossettings to use correct loc strings in menus
        StringLoader::Load(
            resourceText,
            R_FSE_SETTINGS_MAIL_RETRIEVE_LESS_VALUE_PROMPT_MENU );
        StringLoader::Format( finalText, resourceText, KErrNotFound, size );
        // </cmail>

        CIpsSetUiItemLink& linkItem =
            *static_cast<CIpsSetUiItemLink*>( &aBaseItem );
        linkItem.iItemSettingText->Copy(
            finalText.Left( KIpsSetUiMaxSettingsLongTextLength ) );

        resourceText.Close();
        finalText.Close();
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::HandleEventDaysL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::HandleEventDaysL( CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    TInt resource = aBaseItem.Value() == KIpsSetUiAllDays ?
        R_FSE_SETTINGS_MAIL_CONN_DAYS_ALL :
        R_FSE_SETTINGS_MAIL_CONN_DAYS_SELECTED;

    HBufC* text = StringLoader::LoadL( resource );

    CIpsSetUiItemLink& linkItem =
        *static_cast<CIpsSetUiItemLink*>( &aBaseItem );
    linkItem.iItemSettingText->Copy(
        text->Left( KIpsSetUiMaxSettingsLongTextLength ) );

    delete text;
    text = NULL;
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::HandleEventPort()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::HandleEventPort( CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    TInt customport = 0;
    // When default key is set, set the port in editor to correct value
    if ( aBaseItem.Value() == EIpsSetUiDefault )
        {
        TInt port = 0;
        // Handle incoming port
        if ( aBaseItem.iItemId.iUid == EIpsSetUiIncomingPort )
            {
            // Determine the default port
            TUid id = TUid::Uid( EIpsSetUiIncomingSecurity );
            port = IpsSetUtils::GetDefaultSecurityPort(
                GetItem( id )->Value(),
                ETrue,
                iData->Protocol() == KSenduiMtmImap4Uid );

            // Set the port number to editor
            id = TUid::Uid( EIpsSetUiIncomingPortBtnEditor );
            GetItem( id )->SetValue( port );
            }
        // Handle outgoing port
        else
            {
            // Determine the default port
            TUid id = TUid::Uid( EIpsSetUiOutgoingSecurity );
            port = IpsSetUtils::GetDefaultSecurityPort(
                GetItem( id )->Value(),
                EFalse,
                iData->Protocol() == KSenduiMtmImap4Uid );

            // Set the port number to editor
            id = TUid::Uid( EIpsSetUiOutgoingPortBtnEditor );
            GetItem( id )->SetValue( port );
            }
        }
    //custom port settings
    else
        {
        //in
        if ( aBaseItem.iItemId.iUid == EIpsSetUiIncomingPort )
            {
            customport = GetItem( TUid::Uid(
                EIpsSetUiIncomingPortBtnEditor ) )->Value();
            }
        //out
        else
            {
            customport = GetItem( TUid::Uid(
                EIpsSetUiOutgoingPortBtnEditor ) )->Value();
            }
        CIpsSetUiItemLink& linkItem =
            *static_cast<CIpsSetUiItemLink*>( &aBaseItem );
        linkItem.iItemSettingText->Num( customport );
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::HandleEventAuthentication()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::HandleEventAuthentication(
    CIpsSetUiItem& aBaseItem,
    const TBool aForceClear )
    {
    FUNC_LOG;
    // Based on the authentication status...
    CIpsSetUiItem& username =
        *GetItem( TUid::Uid( EIpsSetUiOutgoingUserName ) );
    CIpsSetUiItem& password =
        *GetItem( TUid::Uid( EIpsSetUiOutgoingUserPwd ) );

    // ...hide the username&password -fields and clear them or...
    if ( aBaseItem.Value() == CIpsSetData::ENoAuth )
        {
        SetHideItem( ETrue, username );
        SetHideItem( ETrue, password, ETrue );
        username.SetText( KNullDesC );
        password.SetText( KNullDesC );
        }
    // ...copy the username&password from incoming settings or...
    else if ( aBaseItem.Value() == CIpsSetData::EUseIncoming )
        {
        SetHideItem( ETrue, username );
        SetHideItem( ETrue, password, ETrue );
        username.SetText( GetItem( TUid::Uid(
            EIpsSetUiIncomingUserName ) )->Text() );
        password.SetText( GetItem(
            TUid::Uid( EIpsSetUiIncomingUserPwd ) )->Text() );
        }
    // ...unhide username&password -fields and clear them.
    else
        {
        SetHideItem( EFalse, username );
        SetHideItem( EFalse, password, ETrue );

        // Do not clear the fields, when using own authentication and
        // incoming username/password has changed
        if ( aForceClear )
            {
            username.SetText( KNullDesC );
            password.SetText( KNullDesC );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitSignatureL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitSignatureL(
    CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    iSignature.CreateL( KIpsSetUiMaxSettingsSignatureLength );
    iData->Signature().iRichText->Extract(
        iSignature, 0, KIpsSetUiMaxSettingsSignatureLength );
    InitAnyItem( aBaseItem, 0, iSignature );
    iSignature.Close();

    // Make sure the signature should be shown
    CIpsSetUiItem* item =
        GetItem( TUid::Uid( EIpsSetUiMailboxIncludeSignature ) );
    SetHideItem(
        ( item->Value() == EIpsSetUiOff ),
        aBaseItem,
        ETrue );
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitPort()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitPort( CIpsSetUiItem& aItem )
    {
    FUNC_LOG;
    TBool incoming( aItem.iItemId.iUid == EIpsSetUiIncomingPort );

    // If the port is same as default port, set the default port -radiobutton
    // on, otherwise check the custom button
    TUid id = TUid::Uid(
        incoming ? EIpsSetUiIncomingSecurity : EIpsSetUiOutgoingSecurity );
    TInt defaultPort = IpsSetUtils::GetDefaultSecurityPort(
        GetItem( id )->Value(),
        incoming,
        iData->Protocol() == KSenduiMtmImap4Uid );
    TInt port = iData->Port( incoming );
    TInt button = ( defaultPort == port ) ? EIpsSetUiDefault : EIpsSetUiCustom;

    // Finally initialize items (both radiobutton editor and value editor)
    InitAnyItem( aItem, button, KNullDesC );
    InitAnyItem( PortItem( incoming, ETrue ), port, KNullDesC );
    HandleEventPort( aItem );
    }


// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::PortItem()
// ---------------------------------------------------------------------------
//
CIpsSetUiItem& CIpsSetUiDialogCtrl::PortItem(
    const TBool aIncoming,
    const TBool aEditor )
    {
    FUNC_LOG;
    if ( aIncoming )
        {
        return *GetItem( TUid::Uid( aEditor ?
            EIpsSetUiIncomingPortBtnEditor : EIpsSetUiIncomingPort ) );
        }
    else
        {
        return *GetItem( TUid::Uid( aEditor ?
            EIpsSetUiOutgoingPortBtnEditor : EIpsSetUiOutgoingPort ) );
        }
    }

// ----------------------------------------------------------------------------
// From class CIpsSetUiBaseArray.
// CIpsSetUiBaseArray::IsHidden()
// ----------------------------------------------------------------------------
//
TInt CIpsSetUiDialogCtrl::IsHidden(
    const CIpsSetUiItem& aItem ) const
    {
    FUNC_LOG;
    // The setting can be protocol specific, so make required protocol checks
    TBool onlyPop = aItem.iItemFlags.Flag32( KIpsSetUiFlagPop3Only );
    TBool onlyImap = aItem.iItemFlags.Flag32( KIpsSetUiFlagImap4Only );

    if ( onlyPop || onlyImap )
        {
        TBool isImap = ( iData->Protocol() == KSenduiMtmImap4Uid );
        return ( isImap == onlyPop || !isImap == onlyImap ) ?
            KErrNotSupported : CIpsSetUiBaseArray::IsHidden( aItem );
        }
    else
        {
        return CIpsSetUiBaseArray::IsHidden( aItem );
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::GetIapChoiceL()
// ---------------------------------------------------------------------------
//
TImIAPChoice CIpsSetUiDialogCtrl::GetIapChoiceL( const TUid& aId ) const
    {
    FUNC_LOG;
    CIpsSetUiItemAccessPoint* iapItem =
        static_cast<CIpsSetUiItemAccessPoint*>( GetItem( aId ) );

    TImIAPChoice choice;
    choice.iIAP = 0;
    choice.iDialogPref = iapItem->iIapPref;
    
    //<cmail> ap utils are not used in cmail
    // look into this later
    //TRAPD(error, choice.iIAP = iapItem->ApUtils().IapIdFromWapIdL(
    //    iapItem->GetWapIdL( iapItem->Value() ) ) );
    TRAPD(error, choice.iIAP = iapItem->GetIapIdL() );
    //<cmail>
    


    if ( error )
        {
        //force to always ask
        choice.iIAP = 0;
        choice.iDialogPref = ECommDbDialogPrefPrompt;
        }

    return choice;
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitIapL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitIapL( CIpsSetUiItem& aBaseItem )
    {
    FUNC_LOG;
    CIpsSetUiItemAccessPoint* iapItem =
        static_cast<CIpsSetUiItemAccessPoint*>( &aBaseItem );
    //check which iap we are initializing
    if ( aBaseItem.iItemId.iUid == EIpsSetUiIncomingIap )
    	iapItem->InitL( iData->IncomingIapPref() );
    else
    	iapItem->InitL( iData->OutgoingIapPref() );
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitRetrieve()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitRetrieve(
    CIpsSetUiItem& aBaseItem,
    const TInt aValue )
    {
    FUNC_LOG;
    TInt id = aBaseItem.iItemId.iUid;

    // Initialize radiobutton editor
    if ( id == EIpsSetUiWhatRetrievePop3 ||
         id == EIpsSetUiWhatRetrieveImap4Inbox ||
         id == EIpsSetUiWhatRetrieveImap4Folders )
        {
        TInt button = ( aValue == KIpsSetMaxFetchHeadersDefaultLimit ) ?
            EIpsSetUiDefault : EIpsSetUiCustom;
        InitAnyItem( aBaseItem, button, KNullDesC );
        HandleEmailsToRetrieve( aBaseItem );
        }
    // Initialize value editor
    else if ( id == EIpsSetUiWhatRetrievePop3EditCustom ||
        id == EIpsSetUiWhatRetrieveImap4InboxEditCustom ||
        id == EIpsSetUiWhatRetrieveImap4FolderEditCustom )
        {
        InitAnyItem(
            aBaseItem,
            ( aValue == KIpsSetMaxFetchHeadersDefaultLimit ) 
                ? KIpsSetDefaultLimit : aValue,
            KNullDesC );
        }
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitHoursL()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitHoursL(
    CIpsSetUiItem& aBaseItem,
    const TInt64 aStartTime,
    const TInt64 aStopTime )
    {
    FUNC_LOG;
    // Initialize hour items
    if ( aBaseItem.iItemId.iUid == EIpsSetUiWhenHours )
        {
        if ( aStartTime == aStopTime )
            {
            HBufC* plaa = StringLoader::LoadL(
                R_FSE_SETTINGS_MAIL_CONN_HOURS_ALL );
            InitAnyItem( aBaseItem, CIpsSetData::EAllDay, *plaa );

            delete plaa;
            plaa = NULL;
            }
        else
            {
            RBuf plaa;
            plaa.CreateL( KIpsSetUiMaxPasswordLength );
            FormatTimeStringL( plaa, aStartTime, aStopTime );
            InitAnyItem( aBaseItem, CIpsSetData::ECustomHours, plaa );

            plaa.Close();
            }
        }
    else if ( aBaseItem.iItemId.iUid == EIpsSetUiWhenHoursEditCustomizeFrom )
        {
        CIpsSetUiItem* timeItem = static_cast<CIpsSetUiItem*>( &aBaseItem );
        timeItem->SetValue( aStartTime );
        }
    else // EIpsSetUiWhenHoursEditCustomizeTo
        {
        CIpsSetUiItem* timeItem = static_cast<CIpsSetUiItem*>( &aBaseItem );
        timeItem->SetValue( aStopTime );
        }
    }

// ----------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::FormatTimeStringL()
// ----------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::FormatTimeStringL(
    TDes& aText,
    const TInt64 aStartTime,
    const TInt64 aStopTime ) const
    {
    FUNC_LOG;
    // Define variables for time
    TTime thisTime = aStartTime;
    TTime nextTime = aStopTime;

    HBufC* format = StringLoader::LoadLC(
        R_QTN_TIME_USUAL_WITH_ZERO );

    // Copy the time to text
    RBuf timeText1;
    RBuf timeText2;
    timeText1.CreateL( KIpsSetUiMaxPasswordLength );
    timeText2.CreateL( KIpsSetUiMaxPasswordLength );
    thisTime.FormatL( timeText1, *format );
    nextTime.FormatL( timeText2, *format );
    CleanupStack::PopAndDestroy( format );
    format = NULL;

    RBuf tempText1;
    RBuf tempText2;
    tempText1.CreateL( KIpsSetUiMaxPasswordLength );
    tempText2.CreateL( KIpsSetUiMaxPasswordLength );

    // Load the template
    StringLoader::Load(
        tempText1, R_FSE_SETTINGS_MAIL_CONN_HOURS_BETWEEN2 );

    // Fetch the times
    StringLoader::Format( tempText2, tempText1, 0, timeText1 );
    StringLoader::Format( tempText1, tempText2, 1, timeText2 );

    // Finally copy the text to text
    aText.Copy( tempText1 );

    timeText1.Close();
    timeText2.Close();
    tempText1.Close();
    tempText2.Close();
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitLogin()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitLogin(
    CIpsSetUiItem& aBaseItem,
    const TDesC& aLogin )
    {
    FUNC_LOG;
    InitAnyItem( aBaseItem, 0, aLogin );
    CIpsSetUiItem& auth =
        *GetItem( TUid::Uid( EIpsSetUiOutgoingUserAuthentication ) );
    SetHideItem( auth.Value() != CIpsSetData::EOwnOutgoing, aBaseItem );
    }

// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitRoaming()
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitRoaming(
    CIpsSetUiItem& aBaseItem,
    const TInt aValue )
    {
    FUNC_LOG;
    InitAnyItem( aBaseItem, aValue, KNullDesC );
    CIpsSetUiItem& interaval =
        *GetItem( TUid::Uid( EIpsSetUiWhenSchedule ) );
    SetHideItem( interaval.Value() == CIpsSetData::EManual, aBaseItem );
    }

//<cmail>
// ---------------------------------------------------------------------------
// CIpsSetUiDialogCtrl::InitSecuritySetting
// ---------------------------------------------------------------------------
//
void CIpsSetUiDialogCtrl::InitSecuritySetting(
    CIpsSetUiItem& aBaseItem,
    const TInt aValue )
    {
    InitAnyItem( aBaseItem, aValue, KNullDesC );    
        
    if ( iFlags & EIpsSetUiHideAccountData )
        {
        //editing security settings in hidden mode is prohibited
        aBaseItem.iItemFlags.SetFlag32( KIpsSetUiFlagReadOnly );
        }
    else 
        {
        aBaseItem.iItemFlags.ClearFlag32( KIpsSetUiFlagReadOnly );
        }
    }
    
//</cmail>

// ---------------------------------------------------------------------------
// From class CIpsSetUiBaseDialog.
// CIpsSetUiDialogCtrl::SettingLaunchMultilineEditorL()
// ---------------------------------------------------------------------------
//
TIpsSetUiEventResult CIpsSetUiDialogCtrl::SettingLaunchMultilineEditorL(
    CIpsSetUiItem& /* aBaseItem */,
    TIpsSetUiUpdateMode& /* aUpdateMode */ )
    {
    FUNC_LOG;
    CIpsSetUiItem* sub1 = GetItem( TUid::Uid(
        EIpsSetUiWhenHoursEditCustomizeFrom ) );
    CIpsSetUiItem* sub2 = GetItem( TUid::Uid(
        EIpsSetUiWhenHoursEditCustomizeTo ) );

    // Take times
    TTime thisTime = sub1->Value();
    TTime nextTime = sub2->Value();

    // Set default values to start with, if setting has been 'always on' before
    if( thisTime == nextTime )
        {
        thisTime = KIpsSetDataTimeDialogDefaultStart;
        nextTime = KIpsSetDataTimeDialogDefaultStop;
        }

    CAknMultiLineDataQueryDialog* multiLineDlg =
        CAknMultiLineDataQueryDialog::NewL( thisTime, nextTime );

    // Execute the dialog
    if ( multiLineDlg->ExecuteLD( R_IPS_SETUI_SETTINGS_DIALOG_HOURS_QUERY ) )
        {
        sub1->SetValue( thisTime.Int64() );
        sub2->SetValue( nextTime.Int64() );

        CIpsSetUiItem& radio = *GetItem( TUid::Uid( EIpsSetUiWhenHours ) );
        InitHoursL( radio, thisTime.Int64(), nextTime.Int64() );

        return EIpsSetUiPageEventResultApproved;
        }
    else
        {
        return EIpsSetUiPageEventResultCancel;
        }
    }

// End of File

