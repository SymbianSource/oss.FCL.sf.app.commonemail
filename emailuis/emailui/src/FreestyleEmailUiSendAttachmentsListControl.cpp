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
* Description:  Implements control for the attachment list view
*
*/


// SYSTEM INCLUDES
#include "emailtrace.h"
#include <pathinfo.h>
//<cmail>
#include "fstreelist.h" // for attachment visual list
#include <DocumentHandler.h> // for file opening
#include <apmstd.h> // for TDataType
#include <gulicon.h> // icons
#include <AknIconUtils.h> // icons
#include <AknWaitDialog.h>
#include <freestyleemailui.mbg>
#include <FreestyleEmailUi.rsg>
#include <CommonContentPolicy.h>
#include <aknnotewrappers.h> 
#include <StringLoader.h>
#include "cfsmailclient.h"
#include <CVPbkContactManager.h> // Virtual phonebook contact manager
// <cmail> SF
#include <alf/alfevent.h>
// </cmail>

// for action menu
#include "cfsccontactactionservice.h"
#include "cfsccontactactionmenu.h"
#include "mfsccontactactionmenumodel.h"
#include "cfsccontactactionmenuitem.h"
#include "fsccontactactionmenuuids.hrh"
#include "fsccontactactionmenudefines.h"
//</cmail>

// LOCAL INCLUDES
#include "FreestyleEmailUiSendAttachmentsListControl.h"
#include "FreestyleEmailUiSendAttachmentsListVisualiser.h"
#include "FreestyleEmailUiSendAttachmentsListModel.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiAttachmentsListModel.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "ncscomposeview.h"


// CONSTANTS

// CLASS IMPLEMENTATION

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiSendAttachmentsListControl* CFreestyleEmailUiSendAttachmentsListControl::NewL(
        CAlfEnv& aEnv,
		CFSEmailUiSendAttachmentsListVisualiser* aVisualiser,
		CFreestyleEmailUiAppUi* aAppUi )
    {
    FUNC_LOG;
    CFreestyleEmailUiSendAttachmentsListControl* self = 
    	CFreestyleEmailUiSendAttachmentsListControl::NewLC( aEnv, aVisualiser, aAppUi );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiSendAttachmentsListControl* CFreestyleEmailUiSendAttachmentsListControl::NewLC(
        CAlfEnv& aEnv,
		CFSEmailUiSendAttachmentsListVisualiser* aVisualiser,
		CFreestyleEmailUiAppUi* aAppUi )
    {
    FUNC_LOG;
    CFreestyleEmailUiSendAttachmentsListControl* self = 
    	new (ELeave) CFreestyleEmailUiSendAttachmentsListControl( aVisualiser, aAppUi );
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    return self;
    }

// ---------------------------------------------------------------------------
//C++ constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiSendAttachmentsListControl::CFreestyleEmailUiSendAttachmentsListControl(
		CFSEmailUiSendAttachmentsListVisualiser* aVisualiser,
		CFreestyleEmailUiAppUi* aAppUi )
    : CAlfControl(), 
	iVisualiser( aVisualiser ),
	iAppUi( aAppUi )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiSendAttachmentsListControl::ConstructL( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    CAlfControl::ConstructL( aEnv );
	iService = CFscContactActionService::NewL( iAppUi->GetVPbkManagerL() );
    iModel = CFSEmailUiSendAttachmentsListModel::NewL( iAppUi, Env(), this );
    }

// ---------------------------------------------------------------------------
// Virtual destructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiSendAttachmentsListControl::~CFreestyleEmailUiSendAttachmentsListControl()
    {
    FUNC_LOG;
	delete iService;
    }

// <cmail>
// ---------------------------------------------------------------------------
// Verify file selection (e.g. DRM)
// ---------------------------------------------------------------------------
//
TBool CFreestyleEmailUiSendAttachmentsListControl::VerifySelectionL( const MDesCArray* /*aSelectedFiles*/ )
    {
    return ETrue;
    }
// </cmail>

void CFreestyleEmailUiSendAttachmentsListControl::DeleteModel()
	{
    FUNC_LOG;
	delete iModel;
	iModel = NULL;
	}

//<cmail> touch
void CFreestyleEmailUiSendAttachmentsListControl::SetMskL()
    { 
    iVisualiser->SetMskL();
    }
//</cmail>

// ---------------------------------------------------------------------------
// OfferEventL
// called by ALF
// ---------------------------------------------------------------------------
//
TBool CFreestyleEmailUiSendAttachmentsListControl::OfferEventL( const TAlfEvent& aEvent )
	{
    FUNC_LOG;
    TBool result( EFalse );
    iVisualiser->SetMskL();
    // handle key events
    if ( aEvent.IsKeyEvent() && aEvent.Code() == EEventKey )
    	{

	    TInt scanCode = aEvent.KeyEvent().iScanCode;
	    // Swap right and left controls in mirrored layout
	    if ( AknLayoutUtils::LayoutMirrored() )
	        {
	        if (scanCode == EStdKeyRightArrow)
	            {
	            scanCode = EStdKeyLeftArrow;
	            }
	        else if ( scanCode == EStdKeyLeftArrow )
	            {
	            scanCode = EStdKeyRightArrow;
	            }
	        }

        switch ( scanCode )
            {
            case EStdKeyDevice3:
            case EStdKeyEnter:
                {
                iModel->HandleActionL();
                result = ETrue;
                break;
                }
	    	case EStdKeyUpArrow:
	    		{
	    		// do not highlight header
	    		if ( !iModel->CanSelectPreviousItem() )
	    			{
	    			// Check that this works correctly!
	    			// If previous item is header, mark the event consumed
	    			// so that it is not forwarded to list's control.
	    			result = ETrue;
	    			//result = iModel->AttachmentList()->TreeControl()->OfferEventL( aEvent );
	    			}
	    		break;
	    		}
	    	case EStdKeyLeftArrow:
	    		//hide action menu (automatic)
	    		break;
	    	case EStdKeyRightArrow:
		    	{
	    		// show action menu when we are on list item
	    		CFSEmailUiSendAttachmentsListModelItem* item = NULL;
	    		
	    		item = iModel->GetSelectedItem();
	    		
	    		if ( item )
	    			{
	    			ShowActionMenuL( item );
	    			}
	    			
	    		break;
		    	}
	    	default :
	       	    // Check keyboard shortcuts.
	       	    TInt shortcutCommand = 
	       	        iAppUi->ShortcutBinding().CommandForShortcutKey( aEvent.KeyEvent(),
	       	                                                         CFSEmailUiShortcutBinding::EContextSendAttachmentList );
	       	    if ( shortcutCommand != KErrNotFound )
	       	        {
       	            HandleCommandL( shortcutCommand );
	       	        result = ETrue;
	       	        }
	       	    /*
	       	    else
	       	        {
    	    		result = iModel->AttachmentList()->TreeControl()->OfferEventL( aEvent );
	       	        }
	       	    */
	    		break;
	    	}
    	}
    else if (aEvent.IsPointerEvent())
        {
        result = iModel->AttachmentList()->TreeControl()->OfferEventL(aEvent);
        }
    
    return result;
	}

// ---------------------------------------------------------------------------
// HandleCommandL
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiSendAttachmentsListControl::HandleCommandL( TInt aCommand )
	{
    FUNC_LOG;
    switch ( aCommand )
        {
        case EAknSoftkeyOpen:
        case ESendAttachmentMenuOpen:
            OpenHighlightedFileL();
        break;
        case EFsEmailUiCmdGoToTop:
            {
  		    iModel->GoToTopL();
            }
	    break;
  		case EFsEmailUiCmdGoToBottom:
  		    {
  		    iModel->GoToBottomL();
  		    }
	    break;
  		case EFsEmailUiCmdCollapse:
  		    {
  		    iModel->CollapseL();
  		    }
  		    break;
   		case EFsEmailUiCmdExpand:
  		    {
  		    iModel->ExpandL();
  		    }
	    break;
        case EFsEmailUiCmdActionsCollapseExpandAllToggle:
            {
            iModel->CollapseExpandAllToggleL();
            }
        break;
  		default:
  		    {
        	if ( iVisualiser ) // Offer aCommand to view
        		{
        		return iVisualiser->HandleCommandL( aCommand );
        		}
  		    }
       	break;
        }
	}

// ---------------------------------------------------------------------------
// LoadModelContent
//
// Load model content for list
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiSendAttachmentsListControl::LoadModelContentL()
	{
    FUNC_LOG;
	// Check if saved mail has attachments included

	}

// ---------------------------------------------------------------------------
// Model
//
// return pointer to current item model
// ---------------------------------------------------------------------------
//
CFSEmailUiSendAttachmentsListModel* CFreestyleEmailUiSendAttachmentsListControl::Model()
{
    FUNC_LOG;
	return iModel;
}

// ---------------------------------------------------------------------------
// AppendAttachmentToListL
//
// Open files selection dialog and append given filename to attachment list
// ---------------------------------------------------------------------------
//
TBool CFreestyleEmailUiSendAttachmentsListControl::AppendAttachmentToListL(MsgAttachmentUtils::TMsgAttachmentFetchType aType)
	{
    FUNC_LOG;
	TBool ret = EFalse;
	TFileName filePath;
	// <cmail>
	TBool fetchOK = EFalse;
	
	if (aType != MsgAttachmentUtils::EUnknown)
	    {
        CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
                    
        CDesCArrayFlat* files = new( ELeave ) CDesC16ArrayFlat( 1 );
        CleanupStack::PushL( files );
        
        // MsgAttachmentUtils messes up the display area, save it and set back after the call
        TRect visibleArea = CAlfEnv::Static()->PrimaryDisplay().VisibleArea();
        fetchOK = MsgAttachmentUtils::FetchFileL( aType,
                                                        *files,
                                                        paramList,
                                                        EFalse, // disk space checked in VerifySelectionL or CUniEditorInsertOperation, if image scaled/compressed
                                                        EFalse,
                                                        this );
        CAlfEnv::Static()->PrimaryDisplay().SetVisibleArea(visibleArea);
        
        if (files->MdcaCount() > 0)
            {
            filePath = files->MdcaPoint( 0 );
            //when the filepath contains the Drive letter (ie. "e:\\") in lower case then RecognizeData of class RApaLsSession fails.
            //RecognizeData is used by FS framework to detect the content type of the attachment. To fix this possible failure,
            //we change the drive letter to upper case here.
            TBuf<1> driveLetter;
            driveLetter=filePath.Left(1);
            driveLetter.CopyUC(driveLetter);
            filePath.Replace(0,1,driveLetter);
            }
        CleanupStack::PopAndDestroy(2, paramList);
	    }
	else
	    {
	    fetchOK = TFsEmailUiUtility::ShowSelectFileDialogL( filePath );
	    }
        
	if ( fetchOK && filePath.Length() > 0 ) // </cmail>
		{
        if ( !FileDrmProtectedL( filePath ) )
			{
			// add file as a email message part
	    	CFSMailMessagePart* msgPart = NULL;
	    	
	    	CNcsComposeView* composeView = 
	    		static_cast<CNcsComposeView*>( iAppUi->View(MailEditorId) );
	    		
    		TFSMailMsgId nullId;
	    	if ( composeView->NewMessage() )
	    		{
	    		// use composeview NewMessage if available
				msgPart = composeView->NewMessage()->AddNewAttachmentL( filePath, nullId );
	    		}
	    	else
	    		{
				// Open message from framework and append attachment
				CFSMailClient* mailClient = iAppUi->GetMailClient(); // not owned
				CFSMailMessage* msg = mailClient->GetMessageByUidL( 
				        iVisualiser->EditorParams().iMailboxId, 
				        iVisualiser->EditorParams().iFolderId, 
				        iVisualiser->EditorParams().iMsgId, 
						EFSMsgDataStructure );
				CleanupStack::PushL( msg );
				msgPart = msg->AddNewAttachmentL( filePath, nullId );
				CleanupStack::PopAndDestroy( msg );
	    		}
			CleanupStack::PushL( msgPart );
			msgPart->SaveL();
			
			// append file to list model
			const TPtrC fileName = TParsePtrC( filePath ).NameAndExt();
   			TFileType fileType = TFsEmailUiUtility::GetFileType( fileName, msgPart->GetContentType() );
			AppendFileToModelL( msgPart->GetPartId(), 
			                    fileName,
			                    msgPart->ContentSize(), 
			                    fileType );

			// free resources
			CleanupStack::PopAndDestroy( msgPart );

		    // add new tree list item and update header content
			CFSEmailUiSendAttachmentsListModelItem* newItem = 
			    static_cast<CFSEmailUiSendAttachmentsListModelItem*>( iModel->Item( iModel->Count()-1 ) );
			iModel->AppendItemToAttachmentListL( newItem );
			iModel->UpdateHeaderItemTextL();
			
			ret = ETrue;
			}
		}
	return ret;
	}

// ---------------------------------------------------------------------------
// FileDrmProtectedL
//
// Check if file is DRM protected
// ---------------------------------------------------------------------------
//
TBool CFreestyleEmailUiSendAttachmentsListControl::FileDrmProtectedL( RFile& aFile )
	{
    FUNC_LOG;
	TBool isProtected( EFalse );
	CCommonContentPolicy* ccp = CCommonContentPolicy::NewLC();	
	isProtected = ccp->IsClosedFileL( aFile );
	if ( isProtected )
		{
		TFsEmailUiUtility::ShowErrorNoteL( R_FS_EMAILUI_SENDATTACHMENTS_ITEM_DRM_PROTECTED_TXT, ETrue );
		}
	CleanupStack::PopAndDestroy( ccp );
	return isProtected;
	}

// ---------------------------------------------------------------------------
// FileDrmProtectedL
// 
// Check if file is DRM protected
// ---------------------------------------------------------------------------
//
TBool CFreestyleEmailUiSendAttachmentsListControl::FileDrmProtectedL( const TDesC& aFilePath )
		{
    TBool isProtected( EFalse );
		
    RFile file;
    TInt err = file.Open( CCoeEnv::Static()->FsSession(), 
                          aFilePath, 
                          EFileRead | EFileShareAny );
    User::LeaveIfError( err );
    CleanupClosePushL( file );
    isProtected = FileDrmProtectedL( file );
    CleanupStack::PopAndDestroy( &file );
		
    return isProtected;
	}

// ---------------------------------------------------------------------------
// AppendFileToModelL
//
// Append given file to attachment list model
// 
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiSendAttachmentsListControl::AppendFileToModelL( 
		const TFSMailMsgId aPartId, 
        const TDesC& aFileName,
		TInt aSize,
        TFileType aFileType /*= EUnidentifiedType*/,
        TBool aReadOnly /*= EFalse*/,
        TBool aRemoteFile /*= EFalse*/ )
	{
    FUNC_LOG;
	// add item to model
	CFSEmailUiSendAttachmentsListModelItem* newItem = 
		CFSEmailUiSendAttachmentsListModelItem::NewL( 
		        aFileName, 
				aSize,
				aFileType,
				aRemoteFile,
				aReadOnly );
	newItem->SetMailMsgPartId( aPartId );
	
	iModel->AppendL( newItem );
	}

// ---------------------------------------------------------------------------
// RemoveAttachmentFromListL
//
// removes item by given index
// 	or
// removes currently higlighted item from attachment list and model
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiSendAttachmentsListControl::RemoveAttachmentFromListL( 
		const TInt aIndex )
	{
    FUNC_LOG;
	TInt index = ( aIndex < 0 ? iModel->HighlightedIndex() : aIndex );

	if ( index >= 0 )
	    {
	    CFSEmailUiSendAttachmentsListModelItem* item =
            static_cast<CFSEmailUiSendAttachmentsListModelItem*>( iModel->Item( index ) );

        if ( !item->IsReadOnly() )
		    {
            iModel->RemoveItemByIndexL( index );
	        iModel->UpdateHeaderItemTextL();
	        }
    	}
	}

// ---------------------------------------------------------------------------
// RemoveAllAttachmentsL
//
// removes all items from attachment list and model
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiSendAttachmentsListControl::RemoveAllAttachmentsL()
	{
    FUNC_LOG;
    TFsEmailUiUtility::ShowWaitNoteL( iWaitNote, R_FSE_WAIT_REMOVING_TEXT, EFalse );
	iModel->RemoveAllAttachmentsL();
	iWaitNote->ProcessFinishedL();
	}

// ---------------------------------------------------------------------------
// OpenHighlightedFile
//
// Opens highklighted list item with its corresponding handler
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiSendAttachmentsListControl::OpenHighlightedFileL()
	{
    FUNC_LOG;
	TInt index = iModel->HighlightedIndex();
	if ( index >= 0 )
	    {
	    CFSEmailUiSendAttachmentsListModelItem* item =
            static_cast<CFSEmailUiSendAttachmentsListModelItem*>( iModel->Item( index ) );

        if ( !item->IsRemote() )
            {
            // Open message using the IDs
            TEditorLaunchParams editorParams = iVisualiser->EditorParams();
            TPartData attachmentPart( editorParams.iMailboxId, 
                editorParams.iFolderId, 
                editorParams.iMsgId,
                item->MailMsgPartId() );
            TFsEmailUiUtility::OpenAttachmentL( attachmentPart );
            }
    	}
	}

// ---------------------------------------------------------------------------
// ShowActionMenu
//
// Displays action menu and handles selection
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiSendAttachmentsListControl::ShowActionMenuL( CFSEmailUiSendAttachmentsListModelItem* aItem )
	{
    FUNC_LOG;
	// Remove old items from action menu
	CFSEmailUiActionMenu::RemoveAllL();

	// Construct item list
	RFsEActionMenuIdList itemList;
	CleanupClosePushL( itemList );

	if ( !aItem->IsRemote() )
	    {
	    itemList.AppendL( FsEActionAttachmentOpen );
	    }

	itemList.AppendL( FsEActionAttachmentAdd );
	
	if ( !aItem->IsReadOnly() )
	    {
	    itemList.AppendL( FsEActionAttachmentRemove );
	    }
	
	if ( iModel->Count() > 1 && !iModel->HasReadOnlyAttachments() )
	    {
	    itemList.AppendL( FsEActionAttachmentRemoveAll );         
	    }


	//<cmail> touch        
    // Execute action list
    TActionMenuCustomItemId itemId = CFSEmailUiActionMenu::ExecuteL( itemList, EFscCustom, 0, this );
    //</cmail>
	CleanupStack::PopAndDestroy( &itemList );

	// Handle action menu selection
	switch ( itemId )
	    {
	    case FsEActionAttachmentOpen:
	        HandleCommandL( ESendAttachmentMenuOpen );
	        break;
	    case FsEActionAttachmentRemove:
            HandleCommandL( ESendAttachmentMenuRemoveAttachment );
	        break;
	    case FsEActionAttachmentRemoveAll:
            HandleCommandL( ESendAttachmentMenuRemoveAllAttachments );
	        break;
	    case FsEActionAttachmentAdd:
	        HandleCommandL( ESendAttachmentMenuAddAttachment );
	        break;
	    default:
	        break;
	    }
	}

// <cmail>
TPoint CFreestyleEmailUiSendAttachmentsListControl::ActionMenuPosition()
    {
    TAlfRealRect focusRect;
    TFsTreeItemId listItemId = iModel->GetVisualizer()->FocusedItem();    
    iModel->GetVisualizer()->GetItemDisplayRectTarget(listItemId, focusRect);
    return focusRect.iTl;
    }
// </cmail>

