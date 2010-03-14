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
* Description:  Implements visualiser for the attachment list view
*
*/



// SYSTEM INCLUDES
#include "emailtrace.h"
#include "cfsmailbox.h"
#include <e32base.h>
#include <freestyleemailui.mbg>
#include <AknUtils.h>
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
#include "fstreeplaintwolineitemdata.h"
#include "fstreeplaintwolineitemvisualizer.h"
#include "fstreeplainonelinenodedata.h"
#include "fstreeplainonelinenodevisualizer.h"
//</cmail>
#include <aknlistquerydialog.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
// <cmail> SF
#include <alf/alfanchorlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfevent.h>
#include <csxhelp/cmail.hlp.hrh>
// </cmail>
#include <featmgr.h>
//</cmail>

// <cmail> Use layout data instead of hard-coded values
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
// </cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailUiAttachmentsListVisualiser.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiAttachmentsListModel.h"
#include "FreestyleEmailUiAttachmentsListControl.h"
#include "FreestyleEmailUiFileSystemInfo.h"
#include "FreestyleEmailUiScrollbarClet.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FreestyleEmailUiStatusIndicator.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiLiterals.h"

// POP3 has a specific UI for the attachment list: there is no download-option
// available. This value should be the UID of the POP3 ECOM plugin.
const TUint KFsPop3PluginId = 0x2000e53e;


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiAttachmentsListVisualiser* CFSEmailUiAttachmentsListVisualiser::NewL(
    CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup )
    {
    FUNC_LOG;
    CFSEmailUiAttachmentsListVisualiser* self = 
        CFSEmailUiAttachmentsListVisualiser::NewLC( aEnv, aAppUi, aControlGroup );
    CleanupStack::Pop( self );
    return self;
    }

CFSEmailUiAttachmentsListVisualiser* CFSEmailUiAttachmentsListVisualiser::NewLC(
    CAlfEnv& aEnv, CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( aAppUi, User::Invariant() );
    CFSEmailUiAttachmentsListVisualiser* self =
        new (ELeave) CFSEmailUiAttachmentsListVisualiser( aEnv, *aAppUi, aControlGroup );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::ConstructL()
    {
    FUNC_LOG;
	BaseConstructL( R_FSEMAILUI_ATTACHMENTS_VIEW );
    iFirstStartCompleted = EFalse;	
 	}

// ---------------------------------------------------------------------------
// CFSEmailUiAttachmentsListVisualiser::UpdateSize
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::UpdateSize()
    {
    FUNC_LOG;    
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, screenRect );
    screenRect.Move(-screenRect.iTl.iX, -screenRect.iTl.iY);
    iAnchorLayout->SetRect( screenRect );
    // Set anchors so that list leaves space for control bar
    iAnchorLayout->SetAnchor(EAlfAnchorTopLeft, 0,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint(0, 0 ));
    iAnchorLayout->SetAnchor(EAlfAnchorBottomRight, 0,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint(screenRect.iBr.iX, screenRect.iBr.iY ));
    iAnchorLayout->UpdateChildrenLayout();
    }

// ---------------------------------------------------------------------------
// CFSEmailUiAttachmentsListVisualiser::DoFirstStartL
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::DoFirstStartL()
    {
    FUNC_LOG;
    // Create list screen (list) control and screen anchor layout
    iScreenControl = CFreestyleEmailUiAttachmentsListControl::NewL( iEnv, this );
    
    // Anchor layout
    iAnchorLayout = CAlfAnchorLayout::AddNewL( *iScreenControl );
    
    // Create attachments list layout and set it to fill the main pane
    iListLayout = CAlfDeckLayout::AddNewL( *iScreenControl, iAnchorLayout );  

    UpdateSize();
    
    // Create attachments list tree list component 
    iAttachmentsTreeListVisualizer = CFsTreeVisualizerBase::NewL(iScreenControl, *iListLayout);
    iAttachmentsTreeListVisualizer->SetItemExpansionDelay( iAppUi.LayoutHandler()->ListItemExpansionDelay() );
    
    iAttachmentsList = CFsTreeList::NewL(*iAttachmentsTreeListVisualizer, iEnv);   
    iAttachmentsList->SetScrollbarVisibilityL( EFsScrollbarAuto );
    iAttachmentsList->SetMarkTypeL( CFsTreeList::EFsTreeListMultiMarkable );
    iAttachmentsList->SetIndentationL( 0 );
    iAttachmentsList->AddObserverL( *this );

    iAppUi.LayoutHandler()->SetListMarqueeBehaviour( iAttachmentsList );

    //<cmail> Compared to S60 3.2.3 in S60 5.0 Alf offers the key events in
    // opposite order.
    ControlGroup().AppendL( iScreenControl );
  	ControlGroup().AppendL( iAttachmentsList->TreeControl() );
    //</cmail>

    // create model
	iModel = CFSEmailUiAttachmentsListModel::NewL( iAppUi, *this );

    //<cmail> S60 skin support
    //iAttachmentsTreeListVisualizer->SetBackgroundTextureL( iAppUi.FsTextureManager()->TextureByIndex( EBackgroundTextureMailList ) );  
    //</cmail>
    iAttachmentsTreeListVisualizer->SetMenuIcon( iAppUi.FsTextureManager()->TextureByIndex( EListControlMenuIcon ) );       
    iAttachmentsTreeListVisualizer->SetItemsAlwaysExtendedL( ETrue );

    // Set page up and page down keys
    iAttachmentsTreeListVisualizer->AddCustomPageUpKey( EStdKeyPageUp );
    iAttachmentsTreeListVisualizer->AddCustomPageDownKey( EStdKeyPageDown );
  
    CAlfBrush* selectorBrush = iAppUi.FsTextureManager()->ListSelectorBrushL();
    iAttachmentsTreeListVisualizer->SetSelectorPropertiesL( selectorBrush, 1.0, CFsTreeVisualizerBase::EFsSelectorMoveSmoothly );   
   
    iFirstStartCompleted = ETrue;
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiAttachmentsListVisualiser::CFSEmailUiAttachmentsListVisualiser(
    CAlfEnv& aEnv, CFreestyleEmailUiAppUi& aAppUi, CAlfControlGroup& aControlGroup )
    : CFsEmailUiViewBase( aControlGroup, aAppUi ),
    iEnv( aEnv )
	{
    FUNC_LOG;
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiAttachmentsListVisualiser::~CFSEmailUiAttachmentsListVisualiser()
	{
    FUNC_LOG;
  	delete iModel;
	if ( iAttachmentsList )
		{
		iAttachmentsList->RemoveObserver( *this );
		}
	//<cmail> removed as not used anywhere.

	//</cmail>
	delete iAttachmentsList;
	}


TUid CFSEmailUiAttachmentsListVisualiser::Id() const
	{
    FUNC_LOG;
	return AttachmentMngrViewId;
	}
	

// <cmail> Toolbar
/*void CFSEmailUiAttachmentsListVisualiser::DoActivateL(const TVwsViewId& aPrevViewId,
                     TUid aCustomMessageId,
                     const TDesC8& aCustomMessage)*/
void CFSEmailUiAttachmentsListVisualiser::ChildDoActivateL(const TVwsViewId& /*aPrevViewId*/,
                     TUid /*aCustomMessageId*/,
                     const TDesC8& aCustomMessage)
// </cmail> Toolbar
	{
    FUNC_LOG;

  	if ( !iFirstStartCompleted )
  	    {
  	    DoFirstStartL();
  	    }
  	
    iAttachmentsList->HideListL();

    // Refresh the view with the given part data.
  	// If no part data is passed, the view is refreshed with data 
  	// from previously viewed message.
  	TAttachmentListActivationData activationData;
  	if ( aCustomMessage.Length() )
  	    {
        TPckgC<TAttachmentListActivationData> pkgPtr( activationData );
        pkgPtr.Set( aCustomMessage );
        activationData = pkgPtr();
        iEmbeddedMsgMode = activationData.iEmbeddedMsgMode;
  	    }
  	TPartData part(	activationData.iMailBoxId, activationData.iFolderId, activationData.iMessageId );
  	RefreshL( part );

   	iAttachmentsList->ShowListL();
	
	SetupTitlePaneTextL();    
	
	iAttachmentsList->SetFocusedL(ETrue);
	}

// <cmail> Toolbar
// ---------------------------------------------------------------------------
// OfferToolbarEventL
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::OfferToolbarEventL( TInt /*aCommand*/ )
    {
    
    }

// ---------------------------------------------------------------------------
// ToolbarResourceId
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiAttachmentsListVisualiser::ToolbarResourceId() const
    {
    return R_FREESTYLE_EMAIL_UI_TOOLBAR_ATTACHMENT_LIST;
    }
	
// </cmail>
	             
void CFSEmailUiAttachmentsListVisualiser::ChildDoDeactivate()
	{
    FUNC_LOG;
  	if ( !iAppUi.AppUiExitOngoing() )
  		{
        TRAP_IGNORE(iAttachmentsList->SetFocusedL(EFalse));
    	
        iAttachmentsTreeListVisualizer->NotifyControlVisibilityChange( EFalse );
  		}
	}	
	
CFSEmailUiAttachmentsListModel* CFSEmailUiAttachmentsListVisualiser::Model()
	{
    FUNC_LOG;
	return iModel;	
	}

void CFSEmailUiAttachmentsListVisualiser::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    FUNC_LOG;
	
    TAttachmentData* selectedItem = iModel->GetItem( iAttachmentsList->FocusedItem() );

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
        
        aMenuPane->SetItemDimmed( EFsEmailUiCmdRemoveAttachment, ETrue ); // available only in Download Manager
        }
    else if ( aResourceId == R_FSEMAILUI_ATTACHMENTS_MENUPANE )
        {
        if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
    	   {
    	   // remove help support in pf5250
    	   aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);      
    	   }	
        
        aMenuPane->SetItemDimmed( EFsEmailUiCmdMailActions,
                !IsCommandAvailableOnItem(EFsEmailUiCmdMailActions, selectedItem) );
        }

    // Add shortcut hints
    iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane, 
            CFSEmailUiShortcutBinding::EContextAttachmentList );
    }

void CFSEmailUiAttachmentsListVisualiser::DownloadStatusChangedL( TInt aIndex )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) //Safety
        {
        const RArray<TAttachmentData>& model = iModel->GetModel();

        TBool refreshNeeded = ( aIndex == KErrNotFound );

        if ( iAppUi.CurrentActiveView() == this && 
                ( aIndex < model.Count() || refreshNeeded ) &&
                !iAppUi.AppUiExitOngoing() )
            {
            if ( refreshNeeded )
                {
   		        // We end up here when the model has been changed during the
   		        // download operation. This happens with POP redownload as POP
   		        // creates new entries (message parts) after the operation.
                RefreshL( TPartData() );

            	GoToTopL();
                }
            else
                {
                const TAttachmentData& attachment = model[aIndex];

                CFsTreePlainTwoLineItemData& itemData = 
                    static_cast<CFsTreePlainTwoLineItemData&>(
                        iAttachmentsList->ItemData( attachment.treeId ) );

                itemData.SetDataL( attachment.fileName );

                HBufC* secondLine = CreateSecondLineTextLC( attachment );

                // Move the attachment under the "Downloaded" node if the 
                // download was just completed
                if ( attachment.downloadProgress == KComplete && 
                        iAttachmentsList->Parent( attachment.treeId ) ==
                            iNotDownloadedNode )
                    {
                    itemData.SetSecondaryDataL( *secondLine );
                    MoveItemToNodeL( attachment.treeId, iDownloadedNode );
                    }
                // Otherwise, just order the list item to be updated in its place
                // but only if data has actually changed
                else if ( itemData.SecondaryData() != *secondLine )
                    {
                    itemData.SetSecondaryDataL( *secondLine );
                    iAttachmentsTreeListVisualizer->UpdateItemL( attachment.treeId );
                    }
                else
                    {
                    // no updating is actually needed => do nothing
                    }
                CleanupStack::PopAndDestroy( secondLine );

                // Update the MSK label as the correct function for the
                // focused item may have changed.
                SetMskL();
                }    
            } 	    
        }		
    }
	
void CFSEmailUiAttachmentsListVisualiser::RefreshL( TPartData aPart )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) //Safety
        {
        // Update model. Use previous message if no new message has been provided.
        if ( aPart.iMessageId.IsNullId() )
            {
            aPart = iModel->GetMessageL( KFsTreeNoneID );
            }
        iModel->UpdateListL( aPart );
        
        UpdateSize();

        CFsTreePlainTwoLineItemData* twoLineItemData;
        CFsTreePlainTwoLineItemVisualizer* twoLineItemVisualizer;

        iAttachmentsList->RemoveAllL();
        CreateHeadersAndSeparatorsL();
        const RArray<TAttachmentData>& model = iModel->GetModel();

        TInt modelCount = model.Count();
        for ( TInt i = 0; i < modelCount; i++ )
            {
            const TAttachmentData& attachment = model[i];

            twoLineItemData = CFsTreePlainTwoLineItemData::NewL();
            CleanupStack::PushL( twoLineItemData );
            twoLineItemData->SetDataL( attachment.fileName );

            HBufC* secondLine = CreateSecondLineTextLC( attachment );
            twoLineItemData->SetSecondaryDataL( *secondLine );
            CleanupStack::PopAndDestroy( secondLine );

            CAlfTexture& itemTexture = TFsEmailUiUtility::GetAttachmentIcon(
                    attachment.fileType, *iAppUi.FsTextureManager() );
            twoLineItemData->SetIcon( itemTexture );

            twoLineItemVisualizer = CFsTreePlainTwoLineItemVisualizer::NewL(
                    *iAttachmentsList->TreeControl() );
            CleanupStack::PushL( twoLineItemVisualizer );
            twoLineItemVisualizer->SetFlags( twoLineItemVisualizer->Flags()
                    | KFsTreeListItemHasMenu );
            twoLineItemVisualizer->SetExtendable( ETrue );
            // <cmail> Platform layout changes
            /*
            CAlfTextStyle* textStyle =
                iAppUi.LayoutHandler()->FSTextStyleFromIdL( EFSFontTypeSmall );
            */
            TInt variety( Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0 );
            CAlfTextStyle& textStyle =
                iAppUi.LayoutHandler()->FSTextStyleFromLayoutL(
                    AknLayoutScalable_Apps::list_single_dyc_row_text_pane_t1(variety));
            twoLineItemVisualizer->SetTextStyleId( textStyle.Id() );
            // Set font height
           /*
            twoLineItemVisualizer->SetFontHeight(
                    iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );      
			*/
            twoLineItemVisualizer->SetFontHeight( textStyle.TextSizeInTwips() );
            // </cmail> Platform layout changes

            // Set font bolding
            twoLineItemVisualizer->SetTextBold( EFalse );
            // Set correct skin text colors for the list items  
            TRgb focusedColor =
            iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
            TRgb normalColor =
            iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
            twoLineItemVisualizer->SetFocusedStateTextColor( focusedColor );
            twoLineItemVisualizer->SetNormalStateTextColor( normalColor );

            TFsTreeItemId treeItemId;    
            CleanupStack::Pop( twoLineItemVisualizer );
            CleanupStack::Pop( twoLineItemData );
            if ( attachment.downloadProgress == KComplete )
                {
                treeItemId = iAttachmentsList->InsertItemL(
                        *twoLineItemData,
                        *twoLineItemVisualizer,
                        iDownloadedNode );
                }
            else
                {
                treeItemId = iAttachmentsList->InsertItemL(
                        *twoLineItemData,
                        *twoLineItemVisualizer,
                        iNotDownloadedNode );    
                }
            iModel->SetNodeIdL( i, treeItemId );
            }

        if ( iAttachmentsList->IsEmpty( iDownloadedNode ) )
            {
            iAttachmentsTreeListVisualizer->HideItemL( iDownloadedNode );
            }
        if ( iAttachmentsList->IsEmpty( iNotDownloadedNode ) )
            {
            iAttachmentsTreeListVisualizer->HideItemL( iNotDownloadedNode );
            }
        }
    }
	
void CFSEmailUiAttachmentsListVisualiser::ReScaleUiL()
	{
    FUNC_LOG;
    UpdateSize();
  	}

void CFSEmailUiAttachmentsListVisualiser::ResetColorsL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted )
        {
        TRgb normalTextColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
        TRgb focusedTextColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
        TRgb nodeTextColor = iAppUi.LayoutHandler()->ListNodeTextColor();
   	
        // Header item
        TFsTreeItemId headerId = iAttachmentsList->Child( KFsTreeRootID, 0 );
        MFsTreeItemVisualizer& headerVis = iAttachmentsList->ItemVisualizer( headerId );
        headerVis.SetNormalStateTextColor( normalTextColor );
        headerVis.SetFocusedStateTextColor( focusedTextColor );
   
        // Dividers
        iDownloadedVisualiser->SetNormalStateTextColor( nodeTextColor );
        iDownloadedVisualiser->SetFocusedStateTextColor( focusedTextColor );
        
        iNotDownloadedVisualiser->SetNormalStateTextColor( nodeTextColor );
        iNotDownloadedVisualiser->SetFocusedStateTextColor( focusedTextColor );
        
        // Attachment items
        const RArray<TAttachmentData>& model = iModel->GetModel();
        for ( TInt i=0; i<model.Count(); i++ )
            {
            MFsTreeItemVisualizer& attachmentVis = iAttachmentsList->ItemVisualizer( model[i].treeId );
            attachmentVis.SetNormalStateTextColor( normalTextColor );
            attachmentVis.SetFocusedStateTextColor( focusedTextColor );
            }       
        }
    }
	
void CFSEmailUiAttachmentsListVisualiser::HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType )
	{
    FUNC_LOG;
  	if ( iFirstStartCompleted )
  	    {
        iAttachmentsList->HideListL();
        if ( aType == CFsEmailUiViewBase::EScreenLayoutChanged )
            {
            ReScaleUiL();
            iAttachmentsList->ShowListL();
            }
        else if ( aType ==  CFsEmailUiViewBase::ESkinChanged )
            {
            ResetColorsL();
            iAttachmentsList->ShowListL();
            }	    
  	    }  	
	}

void CFSEmailUiAttachmentsListVisualiser::HandleDynamicVariantSwitchOnBackgroundL( CFsEmailUiViewBase::TDynamicSwitchType aType )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && aType == CFsEmailUiViewBase::ESkinChanged )
        {
        ResetColorsL();
        }
    }

void CFSEmailUiAttachmentsListVisualiser::GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const
    {
    FUNC_LOG;
    aLayoutArray.AppendL( iListLayout );
    }

void CFSEmailUiAttachmentsListVisualiser::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;
  	
  	TFsTreeItemId focusedId = iAttachmentsList->FocusedItem();
  	            
    switch(aCommand)
  		{
  		case EFsEmailUiCmdEmpty: // this is used when focused item is being downloaded
  		case EFsEmailUiCmdOpen:
        case EAknSoftkeyOpen:
  		    {
  		    TryToOpenItemL( focusedId );
  		    }
  			break;
  		case EFsEmailUiCmdDownload:
  		    {
  		    iModel->StartDownloadL( focusedId );
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
  			if ( TFsEmailUiUtility::ShowSaveFolderDialogL( fileName ) )
  				{
  				TInt savedCount( 0 );
  				iModel->SaveAttachmentL( focusedId, fileName, savedCount );
			    if ( savedCount )
			        {
			        TFsEmailUiUtility::ShowFilesSavedToFolderNoteL( savedCount );        
			        }
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
  		case EFsEmailUiCmdDownloadAll:
  			{
  			iModel->DownloadAllAttachmentsL();
  			}
  			break;
  		case EFsEmailUiCmdClearFetchedAttachment:
  		case EFsEmailUiCmdActionsDelete: // from shortcut key
  		    {
  		    RemoveAttachmentContentL( focusedId );
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
            iAttachmentsList->CollapseNodeL( focusedId );
            ChangeMskCommandL( R_FSE_QTN_MSK_EXPAND );
            }
            break;
            
        case EFsEmailUiCmdExpand:
            {
            iAttachmentsList->ExpandNodeL( focusedId );
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
    
void CFSEmailUiAttachmentsListVisualiser::SetViewSoftkeysL( TInt aResourceId )
	{
    FUNC_LOG;
    Cba()->SetCommandSetL( aResourceId );
    Cba()->DrawDeferred();	
	}
	
void CFSEmailUiAttachmentsListVisualiser::SetMskL()
    {
    FUNC_LOG;
    TFsTreeItemId curId = iAttachmentsList->FocusedItem(); 

    // We end up here when the model has been changed during the
    // download operation. This happens with POP redownload as POP
    // creates new entries.
    if ( iAttachmentsList->IsNode( curId ) )
        {
        if ( iAttachmentsList->IsExpanded( curId ) )
            {
            ChangeMskCommandL( R_FSE_QTN_MSK_COLLAPSE );
            } 
        else
            {
            ChangeMskCommandL( R_FSE_QTN_MSK_EXPAND );
            }
        }
    else 
        {
        TAttachmentData* selectedItem =
            iModel->GetItem( iAttachmentsList->FocusedItem() );

        if ( selectedItem &&
                IsCommandAvailableOnItem( EFsEmailUiCmdDownload, selectedItem ) )
            {
            ChangeMskCommandL( R_FSE_QTN_MSK_DOWNLOAD );
            }
        else if ( selectedItem &&
                IsCommandAvailableOnItem( EFsEmailUiCmdOpen, selectedItem ) )
            {
            ChangeMskCommandL( R_FSE_QTN_MSK_OPEN );
            }
        else
            {
            ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
            }
        }
    }
TBool CFSEmailUiAttachmentsListVisualiser::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    TBool result( EFalse ); 
    SetMskL();
    // Handle only key events, no key down or key up events.
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
	   	                                                         CFSEmailUiShortcutBinding::EContextAttachmentList );
	   	    if ( shortcutCommand != KErrNotFound )
	   	        {
	            HandleCommandL( shortcutCommand );
	   	        result = ETrue;
	   	        }
			}
		}
    else if (aEvent.IsPointerEvent())
        {
        result = iAttachmentsList->TreeControl()->OfferEventL(aEvent);
        }
    return result;
    }

CAlfControl* CFSEmailUiAttachmentsListVisualiser::ViewerControl()
	{
    FUNC_LOG;
	return iScreenControl;
	}

void CFSEmailUiAttachmentsListVisualiser::CreateHeadersAndSeparatorsL()
	{
    FUNC_LOG;
	TRect screenRect;
	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );
    // <cmail> Removed attachments row from attachemnts view. New UI
	// requirement. Some other lines of code also removed from this function
	// because of the modification.
  
    //</cmail>
    //<cmail> Platform layout changes
	TInt variety( Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0 );
    CAlfTextStyle& textStyle =
        iAppUi.LayoutHandler()->FSTextStyleFromLayoutL(
                AknLayoutScalable_Apps::list_single_dyc_row_text_pane_t1(variety));

    // </cmail> Platform layout changes

  	// Set correct skin text colors for the list items
    TRgb focusedColor = iAppUi.LayoutHandler()->ListFocusedStateTextSkinColor();
   	TRgb normalColor = iAppUi.LayoutHandler()->ListNormalStateTextSkinColor();
   	TRgb normalNodeColor = iAppUi.LayoutHandler()->ListNodeTextColor();

    // <cmail>

    // </cmail>

    // <cmail>

    // </cmail>

    // <cmail> Platform layout changes

    // </cmail> Platform layout changes


	CFsTreePlainOneLineNodeData* separatorData = CFsTreePlainOneLineNodeData::NewL();
	CleanupStack::PushL( separatorData );
    CFsTreePlainOneLineNodeVisualizer* separatorVisualiser = CFsTreePlainOneLineNodeVisualizer::NewL(*iAttachmentsList->TreeControl());
    CleanupStack::PushL( separatorVisualiser );

    HBufC* resourceString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_LIST_DOWNLOADED );
	separatorData->SetDataL( *resourceString );
	CleanupStack::PopAndDestroy( resourceString );
    separatorVisualiser->SetSize(TSize(screenRect.Width(), iAppUi.LayoutHandler()->OneLineListNodeHeight()));
    separatorVisualiser->SetExtendable(EFalse);
	// Set font height
    // <cmail> Platform layout changes
    //separatorVisualiser->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );
    separatorVisualiser->SetFontHeight( textStyle.TextSizeInTwips() );
    // </cmail> Platform layout changes
	// Set font bolding
	separatorVisualiser->SetTextBold( ETrue );
	separatorData->SetIconExpanded( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeExpanded ) );
	separatorData->SetIconCollapsed( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeCollapsed ) );

    // <cmail> Platform layout changes
    // textStyle = iAppUi.LayoutHandler()->FSTextStyleFromIdL( EFSFontTypeSmall );
	separatorVisualiser->SetTextStyleId( textStyle.Id() );
    // </cmail> Platform layout changes

  	// Set correct skin text colors for the list items
	separatorVisualiser->SetFocusedStateTextColor( focusedColor );
	separatorVisualiser->SetNormalStateTextColor( normalNodeColor );
	// <cmail>
	//separatorVisualiser->SetBackgroundColor( iAppUi.LayoutHandler()->ListNodeBackgroundColor() );
	// </cmail>
    // Gradient background for headings    
    CAlfBrush *titleDividerBgBrush = iAppUi.FsTextureManager()->TitleDividerBgBrushL();
    separatorVisualiser->SetBackgroundBrush( titleDividerBgBrush );

    iDownloadedVisualiser = separatorVisualiser;
	CleanupStack::Pop( separatorVisualiser );
	CleanupStack::Pop( separatorData );
    iDownloadedNode = iAttachmentsList->InsertNodeL(*separatorData, *separatorVisualiser, KFsTreeRootID);

	separatorData = CFsTreePlainOneLineNodeData::NewL();
	CleanupStack::PushL( separatorData );

    separatorVisualiser = CFsTreePlainOneLineNodeVisualizer::NewL(*iAttachmentsList->TreeControl());
    CleanupStack::PushL( separatorVisualiser );
	resourceString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_LIST_NOT_YET_DOWNLOADED );
	separatorData->SetDataL( *resourceString );
	CleanupStack::PopAndDestroy( resourceString );
    separatorVisualiser->SetSize(TSize(screenRect.Width(), iAppUi.LayoutHandler()->OneLineListNodeHeight()));
    separatorVisualiser->SetExtendable(EFalse);
	// Set font height
    // <cmail> Platform layout changes
    //separatorVisualiser->SetFontHeight( iAppUi.LayoutHandler()->ListItemFontHeightInTwips() );
    separatorVisualiser->SetFontHeight( textStyle.TextSizeInTwips() );
    // </cmail> Platform layout changes
	// Set font bolding
	separatorVisualiser->SetTextBold( ETrue );
	
  	// Set correct skin text colors for the list items  
	separatorVisualiser->SetFocusedStateTextColor( focusedColor );
   	separatorVisualiser->SetNormalStateTextColor( normalNodeColor );
   	// <cmail>
   	//separatorVisualiser->SetBackgroundColor( iAppUi.LayoutHandler()->ListNodeBackgroundColor() );
   	// </cmail>
    separatorVisualiser->SetBackgroundBrush( titleDividerBgBrush );
   	
	separatorData->SetIconExpanded( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeExpanded ) );
	separatorData->SetIconCollapsed( iAppUi.FsTextureManager()->TextureByIndex( EListTextureNodeCollapsed ) );
    // <cmail> Platform layout changes
    // textStyle = iAppUi.LayoutHandler()->FSTextStyleFromIdL( EFSFontTypeSmall );
	separatorVisualiser->SetTextStyleId ( textStyle.Id() );
    // </cmail> Platform layout changes
    iNotDownloadedNode = iAttachmentsList->InsertNodeL(*separatorData, *separatorVisualiser, KFsTreeRootID);
	CleanupStack::Pop( separatorVisualiser );
	CleanupStack::Pop( separatorData );    
	iNotDownloadedVisualiser = separatorVisualiser;

	}

void CFSEmailUiAttachmentsListVisualiser::MoveItemToNodeL( TFsTreeItemId aItemId, TFsTreeItemId aTargetNodeId )
    {
    FUNC_LOG;
    TFsTreeItemId origParentNode = iAttachmentsList->Parent( aItemId );
    
    // Make the target node visible
    if ( iAttachmentsList->ItemVisualizer(aTargetNodeId).IsHidden() )
        {
        iAttachmentsTreeListVisualizer->UnhideItemL( aTargetNodeId ); 
        }

    // Expand the target node if it was collapsed. Otherwise the result would
    // be a node which has some hidden and some visible children.
    if ( !iAttachmentsList->IsExpanded(aTargetNodeId) )
        {
        iAttachmentsList->ExpandNodeL(aTargetNodeId);
        }

    iAttachmentsList->MoveItemL( aItemId, aTargetNodeId );          

    // Hide the previous parent of the moved item if it became empty
    if ( iAttachmentsList->IsEmpty(origParentNode) )
        {
        iAttachmentsTreeListVisualizer->HideItemL( origParentNode );
        }
    }

// <cmail> Touch
void CFSEmailUiAttachmentsListVisualiser::TreeListEventL( const TFsTreeListEvent aEvent, 
                                 	const TFsTreeItemId aId,
                                 	const TPoint& /*aPoint*/ )
	{
    FUNC_LOG;

    switch(aEvent)
        {
        case MFsTreeListObserver::EFsTreeItemSelected: //from original code
            TryToOpenItemL( aId );
            break; 
        case MFsTreeListObserver::EFsTreeListItemTouchAction:
            DoHandleActionL(); 
            break; 
        case MFsTreeListObserver::EFsTreeListItemTouchLongTap:
            LaunchActionMenuL();  
            break; 
        case MFsTreeListObserver::EFsTreeListItemTouchFocused:
            SetMskL(); 
            break; 
        default:
            //Just ignore rest of events
            break;             
        }                   
	}

// ---------------------------------------------------------------------------
// CFSEmailUiAttachmentsListVisualiser::DoHandleActionL
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::DoHandleActionL( )
    {
    FUNC_LOG;

    TFsTreeItemId id = iAttachmentsList->FocusedItem(); 
        
    //Expand / Collapse
    if ( iAttachmentsList->IsNode(id) )        
        {
        if( !iAttachmentsList->IsExpanded( id ) )
            {
            iAttachmentsList->ExpandNodeL(id);
            }
        else
            {
            iAttachmentsList->CollapseNodeL(id);
            }
        }
    else //Open
        {
        TryToOpenItemL( id );
        }
  
    SetMskL();
    }

//</cmail>


void CFSEmailUiAttachmentsListVisualiser::LaunchActionMenuL()
	{
    FUNC_LOG;
  	
	TAttachmentData* selectedItem = NULL;
	
	TRAPD( err, selectedItem = &iModel->GetItemL( iAttachmentsList->FocusedItem() ) );		
	
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
		
		// Execute action list and handle the menu command
	    //<cmail> touch
        // Execute action list and handle the menu command
	    TActionMenuCustomItemId itemId = CFSEmailUiActionMenu::ExecuteL( itemList, EFscCustom, 0, this );
	    //</cmail>
	   	
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
			default:
				break;
	  		}
  		}
	}

// ---------------------------------------------------------------------------
// Tells if the given command is available when the focus is on the given item
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiAttachmentsListVisualiser::IsCommandAvailableOnItem( TInt aCommandId, 
                                                                     const TAttachmentData* aFocusedItem /*= NULL*/ ) const
    {
    FUNC_LOG;
    TBool available = EFalse;
    
    const RArray<TAttachmentData>& model = iModel->GetModel();

    switch ( aCommandId )
        {
        case EFsEmailUiCmdOpen:
        case FsEActionAttachmentOpen:
            {
    		if ( aFocusedItem && aFocusedItem->downloadProgress == KComplete )
    			{
    			available = ETrue;
    			}
            }
            break;

        case EFsEmailUiCmdDownload:
        case FsEActionAttachmentDownload:
            {
            if (aFocusedItem)
                {
                TPartData partData = aFocusedItem->partData;
                
                TUint pluginId = partData.iMailBoxId.PluginId().iUid;
                
                TBool isPop3 = ( pluginId == KFsPop3PluginId );
    
                if ( aFocusedItem->downloadProgress == KNone && 
                     !iModel->IsDownloading( *aFocusedItem ) &&
                     !isPop3 )
                    {
                    available = ETrue;
                    }
                }
            }
            break;

        case EFsEmailUiCmdCancelDownload:
        case FsEActionAttachmentCancelDownload:
            {
    		if ( aFocusedItem && iModel->IsDownloading( *aFocusedItem ) )
    			{
    			available = ETrue;
    			}
            }
            break;

        // <cmail> Disabled until there is support in plugins.
        case FsEActionAttachmentClearFetchedContent:
        case EFsEmailUiCmdClearFetchedAttachment:
    		if (  !iEmbeddedMsgMode  && aFocusedItem && !iModel->IsDownloading( *aFocusedItem ) )
    			{
                available = aFocusedItem->downloadProgress > KNone;
    			}
    	    else
	            {
	            available = EFalse;
	            }
            break;
        // </cmail>
        case EFsEmailUiCmdSave:
        case FsEActionAttachmentSave:
            {
            // see if mailbox allows saving of embedded messages
            CFSMailBox* mailbox = iAppUi.GetActiveMailbox();
            
            TBool canSaveMessages( mailbox && mailbox->HasCapability(
            		EFSMboxCapaSupportsSavingOfEmbeddedMessages ) );
            
            // saving message type attachments of embedded messages is blocked
            // due to limitations of the Activesync plugin
    		if ( aFocusedItem && !iModel->IsDownloading( *aFocusedItem ) && 
    			 (!iModel->IsMessage(*aFocusedItem) || !iEmbeddedMsgMode && canSaveMessages) )
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
            if ( model.Count() > 1 )
                {
                available = ETrue;
        		for ( TInt i=0; i<model.Count(); i++)
        			{	
        			if ( iModel->IsDownloading( model[i] ) )
        				{
       					available = EFalse;
       					break;
        				}
        			}
                }
           // In embedded message mode, save all needs to be blocked if there
           // are any message type attachments. This is due to limitations of Activesync plugin.
           if ( available && iEmbeddedMsgMode && iModel->IsThereAnyMessageAttachments() )
               {
               available = EFalse;
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
    			if ( iModel->IsDownloading( model[i] ) )
    				{
    				downloads++;
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
            // Download all is available there is more than one attachments
            // for which "Download" command is available.
            // For POP only "Download All" is available due to protocols's
            // restrictions.
            TInt downloadableFiles = 0;

            TBool popChecked = EFalse;

            TBool isPop3 = EFalse;

            TInt count = model.Count();
            
            for ( TInt i = 0; i < count; i++ )
                {
                const TAttachmentData& item = model[i];
                
                if ( item.downloadProgress == KNone &&
                        !iModel->IsDownloading( item ) )
                    {
                    downloadableFiles++;
                    }

                if ( !popChecked )
                    {
                    TPartData partData = item.partData;
                    
                    TUint pluginId = partData.iMailBoxId.PluginId().iUid;
                    
                    isPop3 = ( pluginId == KFsPop3PluginId );

                    popChecked = ETrue;
                    }
                }

            if ( downloadableFiles > 1 ||
                    ( downloadableFiles > 0 && isPop3 ) )
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

// ---------------------------------------------------------------------------
// Creates the text shown on the second line of the attachment item
// ---------------------------------------------------------------------------
//
HBufC* CFSEmailUiAttachmentsListVisualiser::CreateSecondLineTextLC( const TAttachmentData& aAttachment ) const
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
	HBufC* sizeString = CFSEmailUiAttachmentsListModel::CreateSizeDescLC( aAttachment.fileSize );

    // Download progress
    HBufC* progressString = NULL;
	if ( iModel->IsDownloading( aAttachment ) )
		{
		progressString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_LIST_DOWNLOADING, aAttachment.downloadProgress  );
		}
	else
	    {
	    progressString = KNullDesC().AllocLC();
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
// Attempt to open the given item. Start downloading if the item is not
// downloaded or show note if it's currently being downloaded.
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::TryToOpenItemL( TFsTreeItemId aId )
    {
    FUNC_LOG;
    TAttachmentData* attachment = iModel->GetItem( aId );
    
    if ( attachment )
        {
        if ( iModel->IsDownloading( *attachment ) )
            {
            TFsEmailUiUtility::ShowInfoNoteL( R_FSE_VIEWER_NOTE_ATTACHMENT_DOWNLOADING_PROGRESS );
            }
        else if ( attachment->downloadProgress != KComplete )
            {
            iModel->StartDownloadL( aId );
            }
        else
            {
            TFsEmailUiUtility::OpenAttachmentL( attachment->partData );
            }
        }
    }

// ---------------------------------------------------------------------------
// Show confirmation note and remove contents of given attachment item
// if user accepts. Does nothing if given ID doesn't match an attachment item.
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::RemoveAttachmentContentL( TFsTreeItemId aId )
    {
    FUNC_LOG;
    TAttachmentData* item = iModel->GetItem( aId );
        
    if ( IsCommandAvailableOnItem( EFsEmailUiCmdClearFetchedAttachment, item ) )
        {
        TInt reallyClear = 
            TFsEmailUiUtility::ShowConfirmationQueryL( R_FSE_VIEWER_CLEAR_FETCHED_QUERY );
        if ( reallyClear )
            {
            iModel->RemoveContentL( aId );
            // Move item back to not-yet-downloaded node only if contents were actually
            // removed succesfully
            if ( !iModel->IsCompletelyDownloadedL( *item ) )
                {
                MoveItemToNodeL( aId, iNotDownloadedNode );
                }
            }
        // focused item or proper MSK action for it may have changed
        SetMskL();
        }
    }

// ---------------------------------------------------------------------------
// If there is one or more expanded nodes, collapses all nodes.
// Otherwise expands all nodes.
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::ShortcutCollapseExpandAllToggleL()
    {
    FUNC_LOG;
    TBool collapseAllNodes( EFalse );
    TInt count = iAttachmentsList->CountChildren( KFsTreeRootID );
    for ( TInt i=0 ; i<count ; i++ )
        {
        TFsTreeItemId curId = iAttachmentsList->Child(KFsTreeRootID, i);
        if ( iAttachmentsList->IsNode( curId ) &&
             iAttachmentsList->IsExpanded( curId ) )
            {
            collapseAllNodes = ETrue;
            break;
            }	
        }

    if ( collapseAllNodes )
        {
        iAttachmentsTreeListVisualizer->CollapseAllL();        
        }
    else
        {
       	TFsTreeItemId prevId = iAttachmentsList->FocusedItem(); // the focus may fall out of the screen unless manually reset
        iAttachmentsTreeListVisualizer->ExpandAllL();
    	iAttachmentsTreeListVisualizer->SetFocusedItemL( prevId );    
        }
    }

// ---------------------------------------------------------------------------
// Moves the focus to the topmost item
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::GoToTopL()
    {
    FUNC_LOG;
    TInt topLevelCount = iAttachmentsList->CountChildren( KFsTreeRootID );
    if ( topLevelCount > 1)
        {
        // the item #0 is the non-focusable header item
        TInt indexToFocus = 1;
        TFsTreeItemId topId = iAttachmentsList->Child(KFsTreeRootID, indexToFocus);
        
        // If the top item is invisible, move down until we find a visible item
        while ( iAttachmentsList->ItemVisualizer(topId).IsHidden() && 
                indexToFocus < iAttachmentsList->CountChildren(KFsTreeRootID)-1 )
            {
            indexToFocus++;
            topId = iAttachmentsList->Child(KFsTreeRootID, indexToFocus);
            }

        iAttachmentsTreeListVisualizer->SetFocusedItemL( topId );            
        }
    }

// ---------------------------------------------------------------------------
// Moves the focus to the bottommost item
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::GoToBottomL()
    {
    FUNC_LOG;
    TInt topLevelCount = iAttachmentsList->CountChildren( KFsTreeRootID );
    if ( topLevelCount )
        {
        TInt lastIndex = topLevelCount-1;
        TFsTreeItemId bottomId = iAttachmentsList->Child(KFsTreeRootID, lastIndex);
        // If the bottom item is invisible, move up until we find a visible item
        while ( iAttachmentsList->ItemVisualizer(bottomId).IsHidden() && 
                lastIndex > 0 )
            {
            lastIndex--;
            bottomId = iAttachmentsList->Child(KFsTreeRootID, lastIndex);
            }
        
        // If the last item on top level is an expanded node, navigate to its last child
        if ( iAttachmentsList->IsNode(bottomId) )
            {
            TInt childCount = iAttachmentsList->CountChildren(bottomId);
            if ( childCount && iAttachmentsList->IsExpanded(bottomId) )
                {
                bottomId = iAttachmentsList->Child( bottomId, childCount-1 );
                }
            }

        iAttachmentsTreeListVisualizer->SetFocusedItemL( bottomId );            
        }
    }

// ---------------------------------------------------------------------------
// Sets up the title pane to show the mail title
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::SetupTitlePaneTextL()
    {
    FUNC_LOG;
    // <cmail> UI updates: title text is "Attachments" instead of the mail
    //         subject text.

    // </cmail>
    HBufC* titleText = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_VIEW_TITLE );
    iAppUi.SetTitlePaneTextL( *titleText );
    CleanupStack::PopAndDestroy( titleText );
    }

// ---------------------------------------------------------------------------
// HandleMailBoxEventL
// Mailbox event handler, responds to events sent by the plugin.
// ---------------------------------------------------------------------------
//
void CFSEmailUiAttachmentsListVisualiser::HandleMailBoxEventL( TFSMailEvent aEvent,
    TFSMailMsgId aMailbox, TAny* aParam1, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    FUNC_LOG;
    TFsTreeItemId nullId(0);
    TPartData partData = iModel->GetMessageL( nullId );

    if ( iFirstStartCompleted && aMailbox == partData.iMailBoxId && 
    	 aEvent == TFSEventMailDeleted && aParam1 && iModel ) // Safety, in list events that only concern active mailbox are handled 
        {
		RArray<TFSMailMsgId>* removedEntries = static_cast< RArray<TFSMailMsgId>* >(aParam1);
		for ( TInt i=0 ; i < removedEntries->Count() ; i++ )
			{
			if ( partData.iMessageId == (*removedEntries)[i] )
				{
				TMailListActivationData tmp;	
				tmp.iMailBoxId = iAppUi.GetActiveMailboxId();
				const TPckgBuf<TMailListActivationData> pkgOut( tmp );	
				iAppUi.EnterFsEmailViewL( MailListId, KStartListReturnToPreviousFolder, pkgOut );
				break;
				}			
			}
        }
    }

// <cmail>
// ---------------------------------------------------------------------------
// ActionMenuPosition
// ---------------------------------------------------------------------------
//
TPoint CFSEmailUiAttachmentsListVisualiser::ActionMenuPosition()
    {
    TAlfRealRect focusRect;
    TFsTreeItemId listItemId = iAttachmentsList->FocusedItem();    
    iAttachmentsList->GetItemDisplayRectTarget(listItemId, focusRect);
    return focusRect.iTl;
    }
// </cmail>

