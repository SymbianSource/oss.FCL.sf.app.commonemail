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
* Description:  Implementation of the class CPbkxRclResultInfoDlg.
*
*/


#include "emailtrace.h"
#include <pbkxrclengine.rsg>
#include <cntitem.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <aknappui.h>
#include <akntabgrp.h>
#include <akntitle.h>
#include <StringLoader.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldInfo.h>
#include <eikfrlb.h>
#include <eikfrlbd.h>
#include <eiklbo.h>                 // MEikListBoxObserver

//<cmail>
#include <csxhelp/cmail.hlp.hrh>

#include "cpbkxrclresultinfodlg.h"
#include "cpbkxrclcbrsender.h"
#include "cpbkxrclvcardsender.h"
#include "cpbkxrclactionservicewrapper.h"
#include "mpbkxrclcontactretrieval.h"
#include "cpbkxrclcontactupdater.h"
#include "pbkxrclutils.h"
#include "pbkxrclengineconstants.h"
#include "engine.hrh"

#include "fsccontactactionservicedefines.h"
//</cmail>

// <cmail> S60 UID update
const TInt KPbkxRemoteContactLookupServiceImplImpUid = 0x2001FE0D;
// </cmail> S60 UID update

// list box field format
_LIT( KFieldFormat, "\t%S\t%S" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::CPbkxRclResultInfoDlg
// ---------------------------------------------------------------------------
//
CPbkxRclResultInfoDlg::CPbkxRclResultInfoDlg( 
    TInt& aIndex, 
    CDesCArray* aArray, 
    MEikCommandObserver* aCommand,
    MPbkxRclContactRetrieval* aContactRetrieval,
    CPbkContactEngine& aContactEngine,
    CPbkxRclActionServiceWrapper& aActionService,
    TBool aContactSelectorEnabled,
    TInt aActiveItemIndex ) :
    CPbkxRclBaseDlg( aIndex, aArray, aCommand, EFalse ), iItems( aArray ), 
    iActiveTabIndex( aActiveItemIndex ), 
    iContactSelectorEnabled( aContactSelectorEnabled ),
    iContactRetrieval( aContactRetrieval ), iContactEngine( aContactEngine ),
    iContactActionService( aActionService )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::ConstructL()
    {
    FUNC_LOG;
    
    CPbkxRclBaseDlg::ConstructL( R_RCL_RESULT_INFO_DIALOG_MENU_BAR );
   
    HBufC* text = StringLoader::LoadLC( R_QTN_RCL_OPENING_WAIT_NOTE );
    iWaitNoteText.CreateL( *text );
    CleanupStack::PopAndDestroy( text );

    iCallbackReqSender = CPbkxRclCbRSender::NewL();

    iVisibleFields = new ( ELeave ) CPbkFieldArray;

    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclResultInfoDlg* CPbkxRclResultInfoDlg::NewL( 
    TInt& aIndex, 
    CDesCArray* aArray,
    MEikCommandObserver* aCommand,
    MPbkxRclContactRetrieval* aContactRetrieval,
    CPbkContactEngine& aContactEngine,
    CPbkxRclActionServiceWrapper& aActionService,
    TBool aContactSelectorEnabled,
    TInt aActiveItemIndex ) 
    {
    FUNC_LOG;

    CPbkxRclResultInfoDlg* self = new( ELeave ) CPbkxRclResultInfoDlg( 
        aIndex, 
        aArray, 
        aCommand,
        aContactRetrieval,
        aContactEngine,
        aActionService,
        aContactSelectorEnabled,
        aActiveItemIndex );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::~CPbkxRclResultInfoDlg
// ---------------------------------------------------------------------------
//
CPbkxRclResultInfoDlg::~CPbkxRclResultInfoDlg()
    {
    FUNC_LOG;
    ResetStatusPane();
    delete iCurrentContact;
    iWaitNoteText.Close();
    delete iTitlePaneText;
    delete iCallbackReqSender;
    delete iVisibleFields;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::DynInitMenuPaneL( 
    TInt aResourceId, 
    CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    
    if ( aResourceId == R_RCL_RESULT_INFO_DIALOG_MENU_PANE )
        {

        if ( !iContactSelectorEnabled )
            {
            aMenuPane->SetItemDimmed( ERclCmdAddAsRecipient, ETrue );
            
            aMenuPane->SetItemDimmed(
                ERclCmdCall,
                !CallActionsAvailable() );

            // send menu is always available, since business card can 
            // always be sent
            }
        else
            {
            aMenuPane->SetItemDimmed( ERclCmdCall, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdSend, ETrue );
            }

        // talk menu is shown if we have PTT enabled
        aMenuPane->SetItemDimmed(
            ERclCmdTalk,
            !iContactActionService.IsActionEnabled( KFscAtComCallPoc ) );
        
        }
    else if ( aResourceId == R_RCL_ADD_TO_CONTACTS_MENU_PANE )
        {
        
        aMenuPane->SetItemDimmed( 
            ERclCmdCreateNew, 
            !iContactActionService.IsActionEnabled( KFscAtManSaveAs ) );
               
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
    else if ( aResourceId == R_RCL_TALK_MENU_PANE )
        {
        // PTT settings available, no need to hide items
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
            !iContactActionService.IsActionEnabled( KFscAtComSendCalReq ) );
        
        aMenuPane->SetItemDimmed( 
            ERclCmdSendAudioMsg, 
            !iContactActionService.IsActionEnabled( KFscAtComSendAudio ) );
        
        }
    else if ( aResourceId == R_RCL_CONTEXT_SEND_MENU_PANE )
        {       
        aMenuPane->SetItemDimmed( 
            ERclCmdSendMsg,
            (  !( FocusOnPhoneNumberField() || FocusOnEmailAddressField() ) ||
              !iContactActionService.IsActionEnabled( KFscAtComSendMsg ) ) );
        
        aMenuPane->SetItemDimmed(
            ERclCmdSendEmail,
            ( !FocusOnEmailAddressField() ||
              !iContactActionService.IsActionEnabled( KFscAtComSendEmail ) ) );
       
        aMenuPane->SetItemDimmed(
            ERclCmdSendMeetingReq,
            ( !FocusOnEmailAddressField() ||
              !iContactActionService.IsActionEnabled( KFscAtComSendCalReq ) ) );
        
        aMenuPane->SetItemDimmed(
            ERclCmdSendAudioMsg,
            ( !( FocusOnPhoneNumberField() || FocusOnEmailAddressField() ) ||
              !iContactActionService.IsActionEnabled(KFscAtComSendAudio ) ) );
        
        }
    else if ( aResourceId == R_RCL_RESULT_INFO_DIALOG_CONTEXT_MENU_PANE )
        {
        if ( !iContactSelectorEnabled )
            {
            aMenuPane->SetItemDimmed( ERclCmdAddAsRecipient, ETrue );
            
            aMenuPane->SetItemDimmed(
                ERclCmdCall,
                !( FocusOnPhoneNumberField() || FocusOnVoipField() ) );

            // send menu is always visible, because business card can always
            // be sent
            }
        else
            {
            aMenuPane->SetItemDimmed( ERclCmdCall, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdSend, ETrue );
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::GetHelpContext
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    FUNC_LOG;
    aContext.iMajor = TUid::Uid( KPbkxRemoteContactLookupServiceImplImpUid );
    aContext.iContext = KRLOOK_HLP_INFO_VIEW;
    }

// ----------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::HandleListBoxEventL()
// ----------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::HandleListBoxEventL(
    CEikListBox* /* aListBox */,
    TListBoxEvent aEventType )
    {
    FUNC_LOG;
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemClicked:
        case EEventItemActioned:
            {
            break;
            }
            
        case EEventItemDoubleClicked:
        	{
            // open context sensitive menu  
            iMenuBar->TryDisplayContextMenuBarL();
        	break;
        	}
        	
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CPbkxRclResultInfoDlg::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;

    if ( iExit || aType != EEventKey )
        {
        return EKeyWasConsumed;
        }
      
    switch ( aKeyEvent.iCode )
        {
        case EKeyOK:
            {
            // open context sensitive menu  
            iMenuBar->TryDisplayContextMenuBarL();
            }
            break;
        case EKeyUpArrow: // fall through, same functionality
        case EKeyDownArrow:
            {
            CAknSelectionListDialog::OfferKeyEventL( aKeyEvent, aType );
            }
            break;
        case EKeyRightArrow:
            {
            TInt oldIndex = iActiveTabIndex;
            iActiveTabIndex = ( iActiveTabIndex + 1 ) % iTabGroup->TabCount();
            
            if ( FetchCurrentContactL() )
                {
                UpdateDialogL();
                }
            else
                {
                iActiveTabIndex = oldIndex;
                }
            }
            break;
        case EKeyLeftArrow:
            {
            TInt oldIndex = iActiveTabIndex;
            if ( iActiveTabIndex > 0 )
                {
                iActiveTabIndex--;
                }
            else
                {
                iActiveTabIndex = iTabGroup->TabCount() - 1;
                }
            
            if ( FetchCurrentContactL() )
                {
                UpdateDialogL();
                }
            else
                {
                iActiveTabIndex = oldIndex;
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
// CPbkxRclResultInfoDlg::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::OkToExitL( TInt aButtonId )
	{
    FUNC_LOG;
	if( aButtonId == EAknSoftkeyContextOptions )
		{
        iMenuBar->TryDisplayContextMenuBarL();
		return EFalse;
		}
	else
		{
		return CAknDialog::OkToExitL( aButtonId );
		}
	}

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::ExecuteLD
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::ExecuteLD()
    {
    FUNC_LOG;

    // if current contact cannot be fetched, result info dialog is not opened
    if ( !FetchCurrentContactL() )
        {
        delete this;
        return;
        }

    PrepareLC( R_RCL_RESULT_INFO_DIALOG );
    
    UpdateGraphics();
    UpdateColors();

    iMenuBar->SetContextMenuTitleResourceId( 
        R_RCL_RESULT_INFO_DIALOG_CONTEXT_MENU_BAR );    

    SetupStatusPaneL();

    CreateListBoxEntriesL();
    CAknSelectionListDialog::RunLD();
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::Close
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::Close()
    {
    FUNC_LOG;
    // this will not leave, because TryExitL doesn't call leaving methods
    // when EAknSoftkeyCancel is used
    iExit = ETrue;
    TRAP_IGNORE( TryExitL( EAknSoftkeyCancel ) ); // <cmail>
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SendCallbackRequestL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SendCallbackRequestL()
    {
    FUNC_LOG;
    iCallbackReqSender->SendCallbackRequestL( *iCurrentContact );
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SendBusinessCardL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SendBusinessCardL()
    {
    FUNC_LOG;
    CPbkxRclvCardSender* sender = CPbkxRclvCardSender::NewLC( iContactEngine );
    sender->SendvCardL( *iCurrentContact, *(iContactActionService.ContactConverter()) );
    CleanupStack::PopAndDestroy( sender );
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::UpdateExistingContactL
// ---------------------------------------------------------------------------
//
HBufC* CPbkxRclResultInfoDlg::UpdateExistingContactL()
    {
    FUNC_LOG;
    CPbkxRclContactUpdater* updater = CPbkxRclContactUpdater::NewLC(
        iContactEngine );
    TPbkContactItemField& field = CurrentField();
      
    HBufC* resultText = updater->UpdateContactL( field );
    CleanupStack::PopAndDestroy( updater );
    return resultText;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SetupStatusPaneL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SetupStatusPaneL()
    {
    FUNC_LOG;
    CAknAppUi* appUi = static_cast<CAknAppUi*>( CCoeEnv::Static()->AppUi() );
    CEikStatusPane* statusPane = appUi->StatusPane();
    iNaviPane = reinterpret_cast<CAknNavigationControlContainer*>(
        statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    iTitlePane = reinterpret_cast<CAknTitlePane*>(
        statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    
    const TDesC* titleText = iTitlePane->Text();
    iTitlePaneText = titleText->AllocL();
    
    SetTitlePaneTextL();
    
    SetupTabGroupL();
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::ResetStatusPane
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::ResetStatusPane()
    {
    FUNC_LOG;
    // title pane may be null if dialog constructing failed
    if ( iTitlePane != NULL )
        {
        // ownership of iTitlePaneText is transferred to title pane
        iTitlePane->SetText( iTitlePaneText );
        iTitlePaneText = NULL;
        }
    ResetTabGroup();
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SetupTabGroupL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SetupTabGroupL()
    {
    FUNC_LOG;
    TInt contactCount = iContactRetrieval->ContactCount();
    iNaviDecorator = iNaviPane->CreateTabGroupL();
    iTabGroup = reinterpret_cast<CAknTabGroup*>( 
        iNaviDecorator->DecoratedControl() );
    
    
    CArrayFixFlat<TInt>* array = 
        new ( ELeave ) CArrayFixFlat<TInt>( KArrayGranularity );
    CleanupStack::PushL( array );
    
    // add initial values
    array->AppendL( 0 );
    array->AppendL( contactCount );
    
    iTabGroup->SetTabFixedWidthL( KTabWidthWithOneTab );
    for ( TInt i = 0; i < contactCount; i++ )
        {
        ( *array )[0] = i + 1;
        HBufC* text = StringLoader::LoadLC( R_QTN_RCL_INFO_NAVI, *array );
        iTabGroup->AddTabL( i, *text );
        CleanupStack::PopAndDestroy( text );
        }
    
    CleanupStack::PopAndDestroy( array );
    iTabGroup->SetActiveTabById( iActiveTabIndex );

    SetNaviIndicatorsL();

    iNaviPane->PushL( *iNaviDecorator );
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SetNaviIndicatorsL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SetNaviIndicatorsL()
    {
    FUNC_LOG;
    if ( iNaviPane != NULL && iNaviDecorator != NULL && iTabGroup != NULL )
        {
        TInt contactCount = iTabGroup->TabCount();
        if ( contactCount > 1 )
            {
            iNaviDecorator->MakeScrollButtonVisible( ETrue );
            
            iNaviDecorator->SetScrollButtonDimmed(
                CAknNavigationDecorator::ERightButton, 
                EFalse);
            
            iNaviDecorator->SetScrollButtonDimmed(
                CAknNavigationDecorator::ELeftButton, 
                EFalse);
            
            iNaviPane->PushL( *iNaviDecorator );
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SetTitlePaneTextL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SetTitlePaneTextL()
    {
    FUNC_LOG;
   
    HBufC* firstName = PbkxRclUtils::FieldTextL( 
        iCurrentContact, 
        EPbkFieldIdFirstName );
    CleanupStack::PushL( firstName );
    
    HBufC* lastName = PbkxRclUtils::FieldTextL( 
        iCurrentContact, 
        EPbkFieldIdLastName );
    CleanupStack::PushL( lastName );
    
    RBuf name;
    CleanupClosePushL( name );
    name.CreateL( firstName->Length() + lastName->Length() + 1 );
    name.Format( KNameFormat, firstName, lastName );
       
    iTitlePane->SetTextL( name );
    
    CleanupStack::PopAndDestroy( &name );
    CleanupStack::PopAndDestroy( lastName );
    CleanupStack::PopAndDestroy( firstName );
   
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::ResetTabGroup
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::ResetTabGroup()
    {
    FUNC_LOG;
    // navi pane and navi decorator may be null if dialog construction failed
    if ( iNaviPane != NULL && iNaviDecorator != NULL )
        {
        iNaviPane->Pop( iNaviDecorator );
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::CreateListBoxEntriesL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::CreateListBoxEntriesL()
    {
    FUNC_LOG;

    iItems->Reset();
    iVisibleFields->Reset();

    CPbkFieldArray& fieldArray = iCurrentContact->CardFields();
    TInt count = fieldArray.Count();

    for ( TInt i = 0; i < count; i++ )
        {
    
        TPbkContactItemField field = fieldArray[i];

        HBufC* text = PbkxRclUtils::FieldTextL( field );
        CleanupStack::PushL( text );

        if ( text->Length() > 0 )
            {
            iVisibleFields->AppendL( field );
            const TDesC& header = field.FieldInfo().FieldName();
            
            RBuf entry;
            CleanupClosePushL( entry );
            entry.CreateL( 
                header.Length() + text->Length() + 
                KExtraSpaceInListboxEntry );
            
            entry.Format( KFieldFormat, &header, text );
            iItems->AppendL( entry );
            CleanupStack::PopAndDestroy( &entry );
            }
        CleanupStack::PopAndDestroy( text );
        }
    
    CEikListBox* listBox = ListBox();

    if ( count > 0 )
        {
        // if there are items, set top item as selected
        listBox->SetTopItemIndex( 0 );
        listBox->SetCurrentItemIndex( 0 );
        }

    listBox->HandleItemAdditionL();
    
    // update contact action service
    iContactActionService.SetCurrentContactL( 
        iCurrentContact  );

    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::FetchCurrentContactL
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::FetchCurrentContactL()
    {
    FUNC_LOG;
    CContactCard* currentCard = iContactRetrieval->RetrieveDetailsL( 
        iActiveTabIndex,
        iWaitNoteText );
    
    if ( currentCard != NULL )
        {
        
        delete iCurrentContact;
        iCurrentContact = NULL;
        
        iCurrentContact = PbkxRclUtils::CreateContactItemL(
            currentCard,
            iContactEngine );

        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::UpdateDialogL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::UpdateDialogL()
    {
    FUNC_LOG;
    iContactRetrieval->SetSelectedContactL( iActiveTabIndex );
    CreateListBoxEntriesL();
    SetTitlePaneTextL();
    iTabGroup->SetActiveTabById( iActiveTabIndex );
    
    // set navi indicators if needed
    if ( iActiveTabIndex == 0 || iActiveTabIndex == iTabGroup->TabCount() - 1 )
        {
        SetNaviIndicatorsL();
        }
    
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::CallActionsAvailable
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::CallActionsAvailable() const
    {
    FUNC_LOG;
    return iContactActionService.IsActionEnabled( KFscAtComCallGSM ) ||
        iContactActionService.IsActionEnabled( KFscAtComCallVideo ) ||
        iContactActionService.IsActionEnabled( KFscAtComCallVoip );
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::FocusOnPhoneNumberField
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::FocusOnPhoneNumberField() const
    {
    FUNC_LOG;
    TPbkContactItemField& field = CurrentField();
    return field.FieldInfo().IsPhoneNumberField();
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::FocusOnEmailAddressField
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::FocusOnEmailAddressField() const
    {
    FUNC_LOG;
    TPbkContactItemField& field = CurrentField();
    return field.FieldInfo().IsEmailField();
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::FocusOnVoipField
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::FocusOnVoipField() const
    {
    FUNC_LOG;
    TPbkContactItemField& field = CurrentField();
    return field.FieldInfo().FieldId() == EPbkFieldIdVOIP;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::CurrentField
// ---------------------------------------------------------------------------
//
TPbkContactItemField& CPbkxRclResultInfoDlg::CurrentField() const
    {
    FUNC_LOG;
    TInt curIndex = ListBox()->CurrentItemIndex();
    return ( *iVisibleFields )[curIndex];
    }

