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
* Description:  Implements visualiser for the download manager view
*
*/



// SYSTEM INCLUDES
#include "emailtrace.h"
#include <FreestyleEmailUi.mbg>
#include <aknutils.h>
#include <gulicon.h>
#include <StringLoader.h>
#include <FreestyleEmailUi.rsg>
// <cmail> SF
#include <alf/alfframebrush.h>
#include "fscontrolbar.h"
#include "fscontrolbuttoninterface.h"
#include "fstreelist.h"
#include "fstreevisualizerbase.h"
#include "fstreeplainonelineitemdata.h"
#include "fstreeplainonelineitemvisualizer.h"
#include "fstreeplainonelinenodedata.h"
#include "fstreeplainonelinenodevisualizer.h"
#include "fstreeplaintwolineitemdata.h"
#include "fstreeplaintwolineitemvisualizer.h"
// </cmail>
#include <AknListQueryDialog.h>
#include <caknfileselectiondialog.h>
#include <caknmemoryselectiondialog.h>
#include <pathinfo.h>
#include <aknnotewrappers.h>
// <cmail> SF
#include <alf/alfdecklayout.h>
#include <alf/alfevent.h>
#include <alf/alfcontrolgroup.h>
#include <csxhelp/cmail.hlp.hrh>
#include "mfsmailbrandmanager.h"
#include "cfsmailclient.h"
//</cmail>
#include <FeatMgr.h>
//</cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiAttachmentsListVisualiser.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiDownloadManagerModel.h"
#include "FreestyleEmailUiDownloadManagerVisualiser.h"
#include "FreestyleEmailUiDownloadManagerControl.h"
#include "FreestyleEmailUiMsgDetailsVisualiser.h"
#include "FreestyleEmailUiFileSystemInfo.h"
#include "FreestyleEmailUiScrollbarClet.h"
#include "FreestyleEmailUiAppUi.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiLiterals.h"
#include "FreestyleEmailUiShortcutBinding.h"


CFSEmailUiDownloadManagerVisualiser* CFSEmailUiDownloadManagerVisualiser::NewL(
    CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup )
    {
    FUNC_LOG;
    CFSEmailUiDownloadManagerVisualiser* self = 
        CFSEmailUiDownloadManagerVisualiser::NewLC( aEnv, aAppUi, aControlGroup );
    CleanupStack::Pop( self );
    return self;
    }

CFSEmailUiDownloadManagerVisualiser* CFSEmailUiDownloadManagerVisualiser::NewLC(
    CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup )
    {
    FUNC_LOG;
    CFSEmailUiDownloadManagerVisualiser* self = 
        new (ELeave) CFSEmailUiDownloadManagerVisualiser( aEnv, aAppUi, aControlGroup );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

void CFSEmailUiDownloadManagerVisualiser::ConstructL()
	{
    FUNC_LOG;

	BaseConstructL( R_FSEMAILUI_DOWNLOADMANAGER_VIEW );

	// Create list screen (list) control
	iScreenControl = CFreestyleEmailUiDownloadManagerControl::NewL( iEnv, this );
	
	// Create attachments list layout and set it to fill the main pane
  	TRect screenRect;
 	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, screenRect );	
    iListLayout = CAlfDeckLayout::AddNewL( *iScreenControl );  
    screenRect.Move(0,0);
    iListLayout->SetRect( screenRect );
  
   	// Create attachments list tree list component 
    iDownloadManagerTreeListVisualizer = CFsTreeVisualizerBase::NewL(iScreenControl, *iListLayout);
   	iDownloadManagerTreeListVisualizer->SetItemExpansionDelay( iAppUi.LayoutHandler()->ListItemExpansionDelay() );
   	
    iDownloadManagerList = CFsTreeList::NewL(*iDownloadManagerTreeListVisualizer, iEnv);   
    iDownloadManagerList->SetScrollbarVisibilityL( EFsScrollbarAuto );
  	iDownloadManagerList->SetMarkTypeL( CFsTreeList::EFsTreeListMultiMarkable );
  	iDownloadManagerList->SetIndentationL( 0 );

    //<cmail> Compared to S60 3.2.3 in S60 5.0 Alf offers the key events in
    // opposite order.
    ControlGroup().AppendL( iScreenControl );
    ControlGroup().AppendL( iDownloadManagerList->TreeControl() );
    //</cmail>
    
    iModel = CFSEmailUiDownloadManagerModel::NewL( iAppUi, *this );
    //<cmail> S60 skin support
    //iDownloadManagerTreeListVisualizer->SetBackgroundTextureL( iAppUi.FsTextureManager()->TextureByIndex( EBackgroundTextureMailList ) );  
	//</cmail>
    iDownloadManagerTreeListVisualizer->SetMenuIcon( iAppUi.FsTextureManager()->TextureByIndex( EListControlMenuIcon ) );
    iDownloadManagerTreeListVisualizer->SetItemsAlwaysExtendedL( ETrue );
    iDownloadManagerList->AddObserverL( *this );

    iAppUi.LayoutHandler()->SetListMarqueeBehaviour( iDownloadManagerList );

    // Set page up and page down keys
    iDownloadManagerTreeListVisualizer->AddCustomPageUpKey( EStdKeyPageUp );
    iDownloadManagerTreeListVisualizer->AddCustomPageDownKey( EStdKeyPageDown );
 
   	CAlfBrush* selectorBrush = iAppUi.FsTextureManager()->ListSelectorBrushL();
	iDownloadManagerTreeListVisualizer->SetSelectorPropertiesL( selectorBrush, 1.0, CFsTreeVisualizerBase::EFsSelectorMoveSmoothly );
  
 	}


CFSEmailUiDownloadManagerVisualiser::CFSEmailUiDownloadManagerVisualiser(
    CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup )
    : CFsEmailUiViewBase( aControlGroup, *aAppUi ), iEnv( aEnv )
	{
    FUNC_LOG;
	}

CFSEmailUiDownloadManagerVisualiser::~CFSEmailUiDownloadManagerVisualiser()
	{
    FUNC_LOG;
	delete iModel;
	if ( iDownloadManagerList )
		{
		iDownloadManagerList->RemoveObserver( *this );
		}
	delete iDownloadManagerList;
	}


TInt CFSEmailUiDownloadManagerVisualiser::HighlightedIndex()
	{
    FUNC_LOG;
	return 0;
	}

TUid CFSEmailUiDownloadManagerVisualiser::Id() const
	{
    FUNC_LOG;
	return DownloadManagerViewId;
	}			   

// ---------------------------------------------------------------------------
// CFSEmailUiDownloadManagerVisualiser::ChildDoActivateL
// ---------------------------------------------------------------------------
//
// <cmail> Toolbar
/*void CFSEmailUiDownloadManagerVisualiser::DoActivateL( 
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage )*/
void CFSEmailUiDownloadManagerVisualiser::ChildDoActivateL( 
        const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/,
        const TDesC8& /*aCustomMessage*/ )
// <cmail> Toolbar
	{
    FUNC_LOG;
	RefreshL();
	iDownloadManagerList->ShowListL();
	
	SetMailboxNameToStatusPaneL();
	}

void CFSEmailUiDownloadManagerVisualiser::SetMailboxNameToStatusPaneL()
	{
    FUNC_LOG;
	iAppUi.SetActiveMailboxNameToStatusPaneL();
	}

// ---------------------------------------------------------------------------
// CFSEmailUiDownloadManagerVisualiser::ChildDoDeactivate
// ---------------------------------------------------------------------------
//
void CFSEmailUiDownloadManagerVisualiser::ChildDoDeactivate()
	{
    FUNC_LOG;
    if ( !iAppUi.AppUiExitOngoing() )
        {
        iDownloadManagerTreeListVisualizer->NotifyControlVisibilityChange( EFalse );
        }
    }	
	
CFSEmailUiDownloadManagerModel* CFSEmailUiDownloadManagerVisualiser::Model()
	{
    FUNC_LOG;
	return iModel;	
	}

void CFSEmailUiDownloadManagerVisualiser::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    FUNC_LOG;
    TAttachmentData* selectedItem = iModel->GetItem( iDownloadManagerList->FocusedItem() );      

    if ( aResourceId == R_FSEMAILUI_ATTACHMENTS_SUBMENU_ACTIONS )
        {
        aMenuPane->SetItemDimmed( EFsEmailUiCmdOpen, 
                !IsCommandAvailableOnItem(EFsEmailUiCmdOpen, selectedItem) );

        aMenuPane->SetItemDimmed( EFsEmailUiCmdDownload, 
                !IsCommandAvailableOnItem(EFsEmailUiCmdDownload, selectedItem) );

        aMenuPane->SetItemDimmed( EFsEmailUiCmdCancelDownload, 
                !IsCommandAvailableOnItem(EFsEmailUiCmdCancelDownload, selectedItem) );

        aMenuPane->SetItemDimmed( EFsEmailUiCmdSave, 
                !IsCommandAvailableOnItem(EFsEmailUiCmdSave, selectedItem) );

        aMenuPane->SetItemDimmed( EFsEmailUiCmdDownloadAll, 
                !IsCommandAvailableOnItem(EFsEmailUiCmdDownloadAll, selectedItem) );

        aMenuPane->SetItemDimmed( EFsEmailUiCmdCancelAllDownloads, 
                !IsCommandAvailableOnItem(EFsEmailUiCmdCancelAllDownloads, selectedItem) );

        aMenuPane->SetItemDimmed( EFsEmailUiCmdSaveAll, 
                !IsCommandAvailableOnItem(EFsEmailUiCmdSaveAll, selectedItem) );

        aMenuPane->SetItemDimmed( EFsEmailUiCmdClearFetchedAttachment,
                !IsCommandAvailableOnItem(EFsEmailUiCmdClearFetchedAttachment, selectedItem) );
        
        aMenuPane->SetItemDimmed( EFsEmailUiCmdRemoveAttachment,
                !IsCommandAvailableOnItem(EFsEmailUiCmdRemoveAttachment, selectedItem) );
        }
    else if ( aResourceId == R_FSEMAILUI_DOWNLOADMANAGER_MENUPANE )
        {
        if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
    	   {
    	   // remove help support in pf5250
    	   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);      
    	   }        
        
        aMenuPane->SetItemDimmed( EFsEmailUiCmdMailActions,
                !IsCommandAvailableOnItem(EFsEmailUiCmdMailActions, selectedItem) );

        // Hide message related commands if no message in focus.
        // This may happen when all attachments have been removed from open DM view.
        TPartData messageData;
        TRAPD( err, messageData = iModel->GetMessageL( iDownloadManagerList->FocusedItem() ) );
        if ( err )
            {
            aMenuPane->SetItemDimmed( EFSEmailUICmdViewMessage, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMsgDetails, ETrue );   
            }
        }

    // Add shortcut hints
    iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane, 
            CFSEmailUiShortcutBinding::EContextDownloadManager );	
    }
	
	
// ---------------------------------------------------------------------------
// HandleMailBoxEventL
// Mailbox event handler, responds to events sent by the plugin.
// ---------------------------------------------------------------------------
//
void CFSEmailUiDownloadManagerVisualiser::HandleMailBoxEventL( TFSMailEvent aEvent,
    TFSMailMsgId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    FUNC_LOG;
	// Safety, refresh only if this view is active, has model and mail deleted event has been received.  
    if ( aEvent == TFSEventMailDeleted && iModel ) 
        {
		// Remove deleted items from list. Refresh list if this view is active
        // and some item was removed.
        if ( iModel->ClearInvalidItemsL() && 
             iAppUi.CurrentActiveView() == this )
            {
            RefreshL(); 
            SetMskL();
            }
        }
    }	
		
		
void CFSEmailUiDownloadManagerVisualiser::RefreshL()
	{
    FUNC_LOG;
	TRect screenRect;
	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );
	iListLayout->SetRect( screenRect );

	iDownloadManagerList->RemoveAllL(); 
	
	CFsTreePlainTwoLineItemData* twoLineItemData = CFsTreePlainTwoLineItemData::NewL();
	CleanupStack::PushL( twoLineItemData );
    CFsTreePlainTwoLineItemVisualizer* twoLineItemVisualizer = 
        CFsTreePlainTwoLineItemVisualizer::NewL( *iDownloadManagerList->TreeControl() );
    CleanupStack::PushL( twoLineItemVisualizer );
 	// Set font height
	twoLineItemVisualizer->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );		
	// Set font bolding
	twoLineItemVisualizer->SetTextBold( ETrue );
    // Set manual layout and make item non-focusable
	twoLineItemVisualizer->SetFlags( twoLineItemVisualizer->Flags() & ~KFsTreeListItemFocusable );

  	// Set correct skin text colors for the list items  
   	TRgb focusedColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
   	TRgb normalItemColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
   	TRgb normalNodeColor = iAppUi.LayoutHandler()->ListNodeTextColor();
    TRgb headerBgColor = iAppUi.LayoutHandler()->ListHeaderBackgroundColor();

   	twoLineItemVisualizer->SetFocusedStateTextColor( focusedColor );
   	twoLineItemVisualizer->SetNormalStateTextColor( normalItemColor );
   	twoLineItemVisualizer->SetBackgroundColor( headerBgColor );
	
	const RArray<TMessageData>& modelData = iModel->GetModel();
	HBufC* resourceString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_HEADER_DOWNLOAD_MANAGER );
	twoLineItemData->SetDataL( *resourceString );	
	CleanupStack::PopAndDestroy( resourceString );

	resourceString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_HEADER_TOTAL, iModel->AttachmentCount() );
	twoLineItemData->SetSecondaryDataL( *resourceString );
	CleanupStack::PopAndDestroy( resourceString );
	twoLineItemData->SetIcon ( iAppUi.FsTextureManager()->TextureByIndex( EAttachmentIcon ) );
	CleanupStack::Pop( twoLineItemVisualizer );
	CleanupStack::Pop( twoLineItemData );
    iHeaderItemId = iDownloadManagerList->InsertItemL( *twoLineItemData, *twoLineItemVisualizer, KFsTreeRootID );

	TFsTreeItemId parentNode;
	for ( TInt i=0 ; i<modelData.Count() ; i++ )
		{
		CFsTreePlainOneLineNodeData* separatorData = CFsTreePlainOneLineNodeData::NewL();
		CleanupStack::PushL( separatorData );
	    CFsTreePlainOneLineNodeVisualizer* separatorVisualiser = 
            CFsTreePlainOneLineNodeVisualizer::NewL( *iDownloadManagerList->TreeControl() );
	    CleanupStack::PushL( separatorVisualiser );
	    separatorData->SetDataL( *modelData[i].mailSubject );
 		// Set font height
		separatorVisualiser->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );		
		// Set font bolding
		separatorVisualiser->SetTextBold( ETrue );
	  	// Set correct skin text colors for the list items  
		separatorVisualiser->SetFocusedStateTextColor( focusedColor );
		separatorVisualiser->SetNormalStateTextColor( normalNodeColor );
		separatorVisualiser->SetBackgroundColor( iAppUi.LayoutHandler()->ListNodeBackgroundColor() );
		
	    separatorVisualiser->SetExtendable( EFalse );
		separatorData->SetIconExpanded( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeExpanded ) );
		separatorData->SetIconCollapsed( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeCollapsed ) );
		CleanupStack::Pop( separatorVisualiser );
		CleanupStack::Pop( separatorData );
	    parentNode = iDownloadManagerList->InsertNodeL(*separatorData, *separatorVisualiser, KFsTreeRootID);  
		iModel->SetNodeIdL( i, parentNode );
		for (TInt j=0; j<modelData[i].mailAttachments.Count(); j++)
			{
		    twoLineItemData = CFsTreePlainTwoLineItemData::NewL();
			CleanupStack::PushL( twoLineItemData );
		    twoLineItemData->SetDataL( modelData[i].mailAttachments[j].fileName );
	    
            HBufC* secondLine = CreateSecondLineTextLC( modelData[i].mailAttachments[j] );
    		twoLineItemData->SetSecondaryDataL( *secondLine );
    		CleanupStack::PopAndDestroy( secondLine );		
					
    		CAlfTexture& itemTexture = TFsEmailUiUtility::GetAttachmentIcon( modelData[i].mailAttachments[j].fileType, 
    		                                                                 *iAppUi.FsTextureManager() );
       		twoLineItemData->SetIcon( itemTexture );


	 	    twoLineItemVisualizer = CFsTreePlainTwoLineItemVisualizer::NewL( *iDownloadManagerList->TreeControl());
		    CleanupStack::PushL( twoLineItemVisualizer );
	 		twoLineItemVisualizer->SetExtendable(ETrue);
			twoLineItemVisualizer->SetFlags( twoLineItemVisualizer->Flags() | KFsTreeListItemHasMenu );
			
			CAlfTextStyle* textStyle = iAppUi.LayoutHandler()->FSTextStyleFromIdL( EFSFontTypeSmall );
			twoLineItemVisualizer->SetTextStyleId( textStyle->Id() );

	 		// Set font height
			twoLineItemVisualizer->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );		
			// Set font bolding
			twoLineItemVisualizer->SetTextBold( EFalse );
		  	// Set correct skin text colors for the list items  
		   	twoLineItemVisualizer->SetFocusedStateTextColor( focusedColor );
		   	twoLineItemVisualizer->SetNormalStateTextColor( normalItemColor );

		
		    TFsTreeItemId treeItemId = iDownloadManagerList->InsertItemL( *twoLineItemData, *twoLineItemVisualizer, parentNode);
			CleanupStack::Pop( twoLineItemVisualizer );
			CleanupStack::Pop( twoLineItemData );
		    
			iModel->SetItemIdL( i, j, treeItemId );
			}
	   	}
	}
	
	
void CFSEmailUiDownloadManagerVisualiser::HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType )
	{
    FUNC_LOG;
    iDownloadManagerList->HideListL();
    if ( aType ==  CFsEmailUiViewBase::ESkinChanged )
        {
        ResetColorsL();
        iDownloadManagerList->ShowListL();
        }
  	else if ( aType == CFsEmailUiViewBase::EScreenLayoutChanged )
  	    {
    	ReScaleUiL();
        iDownloadManagerList->ShowListL();	
  	    }
	}

void CFSEmailUiDownloadManagerVisualiser::HandleDynamicVariantSwitchOnBackgroundL( CFsEmailUiViewBase::TDynamicSwitchType aType )
    {
    FUNC_LOG;
    if ( aType == CFsEmailUiViewBase::ESkinChanged )
        {
        ResetColorsL();
        }
    }

void CFSEmailUiDownloadManagerVisualiser::GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const
    {
    aLayoutArray.AppendL( iListLayout );
    }

void CFSEmailUiDownloadManagerVisualiser::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TFsTreeItemId focusedId = iDownloadManagerList->FocusedItem();
    
    switch(aCommand)
  		{
  		case EFsEmailUiCmdEmpty: // used when focused item is being downloaded
        case EAknSoftkeyOpen:
  		case EFsEmailUiCmdOpen:
  		    {
  		    TryToOpenItemL( focusedId );
			}
			break;
  		case EFsEmailUiCmdDownload:
  		    {
  		    iModel->StartDownloadL(focusedId);
  		    }
  		    break;
  		case EFsEmailUiCmdDownloadAll:
  			{
  			iModel->DownloadAllAttachmentsL();
  			}
  			break;
   		case EFsEmailUiCmdCancelDownload:
   		    {
   		    TInt reallyCancel = 
   		        TFsEmailUiUtility::ShowConfirmationQueryL( R_FSE_VIEWER_CANCEL_DOWNLOAD_QUERY );
			if ( reallyCancel )
				{
				iModel->CancelDownloadL( focusedId );
				}
   		    }
   		    break;
  		case EFsEmailUiCmdCancelAllDownloads:
  		    {
   		    TInt reallyCancel = 
   		        TFsEmailUiUtility::ShowConfirmationQueryL( R_FSE_VIEWER_CANCEL_DOWNLOADS_QUERY );
			if ( reallyCancel )
				{
   				iModel->CancelAllDownloadsL();
				}
  		    }
  		    break;
  		case EFsEmailUiCmdSave:
  			{
  		    TFileName fileName;
  		    TInt savedCount( 0 );
  		    if ( TFsEmailUiUtility::ShowSaveFolderDialogL( fileName ) )
  				{
				iModel->SaveAttachmentL( focusedId, fileName, savedCount );
  				}
  		    if ( savedCount )
  		        {
  		        TFsEmailUiUtility::ShowFilesSavedToFolderNoteL( savedCount );        
  		        }
  			}
  			break;
  		case EFsEmailUiCmdSaveAll:
  		    {
  		    TFileName fileName;
  			if ( TFsEmailUiUtility::ShowSaveFolderDialogL( fileName ) )
  				{
  				iModel->SaveAllAttachmentsL( fileName );
  				}
  		    }
  			break;
        case EFsEmailUiCmdClearFetchedAttachment:
        case EFsEmailUiCmdRemoveAttachment:
        case EFsEmailUiCmdActionsDelete: // from shortcut key
            {
            RemoveAttachmentL( focusedId );
            }
            break;
  		case EFsEmailUiCmdMsgDetails:
            {
            TPartData messageData = iModel->GetMessageL( focusedId );
            
            TMsgDetailsActivationData msgDetailsData;
            msgDetailsData.iMailBoxId = messageData.iMailBoxId;
            msgDetailsData.iFolderId = messageData.iFolderId;
            msgDetailsData.iMessageId = messageData.iMessageId;

            const TPckgBuf<TMsgDetailsActivationData> pkgOut( msgDetailsData );	
            iAppUi.EnterFsEmailViewL( MsgDetailsViewId, KStartMsgDetailsToBeginning, pkgOut );
            }
            break;
  		case EFSEmailUICmdViewMessage:
            {
            TPartData messageData = iModel->GetMessageL( focusedId );
            
            TMsgViewerActivationData msgViewerData;
            msgViewerData.iMailBoxId = messageData.iMailBoxId;
            msgViewerData.iFolderId = messageData.iFolderId;
            msgViewerData.iMessageId = messageData.iMessageId;
            msgViewerData.iDetails = EFSMsgDataStructure;
            const TPckgBuf<TMsgViewerActivationData> pkgOut( msgViewerData );	
            iAppUi.EnterFsEmailViewL( MailViewerId, KStartViewerWithMsgId, pkgOut );
            }
            break;
        case EFsEmailUiCmdExit:
            {
            iAppUi.Exit();
            }
            break;
        case EAknSoftkeyBack:
     		{
     		if ( !iAppUi.ViewSwitchingOngoing() )
     			{
	  	       	iAppUi.ReturnToPreviousViewL();
     			}
     		}   
        	break;
        	
        case EFsEmailUiCmdCollapse:
            {
            iDownloadManagerList->CollapseNodeL( focusedId );
            ChangeMskCommandL( R_FSE_QTN_MSK_EXPAND );
            }
            break;

        case EFsEmailUiCmdExpand:
            {
            iDownloadManagerList->ExpandNodeL( focusedId );
            ChangeMskCommandL( R_FSE_QTN_MSK_COLLAPSE );
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
       	case EFsEmailUiCmdHelp:
  			{
			TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
  			}
  			break; 
        default:
        	break;
        }
    }
    
void CFSEmailUiDownloadManagerVisualiser::SetMskL()
    {
    FUNC_LOG;
    TFsTreeItemId curId = iDownloadManagerList->FocusedItem(); 
	if ( curId )
		{
	    if( iDownloadManagerList->IsNode( curId ) )
	        {
	        if (iDownloadManagerList->IsExpanded( curId ) )
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
	        TAttachmentData* selectedItem = iModel->GetItem( iDownloadManagerList->FocusedItem());
	        if ( selectedItem && IsCommandAvailableOnItem( EFsEmailUiCmdDownload, selectedItem ) )
	            {
	            ChangeMskCommandL( R_FSE_QTN_MSK_DOWNLOAD );
	            }
	        else if ( selectedItem && IsCommandAvailableOnItem( EFsEmailUiCmdOpen, selectedItem ) )
	            {
	            ChangeMskCommandL( R_FSE_QTN_MSK_OPEN );
	            }
	        else
	            {
	            ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
	            }
	        }		
		}
	else
		{
        ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );		
		}	
    }	

TBool CFSEmailUiDownloadManagerVisualiser::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    TBool result( EFalse ); 
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
	
		if ( scanCode == EStdKeyRightArrow )
			{
			LaunchActionMenuL();
			result = ETrue;
			}
        else
            {
            // Check keyboard shortcuts.
            TInt shortcutCommand = 
                iAppUi.ShortcutBinding().CommandForShortcutKey( aEvent.KeyEvent(),
                                                                 CFSEmailUiShortcutBinding::EContextDownloadManager );
            if ( shortcutCommand != KErrNotFound )
                {
                HandleCommandL( shortcutCommand );
                result = ETrue;
                }
            }
        }

	/*
    if (!result)
        {
        result = iDownloadManagerList->TreeControl()->OfferEventL(aEvent);
        }
	*/
    return result;
    }

void CFSEmailUiDownloadManagerVisualiser::ReScaleUiL()
	{
    FUNC_LOG;
    TInt fontHeight = iAppUi.LayoutHandler()->ListItemFontHeightInTwips();
    
    // Header item
    TFsTreeItemId headerId = iDownloadManagerList->Child( KFsTreeRootID, 0 );
    MFsTreeItemVisualizer& headerVis = iDownloadManagerList->ItemVisualizer( headerId );
    headerVis.SetFontHeight( fontHeight );

    // Mail nodes and attachment items
    const RArray<TMessageData>& modelData = iModel->GetModel();
    for ( TInt i=0 ; i<modelData.Count() ; ++i )
        {
        const TMessageData& message = modelData[ i ];
        
        MFsTreeItemVisualizer& messageVis = iDownloadManagerList->ItemVisualizer( message.treeId );
        messageVis.SetFontHeight( fontHeight );
        
        for ( TInt j=0 ; j<message.mailAttachments.Count() ; j++ )
            {
            const TAttachmentData& attachment = message.mailAttachments[j];
            
            MFsTreeItemVisualizer& attachmentVis = iDownloadManagerList->ItemVisualizer( attachment.treeId );
            attachmentVis.SetFontHeight( fontHeight );
            }
        }	
    }

void CFSEmailUiDownloadManagerVisualiser::ResetColorsL()
    {
    FUNC_LOG;
   	TRgb normalTextColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
   	TRgb focusedTextColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
   	TRgb nodeTextColor = iAppUi.LayoutHandler()->ListNodeTextColor();
   	TRgb nodeBgColor = iAppUi.LayoutHandler()->ListNodeBackgroundColor();
    TRgb headerBgColor = iAppUi.LayoutHandler()->ListHeaderBackgroundColor();
   	
   	// Header item
   	TFsTreeItemId headerId = iDownloadManagerList->Child( KFsTreeRootID, 0 );
   	MFsTreeItemVisualizer& headerVis = iDownloadManagerList->ItemVisualizer( headerId );
   	headerVis.SetNormalStateTextColor( normalTextColor );
   	headerVis.SetFocusedStateTextColor( focusedTextColor );
   	headerVis.SetBackgroundColor( headerBgColor );

   	// Mail nodes and attachment items
	const RArray<TMessageData>& modelData = iModel->GetModel();
    for ( TInt i=0 ; i<modelData.Count() ; ++i )
        {
    	const TMessageData& message = modelData[ i ];
    	
       	MFsTreeItemVisualizer& messageVis = iDownloadManagerList->ItemVisualizer( message.treeId );
       	messageVis.SetNormalStateTextColor( nodeTextColor );
       	messageVis.SetFocusedStateTextColor( focusedTextColor );
       	messageVis.SetBackgroundColor( nodeBgColor );
    	
    	for ( TInt j=0 ; j<message.mailAttachments.Count() ; j++ )
    	    {
    	    const TAttachmentData& attachment = message.mailAttachments[j];
    	    
           	MFsTreeItemVisualizer& attachmentVis = iDownloadManagerList->ItemVisualizer( attachment.treeId );
           	attachmentVis.SetNormalStateTextColor( normalTextColor );
           	attachmentVis.SetFocusedStateTextColor( focusedTextColor );
    	    }
        }
    }

CAlfControl* CFSEmailUiDownloadManagerVisualiser::ViewerControl()
	{
    FUNC_LOG;
	return iScreenControl;
	}

TBool CFSEmailUiDownloadManagerVisualiser::IsCommandAvailableOnItem( TInt aCommandId, 
                                                                     const TAttachmentData* aFocusedItem ) const
    {
    FUNC_LOG;
    TBool available = EFalse;
    
    const RArray<TMessageData>& model = iModel->GetModel();

    switch ( aCommandId )
        {
        case EFsEmailUiCmdOpen:
        case FsEActionAttachmentOpen:
        // <cmail> Disabled until plugins support this.
        //case EFsEmailUiCmdClearFetchedAttachment:
        //case FsEActionAttachmentClearFetchedContent:
        // </cmail>
            {
    		if ( aFocusedItem && aFocusedItem->downloadProgress == KComplete )
    			{
    			available = ETrue;
    			}
            }
            break;

        // <cmail>
        case FsEActionAttachmentClearFetchedContent:
        case EFsEmailUiCmdClearFetchedAttachment:
            available = EFalse;
            break;
        // </cmail>

        case EFsEmailUiCmdDownload:
        case FsEActionAttachmentDownload:
        case EFsEmailUiCmdRemoveAttachment:
        case FsEActionAttachmentRemoveUnfetchedAttachment:
            {
    		if ( iAppUi.DownloadInfoMediator() && aFocusedItem && aFocusedItem->downloadProgress == KNone && 
    		     !iAppUi.DownloadInfoMediator()->IsDownloading( aFocusedItem->partData.iMessagePartId ) )
    			{
    			available = ETrue;
    			}
            }
            break;

        case EFsEmailUiCmdCancelDownload:
        case FsEActionAttachmentCancelDownload:
            {
    		if ( iAppUi.DownloadInfoMediator() && aFocusedItem && 
    			 iAppUi.DownloadInfoMediator()->IsDownloading( aFocusedItem->partData.iMessagePartId ) )
    			{
    			available = ETrue;
    			}
            }
            break;

        case EFsEmailUiCmdSave:
        case FsEActionAttachmentSave:
            {
    		if ( iAppUi.DownloadInfoMediator() && aFocusedItem && 
    			 !iAppUi.DownloadInfoMediator()->IsDownloading( aFocusedItem->partData.iMessagePartId ) )
    			{
    			available = ETrue;
    			}
            }
            break;

        case EFsEmailUiCmdSaveAll:
        case FsEActionAttachmentSaveAll:
            {
            // Save all is available if there is more than one attachment
            // and none of them are currently being downloaded
            if ( iModel->AttachmentCount() > 1 )
                {
                available = ETrue;
        		for ( TInt i=0; i<model.Count(); i++)
        			{
        			const RArray<TAttachmentData>& attachments = model[i].mailAttachments;
        			for ( TInt j=0 ; j<attachments.Count() ; ++j )
        			    {
            			if ( iAppUi.DownloadInfoMediator() && 
            				 iAppUi.DownloadInfoMediator()->IsDownloading( attachments[j].partData.iMessagePartId ) )
            				{
           					available = EFalse;
           					break;
            				}
        			    }
        			}
                }
            }
            break;

        case EFsEmailUiCmdCancelAllDownloads:
        case FsEActionAttachmentCancelAllDownloads:
            {
            // Cancel all is available if there is more than one ongoing download
            TInt downloads = 0;
    		for ( TInt i=0; i<model.Count(); i++)
    			{	
    			const RArray<TAttachmentData>& attachments = model[i].mailAttachments;
    			for ( TInt j=0 ; j<attachments.Count() ; ++j )
    			    {
        			if ( iAppUi.DownloadInfoMediator() && 
        				 iAppUi.DownloadInfoMediator()->IsDownloading( attachments[j].partData.iMessagePartId ) )
        				{
        				downloads++;
        				}
    			    }
    			}
    		if ( downloads > 1 )
    		    {
    		    available = ETrue;
    		    }
            }
            break;

        case EFsEmailUiCmdDownloadAll:
        case FsEActionAttachmentDownloadAll:
            {
            // Download all is available there is more than one attachments for which
            // "Download" command is available.
            TInt downloadableFiles = 0;
    		for ( TInt i=0; i<model.Count(); i++)
    			{
    			const RArray<TAttachmentData>& attachments = model[i].mailAttachments;
    			for ( TInt j=0 ; j<attachments.Count() ; ++j )
    			    {
        			if ( IsCommandAvailableOnItem( EFsEmailUiCmdDownload, &attachments[j] ) )
        				{
                        downloadableFiles++;
        				}
    			    }
    			}
    		if ( downloadableFiles > 1 )
    		    {
    		    available = ETrue;
    		    }
            }
            break;

        case EFsEmailUiCmdMailActions:
            {
            // Actions menu is available if any sub item is available
            available = 
                IsCommandAvailableOnItem( EFsEmailUiCmdOpen, aFocusedItem ) ||
                IsCommandAvailableOnItem( EFsEmailUiCmdDownload, aFocusedItem ) ||
                IsCommandAvailableOnItem( EFsEmailUiCmdCancelDownload, aFocusedItem ) ||
                IsCommandAvailableOnItem( EFsEmailUiCmdSave, aFocusedItem ) ||
                IsCommandAvailableOnItem( EFsEmailUiCmdDownloadAll, aFocusedItem ) ||
                IsCommandAvailableOnItem( EFsEmailUiCmdCancelAllDownloads, aFocusedItem ) ||
                IsCommandAvailableOnItem( EFsEmailUiCmdSaveAll, aFocusedItem ) ||
                IsCommandAvailableOnItem( EFsEmailUiCmdClearFetchedAttachment, aFocusedItem );
            }
            break;
            
        default:
            // All commands not separately specified are always available
            available = ETrue;
            break;
        }
        
    return available;
    }

HBufC* CFSEmailUiDownloadManagerVisualiser::CreateSecondLineTextLC( const TAttachmentData& aAttachment ) const
    {
    FUNC_LOG;
    // File extension
	TPtrC extString = TParsePtrC( aAttachment.fileName ).Ext();
	// It seems to vary if preceeding dot is included in the file extension. Remove it if present.
	if ( extString.Length() && extString[0] == '.' )
	    {
	    extString.Set( extString.Mid(1) );
	    }

    // File size	
	HBufC* sizeString = iModel->CreateSizeDescLC( aAttachment.fileSize );

    // Download progress
    HBufC* progressString = NULL;
	if ( iAppUi.DownloadInfoMediator() && iAppUi.DownloadInfoMediator()->IsDownloading( aAttachment.partData.iMessagePartId ) )
		{
		progressString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_LIST_DOWNLOADING, aAttachment.downloadProgress  );
		}
	else if ( aAttachment.downloadProgress == KComplete )
	    {
		progressString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_LIST_DOWNLOADED_100 );
	    }
	else
	    {
	    progressString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_LIST_DOWNLOADING_CANCELLED );
	    }

    // Combine the accumulated data to one buffer
    HBufC* result = HBufC::NewL( extString.Length() + 
                                 KSpace().Length() + 
                                 sizeString->Length() + 
                                 KSpace().Length() + 
                                 progressString->Length() );
    TPtr resultPtr = result->Des();
    resultPtr.Append( extString );
    resultPtr.Append( KSpace );
    resultPtr.Append( *sizeString );
    resultPtr.Append( KSpace );
    resultPtr.Append( *progressString );
    	
	CleanupStack::PopAndDestroy( progressString );
	CleanupStack::PopAndDestroy( sizeString );
	
	CleanupStack::PushL( result );
	return result;
    }

// ---------------------------------------------------------------------------
// Attempt to open the focused item. Start downloading if the item is not
// downloaded or show note if it's currently being downloaded.
// ---------------------------------------------------------------------------
//
void CFSEmailUiDownloadManagerVisualiser::TryToOpenItemL( TFsTreeItemId aId )
    {
    FUNC_LOG;
    TAttachmentData* attachment = NULL;
    
    TRAPD( err, attachment = &iModel->GetItemL( aId ) );
    
    if( !err )
    	{
	    if ( attachment )
	        {
	        if ( iModel->IsDownloading( *attachment ) )
	            {
	            TFsEmailUiUtility::ShowInfoNoteL( R_FSE_VIEWER_NOTE_ATTACHMENT_DOWNLOADING_PROGRESS );
	            }
	        else if ( attachment->downloadProgress != KComplete )
	            {
	            TInt restartDownload = 
	                TFsEmailUiUtility::ShowConfirmationQueryL( R_FSE_VIEWER_NOTE_ATTACHMENT_RESTART_DOWNLOADING );
	            if ( restartDownload )
	                {
	                HandleCommandL( EFsEmailUiCmdDownload );
	                }
	            }
	        else
	            {
	            TFsEmailUiUtility::OpenAttachmentL( attachment->partData );
	            }
	        }
	    }
    }

// ---------------------------------------------------------------------------
// Show confirmation note and remove contents of given attachment item
// if user accepts. Does nothing if given ID doesn't match an attachment item.
// ---------------------------------------------------------------------------
//
void CFSEmailUiDownloadManagerVisualiser::RemoveAttachmentL( TFsTreeItemId aId )
    {
    FUNC_LOG;
    TAttachmentData* item = iModel->GetItem( aId );
    
    // show confirmation note only when removing fully fetched attachment
    TInt reallyClear = EFalse;
    if ( IsCommandAvailableOnItem( EFsEmailUiCmdClearFetchedAttachment, item ) )
        {
        reallyClear = TFsEmailUiUtility::ShowConfirmationQueryL( R_FSE_VIEWER_CLEAR_FETCHED_QUERY );
        }
    else if ( IsCommandAvailableOnItem( EFsEmailUiCmdRemoveAttachment, item ) )
        {
        reallyClear = ETrue;
        }
    
    if ( reallyClear )
        {
        iModel->RemoveContentL( aId );
        // Remove item from DM only if contents were actually
        // removed succesfully
        if ( !iModel->IsCompletelyDownloadedL( *item ) )
            {
            // remove model item, message item is also removed automatically if it becomes empty
            iModel->RemoveItem( aId );

            // remove list item
            TFsTreeItemId parentId = iDownloadManagerList->Parent( aId );
            iDownloadManagerList->RemoveL( aId );
            // remove also the message node if it became empty
            if ( iDownloadManagerList->IsEmpty(parentId) )
                {
                iDownloadManagerList->RemoveL( parentId );
                }
            
            // focused item has changed => change MSK function
            SetMskL();
            
            // update title item
            HBufC* attCountText = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_HEADER_TOTAL, iModel->AttachmentCount() );
            CFsTreePlainTwoLineItemData& headerData = 
                static_cast<CFsTreePlainTwoLineItemData&>( iDownloadManagerList->ItemData(iHeaderItemId) );
            headerData.SetSecondaryDataL( *attCountText );
            CleanupStack::PopAndDestroy( attCountText );
            iDownloadManagerTreeListVisualizer->UpdateItemL( iHeaderItemId );
            }
        }
    }

void CFSEmailUiDownloadManagerVisualiser::DownloadStatusChangedL( TInt aIndex )
	{
    FUNC_LOG;
	if ( iAppUi.CurrentActiveView() == this &&
	     aIndex < iModel->GetModel().Count() &&
	     !iAppUi.AppUiExitOngoing() )
	    {
      	TRect screenRect;
     	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );	
    
       	TRgb focusedColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
       	TRgb normalItemColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
       	TRgb normalNodeColor = iAppUi.LayoutHandler()->ListNodeTextColor();
    
    	const RArray<TMessageData>& modelData = iModel->GetModel();
    	const TMessageData& message = modelData[ aIndex ];
    	
        // Insert the message node if it's not yet present
        if ( message.treeId == KFsTreeNoneID )
            {
            CFsTreePlainOneLineNodeData* separatorData = CFsTreePlainOneLineNodeData::NewL();
            CleanupStack::PushL( separatorData );
            CFsTreePlainOneLineNodeVisualizer* separatorVisualiser = 
                CFsTreePlainOneLineNodeVisualizer::NewL(*iDownloadManagerList->TreeControl());
            CleanupStack::PushL( separatorVisualiser );
            separatorData->SetDataL( *message.mailSubject );
            separatorVisualiser->SetExtendable(EFalse);
            separatorVisualiser->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );     
            separatorVisualiser->SetTextBold( ETrue );
            separatorVisualiser->SetFocusedStateTextColor( focusedColor );
            separatorVisualiser->SetNormalStateTextColor( normalNodeColor );
            separatorVisualiser->SetBackgroundColor( iAppUi.LayoutHandler()->ListNodeBackgroundColor() );
            
            separatorData->SetIconExpanded( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeExpanded ) );
            separatorData->SetIconCollapsed( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeCollapsed ) );
            CleanupStack::Pop( separatorVisualiser );
            CleanupStack::Pop( separatorData );
            TFsTreeItemId parentNode = 
                iDownloadManagerList->InsertNodeL( *separatorData, *separatorVisualiser, KFsTreeRootID, aIndex ); 
            iModel->SetNodeIdL( aIndex, parentNode );
            }

        // Insert or update the attachment items 
        for ( TInt j = 0 ; j < message.mailAttachments.Count() ; j++ )
    		{
            HBufC* secondLine = CreateSecondLineTextLC( message.mailAttachments[j] );
    
    		//is the item already drawn
       		TFsTreeItemId itemId = message.mailAttachments[j].treeId;
    		if ( itemId != KFsTreeNoneID )
    			{
    			// Item is already present. Update the existing item but only if the second line data
    			// is actually changed. 
    		    CFsTreePlainTwoLineItemData& itemData = 
    		            static_cast<CFsTreePlainTwoLineItemData&>( iDownloadManagerList->ItemData( itemId ) );

    		    if ( itemData.SecondaryData() != *secondLine )
    		        {
    		        itemData.SetSecondaryDataL( *secondLine );
        		    iDownloadManagerTreeListVisualizer->UpdateItemL( itemId );
        			
        	        // Update the MSK label if the updated item happens to be the focused item
        			if ( itemId == iDownloadManagerList->FocusedItem() )
        			    {
                        SetMskL();
        			    }
    		        }
    			}
    		else 
    			{
    			//create and draw new item
                CFsTreePlainTwoLineItemData* twoLineItemData = CFsTreePlainTwoLineItemData::NewL();
                CleanupStack::PushL( twoLineItemData );
        
                twoLineItemData->SetDataL( message.mailAttachments[j].fileName );
                twoLineItemData->SetSecondaryDataL( *secondLine );
                
                CAlfTexture& itemTexture = TFsEmailUiUtility::GetAttachmentIcon( message.mailAttachments[j].fileType, 
                                                                                 *iAppUi.FsTextureManager() );
                twoLineItemData->SetIcon( itemTexture );
        
    	 	    CFsTreePlainTwoLineItemVisualizer* twoLineItemVisualizer = 
    	 	        CFsTreePlainTwoLineItemVisualizer::NewL( *iDownloadManagerList->TreeControl() );
    	 	    CleanupStack::PushL( twoLineItemVisualizer );
    			twoLineItemVisualizer->SetFlags( twoLineItemVisualizer->Flags() | KFsTreeListItemHasMenu );
    	 		twoLineItemVisualizer->SetExtendable( ETrue );
				CAlfTextStyle* textStyle = iAppUi.LayoutHandler()->FSTextStyleFromIdL( EFSFontTypeSmall );
    			twoLineItemVisualizer->SetTextStyleId( textStyle->Id() );
    	 		// Set font height
    			twoLineItemVisualizer->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );		
    			// Set font bolding
    			twoLineItemVisualizer->SetTextBold( EFalse );
    		  	// Set correct skin text colors for the list items  
    		   	twoLineItemVisualizer->SetFocusedStateTextColor( focusedColor );
    		   	twoLineItemVisualizer->SetNormalStateTextColor( normalItemColor );
    			TFsTreeItemId treeItemId = 
    			    iDownloadManagerList->InsertItemL( *twoLineItemData, *twoLineItemVisualizer, message.treeId, j );
    			CleanupStack::Pop( twoLineItemVisualizer );  // ownership transferred to TreeList
    			CleanupStack::Pop( twoLineItemData );        // ownership transferred to TreeList
    			iModel->SetItemIdL( aIndex, j, treeItemId );
    			}

    		CleanupStack::PopAndDestroy( secondLine );      
    		}
	    }
	}

// <cmail> Touch
void CFSEmailUiDownloadManagerVisualiser::TreeListEventL(
	const TFsTreeListEvent aEvent, const TFsTreeItemId aId )
// </cmail>
	{
    FUNC_LOG;
	if ( aEvent == EFsTreeItemSelected )
		{
		TRAPD( err, TryToOpenItemL( aId ) );
		if ( err != KErrNone )
			{
			}
		}
	}

void CFSEmailUiDownloadManagerVisualiser::LaunchActionMenuL()
	{
    FUNC_LOG;
  	
	TAttachmentData* selectedItem = NULL;
	
	TRAPD( err, selectedItem = &iModel->GetItemL( iDownloadManagerList->FocusedItem() ) );
	
	if ( !err )
		{
		// Remove old items from action menu
	    CFSEmailUiActionMenu::RemoveAllL();
		
		// Construct item list
	    RFsEActionMenuIdList itemList;
        CleanupClosePushL( itemList );

		if ( IsCommandAvailableOnItem( FsEActionAttachmentOpen, selectedItem ) )
			{
			itemList.AppendL( FsEActionAttachmentOpen );
			}

		if ( IsCommandAvailableOnItem( FsEActionAttachmentDownload, selectedItem ) )
			{
			itemList.AppendL( FsEActionAttachmentDownload );
			}

        if ( IsCommandAvailableOnItem( FsEActionAttachmentDownloadAll, selectedItem ) )
            {
            itemList.AppendL( FsEActionAttachmentDownloadAll );
            }

		if ( IsCommandAvailableOnItem( FsEActionAttachmentCancelDownload, selectedItem ) )
			{
			itemList.AppendL( FsEActionAttachmentCancelDownload );
			}

        if ( IsCommandAvailableOnItem( FsEActionAttachmentCancelAllDownloads, selectedItem ) )
            {
            itemList.AppendL( FsEActionAttachmentCancelAllDownloads );
            }
            
		if ( IsCommandAvailableOnItem( FsEActionAttachmentSave, selectedItem ) )
			{
			itemList.AppendL( FsEActionAttachmentSave );
			}
		
        if ( IsCommandAvailableOnItem( FsEActionAttachmentSaveAll, selectedItem ) )
            {
            itemList.AppendL( FsEActionAttachmentSaveAll );
            }

        if ( IsCommandAvailableOnItem( FsEActionAttachmentClearFetchedContent, selectedItem ) )
            {
            itemList.AppendL( FsEActionAttachmentClearFetchedContent );
            }

        if ( IsCommandAvailableOnItem( FsEActionAttachmentRemoveUnfetchedAttachment, selectedItem ) )
            {
            itemList.AppendL( FsEActionAttachmentRemoveUnfetchedAttachment );
            }
        
		// Execute action list and handle the menu command
	   	TActionMenuCustomItemId itemId = CFSEmailUiActionMenu::ExecuteL( itemList );
	   	
	   	CleanupStack::PopAndDestroy( &itemList );
	   	
	  	switch( itemId )
	  		{
			case FsEActionAttachmentOpen:
			    HandleCommandL( EFsEmailUiCmdOpen );
			    break;
			case FsEActionAttachmentDownload:
			    HandleCommandL( EFsEmailUiCmdDownload );
			    break;
			case FsEActionAttachmentCancelDownload:
                HandleCommandL( EFsEmailUiCmdCancelDownload );
				break;
			case FsEActionAttachmentSave:
			    HandleCommandL( EFsEmailUiCmdSave );
			    break;
            case FsEActionAttachmentDownloadAll:
                HandleCommandL( EFsEmailUiCmdDownloadAll );
                break;
            case FsEActionAttachmentCancelAllDownloads:
                HandleCommandL( EFsEmailUiCmdCancelAllDownloads );
                break;
            case FsEActionAttachmentSaveAll:
                HandleCommandL( EFsEmailUiCmdSaveAll );
                break;
            case FsEActionAttachmentClearFetchedContent:
                HandleCommandL( EFsEmailUiCmdClearFetchedAttachment );
                break;
            case FsEActionAttachmentRemoveUnfetchedAttachment:
                HandleCommandL( EFsEmailUiCmdRemoveAttachment );
                break;
			default:
				break;
	  		}
  		}
	}

// ---------------------------------------------------------------------------
// If there is one or more expanded nodes, collapses all nodes.
// Otherwise expands all nodes.
// ---------------------------------------------------------------------------
//
void CFSEmailUiDownloadManagerVisualiser::ShortcutCollapseExpandAllToggleL()
    {
    FUNC_LOG;
    TBool collapseAllNodes( EFalse );
    TInt count = iDownloadManagerList->CountChildren( KFsTreeRootID );
    for ( TInt i=0 ; i<count ; i++ )
        {
        TFsTreeItemId curId = iDownloadManagerList->Child(KFsTreeRootID, i);
        if ( iDownloadManagerList->IsNode( curId ) &&
             iDownloadManagerList->IsExpanded( curId ) )
            {
            collapseAllNodes = ETrue;
            break;
            }	
        }

    if ( collapseAllNodes )
        {
        iDownloadManagerTreeListVisualizer->CollapseAllL();        
        }
    else
        {
       	TFsTreeItemId prevId = iDownloadManagerList->FocusedItem(); // the focus may fall out of the screen unless manually reset
        iDownloadManagerTreeListVisualizer->ExpandAllL();
    	iDownloadManagerTreeListVisualizer->SetFocusedItemL( prevId );    
        }
    }

// ---------------------------------------------------------------------------
// Moves the focus to the topmost item
// ---------------------------------------------------------------------------
//
void CFSEmailUiDownloadManagerVisualiser::GoToTopL()
    {
    FUNC_LOG;
    TInt topLevelCount = iDownloadManagerList->CountChildren( KFsTreeRootID );
    if ( topLevelCount > 1 )
        {
        // the item #0 is the non-focusable header item
        TFsTreeItemId topId = iDownloadManagerList->Child(KFsTreeRootID, 1);
        iDownloadManagerTreeListVisualizer->SetFocusedItemL( topId );            
        }
    }

// ---------------------------------------------------------------------------
// Moves the focus to the bottommost item
// ---------------------------------------------------------------------------
//
void CFSEmailUiDownloadManagerVisualiser::GoToBottomL()
    {
    FUNC_LOG;
    TInt topLevelCount = iDownloadManagerList->CountChildren( KFsTreeRootID );
    if ( topLevelCount )
        {
        TFsTreeItemId bottomId = iDownloadManagerList->Child(KFsTreeRootID, topLevelCount-1);
        if ( iDownloadManagerList->IsNode(bottomId) )
            {
            TInt childCount = iDownloadManagerList->CountChildren(bottomId);
            if ( childCount && iDownloadManagerList->IsExpanded(bottomId) )
                {
                // Focus the last child of the bottom node if the node is expanded.
                bottomId = iDownloadManagerList->Child( bottomId, childCount-1 );
                }
            }
        iDownloadManagerTreeListVisualizer->SetFocusedItemL( bottomId );            
        }
    }

	
