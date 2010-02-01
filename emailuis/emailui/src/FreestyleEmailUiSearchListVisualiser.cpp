/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file implements class CFSEmailUiSearchListVisualiser.
*
*/



// SYSTEM INCLUDES
#include "emailtrace.h"
#include <freestyleemailui.mbg>
#include <AknUtils.h>
#include <gulicon.h>
#include <StringLoader.h>
#include <FreestyleEmailUi.rsg>

//<cmail>
#include "CFSMailMessage.h"
#include "CFSMailClient.h"
#include "CFSMailFolder.h"
#include "CFSMailBox.h"
#include <alf/alfframebrush.h>
#include <alf/alftextvisual.h>
#include "fstreelist.h"
#include "fstreevisualizerbase.h"
#include "fstreeplaintwolineitemdata.h"
#include "fstreeplaintwolineitemvisualizer.h"
#include "fstreeplainonelinenodedata.h"
#include "fstreeplainonelinenodevisualizer.h"

//</cmail>
#include <AknQueryDialog.h>
//<cmail>
#include "CFSMailCommon.h"
#include <alf/alfanchorlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfevent.h>
#include <alf/alfbrusharray.h>
// </cmail>
#include <alf/alfenv.h>
#include <alf/alfcommand.h>
#include <aknnotewrappers.h>
// <cmail>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
// </cmail>
#include <featmgr.h>
//</cmail>
#include <aknstyluspopupmenu.h>

//<cmail>
#include "mfsccontactactionmenumodel.h"
#include "fscontrolbar.h"
#include "fscontrolbuttoninterface.h"
#include "fscontrolbuttonvisualiser.h"
#include <csxhelp/cmail.hlp.hrh>
// </cmail>

// Meeting request headers
#include <MeetingRequestUids.hrh>
//<cmail>
#include "cesmricalviewer.h"
//</cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiMailListModel.h"
#include "FreestyleEmailUiMailListVisualiser.h"
#include "FreestyleEmailUiSearchListVisualiser.h"
#include "FreestyleEmailUiSearchListControl.h"
#include "FreestyleEmailUiFileSystemInfo.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiMailViewerVisualiser.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiLiterals.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FreestyleEmailUiMsgDetailsVisualiser.h"
#include "FreestyleEmailCenRepHandler.h"
#include "FreestyleEmailUiHtmlViewerView.h"
#include "FSEmail.pan"

// CONST VALUES
static const TInt KMaxLengthOfSearchString = 255;
//<cmail> s60 platform layouts
//static const TInt KSearchIconWidth = 30;
const TReal KFSHeaderTextBackgroundOpacity = 0.3f;
//</cmail>
static const TInt KItemExpansionDelay = 400;
static const TInt KListScrollingDelay = 200;
static const TInt KMaxPreviewPaneLength = 60;

//<cmail> define search required priorities
#define KStandardSearchPriority   CActive::EPriorityStandard  
#define KCallingSearchPriority   ( CActive::EPriorityIdle - 1 )


CFSEmailUiSearchListVisualiser* CFSEmailUiSearchListVisualiser::NewL(CAlfEnv& aEnv,
																 CFreestyleEmailUiAppUi* aAppUi,
																 CAlfControlGroup& aSearchListControlGroup )
    {
    FUNC_LOG;
    CFSEmailUiSearchListVisualiser* self = CFSEmailUiSearchListVisualiser::NewLC(aEnv, aAppUi, aSearchListControlGroup );
    CleanupStack::Pop(self);
    return self;
    }

CFSEmailUiSearchListVisualiser* CFSEmailUiSearchListVisualiser::NewLC(CAlfEnv& aEnv,
																  CFreestyleEmailUiAppUi* aAppUi,
																  CAlfControlGroup& aSearchListControlGroup)
{
    FUNC_LOG;
    CFSEmailUiSearchListVisualiser* self = new (ELeave) CFSEmailUiSearchListVisualiser(aAppUi, aEnv, aSearchListControlGroup);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

void CFSEmailUiSearchListVisualiser::ConstructL()
	{
    FUNC_LOG;
	BaseConstructL( R_FSEMAILUI_MAIL_SEARCH_VIEW );

	// Set list as initial focused control
	iThisViewActive = EFalse;
	iFirstStartCompleted = EFalse;
   	}

// CFSEmailUiSearchListVisualiser::DoFirstStartL()
// Purpose of this function is to do first start only when search list is
// really needed to be shown. Implemented to make app startuo faster.
void CFSEmailUiSearchListVisualiser::DoFirstStartL()
    {
    FUNC_LOG;
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );

    // Create control and anchor layout
    iSearchListControl = CFreestyleEmailUiSearchListControl::NewL( *iEnv, this );
    iScreenAnchorLayout = CAlfAnchorLayout::AddNewL( *iSearchListControl );
    iScreenAnchorLayout->SetFlags(EAlfVisualFlagAutomaticLocaleMirroringEnabled);
    SetSearchListLayoutAnchors();

    // Create top bar layout
    iSearchTopBarLayout = CAlfDeckLayout::AddNewL( *iSearchListControl, iScreenAnchorLayout );
    iBarBgVisual = CAlfImageVisual::AddNewL( *iSearchListControl, iSearchTopBarLayout );
        /*<cmail> paltform layout changes to cmail
    TAlfTimedValue barBgTextureOpacity;
    barBgTextureOpacity.SetValueNow( 1 );
    iBarBgVisual->SetOpacity( barBgTextureOpacity );
    CAlfTexture* barBgTexture = &iAppUi.FsTextureManager()->TextureByIndex( EMailListBarBgIcon );
    barBgTexture->Size().SetSize( screenRect.Width(), iAppUi.LayoutHandler()->ControlBarHeight() );
    iBarBgVisual->SetImage( TAlfImage( *barBgTexture ) );
        */ //</cmail>

        /*<cmail> magnifier image is not in cmail
    // Add bar image component
    iBarFindImage = CAlfImageVisual::AddNewL( *iSearchListControl, iScreenAnchorLayout );
    TAlfTimedValue barFindImageOpacity;
    barFindImageOpacity.SetValueNow( 1 );
    iBarFindImage->SetOpacity( barFindImageOpacity );
    CAlfTexture* barFindTexture = &iAppUi.FsTextureManager()->TextureByIndex( ESearchLookingGlassIcon );
    barFindTexture->Size().SetSize( 15, 15 );
    iBarFindImage->SetImage( TAlfImage( *barFindTexture ) );
        */ //</cmail>

	// <cmail>
    // Add bar text component
    iBarTextVisual = CAlfTextVisual::AddNewL( *iSearchListControl, iScreenAnchorLayout );
    iBarTextVisual->EnableShadow(EFalse);
    iBarTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
	// </cmail>

	/*<cmail> no initial text or opacity required in cmail
    HBufC* initialSearchText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FIND_DLG_SEARCH );
    iBarTextVisual->SetTextL( *initialSearchText );
    CleanupStack::PopAndDestroy( initialSearchText );
    TAlfTimedValue barTextVisualOpacity;
    barTextVisualOpacity.SetValueNow( 1 );
    iBarTextVisual->SetOpacity( barTextVisualOpacity );

    CAlfTextStyle* textStyle = iAppUi.LayoutHandler()->FSTextStyleFromIdL( EFSFontTypeSmallBold );
    iBarTextVisual->SetTextStyle ( textStyle->Id() );*/ //</cmail>
    if ( !AknLayoutUtils::LayoutMirrored() )
        {
        iBarTextVisual->SetAlign(EAlfAlignHLeft, EAlfAlignVCenter);
        }
    else
        {
        iBarTextVisual->SetAlign(EAlfAlignHRight, EAlfAlignVCenter);
        }

    // Create model
    iModel = CFSEmailUiMailListModel::NewL( &iAppUi, ETrue );

    // Create list and append to control group and anchor layout
    iSearchListLayout = CAlfDeckLayout::AddNewL( *iSearchListControl, iScreenAnchorLayout );
    iSearchListLayout->SetSize( TSize( screenRect.Width(), screenRect.Height()-
            iAppUi.LayoutHandler()->ControlBarHeight() ) );
    iSearchTreeListVisualizer = CFsTreeVisualizerBase::NewL(iSearchListControl, *iSearchListLayout);
    iSearchList = CFsTreeList::NewL(*iSearchTreeListVisualizer, *iEnv );
    iSearchList->SetScrollbarVisibilityL( EFsScrollbarHideAlways );

    // Set mark type and icon
    iSearchList->SetMarkTypeL( CFsTreeList::EFsTreeListMultiMarkable );
    iSearchTreeListVisualizer->SetMarkIcon( iAppUi.FsTextureManager()->TextureByIndex( EListControlMarkIcon ) );
    iSearchTreeListVisualizer->SetMenuIcon( iAppUi.FsTextureManager()->TextureByIndex( EListControlMenuIcon ) );
	iSearchTreeListVisualizer->SetFlipState( iKeyboardFlipOpen );
	iSearchTreeListVisualizer->SetFocusVisibility( iFocusVisible );
    iSearchList->SetIndentationL(0);

    //<cmail> events are now offered to controls in different order
    ControlGroup().AppendL( iSearchListControl );
    ControlGroup().AppendL( iSearchList->TreeControl() );
    //</cmail>
    //<cmail> touch
    iSearchList->AddObserverL(*this);
    //</cmail>
    iSearchOngoing = EFalse;


    // Initializing the default stylus long tap popup menu
    if( !iStylusPopUpMenu )
        {
        TPoint point( 0, 0 );
        iStylusPopUpMenu = CAknStylusPopUpMenu::NewL( this , point );
        TResourceReader reader;
        iCoeEnv->CreateResourceReaderLC( reader,
                R_STYLUS_POPUP_MENU_SEARCH_LIST_VIEW );
        iStylusPopUpMenu->ConstructFromResourceL( reader );
        CleanupStack::PopAndDestroy();
        }

    // Set mail list background
    //<cmail> S60 skin support
    //iSearchTreeListVisualizer->SetBackgroundTextureL( iAppUi.FsTextureManager()->TextureByIndex( EBackgroundTextureMailList ) );
    //</cmail>

    iSearchTreeListVisualizer->SetItemExpansionDelay( KItemExpansionDelay );
    iSearchList->SetScrollTime( KListScrollingDelay, 0.5 );

    // Set empty text
    HBufC* emptyText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_EMPTY_MSG_LIST_TEXT );
    iSearchTreeListVisualizer->SetEmptyListTextL( *emptyText );
    CleanupStack::PopAndDestroy( emptyText );
    TRgb normalColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
    iSearchTreeListVisualizer->RootNodeVisualizer()->SetNormalStateTextColor( normalColor );

    // Create startup timer
    iStartupCallbackTimer = CFSEmailUiGenericTimer::NewL( this );

    // Set page up and page down keys
    iSearchTreeListVisualizer->AddCustomPageUpKey( EStdKeyPageUp );
    iSearchTreeListVisualizer->AddCustomPageDownKey( EStdKeyPageDown );

    CAlfBrush* selectorBrush = iAppUi.FsTextureManager()->ListSelectorBrushL();
    iSearchTreeListVisualizer->SetSelectorPropertiesL( selectorBrush, 1.0, CFsTreeVisualizerBase::EFsSelectorMoveSmoothly );

    iAppUi.LayoutHandler()->SetListMarqueeBehaviour( iSearchList );

    iFirstStartCompleted = ETrue;
   	}

void CFSEmailUiSearchListVisualiser::TimerEventL( CFSEmailUiGenericTimer* /*aTriggeredTimer*/ )
	{
    FUNC_LOG;
	LaunchSearchDialogL();
	}

CFSEmailUiSearchListVisualiser::CFSEmailUiSearchListVisualiser( CFreestyleEmailUiAppUi* aAppUi,
                                                                CAlfEnv& aEnv,
                                                                CAlfControlGroup& aSearchListControlGroup )
    :CFsEmailUiViewBase( aSearchListControlGroup, *aAppUi ),
    iEnv( &aEnv ),
    iLatestSearchText( 0 ),
    iListAddedToControlGroup( EFalse ),
    iPreparedForExit( EFalse ),
    iConsumeStdKeyYes_KeyUp( EFalse ), // for consuming the call event if call for contact processed
    iRequiredSearchPriority( KStandardSearchPriority )  // for search email priority decreasing
	{
    FUNC_LOG;
	}

CFSEmailUiSearchListVisualiser::~CFSEmailUiSearchListVisualiser()
	{
    FUNC_LOG;
    if ( iStartupCallbackTimer )
    	{
    	iStartupCallbackTimer->Cancel();
    	delete iStartupCallbackTimer;
    	}
	iSearchStrings.ResetAndDestroy();
	iSearchListItemArray.Reset();
    delete iModel;
	delete iLatestSearchText;
	delete iSearchList;
    delete iStylusPopUpMenu;
	}

void CFSEmailUiSearchListVisualiser::PrepareForExit()
    {
    FUNC_LOG;
    //<cmail> Not needed anymore as controls aren't removed from control group
    /*if(!iListAddedToControlGroup)
        {
        if(iSearchList && iSearchList->TreeControl())
            TRAP_IGNORE(ControlGroup().AppendL( iSearchList->TreeControl() ));
        if(iSearchListControl)
        TRAP_IGNORE(ControlGroup().AppendL( iSearchListControl ));
        iListAddedToControlGroup = ETrue;
        }*/
    //<cmail>
    if ( iSearchOngoing )
        {
        TRAP_IGNORE( StopSearchL() );
        }
    delete iMailBox;
    iMailBox = NULL;
    TRAP_IGNORE( ResetResultListL() );
    iPreparedForExit = ETrue;
    }

TInt CFSEmailUiSearchListVisualiser::HighlightedIndex() const
	{
    FUNC_LOG;
	TFsTreeItemId focusedId = static_cast<TFsTreeItemId>( iSearchList->FocusedItem() );
	// Map id to the index in model
	TInt ret( KErrNotFound );
	for ( TInt i=0; i<iSearchListItemArray.Count();i++ )
		{
		if ( focusedId == iSearchListItemArray[i].iSearchListItemId)
			{
			ret = i;
			break;
			}
		}
	return ret;
	}


TUid CFSEmailUiSearchListVisualiser::Id() const
	{
    FUNC_LOG;
	return SearchListViewId;
	}

// <cmail> Toolbar
/*void CFSEmailUiSearchListVisualiser::DoActivateL(const TVwsViewId& aPrevViewId,
                     TUid aCustomMessageId,
                     const TDesC8& aCustomMessage)*/
void CFSEmailUiSearchListVisualiser::ChildDoActivateL(const TVwsViewId& aPrevViewId,
                     TUid aCustomMessageId,
                     const TDesC8& aCustomMessage)
// </cmail> Toolbar
	{
    FUNC_LOG;
	if ( !iFirstStartCompleted )
	    {
	    DoFirstStartL();
	    }

	// <cmail>
    //iSearchList->SetScrollbarVisibilityL( EFsScrollbarHideAlways );
    if ( aCustomMessageId != KStartListReturnToPreviousFolder )
    	{
    	iBarTextVisual->SetTextL(KNullDesC);
    	}

	if ( &aCustomMessage && aCustomMessageId == KStartNewSearch )
		{
		// If new search, clear old search list
		iSearchCount=0;
		iMsgDataCouldBeChanged = EFalse;
		// <cmail> fixed CS high cat. finding
		ResetResultListL();
		}
	// </cmail>

	// Store previous view ID
	iPreviousViewUid = aPrevViewId.iViewUid;
	TSearchListActivationData subView;
	TPckgBuf<TSearchListActivationData> viewData( subView );
	viewData.Copy( aCustomMessage );
	subView = viewData();

	// Update settings and set list mode according to settings
	UpdateMailListSettingsL();

	if ( iMailBox )
		{
		delete iMailBox;
		iMailBox = NULL;
		}
	iMailBox = iAppUi.GetMailClient()->GetMailBoxByUidL( subView.iMailBoxId );
	// If mailbox data is not, try to get default MCE account
	if ( !iMailBox )
		{
		CMsvSession* msvSession = iAppUi.GetMsvSession();
		CFSMailClient* client = iAppUi.GetMailClient();
		iMailBox = TFsEmailUiUtility::GetMceDefaultMailboxL( *client, *msvSession );
		}

	if ( iMailBox )
		{
		iThisViewActive = ETrue;
                //<cmail> visible/unvisible is not controlled by opacity in cmail
		//TAlfTimedValue opacity;
		//opacity.SetValueNow( 1 );
		//iScreenAnchorLayout->SetOpacity( opacity );
                //</cmail>
		iSearchList->ShowListL();
		iSearchList->SetFocusedL(ETrue);
		// Set empty text color
	   	TRgb normalColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
		iSearchTreeListVisualizer->RootNodeVisualizer()->SetNormalStateTextColor( normalColor );

		// Launch search dialog automatically when activated
		if ( &aCustomMessage && aCustomMessageId == KStartNewSearch )
			{
			// <cmail> list reseting moved into the begining of the function
			// requeset async launching of the search dialog because DoActivateL must
			// not be blocked
			iStartupCallbackTimer->Cancel(); // just in case
			iStartupCallbackTimer->Start( iAppUi.LayoutHandler()->ViewSlideEffectTime() );
			}
		else if ( iModel && iModel->Count() )
			{
			// Need to make sure that return to this view works even if following function leaves.
			TRAP_IGNORE( CheckAndUpdateFocusedMessageL() );
			}
		}
    CAknEnv::Static()->GetCurrentGlobalUiZoom( iCurrentZoomLevel );
	// Set msk always empty when view is activated.
	SetMskL();
	// <cmail>
	ReScaleUiL(); // In case that Landscape/Portrait has changed.
	// </cmail>

	// <cmail> Touch
	iSearchList->SetFocusedL(ETrue);
	FocusVisibilityChange( iAppUi.IsFocusShown() );
	// </cmail>
 	}

void CFSEmailUiSearchListVisualiser::ChildDoDeactivate()
	{
    FUNC_LOG;
    if(iListAddedToControlGroup && (!iPreparedForExit))
        {
        iListAddedToControlGroup = EFalse;
        }
	iThisViewActive = EFalse;

    if ( !iAppUi.AppUiExitOngoing() )
        {
        if ( iSearchList->IsFocused() )
            {
            TRAP_IGNORE(iSearchList->SetFocusedL(EFalse));
            }
        iSearchTreeListVisualizer->NotifyControlVisibilityChange( EFalse );
        }
	}

CFSEmailUiMailListModel* CFSEmailUiSearchListVisualiser::Model()
	{
    FUNC_LOG;
	return iModel;
	}

void CFSEmailUiSearchListVisualiser::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane )
	{
    FUNC_LOG;
	if ( iSearchList->Count() && iModel )
		{
		if ( aResourceId == R_FSEMAILUI_SEARCHLIST_MENUPANE )
			{
		    if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
			   {
			   // remove help support in pf5250
			   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);
			   }

		  	CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
			if ( item && item->ModelItemType() == ETypeMailItem &&
				 item->MessagePtr().IsFlagSet( EFSMsgFlag_CalendarMsg ) )
				{
				// Highlighted mail is calendar msg
				aMenuPane->SetItemDimmed(EFsEmailUiCmdMailActions, ETrue);
				aMenuPane->SetItemDimmed(EFsEmailUiCmdCalActions, EFalse);
				}
			else
				{
				// Highlighted mail is mail msg
				aMenuPane->SetItemDimmed(EFsEmailUiCmdMailActions, EFalse);
				aMenuPane->SetItemDimmed(EFsEmailUiCmdCalActions, ETrue);
				}
			aMenuPane->SetItemDimmed(EFsEmailUiCmdOpen, EFalse);
			}
		}
	else
		{
		if ( aResourceId == R_FSEMAILUI_SEARCHLIST_MENUPANE )
			{

		    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
			   {
			   // remove help support in pf5250
			   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);
			   }

			aMenuPane->SetItemDimmed(EFsEmailUiCmdMore, ETrue);
			aMenuPane->SetItemDimmed(EFsEmailUiCmdMailActions, ETrue);
			aMenuPane->SetItemDimmed(EFsEmailUiCmdCalActions, ETrue);
			aMenuPane->SetItemDimmed(EFsEmailUiCmdOpen, ETrue);
			aMenuPane->SetItemDimmed(EFsEmailUiCmdReadEmail, ETrue);
			}
		}

	if ( aResourceId == R_FSEMAILUI_SEARCHLIST_SUBMENU_ACTIONS ||
	     aResourceId == R_FSEMAILUI_SEARCHLIST_SUBMENU_CALEVENT_ACTIONS)
		{
		if ( iSearchList->Count() )
			{
		  	CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
			if ( item && item->ModelItemType() == ETypeMailItem )
				{
				CFSMailMessage* messagePtr = &item->MessagePtr();
				if ( messagePtr && messagePtr->IsFlagSet( EFSMsgFlag_Read ) )
					{
					aMenuPane->SetItemDimmed(EFsEmailUiCmdMarkAsRead, ETrue);
					}
				else
					{
					aMenuPane->SetItemDimmed(EFsEmailUiCmdMarkAsUnread, ETrue);
					}
				}
			}
		}

	if ( aResourceId == R_FSEMAILUI_SEARCHLIST_SUBMENU_CALEVENT_ACTIONS)
		{
		if ( iSearchList->Count() )
			{
		  	CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
			if ( item && item->ModelItemType() == ETypeMailItem )
				{
				CFSMailMessage* messagePtr = &item->MessagePtr();
				if ( messagePtr && messagePtr->IsFlagSet( EFSMsgFlag_CalendarMsg ) )
					{
				    TESMRMeetingRequestMethod mrMethod( EESMRMeetingRequestMethodUnknown );
				    if ( iAppUi.MrViewerInstanceL() )
				    	{
				    	// Search must be stopped, otherwise resolve will lead to crash
		                if ( iSearchOngoing )
		                     {
		                     StopSearchL();
		                     }
					    mrMethod = iAppUi.MrViewerInstanceL()->ResolveMeetingRequestMethodL( item->MessagePtr() );
				   		}
					switch ( mrMethod )
						{
						case EESMRMeetingRequestMethodRequest:
						    {
							aMenuPane->SetItemDimmed(EFsEmailUiCmdCalRemoveFromCalendar, ETrue);
						    }
							break;
						case EESMRMeetingRequestMethodCancellation:
						    {
							aMenuPane->SetItemDimmed(EFsEmailUiCmdCalActionsAccept, ETrue);
							aMenuPane->SetItemDimmed(EFsEmailUiCmdCalActionsTentative, ETrue);
							aMenuPane->SetItemDimmed(EFsEmailUiCmdCalActionsDecline, ETrue);

							TBool supportsRemove = iAppUi.GetActiveMailbox()->HasCapability( EFSMBoxCapaRemoveFromCalendar );
							if( !supportsRemove )
                                {
                                aMenuPane->SetItemDimmed(EFsEmailUiCmdCalRemoveFromCalendar, ETrue);
                                }
						    }
							break;
						default:
						case EESMRMeetingRequestMethodUnknown:
						case EESMRMeetingRequestMethodResponse:
						    {
							aMenuPane->SetItemDimmed(EFsEmailUiCmdCalRemoveFromCalendar, ETrue);
							aMenuPane->SetItemDimmed(EFsEmailUiCmdCalActionsAccept, ETrue);
							aMenuPane->SetItemDimmed(EFsEmailUiCmdCalActionsTentative, ETrue);
							aMenuPane->SetItemDimmed(EFsEmailUiCmdCalActionsDecline, ETrue);
						    }
							break;
						}
					}
				}

			}
		}


	if ( aResourceId == R_FSEMAILUI_SEARCHLIST_SUBMENU_MORE )
		{
// <cmail> Prevent Download Manager opening with attachments
//		if ( iAppUi.DownloadInfoMediator() && !iAppUi.DownloadInfoMediator()->IsAnyAttachmentDownloads() )
//			{
//			aMenuPane->SetItemDimmed(EFsEmailUiCmdDownloadManager, ETrue);
//			}
// </cmail>
		}

	if (aResourceId == R_FSEMAILUI_SEARCHLIST_SUBMENU_ACTIONS)
	    {
        CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
        CFSMailMessage* messagePtr = &item->MessagePtr();
        TInt menuIndex( 0 );

        //Get # of recipients
        TInt numRecipients(0);
        if ( messagePtr )
            {
            numRecipients =TFsEmailUiUtility::CountRecepients( messagePtr );
            if ( numRecipients == 1 )
                {
                //check if the malbox ownmailaddress is same as the recipients email address. If not, then assume that the
                //email is a distribution list and we need to inc num of Recipients so that "Reply ALL" option appears in UI.
                if ( messagePtr->GetToRecipients().Count() )
                    {
                    if ( iAppUi.GetActiveMailbox()->OwnMailAddress().GetEmailAddress().Compare(messagePtr->GetToRecipients()[0]->GetEmailAddress()) )
                        {
                        numRecipients++;
                        }
                    }
                    if ( messagePtr->GetCCRecipients().Count() )
                        {
                        if ( iAppUi.GetActiveMailbox()->OwnMailAddress().GetEmailAddress().Compare(messagePtr->GetCCRecipients()[0]->GetEmailAddress()) )
                            {
                            numRecipients++;
                            }
                        }
                    if ( messagePtr->GetBCCRecipients().Count() )
                        {
                        if ( iAppUi.GetActiveMailbox()->OwnMailAddress().GetEmailAddress().Compare(messagePtr->GetBCCRecipients()[0]->GetEmailAddress()) )
                            {
                            numRecipients++;
                            }
                        }
                }

            }
        if ( numRecipients > 1 )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReplyAll, EFalse );
            }
        else if ( aMenuPane->MenuItemExists( EFsEmailUiCmdActionsReplyAll, menuIndex ) )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReplyAll, ETrue );
            }
	    }

	if ( !iSearchOngoing && aResourceId == R_FSEMAILUI_SEARCHLIST_MENUPANE )
		{
	    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);
		   }

		aMenuPane->SetItemDimmed(EFsEmailUiCmdStopSearch, ETrue);
		if ( iSearchCount == 0 )
			{
			aMenuPane->SetItemDimmed(EFsEmailUiCmdNewSearch, ETrue);
			}
		else
			{
			aMenuPane->SetItemDimmed(EFsEmailUiCmdSearch, ETrue);
			}
		}

	if ( iSearchOngoing && aResourceId == R_FSEMAILUI_SEARCHLIST_MENUPANE )
		{
	    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);
		   }

		aMenuPane->SetItemDimmed(EFsEmailUiCmdSearch, ETrue);
		aMenuPane->SetItemDimmed(EFsEmailUiCmdNewSearch, ETrue);
		}

	iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane, CFSEmailUiShortcutBinding::EContextSearchResults );
	}

void CFSEmailUiSearchListVisualiser::RefreshL()
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) //Safety
        {
        if ( iModel && iModel->Count() )
             {
             // Check if zoom level has been switched and refresh items if so
             TAknUiZoom prevZoomLevel = iCurrentZoomLevel;
             CAknEnv::Static()->GetCurrentGlobalUiZoom( iCurrentZoomLevel );
             if ( prevZoomLevel != iCurrentZoomLevel )
                 {
                 for ( TInt i=0; i<iModel->Count(); i++ )
                     {
                     // Set font height
                     iSearchListItemArray[i].iTreeItemVisualiser->
                         SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );
                     iSearchTreeListVisualizer->UpdateItemL( iSearchListItemArray[i].iSearchListItemId );
                     }
                 }
             }
        }
	}


void CFSEmailUiSearchListVisualiser::CreatePlainNodeL( const TDesC& aItemDataBuff,
                                                     CFsTreePlainOneLineNodeData* &aItemData,
                                                     CFsTreePlainOneLineNodeVisualizer* &aNodeVisualizer ) const
	{
    FUNC_LOG;
    aItemData = CFsTreePlainOneLineNodeData::NewL();
    aItemData->SetDataL( aItemDataBuff );
	aItemData->SetIconExpanded( iAppUi.FsTextureManager()->TextureByIndex(EListTextureNodeExpanded) );
    aItemData->SetIconCollapsed( iAppUi.FsTextureManager()->TextureByIndex(EListTextureNodeCollapsed) );
    aNodeVisualizer = CFsTreePlainOneLineNodeVisualizer::NewL( *iSearchList->TreeControl() );
   	TRect screenRect;
 	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );
    TInt nodeHeight = iAppUi.LayoutHandler()->OneLineListNodeHeight();
    aNodeVisualizer->SetSize( TSize(screenRect.Width(), nodeHeight) );
    aNodeVisualizer->SetExtendable(EFalse);
  	// Set correct skin text colors for the list items
   	TRgb focusedColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
   	TRgb normalColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
    aNodeVisualizer->SetFocusedStateTextColor( focusedColor );
    aNodeVisualizer->SetNormalStateTextColor( normalColor );
 	}


void CFSEmailUiSearchListVisualiser::HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType )
	{
    FUNC_LOG;
    CFsEmailUiViewBase::HandleDynamicVariantSwitchL( aType );
    if ( iFirstStartCompleted ) // Safety
        {
        if ( aType == EScreenLayoutChanged )
            {
            SetStatusBarLayout();
            }

        if ( iSearchTreeListVisualizer )
            {
            TRgb normalColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
            iSearchTreeListVisualizer->RootNodeVisualizer()->SetNormalStateTextColor( normalColor );
            ReScaleUiL();
            }       
        }
	}

void CFSEmailUiSearchListVisualiser::GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const
    {
    aLayoutArray.Append( iScreenAnchorLayout );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailListVisualiser::FlipStateChangedL
// Called when keyboard flip state is changed
// -----------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::FlipStateChangedL( TBool aKeyboardFlipOpen )
    {
    CFsEmailUiViewBase::FlipStateChangedL( aKeyboardFlipOpen );
    iSearchTreeListVisualizer->SetFlipState( iKeyboardFlipOpen );
    }

// -----------------------------------------------------------------------------
//  CFSEmailUiSearchListVisualiser::FocusVisibilityChange
// -----------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::FocusVisibilityChange(
        TBool aVisible )
    {
    CFsEmailUiViewBase::FocusVisibilityChange( aVisible );

    if ( iStylusPopUpMenuVisible && !aVisible )
        {
        // Do not allow to remove the focus from a list element if the pop up
        // menu was just launched.
        return;
        }

    iSearchTreeListVisualizer->SetFocusVisibility( aVisible );
    }

// ---------------------------------------------------------------------------
// CFSEmailUiSearchListVisualiser::SetStatusBarLayout()
// Sets status bar layout
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::SetStatusBarLayout()
	{
    TInt res = R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT;
    if( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        // landscape must use different layout
        res = R_AVKON_STATUS_PANE_LAYOUT_IDLE_FLAT;
        }

    if ( StatusPane()->CurrentLayoutResId() !=  res )
        {
        TRAP_IGNORE(
            StatusPane()->SwitchLayoutL( res ));
        }
	}

void CFSEmailUiSearchListVisualiser::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;


    switch ( aCommand )
        {
       	case EAknSoftkeySelect:
			{
			TInt modelCount(0);
			if ( iModel )
				{
				modelCount = iModel->Count();
				}
		 	if ( modelCount ) // Safety check
		 		{
			  	CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
				if ( item && item->ModelItemType() == ETypeMailItem )
					{
					CFSMailMessage* messagePtr = &item->MessagePtr();
					if ( messagePtr )
						{
						OpenHighlightedMailL();
						}
					}
		 		}
 			}
        	break;
        case EAknSoftkeyBack:
			{
			if ( !iAppUi.ViewSwitchingOngoing())
				{
				if ( iSearchOngoing )
					{
					StopSearchL();
					}
                                //<cmail> useless code removed
				//HBufC* searchText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FIND_DLG_SEARCH );
				//iBarTextVisual->SetTextL( *searchText );
				//CleanupStack::PopAndDestroy( searchText );
                                //</cmail>
		 		TVwsViewId viewId = TVwsViewId( KFSEmailUiUid, MailListId );
				TMailListActivationData tmp;
				if ( iMsgDataCouldBeChanged )
					{
					tmp.iRequestRefresh = ETrue;
					iMsgDataCouldBeChanged = EFalse; // List starts a new search
					}
				const TPckgBuf<TMailListActivationData> pkgOut( tmp );
				iAppUi.ReturnToPreviousViewL( pkgOut );
				}
			}
	       	break;
        case EEikCmdExit:
        case EAknSoftkeyExit:
        case EFsEmailUiCmdExit:
            iAppUi.Exit();
            break;
       	case EFsEmailUiCmdCalActionsReplyAsMail:
		case EFsEmailUiCmdActionsReply:
			{
			if ( iSearchList->Count() )
				{
				iMsgDataCouldBeChanged = ETrue;
		   	 	ReplyL( NULL ); // Function will check marked/highlighted msg
				}
			}
            break;
		case EFsEmailUiCmdActionsReplyAll:
			{
			if ( iSearchList->Count() )
				{
				iMsgDataCouldBeChanged = ETrue;
				ReplyAllL( NULL ); // Function will check marked/highlighted msg
				}
			}
            break;
       	case EFsEmailUiCmdCalActionsForwardAsMail:
		case EFsEmailUiCmdActionsForward:
			{
			if ( iSearchList->Count() )
				{
				iMsgDataCouldBeChanged = ETrue;
				ForwardL( NULL ); // Function will check marked/highlighted msg
				}
			}
            break;
		case EFsEmailUiCmdGoToTop:
			{
			if ( iSearchListItemArray.Count() )
			    {
			    TInt bottomItemId = iSearchListItemArray[0].iSearchListItemId;
    			iSearchTreeListVisualizer->SetFocusedItemL( bottomItemId );
			    }
			}
            break;
		case EFsEmailUiCmdGoToBottom:
			{
			if ( iSearchListItemArray.Count() )
			    {
			    TInt bottomItemId = iSearchListItemArray[ iSearchListItemArray.Count()-1 ].iSearchListItemId;
    			iSearchTreeListVisualizer->SetFocusedItemL( bottomItemId );
			    }
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

       	case EFsEmailUiCmdActionsDeleteCalEvent:
		case EFsEmailUiCmdActionsDelete:
			{
 			if ( iSearchList->Count() )
				{
				DeleteFocusedMessageL();
				}
			}
            break;
        case EAknSoftkeyOpen:
    		if (!iAppUi.IsFocusShown())
    			{
    			iAppUi.SetFocusVisibility( ETrue);
    			break;
    			}
        case EFsEmailUiCmdOpen:
        	{
 			if ( iSearchList->Count() )
				{
				OpenHighlightedMailL();
				}
			}
        	break;
       	case EFsEmailUiCmdNewSearch:
        case EFsEmailUiCmdSearch:
        	{
			LaunchSearchDialogL();
        	}
        	break;
        case EFsEmailUiCmdStopSearch:
        	{
			StopSearchL();
        	}
        	break;
       	case EFsEmailUiCmdMessageDetails:
			{
 			if ( iSearchList->Count() )
				{
				CFSEmailUiMailListModelItem* item =
					static_cast<CFSEmailUiMailListModelItem*>( Model()->Item( HighlightedIndex() ) );
				CFSMailMessage& msg = item->MessagePtr();

	  			TMsgDetailsActivationData msgDetailsData;
	  			msgDetailsData.iMailBoxId = msg.GetMailBoxId();
	  			msgDetailsData.iFolderId = msg.GetFolderId();
	  			msgDetailsData.iMessageId = msg.GetMessageId();

				const TPckgBuf<TMsgDetailsActivationData> pkgOut( msgDetailsData );
				iAppUi.EnterFsEmailViewL( MsgDetailsViewId, KStartMsgDetailsToBeginning,  pkgOut);
				}
			}
			break;
       	case EFsEmailUiCmdHelp:
  			{
			TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
  			}
  			break;
       	case EFsEmailUiCmdActionsCallSender:
			{
			if ( iSearchList->Count() )
				{
				CallToSenderL();
				}
			}
			break;
    	case EFsEmailUiCmdActionsAddContact:
    		{
			if ( iSearchList->Count() )
				{
			    CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
				if ( item && item->ModelItemType() == ETypeMailItem )
					{
					CFSMailAddress* fromAddress = item->MessagePtr().GetSender();
					TDesC* emailAddress(0);
					if ( fromAddress )
						{
						emailAddress = &fromAddress->GetEmailAddress();
						}
					if ( emailAddress && emailAddress->Length() )
						{
						TAddToContactsType aType;
						//Query to "update existing" or "Create new" --> EFALSE = user choosed "cancel"
						if ( CFsDelayedLoader::InstanceL()->GetContactHandlerL()->AddtoContactsQueryL( aType ) )
							{
							CFsDelayedLoader::InstanceL()->GetContactHandlerL()->AddToContactL(
									*emailAddress, EContactUpdateEmail, aType, this );
							}
						}
					}
				}
    		}
			break;
        case EFsEmailUiCmdMarkAsReadUnreadToggle:
            {
            if ( iSearchList->Count() )
                {
           		CFSEmailUiMailListModelItem* item =
        			static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ));
        		if ( item && item->ModelItemType() == ETypeMailItem )
        			{
        			if ( item->MessagePtr().IsFlagSet(EFSMsgFlag_Read) )
        				{
        				HandleCommandL( EFsEmailUiCmdMarkAsUnread );
        				}
        			else
        				{
        				HandleCommandL( EFsEmailUiCmdMarkAsRead );
        				}
        			}
                }
            }
            break;
        case EFsEmailUiCmdMarkAsUnread:
            {
            ChangeReadStatusOfHighlightedL( EFalse );
            }
            break;
        case EFsEmailUiCmdMarkAsRead:
            {
            ChangeReadStatusOfHighlightedL( ETrue );
            }
            break;
        case EFsEmailUiCmdCalActionsAccept:
        case EFsEmailUiCmdCalActionsTentative:
        case EFsEmailUiCmdCalActionsDecline:
        case EFsEmailUiCmdCalRemoveFromCalendar:
			{
		    CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
			if ( item && item->ModelItemType() == ETypeMailItem &&
				 item->MessagePtr().IsFlagSet( EFSMsgFlag_CalendarMsg ) )
				{
			    ChangeReadStatusOfHighlightedL( ETrue );
                iAppUi.MailViewer().HandleMrCommandL( aCommand,
                                                       iAppUi.GetActiveMailbox()->GetId(),
                                                       item->MessagePtr().GetFolderId(),
                                                       item->MessagePtr().GetMessageId() );
				}
			}
        	break;
// <cmail> Prevent Download Manager opening with attachments
//       	case EFsEmailUiCmdDownloadManager:
//       		{
//			if (iSearchOngoing)
//				{
//				StopSearchL();
//				}
//  			iAppUi.EnterFsEmailViewL( DownloadManagerViewId );
//       		}
//			break;
// </cmail>
       	case EFsEmailUiCmdReadEmail:
			{
			iAppUi.StartReadingEmailsL();
			}
			break;
        default:
        	break;
        }

    // Check if the focus needs to be removed after selecting an command from
    // the stylus popup menu.
    if ( ( iStylusPopUpMenuVisible ) &&
         ( aCommand == KErrCancel ||
           aCommand == EFsEmailUiCmdActionsDelete ||
           aCommand == EFsEmailUiCmdMarkAsRead ||
           aCommand == EFsEmailUiCmdMarkAsUnread ||
           aCommand == EFsEmailUiCmdActionsMove ||
           aCommand == EFsEmailUiCmdMarkUnmarkToggle ) )
        {
        // We end up here if the user selects an option from the pop up menu
        // or exits the menu by tapping outside of it's area.
        // Remove the focus from a list item if an item is focused.
        iStylusPopUpMenuVisible = EFalse;
        iAppUi.SetFocusVisibility( EFalse );
        }

    }


TInt CFSEmailUiSearchListVisualiser::LaunchSearchDialogL()
	{
    FUNC_LOG;
	if ( !iLatestSearchText )
		{
		iLatestSearchText = HBufC::NewL( KMaxLengthOfSearchString );
		}
	TPtr16 textData = iLatestSearchText->Des();
	CAknQueryDialog* dlg = CAknQueryDialog::NewL( textData );
	HBufC* searchPrompt = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FIND_DLG_LABEL );
	dlg->SetPromptL(*searchPrompt);
	CleanupStack::PopAndDestroy( searchPrompt );
	TInt ret = dlg->ExecuteLD( R_FSEMAILUI_FIND_MAIL_DIALOG );
	if ( ret )
		{
		// Stop any ongoing search before starting a new one
		if ( iSearchOngoing )
		    {
		    StopSearchL();
		    }
              //<cmail> make list and header visible by showing control group
		else if( !iListAddedToControlGroup )
            {
            //ControlGroup().AppendL( iSearchListControl );
            //ControlGroup().AppendL( iSearchList->TreeControl() );
            iListAddedToControlGroup = ETrue;
            }
		iSearchList->SetScrollbarVisibilityL( EFsScrollbarAuto );
             //</cmail>
		// Set searching text
		HBufC* searchText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FIND_DLG_SEARCHING );
		HBufC* finalText = HBufC::NewLC( searchText->Length() + iLatestSearchText->Length() + KSpace().Length() );
		finalText->Des().Append( *searchText );
		finalText->Des().Append( KSpace );
		finalText->Des().Append( *iLatestSearchText );
		iBarTextVisual->SetTextL( *finalText );
		CleanupStack::PopAndDestroy( finalText );
		CleanupStack::PopAndDestroy( searchText );
		StartSearchL();
		}
	return ret;
	}


void CFSEmailUiSearchListVisualiser::UpdateMailListSettingsL()
	{
    FUNC_LOG;
	if ( iAppUi.GetCRHandler() )
		{
		iNodesInUse = iAppUi.GetCRHandler()->TitleDividers();
		TInt lineValue = iAppUi.GetCRHandler()->MessageListLayout();
		TInt bodyPreviewValue = iAppUi.GetCRHandler()->BodyPreview();
		if ( lineValue == 1 ) // 1-line layouts
			{
			if ( bodyPreviewValue == 0 )
				{
				iListMode = EListControlTypeSingleLinePreviewOff;
				}
			else
				{
				iListMode = EListControlTypeSingleLinePreviewOn;
				}
			}
		else //
			{
			if ( bodyPreviewValue == 0 )
				{
				iListMode = EListControlTypeDoubleLinePreviewOff;
				}
			else
				{
				iListMode = EListControlTypeDoubleLinePreviewOn;
				}
			}
		}
	else
		{
		iNodesInUse = EListControlSeparatorDisabled;
		iListMode = EListControlTypeDoubleLinePreviewOff;
		}
	}


TBool CFSEmailUiSearchListVisualiser::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    TBool result(EFalse);
    // On KeyUp of EStdKeyYes usually Call application is called - prevent it if call for contact was previously called
    if ( iConsumeStdKeyYes_KeyUp && aEvent.IsKeyEvent() && (aEvent.Code() == EEventKeyUp )) 
		{
		iConsumeStdKeyYes_KeyUp = EFalse; // in case call button was consumed elsewhere first key up enables calling Call application
		if ( EStdKeyYes == aEvent.KeyEvent().iScanCode) 
			{
			  result = ETrue; // consume not to switch to Call application when call to contact was processed
			}
		}

    if ( aEvent.IsKeyEvent() && aEvent.Code() == EEventKey )
        {
        TInt scanCode = aEvent.KeyEvent().iScanCode;
        // Swap right and left controls in mirrored layout
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            if ( scanCode == EStdKeyRightArrow ) scanCode = EStdKeyLeftArrow;
            else if ( scanCode == EStdKeyLeftArrow ) scanCode = EStdKeyRightArrow;
            }
        // Handle possible focus visibility change
		if ((scanCode == EStdKeyRightArrow)
			|| (scanCode == EStdKeyLeftArrow)
			|| (scanCode == EStdKeyUpArrow)
			|| (scanCode == EStdKeyDownArrow)
			|| (scanCode == EStdKeyEnter)
			|| (scanCode == EStdKeyDeviceA)
			|| (scanCode == EStdKeyDevice3))
			{
            // If the focus was not active already, ignore the key press
            if( !iAppUi.SetFocusVisibility( ETrue ) )
                {
                return ETrue;
                }

			}
        switch ( scanCode )
            {
            case EStdKeyDevice3: // CENTER CLICK
            case EStdKeyEnter:  // ENTER EITHER SELECTS ITEM IN TOOLBAR OR OPENS MAIL
            case EStdKeyNkpEnter:
                {
                if ( iSearchList->Count() )
                    {
                    OpenHighlightedMailL();
                    }
                result = ETrue;
                }
                break;
            case EStdKeyYes:
                {
                if ( iSearchList->Count() )
                    {
                    result = CallToSenderL();	
                    iConsumeStdKeyYes_KeyUp = result;
                    }
                }
                break;
            default:
                {
                // check keyboard shortcuts:
                TInt command =
                    iAppUi.ShortcutBinding().CommandForShortcutKey( aEvent.KeyEvent(),
                        CFSEmailUiShortcutBinding::EContextSearchResults );
                if ( command >= 0 )
                    {
                    HandleCommandL( command );
                    result = ETrue;
                    }
                }
                break;
            }
        }
    else if (aEvent.IsPointerEvent())
        {
        iSearchList->TreeControl()->OfferEventL(aEvent);
        }

    return result;
    }



void CFSEmailUiSearchListVisualiser::ReScaleUiL()
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) //Safety
        {
        SetSearchListLayoutAnchors();
        iScreenAnchorLayout->UpdateChildrenLayout();
        iSearchListLayout->UpdateChildrenLayout();
        // Set bar text color from skin
        if ( iBarTextVisual )
            {
			//<cmail>
			SetHeaderAttributesL();
			/*TRgb barTextColor( KRgbBlack );
		    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
		                               barTextColor, KAknsIIDFsTextColors, EAknsCIFsTextColorsCG10 );
			iBarTextVisual->SetColor( barTextColor );*/
			//</cmail>
            }
        RefreshL();
        //iSearchTreeListVisualizer->HideList();
        iSearchTreeListVisualizer->ShowListL();
        }
 	}

void CFSEmailUiSearchListVisualiser::SetSearchListLayoutAnchors()
	{
    FUNC_LOG;
	// Set anchors so that list leaves space for control bar

	// The anchor layout mirrors itself automatically when necessary.
	// There's no need to mirror anything manually here.

	// BAR BACGROUND IMAGE
    TRect contBarRect = iAppUi.LayoutHandler()->GetControlBarRect();
    TPoint& tl( contBarRect.iTl );
    iScreenAnchorLayout->SetAnchor(EAlfAnchorTopLeft, 0,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( tl.iX, tl.iY ));
    TPoint& br( contBarRect.iBr );
    iScreenAnchorLayout->SetAnchor(EAlfAnchorBottomRight, 0,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( br.iX, br.iY ));

	// TEXT
    TRect textRect =  iAppUi.LayoutHandler()->GetSearchListHeaderTextLayout().TextRect();
    tl = textRect.iTl;
    iScreenAnchorLayout->SetAnchor(EAlfAnchorTopLeft, 1,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( tl.iX, tl.iY ));
    br = textRect.iBr;
    iScreenAnchorLayout->SetAnchor(EAlfAnchorBottomRight, 1,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( br.iX, br.iY ));

    TRect listRect = iAppUi.LayoutHandler()->GetListRect( ETrue );
    iScreenAnchorLayout->SetAnchor(EAlfAnchorTopLeft, 2,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint(listRect.iTl.iX, listRect.iTl.iY));
    iScreenAnchorLayout->SetAnchor(EAlfAnchorBottomRight, 2,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint(listRect.iBr.iX, listRect.iBr.iY));
	}


TFSMailMsgId CFSEmailUiSearchListVisualiser::MsgIdFromIndex( TInt aItemIdx ) const
    {
    FUNC_LOG;
	TFSMailMsgId msgId; // constructs null ID
    if ( 0 <= aItemIdx && aItemIdx < iModel->Count() )
        {
        CFSEmailUiMailListModelItem* item =
    			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(aItemIdx));
    	if ( item->ModelItemType() == ETypeMailItem )
    	    {
    	    msgId = item->MessagePtr().GetMessageId();
    	    }
        }

    return msgId;
    }

TFSMailMsgId CFSEmailUiSearchListVisualiser::MsgIdFromListId( TFsTreeItemId aListId ) const
	{
    FUNC_LOG;
	TFSMailMsgId msgId;
	for ( TInt i=0; i<iModel->Count();i++)
		{
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(i));
		if ( item->ModelItemType() == ETypeMailItem &&
		     aListId == item->CorrespondingListId() )
			{
			msgId = item->MessagePtr().GetMessageId();
			break;
			}
		}
	return msgId;
	}

CFSMailMessage& CFSEmailUiSearchListVisualiser::MsgPtrFromListId( TFsTreeItemId aListId )
	{
    FUNC_LOG;
	CFSMailMessage* msgPtr(NULL);
	for ( TInt i=0; i<iModel->Count();i++)
		{
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(i));
		if ( aListId == item->CorrespondingListId() )
			{
			msgPtr = &item->MessagePtr();
			}
		}
	return *msgPtr;
	}

	// Item data and visualiser helper functions
MFsTreeItemData* CFSEmailUiSearchListVisualiser::ItemDataFromItemId( TFsTreeItemId aItemId )
	{
    FUNC_LOG;
	TInt i(0);
	MFsTreeItemData* itemData(NULL);
	for ( ; i<iSearchListItemArray.Count(); i++ )
		{
		if ( aItemId == iSearchListItemArray[i].iSearchListItemId )
			{
			itemData = iSearchListItemArray[i].iTreeItemData;
			}
		}
	return itemData;
	}

MFsTreeItemVisualizer* CFSEmailUiSearchListVisualiser::ItemVisualiserFromItemId( TFsTreeItemId aItemId )
	{
    FUNC_LOG;
	TInt i(0);
	MFsTreeItemVisualizer* itemVis(NULL);
	for ( ; i<iSearchListItemArray.Count(); i++ )
		{
		if ( aItemId == iSearchListItemArray[i].iSearchListItemId )
			{
			itemVis = iSearchListItemArray[i].iTreeItemVisualiser;
			}
		}
	return itemVis;
	}

// Helpers to get the ordinal of a message in the iModel
TInt CFSEmailUiSearchListVisualiser::ItemIndexFromMessageId( const TFSMailMsgId& aMessageId ) const
    {
    FUNC_LOG;
	TInt idx = KErrNotFound;

	for ( TInt i=0; i<iModel->Count() ; i++ )
		{
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(i));
		if ( item->ModelItemType() == ETypeMailItem &&
		     aMessageId == item->MessagePtr().GetMessageId() )
			{
			idx = i;
			break;
			}
		}

	return idx;
    }

TInt CFSEmailUiSearchListVisualiser::NextMessageIndex( TInt aCurMsgIdx ) const
    {
    FUNC_LOG;
	TInt idx = KErrNotFound;

	for ( TInt i=aCurMsgIdx+1 ; i<iModel->Count() ; i++ )
		{
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(i));
		if ( item && item->ModelItemType() == ETypeMailItem )
			{
			idx = i;
			break;
			}
		}

	return idx;
    }

TInt CFSEmailUiSearchListVisualiser::PreviousMessageIndex( TInt aCurMsgIdx ) const
    {
    FUNC_LOG;
	TInt idx = KErrNotFound;

	if ( aCurMsgIdx < iModel->Count() )
	    {
    	for ( TInt i=aCurMsgIdx-1 ; i>=0 ; i-- )
    		{
    		CFSEmailUiMailListModelItem* item =
    			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item(i));
    		if ( item && item->ModelItemType() == ETypeMailItem )
    			{
    			idx = i;
    			break;
    			}
    		}
	    }

	return idx;
    }


void CFSEmailUiSearchListVisualiser::OpenHighlightedMailL()
	{
    FUNC_LOG;
	if (iSearchOngoing)
		{
		StopSearchL();
		}

	CFSEmailUiMailListModelItem* item =
		static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ) );
	if ( item->ModelItemType() == ETypeMailItem )
		{
		// First make sure that the highlighted message really exists in the store
		// Get confirmed msg ptr
		CFSMailMessage* confirmedMsgPtr(0);
		TRAPD( err, confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL(iAppUi.GetActiveMailboxId(),
							item->MessagePtr().GetFolderId(), item->MessagePtr().GetMessageId(), EFSMsgDataEnvelope ) );
		if ( confirmedMsgPtr && err == KErrNone )
			{
		 	CFSMailFolder* highlightedMsgFolder =
		 		iAppUi.GetMailClient()->GetFolderByUidL( iAppUi.GetActiveMailboxId(), confirmedMsgPtr->GetFolderId() );
			CleanupStack::PushL( highlightedMsgFolder );
			TInt msgFolderType = highlightedMsgFolder->GetFolderType();
			TFSMailMsgId highlightedMsgFolderId = highlightedMsgFolder->GetFolderId();
			CleanupStack::PopAndDestroy( highlightedMsgFolder );
			// Pointer confirmed, store Id and delete not needed anymore
			TFSMailMsgId confirmedId = confirmedMsgPtr->GetMessageId();
			delete confirmedMsgPtr;

			// Open to editor from drafts
			if ( msgFolderType == EFSDraftsFolder )
				{
				TEditorLaunchParams params;
				params.iMailboxId = iAppUi.GetActiveMailboxId();
				params.iActivatedExternally = EFalse;
				params.iMsgId = confirmedId;
				params.iFolderId = highlightedMsgFolderId;
			    iAppUi.LaunchEditorL( KEditorCmdOpen, params );
				}
			else if ( msgFolderType == EFSOutbox )
				{
				TFsEmailUiUtility::ShowErrorNoteL( R_FREESTYLE_EMAIL_UI_OPEN_FROM_OUTBOX_NOTE, ETrue );
				}
			else
				{
				THtmlViewerActivationData tmp;
				tmp.iMailBoxId = iAppUi.GetActiveMailbox()->GetId();
				tmp.iMessageId = confirmedId;
				tmp.iFolderId = highlightedMsgFolderId;

				const TPckgBuf<THtmlViewerActivationData> pkgOut( tmp );
				ChangeReadStatusOfHighlightedL( ETrue );
				iAppUi.EnterFsEmailViewL( HtmlViewerId, KStartViewerWithMsgId, pkgOut );
				}
			}
		else if ( err == KErrNotFound )
			{
			TRAP_IGNORE( CheckAndUpdateFocusedMessageL() );
			}
		}
	}



void CFSEmailUiSearchListVisualiser::ReplyL( CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
	DoReplyForwardL( KEditorCmdReply, aMsgPtr );
	}

void CFSEmailUiSearchListVisualiser::ReplyAllL(  CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
	DoReplyForwardL( KEditorCmdReplyAll, aMsgPtr );
	}

void CFSEmailUiSearchListVisualiser::ForwardL( CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
	DoReplyForwardL( KEditorCmdForward, aMsgPtr );
	}

void CFSEmailUiSearchListVisualiser::DoReplyForwardL( TEditorLaunchMode aMode, CFSMailMessage* aMsgPtr )
    {
    FUNC_LOG;
    // Stop search when reply/forward has been selected
	if ( iSearchOngoing )
		{
		StopSearchL();
		}

    if ( iModel->Count() )
        {
       	CFSMailMessage* messagePointer = aMsgPtr;
    	if ( !messagePointer  )
    		{
            RFsTreeItemIdList markedEntries;
    		iSearchList->GetMarkedItemsL( markedEntries );
    		TInt markedCount = markedEntries.Count();
    		if ( markedCount == 0 )
    			{
       		    CFSEmailUiMailListModelItem* item = NULL;
       		    item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
       		    if (item)
       		        {
       		        messagePointer = &item->MessagePtr();
       		        }
    			}
    		else if ( markedCount == 1)
    			{
    			messagePointer = &MsgPtrFromListId( markedEntries[0] );
    			}
    		}
    	if ( messagePointer )
    		{
    		// No reply/Forward for calendar messages, at least not in 1.0
	   		if ( !messagePointer->IsFlagSet( EFSMsgFlag_CalendarMsg ) )
    			{
	    		TEditorLaunchParams params;
	    		params.iMailboxId = iAppUi.GetActiveMailboxId();
	    		params.iActivatedExternally = EFalse;
	    		params.iMsgId = messagePointer->GetMessageId();
	    		iAppUi.LaunchEditorL( aMode, params );
    			}
    		}
        }
    }

void CFSEmailUiSearchListVisualiser::StartSearchL()
	{
    FUNC_LOG;
	iSearchCount++;

    // Reset previous results before starting new search
	// <cmail> fixed CS high cat. finding
	ResetResultListL();
    TLex lex( *iLatestSearchText );
    while ( !lex.Eos() )
        {
        HBufC* token = lex.NextToken().AllocLC();

        iSearchStrings.AppendL( token );

        CleanupStack::Pop( token );
        }

	TFSMailSortCriteria sortCriteria;
    sortCriteria.iField = EFSMailSortByDate;
    sortCriteria.iOrder = EFSMailDescending;
   	iSearchOngoing = ETrue;
	// Model and list is set to be empty, set also MSK to empty
	SetMskL();
	// Initialisr TextSearcher

	// Start search.
	iMailBox->SearchL( iSearchStrings, sortCriteria, *this );
	}

void CFSEmailUiSearchListVisualiser::StopSearchL()
	{
    FUNC_LOG;
	iSearchOngoing = EFalse;
	if ( iMailBox )
		{
		iMailBox->CancelSearch();
		}
	HBufC* searchText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FIND_DLG_SEARCH_RESULTS );
	HBufC* finalText = HBufC::NewLC( searchText->Length() + iLatestSearchText->Length() + 4 );
	finalText->Des().Append( *searchText );
	finalText->Des().Append( KSpace );
	finalText->Des().Append( *iLatestSearchText );
	iBarTextVisual->SetTextL( *finalText );
	CleanupStack::PopAndDestroy( finalText );
	CleanupStack::PopAndDestroy( searchText );
	iRequiredSearchPriority = KStandardSearchPriority; // <cmail> return back
	}

void CFSEmailUiSearchListVisualiser::ResetResultListL()
    {
    FUNC_LOG;
    if( iModel )
        {
        iModel->Reset();
        }
    if( iSearchList )
        {
        iSearchList->RemoveAllL();
        }
    iSearchListItemArray.Reset();
    iSearchStrings.ResetAndDestroy();
    }

void CFSEmailUiSearchListVisualiser::MatchFoundL( CFSMailMessage* aMatchMessage )
	{
    FUNC_LOG;

	if ( !iSearchList->IsFocused() )
		{
		iSearchList->SetFocusedL( ETrue );
		}

	// Delete receved owned object and add confirmed msg pointer to model insteaed
	// This is done because it seems that matched message objects seems to be different
	// in some protocols than original messages
	CleanupStack::PushL( aMatchMessage );
	CFSMailMessage* confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL( iAppUi.GetActiveMailboxId(),
						aMatchMessage->GetFolderId(), aMatchMessage->GetMessageId(), EFSMsgDataEnvelope );
	CleanupStack::PopAndDestroy( aMatchMessage );

	if ( confirmedMsgPtr ) // Append item into model and list if msg pointer was confirmed
		{
		// Append to model
		CFSEmailUiMailListModelItem* newItem = CFSEmailUiMailListModelItem::NewL( confirmedMsgPtr, ETypeMailItem);
		iModel->AppendL(newItem);

		// Append to list
	   	TRect screenRect;
	 	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );

		CFSMailAddress* fromAddress = confirmedMsgPtr->GetSender();
		TDesC* diplayName = NULL;
		if ( fromAddress )
			{
			diplayName = &fromAddress->GetDisplayName();
			}

		// Set first line of data
		HBufC* dispName = NULL;
		if ( fromAddress && diplayName && diplayName->Length() != 0 )
			{
			dispName = HBufC::NewLC( diplayName->Length() );
			dispName->Des().Append( *diplayName );
			}
		else if ( fromAddress && fromAddress->GetEmailAddress().Length() != 0 )
			{
			dispName = HBufC::NewLC( fromAddress->GetEmailAddress().Length() );
			dispName->Des().Append( fromAddress->GetEmailAddress() );
			}
		else
			{
			dispName = HBufC::NewLC( 0 );
			dispName->Des().Append( KNullDesC );
			}

		// Drop out unwanted characters from display name such as <> and ""
		// And set display name data
		if ( dispName )
			{
			TFsEmailUiUtility::StripDisplayName( *dispName );
			}
		// Create item data and
		CFsTreePlainTwoLineItemData* itemData = CFsTreePlainTwoLineItemData::NewL();

		itemData->SetDataL( *dispName );
		CleanupStack::PopAndDestroy( dispName );

		// Set time text data
		HBufC* timeText = TFsEmailUiUtility::ListMsgTimeTextFromMsgLC( confirmedMsgPtr, EFalse );
		itemData->SetDateTimeDataL ( *timeText );
		CleanupStack::PopAndDestroy();

		// Set second line of data
		HBufC* subjectText = TFsEmailUiUtility::CreateSubjectTextLC( confirmedMsgPtr );
		itemData->SetSecondaryDataL( *subjectText );
		CleanupStack::PopAndDestroy( subjectText );

		// Set message icon
		CAlfTexture* itemTexture = &TFsEmailUiUtility::GetMsgIcon( confirmedMsgPtr, *iAppUi.FsTextureManager() );
		itemData->SetIcon ( *itemTexture );
	    CFsTreePlainTwoLineItemVisualizer* itemVisualizer = CFsTreePlainTwoLineItemVisualizer::NewL(*iSearchList->TreeControl());

		itemVisualizer->SetExtendable( ETrue );
		TInt itemHeight = iAppUi.LayoutHandler()->OneLineListItemHeight();

		itemVisualizer->SetSize( TSize( screenRect.Width(), itemHeight ) );
	 	itemVisualizer->SetExtendedSize( TSize( screenRect.Width(), 2*itemHeight ) );

		// Set font height
		itemVisualizer->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );

		// Set font bolding
		if ( confirmedMsgPtr->IsFlagSet( EFSMsgFlag_Read ) )
			{
			itemVisualizer->SetTextBold( EFalse );
			}
		else
			{
			itemVisualizer->SetTextBold( ETrue );
			}

	  	// Set correct skin text colors for the list items
	   	TRgb focusedColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
	   	TRgb normalColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
	    itemVisualizer->SetFocusedStateTextColor( focusedColor );
	    itemVisualizer->SetNormalStateTextColor( normalColor );

		switch ( iListMode )
		    {
		    case EListControlTypeDoubleLinePreviewOn:
		    case EListControlTypeSingleLinePreviewOn:
				itemVisualizer->SetPreviewPaneOn( ETrue );
				itemVisualizer->SetPreviewPaneEnabledSize( TSize(screenRect.Width(), 3*itemHeight) );
		        // fall through
		    case EListControlTypeDoubleLinePreviewOff:
		    case EListControlTypeSingleLinePreviewOff:
	   	 		itemVisualizer->SetExtendable(ETrue);
	 			itemVisualizer->SetExtendedSize(TSize(screenRect.Width(), 2*itemHeight));
	 			break;
		    default:
		        break;
		    }

		// Update initial preview pane text for items if needed
	  	if ( iListMode == EListControlTypeSingleLinePreviewOn ||
			 iListMode == EListControlTypeDoubleLinePreviewOn )
			{
			UpdatePreviewPaneTextForItemL( itemData, confirmedMsgPtr );
			}

		// Set follow up flag icon
		if ( TFsEmailUiUtility::IsFollowUpSupported( *iAppUi.GetActiveMailbox() ) )
			{
			if ( confirmedMsgPtr->IsFlagSet( EFSMsgFlag_FollowUp ) )
				{
			 	itemData->SetFlagIcon( iAppUi.FsTextureManager()->TextureByIndex( EFollowUpFlagList ) );
				itemVisualizer->SetFlagIconVisible( ETrue );
				}
			else if ( confirmedMsgPtr->IsFlagSet( EFSMsgFlag_FollowUpComplete ) )
				{
			 	itemData->SetFlagIcon( iAppUi.FsTextureManager()->TextureByIndex( EFollowUpFlagCompleteList ) );
				itemVisualizer->SetFlagIconVisible( ETrue );
				}
			else
				{
		 	    itemVisualizer->SetFlagIconVisible( EFalse );
				}
			}
	 	else
	 	    {
	 	    itemVisualizer->SetFlagIconVisible( EFalse );
	 	    }

		TFsTreeItemId itemId = iSearchList->InsertItemL( *itemData, *itemVisualizer, KFsTreeRootID );
		if (iSearchList->FocusedItem() == KFsTreeNoneID)
		    {
		    iSearchList->SetFocusedItemL(itemId);
		    }

		SSearchListItem searchListItem;
		searchListItem.iSearchListItemId = itemId;
		searchListItem.iTreeItemData = itemData;
		searchListItem.iTreeItemVisualiser = itemVisualizer;
	    iSearchListItemArray.AppendL( searchListItem );
	    newItem->AddCorrespondingListId( itemId );

		if ( iSearchList->Count() == 1 ) // Call only once, temp fix because of error in generic
			{
			if ( iListMode == EListControlTypeDoubleLinePreviewOn ||
				 iListMode == EListControlTypeDoubleLinePreviewOff )
				{
		        iSearchTreeListVisualizer->SetItemsAlwaysExtendedL( ETrue );
		 		}
			else
				{
				// Set the extendedability and extended size
		   		iSearchTreeListVisualizer->SetItemsAlwaysExtendedL( EFalse );
				}

			// Set msk to "Open when first is found".
			SetMskL();
			}
		}

	}

// ---------------------------------------------------------------------------
// HandleMailBoxEventL
// Function removes objects from the list that are deleted.
// Also updates icon and read status if synced so from the server
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::HandleMailBoxEventL( TFSMailEvent aEvent,
    TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2, TAny* /*aParam3*/ )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        CFSMailBox* activeMailbox = iAppUi.GetActiveMailbox();
        if ( activeMailbox && aMailbox.Id() == activeMailbox->GetId().Id() ) // Safety, in list events that only concern active mailbox are handled
            {
            if ( iModel && iModel->Count() && aEvent == TFSEventMailDeleted )
                {
                RArray<TFSMailMsgId>* removedEntries = static_cast<RArray<TFSMailMsgId>*>(aParam1);
                if ( removedEntries && removedEntries->Count() )
                    {
                    RemoveMsgItemsFromListIfFoundL( *removedEntries );
                    }
                }
            else if ( iModel && iModel->Count() && aEvent == TFSEventMailChanged )
                {
                // Get array of changed entries
                RArray<TFSMailMsgId>* entries = static_cast<RArray<TFSMailMsgId>*>(aParam1);
                // Get ID of the folder that this cahnge concerns.
                TFSMailMsgId* parentFolderId = static_cast<TFSMailMsgId*>( aParam2 );
                for ( TInt i=0 ; i < entries->Count() ; i++)
                    {
                    TFSMailMsgId entryId = (*entries)[i];
                    CFSMailMessage* confirmedMsgPtr(0);
                    TRAPD( err, confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL( iAppUi.GetActiveMailboxId(),
                                            *parentFolderId, entryId, EFSMsgDataEnvelope ) );
                    if ( confirmedMsgPtr && err == KErrNone )
                        {
                        CleanupStack::PushL( confirmedMsgPtr );
                        UpdateMsgIconAndBoldingL( confirmedMsgPtr );
                        CleanupStack::PopAndDestroy( confirmedMsgPtr );
                        }
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// RemoveMsgItemsFromListIfFoundL
// Message removing from list if found. Does not panic or return found status.
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::RemoveMsgItemsFromListIfFoundL( const RArray<TFSMailMsgId>& aEntryIds )
	{
    FUNC_LOG;
	if ( iModel && iModel->Count() )
		{
		for ( TInt i=0 ; i<aEntryIds.Count() ; ++i )
		    {
		    const TFSMailMsgId& entryId = aEntryIds[i];
	    	if ( !entryId.IsNullId() )
	    		{
	        	TInt idx = ItemIndexFromMessageId( entryId );
	        	if ( idx >= 0 )
	        	    {
	    			iSearchList->RemoveL( iSearchListItemArray[idx].iSearchListItemId ); // remove from list
	    			iSearchListItemArray.Remove( idx ); // remove from internal array.
	    	 		iModel->RemoveAndDestroy( idx ); // Remove from model
	        	    }
	    		}
		    }
		}
	}

// ---------------------------------------------------------------------------
// UpdatePreviewPaneTextForItemL
// Updates preview pane text for item
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::UpdatePreviewPaneTextForItemL(CFsTreePlainTwoLineItemData* aItemData, CFSMailMessage* aMsgPtr  )
	{
    FUNC_LOG;
	// Preview pane data update
	if ( aMsgPtr && ( iListMode == EListControlTypeSingleLinePreviewOn ||
		 iListMode == EListControlTypeDoubleLinePreviewOn ) )
		{
		CFSMailMessagePart* textPart = aMsgPtr->PlainTextBodyPartL();
		CleanupStack::PushL( textPart );
		if ( textPart && ( textPart->FetchLoadState() == EFSPartial ||
			 textPart->FetchLoadState() == EFSFull ) )
			{
            TInt previewSize = Min( KMaxPreviewPaneLength, textPart->FetchedContentSize() );
            HBufC* plainTextData16 = HBufC::NewLC( previewSize );
            TPtr textPtr = plainTextData16->Des();

            textPart->GetContentToBufferL( textPtr, 0 ); // Zero is start offset
            // Crop out line feed, paragraph break, and tabulator
            TFsEmailUiUtility::FilterListItemTextL( textPtr );
            aItemData->SetPreviewPaneDataL( *plainTextData16 );

            CleanupStack::PopAndDestroy( plainTextData16 );
			}

		// Else display message size in preview pane
		else
			{
			TUint contentSize = aMsgPtr->ContentSize();
			HBufC* sizeDesc = TFsEmailUiUtility::CreateSizeDescLC( contentSize );
			HBufC* msgSizeText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_PREV_PANE_MSG_SIZE, *sizeDesc );
			aItemData->SetPreviewPaneDataL( *msgSizeText );
			CleanupStack::PopAndDestroy( msgSizeText );
			CleanupStack::PopAndDestroy( sizeDesc );
			}
		CleanupStack::PopAndDestroy( textPart );
		}
	}

void CFSEmailUiSearchListVisualiser::SearchCompletedL()
	{
    FUNC_LOG;
	iSearchOngoing = EFalse;
	HBufC* searchText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FIND_DLG_SEARCH_RESULTS );
	HBufC* finalText = HBufC::NewLC( searchText->Length() + iLatestSearchText->Length() + 4 );
	finalText->Des().Append( *searchText );
	finalText->Des().Append( KSpace );
	finalText->Des().Append( *iLatestSearchText );
	iBarTextVisual->SetTextL( *finalText );
	CleanupStack::PopAndDestroy( finalText );
	CleanupStack::PopAndDestroy( searchText );
	iRequiredSearchPriority = KStandardSearchPriority; //<cmail> return back
	}


void CFSEmailUiSearchListVisualiser::ChangeReadStatusOfHighlightedL( TInt aRead )
	{
    FUNC_LOG;
	if ( iSearchList->Count() )
		{
		CFSEmailUiMailListModelItem* selectedItem =
	 		static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ));

	 	TBool wasRead = selectedItem->MessagePtr().IsFlagSet( EFSMsgFlag_Read );
	 	if ( (wasRead && !aRead) || (!wasRead && aRead) )
	 	    {
	 	    // Read status is changed
	 	    iMsgDataCouldBeChanged = ETrue;

    		if ( aRead )
     			{
    			// Send flags, local and server
     			selectedItem->MessagePtr().SetFlag( EFSMsgFlag_Read );
    			}
    		else
    			{
    			// Send flags, local and server
     			selectedItem->MessagePtr().ResetFlag( EFSMsgFlag_Read );
    			}
    		selectedItem->MessagePtr().SaveMessageL();	// Save flag
    		// Switch icon to correct one
    		UpdateMsgIconAndBoldingL( HighlightedIndex() );
	 	    }
		}
	}

void CFSEmailUiSearchListVisualiser::CheckAndUpdateFocusedMessageL()
	{
    FUNC_LOG;
	if ( iModel && iModel->Count() ) // Needed safety check
		{
		TInt highlightedIndex = HighlightedIndex();
		CFSEmailUiMailListModelItem* selectedItem =
	 		static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( highlightedIndex ));
		if ( selectedItem->ModelItemType() == ETypeMailItem )
			{
			CFSMailMessage* msgPtr = &selectedItem->MessagePtr();
			if ( msgPtr )
				{
				CFSMailFolder* folderPtr =
				    iAppUi.GetMailClient()->GetFolderByUidL( iAppUi.GetActiveMailboxId(), msgPtr->GetFolderId() );
				CleanupStack::PushL( folderPtr );

	    		if ( !folderPtr )
	    			{
	    			// Pointer is not valid anymore, msg has been delete,
	    			// so it must be removed from the search list
	    			RemoveFocusedFromListL();
	    			}
	    		else
	    			{
	    			CFSMailMessage* confirmedMsgPtr(0);
	    			TRAPD( err, confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL( iAppUi.GetActiveMailboxId(),
	    										folderPtr->GetFolderId(), msgPtr->GetMessageId(), EFSMsgDataEnvelope ) );
	    			if ( confirmedMsgPtr && err == KErrNone)
	    				{
	    				CleanupStack::PushL( confirmedMsgPtr );
	    				UpdateMsgIconAndBoldingL( confirmedMsgPtr );
	    				CleanupStack::PopAndDestroy( confirmedMsgPtr );
	    				}
	    			else
	    				{
	    				RemoveFocusedFromListL();
	    				}
	    			}

	    		CleanupStack::PopAndDestroy( folderPtr );
				}
			}
		}
	}

void CFSEmailUiSearchListVisualiser::RemoveFocusedFromListL()
	{
    FUNC_LOG;
	iMsgDataCouldBeChanged = ETrue;
	SSearchListItem item;
	item.iSearchListItemId = iSearchList->FocusedItem();
	TInt IndexToBeDestroyed = iSearchListItemArray.Find( item );
	iSearchListItemArray.Remove( IndexToBeDestroyed ); // remove from internal array.
	iModel->RemoveAndDestroy( IndexToBeDestroyed ); // Remove from model
	iSearchList->RemoveL( iSearchList->FocusedItem() );		// remove from list
	}


// Updates our own message object with the data from a given message object.
// The messages are matched with the message ID.
void CFSEmailUiSearchListVisualiser::UpdateMsgIconAndBoldingL( CFSMailMessage* aMsgPtr )
	{
    FUNC_LOG;
	if ( aMsgPtr )
		{
		for ( TInt i=0 ; i < iModel->Count() ; i++)
			{
			CFSEmailUiMailListModelItem* item =
				static_cast<CFSEmailUiMailListModelItem*>( Model()->Item(i) );
			if ( item && item->ModelItemType()==ETypeMailItem &&
				 item->MessagePtr().GetMessageId() == aMsgPtr->GetMessageId() )
				{
				// Update all flags
				TUint32 prevFlags = item->MessagePtr().GetFlags();
				TUint32 newFlags = aMsgPtr->GetFlags();
				if ( prevFlags != newFlags )
				    {
    				item->MessagePtr().ResetFlag( prevFlags );
    				item->MessagePtr().SetFlag( newFlags );

    				// Save changed flags in internal model array
    				item->MessagePtr().SaveMessageL();
    				iMsgDataCouldBeChanged = ETrue;
				    }

				// Update the list item graphics
				UpdateMsgIconAndBoldingL( i );
				break;
				}
			}
		}
	}

// Updates list item at given index to match the state of the message object
void CFSEmailUiSearchListVisualiser::UpdateMsgIconAndBoldingL( TInt aListIndex )
    {
    FUNC_LOG;
    if (aListIndex < iSearchListItemArray.Count())
    	{
	    CFSEmailUiMailListModelItem* item =
		    static_cast<CFSEmailUiMailListModelItem*>( Model()->Item(aListIndex) );
	    if ( item && item->ModelItemType()==ETypeMailItem )
		    {
		    CFSMailMessage* msgPtr = &item->MessagePtr();
		    CAlfTexture* itemTexture = &TFsEmailUiUtility::GetMsgIcon( msgPtr, *iAppUi.FsTextureManager() );
		    CFsTreePlainTwoLineItemData* itemData =
                static_cast<CFsTreePlainTwoLineItemData*>( iSearchListItemArray[aListIndex].iTreeItemData );
		    CFsTreePlainTwoLineItemVisualizer* itemVis =
                static_cast<CFsTreePlainTwoLineItemVisualizer*>( iSearchListItemArray[aListIndex].iTreeItemVisualiser );
		    itemData->SetIcon( *itemTexture );
		    if ( msgPtr->IsFlagSet( EFSMsgFlag_Read ) )
			    {
			    itemVis->SetTextBold( EFalse );
			    }
		    else
			   {
			   itemVis->SetTextBold( ETrue );
			   }
		    // Set follow up flag icon correctly
		    if ( TFsEmailUiUtility::IsFollowUpSupported( *iAppUi.GetActiveMailbox() ) )
			    {
			    if ( msgPtr->IsFlagSet( EFSMsgFlag_FollowUp ) )
				    {
			 	    itemData->SetFlagIcon( iAppUi.FsTextureManager()->TextureByIndex( EFollowUpFlagList ) );
				    itemVis->SetFlagIconVisible( ETrue );
				    }
			    else if ( msgPtr->IsFlagSet( EFSMsgFlag_FollowUpComplete ) )
				    {
			 	    itemData->SetFlagIcon( iAppUi.FsTextureManager()->TextureByIndex( EFollowUpFlagCompleteList ) );
				    itemVis->SetFlagIconVisible( ETrue );
				    }
			    else
				    {
		 	        itemVis->SetFlagIconVisible( EFalse );
				    }
			    }
		    else
		        {
		        itemVis->SetFlagIconVisible( EFalse );
		        }
		    iSearchTreeListVisualizer->UpdateItemL( iSearchListItemArray[aListIndex].iSearchListItemId );
		    }
    	}
    }


// Delete messages
void CFSEmailUiSearchListVisualiser::DeleteFocusedMessageL()
	{
    FUNC_LOG;
	if ( iSearchList->Count() )
		{
		TInt currentItemIndex = HighlightedIndex();
		CFSEmailUiMailListModelItem* item =
			static_cast<CFSEmailUiMailListModelItem*>( Model()->Item(currentItemIndex));
		if ( item && item->ModelItemType() == ETypeMailItem )
			{
			CFSMailMessage& messagePtr = item->MessagePtr();
			TInt queryTextId(0);
            if ( messagePtr.IsFlagSet( EFSMsgFlag_CalendarMsg ))
                {
                queryTextId = R_FREESTYLE_EMAIL_DELETE_CALEVENT_NOTE;
                }
            else
                {
                queryTextId = R_FREESTYLE_EMAIL_DELETE_MAIL_NOTE;
                }
            HBufC* msgSubject = TFsEmailUiUtility::CreateSubjectTextLC( &messagePtr );

            TInt okToDelete( ETrue );
            if ( iAppUi.GetCRHandler()->WarnBeforeDelete() )
               {
               okToDelete = TFsEmailUiUtility::ShowConfirmationQueryL( queryTextId, *msgSubject );
               }
            CleanupStack::PopAndDestroy( msgSubject );

			if ( okToDelete )
 				{
 				iMsgDataCouldBeChanged = ETrue; // Refresh
 				// Delete message from framework, and perform internal housekeeping
 				TFSMailMsgId msgId = messagePtr.GetMessageId();
 				RArray<TFSMailMsgId> msgIds;
 				msgIds.Append( msgId );
				TFSMailMsgId folderId = messagePtr.GetFolderId();
				TFSMailMsgId mailBox = iAppUi.GetActiveMailbox()->GetId();
				iAppUi.GetMailClient()->DeleteMessagesByUidL( mailBox, folderId, msgIds );
	 			msgIds.Reset();
				SSearchListItem item;
				item.iSearchListItemId = iSearchList->FocusedItem();
				TInt IndexToBeDestroyed = iSearchListItemArray.Find( item );
				iSearchListItemArray.Remove( IndexToBeDestroyed ); // remove from internal array.
		 		iModel->RemoveAndDestroy( IndexToBeDestroyed ); // Remove from model
				iSearchList->RemoveL( iSearchList->FocusedItem() );		// remove from list
				}
			}
		}
	}


// Navigation functions, used mainly from viewer
TBool CFSEmailUiSearchListVisualiser::IsNextMsgAvailable( TFSMailMsgId aCurrentMsgId,
														  TFSMailMsgId& aFoundNextMsgId,
														  TFSMailMsgId& aFoundNextMsgFolderId ) const
	{
    FUNC_LOG;
	TBool ret(EFalse);

	TInt curIdx = ItemIndexFromMessageId( aCurrentMsgId );
	if ( curIdx >= 0 )
	    {
	    TInt nextIdx = NextMessageIndex(curIdx);
	    if ( nextIdx >= 0 )
	        {
	        ret = ETrue;
	        aFoundNextMsgId = MsgIdFromIndex(nextIdx);
	        CFSEmailUiMailListModelItem* item =
    			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item( nextIdx ));
    		if ( item->ModelItemType() == ETypeMailItem )
    	  	  {
    	 	   aFoundNextMsgFolderId = item->MessagePtr().GetFolderId();
    	 	   }
	        }
	    }

	return ret;
	}


TBool CFSEmailUiSearchListVisualiser::IsPreviousMsgAvailable( TFSMailMsgId aCurrentMsgId,
														  	  TFSMailMsgId& aFoundPreviousMsgId,
														  	  TFSMailMsgId& aFoundPrevMsgFolderId ) const
	{
    FUNC_LOG;
	TBool ret(EFalse);

	TInt curIdx = ItemIndexFromMessageId( aCurrentMsgId );
	if ( curIdx >= 0 )
	    {
	    TInt prevIdx = PreviousMessageIndex(curIdx);
	    if ( prevIdx >= 0 )
	        {
	        ret = ETrue;
	        aFoundPreviousMsgId = MsgIdFromIndex(prevIdx);
	     	CFSEmailUiMailListModelItem* item =
    			static_cast<CFSEmailUiMailListModelItem*>(iModel->Item( prevIdx ));
    		if ( item->ModelItemType() == ETypeMailItem )
    	  	  {
    	 	   aFoundPrevMsgFolderId = item->MessagePtr().GetFolderId();
    	 	   }
	        }
	    }

    return ret;
	}

TInt CFSEmailUiSearchListVisualiser::MoveToNextMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundNextMsgId )
	{
    FUNC_LOG;
	TInt ret(KErrNotFound);

	TInt curIdx = ItemIndexFromMessageId( aCurrentMsgId );
	TInt nextIdx = NextMessageIndex( curIdx );

	if ( curIdx >= 0 && nextIdx >= 0 )
	    {
	    // Focus the new message
	    iSearchTreeListVisualizer->SetFocusedItemL( iSearchListItemArray[nextIdx].iSearchListItemId );
	    ChangeReadStatusOfHighlightedL( ETrue );
        aFoundNextMsgId = MsgIdFromIndex( nextIdx );
	    ret = KErrNone;
	    }
	if ( ret == KErrNone )
		{
		OpenHighlightedMailL();
		}
	return ret;
	}

TInt CFSEmailUiSearchListVisualiser::MoveToPreviousMsgL( TFSMailMsgId aCurrentMsgId, TFSMailMsgId& aFoundPreviousMsgId )
	{
    FUNC_LOG;
	TInt ret(KErrNotFound);

	TInt curIdx = ItemIndexFromMessageId( aCurrentMsgId );
	TInt prevIdx = PreviousMessageIndex( curIdx );

	if ( curIdx >= 0 && prevIdx >= 0 )
	    {
	    // Focus the new message
	    iSearchTreeListVisualizer->SetFocusedItemL( iSearchListItemArray[prevIdx].iSearchListItemId );
	    ChangeReadStatusOfHighlightedL( ETrue );
        aFoundPreviousMsgId = MsgIdFromIndex( prevIdx );
	    ret = KErrNone;
	    }

	if ( ret == KErrNone )
		{
		OpenHighlightedMailL();
		}
	return ret;
	}

TInt CFSEmailUiSearchListVisualiser::MoveToPreviousMsgAfterDeleteL( TFSMailMsgId aFoundPreviousMsgId )
	{
	FUNC_LOG;
	TInt ret(KErrNotFound);

	TInt idx = ItemIndexFromMessageId( aFoundPreviousMsgId );
	if ( idx >= 0 )
		{
		// Focus the previous message
		iSearchTreeListVisualizer->SetFocusedItemL( iSearchListItemArray[idx].iSearchListItemId );
		ChangeReadStatusOfHighlightedL( ETrue );
		ret = KErrNone;
		}

	if ( ret == KErrNone )
		{
		OpenHighlightedMailL();
		}

	return ret;
	}

	// Helper functions to get highlighted message id and folder id
TFSMailMsgId CFSEmailUiSearchListVisualiser::HighlightedMessageFolderId()
	{
    FUNC_LOG;
	TFSMailMsgId ret;
 	if ( iSearchList->Count() && iModel )
		{
	  	CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
		if ( item && item->ModelItemType() == ETypeMailItem )
			{
			ret = item->MessagePtr().GetFolderId();
			}
		}
	return ret;
	}

TFSMailMsgId CFSEmailUiSearchListVisualiser::HighlightedMessageId()
	{
    FUNC_LOG;
	TFSMailMsgId ret;
	if ( iSearchList->Count() && iModel )
		{
	  	CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
		if ( item && item->ModelItemType() == ETypeMailItem )
			{
			ret = item->MessagePtr().GetMessageId();
			}
		}
	return ret;
	}


//<cmail> Touch
// ---------------------------------------------------------------------------
// CFSEmailUiSearchListVisualiser::TreeListEventL
// From MFsTreeListObserver
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::TreeListEventL( const TFsTreeListEvent aEvent,
                                                     const TFsTreeItemId /*aId*/,
                                                     const TPoint& aPoint )
    {
    FUNC_LOG;

    switch(aEvent)
        {
        case MFsTreeListObserver::EFsTreeListItemTouchAction:
            DoHandleActionL();
            break;
        case MFsTreeListObserver::EFsTreeListItemTouchLongTap:
            if ( iSearchList->Count() )
                {
                LaunchStylusPopupMenuL( aPoint );
                }
            break;
        case MFsTreeListObserver::EFsTreeListItemWillGetFocused:
            {
            SetMskL();
            break;
            }
        case MFsTreeListObserver::EFsFocusVisibilityChange:
        	{
        	iAppUi.SetFocusVisibility( EFalse );
        	break;
        	}
        case MFsTreeListObserver::EFsTreeListItemTouchFocused:
        default:
            //Just ignore rest of events
            break;
        }
    }

// ---------------------------------------------------------------------------
// CFSEmailUiSearchListVisualiser::DoHandleActionL
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::DoHandleActionL()
    {
    FUNC_LOG;
    TInt modelCount = 0;

    if ( iModel )
        {
        modelCount = iModel->Count();
        }
    if ( modelCount ) // Safety check
        {
        CFSEmailUiMailListModelItem* item = dynamic_cast<CFSEmailUiMailListModelItem*>(iModel->Item(HighlightedIndex()));
        if ( item && item->ModelItemType() == ETypeMailItem )
            {
            CFSMailMessage* messagePtr = &item->MessagePtr();
            if ( messagePtr )
                {
                OpenHighlightedMailL();
                }
            }
        }
    }

//</cmail>

// ---------------------------------------------------------------------------
// From MFSEmailUiContactHandlerObserver
// The ownership of the CLS items in the contacts array is transferred to the
// observer, and they must be deleted by the observer.
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::OperationCompleteL(
    TContactHandlerCmd /*aCmd*/, const RPointerArray<CFSEmailUiClsItem>& /*aContacts*/ )
    {
    FUNC_LOG;
    iRequiredSearchPriority = KStandardSearchPriority; //<cmail> return back
    }

// ---------------------------------------------------------------------------
// From MFSEmailUiContactHandlerObserver
// Handles error in contact handler operation.
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::OperationErrorL(
    TContactHandlerCmd /*aCmd*/, TInt /*aError*/ )
    {
    FUNC_LOG;
    iRequiredSearchPriority = KStandardSearchPriority; //<cmail> return back
    }

TBool CFSEmailUiSearchListVisualiser::CallToSenderL()
	{
    FUNC_LOG;
	CFSEmailUiMailListModelItem* item =
		dynamic_cast<CFSEmailUiMailListModelItem*>
			( iModel->Item( HighlightedIndex() ) );
	if ( item && item->ModelItemType() == ETypeMailItem )
		{
		CFSMailAddress* fromAddress = item->MessagePtr().GetSender();
		TDesC* emailAddress(0);
		if ( fromAddress )
			{
			emailAddress = &fromAddress->GetEmailAddress();
			}
		if ( emailAddress && emailAddress->Length() )
			{
			CFsDelayedLoader::InstanceL()->GetContactHandlerL()->
				FindAndCallToContactByEmailL( *emailAddress,
					iAppUi.GetActiveMailbox(), this, ETrue );
                   iRequiredSearchPriority = KCallingSearchPriority; //decrease priority to enable search for contact
                   return ETrue; // searching started - consume keyup event
			}
		}
  return EFalse; //no calling key up will execute Call app
	}


// ---------------------------------------------------------------------------
// Logic for changing msk
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::SetMskL()
	{
    FUNC_LOG;
	if ( iFirstStartCompleted )
	    {
	    if ( iSearchList && iSearchList->Count() )
	        {
	        ChangeMskCommandL( R_FSE_QTN_MSK_OPEN );
	        }
	    else
	        {
	        ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
	        }
	    }
	}

//<cmail>
// ---------------------------------------------------------------------------
// CFSEmailUiSearchListVisualiser::SetHeaderTextAttributesL()
//
// Apply header text/background attributes
// This creates i.e "Search result: %S " title caption and background graphics
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::SetHeaderAttributesL()
	{
	// Set bar text color/style from skin
	iBarTextVisual->SetTextStyle( iAppUi.LayoutHandler()->
	        FSTextStyleFromLayoutL( AknLayoutScalable_Apps::main_sp_fs_ctrlbar_pane_t1( 0 ) ).Id() );
	iBarTextVisual->SetColor( iAppUi.LayoutHandler()->
							ListNormalStateTextSkinColor() );

    if( iBarBgVisual->Brushes() )
        {
        iBarBgVisual->Brushes()->Reset();
        }
    iBarBgVisual->EnableBrushesL();
    CAlfBrush* selectorBrush = iAppUi.FsTextureManager()->
                   NewCtrlBarSelectorBrushLC();
    selectorBrush->SetOpacity( KFSHeaderTextBackgroundOpacity );
    iBarBgVisual->Brushes()->AppendL( selectorBrush, EAlfHasOwnership );
    CleanupStack::Pop( selectorBrush );
	}
//</cmail>



// ---------------------------------------------------------------------------
// LaunchStylusPopupMenuL
// Function launches avkon stylus popup menu based for the tapped item
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::LaunchStylusPopupMenuL( const TPoint& aPoint )
    {
    // Verify that the item's type is correct
    CFSEmailUiMailListModelItem* item =
        static_cast<CFSEmailUiMailListModelItem*>( iModel->Item( HighlightedIndex() ) );
    if ( item && ( item->ModelItemType() == ETypeMailItem ) )
        {
        // Add mark as read / unread options
        const CFSMailMessage& message = item->MessagePtr();

        TBool messageRead( message.IsFlagSet( EFSMsgFlag_Read ) );
        iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdMarkAsUnread, !messageRead );
        iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdMarkAsRead, messageRead );

        // Set the position for the popup
        iStylusPopUpMenu->SetPosition( aPoint );

        // Display the popup and set the flag to indicate that the menu was
        // launched so that list focus stays visible.
        iStylusPopUpMenu->ShowMenu();
        iStylusPopUpMenuVisible = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// ClientRequiredSearchPriority
// the email searching priority may be decreased to enable searching for contacts
// ---------------------------------------------------------------------------
//
void CFSEmailUiSearchListVisualiser::ClientRequiredSearchPriority( TInt *apRequiredSearchPriority )
    {
    FUNC_LOG;
    if (iRequiredSearchPriority != (*apRequiredSearchPriority))
    *apRequiredSearchPriority = iRequiredSearchPriority; 
    return;
    }

// End of file
