/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Creates and display action menu.
*
*/


#include "cesmrcontactmenuattachmenthandler.h"
#include "cesmrconfirmationquery.h"

#include "cfsccontactactionmenu.h"
#include "fsccontactactionmenuuids.hrh"
#include "cfsccontactactionmenuitem.h" 
#include "tfsccontactactionmenuconstructparameters.h"
#include "mfsccontactactionmenumodel.h"
#include "cesmrattachmentinfo.h"
#include "esmrhelper.h"
#include "cesmriconfield.h"
#include "nmrbitmapmanager.h"
#include "cfsccontactactionservice.h"
#include "esmrcommands.h"
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactStoreList.h>
#include <TVPbkContactStoreUriPtr.h>
#include <esmrgui.rsg>
#include <data_caging_path_literals.hrh>
#include <AknsUtils.h>
#include <eikmenup.h>
#include <gulicon.h>
// DEBUG
#include "emailtrace.h"

/// Unnamed namespace for local definitions
namespace {

// MR contains one attachment
const TInt KOneAttachment( 1 );

// Attachment actions menu uids
static const TUid KOpenAttachmentViewUid = { 1 };
static const TUid KOpenAttachmentUid = { 2 };
static const TUid KDownloadAttachmentUid = { 3 };
static const TUid KDownloadAllAttachmentsUid = { 4 };
static const TUid KSaveAttachmentUid = { 5 };
static const TUid KSaveAllAttachmentsUid = { 6 };

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor.
// ---------------------------------------------------------------------------
//
CESMRContactMenuAttachmentHandler::CESMRContactMenuAttachmentHandler(
        CFscContactActionMenu& aContactActionMenu ) :
        iContactActionMenu( aContactActionMenu )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor that does not leave a pointer to the cleanup
// stack.
// ---------------------------------------------------------------------------
//
CESMRContactMenuAttachmentHandler* CESMRContactMenuAttachmentHandler::NewL(
        CFscContactActionMenu& aContactActionMenu )
    {
    FUNC_LOG;
    CESMRContactMenuAttachmentHandler* self = 
		new (ELeave) CESMRContactMenuAttachmentHandler( aContactActionMenu );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// Creates the action menu object.
// ---------------------------------------------------------------------------
//
void CESMRContactMenuAttachmentHandler::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CESMRContactMenuAttachmentHandler::~CESMRContactMenuAttachmentHandler()
    {
    FUNC_LOG;
    iIconArray.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuAttachmentHandler::Reset( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuAttachmentHandler::Reset( )
    {
    FUNC_LOG;
    iOptionsMenuAvailable = EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuAttachmentHandler::SetAttachmentInfo( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuAttachmentHandler::SetAttachmentInfo( 
        CESMRAttachmentInfo* aAttachmentInfo )
    {
    iAttachmentInfo = aAttachmentInfo;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuAttachmentHandler::InitActionMenuL
// ---------------------------------------------------------------------------
//
void CESMRContactMenuAttachmentHandler::InitActionMenuL( )
    {
    FUNC_LOG;

    if ( iAttachmentInfo )
        {
        iOptionsMenuAvailable = ETrue;
        
        MFscContactActionMenuModel& actionMenuModel = iContactActionMenu.Model();

        TInt attachmentCount( iAttachmentInfo->AttachmentCount() );
        
        if( attachmentCount > KOneAttachment )
        	{
        	CreateMenuItemL( R_QTN_MEET_REQ_VIEWER_ACTION_ATTACH_VIEW_LIST,
                KOpenAttachmentViewUid );
                
            // go through the list to see if we need to download 
            // some attachment
            TBool needDownload(EFalse);
            
            for( TInt ii = attachmentCount-1; ii >= 0 && !needDownload; --ii )
                {
                const CESMRAttachment& attachment( 
                    iAttachmentInfo->AttachmentL( ii ) );
                    
                needDownload = attachment.AttachmenState() != 
                    CESMRAttachment::EAttachmentStateDownloaded;
                }
            
            // download all command    
            if( needDownload )
                {
                CreateMenuItemL( R_QTN_MEET_REQ_VIEWER_ACTION_ATTACH_DOWNLOAD_ALL,
                    KDownloadAllAttachmentsUid );
                }
            
            // save all command    
            CreateMenuItemL( R_QTN_MEET_REQ_VIEWER_ACTION_ATTACH_SAVE_ALL,
                KSaveAllAttachmentsUid );
        	}
       	else
       		{
            // There is one attachment
            const CESMRAttachment& attachment(
                    iAttachmentInfo->AttachmentL( 0 ) );
       		
       		// open or download command
       		if( attachment.AttachmenState() == CESMRAttachment::EAttachmentStateDownloaded )
       			{
                CreateMenuItemL( R_QTN_MEET_REQ_VIEWER_ACTION_ATTACH_OPEN,
                    KOpenAttachmentUid );
       			}
       		else
       			{
                CreateMenuItemL( R_QTN_MEET_REQ_VIEWER_ACTION_ATTACH_DOWNLOAD,
                    KDownloadAttachmentUid );
       			}
       		
       		// save command   		
            CreateMenuItemL( R_QTN_MEET_REQ_VIEWER_ACTION_ATTACH_SAVE,
                KSaveAttachmentUid );
       		}       		
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuAttachmentHandler::CreateMenuItemL
// ---------------------------------------------------------------------------
//
void CESMRContactMenuAttachmentHandler::CreateMenuItemL( 
		TInt aResourceId, 
		TUid aCommandUid )
    {
    FUNC_LOG;
    CGulIcon* itemIcon = CreateIconL(aCommandUid);
    //ownership transferred
    CleanupStack::PushL( itemIcon );
    iIconArray.AppendL( itemIcon );
    CleanupStack::Pop( itemIcon );

    HBufC* text = CCoeEnv::Static()->AllocReadResourceLC( aResourceId );
    CFscContactActionMenuItem* newItem = 
		iContactActionMenu.Model().NewMenuItemL( *text,
												 itemIcon,
                                                 1,
                                                 EFalse,
                                                 aCommandUid );

    CleanupStack::PopAndDestroy( text );
    CleanupStack::PushL( newItem );
    iContactActionMenu.Model().AddItemL( newItem );
    CleanupStack::Pop( newItem );
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuAttachmentHandler::CreateIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CESMRContactMenuAttachmentHandler::CreateIconL(TUid aCommandUid)
    {
    FUNC_LOG;
    // Define default icon object
    CFbsBitmap* iconBitmap( NULL );
    CFbsBitmap* iconMaskBitmap( NULL );
    NMRBitmapManager::TMRBitmapId iconID( NMRBitmapManager::EMRBitmapNotSet );

    //select correct icon for item
    if( aCommandUid == KOpenAttachmentViewUid )
        {
        iconID = NMRBitmapManager::EMRBitmapOpenAttachmentView;
        }
    else if ( aCommandUid ==KOpenAttachmentUid )
        {
        iconID = NMRBitmapManager::EMRBitmapOpenAttachment;
        }
    else if( aCommandUid == KDownloadAttachmentUid )
        {
        iconID = NMRBitmapManager::EMRBitmapDownloadAttachment;
        }
    else if( aCommandUid == KDownloadAllAttachmentsUid )
        {
        iconID = NMRBitmapManager::EMRBitmapDownloadAllAttachments;
        }
    else if( aCommandUid == KSaveAttachmentUid )
        {
        iconID = NMRBitmapManager::EMRBitmapSaveAttachment;
        }
    else if( aCommandUid == KSaveAllAttachmentsUid )
        {
        iconID = NMRBitmapManager::EMRBitmapSaveAllAttachments;
        }

    User::LeaveIfError( 
            NMRBitmapManager::GetSkinBasedBitmap( 
                    iconID, iconBitmap, iconMaskBitmap, KIconSize ) );
    
    CGulIcon* icon = CGulIcon::NewL( iconBitmap, iconMaskBitmap );
    return icon;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuAttachmentHandler::OptionsMenuAvailable( )
// ---------------------------------------------------------------------------
//
TBool CESMRContactMenuAttachmentHandler::OptionsMenuAvailable( )
    {
    FUNC_LOG;
    return iOptionsMenuAvailable;
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuAttachmentHandler::InitOptionsMenuL
// ---------------------------------------------------------------------------
//
void CESMRContactMenuAttachmentHandler::InitOptionsMenuL( 
		CEikMenuPane* aActionMenuPane )
    {
    FUNC_LOG;

    if ( iAttachmentInfo )
        {
        iOptionsMenuAvailable = ETrue;

        CEikMenuPaneItem::SData data;

        TInt attachmentCount( iAttachmentInfo->AttachmentCount() );
        
        CCoeEnv* coeEnv = CCoeEnv::Static();
        
        if( attachmentCount > KOneAttachment )
        	{
        	// command to open download manager
            data.iFlags = 0;
            data.iCascadeId = 0;
            data.iCommandId = EESMRCmdOpenAttachmentView;

            TRAP_IGNORE( coeEnv->ReadResourceL( 
                	data.iText, R_QTN_MEET_REQ_VIEWER_OPTIONS_ATTACH_VIEW_LIST ) );
            aActionMenuPane->AddMenuItemL( data );
            
            // go through the list to see if we need to download 
            // some attachment
            TBool needDownload(EFalse);
            
            for( TInt ii = attachmentCount-1; ii >= 0 && !needDownload; --ii )
                {
                const CESMRAttachment& attachment( 
                    iAttachmentInfo->AttachmentL( ii ) );
                    
                needDownload = attachment.AttachmenState() != 
                    CESMRAttachment::EAttachmentStateDownloaded;
                }
            
            // download all command    
            if( needDownload )
                {
                data.iFlags = 0;
                data.iCascadeId = 0;
                data.iCommandId = EESMRCmdDownloadAllAttachments;

                TRAP_IGNORE( coeEnv->ReadResourceL( 
                    	data.iText, R_QTN_MEET_REQ_VIEWER_OPTIONS_ATTACH_DOWNLOAD_ALL ) );
                aActionMenuPane->AddMenuItemL( data );    
                }
            
            // save all command    
            data.iFlags = 0;
            data.iCascadeId = 0;
            data.iCommandId = EESMRCmdSaveAllAttachments;

            TRAP_IGNORE( coeEnv->ReadResourceL( 
                	data.iText, R_QTN_MEET_REQ_VIEWER_OPTIONS_ATTACH_SAVE_ALL ) );
            aActionMenuPane->AddMenuItemL( data );            
        	}
       	else
       		{
            // There is one attachment
            const CESMRAttachment& attachment(
                    iAttachmentInfo->AttachmentL( 0 ) );
       		
       		// open or download command
       		if( attachment.AttachmenState() == CESMRAttachment::EAttachmentStateDownloaded )
       			{
                data.iFlags = 0;
                data.iCascadeId = 0;
                data.iCommandId = EESMRCmdOpenAttachment;

                TRAP_IGNORE( coeEnv->ReadResourceL( 
                		data.iText, R_QTN_OPTIONS_OPEN ) );
                aActionMenuPane->AddMenuItemL( data );                
       			}
       		else
       			{
                data.iFlags = 0;
                data.iCascadeId = 0;
                data.iCommandId = EESMRCmdDownloadAttachment;

                TRAP_IGNORE( coeEnv->ReadResourceL( 
                    	data.iText, R_QTN_MEET_REQ_VIEWER_OPTIONS_ATTACH_DOWNLOAD ) );
                aActionMenuPane->AddMenuItemL( data );    
       			}
       		
       		// save command
            data.iFlags = 0;
            data.iCascadeId = 0;
            data.iCommandId = EESMRCmdSaveAttachment;

            TRAP_IGNORE( coeEnv->ReadResourceL( 
                	data.iText, R_QTN_MEET_REQ_VIEWER_OPTIONS_ATTACH_SAVE ) );
            aActionMenuPane->AddMenuItemL( data );    
       		}
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuAttachmentHandler::ShowActionMenuL( )
// ---------------------------------------------------------------------------
//
void CESMRContactMenuAttachmentHandler::ShowActionMenuL( )
    {
    FUNC_LOG;
    TFscActionMenuResult menuResult = iContactActionMenu.ExecuteL();

    if ( menuResult == EFscCustomItemSelected )
        {
        TUid uid = iContactActionMenu.FocusedItem().ImplementationUid();
        if ( uid == KOpenAttachmentViewUid )
            {
            //OpenAttachmentViewL();
            iCommandObserver->ProcessCommandL( EESMRCmdOpenAttachmentView );
            }
        else
        if( uid == KOpenAttachmentUid )
            {
            iCommandObserver->ProcessCommandL( EESMRCmdOpenAttachment );
            }
        else
        if( uid == KDownloadAllAttachmentsUid )
            {
            iCommandObserver->ProcessCommandL( EESMRCmdDownloadAllAttachments );
            }
        else
        if( uid == KDownloadAttachmentUid )
            {
            iCommandObserver->ProcessCommandL( EESMRCmdDownloadAttachment );
            }
        else
        if( uid == KSaveAllAttachmentsUid )
            {
            iCommandObserver->ProcessCommandL( EESMRCmdSaveAllAttachments );
            }
        else
        if( uid == KSaveAttachmentUid )
            {
            iCommandObserver->ProcessCommandL( EESMRCmdSaveAttachment );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRContactMenuAttachmentHandler::SetCommandObserver()
// ---------------------------------------------------------------------------
//
void CESMRContactMenuAttachmentHandler::SetCommandObserver(
            MEikCommandObserver* aCommandObserver )
    {
    FUNC_LOG;
    iCommandObserver = aCommandObserver;
    }
// End of file

