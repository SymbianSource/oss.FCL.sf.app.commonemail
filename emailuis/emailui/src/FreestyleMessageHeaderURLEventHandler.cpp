/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Message header URL event handler
*
*/

#include "FreestyleMessageHeaderURLEventHandler.h"
#include "FreestyleMessageHeaderURL.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiUtilities.h"
#include "CFSMailMessage.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiHtmlViewerView.h"
#include "FreestyleEmailUi.hrh"

#include <aknnotewrappers.h>
#include <BrCtlDefs.h>
#include <e32std.h>

EXPORT_C CFreestyleMessageHeaderURLEventHandler* CFreestyleMessageHeaderURLEventHandler::NewL( 
        CFreestyleEmailUiAppUi& aAppUi, 
        CFsEmailUiHtmlViewerView& aView )
    {
    CFreestyleMessageHeaderURLEventHandler* obj = new (ELeave) CFreestyleMessageHeaderURLEventHandler( aAppUi, aView );
    CleanupStack::PushL( obj );
    obj->ConstructL();
    CleanupStack::Pop( obj );
    return obj;
    }

CFreestyleMessageHeaderURLEventHandler::CFreestyleMessageHeaderURLEventHandler( 
        CFreestyleEmailUiAppUi& aAppUi, 
        CFsEmailUiHtmlViewerView& aView )
    : iAppUi( aAppUi ), 
    iView( aView ), 
    iMailMessage( NULL ), 
    iAttachmentsListModel( NULL )
    {
    }

void CFreestyleMessageHeaderURLEventHandler::ConstructL()
    {
    iMessageHeaderURL = CFreestyleMessageHeaderURL::NewL();
    }

CFreestyleMessageHeaderURLEventHandler::~CFreestyleMessageHeaderURLEventHandler ()
    {
    delete iMessageHeaderURL;
    }

EXPORT_C TBool CFreestyleMessageHeaderURLEventHandler::HandleEventL( const TDesC& aUri )
    {
    iMailMessage = iView.CurrentMessage(); 
    iAttachmentsListModel = iView.CurrentAttachmentsListModel();
    
    if ( ! CFreestyleMessageHeaderURL::IsMessageHeaderURL( aUri ) )
        {
        return EFalse;
        }
    else
        {
        //URL is of the message header format, hence parse it
        iMessageHeaderURL->InternalizeL( aUri );

        if ( ( iMessageHeaderURL->Type()->CompareF( KURLTypeTo ) == 0 )
             || ( iMessageHeaderURL->Type()->CompareF( KURLTypeFrom ) == 0 )
             || ( iMessageHeaderURL->Type()->CompareF( KURLTypeCc ) == 0 ) )
            {
            LaunchEmailAddressMenuL( *iMessageHeaderURL );
            }

        else if ( ( iMessageHeaderURL->Type()->CompareF( KURLTypeAttachment ) == 0 ) )
            {
            LaunchAttachmentMenuL( FindAttachmentL( *iMessageHeaderURL ) );
            }

        return ETrue;
        }
    }

/*
 * Launches the menu and populates it with the appropriate menu items and handles the user
 * menu item selection.
 * @param aType the type of the link the user selected
 */
void CFreestyleMessageHeaderURLEventHandler::LaunchEmailAddressMenuL( 
        const CFreestyleMessageHeaderURL& iMessageHeaderURL )
    {
    CFSEmailUiActionMenu::RemoveAllL();
    
    RArray<TActionMenuCustomItemId> uids;
    CleanupClosePushL( uids );
    uids.Append( FsEActionMenuCall );
    uids.Append( FsEActionMenuCreateMessage );
    uids.Append( FsEActionMenuCreateEmail );
    uids.Append( FsEActionMenuContactDetails );
    uids.Append( FsEActionMenuAddToContacts );

    if ( iView.IsRemoteLookupSupportedL() )
        {
        uids.Append( FsEActionMenuRemoteLookup );
        }


    for ( TInt i = 0; i < uids.Count(); i++ )
        {
        CFSEmailUiActionMenu::AddCustomItemL( uids[i] ); ///here is where you add stuff
        }

    CleanupStack::PopAndDestroy( &uids );
    TActionMenuCustomItemId menuResult = CFSEmailUiActionMenu::ExecuteL( EFscCenter );

    if ( menuResult != FsEActionMenuCasItemSelectedAndExecuted &&
            menuResult != FsEActionMenuDismissed    )
        {
        HandleEmailAddressActionMenuCommandL( menuResult, iMessageHeaderURL );
        }
    }

void CFreestyleMessageHeaderURLEventHandler::HandleEmailAddressActionMenuCommandL(
    TActionMenuCustomItemId aSelectedActionMenuItem,
    const CFreestyleMessageHeaderURL& iMessageHeaderURL )
    {
    TInt command( 0 );

    switch ( aSelectedActionMenuItem )
        {
        case FsEActionMenuCreateEmail: // Create message
            {
            command = EFsEmailUiCmdActionsReply;
            }
        break;
        case FsEActionMenuAddToContacts: // Add to Contacts
            {
            command = FsEActionMenuAddToContacts;
            }
        break;
        case FsEActionMenuCall: // Call
            {
            command = EFsEmailUiCmdActionsCall;
            }
        break;
        case FsEActionMenuCreateMessage: // Create message
            {
            command = EFsEmailUiCmdActionsCreateMessage;
            }
        break;
        case FsEActionMenuContactDetails: // Contact details
            {
            command = EFsEmailUiCmdActionsContactDetails;
            }
        break;
        case FsEActionMenuRemoteLookup: // Remote lookup
            {
            command = EFsEmailUiCmdActionsRemoteLookup;
            }
        break;
        }

    iView.HandleEmailAddressCommandL( command, *iMessageHeaderURL.ItemId() );
    }

const TAttachmentData& CFreestyleMessageHeaderURLEventHandler::FindAttachmentL( 
        const CFreestyleMessageHeaderURL& aAttachmentUrl )
    {
    User::LeaveIfNull( iAttachmentsListModel );
    TUint id;
    TLex parser( *aAttachmentUrl.ItemId() );
    parser.Val( id );
    
    TInt found = KErrNotFound;
    for (TInt i=0; i<iAttachmentsListModel->GetModel().Count(); i++)
        {
        if ( iAttachmentsListModel->GetModel()[i].partData.iMessagePartId.Id() == id )
            {
            found = i;
            break;
            }
        }
    
    if ( found == KErrNotFound )
        {
        // Probably, only the headers were downloaded. Check if attachments 
        // were downloaded later.
        iAttachmentsListModel->UpdateListL( iMailMessage );
        for (TInt i=0; i<iAttachmentsListModel->GetModel().Count(); i++)
            {
            if ( iAttachmentsListModel->GetModel()[i].partData.iMessagePartId.Id() == id )
                {
                found = i;
                break;
                }
            }

        if ( found == KErrNotFound )
            {
            User::Leave( KErrNotFound );
            }
        }
    
    return iAttachmentsListModel->GetModel()[found];
    }

void CFreestyleMessageHeaderURLEventHandler::LaunchAttachmentMenuL( 
        const TAttachmentData& aAttachment )
    {
    ASSERT( iAppUi.DownloadInfoMediator() );
    CFSEmailUiActionMenu::RemoveAllL();

    if ( iAppUi.DownloadInfoMediator()->IsDownloading( aAttachment.partData.iMessagePartId ) )
        {        
        CFSEmailUiActionMenu::AddCustomItemL( FsEActionAttachmentCancelDownload ); 
        }
    else if ( aAttachment.downloadProgress == KComplete )
        {
        CFSEmailUiActionMenu::AddCustomItemL( FsEActionAttachmentOpen ); 
        
        // block saving of embedded messages if needed.
        if ( iView.IsEmbeddedMsgView() )
            {
            if ( iView.IsEmbeddedMsgSavingAllowed() || !iAttachmentsListModel->IsMessage( aAttachment ) )
                {
                CFSEmailUiActionMenu::AddCustomItemL( FsEActionAttachmentSave );    
                } 
             
            }
        else
            {
            CFSEmailUiActionMenu::AddCustomItemL( FsEActionAttachmentSave ); 
            }
        
        if ( iAttachmentsListModel->GetModel().Count() > 1 )
            {
            // In embedded message mode, save all needs to be blocked if there
            // are any message type attachments. This is due to limitations of Activesync plugin.
            if( !(iView.IsEmbeddedMsgView() && iAttachmentsListModel->IsThereAnyMessageAttachments()) )
                {
                CFSEmailUiActionMenu::AddCustomItemL( FsEActionAttachmentSaveAll );            
                }
            
            }         
        }
    else
        {
        CFSEmailUiActionMenu::AddCustomItemL( FsEActionAttachmentOpen ); 
        CFSEmailUiActionMenu::AddCustomItemL( FsEActionAttachmentSave ); 
        if ( iAttachmentsListModel->GetModel().Count() > 1 )
            {
            CFSEmailUiActionMenu::AddCustomItemL( FsEActionAttachmentSaveAll ); 
            }         
        }

    TActionMenuCustomItemId menuResult = CFSEmailUiActionMenu::ExecuteL( EFscCenter );

    if ( menuResult != FsEActionMenuCasItemSelectedAndExecuted &&
            menuResult != FsEActionMenuDismissed    )
        {
        HandAttachmentActionMenuCommandL( menuResult, aAttachment );
        }
    }

void CFreestyleMessageHeaderURLEventHandler::HandAttachmentActionMenuCommandL( 
        TActionMenuCustomItemId aSelectedActionMenuItem,
        const TAttachmentData& aAttachment )
    {
    switch ( aSelectedActionMenuItem )
        {
        case FsEActionAttachmentCancelDownload:
            iView.CancelAttachmentL( aAttachment );
            break;
            
        case FsEActionAttachmentOpen:
            iView.OpenAttachmentL( aAttachment );
            break;
            
        case FsEActionAttachmentSave:
            iView.SaveAttachmentL( aAttachment );
            break;
            
        case FsEActionAttachmentSaveAll:
            iView.SaveAllAttachmentsL();
            break;
            
        case FsEActionAttachmentClearFetchedContent:
            iView.RemoveAttachmentContentL( aAttachment );
            break;
            
        case FsEActionAttachmentDownload:
            iView.DownloadAttachmentL( aAttachment );
            break;
            
        case FsEActionAttachmentDownloadAll:
            iView.DownloadAllAttachmentsL();
            break;
            
        case FsEActionAttachmentViewAll:
            iView.OpenAttachmentsListViewL();
            break;            
        }
    }


