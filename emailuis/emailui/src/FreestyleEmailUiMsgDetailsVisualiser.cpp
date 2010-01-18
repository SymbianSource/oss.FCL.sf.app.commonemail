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
*  Description : FreestyleEmailUi message details view implementation
*  Version     : %version: e002sa38#46 %
*
*/



// SYSTEM INCLUDES
#include "emailtrace.h"
#include <AknUtils.h>
#include <gulicon.h>
#include <freestyleemailui.mbg>
#include <FreestyleEmailUi.rsg>
#include <StringLoader.h>
#include <AknBidiTextUtils.h>
#include <akntitle.h>
// <cmail> SF
#include <alf/alfdecklayout.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfframebrush.h>
#include <alf/alfevent.h>
// </cmail>
#include <featmgr.h>
//</cmail>

//<cmail>
#include "CFSMailMessage.h"
#include "CFSMailClient.h"
//</cmail>

#include "FSEmailBuildFlags.h"
//<cmail>
#include "cfsccontactactionmenu.h"
#include "mfsccontactactionmenumodel.h"
//</cmail>

//<cmail>
#include "fstreelist.h"
#include "fstreevisualizerbase.h"
#include "fstreeplainonelinenodedata.h"
#include "fstreeplainonelinenodevisualizer.h"
#include "fstreeplainonelineitemdata.h"
#include "fstreeplainonelineitemvisualizer.h"
#include "fstreeplaintwolineitemdata.h"
#include "fstreeplaintwolineitemvisualizer.h"
#include <csxhelp/cmail.hlp.hrh>
// </cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiMsgDetailsControl.h"
#include "FreestyleEmailUiMsgDetailsModel.h"
#include "FreestyleEmailUiMsgDetailsVisualiser.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FreestyleEmailUiContactHandler.h"
#include "FSDelayedLoader.h"


CFSEmailUiMsgDetailsVisualiser* CFSEmailUiMsgDetailsVisualiser::NewL( CAlfEnv& aEnv,
                                                 CAlfControlGroup& aControlGroup,
                                                 CFreestyleEmailUiAppUi& aAppUi )
    {
    FUNC_LOG;
    CFSEmailUiMsgDetailsVisualiser* self = CFSEmailUiMsgDetailsVisualiser::NewLC(aEnv, aControlGroup, aAppUi );
    CleanupStack::Pop(self);
    return self;
    }

CFSEmailUiMsgDetailsVisualiser* CFSEmailUiMsgDetailsVisualiser::NewLC( CAlfEnv& aEnv,
                                                 CAlfControlGroup& aControlGroup,
                                                 CFreestyleEmailUiAppUi& aAppUi )
{
    FUNC_LOG;
    CFSEmailUiMsgDetailsVisualiser* self = new (ELeave) CFSEmailUiMsgDetailsVisualiser( aEnv, aAppUi, aControlGroup );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

void CFSEmailUiMsgDetailsVisualiser::ConstructL()
	{
    FUNC_LOG;

    BaseConstructL( R_FSEMAILUI_MAIL_DETAILS_VIEW );

    iFirstStartCompleted = EFalse;    
 
	}

// CFSEmailUiMsgDetailsVisualiser::DoFirstStartL()
// Purpose of this function is to do first start only when msg details is
// really needed to be shown. Implemented to make app startuo faster.
void CFSEmailUiMsgDetailsVisualiser::DoFirstStartL()
    {
    FUNC_LOG;
    iControl = CFSEmailUiMsgDetailsControl::NewL( iEnv, *this );
    iModel = new (ELeave) CFSEmailUiMsgDetailsModel();

    UpdateListSizeAttributes();
     
    iParentLayout = CAlfDeckLayout::AddNewL( *iControl );
    iParentLayout->SetFlags( EAlfVisualFlagLayoutUpdateNotification );
    iParentLayout->SetRect( iScreenRect );
    
    iTreeVisualizer = CFsTreeVisualizerBase::NewL(iControl, *iParentLayout);
    iTreeVisualizer->SetItemExpansionDelay( iAppUi.LayoutHandler()->ListItemExpansionDelay() );
    iTreeVisualizer->SetScrollTime( iAppUi.LayoutHandler()->ListScrollingTime() );
    iTreeVisualizer->SetFadeInEffectTime( iAppUi.LayoutHandler()->ListFadeInEffectTime() );
    iTreeVisualizer->SetFadeOutEffectTime( iAppUi.LayoutHandler()->ListFadeOutEffectTime() );
    
    iTreeList = CFsTreeList::NewL( *iTreeVisualizer, iEnv );
    iTreeList->HideListL();
    iTreeList->SetLoopingType( EFsTreeListLoopingJumpToFirstLast );
    iTreeList->SetScrollbarVisibilityL( EFsScrollbarAuto );
    iTreeList->SetIndentationL( 0 );

    //<cmail> Compared to S60 3.2.3 in S60 5.0 Alf offers the key events in
    // opposite order.
    ControlGroup().AppendL( iControl );
    ControlGroup().AppendL( iTreeList->TreeControl() ); 
    //</cmail>
    
    // <cmail> Touch
    iTreeList->AddObserverL(*this);
    //</cmail>
    
    // Set page up and page down keys
    iTreeVisualizer->AddCustomPageUpKey( EStdKeyPageUp );
    iTreeVisualizer->AddCustomPageDownKey( EStdKeyPageDown );
    iTreeVisualizer->SetItemsAlwaysExtendedL( ETrue );

    iAppUi.LayoutHandler()->SetListMarqueeBehaviour( iTreeList );
    
    iFirstStartCompleted = ETrue;
    }

CFSEmailUiMsgDetailsVisualiser::CFSEmailUiMsgDetailsVisualiser( CAlfEnv& aEnv, CFreestyleEmailUiAppUi& aAppUi, CAlfControlGroup& aControlGroup  )
  : CFsEmailUiViewBase(aControlGroup, aAppUi),
    iEnv(aEnv),
    iFirstViewActivation( ETrue ),
    iExpandCollapseMode( EExpandAll ),
    // <cmail> video call
    iVideoCall( EFalse )
    // </cmail>
	{
    FUNC_LOG;
	}
	
CFSEmailUiMsgDetailsVisualiser::~CFSEmailUiMsgDetailsVisualiser()
	{
    FUNC_LOG;
	delete iModel;
	iModel = NULL;

	delete iPreviousTitleText;
	iPreviousTitleText = NULL;

	delete iViewedMsg;
	iViewedMsg = NULL;
	
	delete iNoDisplayNameAvailableText;
	iNoDisplayNameAvailableText = NULL;
	
	delete iNoEmailAddressAvailableText;
	iNoEmailAddressAvailableText = NULL;
	
	delete iTreeList;
	iTreeList = NULL;
	
	iNodeIds.Close();
	}

TUid CFSEmailUiMsgDetailsVisualiser::Id() const
	{
    FUNC_LOG;
	return MsgDetailsViewId;
	}			   

// <cmail> Toolbar
/*void CFSEmailUiMsgDetailsVisualiser::DoActivateL(const TVwsViewId& aPrevViewId,
                     TUid aCustomMessageId,
                     const TDesC8& aCustomMessage)*/
void CFSEmailUiMsgDetailsVisualiser::ChildDoActivateL(
    const TVwsViewId& aPrevViewId, TUid aCustomMessageId, 
        const TDesC8& aCustomMessage)
// </cmail> Toolbar
	{
    FUNC_LOG;
	if ( !iFirstStartCompleted )
	    {
	    DoFirstStartL();
	    }
	
	UpdateListSizeAttributes();
    iParentLayout->SetRect( iScreenRect );

	if ( aCustomMessageId == KStartMsgDetailsReturnToPrevious )
	    {
	    // Fisrt handle special case where we are returning to msg details view,
	    // so we don't need to recreate list contents, just refresh the view
	    if ( iViewedMsg ) // Safety check
	        {
	        // Set title bar text to "Message/Meeting details"
	        ChangeTitleBarTextL( ETrue );

	        // Hide list and switch this view as active
	        iTreeList->HideListL();

	        // Then just refresh the list as in dynamic variant switch,
	        // because screen orientation might have changed
            HandleDynamicVariantSwitchL( EScreenLayoutChanged );
	        }
	    }
	else
		{
		// This is the "normal" case, so we need to recreate the list contents
		
		// Store previous view ID
		iPreviousViewUid = aPrevViewId.iViewUid;

		TMsgDetailsActivationData subView;
		TPckgBuf<TMsgDetailsActivationData> viewData( subView );
		viewData.Copy( aCustomMessage );
		subView = viewData();

		delete iViewedMsg;
		iViewedMsg = NULL;

		iViewedMsg = iAppUi.GetMailClient()->GetMessageByUidL( subView.iMailBoxId, subView.iFolderId, subView.iMessageId, EFSMsgDataEnvelope  );
		CFSMailBox* mailBox = iAppUi.GetMailClient()->GetMailBoxByUidL( subView.iMailBoxId );
	    CleanupStack::PushL( mailBox );    
	    if ( mailBox && TFsEmailUiUtility::IsRemoteLookupSupported( *mailBox ) )
	        {
	        iRCLSupported = ETrue;       
	        } 
	    else
	        {
	        iRCLSupported = EFalse;
	        }
	    CleanupStack::PopAndDestroy( mailBox );		

		if( iFirstViewActivation )
			{
			iTreeVisualizer->SetMenuIcon( iAppUi.FsTextureManager()->TextureByIndex( EListControlMenuIcon ) );		
		    //<cmail> S60 skin support
		    //iTreeVisualizer->SetBackgroundTextureL( iAppUi.FsTextureManager()->TextureByIndex( EBackgroundTextureMailList ) );
		    //</cmail>

		  	CAlfBrush* selectorBrush = iAppUi.FsTextureManager()->ListSelectorBrushL();
			iTreeVisualizer->SetSelectorPropertiesL( selectorBrush, 1.0, CFsTreeVisualizerBase::EFsSelectorMoveSmoothly );
	  
		    iFirstViewActivation = EFalse;
			}

	    if ( iViewedMsg ) // Safety check
	        {
	        // Set title bar text to "Message/Meeting details"
	        ChangeTitleBarTextL( ETrue );

	        iTreeList->HideListL();

            // By default we expand all nodes, if parameters doesn't state
            // something else
            iExpandCollapseMode = EExpandAll;
            if ( aCustomMessageId == KStartMsgDetailsToTo )
                {
                iExpandCollapseMode = ECollapseAllExceptTo;
                }
            else if ( aCustomMessageId == KStartMsgDetailsToCc )
                {
                iExpandCollapseMode = ECollapseAllExceptCc;
                }
            else if ( aCustomMessageId == KStartMsgDetailsToBcc )
                {
                iExpandCollapseMode = ECollapseAllExceptBcc;
                }
            
            iParentLayout->SetRect( iScreenRect );
            ClearMsgDetailsModelL();        
            
            UpdateMsgDetailsModelL();
            iTreeList->ShowListL();
	        }
		}
	// <cmail> Touch
	iTreeList->SetFocusedL(ETrue);
	// </cmail>
	}

void CFSEmailUiMsgDetailsVisualiser::ChildDoDeactivate()
	{
    FUNC_LOG;
    if ( !iAppUi.AppUiExitOngoing() )
        {
        if ( iTreeList->IsFocused() )
            {
            TRAP_IGNORE( { 
                iTreeList->SetFocusedL(EFalse);
                iTreeList->SetFocusedItemL(KFsTreeNoneID);
                } );
            }
        iTreeVisualizer->NotifyControlVisibilityChange( EFalse );
        }
	}

void CFSEmailUiMsgDetailsVisualiser::PrepareForExit()
    {
    delete iViewedMsg;
    iViewedMsg = NULL;
    }


void CFSEmailUiMsgDetailsVisualiser::ChangeTitleBarTextL( TBool aViewStarted )
	{
    FUNC_LOG;
	if ( iFirstStartCompleted ) // Safety
	    {
		if ( aViewStarted )
			{
			// Store previous application title text
			delete iPreviousTitleText;
			iPreviousTitleText = NULL;
			iPreviousTitleText = iAppUi.TitlePaneTextL().AllocL();
			
			HBufC* titleText = NULL;
			if( iViewedMsg->IsFlagSet( EFSMsgFlag_CalendarMsg ) )
				{
				titleText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_MEETING_HEADING );
				}
			else
				{
				titleText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_HEADING );
				}

			iAppUi.SetTitlePaneTextL( *titleText );

			CleanupStack::PopAndDestroy( titleText ); 
			}
		else
			{
			if ( iPreviousTitleText )
				{
				iAppUi.SetTitlePaneTextL( *iPreviousTitleText ); // Set application title text back			
				}
			}
		}
	}

void CFSEmailUiMsgDetailsVisualiser::LaunchActionMenuL()
	{
    FUNC_LOG;
    CFSEmailUiActionMenu::RemoveAllL();

    RFsEActionMenuIdList itemList;
    itemList.AppendL(FsEActionMenuCall);
    // <cmail> video call
    itemList.AppendL( FsEActionMenuCallVideo );
    // </cmail>
    itemList.AppendL(FsEActionMenuCreateMessage);
    itemList.AppendL(FsEActionMenuContactDetails);
    itemList.AppendL(FsEActionMenuAddToContacts);
    if ( iRCLSupported )
        {
        itemList.AppendL(FsEActionMenuRemoteLookup);
        }     
    CFSEmailUiActionMenu::AddCustomItemsL( itemList );
    itemList.Close();
    
    // <cmail> Touch	
    TActionMenuCustomItemId itemId = CFSEmailUiActionMenu::ExecuteL( EFscCustom, 0, this );
    // </cmail>
	
   	HandleActionMenuCommandL( itemId );
	}

void CFSEmailUiMsgDetailsVisualiser::HandleActionMenuCommandL( TActionMenuCustomItemId itemId )
	{
    FUNC_LOG;
	switch( itemId )
		{
		case FsEActionMenuCall:
			{
			CallToFocusedItemL();
			}
			break;
        // <cmail> video call
        case FsEActionMenuCallVideo: // Video Call
            {
            iVideoCall = ETrue;
            CallToFocusedItemL();
            }
            break;
        // </cmail>
		case FsEActionMenuCreateMessage:
			{
			CreateMessageToFocusedItemL();
			}
			break;

		case FsEActionMenuContactDetails:
			{
			ShowContactDetailsForFocusedItemL();
			}
			break;

		case FsEActionMenuAddToContacts:
			{
			AddFocusedItemToContactsL();
			}
			break;

		case FsEActionMenuRemoteLookup:
			{
			LaunchRemoteLookupForFocusedItemL();
			}
			break;
		}
	}


// -----------------------------------------------------------------------------
// CFSEmailUiMsgDetailsVisualiser::GetEmailAddressForFocusedItem
// Returns email address of the focused list item. NULL returned if not found.
// -----------------------------------------------------------------------------
CFSMailAddress* CFSEmailUiMsgDetailsVisualiser::GetEmailAddressForFocusedItem() const
	{
    FUNC_LOG;
	CFSMailAddress* foundEmailAddress = NULL;
	CFSEmailUiMsgDetailsItem* item = iModel->ItemByListId( iTreeList->FocusedItem() );
	if( item )
		{
		foundEmailAddress = item->iMailAddress;
		}
	return foundEmailAddress;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMsgDetailsVisualiser::GetEmailAddressForFocusedItem
// Returns email address of the focused list item. NULL returned if not found.
// -----------------------------------------------------------------------------
TDesC* CFSEmailUiMsgDetailsVisualiser::GetEmailAddressForFocusedItemAsTDes() const
	{
    FUNC_LOG;
	TDesC* foundEmailAddress = NULL;
	CFSMailAddress* address = GetEmailAddressForFocusedItem();
	if( address )
		{
		foundEmailAddress = &address->GetEmailAddress();
		}
	return foundEmailAddress;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMsgDetailsVisualiser::SendEmailToFocusedItemL
// 
// Open composer and set the focused item as recipient, if valid email address
// is available. Return ETrue if valid email address was found, EFalse if not.
// -----------------------------------------------------------------------------
TBool CFSEmailUiMsgDetailsVisualiser::SendEmailToFocusedItemL() const
	{
    FUNC_LOG;
	TBool addrFound = EFalse;
	CFSMailAddress* address = GetEmailAddressForFocusedItem();
	if( address )
		{
		iAppUi.LaunchEditorL( address );
		addrFound = ETrue;
		}
	return addrFound;	
	}
	
// -----------------------------------------------------------------------------
// CFSEmailUiMsgDetailsVisualiser::CallToFocusedItemL
// -----------------------------------------------------------------------------
void CFSEmailUiMsgDetailsVisualiser::CallToFocusedItemL()
	{
    FUNC_LOG;
	TDesC* email = GetEmailAddressForFocusedItemAsTDes();
	if( email )
		{
		CFSMailClient* mailClient =	iAppUi.GetMailClient();
		CFSMailBox* mailBox = mailClient->GetMailBoxByUidL( iViewedMsg->GetMailBoxId() );
	   	CleanupStack::PushL( mailBox );

        // <cmail> video call
        if ( iVideoCall )
            {
            iVideoCall = EFalse;
            CFsDelayedLoader::InstanceL()->GetContactHandlerL()->SetVideoCall( ETrue );
            }
        // </cmail>
	   	
	   	CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCallToContactByEmailL( *email, 
	   			 iAppUi.GetActiveMailbox(), this, EFalse );

	   	CleanupStack::PopAndDestroy( mailBox );
		}
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMsgDetailsVisualiser::CreateMessageToFocusedItemL
// -----------------------------------------------------------------------------
void CFSEmailUiMsgDetailsVisualiser::CreateMessageToFocusedItemL() const
	{
    FUNC_LOG;
	TDesC* email = GetEmailAddressForFocusedItemAsTDes();
	if( email )
		{
		CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCreateMsgToContactByEmailL( *email, iAppUi.GetActiveMailbox() );
		}
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMsgDetailsVisualiser::LaunchRemoteLookupForFocusedItemL
// -----------------------------------------------------------------------------
void CFSEmailUiMsgDetailsVisualiser::LaunchRemoteLookupForFocusedItemL() const
	{
    FUNC_LOG;
	CFSEmailUiMsgDetailsItem* item = iModel->ItemByListId( iTreeList->FocusedItem() );
	if( !item )
		{
		return;
		}
	CFSMailAddress* address = item->iMailAddress;
	if( !address )
		{
		return;
		}
	TDesC* queryString = &address->GetEmailAddress();
	if( !queryString )
		{
		queryString = &address->GetDisplayName();
		if( !queryString )
			{
			return;
			}
		}

	// this method assumes that remote lookup is available with current plugin.
	CFSMailClient* mailClient =	iAppUi.GetMailClient();
	CFSMailBox* mailBox = mailClient->GetMailBoxByUidL( iViewedMsg->GetMailBoxId() );
   	CleanupStack::PushL( mailBox );
   	
	CFsDelayedLoader::InstanceL()->GetContactHandlerL()->LaunchRemoteLookupWithQueryL( *mailBox, *queryString );

   	CleanupStack::PopAndDestroy( mailBox );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMsgDetailsVisualiser::AddFocusedItemToContactsL
// -----------------------------------------------------------------------------
void CFSEmailUiMsgDetailsVisualiser::AddFocusedItemToContactsL() const
	{
    FUNC_LOG;
	TDesC* email = GetEmailAddressForFocusedItemAsTDes();
	if( email )
		{
		TAddToContactsType aType;
		//Query to "update existing" or "Create new" --> EFALSE = user choosed "cancel"
		if ( CFsDelayedLoader::InstanceL()->GetContactHandlerL()->AddtoContactsQueryL( aType ) )
			{
			CFsDelayedLoader::InstanceL()->GetContactHandlerL()->AddToContactL( 
					*email, EContactUpdateEmail, aType );		
			}						
		}										
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMsgDetailsVisualiser::ShowContactDetailsForFocusedItemL
// -----------------------------------------------------------------------------
void CFSEmailUiMsgDetailsVisualiser::ShowContactDetailsForFocusedItemL() const
	{
    FUNC_LOG;
	TDesC* email = GetEmailAddressForFocusedItemAsTDes();
	if( email )
		{
		CFsDelayedLoader::InstanceL()->GetContactHandlerL()->ShowContactDetailsL( 
					*email, EContactUpdateEmail, NULL );
		}
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMsgDetailsVisualiser::CopyFocusedItemToClipboardL
// -----------------------------------------------------------------------------
void CFSEmailUiMsgDetailsVisualiser::CopyFocusedItemToClipboardL() const
	{
    FUNC_LOG;
	CFSEmailUiMsgDetailsItem* item = iModel->ItemByListId( iTreeList->FocusedItem() );
	if (item)
	    {
    	CFSMailAddress* address = item->iMailAddress;
    	TDesC* text = item->iText;
    	if ( address )
    	    {
        	TDesC* clipBoardText = &address->GetEmailAddress();
        	if( !clipBoardText ||
        		(clipBoardText && clipBoardText->Length() == 0) )
        		{
        		clipBoardText = &address->GetDisplayName();
        		if( !clipBoardText )
        			{
        			return;
        			}
        		}
	       	TFsEmailUiUtility::CopyToClipboardL( *clipBoardText );
    	    }
    	else if( text )
    		{
	       	TFsEmailUiUtility::CopyToClipboardL( *text );
    		}
	    }		
	}

// -----------------------------------------------------------------------------
// Action menu is available for items that has iMailAddress set in model.
// -----------------------------------------------------------------------------
TBool CFSEmailUiMsgDetailsVisualiser::HasFocusedItemActionMenu() const
	{
    FUNC_LOG;
	CFSEmailUiMsgDetailsItem* item = iModel->ItemByListId( iTreeList->FocusedItem() );
	if( item && item->iMailAddress )
		{
		const TDesC* displayName = &item->iMailAddress->GetDisplayName();
		const TDesC* emailAddress = &item->iMailAddress->GetEmailAddress();

		if ( (displayName && displayName->Length()) ||
		     (emailAddress && emailAddress->Length()) )
		    {
		    return ETrue;
		    }
		}
	return EFalse;
	}

void CFSEmailUiMsgDetailsVisualiser::SetMskL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        TFsTreeItemId curId = iTreeList->FocusedItem(); 

        if ( iTreeList->IsNode( curId ) )
            {
            if ( iTreeList->IsExpanded( curId ) )
                {
                ChangeMskCommandL( R_FSE_QTN_MSK_COLLAPSE );
                } 
            else  
                {
                ChangeMskCommandL( R_FSE_QTN_MSK_EXPAND );
                }
            }
        else // non-node item
            {
            if( GetEmailAddressForFocusedItem() )
                {
                ChangeMskCommandL( R_FSE_QTN_MSK_COMPOSE );
                }
            else
                {
                ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
                }
            }        
        }
    }

TBool CFSEmailUiMsgDetailsVisualiser::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    TBool result = EFalse;
    SetMskL();
    if ( aEvent.IsKeyEvent() && aEvent.Code() == EEventKey )
        {
        TInt scanCode = aEvent.KeyEvent().iScanCode;
        // Swap right and left controls in mirrored layout
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            if (scanCode == EStdKeyRightArrow) scanCode = EStdKeyLeftArrow;
            else if (scanCode == EStdKeyLeftArrow ) scanCode = EStdKeyRightArrow;
            }
        
        switch ( scanCode )
            {
            case EStdKeyDevice3:	// CENTER CLICK
            case EStdKeyEnter:		// ENTER
            	{
            	// Send email to focused item. Set the event as consumed if
            	// valid email address was found and composer was opened with
            	// that email address as recipent. If email address was not
            	// found, offer the event to list by not consuming it.
            	result = SendEmailToFocusedItemL();
            	}
            	break;
				
            case EStdKeyYes:
            	{
				CallToFocusedItemL();
            	result = ETrue;
            	}
            	break;
            
	        case EStdKeyRightArrow:
            	{
            	// Show action toolbar if the item has action menu.
            	if( HasFocusedItemActionMenu() )
            		{
            		LaunchActionMenuL();
            		result = ETrue;
            		}
            	else
            		{
            		result = EFalse;
            		}
            	}
				break;
			
			default:
			    // Check keyboard shortcuts.
	       	    TInt shortcutCommand = 
	       	        iAppUi.ShortcutBinding().CommandForShortcutKey( aEvent.KeyEvent(),
	       	                                                         CFSEmailUiShortcutBinding::EContextMailDetails );
	       	    if ( shortcutCommand != KErrNotFound )
	       	        {
       	            HandleCommandL( shortcutCommand );
	       	        result = ETrue;
       	            }
				break;
            }
        }
    else if (aEvent.IsPointerEvent())
        {
        result = iTreeList->TreeControl()->OfferEventL(aEvent);
        }

	return result;
    }

// ---------------------------------------------------------------------------
// From MFSEmailUiContactHandlerObserver
// The ownership of the CLS items in the contacts array is transferred to the
// observer, and they must be deleted by the observer.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsVisualiser::OperationCompleteL(
    TContactHandlerCmd /*aCmd*/, const RPointerArray<CFSEmailUiClsItem>& /*aContacts*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// From MFSEmailUiContactHandlerObserver
// Handles error in contact handler operation.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsVisualiser::OperationErrorL( TContactHandlerCmd /*aCmd*/,
    TInt /*aError*/ )
    {
    FUNC_LOG;
    }

void CFSEmailUiMsgDetailsVisualiser::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
    FUNC_LOG;
	   
    if ( aResourceId == R_FSEMAILUI_MAILDETAILS_MENUPANE )
        {
        if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
    	   {
    	   // remove help support in pf5250
    	   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);      
    	   }
        
        // Hide "actions" option if currently focused item doesn't have action menu
       	aMenuPane->SetItemDimmed( EFsEmailUiCmdMailActions, !HasFocusedItemActionMenu() );
        // Hide "copy to clipboard" option if there is nothing to copy on the focused row
    	CFSEmailUiMsgDetailsItem* item = iModel->ItemByListId( iTreeList->FocusedItem() );
    	if ( !item )
    	    {
        	aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCopyToClipboard, ETrue );
            }
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCollapseAll, AllNodesCollapsed() );
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsExpandAll, AllNodesExpanded() );
        }
    
    if ( aResourceId == R_FSEMAILUI_MAILDETAILS_SUBMENU_ACTIONS )
        {
        TInt pos( 0 );
        if ( !iRCLSupported && aMenuPane->MenuItemExists( EFsEmailUiCmdActionsRemoteLookup ,pos) )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsRemoteLookup, ETrue );
            }
        }
    
    // Add shortcut hints
	iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane, 
	                           CFSEmailUiShortcutBinding::EContextMailDetails );
	}

void CFSEmailUiMsgDetailsVisualiser::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;
    switch(aCommand)
        {
        case EAknSoftkeyBack:
        	{
            if ( !iAppUi.ViewSwitchingOngoing() )
				{
				ChangeTitleBarTextL( EFalse );
	  	       	iAppUi.ReturnToPreviousViewL();
				}
     	 	}
        	break;
        	
        case EFsEmailUiCmdActionsCopyToClipboard:
		    {
		    CopyFocusedItemToClipboardL();
	        }
	        break;
        case EFsEmailUiCmdCollapse:
			{
            TFsTreeItemId focId1 = iTreeList->FocusedItem();
            iTreeList->CollapseNodeL(focId1);
            ChangeMskCommandL( R_FSE_QTN_MSK_EXPAND);
            }
            break;
            
        case EFsEmailUiCmdExpand:
			{
            TFsTreeItemId focId2 = iTreeList->FocusedItem();
            iTreeList->ExpandNodeL(focId2);
            ChangeMskCommandL( R_FSE_QTN_MSK_COLLAPSE);
			}
            break;
		// Options menu commands
        case EFsEmailUiCmdActionsCollapseAll:
		    {
        	TFsTreeItemId prevId = iTreeList->FocusedItem();
        	prevId = GetRootParent( prevId );
	    	iTreeVisualizer->CollapseAllL();
    	    if ( prevId != KFsTreeRootID && prevId != KFsTreeNoneID )
    	    	{
		        iTreeVisualizer->SetFocusedItemL( prevId );
    	    	}
	        }
	        break;

        case EFsEmailUiCmdActionsExpandAll:
		    {
        	TFsTreeItemId prevId = iTreeList->FocusedItem();	
		   	iTreeVisualizer->ExpandAllL();
    	    if ( prevId != KFsTreeRootID && prevId != KFsTreeNoneID )
    	    	{
		        iTreeVisualizer->SetFocusedItemL( prevId );
    	    	}
	        }
	        break;

        case EFsEmailUiCmdActionsCall:
		    {
		    CallToFocusedItemL();
	        }
	        break;
        // <cmail> video call
        case EFsEmailUiCmdActionsCallVideo:
            {
            iVideoCall = ETrue;
            CallToFocusedItemL();
            }
            break;
        // </cmail>
        case EFsEmailUiCmdActionsCreateMessage:
		    {
		    CreateMessageToFocusedItemL();
	        }
	        break;

        case EFsEmailUiCmdComposeTo:
            {
            SendEmailToFocusedItemL();
            }
            break;
            
        case EFsEmailUiCmdActionsContactDetails:
		    {
		    ShowContactDetailsForFocusedItemL();
	        }
	        break;

        case EFsEmailUiCmdActionsAddContact:
		    {
		    AddFocusedItemToContactsL();
	        }
	        break;

        case EFsEmailUiCmdActionsRemoteLookup:
		    {
		    LaunchRemoteLookupForFocusedItemL();
	        }
	        break;

        case EFsEmailUiCmdHelp:
		    {
        	TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
	        }
	        break;

        case EFsEmailUiCmdExit:
		    {
		    //<cmail>
		    iTreeList->SetFocusedL(EFalse);
		    //</cmail>
		    iAppUi.Exit();
	        }
	        break;
        
        case EFsEmailUiCmdActionsCollapseExpandAllToggle:
            {
            ShortcutCollapseExpandAllToggleL();
            }
            break;
        
        case EFsEmailUiCmdGoToTop:
            {
            GoToTopL();
            }
            break;
            
        case EFsEmailUiCmdGoToBottom:
            {
            GoToBottomL();
            }
            break;
            
        case EFsEmailUiCmdPageUp:
            {
            TKeyEvent simEvent = { EKeyPageUp, EStdKeyPageUp, 0, 0 };
            iCoeEnv->SimulateKeyEventL( simEvent, EEventKey );
            }
            break;
            
        case EFsEmailUiCmdPageDown:
            {
            TKeyEvent simEvent = { EKeyPageDown, EStdKeyPageDown, 0, 0 };
            iCoeEnv->SimulateKeyEventL( simEvent, EEventKey );
            }
            break;

        default:
        	break;
        }
    }


void CFSEmailUiMsgDetailsVisualiser::ClearMsgDetailsModelL()
	{
    FUNC_LOG;
	iTreeList->RemoveAllL();
	iModel->RemoveAll();

	iNodeIds.Reset();
	iToNodeId = iCcNodeId = iBccNodeId = KFsTreeNoneID;
	}

void CFSEmailUiMsgDetailsVisualiser::UpdateMsgDetailsModelL()
    {
    FUNC_LOG;
    // If there are lots of items under some node, then the list drawing
    // might become very slow because of scroll bar updating. So we deny
    // the list refresh during the construction. List refresh is still
    // allowed in case of node insert, as there aren't that many nodes,
    // and it will keep the scroll bar roughly in the map.
    iAllowListRefreshInInsert = ETrue;
    iExpandAndHighlightNextNode = EFalse;

	// Append lines to the model
	AppendFromLinesL();
	AppendSubjectLinesL();
	
	if( iExpandCollapseMode == ECollapseAllExceptTo )
	    {
	    iExpandAndHighlightNextNode = ETrue;
	    }
	AppendToLinesL();

	if( iExpandCollapseMode == ECollapseAllExceptCc )
        {
        iExpandAndHighlightNextNode = ETrue;
        }
	AppendCcLinesL();

	if( iExpandCollapseMode == ECollapseAllExceptBcc )
        {
        iExpandAndHighlightNextNode = ETrue;
        }
	AppendBccLinesL();
	AppendSizeLinesL();
	AppendSentLinesL();	
	AppendPriorityLinesL();
    // Allow list (scroll bar) refresh for last item(s)
    //iAllowListRefreshInInsert = ETrue;
	AppendMessageTypeLinesL();
  	}

void CFSEmailUiMsgDetailsVisualiser::CreateOneLinePlainItemLC2( const TDesC& aItemDataBuff,
                                                       		    CFsTreePlainOneLineItemData* &aItemData,
                                                       		    CFsTreePlainOneLineItemVisualizer* &aItemVisualizer )
	{
    FUNC_LOG;
    aItemData = CFsTreePlainOneLineItemData::NewL();
    CleanupStack::PushL( aItemData );
    aItemData->SetDataL( aItemDataBuff );
    
    aItemVisualizer = CFsTreePlainOneLineItemVisualizer::NewL(*iTreeList->TreeControl());
    CleanupStack::PushL( aItemVisualizer );
	aItemVisualizer->SetExtendable( EFalse ); // One line items are obviously not extendable
	aItemVisualizer->SetLayoutHints( CFsTreeItemVisualizerBase::EFolderLayout );

    SetItemVisualizerCommonProperties( *aItemVisualizer );
	}

void CFSEmailUiMsgDetailsVisualiser::CreateTwoLinePlainItemLC2( const TDesC& aPrimaryDataBuff,
															    const TDesC& aSecondaryDataBuff,
															    CFsTreePlainTwoLineItemData* &aItemData,
															    CFsTreePlainTwoLineItemVisualizer* &aItemVisualizer )
	{
    FUNC_LOG;
    aItemData = CFsTreePlainTwoLineItemData::NewL();
    CleanupStack::PushL( aItemData );
    aItemData->SetDataL( aPrimaryDataBuff );
	aItemData->SetSecondaryDataL( aSecondaryDataBuff );

    aItemVisualizer = CFsTreePlainTwoLineItemVisualizer::NewL(*iTreeList->TreeControl());
    CleanupStack::PushL( aItemVisualizer );
	aItemVisualizer->SetExtendable( ETrue ); // All two line items are extendable
	aItemVisualizer->SetMenu( NULL );
	aItemVisualizer->SetLayoutHints( CFsTreeItemVisualizerBase::EFolderLayout );

    SetItemVisualizerCommonProperties( *aItemVisualizer );
	}

void CFSEmailUiMsgDetailsVisualiser::SetItemVisualizerCommonProperties( MFsTreeItemVisualizer& aItemVisualizer )
	{
    FUNC_LOG;
    aItemVisualizer.SetSize(TSize(iScreenRect.Width(), iAppUi.LayoutHandler()->OneLineListItemHeight()));
	aItemVisualizer.SetExtendedSize(TSize(iScreenRect.Width(), iAppUi.LayoutHandler()->TwoLineListItemHeight()));

  	// Set correct skin text colors for the list items  
   	TRgb focusedColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
   	TRgb normalColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
    aItemVisualizer.SetFocusedStateTextColor( focusedColor );
    aItemVisualizer.SetNormalStateTextColor( normalColor );	
	}

void CFSEmailUiMsgDetailsVisualiser::CreatePlainNodeLC2( const TDesC& aItemDataBuff,
                                                         CFsTreePlainOneLineNodeData* &aItemData,
                                                         CFsTreePlainOneLineNodeVisualizer* &aNodeVisualizer )
	{
    FUNC_LOG;
    aItemData = CFsTreePlainOneLineNodeData::NewL();
    CleanupStack::PushL( aItemData );
    
    aItemData->SetDataL( aItemDataBuff );
	aItemData->SetIconExpanded( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeExpanded ) );
    aItemData->SetIconCollapsed( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeCollapsed ) );
    
    aNodeVisualizer = CFsTreePlainOneLineNodeVisualizer::NewL(*iTreeList->TreeControl());
    CleanupStack::PushL( aNodeVisualizer );
    
    SetNodeVisualizerProperties( *aNodeVisualizer );
    
    // Gradient background for headings    
    CAlfBrush *titleDividerBgBrush = iAppUi.FsTextureManager()->TitleDividerBgBrushL();
    aNodeVisualizer->SetBackgroundBrush( titleDividerBgBrush );
	}

void CFSEmailUiMsgDetailsVisualiser::SetNodeVisualizerProperties( MFsTreeItemVisualizer& aNodeVisualizer )
	{
    FUNC_LOG;
    aNodeVisualizer.SetSize(TSize(iScreenRect.Width(), iListNodeHeight));

  	// Set correct skin text colors for the list items  
   	TRgb focusedColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
   	TRgb normalColor = iAppUi.LayoutHandler()->ListNodeTextColor();
    aNodeVisualizer.SetFocusedStateTextColor( focusedColor );
    aNodeVisualizer.SetNormalStateTextColor( normalColor );
	
	// Set font size
	aNodeVisualizer.SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );		
	// Set node bolded	
	aNodeVisualizer.SetTextBold( ETrue );
	
	// Temporary fix for EASV-7GJFVD
	//aNodeVisualizer.SetBackgroundColorL( iAppUi.LayoutHandler()->ListNodeBackgroundColor() );
		
	}

TFsTreeItemId CFSEmailUiMsgDetailsVisualiser::AppendHeadingToListL( TInt aResourceId )
	{
    FUNC_LOG;
    CFsTreePlainOneLineNodeData* plainNodeData;
    CFsTreePlainOneLineNodeVisualizer* plainNodeVisualizer;

 	HBufC* headingText = StringLoader::LoadLC( aResourceId );

	CreatePlainNodeLC2( *headingText, plainNodeData, plainNodeVisualizer );
	
    TFsTreeItemId nodeId = iTreeList->InsertNodeL( *plainNodeData, *plainNodeVisualizer, KFsTreeRootID);
    CleanupStack::Pop( 2 ); // plainNodeData & plainNodeVisualizer

    // Set the node expanded/collapsed according to the member variables
    if( nodeId != KFsTreeNoneID ) // Safety check
        {
        if( iExpandCollapseMode == EExpandAll )
            {
            // The "normal" case, expand all and keep the first one highlighted
            iTreeList->ExpandNodeL( nodeId );
            }
        else if( iExpandAndHighlightNextNode )
            {
            // View opened to To, Cc or Bcc field and we just added the
            // corresponding node, so expand and focus it
            iTreeList->ExpandNodeL( nodeId );
            iTreeVisualizer->SetFocusedItemL( nodeId );
            }
        else
            {
            // View opened to To, Cc or Bcc field but we added some other
            // node, so collapse it
            iTreeList->CollapseNodeL( nodeId );
            }
        }
    // Reset the node expanding and highlighting flag
    iExpandAndHighlightNextNode = EFalse;
    
   	CleanupStack::PopAndDestroy(headingText);

   	return nodeId;
	}

TFsTreeItemId CFSEmailUiMsgDetailsVisualiser::AppendOneLineItemToListL( const TDesC& aItemData, TFsTreeItemId aParentNode )
	{
    FUNC_LOG;
    CFsTreePlainOneLineItemData* plainItemData;
    CFsTreePlainOneLineItemVisualizer* plainItemVisualizer;

    CreateOneLinePlainItemLC2( aItemData, plainItemData, plainItemVisualizer );

    TFsTreeItemId itemId = iTreeList->InsertItemL( *plainItemData, *plainItemVisualizer, aParentNode, KErrNotFound, iAllowListRefreshInInsert );
    CleanupStack::Pop( 2 ); // plainItemData & plainItemVisualizer

    return itemId;
	}

TFsTreeItemId CFSEmailUiMsgDetailsVisualiser::AppendTwoLineItemToListL( const TDesC& aPrimaryDataBuff,
															  			const TDesC& aSecondaryDataBuff,
															  			TFsTreeItemId aParentNode,
															  			TBool aItemHasActionMenu /*= EFalse*/ )
	{
    FUNC_LOG;
    CFsTreePlainTwoLineItemData* plainItemData;
    CFsTreePlainTwoLineItemVisualizer* plainItemVisualizer;

    CreateTwoLinePlainItemLC2( aPrimaryDataBuff, aSecondaryDataBuff, plainItemData, plainItemVisualizer );
    
    if ( aItemHasActionMenu )
        {
        plainItemVisualizer->SetFlags( plainItemVisualizer->Flags() | KFsTreeListItemHasMenu );
        }
   
    plainItemVisualizer->SetFlags(plainItemVisualizer->Flags() & ~KFsTreeListItemManagedLayout);  
  
    TFsTreeItemId itemId = iTreeList->InsertItemL( *plainItemData, *plainItemVisualizer, aParentNode, KErrNotFound, iAllowListRefreshInInsert );
    CleanupStack::Pop( 2 ); // plainItemData & plainItemVisualizer

    return itemId;
	}

TFsTreeItemId CFSEmailUiMsgDetailsVisualiser::AppendDateTimeItemToListL( const TDesC& aPrimaryDataBuff,
															  			 const TDesC& aSecondaryDataBuff,
															  			 const TDesC& aDateTimeDataBuff,
															  			 TFsTreeItemId aParentNode )
	{
    FUNC_LOG;
    CFsTreePlainTwoLineItemData* plainItemData;
    CFsTreePlainTwoLineItemVisualizer* plainItemVisualizer;

    CreateTwoLinePlainItemLC2( aPrimaryDataBuff, aSecondaryDataBuff, plainItemData, plainItemVisualizer );

	plainItemData->SetDateTimeDataL( aDateTimeDataBuff );
	plainItemVisualizer->SetExtendable( EFalse );
    
	TFsTreeItemId itemId = iTreeList->InsertItemL( *plainItemData, *plainItemVisualizer, aParentNode, KErrNotFound, iAllowListRefreshInInsert );
    CleanupStack::Pop( 2 ); // plainItemData & plainItemVisualizer

    return itemId;
	}

TBool CFSEmailUiMsgDetailsVisualiser::GetDisplayNameAndEmailAddressL( CFSMailAddress* aAddressData, TDesC* &aDisplayName, TDesC* &aEmailAddress )
	{
    FUNC_LOG;
 	aDisplayName = &aAddressData->GetDisplayName();
 	aEmailAddress = &aAddressData->GetEmailAddress();
 	TInt notFoundCount(0);
 	
 	// If display name is not set, then the plugins seem to set it to be same
 	// as email address. So there's no safe way to know wheter the display name
 	// is set or not. Best quess is that if display name is the same as email
 	// address, then there are no display name available.
 	if ( !aDisplayName || ( aDisplayName->Length() == 0 ) || ( *aDisplayName == *aEmailAddress ) )
		{
		// Internal variable used to store the text to avoid problems with ownership handling
		if( !iNoDisplayNameAvailableText )
			{
			iNoDisplayNameAvailableText = StringLoader::LoadL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_NO_DISPLAY_NAME );
			}
	
		aDisplayName = iNoDisplayNameAvailableText;
		notFoundCount++;
		}
 	if ( !aEmailAddress || ( aEmailAddress->Length() == 0 ) )
		{
		// Internal variable used to store the text to avoid problems with ownership handling
		if( !iNoEmailAddressAvailableText )
			{
			iNoEmailAddressAvailableText = StringLoader::LoadL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_NO_EMAIL_ADDRESS );
			}

		aEmailAddress = iNoEmailAddressAvailableText;
		notFoundCount++;
		}

	// If both display name and email address are empty, return EFalse
	if( notFoundCount > 1 )
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

void CFSEmailUiMsgDetailsVisualiser::AppendFromLinesL()
	{
    FUNC_LOG;
	TFsTreeItemId nodeId;
	if( iViewedMsg->IsFlagSet( EFSMsgFlag_CalendarMsg ) )
		{
		nodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_ORGANIZER );
		}
	else
		{
	 	nodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_FROM );
		}
 	iNodeIds.Append( nodeId );

 	CFSMailAddress* fromAddress = iViewedMsg->GetSender();
	// If CFSMailAddress not available, show "No sender info available"
 	if( fromAddress == NULL )
 		{
	 	HBufC* noSender = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_NO_SENDER_INFO_AVAILABLE );
		AppendOneLineItemToListL( *noSender, nodeId );
		CleanupStack::PopAndDestroy( noSender );
 		}
 	else
 		{
		TDesC* displayName( NULL );
		TDesC* emailAddress( NULL );
		if( GetDisplayNameAndEmailAddressL( fromAddress, displayName, emailAddress ) )
			{
			TFsTreeItemId itemId = AppendTwoLineItemToListL( *displayName, *emailAddress, nodeId, ETrue );
			iModel->AppendL( itemId, fromAddress );
			}
		else
			{
		 	HBufC* noSender = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_NO_SENDER_INFO_AVAILABLE );
			AppendOneLineItemToListL( *noSender, nodeId );
			CleanupStack::PopAndDestroy( noSender );
			}
 		}
	}

void CFSEmailUiMsgDetailsVisualiser::AppendSubjectLinesL()
	{
    FUNC_LOG;
 	TFsTreeItemId nodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_SUBJECT );
 	iNodeIds.Append( nodeId );

	TDesC* subject = &iViewedMsg->GetSubject();
	if ( subject == NULL || subject->Length() <= 0 )
		{
	 	HBufC* noSubject = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_NO_SUBJECT );
		AppendOneLineItemToListL( *noSubject, nodeId );
		CleanupStack::PopAndDestroy( noSubject );
		}
	else
		{
		HBufC* subjectText = TFsEmailUiUtility::CreateSubjectTextLC( iViewedMsg );
		TFsTreeItemId itemId = AppendOneLineItemToListL( *subjectText, nodeId );
		CleanupStack::PopAndDestroy( subjectText );
		iModel->AppendL( itemId, subject );
		}
	}

// Duplicate code in AppendToLinesL/AppendCcLinesL/AppendBccLinesL, create
// one generic function that can be used from all of these functions
void CFSEmailUiMsgDetailsVisualiser::AppendToLinesL()
	{
    FUNC_LOG;
	if( iViewedMsg->IsFlagSet( EFSMsgFlag_CalendarMsg ) )
		{
		iToNodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_REQUIRED );
		}
	else
		{
	 	iToNodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_TO );
		}
 	iNodeIds.Append( iToNodeId );
 
 	RPointerArray<CFSMailAddress>& toArray = iViewedMsg->GetToRecipients();
	TInt toArrayCount = toArray.Count(); 
	if ( toArrayCount )
		{
		for ( TInt i=0 ; i<toArrayCount ; i++ )
			{
		  	CFSMailAddress* toAddress = toArray[i];
		  	if( toAddress )
		  		{
			 	TDesC* displayName( NULL );
			 	TDesC* emailAddress( NULL );
				TBool toNameFound = GetDisplayNameAndEmailAddressL( toAddress, displayName, emailAddress );

				TFsTreeItemId itemId = AppendTwoLineItemToListL( *displayName, *emailAddress, iToNodeId, toNameFound );
				iModel->AppendL( itemId, toAddress );
		  		}
			}		
		}		
	else
		{
		//Laske To- ja Cc-vastaanottajat yhteens‰ ja n‰yt‰ t‰m‰ vain jos kumpiakaan ei ole yht‰‰n
	 	HBufC* noToText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_NO_VISIBLE_RECIPIENTS );
		AppendOneLineItemToListL( *noToText, iToNodeId );
		CleanupStack::PopAndDestroy( noToText );
		}
	}

void CFSEmailUiMsgDetailsVisualiser::AppendCcLinesL()
	{
    FUNC_LOG;
 	RPointerArray<CFSMailAddress>& ccArray = iViewedMsg->GetCCRecipients();
	TInt ccArrayCount = ccArray.Count(); 

	if ( ccArrayCount )
		{
		if( iViewedMsg->IsFlagSet( EFSMsgFlag_CalendarMsg ) )
			{
			iCcNodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_OPTIONAL );
			}
		else
			{
		 	iCcNodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_CC );
			}
	 	iNodeIds.Append( iCcNodeId );
	 	
		for ( TInt i=0 ; i<ccArrayCount ; i++ )
			{
		  	CFSMailAddress* ccAddress = ccArray[i];
		  	if( ccAddress )
		  		{
			 	TDesC* displayName( NULL );
			 	TDesC* emailAddress( NULL );
				TBool ccNameFound = GetDisplayNameAndEmailAddressL( ccAddress, displayName, emailAddress );

				TFsTreeItemId itemId = AppendTwoLineItemToListL( *displayName, *emailAddress, iCcNodeId, ccNameFound );
				iModel->AppendL( itemId, ccAddress );
		  		}
			}
		}
	}

void CFSEmailUiMsgDetailsVisualiser::AppendBccLinesL()
	{
    FUNC_LOG;
	// Get message's parent folder
	TFSMailMsgId folderId = iViewedMsg->GetFolderId();
	TFSMailMsgId mailboxId = iViewedMsg->GetMailBoxId();
	CFSMailFolder* folder = iAppUi.GetMailClient()->GetFolderByUidL( mailboxId, folderId );
	
	// Show bcc field only if message's parent folder is some outgoing folder,
	// so basically outbox, drafts or sent items.
	TBool showBcc( EFalse );
	if( folder )
		{
		TInt folderType = folder->GetFolderType();

		switch( folderType )
			{
			case EFSOutbox:
			case EFSDraftsFolder:
			case EFSSentFolder:
				{
				showBcc = ETrue;
				}
				break;

			case EFSInbox:
			case EFSDeleted:
			default:
				break;
			}
		}
	delete folder; 
	
	if( showBcc )
		{
	 	RPointerArray<CFSMailAddress>& bccArray = iViewedMsg->GetBCCRecipients();
		TInt bccArrayCount = bccArray.Count(); 

		if ( bccArrayCount )
			{
		 	iBccNodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_BCC );
		 	iNodeIds.Append( iBccNodeId );
		 	
			for ( TInt i=0 ; i<bccArrayCount ; i++ )
				{
			  	CFSMailAddress* bccAddress = bccArray[i];
			  	if( bccAddress )
			  		{
				 	TDesC* displayName( NULL );
				 	TDesC* emailAddress( NULL );
					TBool bccNameFound = GetDisplayNameAndEmailAddressL( bccAddress, displayName, emailAddress );

					TFsTreeItemId itemId = AppendTwoLineItemToListL( *displayName, *emailAddress, iBccNodeId, bccNameFound );
					iModel->AppendL( itemId, bccAddress );
			  		}
				}
			}
		}
	}

void CFSEmailUiMsgDetailsVisualiser::AppendSizeLinesL()
	{
    FUNC_LOG;
 	TFsTreeItemId nodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_SIZE_HEADER );
 	iNodeIds.Append( nodeId );
 	
 	// Gets the full content size (in bytes)
    TUint msgSize = iViewedMsg->ContentSize();

    HBufC* sizeText = TFsEmailUiUtility::CreateSizeDescLC( msgSize, ETrue );

	AppendOneLineItemToListL( *sizeText, nodeId );
	CleanupStack::PopAndDestroy( sizeText );
	}

void CFSEmailUiMsgDetailsVisualiser::AppendSentLinesL()
	{
    FUNC_LOG;
 	TFsTreeItemId nodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_SENT );
 	iNodeIds.Append( nodeId );

 	HBufC* dateFromMsg = TFsEmailUiUtility::DateTextFromMsgLC( iViewedMsg );
	HBufC* dateText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_DATE_U, *dateFromMsg );
 	
	AppendOneLineItemToListL( *dateText, nodeId );
		
	CleanupStack::PopAndDestroy( dateText );
 	CleanupStack::PopAndDestroy( dateFromMsg );
	
	//////////////////////////
 	HBufC* timeFromMsg = TFsEmailUiUtility::TimeTextFromMsgLC( iViewedMsg );
	HBufC* timeText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_TIME_U, *timeFromMsg );
 	
	AppendOneLineItemToListL( *timeText, nodeId );
	
	CleanupStack::PopAndDestroy( timeText );
 	CleanupStack::PopAndDestroy( timeFromMsg );
	}

void CFSEmailUiMsgDetailsVisualiser::AppendPriorityLinesL()
	{
    FUNC_LOG;
 	TFsTreeItemId nodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_PRIORITY );
 	iNodeIds.Append( nodeId );
 	
 	HBufC* priorityText( NULL );
	if ( iViewedMsg->IsFlagSet( EFSMsgFlag_Important ) )
		{
		priorityText = StringLoader::LoadL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_PRIORITY_HIGH );			
		}
	else if ( iViewedMsg->IsFlagSet( EFSMsgFlag_Low ) )
		{
		priorityText = StringLoader::LoadL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_PRIORITY_LOW );				
		}
	else
		{
		priorityText = StringLoader::LoadL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_PRIORITY_NORMAL );					
		}

	CleanupStack::PushL( priorityText );
	AppendOneLineItemToListL( *priorityText, nodeId );
	CleanupStack::PopAndDestroy( priorityText );
	}

void CFSEmailUiMsgDetailsVisualiser::AppendMessageTypeLinesL()
	{
    FUNC_LOG;
 	TFsTreeItemId nodeId = AppendHeadingToListL( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_MSG_TYPE );
 	iNodeIds.Append( nodeId );
 	
 	HBufC* msgType;
	if( iViewedMsg->IsFlagSet( EFSMsgFlag_CalendarMsg ) )
		{
		msgType = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_MSG_TYPE_MEETING );
		}
	else
		{
		msgType = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_MSG_DETAILS_MSG_TYPE_EMAIL );
		}
		
	AppendOneLineItemToListL( *msgType, nodeId );
	CleanupStack::PopAndDestroy( msgType );
	}

void CFSEmailUiMsgDetailsVisualiser::HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType /*aType*/ )
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        iTreeList->HideListL();        
        UpdateListSizeAttributes();
        iParentLayout->SetRect( iScreenRect );
        for ( TInt i = 0; i < iNodeIds.Count(); i++ )
            {
            MFsTreeItemVisualizer& vis = iTreeList->ItemVisualizer( iNodeIds[i] );
            SetNodeVisualizerProperties( vis );           
            SetChildVisualizersProperties( iNodeIds[i] );
            }
        iTreeList->ShowListL();        
        }
	}

// ---------------------------------------------------------------------------
// Sets tree item visualizer properties for all the childs of the given node
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsVisualiser::SetChildVisualizersProperties( TFsTreeItemId aNodeId )
	{
    FUNC_LOG;
	TUint childrenCount = iTreeList->CountChildren( aNodeId );
	for( TInt i = 0; i < childrenCount; ++i )
		{
		TFsTreeItemId childId = iTreeList->Child( aNodeId, i );
    	MFsTreeItemVisualizer& vis = iTreeList->ItemVisualizer( childId );
	    SetItemVisualizerCommonProperties( vis );
		}
	}

// ---------------------------------------------------------------------------
// Update list size members variables
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsVisualiser::UpdateListSizeAttributes()
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, iScreenRect );
        iScreenRect.SetRect( 0, 0, iScreenRect.Width(), iScreenRect.Height() );


        iListNodeHeight = iAppUi.LayoutHandler()->OneLineListNodeHeight();        
        }
	}

// ---------------------------------------------------------------------------
// Collapse nodes except the specified one (used when starting the list in 
// specific loaction)
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsVisualiser::CollapseNodesExceptL( TFsTreeItemId aExcludedNode )
	{
    FUNC_LOG;
	for( TInt i = 0; i < iNodeIds.Count(); i++ )
		{
		if( iNodeIds[i] != aExcludedNode )
			{
			iTreeList->CollapseNodeL( iNodeIds[i] );
			}
		}
	}

// ---------------------------------------------------------------------------
// If there is one or more expanded nodes, collapses all nodes.
// Otherwise expands all nodes.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsVisualiser::ShortcutCollapseExpandAllToggleL()
    {
    FUNC_LOG;
    TBool collapseAllNodes( EFalse );
    for( TInt i=0 ; i<iNodeIds.Count() ; i++ )
        {
        if( iTreeList->IsExpanded( iNodeIds[i] ) )
            {
            collapseAllNodes = ETrue;
            break;
            }	
        }

    if( collapseAllNodes )
        {
        HandleCommandL( EFsEmailUiCmdActionsCollapseAll );
        }
    else
        {
        HandleCommandL( EFsEmailUiCmdActionsExpandAll );
        }
    }

// ---------------------------------------------------------------------------
// Moves the focus to the topmost item
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsVisualiser::GoToTopL()
    {
    FUNC_LOG;
    if ( iNodeIds.Count() )
        {
        TFsTreeItemId topId = iNodeIds[0];
        iTreeVisualizer->SetFocusedItemL( topId );            
        }
    }

// ---------------------------------------------------------------------------
// Moves the focus to the bottommost item
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsVisualiser::GoToBottomL()
    {
    FUNC_LOG;
    if ( iNodeIds.Count() )
        {
        TFsTreeItemId bottomId = iNodeIds[ iNodeIds.Count()-1 ];
        TInt childCount = iTreeList->CountChildren(bottomId);
        if ( childCount && iTreeList->IsExpanded(bottomId) )
            {
            // Focus the last child of the bottom node if the node is expanded.
            bottomId = iTreeList->Child( bottomId, childCount-1 );
            }
        iTreeVisualizer->SetFocusedItemL( bottomId );            
        }
    }


// ---------------------------------------------------------------------------
// Recursive function to get the root parent of given item
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiMsgDetailsVisualiser::GetRootParent( const TFsTreeItemId aItemId ) const
	{
    FUNC_LOG;
	TFsTreeItemId parentId = iTreeList->Parent( aItemId );
	// If current item's parent is KFsTreeRootID, return its id
	if( parentId == KFsTreeRootID || parentId == KFsTreeNoneID )
		{
		return aItemId;
		}
	else
		{
		// Get the root parent recursively
		return GetRootParent( parentId );
		}
	}


// ---------------------------------------------------------------------------
// Tells if all expandable nodes are collapsed
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMsgDetailsVisualiser::AllNodesCollapsed() const
    {
    FUNC_LOG;
    TFsTreeItemId itemId = KFsTreeNoneID;
    TInt count = iTreeList->CountChildren(KFsTreeRootID);
	
	// If top level is collapsed, then everything is collapsed. There's no need
	// to crawl any deeper in the tree hierarchy.
	for ( TInt i=0 ; i<count ; ++i )
	    {
	    itemId = iTreeList->Child( KFsTreeRootID, i );
	    if ( iTreeList->IsNode(itemId) &&
	         iTreeList->IsExpanded(itemId) )
	        {
	        return EFalse;
	        }
	    }
	
	return ETrue;
    }

// ---------------------------------------------------------------------------
// Tells if all expandable nodes are expanded
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMsgDetailsVisualiser::AllNodesExpanded( TFsTreeItemId aParentNodeId ) const
    {
    FUNC_LOG;
    // We must crawl through the whole tree to see, if there are any collapsed nodes
    // at any level. We do this with recursive depth-first-search.
    
    TFsTreeItemId itemId = KFsTreeNoneID;
    TInt count = iTreeList->CountChildren(aParentNodeId);
    
    for ( TInt i=0 ; i<count ; ++i )
        {
        itemId = iTreeList->Child( aParentNodeId, i );
        if ( iTreeList->IsNode(itemId) )
            {
            if ( !iTreeList->IsExpanded(itemId) ||
                 !AllNodesExpanded(itemId) )
                {
                return EFalse;
                }
            }
        }
	
	return ETrue;
    }

// <cmail> Touch
// ---------------------------------------------------------------------------
// Process a treelist event
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsVisualiser::TreeListEventL( const TFsTreeListEvent aEvent, 
                                    const TFsTreeItemId /*aId*/,
                                    const TPoint& /*aPoint*/ )
    {
    switch (aEvent)
        {
        case EFsTreeListItemTouchAction:
            {
            if (TFsTreeItemId focId1 = iTreeList->FocusedItem())
                {
                if (iTreeList->IsNode(focId1))
                    {
                    if (iTreeList->IsExpanded(focId1))
                        {
                        HandleCommandL(EFsEmailUiCmdCollapse);
                        }
                    else
                        {
                        HandleCommandL(EFsEmailUiCmdExpand);
                        }
                    }
                else
                    {
                    SendEmailToFocusedItemL();
                    }
                }
            break;
            }
        case EFsTreeListItemTouchLongTap:
            {
            // Show action toolbar if the item has action menu.
            if( HasFocusedItemActionMenu() )
                {
                LaunchActionMenuL();
                }
            break;
            }
        case EFsTreeListItemWillGetFocused:
            {
            SetMskL(); 
            break;
            }
        }
    }


TPoint CFSEmailUiMsgDetailsVisualiser::ActionMenuPosition()
    {
    TAlfRealRect focusRect;
    TFsTreeItemId listItemId = iTreeList->FocusedItem();
    iTreeList->GetItemDisplayRectTarget(listItemId, focusRect);
    return focusRect.iTl;
    }
// </cmail>

void CFSEmailUiMsgDetailsVisualiser::GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const
    {
    aLayoutArray.AppendL( iParentLayout );
    }

