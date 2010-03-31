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
* Description:  Implements model for the attachment list view
*
*/

// SYSTEM INCLUDES
//<cmail>
#include "emailtrace.h"
#include "fstreelist.h"
#include <AknUtils.h>
#include <StringLoader.h>
#include <FreestyleEmailUi.rsg>
#include "cfsmailclient.h"
// <cmail> SF
#include <alf/alfdecklayout.h>
// </cmail>
#include "fstreelist.h"
#include "fstreevisualizerbase.h"
#include "fstreeplainonelineitemdata.h"
#include "fstreeplainonelineitemvisualizer.h"
#include "fstreeplainonelinenodedata.h"
#include "fstreeplainonelinenodevisualizer.h"
#include "fstreeplaintwolineitemdata.h"
#include "fstreeplaintwolineitemvisualizer.h"
//</cmail>

// <cmail> Use layout data instead of hard-coded values
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
// </cmail>


// INTERNAL INCLUDES
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiSendAttachmentsListModel.h"
#include "FreestyleEmailUiSendAttachmentsListControl.h"
#include "FreestyleEmailUiSendAttachmentsListVisualiser.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FreestyleEmailUiUtilities.h"
#include "ncscomposeview.h"
#include "ncsconstants.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModelItem* CFSEmailUiSendAttachmentsListModelItem::NewL( 
    const TDesC& aFileName, 
    TInt aFileSize,
    TFileType aFileType,
    TBool aRemote, 
    TBool aReadOnly )
    {
    FUNC_LOG;
    CFSEmailUiSendAttachmentsListModelItem* self = 
    	CFSEmailUiSendAttachmentsListModelItem::NewLC( 
    		aFileName, 
    		aFileSize,
    		aFileType,
    		aRemote,
    		aReadOnly );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModelItem* CFSEmailUiSendAttachmentsListModelItem::NewLC( 
    const TDesC& aFileName, 
    TInt aFileSize,
    TFileType aFileType,
    TBool aRemote, 
    TBool aReadOnly )
	{
    FUNC_LOG;
    CFSEmailUiSendAttachmentsListModelItem* self = 
    	new (ELeave) CFSEmailUiSendAttachmentsListModelItem( 
    		aFileName, 
			aFileSize,
			aFileType,
			aRemote, 
			aReadOnly );
    CleanupStack::PushL( self );
    return self;
	}

// ---------------------------------------------------------------------------
// c++ constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModelItem::CFSEmailUiSendAttachmentsListModelItem( 
    const TDesC& aFileName, 
    TInt aFileSize,
    TFileType aFileType,
    TBool aRemote, 
    TBool aReadOnly )
   	:iFileName( aFileName ), 
     iFileSize( aFileSize ),
     iRemote( aRemote ),
     iReadOnly( aReadOnly ),
     iFileType( aFileType )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// c++ destructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModelItem::~CFSEmailUiSendAttachmentsListModelItem() 
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// FileName
// ---------------------------------------------------------------------------
//
const TDesC& CFSEmailUiSendAttachmentsListModelItem::FileName() const
    {
    FUNC_LOG;
    return iFileName;
    }

// ---------------------------------------------------------------------------
// FileExtension
// ---------------------------------------------------------------------------
//
TPtrC CFSEmailUiSendAttachmentsListModelItem::FileExtension() const
    {
    FUNC_LOG;
    return TParsePtrC( iFileName ).Ext(); 
    }


// ---------------------------------------------------------------------------
// MailMsgPartId
// ---------------------------------------------------------------------------
//
TFSMailMsgId CFSEmailUiSendAttachmentsListModelItem::MailMsgPartId() const
    {
    FUNC_LOG;
    return iMailMsgPartId;
    }

// ---------------------------------------------------------------------------
// MailMsgPartId
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModelItem::SetMailMsgPartId( 
    const TFSMailMsgId aPartId )
    {
    FUNC_LOG;
    iMailMsgPartId = aPartId;
    }

// ---------------------------------------------------------------------------
// GetFileSize
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiSendAttachmentsListModelItem::FileSize() const
	{
    FUNC_LOG;
	return iFileSize;
	}

// ---------------------------------------------------------------------------
// GetFileSize
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiSendAttachmentsListModelItem::ItemId() const
	{
    FUNC_LOG;
	return iItemId;
	}

// ---------------------------------------------------------------------------
// SetItemId
// Sets Id to item for identification
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModelItem::SetItemId( TFsTreeItemId aItemId )
	{
    FUNC_LOG;
	iItemId = aItemId;
	}

// ---------------------------------------------------------------------------
// IsRemote
// 
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiSendAttachmentsListModelItem::IsRemote()
    {
    FUNC_LOG;
    return iRemote;
    }

// ---------------------------------------------------------------------------
// IsReadOnly
// 
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiSendAttachmentsListModelItem::IsReadOnly()
    {
    FUNC_LOG;
    // As a temporary solution, all remote attachments are made non-removable.
    // Intellisync protocol should support removing also remote attachments
    // but this doesn't seem to work currently in the Intellisync plugin.
    return iReadOnly || iRemote;
    }

// ---------------------------------------------------------------------------
// FileType
// 
// ---------------------------------------------------------------------------
//
TFileType CFSEmailUiSendAttachmentsListModelItem::FileType()
    {
    FUNC_LOG;
    return iFileType;
    }

// ---------------------------------------------------------------------------
// SetRemote
// 
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModelItem::SetRemote( TBool aIsRemote )
    {
    FUNC_LOG;
    iRemote = aIsRemote;
    }


//MODEL CLASS CONSTRUCTION

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModel* CFSEmailUiSendAttachmentsListModel::NewL( 
    CFreestyleEmailUiAppUi* aAppUi,
    CAlfEnv& aEnv,
    CFreestyleEmailUiSendAttachmentsListControl* aControl )
    {
    FUNC_LOG;
    CFSEmailUiSendAttachmentsListModel* self = 
    	CFSEmailUiSendAttachmentsListModel::NewLC( 
            aAppUi,
            aEnv,
            aControl );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModel* CFSEmailUiSendAttachmentsListModel::NewLC( 
    CFreestyleEmailUiAppUi* aAppUi,
    CAlfEnv& aEnv,
    CFreestyleEmailUiSendAttachmentsListControl* aControl )
    {
    FUNC_LOG;
    CFSEmailUiSendAttachmentsListModel* self = 
    	new (ELeave) CFSEmailUiSendAttachmentsListModel( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aControl );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::ConstructL( 
    CAlfEnv& aEnv,
    CFreestyleEmailUiSendAttachmentsListControl* aControl )
    {
    FUNC_LOG;
    iEnv = &aEnv;
    iControl = aControl;
    iAttachmentsList = NULL;
    InitAttachmentListL();
    }

// ---------------------------------------------------------------------------
// c++ constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModel::CFSEmailUiSendAttachmentsListModel( 
    CFreestyleEmailUiAppUi* aAppUi ) 
	: iAppUi( aAppUi )
    {
    FUNC_LOG;
    }
	
// ---------------------------------------------------------------------------
// c++ destructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModel::~CFSEmailUiSendAttachmentsListModel()
    {
    FUNC_LOG;
	while ( Count() > 0 )
        {
        RemoveAndDestroy( 0 );
        }
	// Close list resource.
	iItems.Close();
    delete iAttachmentsList;
    }

// ---------------------------------------------------------------------------
// AppendL
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::AppendL( MFSListModelItem* aItem )
    {
    FUNC_LOG;
	TInt err;
	CFSEmailUiSendAttachmentsListModelItem* item = 
		static_cast<CFSEmailUiSendAttachmentsListModelItem*>( aItem );
	
	err = iItems.Append( item );
	User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// InsertL
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::InsertL( MFSListModelItem* aItem, TInt aIndex)
    {
    FUNC_LOG;
	TInt err;
	CFSEmailUiSendAttachmentsListModelItem* item = 
		static_cast<CFSEmailUiSendAttachmentsListModelItem*>( aItem );
	
	err = iItems.Insert( item, aIndex );
	User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// RemoveAndDestroy
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::RemoveAndDestroy( TInt aIndex )
    {
    FUNC_LOG;
	delete Item( aIndex );
	iItems.Remove( aIndex );
    }

// ---------------------------------------------------------------------------
// Item
// Retuns valid item in model by index
// ---------------------------------------------------------------------------
//
MFSListModelItem* CFSEmailUiSendAttachmentsListModel::Item( TInt aIndex )
	{
    FUNC_LOG;
	TInt numInList = iItems.Count();
	if ( aIndex < 0 || aIndex >= numInList )
        {
        return NULL;
        }
	
	return iItems[aIndex];
	}

// ---------------------------------------------------------------------------
// Count
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiSendAttachmentsListModel::Count() const
    {
    FUNC_LOG;
	return iItems.Count();
    }

// ---------------------------------------------------------------------------
// AppUi
// return appui pointer
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiAppUi* CFSEmailUiSendAttachmentsListModel::AppUi()
	{
    FUNC_LOG;
	return iAppUi;
	}

// ---------------------------------------------------------------------------
// ReFreshList
// Refresh attachment list eg. after item removal
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::ReFreshListL()
	{
    FUNC_LOG;
	// empty the list
	if ( !iAttachmentsList ) 
		{
        // Coverity error fix. iAttachmentsList was used also later without null checks
        return;
		}
	
    iAttachmentsList->RemoveAllL();
    iAttachmentsList->SetFocusedL(ETrue);
    iAttachmentsList->HideListL();
	TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, screenRect );
    iListLayout->SetSize( screenRect.Size() );
    iListLayout->UpdateChildrenLayout();

	// header and dividers
	CreateListSkeletonL();
	// list model items
	CreateListItemsL();

    if ( iAttachmentsList->IsEmpty(iLocalNode) &&
         !iAttachmentsList->ItemVisualizer(iLocalNode).IsHidden() )
        {
        iAttachmentsTreeListVisualizer->HideItemL( iLocalNode );
        }
    if ( iAttachmentsList->IsEmpty(iRemoteNode) &&
         !iAttachmentsList->ItemVisualizer(iRemoteNode).IsHidden() )
        {
        iAttachmentsTreeListVisualizer->HideItemL( iRemoteNode );
        }

    iAttachmentsList->ShowListL();

    // Set the focus on the topmost visible focusable item
    GoToTopL();
	}


// ---------------------------------------------------------------------------
// ReScaleUiL
//
// 
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::ReScaleUiL()
    {
    FUNC_LOG;
    
    // No rescaling to do if the list is empty
    if ( iAttachmentsList->IsEmpty(iLocalNode) && iAttachmentsList->IsEmpty(iRemoteNode) )
        return;
    
    iAttachmentsList->HideListL();
        
    // Reset font heights
    // ------------------
    TInt fontHeight = iAppUi->LayoutHandler()->ListItemFontHeightInTwips();

    // <cmail/> Header item not used - UI update.
	//</cmail>

    // Dividers
    iLocalNodeVisualiser->SetFontHeight( fontHeight );
    iRemoteNodeVisualiser->SetFontHeight( fontHeight );
    
    // Attachment items
    for ( TInt i=0; i<iItems.Count(); i++ )
        {
        MFsTreeItemVisualizer& attachmentVis = iAttachmentsList->ItemVisualizer( iItems[i]->ItemId() );
        attachmentVis.SetFontHeight( fontHeight );
        }       
    
    iAttachmentsList->ShowListL();
    }

// ---------------------------------------------------------------------------
// CreateListSkeletonL
//
// 
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::CreateListSkeletonL()
	{
    FUNC_LOG;
// <cmail> Removed listbox header lines because of modified UI specification.

//</cmail>
    
    CreateListSeparatorL( 
        R_FSE_EDITOR_LOCAL_ATTACHMENTS,
        KFsTreeRootID,
        iLocalNodeVisualiser,
        iLocalNode );

    CreateListSeparatorL( 
        R_FSE_EDITOR_REMOTE_ATTACHMENTS,
        KFsTreeRootID,
        iRemoteNodeVisualiser,
        iRemoteNode );
    
    iAttachmentsList->SetItemsAlwaysExtendedL(ETrue);
    
	}

// ---------------------------------------------------------------------------
// CreateListItemsL
// 
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::CreateListItemsL()
	{
    FUNC_LOG;
	CFSEmailUiSendAttachmentsListModelItem* item;
	TInt count = Count();

	for ( TInt i=0; i < count; ++i )
		{
		item = static_cast<CFSEmailUiSendAttachmentsListModelItem*>( Item( i ) );
		// append item content to Freestyle tree list
		AppendItemToAttachmentListL( item );
        }
	}

// ---------------------------------------------------------------------------
// CreateListSeparatorL
// 
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::CreateListSeparatorL( 
    TInt aResourceId, 
    TFsTreeItemId aParentNodeId,
    CFsTreePlainOneLineNodeVisualizer*& aVisualizer, 
    TFsTreeItemId& aSeparatorNodeId )
    {
    FUNC_LOG;
    // get screen rectangle
	TRect screenRect;
	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );	
	
	// create data and visualizer objects
	CFsTreePlainOneLineNodeData* separatorData = CFsTreePlainOneLineNodeData::NewL();
	CleanupStack::PushL( separatorData );
    CFsTreePlainOneLineNodeVisualizer* separatorVisualiser = CFsTreePlainOneLineNodeVisualizer::NewL(*iAttachmentsList->TreeControl());
    CleanupStack::PushL( separatorVisualiser );
    
    // load resource string and set to separator object
	HBufC* resourceString = StringLoader::LoadLC( aResourceId );
	separatorData->SetDataL( *resourceString );
	CleanupStack::PopAndDestroy( resourceString );

    // set properties of visualizer
    separatorVisualiser->SetSize( TSize( screenRect.Width(), iAppUi->LayoutHandler()->OneLineListNodeHeight() ) );
    separatorVisualiser->SetExtendable( EFalse );
	
    //<cmail>
    //separatorVisualiser->SetFontHeight( iAppUi->LayoutHandler()->ListItemFontHeightInTwips() );		
    //</cmail>
	separatorVisualiser->SetTextBold( ETrue );
	separatorData->SetIconExpanded( iAppUi->FsTextureManager()->TextureByIndex( EListTextureNodeExpanded ) );
	separatorData->SetIconCollapsed( iAppUi->FsTextureManager()->TextureByIndex( EListTextureNodeCollapsed ) );
	
   	TRgb nodeTextColor = iAppUi->LayoutHandler()->ListNodeTextColor();
   	TRgb focusedTextColor = iAppUi->LayoutHandler()->ListFocusedStateTextSkinColor();
   	//<cmail>
   	//TRgb nodeBgColor = iAppUi->LayoutHandler()->ListNodeBackgroundColor();
    //</cmail>
	separatorVisualiser->SetNormalStateTextColor( nodeTextColor );
	separatorVisualiser->SetFocusedStateTextColor( focusedTextColor );
    //<cmail>
	//separatorVisualiser->SetBackgroundColor( nodeBgColor );
    //</cmail>

    aVisualizer = separatorVisualiser;
	CleanupStack::Pop( separatorVisualiser );
	CleanupStack::Pop( separatorData );    
    aSeparatorNodeId = iAttachmentsList->InsertNodeL( *separatorData, *separatorVisualiser, aParentNodeId );
    }

// ---------------------------------------------------------------------------
// UpdateHeaderItemTextL
// Update the attachment count and total size information shown in header
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::UpdateHeaderItemTextL()
    {
    FUNC_LOG;
    // Currently send attachment list headers doesn't contain any data that
    // need to be updated
    }

// ---------------------------------------------------------------------------
// AppendItemToAttachmentList
// Appends item to graphical list from model list
// ---------------------------------------------------------------------------
//
TFsTreeItemId CFSEmailUiSendAttachmentsListModel::AppendItemToAttachmentListL( 
    CFSEmailUiSendAttachmentsListModelItem* aItem )
	{
    FUNC_LOG;
	TFsTreeItemId itemId;
	CFsTreePlainTwoLineItemData* twoLineItemData;
	CFsTreePlainTwoLineItemVisualizer* twoLineItemVisualizer;

	twoLineItemData = CFsTreePlainTwoLineItemData::NewL();
	twoLineItemVisualizer = CFsTreePlainTwoLineItemVisualizer::NewL( *iAttachmentsList->TreeControl() );

	// load localization strings for file size
	HBufC* sizeDesc = TFsEmailUiUtility::CreateSizeDescLC( aItem->FileSize() );
	HBufC* decoratedSize = StringLoader::LoadLC( R_FS_EMAILUI_SENDATTACHMENTS_ITEMSIZE_TXT,
	                                             *sizeDesc );

	// construct item 2nd row text
	HBufC* row2Buffer = HBufC::NewLC( aItem->FileExtension().Length() + KSpace().Length() + decoratedSize->Length() );
	TPtr row2Des = row2Buffer->Des();
	row2Des.Copy( aItem->FileExtension() );
	row2Des.Append( KSpace );
	row2Des.Append( *decoratedSize );
    
    // set data to item
    twoLineItemData->SetDataL( aItem->FileName() );
    twoLineItemData->SetSecondaryDataL( *row2Buffer );
    
    // free resources
    CleanupStack::PopAndDestroy( row2Buffer );
   	CleanupStack::PopAndDestroy( decoratedSize );
   	CleanupStack::PopAndDestroy( sizeDesc );
    
   	// set icon
    CAlfTexture& itemTexture = TFsEmailUiUtility::GetAttachmentIcon( 
        aItem->FileType(), 
        *iAppUi->FsTextureManager() );
    twoLineItemData->SetIcon( itemTexture );
    
    // set item size and properties
	TRect screenRect;
	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );	

	twoLineItemVisualizer->SetSize( TSize(screenRect.Width(), iAppUi->LayoutHandler()->OneLineListItemHeight()) );
    twoLineItemVisualizer->SetExtendedSize( TSize(screenRect.Width(), 2 * iAppUi->LayoutHandler()->OneLineListItemHeight()) );
    twoLineItemVisualizer->SetFlags( twoLineItemVisualizer->Flags()|KFsTreeListItemHasMenu );

    twoLineItemVisualizer->SetExtendable( ETrue );
    twoLineItemVisualizer->SetFocusable( ETrue );

	twoLineItemVisualizer->SetTextBold( EFalse );

   	TRgb normalTextColor = iAppUi->LayoutHandler()->ListNormalStateTextSkinColor();
   	if ( aItem->IsReadOnly() )
   	    {
   	    normalTextColor = iAppUi->LayoutHandler()->ComposerDimmedTextColor();
   	    }
   	TRgb focusedTextColor = iAppUi->LayoutHandler()->ListFocusedStateTextSkinColor();
    twoLineItemVisualizer->SetNormalStateTextColor( normalTextColor );
   	twoLineItemVisualizer->SetFocusedStateTextColor( focusedTextColor );
    
   	// Ensure the parent node of the item to be added is visible and expanded.
    TFsTreeItemId parentNodeId = aItem->IsRemote() ? iRemoteNode : iLocalNode;
    if ( iAttachmentsList->ItemVisualizer(parentNodeId).IsHidden() )
        {
        // Unhiding a node is a relatively heavy-weight operation. Thus, 
        // we need to ensure it's not called in vain.
        iAttachmentsTreeListVisualizer->UnhideItemL( parentNodeId );
        }
    iAttachmentsList->ExpandNodeL( parentNodeId );

    // insert item and catch id returned
    itemId = iAttachmentsList->InsertItemL( 
        *twoLineItemData, 
        *twoLineItemVisualizer, 
        parentNodeId );

    // set list item id to model item
    aItem->SetItemId( itemId );
    return itemId;
	}

// ---------------------------------------------------------------------------
// RemoveItemByIndexL
// Removes item by given index from list
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::RemoveItemByIndexL( const TInt aIndex )
	{
    FUNC_LOG;
	if ( aIndex >= 0 && aIndex < Count() )
		{
		CFSEmailUiSendAttachmentsListModelItem* item =
		    static_cast<CFSEmailUiSendAttachmentsListModelItem*>( Item(aIndex) );
		TFsTreeItemId deletedItemId = item->ItemId();

		// remove attachment from mail message 
		CNcsComposeView* composeView = 
			static_cast<CNcsComposeView*>( iAppUi->View( MailEditorId ) );
			
		if ( composeView->NewMessage() )
			{
			// compose view is open, so remove attachment there
			composeView->NewMessage()->RemoveChildPartL(
				item->MailMsgPartId() );
			if ( iItems.Count() == 1 )
				{
					composeView->NewMessage()->ResetFlag( EFSMsgFlag_Attachments );
				}
			}
		else
			{
			CFSEmailUiSendAttachmentsListVisualiser* parentView = 
				static_cast<CFSEmailUiSendAttachmentsListVisualiser*>(
                    iAppUi->View( SendAttachmentMngrViewId ) );
			
            // Open message from framework and delete attachment
            CFSMailClient* mailClient = iAppUi->GetMailClient(); // not owned
			CFSMailMessage* msg = mailClient->GetMessageByUidL( 
                parentView->EditorParams().iMailboxId, 
                parentView->EditorParams().iFolderId, 
                parentView->EditorParams().iMsgId, 
                EFSMsgDataStructure );
			CleanupStack::PushL( msg );
			msg->RemoveChildPartL( item->MailMsgPartId() );
			msg->ResetFlag( EFSMsgFlag_Attachments );
			msg->SaveMessageL();
			CleanupStack::PopAndDestroy( msg );
			}
		
		// Remove attachment item from model
		RemoveAndDestroy( aIndex );

		// Remove attachment item from tree list if present
		if ( deletedItemId != KFsTreeNoneID )
		    {
    		iAttachmentsList->RemoveL( deletedItemId );
    		
    		// Hide parent node of the removed item if it just became empty
    	    if ( iAttachmentsList->IsEmpty(iLocalNode) &&
    	         !iAttachmentsList->ItemVisualizer(iLocalNode).IsHidden() )
    	        {
    	        iAttachmentsTreeListVisualizer->HideItemL( iLocalNode );
    	        }
    	    if ( iAttachmentsList->IsEmpty(iRemoteNode) &&
    	         !iAttachmentsList->ItemVisualizer(iRemoteNode).IsHidden() )
    	        {
    	        iAttachmentsTreeListVisualizer->HideItemL( iRemoteNode );
    	        }
		    }
		}	
	}

// ---------------------------------------------------------------------------
// RemoveAllAttachmentsL
// Removes all attachment items from the model, list, and message
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::RemoveAllAttachmentsL()
    {
    FUNC_LOG;
    CNcsComposeView* composeView = 
        static_cast<CNcsComposeView*>( iAppUi->View(MailEditorId) );
    CFSMailMessage* msg = composeView->NewMessage();
        
    if ( !msg )
        {
        // Open message from framework
        CFSEmailUiSendAttachmentsListVisualiser* attListView = 
            static_cast<CFSEmailUiSendAttachmentsListVisualiser*>(
                    iAppUi->View( SendAttachmentMngrViewId ) );
        CFSMailClient* mailClient = iAppUi->GetMailClient(); // not owned
        msg = mailClient->GetMessageByUidL( 
                attListView->EditorParams().iMailboxId, 
                attListView->EditorParams().iFolderId, 
                attListView->EditorParams().iMsgId, 
                EFSMsgDataStructure );
        CleanupStack::PushL( msg );
        }

    // remove attachments from mail message 
    for ( TInt i = Count()-1 ; i >= 0 ; --i )
        {
        CFSEmailUiSendAttachmentsListModelItem* item =
            static_cast<CFSEmailUiSendAttachmentsListModelItem*>( Item(i) );
    
        TRAPD( err, msg->RemoveChildPartL( item->MailMsgPartId() ) );
        // Remove attachment item from model if message part was succesfully removed
        if ( !err )
            {
            RemoveAndDestroy( i );
            }
        }
    msg->ResetFlag( EFSMsgFlag_Attachments );
    msg->SaveMessageL();

    // Delete msg if we own it
    if ( !composeView->NewMessage() )
        {
        CleanupStack::PopAndDestroy( msg );
        }
    
    // Update the tree list to match the model.
    ReFreshListL();
    }   

// ---------------------------------------------------------------------------
// HighlightedIndex
// Returns currenlty higlighted index from list
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiSendAttachmentsListModel::HighlightedIndex()
	{
    FUNC_LOG;
	TInt ret( KErrNotFound );
	
	TFsTreeItemId focusedId = iAttachmentsList->FocusedItem();
	// Map id to the index in model

	for ( TInt i=0; i < iItems.Count(); ++i )
		{
		if ( focusedId == iItems[i]->ItemId() )
			{
			ret = i;
			break;
			}
		}
	return ret;
	}

// ---------------------------------------------------------------------------
// AttachmentList
// Returns pointer to attachment list
// ---------------------------------------------------------------------------
//
CFsTreeList* CFSEmailUiSendAttachmentsListModel::AttachmentList()
	{
    FUNC_LOG;
	return iAttachmentsList;
	}

// ---------------------------------------------------------------------------
// TotalAttachmentSize
//
// Retuns sum of all attachments sizes in kB from the model
// ---------------------------------------------------------------------------
//
TInt CFSEmailUiSendAttachmentsListModel::TotalAttachmentSize()
	{
    FUNC_LOG;
	CFSEmailUiSendAttachmentsListModelItem* fileItem;
	TInt totalSize = 0;
	TInt count = Count();
	
	for ( TInt i=0; i < count; ++i )
		{
		fileItem = static_cast<CFSEmailUiSendAttachmentsListModelItem*>( Item( i ) );
		totalSize += fileItem->FileSize();
		}
	return totalSize;
	}

// ---------------------------------------------------------------------------
// InitAttachmentListL
//
// Creates new instance of attachment tree list
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::InitAttachmentListL()
	{
    FUNC_LOG;
	
	if ( iAttachmentsList == NULL )
		{

		/*TRect screenRect;
	 	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );	
		iListLayout = CAlfDeckLayout::AddNewL( *iControl );  
	    iListLayout->SetSize( TSize( screenRect.Width(), screenRect.Height()) );*/
	
	    TRect screenRect;
	    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, screenRect );
        iListLayout = CAlfDeckLayout::AddNewL( *iControl );  
	    iListLayout->SetSize( screenRect.Size() );
	    
	   	// Create attachments list tree list component 
	    iAttachmentsTreeListVisualizer = CFsTreeVisualizerBase::NewL( 
            iControl, 
            *iListLayout );
	   	iAttachmentsTreeListVisualizer->SetItemExpansionDelay( KListItemExpansionDelay );
	    //<cmail> S60 skin support
	    //iAttachmentsTreeListVisualizer->SetBackgroundTextureL( 
        //    iAppUi->FsTextureManager()->TextureByIndex( EBackgroundTextureMailList ) );
	    //</cmail>
	   	iAttachmentsTreeListVisualizer->SetMenuIcon( 
            iAppUi->FsTextureManager()->TextureByIndex( EListControlMenuIcon ) );		
  	    // Set page up and page down keys
	   	iAttachmentsTreeListVisualizer->AddCustomPageUpKey( EStdKeyPageUp );
	   	iAttachmentsTreeListVisualizer->AddCustomPageDownKey( EStdKeyPageDown );

	  	// create list
	    iAttachmentsList = CFsTreeList::NewL( *iAttachmentsTreeListVisualizer, *iEnv );
	    iAttachmentsList->SetScrollbarVisibilityL( EFsScrollbarAuto );
	  	iAttachmentsList->SetMarkTypeL( CFsTreeList::EFsTreeListMultiMarkable );	
	  	iAttachmentsList->SetLoopingType( EFsTreeListLoopingDisabled );
	    iAttachmentsList->SetIndentationL( 0 );
	  	iAttachmentsList->AddObserverL( *this );
	    iAppUi->LayoutHandler()->SetListMarqueeBehaviour( iAttachmentsList );

	  	CreateListSkeletonL();
		}

	}

//<cmail> touch

// ---------------------------------------------------------------------------
// TreeListEventL
//
// ---------------------------------------------------------------------------
//

void CFSEmailUiSendAttachmentsListModel::TreeListEventL( 
    const TFsTreeListEvent aEvent, 
    const TFsTreeItemId /*aId*/,
    const TPoint& /*aPoint*/ )
	{
    FUNC_LOG;
    
    CFSEmailUiSendAttachmentsListModelItem* item = 0;
    
    switch(aEvent)
        {
        case MFsTreeListObserver::EFsTreeListItemTouchAction:
            DoHandleActionL(); 
            break; 
        case MFsTreeListObserver::EFsTreeListItemTouchLongTap:
            item = GetSelectedItem();
            if(NULL != item)
                iControl->ShowActionMenuL( item ); 
            break; 
        case MFsTreeListObserver::EFsTreeListItemTouchFocused:
            iControl->SetMskL(); 
            break; 
        default:
            //Just ignore rest of events
            break;             
        }                   
              
	}

// ---------------------------------------------------------------------------
// Handles action for focused item.
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::HandleActionL()
    {
    FUNC_LOG;
    DoHandleActionL();
    }

// ---------------------------------------------------------------------------
// CFSEmailUiSendAttachmentsListModel::DoHandleActionL
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::DoHandleActionL( )
    {
    FUNC_LOG;

    TFsTreeItemId id = iAttachmentsList->FocusedItem(); 
    
    //Expand / Collapse
    if ( iAttachmentsList->IsNode(id) )
        {
        if ( iAttachmentsList->IsExpanded(id) )
            {
            CollapseL(); 
            }
        else
            {
            ExpandL(); 
            }
        }
    else //Open
        {
        iControl->OpenHighlightedFileL();
        }
  
    iControl->SetMskL();
    }

//</cmail>




// ---------------------------------------------------------------------------
// HasRemoteAttachments
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiSendAttachmentsListModel::HasRemoteAttachments()
    {
    FUNC_LOG;
    // remote files have no file path within local system
    for ( TInt i=0; i<iItems.Count(); i++ )
        {
        if ( iItems[i]->IsRemote() )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// HasReadOnlyAttachments
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiSendAttachmentsListModel::HasReadOnlyAttachments()
    {
    FUNC_LOG;
    // remote files have no file path within local system
    for ( TInt i=0; i<iItems.Count(); i++ )
        {
        if ( iItems[i]->IsReadOnly() )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// GetSelectedItem
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModelItem* CFSEmailUiSendAttachmentsListModel::GetSelectedItem()
    {
    FUNC_LOG;
    TFsTreeItemId nodeId = iAttachmentsList->FocusedItem();
	//<cmail>
    if ( nodeId == iLocalNode ||
         nodeId == iRemoteNode ||
         nodeId == KFsTreeNoneID )
	//</cmail>		 
        {
        return NULL;
        }
    else
        {
        return GetItemByNodeId( nodeId );
        }
    }

// ---------------------------------------------------------------------------
// GetItemByNodeId
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModelItem* CFSEmailUiSendAttachmentsListModel::GetItemByNodeId( TFsTreeItemId aNodeId )
    {
    FUNC_LOG;
    for ( TInt i=0; i<iItems.Count(); i++ )
        {
        if ( iItems[i]->ItemId() == aNodeId )
            {
            return iItems[i];
            }
        }
    return NULL;
    }

TBool CFSEmailUiSendAttachmentsListModel::CanSelectPreviousItem()
    {
    FUNC_LOG;
    return iAttachmentsList->FocusedItem() != iLocalNode;
    }

// ---------------------------------------------------------------------------
// Moves the focus to the topmost item
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::GoToTopL()
    {
    FUNC_LOG;
    // Focus first visible focusable item.
    TFsTreeItemId topId = KErrNotFound;

    const TInt count = iAttachmentsList->CountChildren( KFsTreeRootID );
    for ( TInt index = 0; index < count && topId == KErrNotFound; ++index )
        {
        TFsTreeItemId id =iAttachmentsList->Child( KFsTreeRootID, index );
        if ( !iAttachmentsList->ItemVisualizer( id ).IsHidden() )
            {
            topId = id;
            }
        }

    if ( topId != KErrNotFound )
        {
        iAttachmentsTreeListVisualizer->SetFocusedItemL( topId );
        }
    }

// ---------------------------------------------------------------------------
// Moves the focus to the bottommost item
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::GoToBottomL()
    {
    FUNC_LOG;
    if ( Count() )
        { 
        TInt topLevelCount = iAttachmentsList->CountChildren( KFsTreeRootID );
        if ( topLevelCount )
            {
            // Get last visible top level item 
            TFsTreeItemId bottomId = KErrNotFound;
            TInt index = topLevelCount - 1;
            do
                {
                bottomId = iAttachmentsList->Child( KFsTreeRootID, index );
                index--;
                }
            while ( iAttachmentsList->ItemVisualizer(bottomId).IsHidden() && 
                    index >= 0 );
                
            if ( iAttachmentsList->IsNode(bottomId) )
                {
                TInt childCount = iAttachmentsList->CountChildren(bottomId);
                if ( childCount && iAttachmentsList->IsExpanded(bottomId) )
                    {
                    // Focus the last child of the bottom node if the node is expanded.
                    bottomId = iAttachmentsList->Child( bottomId, childCount-1 );
                    }
                }
            iAttachmentsTreeListVisualizer->SetFocusedItemL( bottomId );            
            }
        }
    }

// ---------------------------------------------------------------------------
// Collapse nodes
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::CollapseL()
    {
    FUNC_LOG;
    TFsTreeItemId focId1 = iAttachmentsList->FocusedItem();
    iAttachmentsList->CollapseNodeL(focId1);
    }
// ---------------------------------------------------------------------------
// Expands nodes
// ---------------------------------------------------------------------------
//            
void CFSEmailUiSendAttachmentsListModel::ExpandL()
    {
    FUNC_LOG;
    TFsTreeItemId focId = iAttachmentsList->FocusedItem();
    iAttachmentsList->ExpandNodeL( focId );
    }
// ---------------------------------------------------------------------------
// Collapses or expands all nodes
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::CollapseExpandAllToggleL()
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
// 
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::Clear()
    {
    FUNC_LOG;
    // <cmail> Remove all children of nodes on the list
    if ( iAttachmentsList )
        {
        TRAP_IGNORE( iAttachmentsList->RemoveChildrenL( iRemoteNode ) );
        TRAP_IGNORE( iAttachmentsList->RemoveChildrenL( iLocalNode ) );
        }
    // </cmail>
    iItems.ResetAndDestroy();
    }

//<cmail>
// ---------------------------------------------------------------------------
// CFSEmailUiSendAttachmentsListModel::GetVisualizer
// ---------------------------------------------------------------------------
//
CFsTreeVisualizerBase* CFSEmailUiSendAttachmentsListModel::GetVisualizer()
    {
    FUNC_LOG;
    return iAttachmentsTreeListVisualizer;
    }
//</cmail>

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAlfLayout* CFSEmailUiSendAttachmentsListModel::GetParentLayout() const
    {
    return iListLayout;
    }


// ---------------------------------------------------------------------------
// Sets up the title pane
// ---------------------------------------------------------------------------
//
void CFSEmailUiSendAttachmentsListModel::SetupTitlePaneTextL()
    {
    FUNC_LOG;
    HBufC* titleText = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_VIEW_TITLE );
    AppUi()->SetTitlePaneTextL( *titleText );
    CleanupStack::PopAndDestroy( titleText );
    }

//</cmail>

