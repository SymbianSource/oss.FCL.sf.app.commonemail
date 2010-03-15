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
#include "cfsmailmessage.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiHtmlViewerView.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUi.rsg"
#include "FSHtmlReloadAO.h"

#include <aknnotewrappers.h>
#include <aknstyluspopupmenu.h>
#include <brctldefs.h>
#include <e32std.h>
#include <eikmobs.h>
#include <coemain.h>  
#include <schemehandler.h>

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
    iHTMLReloadAO = CFSHtmlReloadAO::NewL(iView);
    
    }

CFreestyleMessageHeaderURLEventHandler::~CFreestyleMessageHeaderURLEventHandler ()
    {
    delete iMessageHeaderURL;
    delete iHTMLReloadAO; 
    if( iEmailAddressStylusPopup )
        {
        delete iEmailAddressStylusPopup; 
        }
    
    if( iAttachmentStylusPopup )
        {
        delete iAttachmentStylusPopup; 
        }
    
    if( iWebAddressStylusPopup )
        {
        delete iWebAddressStylusPopup; 
        }   
    
    delete iUrl;
    }

EXPORT_C TBool CFreestyleMessageHeaderURLEventHandler::HandleEventL( const TDesC& aUri )
    {
    iMailMessage = iView.CurrentMessage(); 
    iAttachmentsListModel = iView.CurrentAttachmentsListModel();
    
    if ( ! CFreestyleMessageHeaderURL::IsMessageHeaderURL( aUri ) )
        {
        //Handle http and https links
        if( ( aUri.FindF( KURLHttpPrefix ) ) == 0 
                ||( aUri.FindF( KURLHttpsPrefix ) ) == 0 )
            {
            if ( iUrl )
                {
                delete iUrl;
                iUrl = NULL;
                }
            iUrl = aUri.AllocL();
            LaunchWebAddressMenuL( );
            return ETrue;
            }         
        //Link wasn't handled
        return EFalse;
        }
    else
        {
        //URL is of the message header format, hence parse it
        iMessageHeaderURL->InternalizeL( aUri );
        iMenuVisible = ETrue;
        if ( ( iMessageHeaderURL->Type()->CompareF( KURLTypeTo ) == 0 )
             || ( iMessageHeaderURL->Type()->CompareF( KURLTypeFrom ) == 0 )
             || ( iMessageHeaderURL->Type()->CompareF( KURLTypeCc ) == 0 ) )
            {
            LaunchEmailAddressMenuL( );
            }

        else if ( ( iMessageHeaderURL->Type()->CompareF( KURLTypeAttachment ) == 0 ) )
            {
            LaunchAttachmentMenuL( FindAttachmentL( *iMessageHeaderURL ) );
            }
        else if ( iMessageHeaderURL->Type()->CompareF( KURLTypeSubject ) )
            {
            CSchemeHandler* handler = CSchemeHandler::NewL( aUri );
            CleanupStack::PushL( handler );
            handler->HandleUrlStandaloneL();
            CleanupStack::PopAndDestroy( handler );
            }
        iMenuVisible=EFalse;
        if( iPendingReload )
            {
            //Load web page aysnchronously
            iHTMLReloadAO->ReloadPageAysnc();
            iPendingReload=EFalse;
            }
        return ETrue;
        }
    }

/*
 * Launches the avkon stylus popup and dims the inappropriate menu items and handles the user
 * menu item selection.
 * @param aType the type of the link the user selected
 */
void CFreestyleMessageHeaderURLEventHandler::LaunchEmailAddressMenuL()
    {
    
    //Change the creation of the stylus menu here to avoid crash when calling SetItemDimmed(ETrue) multiple times 
    //on same instance of the menu (if created only once in constructor).
    //Creating the menu everytime the user clicks on the link avoids this crash however performance is affected.
    if( iEmailAddressStylusPopup)
       {
       delete iEmailAddressStylusPopup;
       iEmailAddressStylusPopup = NULL;
       }
   
    TPoint point( 0, 0 );
    iEmailAddressStylusPopup = CAknStylusPopUpMenu::NewL( this , point );
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, R_STYLUS_POPUP_MENU_HTML_VIEW_EMAIL_ADDRESS );
    iEmailAddressStylusPopup->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy(); //resource reader
         
    iEmailAddressStylusPopup->SetItemDimmed( EFsEmailUiCmdActionsRemoteLookup, 
                                             !iView.IsRemoteLookupSupportedL() ); 
    iEmailAddressStylusPopup->SetPosition( iAppUi.ClientRect().Center(), 
                                           CAknStylusPopUpMenu::EPositionTypeRightBottom );
    iEmailAddressStylusPopup->ShowMenu();
    }

//From MEikMenuObserver
void CFreestyleMessageHeaderURLEventHandler::ProcessCommandL( TInt aCommand )
    {
    
    switch ( aCommand )
        {
        case EFsEmailUiCmdActionsReply:
        case EFsEmailUiCmdActionsAddContact:
        case EFsEmailUiCmdActionsRemoteLookup:
        case EFsEmailUiCmdActionsCopyToClipboard:
        case EFsEmailUiCmdActionsContactDetails:
            {
            iView.HandleEmailAddressCommandL( aCommand, *iMessageHeaderURL->ItemId() );
            break;
            }
            
        case EFsEmailUiCmdCancelDownload:
            {
            iView.CancelAttachmentL( FindAttachmentL( *iMessageHeaderURL ) );
            break;
            }
            
        case EFsEmailUiCmdOpenAttachment:
            {
            iView.OpenAttachmentL( FindAttachmentL( *iMessageHeaderURL ) );
            break;
            }
            
        case EFsEmailUiCmdSave:
            {
            iView.SaveAttachmentL( FindAttachmentL( *iMessageHeaderURL ) );
            break;
            }
            
        case EFsEmailUiCmdSaveAll:
            {
            iView.SaveAllAttachmentsL( );
            break;      
            }
            
        case EFsEmailUiCmdActionsOpenWeb:
        case EFsEmailUiCmdActionsAddBookmark:
        case EFsEmailUiCmdActionsCopyWWWAddressToClipboard:
            {
            iView.HandleWebAddressCommandL( aCommand, *iUrl );
            break;
            }
            
        }
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
    
    //Change the creation of the stylus menu here to avoid crash when calling SetItemDimmed(ETrue) multiple times 
    //on same instance of the menu (if created only once in constructor).
    //Creating the menu everytime the user clicks on the link avoids this crash however performance is affected.
    if( iAttachmentStylusPopup )
       {
       delete iAttachmentStylusPopup;
       iAttachmentStylusPopup = NULL;
       }
    TPoint point( 0, 0 );
    iAttachmentStylusPopup = CAknStylusPopUpMenu::NewL( this , point );
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, R_STYLUS_POPUP_MENU_HTML_VIEW_ATTACHMENT );
    iAttachmentStylusPopup->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy(); //resource reader
    
    
    //Dim all item by default
    iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdOpenAttachment, ETrue );
    iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdSave, ETrue );
    iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdSaveAll, ETrue );
    iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdCancelDownload, ETrue );   

    const TBool isMessage( iAttachmentsListModel->IsMessage( aAttachment ) );
    
    if ( iAppUi.DownloadInfoMediator()->IsDownloading( aAttachment.partData.iMessagePartId ) )
        {        
        iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdCancelDownload, EFalse );  
        }
    else if ( aAttachment.downloadProgress == KComplete )
        {
        iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdOpenAttachment, EFalse );
        
        // block saving of embedded messages if needed.
        if ( iView.IsEmbeddedMsgView() )
            {
            if ( iView.IsEmbeddedMsgSavingAllowed() || !isMessage )
                {
                iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdSave, EFalse );    
                }              
            }
        else
            {
            iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdSave, isMessage && !iView.IsEmbeddedMsgSavingAllowed() );
            }
        
        if ( iAttachmentsListModel->GetModel().Count() > 1 )
            {
            // Save all cannot be shown if there is one message attachment and saving is not supported
            if ( !( iAttachmentsListModel->IsThereAnyMessageAttachments() && !iView.IsEmbeddedMsgSavingAllowed() ) )
                {
                // In embedded message mode, save all needs to be blocked if there
                // are any message type attachments. This is due to limitations of Activesync plugin.
                if( !(iView.IsEmbeddedMsgView() && iAttachmentsListModel->IsThereAnyMessageAttachments()) )
                    {
                    iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdSaveAll, EFalse );    
                    }
                }
            }         
        }
    else
        {
        iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdOpenAttachment, EFalse );
        iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdSave, isMessage ); 
        if ( iAttachmentsListModel->GetModel().Count() > 1 )
            {
            iAttachmentStylusPopup->SetItemDimmed( EFsEmailUiCmdSaveAll,
                    iAttachmentsListModel->IsThereAnyMessageAttachments() && !iView.IsEmbeddedMsgSavingAllowed() );
            }         
        }
   
    
    iAttachmentStylusPopup->SetPosition( iAppUi.LastSeenPointerPosition(), 
                                         CAknStylusPopUpMenu::EPositionTypeLeftTop );
    iAttachmentStylusPopup->ShowMenu();
    }

//Open the Avkon stylus popup when a web address link was pressed
void CFreestyleMessageHeaderURLEventHandler::LaunchWebAddressMenuL()
    {
    
    //Change the creation of the stylus menu here to avoid crash when calling SetItemDimmed(ETrue) multiple times 
    //on same instance of the menu (if created only once in constructor).
    //Creating the menu everytime the user clicks on the link avoids this crash however performance is affected.
    if( iWebAddressStylusPopup )
        {
        delete iWebAddressStylusPopup;
        iWebAddressStylusPopup = NULL;
        }
    
    TPoint point( 0, 0 );
    iWebAddressStylusPopup = CAknStylusPopUpMenu::NewL( this , point );
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, R_STYLUS_POPUP_MENU_HTML_VIEW_WEB_ADDRESS );
    iWebAddressStylusPopup->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy(); //resource reader
 
    iWebAddressStylusPopup->SetPosition( iAppUi.ClientRect().Center(), 
                                           CAknStylusPopUpMenu::EPositionTypeRightBottom );
    iWebAddressStylusPopup->ShowMenu();
    }

//From MEikMenuObserver
void CFreestyleMessageHeaderURLEventHandler::SetEmphasis(CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/)
    {
    }

void CFreestyleMessageHeaderURLEventHandler::DismissMenuAndReload()
    {
        CFSEmailUiActionMenu::Dismiss(ETrue);
        iPendingReload=ETrue;
    }
TBool CFreestyleMessageHeaderURLEventHandler::IsMenuVisible()
    {
    return iMenuVisible;
    }
