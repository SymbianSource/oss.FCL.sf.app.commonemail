/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRclSearchResultDlg.
*
*/


#include "emailtrace.h"
#include <pbkxrclengine.rsg>
#include <eikmover.h>
#include <coemain.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <aknappui.h>
#include <AknIconUtils.h>
#include <cntitem.h>
#include <badesca.h>
#include <StringLoader.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <gulicon.h>
#include <avkon.mbg>
#include <pbkxrclengine.mbg>
//<cmail>
#include "cfsccontactactionmenu.h"  
#include "mfsccontactactionmenumodel.h"
#include "mfsccontactaction.h"
#include "cfsccontactactionmenuitem.h"
#include "fsccontactactionservicedefines.h"
#include "fsccontactactionmenudefines.h"
//</cmail>

#include <data_caging_path_literals.hrh>
#include <AknsUtils.h>
#include <eikclb.h>
#include <eikclbd.h>
#include <eiklbo.h>                 // MEikListBoxObserver

#include <csxhelp/cmail.hlp.hrh>

#include "cpbkxrclsearchresultdlg.h"
#include "pbkxremotecontactlookuppanic.h"
#include "engine.hrh"
#include "pbkxrclutils.h"
#include "cpbkxrclvcardsender.h"
#include "cpbkxrclactionservicewrapper.h"

// list box field formats
_LIT( KFieldFormat, "\t%S %S" );
_LIT( KFieldFormatWithIcon, "\t%S %S\t%d" );

_LIT( KFieldFormatDispName, "\t%S" );
_LIT( KFieldFormatDispNameWithIcon, "\t%S\t%d" );

// Constant to measure when user is scrolling.
const TInt KKeyScrolling = 1;
// <cmail> S60 UID update
const TInt KPbkxRemoteContactLookupServiceImplImpUid = 0x2001FE0D;
// </cmail> S60 UID update


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultDlg* CPbkxRclSearchResultDlg::NewL(
    TInt& aIndex,
    CDesCArray* aArray,
    MEikCommandObserver* aCommand,
    RPointerArray<CContactCard>& aContactItems,
    CPbkContactEngine& aContactEngine,
    CPbkxRclActionServiceWrapper& aActionService,
    TBool aContactSelectorEnabled )
    {
    FUNC_LOG;
    CPbkxRclSearchResultDlg* dialog = new ( ELeave ) CPbkxRclSearchResultDlg(
        aIndex, 
        aArray,
        aCommand,
        aContactItems,
        aContactEngine,
        aActionService,
        aContactSelectorEnabled );

    CleanupStack::PushL( dialog );
    dialog->ConstructL();
    CleanupStack::Pop( dialog );
    return dialog;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CPbkxRclSearchResultDlg
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultDlg::CPbkxRclSearchResultDlg(
    TInt& aIndex,
    CDesCArray* aArray,
    MEikCommandObserver* aCommand,
    RPointerArray<CContactCard>& aContactItems,
    CPbkContactEngine& aContactEngine,
    CPbkxRclActionServiceWrapper& aActionService,
    TBool aContactSelectorEnabled ) :
    CPbkxRclBaseDlg( aIndex, aArray, aCommand, ETrue ), 
    iContactEngine( aContactEngine ),
    iContactActionService( aActionService ),
    iItems( aArray ), iContactItems( aContactItems ), 
    iContactSelectorEnabled( aContactSelectorEnabled ), 
    iSelectedItemIndex( aIndex ), 
    iIsInfoDlgVisible( EFalse )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ConstructL()
    {
    FUNC_LOG;
    CPbkxRclBaseDlg::ConstructL( R_RCL_SEARCH_RESULT_DIALOG_MENU_BAR );
    LoadNaviTextL();

    // add foreground observer
    CCoeEnv::Static()->AddForegroundObserverL( *this );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::~CPbkxRclSearchResultDlg
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultDlg::~CPbkxRclSearchResultDlg()
    {
    FUNC_LOG;
    CCoeEnv::Static()->RemoveForegroundObserver( *this );
    ResetStatusPane();
    iNaviText.Close();
    delete iAddRecipientIcon;

    iContactActionService.CancelQuery();
    if ( iActionMenu )
    	{
     	delete iActionMenu;
    	iActionMenu = NULL;
    	}
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ExecuteLD
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ExecuteLD()
    {
    FUNC_LOG;

    PrepareLC( R_RCL_SEARCH_RESULT_DIALOG );

    UpdateGraphics();
    UpdateColors();

    HBufC* noItemText = StringLoader::LoadLC( R_QTN_RCL_NO_RESULTS );
    ListBox()->View()->SetListEmptyTextL( *noItemText );
    CleanupStack::PopAndDestroy( noItemText );

    CreateListBoxIconsL();

    InitializeActionMenuL();

    SetupStatusPaneL();
    CreateListBoxEntriesL();
    
    if ( iContactItems.Count() == 0 )
      {
      ConstructMenuBarL( R_RCL_OPTIONS_BACK_CONTEXTMENU );
      }

    CAknSelectionListDialog::RunLD();
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CurrentItemIndex
// ---------------------------------------------------------------------------
//
TInt CPbkxRclSearchResultDlg::CurrentItemIndex() const
    {
    FUNC_LOG;
    return ListBox()->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::SetCurrentItemIndex
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::SetCurrentItemIndexL( TInt aIndex )
    {
    FUNC_LOG;
    if ( aIndex >= 0 && aIndex < iContactItems.Count() )
        {
        // update list box entrys first
        TInt oldIndex = iSelectedItemIndex;
        
        ListBox()->SetCurrentItemIndex( aIndex );
        iSelectedItemIndex = aIndex;
        
        UpdateListBoxEntryL( iSelectedItemIndex );
        UpdateListBoxEntryL( oldIndex );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::Close
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::Close()
    {
    FUNC_LOG;
    // this will not leave, because TryExitL doesn't call leaving methods
    // when EAknSoftkeyCancel is used
    iExit = ETrue;
    TRAP_IGNORE( TryExitL( EAknSoftkeyCancel ) ); // <cmail>
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::UpdateDialogL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::UpdateDialogL()
    {
    FUNC_LOG;
    LoadNaviTextL();
    CreateListBoxEntriesL();
    UpdateStatusPaneL();
    
    if ( iContactItems.Count() == 0 )
        {
        ConstructMenuBarL( R_RCL_OPTIONS_BACK_CONTEXTMENU );
        }
    else
        {
        ConstructMenuBarL( R_RCL_OPTIONS_BACK_OPEN );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::SendBusinessCardL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::SendBusinessCardL()
    {
    FUNC_LOG;
    CPbkxRclvCardSender* sender = CPbkxRclvCardSender::NewLC( iContactEngine );

    TInt curIndex = CurrentItemIndex();


    CContactCard* current = iContactItems[curIndex];
   
    CPbkContactItem* contactItem = PbkxRclUtils::CreateContactItemL(
        current,
        iContactEngine );
    CleanupStack::PushL( contactItem );
    
    sender->SendvCardL( *contactItem, *(iContactActionService.ContactConverter()) );

    CleanupStack::PopAndDestroy( contactItem );
    CleanupStack::PopAndDestroy( sender );
    
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::DynInitMenuPaneL( 
    TInt aResourceId, 
    CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    if ( aResourceId == R_RCL_SEARCH_RESULT_DIALOG_MENU_PANE )
        {

        if ( iContactItems.Count() == 0 )
            {
            // when there are 0 results, only new search option is available
            aMenuPane->SetItemDimmed( ERclCmdViewDetails, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdAddAsRecipient, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdSaveToContacts, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdCall, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdSend, ETrue );
            }
        else
            {
            if ( !iContactSelectorEnabled )
                {
                
                aMenuPane->SetItemDimmed( ERclCmdAddAsRecipient, ETrue );
                
                aMenuPane->SetItemDimmed(
                    ERclCmdCall,
                    !CallActionsAvailable() );
                
                // send menu is always visible, since business card can 
                // always be sent
                }
            else
                {
                aMenuPane->SetItemDimmed( ERclCmdCall, ETrue );
                aMenuPane->SetItemDimmed( ERclCmdSend, ETrue );
                }
            
            aMenuPane->SetItemDimmed(
                ERclCmdSaveToContacts,
                !iContactActionService.IsActionEnabled( KFscAtManSaveAs ) );
            }
        }
    else if ( aResourceId == R_RCL_CALL_MENU_PANE )
        {
        
        aMenuPane->SetItemDimmed( 
            ERclCmdVoiceCall,
            !iContactActionService.IsActionEnabled( KFscAtComCallGSM ) );

        aMenuPane->SetItemDimmed(
            ERclCmdVideoCall,
            !iContactActionService.IsActionEnabled( KFscAtComCallVideo ) );

        aMenuPane->SetItemDimmed(
            ERclCmdVoip,
            !iContactActionService.IsActionEnabled( KFscAtComCallVoip ) );
        
        }
    else if ( aResourceId == R_RCL_SEND_MENU_PANE )
        {       
        aMenuPane->SetItemDimmed( 
            ERclCmdSendMsg, 
            !iContactActionService.IsActionEnabled( KFscAtComSendMsg ) );
       
        aMenuPane->SetItemDimmed( 
            ERclCmdSendEmail, 
            !iContactActionService.IsActionEnabled( KFscAtComSendEmail ) );
      
        aMenuPane->SetItemDimmed( 
            ERclCmdSendMeetingReq, 
            ETrue /*!iContactActionService.IsActionEnabled( KFscAtComSendCalReq )*/ );
            // "Send meeting request" option should not be available
                
        aMenuPane->SetItemDimmed( 
            ERclCmdSendAudioMsg, 
            !iContactActionService.IsActionEnabled( KFscAtComSendAudio ) );
        
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::GetHelpContext
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    FUNC_LOG;

    if ( !iIsInfoDlgVisible )
        {
        aContext.iMajor = TUid::Uid( KPbkxRemoteContactLookupServiceImplImpUid );
        aContext.iContext = KRLOOK_HLP_RESULT_VIEW;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::HandleGainingForeground
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::HandleGainingForeground()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::HandleLosingForeground
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::HandleLosingForeground()
    {
    FUNC_LOG;
    if ( ScrollingInProgress() )
        {
        // scrolling was in progress
        TRAP_IGNORE( ScrollingStoppedL() );
        iKeyDown = EFalse;
        iKeyCounter = 0;
        }
    }

// ----------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::HandleListBoxEventL()
// ----------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::HandleListBoxEventL(
    CEikListBox* /* aListBox */,
    TListBoxEvent aEventType )
    {
    FUNC_LOG;
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemClicked:
        	{
        	if ( iContactItems.Count() > 0 )
                {
                TInt oldIndex = iSelectedItemIndex;
                iSelectedItemIndex = CurrentItemIndex();
                if ( oldIndex != iSelectedItemIndex )
                    {
                    UpdateListBoxEntryL( oldIndex );
                    // update listbox entry if we are not yet fast scrolling
                    if ( !ScrollingInProgress() )
                        {
                        SetCurrentContactToActionServiceL();
                        UpdateListBoxEntryL( iSelectedItemIndex );
                        }
                    }
                }
        	break;
        	}
        case EEventItemActioned:
            {
            break;
            }
            
        case EEventItemDoubleClicked:
        	{
            ProcessCommandL( ERclCmdViewDetails );
        	break;
        	}
        	
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CPbkxRclSearchResultDlg::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    FUNC_LOG;

    // if there are no items, don't handle key presses
    TInt count = iContactItems.Count();

    if ( iExit || aType != EEventKey )
        {
        if ( aType == EEventKeyUp )
            {
            // added count condition
            if ( count && ScrollingInProgress() && !iIsScrollLaunched )
                {
                // scrolling ended
                iIsScrollLaunched = ETrue;
                ScrollingStoppedL();
                }
            iKeyCounter = 0;
            iKeyDown = EFalse;
            }
        
        return EKeyWasConsumed;
        }
    
    if ( aKeyEvent.iCode == EKeyUpArrow || 
         aKeyEvent.iCode == EKeyDownArrow )
        {
        iKeyDown = ETrue;
        iKeyCounter++;
        }
    else
        {
        // make sure that if we were scrolling we stop it now
        if ( ScrollingInProgress() )
            {
            ScrollingStoppedL();
            }
        iKeyDown = EFalse;
        iKeyCounter = 0;
        }
    
    switch ( aKeyEvent.iCode )
        {
        case EKeyPhoneSend:
            {
            ProcessCommandL( ERclCmdVoiceCall );
            break;
            }
        case EKeyOK:
            {
            if ( count > 0 )
                {
                // opens result information view
                ProcessCommandL( ERclCmdViewDetails );
                }
            }
            break;
        case EKeyUpArrow: // fall through, same functionality with these two
        case EKeyDownArrow:
            {
            if ( count > 0 )
                {
                TInt oldIndex = iSelectedItemIndex;
                CAknSelectionListDialog::OfferKeyEventL( aKeyEvent, aType );
                iSelectedItemIndex = CurrentItemIndex();
                if ( oldIndex != iSelectedItemIndex )
                    {
                    UpdateListBoxEntryL( oldIndex );
                    // update listbox entry if we are not yet fast scrolling
                    if ( !ScrollingInProgress() )
                        {
                        SetCurrentContactToActionServiceL();
                        UpdateListBoxEntryL( iSelectedItemIndex );
                        }
                    }
                }
            }
            break;
        case EKeyRightArrow:
            {
            if ( !AknLayoutUtils::LayoutMirrored() && ActionMenuAvailable() )
                {
                // opens action menu
                ShowActionMenuL();
                }
            }
            break;
        case EKeyLeftArrow:
            {
            if ( AknLayoutUtils::LayoutMirrored() && ActionMenuAvailable() )
                {
                ShowActionMenuL();
                }
            }
            break;
        case EKeyEscape:
            {
            // exit application
            iExit = ETrue;
            ProcessCommandL( EAknCmdExit );
            }
            break;
        default:
            break;
        }
    return EKeyWasConsumed;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CPbkxRclSearchResultDlg::OkToExitL( TInt aButtonId )
	{
    FUNC_LOG;
	if( aButtonId == EAknSoftkeyOpen )
		{
		ProcessCommandL( ERclCmdViewDetails );
		return EFalse;
		}
	else if ( aButtonId == EAknSoftkeyContextOptions )
	    {
	    iMenuBar->TryDisplayMenuBarL();
        return EFalse;
	    }
	else
		{
		return CAknDialog::OkToExitL( aButtonId );
		}
	}

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::SetupStatusPaneL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::SetupStatusPaneL()
    {
    FUNC_LOG;
    CAknAppUi* appUi = static_cast<CAknAppUi*>( CCoeEnv::Static()->AppUi() );
    CEikStatusPane* statusPane = appUi->StatusPane();
    iNaviPane = reinterpret_cast<CAknNavigationControlContainer*>(
        statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( iNaviText );
    
    iNaviPane->PushL( *iNaviDecorator );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::UpdateStatusPaneL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::UpdateStatusPaneL()
    {
    FUNC_LOG;
    
    ResetStatusPane();
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( iNaviText );
    iNaviPane->PushL( *iNaviDecorator );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ResetStatusPane
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ResetStatusPane()
    {
    FUNC_LOG;
    // navi pane and navi decorator may be null if dialog construction
    // failed
    if ( iNaviPane != NULL && iNaviDecorator != NULL )
        {
        iNaviPane->Pop( iNaviDecorator );
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CreateListBoxEntriesL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::CreateListBoxEntriesL()
    {
    FUNC_LOG;
    iItems->Delete( 0, iItems->Count() );
    TInt count = iContactItems.Count();
    
    if ( count > 0 )
        {
        iSelectedItemIndex = 0;
        }
    
    SetCurrentContactToActionServiceL();

    for ( TInt i = 0; i < count; i++ )
        {
        CreateListBoxEntryL( i );
        }
    
    CEikListBox* listBox = ListBox();

    if ( count > 0 )
        {
        listBox->SetTopItemIndex( 0 );
        listBox->SetCurrentItemIndex( 0 );
        }

    listBox->HandleItemAdditionL();
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CreateListBoxIconsL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::CreateListBoxIconsL()
    {
    FUNC_LOG;
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    TFileName dllFileName; 
    Dll::FileName( dllFileName );
    TParse parse;
    parse.Set( KEngineIconFile, &KDC_APP_RESOURCE_DIR, &dllFileName );

// <cmail> icons changed
    if ( iContactSelectorEnabled )
        {
        // add as recipient icon
        AknsUtils::CreateIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnFsRclActionRecipient,
            bitmap,
            mask,
            parse.FullName(),
            EMbmPbkxrclengineQgn_prop_cmail_contacts_group,
            EMbmPbkxrclengineQgn_prop_cmail_contacts_group_mask );
        
        CleanupStack::PushL( bitmap );
        CleanupStack::PushL( mask );
        
        iAddRecipientIcon = CGulIcon::NewL( bitmap, mask );
        
        CleanupStack::Pop( mask );
        CleanupStack::Pop( bitmap );
        }
    
    // action menu icon
    AknsUtils::CreateIconL(
        AknsUtils::SkinInstance(),
        KAknsIIDQgnFsHscrActionArrowRight,
        bitmap,
        mask,
        parse.FullName(),
        EMbmPbkxrclengineQgn_indi_cmail_action_arrow_right,
        EMbmPbkxrclengineQgn_indi_cmail_action_arrow_right_mask );
// </cmail>    
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );
    
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::PushL( icon );

    CArrayPtr<CGulIcon>* iconArray = 
        new ( ELeave ) CArrayPtrFlat<CGulIcon>( KArrayGranularity );
    CleanupStack::PushL( iconArray );
    iconArray->AppendL( icon );

    SetIconArrayL( iconArray );

    CleanupStack::Pop( iconArray );

    CleanupStack::Pop( icon );
    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );
        
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CreateListBoxEntryL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::CreateListBoxEntryL( TInt aIndex )
    {
    FUNC_LOG;
     
    CContactCard* card = iContactItems[aIndex];
    TPtrC dispName = PbkxRclUtils::FieldText( card, KUidContactFieldDefinedText );
    TPtrC firstName = PbkxRclUtils::FieldText( card, KUidContactFieldGivenName );
    TPtrC lastName = PbkxRclUtils::FieldText( card, KUidContactFieldFamilyName );
    
    RBuf entry;
    TPtrC entryFirstName;

    if ( dispName.Length() != 0 )
        {
        entryFirstName.Set(dispName);
        entry.CreateL( entryFirstName.Length() + KExtraSpaceInListboxEntry );
        }
    else
        {
        entryFirstName.Set(firstName);
        entry.CreateL( firstName.Length() + lastName.Length() + KExtraSpaceInListboxEntry );
        }
    
    CleanupClosePushL( entry );
    if ( dispName.Length() != 0 )
        {
        if ( aIndex == iSelectedItemIndex && ActionMenuAvailable() )
            {
            entry.Format( 
                KFieldFormatDispNameWithIcon, 
                &entryFirstName, 
                EActionMenuEnabled );
            }
        else
            {
            entry.Format( KFieldFormatDispName, &entryFirstName );
            }
        }
    else
        {
        if ( aIndex == iSelectedItemIndex && ActionMenuAvailable() )
            {
            entry.Format( 
                KFieldFormatWithIcon, 
                &firstName, 
                &lastName, 
                EActionMenuEnabled );
            }
        else
            {
            entry.Format( KFieldFormat, &firstName, &lastName );
            }
        }
    iItems->InsertL( aIndex, entry );
    CleanupStack::PopAndDestroy( &entry );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::UpdateListBoxEntryL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::UpdateListBoxEntryL( TInt aIndex )
    {
    FUNC_LOG;
    iItems->Delete( aIndex );
    CreateListBoxEntryL( aIndex );
    ListBox()->DrawItem( aIndex );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::SetCurrentContactToActionServiceL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::SetCurrentContactToActionServiceL()
    {
    FUNC_LOG;
   
    if ( iActionMenu )
    	{
        if ( iContactItems.Count() > 0 )
            {
            // set current contact to action service
        
            CPbkContactItem* currentContact = PbkxRclUtils::CreateContactItemL(
                iContactItems[iSelectedItemIndex],
                iContactEngine );
            CleanupStack::PushL( currentContact );
    
            iContactActionService.SetCurrentContactL( 
                currentContact );
        
            CleanupStack::PopAndDestroy( currentContact );
        
            }
        else
            {
            iContactActionService.SetCurrentContactL( NULL );
            }
    
    	if ( iActionMenu )
			{
            // update action menu
            iActionMenu->Model().AddPreQueriedCasItemsL();
            // save as contact not added for this contact
            iSaveAsAdded = EFalse;
            }
    	} 
    iIsScrollLaunched = EFalse;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::LoadNaviTextL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::LoadNaviTextL()
    {
    FUNC_LOG;
    iNaviText.Close();
    HBufC* naviText = NULL;
    if ( iContactItems.Count() == 1 )
        {
        naviText = StringLoader::LoadLC( R_QTN_RCL_ONE_RESULT_NAVI );
        }
    else if ( iContactItems.Count() == 0 )
        {
        naviText = NULL;
        }
    else
        {
        naviText = StringLoader::LoadLC( 
            R_QTN_RCL_RESULT_NAVI,
            iContactItems.Count() );
        }
    
    if (naviText)
        {
        iNaviText.CreateL( *naviText );
        CleanupStack::PopAndDestroy( naviText );
        }
    else
        {
        iNaviText = KNullDesC;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CallActionsAvailable
// ---------------------------------------------------------------------------
//
TBool CPbkxRclSearchResultDlg::CallActionsAvailable() const
    {
    FUNC_LOG;
    return iContactActionService.IsActionEnabled( KFscAtComCallGSM ) ||
        iContactActionService.IsActionEnabled( KFscAtComCallVideo );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::InitializeActionMenuL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::InitializeActionMenuL()
    {
    FUNC_LOG;
    
    iActionMenu = CFscContactActionMenu::NewL( 
        *(iContactActionService.ActionService()) );

    MFscContactActionMenuModel& model = iActionMenu->Model();

    if ( iContactSelectorEnabled && iContactItems.Count() > 0 )
        {

        // add static custom item
        HBufC* menuText = StringLoader::LoadLC( 
            R_QTN_RCL_ADD_RECIPIENT_ACTION_MENU_TEXT );
        
        CFscContactActionMenuItem* item = model.NewMenuItemL(
            *menuText,
            iAddRecipientIcon,
            KPbkxRclAddRecipientPriority,
            ETrue,
            KPbkxRclAddRecipientActionUid );
        
        CleanupStack::PushL( item );
        
        model.AddItemL( item );
        
        CleanupStack::Pop( item );
        CleanupStack::PopAndDestroy( menuText );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ShowActionMenuL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ShowActionMenuL()
    {
    FUNC_LOG;

    CreateAndAddSaveAsContactActionL();
    
    TFscContactActionMenuPosition pos = GetActionMenuPosition();
    TFscActionMenuResult result = iActionMenu->ExecuteL( pos );

    // if custom item was selected, it was our add to recipient or
    // save as contact
    if ( result == EFscCustomItemSelected )
        {
        const TFscContactActionQueryResult* saveAsContact = 
            iContactActionService.GetResult( KFscAtManSaveAs );
        TInt itemIndex = iActionMenu->FocusedItemIndex();
        CFscContactActionMenuItem& item = 
            iActionMenu->Model().ItemL( itemIndex );
        
        if ( item.ImplementationUid() == KPbkxRclAddRecipientActionUid )
            {
            ProcessCommandL( ERclCmdAddAsRecipient );
            }
        else if ( saveAsContact != NULL &&
                  item.ImplementationUid() == saveAsContact->iAction->Uid() ) 
            {
            ProcessCommandL( ERclCmdSaveToContacts );
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::GetActionMenuPosition
// ---------------------------------------------------------------------------
//
TFscContactActionMenuPosition CPbkxRclSearchResultDlg::GetActionMenuPosition()
    {
    FUNC_LOG;
    const TInt KNumberOfMenuPositions = 3;

    CEikListBox* listBox = ListBox();
    TInt itemsInView = listBox->View()->NumberOfItemsThatFitInRect( Rect() );

    TInt topIndex = listBox->TopItemIndex();
    TInt curIndex = listBox->CurrentItemIndex();

    TInt normalizedIndex = curIndex - topIndex;
    
    TInt segmentSize = itemsInView / KNumberOfMenuPositions;

    if ( normalizedIndex < segmentSize )
        {
        return EFscTop;
        }
    else if ( normalizedIndex < ( 2 * segmentSize) )
        {
        return EFscCenter;
        }
    else
        {
        return EFscBottom;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ActionMenuAvailable
// ---------------------------------------------------------------------------
//
TBool CPbkxRclSearchResultDlg::ActionMenuAvailable() const
    {
    FUNC_LOG;
    TInt count = iActionMenu->Model().ItemCount() > 0;
    if ( !iSaveAsAdded )
        {
        if ( iContactActionService.IsActionEnabled( KFscAtManSaveAs ) )
            {
            count++;
            }
        }
    
    return count > 0;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CreateAndAddSaveAsContactActionL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::CreateAndAddSaveAsContactActionL()
    {
    FUNC_LOG;
    if ( !iSaveAsAdded )
        {
        const TFscContactActionQueryResult* saveAsContact = 
            iContactActionService.GetResult( KFscAtManSaveAs );
        if ( saveAsContact != NULL )
            {
            MFscContactActionMenuModel& model = iActionMenu->Model();
            
            const MFscContactAction* action = saveAsContact->iAction;
            
            CFscContactActionMenuItem* item = model.NewMenuItemL(
                action->ActionMenuText(),
                action->Icon(),
                saveAsContact->iPriority,
                EFalse,
                action->Uid() );
            
            CleanupStack::PushL( item );
            
            model.AddItemL( item );
            
            iSaveAsAdded = ETrue;

            CleanupStack::Pop( item );
            }
        }
    }


// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ScrollingInProgress
// ---------------------------------------------------------------------------
//
TBool CPbkxRclSearchResultDlg::ScrollingInProgress() const
    {
    FUNC_LOG;
    return iKeyDown && iKeyCounter > KKeyScrolling;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ScrollingStopped
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ScrollingStoppedL()
    {
    FUNC_LOG;
    SetCurrentContactToActionServiceL();
    UpdateListBoxEntryL( iSelectedItemIndex ); 
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ConstructContextMenuL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ConstructMenuBarL( TInt aResourceId )
    {
    FUNC_LOG;
    CEikButtonGroupContainer& butContainer = ButtonGroupContainer();
    butContainer.SetCommandSetL( aResourceId );
    butContainer.DrawNow();
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::InfoDlgVisible
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::InfoDlgVisible( TBool aVisible )
    {
    FUNC_LOG;
    iIsInfoDlgVisible = aVisible;
    }
