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
* Description:  CFsEmailUiHtmlViewerView class implementation
*
*/


#include "emailtrace.h"
#include <coemain.h>
#include <aknconsts.h>
#include <AknsUtils.h>
#include <StringLoader.h>
#include <AknWaitDialog.h>
#include <AknGlobalListQuery.h>
#include <AknGlobalNote.h>
#include <aknnavi.h>
#include <aknnavide.h>
//<cmail>
#include <featmgr.h>
#include "CFSMailMessage.h"
#include "CFSMailClient.h"
#include <FreestyleEmailUi.rsg>
#include <SchemeHandler.h> // CSchemeHandler
#include <brctlinterface.h>
#include <csxhelp/cmail.hlp.hrh>
// Meeting request
#include <MeetingRequestUids.hrh>
//<cmail>
#include "cesmricalviewer.h"
#include "FreestyleEmailUiHtmlViewerView.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiHtmlViewerContainer.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FreestyleEmailCenRepHandler.h"
#include "FreestyleEmailUiAttachmentsListModel.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleAttachmentDownloadProgressBar.h"
#include "FSEmail.pan"
#include "CFSMailCommon.h"

#include "FreestyleEmailUiMailViewerConstants.h"
#include "FSDelayedLoader.h"
#include "FreestyleMessageHeaderURL.h"
#include <akntoolbar.h>
// CONSTANTS
// Zoom levels available on the UI
const TInt KZoomLevels[] = { 50, 75, 100, 125 };
const TInt KZoomLevelCount = sizeof( KZoomLevels ) / sizeof( TInt );

CFsEmailUiHtmlViewerView* CFsEmailUiHtmlViewerView::NewL( 
    CAlfEnv& aEnv,
    CFreestyleEmailUiAppUi& aAppUi, 
    CAlfControlGroup& aControlGroup )
    {
    FUNC_LOG;
    CFsEmailUiHtmlViewerView* self = new ( ELeave ) CFsEmailUiHtmlViewerView( aEnv, aAppUi, aControlGroup );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CFsEmailUiHtmlViewerView::CFsEmailUiHtmlViewerView( CAlfEnv& aEnv,
        CFreestyleEmailUiAppUi& aAppUi, 
        CAlfControlGroup& aControlGroup )
        : CFsEmailUiViewBase( aControlGroup, aAppUi ),
        iEnv( aEnv )
    {
    FUNC_LOG;
    }

CFsEmailUiHtmlViewerView::~CFsEmailUiHtmlViewerView()
    {
    FUNC_LOG;
    CancelFetchings();
    if ( iAppUi.DownloadInfoMediator() && iMessage )
        {
        iAppUi.DownloadInfoMediator()->StopObserving( this, iMessage->GetMessageId() );
        }

    // <cmail>
    if ( iContainer )
        {
        iContainer->PrepareForExit();
        }

    delete iNewMailTempAddress;
    // </cmail>
    if ( iContainer )
        {
        iContainer->CancelFetch();
        }
    delete iContainer;
    delete iAttachmentsListModel;
    delete iProgressBar;

    delete iOpenMessages;
    iOpenMessages = NULL;
    delete iEmbeddedMessages;
    iEmbeddedMessages = NULL;
    iMessage = NULL;
    
    delete iMailBox; 
    iMailBox = NULL;
    delete iAsyncCallback;


   
    }

void CFsEmailUiHtmlViewerView::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL( R_FSEMAILUI_HTML_VIEW );
    iAsyncCallback = new (ELeave) CAsyncCallBack( CActive::EPriorityLow );
    iOpenMessages = new (ELeave) CStack<CFSMailMessage, ETrue>();
    iEmbeddedMessages = new (ELeave) CStack<CFSMailMessage, EFalse>();
    iNextOrPrevMessageSelected = EFalse; 
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::Id()
// Returns View's ID.
// -----------------------------------------------------------------------------
//
TUid CFsEmailUiHtmlViewerView::Id() const
    {
    FUNC_LOG;
    return HtmlViewerId;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::HandleCommandL()
// Takes care of Command handling.
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;

    if ( !iAppUi.ViewSwitchingOngoing() )
        {
        switch ( aCommand )
            {
                // Open MSK handling
            case EFsEmailUiCmdActionsCall:
            case EFsEmailUiCmdActionsOpenWeb:
            case EFsEmailUiCmdComposeTo:
                {
                TBrCtlDefs::TBrCtlElementType elementType = iContainer->BrowserControlIf()->FocusedElementType();

                switch ( elementType )
                    {
                    case TBrCtlDefs::EElementTelAnchor:
                    case TBrCtlDefs::EElementSmartLinkTel:
                        {
                        iContainer->BrowserControlIf()->HandleCommandL( ( TInt )TBrCtlDefs::ECommandIdBase +
                                ( TInt )TBrCtlDefs::ECommandMakeCall );
                        }
                    break;
                    case TBrCtlDefs::EElementMailtoAnchor:
                    case TBrCtlDefs::EElementSmartLinkEmail:
                        {
                        // <cmail> change made because the functionality of
                        // sending email is not present in the browser control
                        // anymore

                        // Create buffer and strip scheme data "mailto:"
                        HBufC* mailAddress =
                            iContainer->BrowserControlIf()->PageInfoLC(
                                TBrCtlDefs::EPageInfoFocusedNodeUrl );

                        if ( mailAddress->FindC( KMailtoPrefix ) == 0 )
                            {
                            mailAddress->Des().Delete( 0,
                                                       KMailtoPrefix().Length() );
                            }

                        delete iNewMailTempAddress;
                        iNewMailTempAddress = NULL;
                        iNewMailTempAddress = CFSMailAddress::NewL();
                        iNewMailTempAddress->SetEmailAddress( *mailAddress );
                        CleanupStack::PopAndDestroy( mailAddress );
                        iAppUi.LaunchEditorL( iNewMailTempAddress );

                        /*iContainer->BrowserControlIf()->HandleCommandL( (TInt)TBrCtlDefs::ECommandIdBase +
                                                                        (TInt)TBrCtlDefs::ECommandSmartLinkSendEmail );*/
                        // </cmail>
                        }
                    break;
                    case TBrCtlDefs::EElementAnchor:
                        {
                        // OPEN url in standalone browser UI
                        /*                         HBufC* url = iContainer->BrowserControlIf()->PageInfoLC(TBrCtlDefs::EPageInfoFocusedNodeUrl);
                                               // Use scheme handler to launch the browser as a stand alone application
                                               TInt urlPos = url->Find( KUrlPrefixIdentifier );
                                               if( urlPos == KErrNotFound )
                                                   {
                                                   HBufC* newBuf = url->ReAllocL( url->Length() + KHttpUrlPrefix().Length() );
                                                   CleanupStack::Pop( url );
                                                   url = newBuf;
                                                   CleanupStack::PushL( url );
                                                   TPtr urlPtr = url->Des();
                                                   urlPtr.Insert( 0, KHttpUrlPrefix );
                                                   }
                                               CSchemeHandler* handler = CSchemeHandler::NewL( *url );
                                               CleanupStack::PushL( handler );
                                               handler->HandleUrlStandaloneL();
                                               CleanupStack::PopAndDestroy( handler );
                                               CleanupStack::PopAndDestroy( url );                                     */
                        }
                    break;
                    case TBrCtlDefs::EElementNone:
                    default:
                        break;
                    }
                }
            break;
            case EAknSoftkeyBack:
                {
                CancelPendingMrCommandL();
                NavigateBackL();
                }
            break;
            case EFsEmailUiCmdActionsOpen:
            case EFsEmailUiCmdActionsReply:
                {
                if ( iMessage && !iActivationData.iEmbeddedMessageMode )
                    {
                    TEditorLaunchParams params;

                    params.iMailboxId = iAppUi.GetActiveMailboxId();
                    params.iMsgId = iMessage->GetMessageId();
                    params.iActivatedExternally = EFalse;

                    if ( iContainer )
                        {
                        HideContainer();
                        iContainer->ResetContent();
                        }

                    iAppUi.LaunchEditorL( KEditorCmdReply, params );
                    }
                }
            break;
            case EFsEmailUiCmdActionsReplyAll:
                {
                if ( iMessage && !iActivationData.iEmbeddedMessageMode )
                    {
                    TEditorLaunchParams params;
                    params.iMailboxId = iAppUi.GetActiveMailboxId();
                    params.iMsgId = iMessage->GetMessageId();
                    params.iActivatedExternally = EFalse;

                    if ( iContainer )
                        {
                        HideContainer();
                        iContainer->ResetContent();
                        }

                    iAppUi.LaunchEditorL( KEditorCmdReplyAll, params );
                    }
                }
            break;
            case EFsEmailUiCmdActionsForward:
                {
                if ( iMessage && !iActivationData.iEmbeddedMessageMode )
                    {
                    TEditorLaunchParams params;
                    params.iMailboxId = iAppUi.GetActiveMailboxId();
                    params.iMsgId = iMessage->GetMessageId();
                    params.iActivatedExternally = EFalse;

                    if ( iContainer )
                        {
                        HideContainer();
                        iContainer->ResetContent();
                        }

                    iAppUi.LaunchEditorL( KEditorCmdForward, params );
                    }
                }
            break;
            case EFsEmailUiCmdActionsDelete:
                {
                DeleteMailL();
                }
            break;
            case EFsEmailUiCmdZoomIn:
                {
                TUint zoomLevelIdx = ZoomLevelIndexL();
                SetZoomLevelIndexL( zoomLevelIdx + 1 );
                }
            break;
            case EFsEmailUiCmdZoomOut:
                {
                TUint zoomLevelIdx = ZoomLevelIndexL();
                SetZoomLevelIndexL( zoomLevelIdx - 1 );
                }
            break;
            case EFsEmailUiCmdHelp:
                {
                TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
                }
            break;
            case EFsEmailUiCmdExit:
                {
                // <cmail>
                iContainer->PrepareForExit();
                // </cmail>
                iAppUi.Exit();
                }
            break;
            case EFsEmailUiCmdOpenAttachmentList:
                {
                TAttachmentListActivationData params;
                params.iMailBoxId = iMessage->GetMailBoxId();
                params.iFolderId = iMessage->GetFolderId();
                params.iMessageId = iMessage->GetMessageId();
                // use package buffer to pass the params
                TPckgBuf<TAttachmentListActivationData> buf( params );
                TUid emptyCustomMessageId = { 0 };
                iAppUi.EnterFsEmailViewL( AttachmentMngrViewId, emptyCustomMessageId, buf );

                }
            break;
             case EFsEmailUiCmdPreviousMessage:
                 {
                 ShowPreviousMessageL();
                 }
                 break;
             case EFsEmailUiCmdNextMessage:
                 {
                 ShowNextMessageL();
                 }
                 break;
             case EFsEmailUiCmdMarkAsUnread:
                 {
                 ChangeMsgReadStatusL( EFalse, EFalse );
                 }
                 break;
             case EFsEmailUiCmdMarkAsRead:
                 {
                 ChangeMsgReadStatusL( ETrue, EFalse );
                 }
                 break;
             case EFsEmailUiCmdActionsMoveMessage:
                 {
                 OpenFolderListForMessageMovingL();
                 }
                 break;
             case EFsEmailUiCmdActionsFlag:
                 {
                 SetMessageFollowupFlagL();
                 }
                 break;
             case EFsEmailUiCmdMessageDetails:
                 {
                 }
             case EFsEmailUiCmdCompose:
                 {
                 if ( iMessage )
                     {
                     
                     if ( iActivationData.iEmbeddedMessageMode )
                         {
                         iCreateNewMsgFromEmbeddedMsg = ETrue;
                         }
                     
                     HBufC* mailAddress=iMessage->GetSender()->GetEmailAddress().AllocLC();
                     delete iNewMailTempAddress;
                     iNewMailTempAddress = NULL;
                     iNewMailTempAddress = CFSMailAddress::NewL();
                     iNewMailTempAddress->SetEmailAddress( *mailAddress );
                     CleanupStack::PopAndDestroy( mailAddress );
                     iAppUi.LaunchEditorL( iNewMailTempAddress );
                     }
                 }
                 break;
            default:
                {
                if ( aCommand >= EFsEmailUiCmdZoomSmallest )
                    {
                    TUint zoomLevelIndex = aCommand - EFsEmailUiCmdZoomSmallest;
                    SetZoomLevelIndexL( zoomLevelIndex );
                    }
                }
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Changes the MSK command depending on the focused element.
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::SetMskL()
    {
    FUNC_LOG;
    TBrCtlDefs::TBrCtlElementType elementType = iContainer->BrowserControlIf()->FocusedElementType();

    switch ( elementType )
        {
        case TBrCtlDefs::EElementTelAnchor:
        case TBrCtlDefs::EElementSmartLinkTel:
            ChangeMskCommandL( R_FSE_QTN_MSK_CALL );
            break;
        case TBrCtlDefs::EElementMailtoAnchor:
        case TBrCtlDefs::EElementSmartLinkEmail:
            ChangeMskCommandL( R_FSE_QTN_MSK_COMPOSE );
            break;
        case TBrCtlDefs::EElementAnchor:
            ChangeMskCommandL( R_FSE_QTN_MSK_OPENURL );
            break;
        case TBrCtlDefs::EElementNone:
        default:
            ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Hides the container and removes it from control stack.
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::HideContainer()
    {
    FUNC_LOG;

    if ( iContainer )
        {
        iContainer->MakeVisible( EFalse );
        iAppUi.RemoveFromStack( iContainer );
        }
    }

// -----------------------------------------------------------------------------
// Add the container to control stack and sets it visible.
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::ShowContainerL()
    {
    FUNC_LOG;

    if ( iContainer )
        {
        iAppUi.AddToStackL( iContainer );
        iContainer->MakeVisible( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::DoActivateL()
// Activate an Compose view
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::ChildDoActivateL( const TVwsViewId& /*aPrevViewId*/,
        TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    FUNC_LOG;
    
    TBool bodypartNotFound ( EFalse );
    
    if ( iMrObserverToInform && 
         aCustomMessageId == KStartViewerReturnToPreviousMsg )
        {
        // If returning from view launched by MRUI, complete the ongoing
        // async process.
        // Exception is the move message operation as it is not done through
        // normal async process for MR.
        if ( iOpResult.iOpType != EESMRCmdMailMoveMessage )
            {
            CompletePendingMrCommand();
            }
        else
            {
            CancelPendingMrCommandL();
            }
        }
    else if ( aCustomMessageId != KStartViewerReturnFromEmbeddedMsg )
        {
        // Cancel any pending asynchronous MR operation.
        CancelPendingMrCommandL();
        }

    // When returning from another view, we use previous activation data.
    // Otherwise it's read from the custom message.
    if ( aCustomMessageId != KHtmlViewerReturnToPrevious )
        {
        TPckgBuf<THtmlViewerActivationData> pckgData;
        pckgData.Copy( aCustomMessage );
        iActivationData = pckgData();
        }

    CleanupClosePushL( iActivationData.iFile );
    
    if ( !iMessage && iContainer )
        {
        iAppUi.RemoveFromStack( iContainer );
        iContainer->CancelFetch();
        iContainer->ClearCacheAndLoadEmptyContent();
        }
    
    if ( !iContainer )
        {
        iContainer = CFsEmailUiHtmlViewerContainer::NewL( iAppUi, *this );
        }
    else if ( iContainer ) // Container exists, make sure that it has a correct rect
        {
        iContainer->SetRect( ClientRect() );
        }

    // Check whether this activation is actually for a meeting request
    TBool openedInMrViewer( EFalse );    
    if ( THtmlViewerActivationData::EMailMessage == iActivationData.iActivationDataType )
        {
        openedInMrViewer = IsOpenedInMRViewerL();
        }

    if ( openedInMrViewer )
        {
        // The previous contents are cleared (otherwise they are shown just
        // before the meeting request viewer kicks in, especially annoying 
        // when navigating between prev/next meeting requests by using navi
        // decorator array buttons.
        iContainer->ClearCacheAndLoadEmptyContent();
        ClearMailViewer();
        }
    
    ShowContainerL();
        
    switch ( iActivationData.iActivationDataType )
        {
        case THtmlViewerActivationData::EMailMessage:
            {
                
            if ( iMessage && iAppUi.DownloadInfoMediator() )
                {
                iAppUi.DownloadInfoMediator()->StopObserving( this, iMessage->GetMessageId() );
                }
            
            //clean message stack which owns object iMessage
            EraseMessageStack();
            iMessage = NULL;
            iMessage = iAppUi.GetMailClient()->GetMessageByUidL(
            iActivationData.iMailBoxId,
            iActivationData.iFolderId,
            iActivationData.iMessageId,
            EFSMsgDataEnvelope  );
            
            UpdateToolbarL();

            if ( iMessage && iAppUi.DownloadInfoMediator() )
                {
                iAppUi.DownloadInfoMediator()->AddObserver( this, iMessage->GetMessageId() );
                }
            
            delete iAttachmentsListModel;
            iAttachmentsListModel = NULL;

            if ( iMessage )
                {
                if ( !( iMrObserverToInform && aCustomMessageId == KStartViewerReturnToPreviousMsg )
                     && aCustomMessageId != KStartViewerReturnFromEmbeddedMsg )
                    {
                    //take ownership of iMessage
                    PushMessageL( iMessage, EFalse );
                    }

                if ( openedInMrViewer )       
                    {
                    // Setting MSK empty. This blocks unwanted MSK keypresses before MrViewer is initialized.
                    ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
                    iMrUiActive = ETrue;
                    iAppUi.MrViewerInstanceL()->ExecuteViewL( *iMessage, *this );  
                    }                              
                else
            		{
		            iAttachmentsListModel = CFSEmailUiAttachmentsListModel::NewL( iAppUi, *this );
		            TPartData msgPartData( iActivationData.iMailBoxId, iActivationData.iFolderId, iActivationData.iMessageId);
		            iAttachmentsListModel->UpdateListL( msgPartData );
                       
            		TRAPD( error, LoadContentFromMailMessageL( iMessage ) );
            		if ( error == KErrNotFound || error == KErrInUse || error == KErrLocked )
            		    {
            		    bodypartNotFound = ETrue;
            		    }
            		else if ( error != KErrNone)
            		    {
            		    User::Leave( error );
            		    }
            		}
                
                }
            else
                {
                User::Leave( KErrNotFound );
                }
            
            if ( aCustomMessageId != KStartViewerReturnToPreviousMsg )
                {
                delete iMailBox; iMailBox = NULL;
                iMailBox = iAppUi.GetMailClient()->GetMailBoxByUidL( iMessage->GetMailBoxId() );
                
                // Change active mailbox if necessary
                if ( !iMessage->GetMailBoxId().IsNullId() )
                    {
                    iAppUi.SetActiveMailboxL( iMessage->GetMailBoxId(), EFalse );
                    }
                }

            }
        break;
        case THtmlViewerActivationData::EmbeddedEmailMessage:
            {
            if ( aCustomMessageId != KStartViewerReturnFromEmbeddedMsg && !iCreateNewMsgFromEmbeddedMsg )
                {
                PushMessageL(iActivationData.iEmbeddedMessage, ETrue);
                }
            
            iMessage = iOpenMessages->Head();
            iCreateNewMsgFromEmbeddedMsg = EFalse;
            
            delete iAttachmentsListModel;
            iAttachmentsListModel = NULL;
            iAttachmentsListModel = CFSEmailUiAttachmentsListModel::NewL( iAppUi, *this );
            
            if ( iOpenMessages->Head()== iOpenMessages->Last() )
                {
                TPartData msgPartData( iActivationData.iMailBoxId, iActivationData.iFolderId, iActivationData.iMessageId);
                iAttachmentsListModel->UpdateListL( msgPartData );
                                
                }
            else{
                iAttachmentsListModel->UpdateListL( iOpenMessages->Head() );         
                }                
                
    
            TRAPD( error, LoadContentFromMailMessageL( iOpenMessages->Head() ) );
            if ( error == KErrNotFound || error == KErrInUse || error == KErrLocked )
                {
                bodypartNotFound = ETrue;
                }
            else if ( error != KErrNone)
                {
                User::Leave( error );
                }
            
            // update tool bar as some action menu boton should not appear in embbeded messages.
            UpdateToolbarL();
            
            }
        break;
        case THtmlViewerActivationData::EFile:
            {
            LoadContentFromFileL( iActivationData.iFile );
            }
        break;
        case THtmlViewerActivationData::EFileName:
            {
            LoadContentFromFileL( iActivationData.iFileName );
            }
        break;
        case THtmlViewerActivationData::EUrl:
            {
            LoadContentFromUrlL( iActivationData.iUrl );
            }
        break;
        }
    
    UpdateNaviPaneL();

    CleanupStack::PopAndDestroy( &iActivationData.iFile );
    SetMskL();
    
    if(iMessage)
        {
        CFSMailAddress* address = iMessage->GetSender();
        if ( address )
            {
            HBufC* displayName = address->GetDisplayName().AllocLC();
            
            if ( displayName->Length() )
                {
                iAppUi.SetTitlePaneTextL( *displayName );
                }
            else
                {
                HBufC* emailAddress = address->GetEmailAddress().AllocLC();
                iAppUi.SetTitlePaneTextL(*emailAddress);
                CleanupStack::PopAndDestroy(emailAddress);
                }
            CleanupStack::PopAndDestroy( displayName );      
            }
        }
    
    if ( !openedInMrViewer )
        {
        CFSMailMessagePart* htmlBodyPart = iMessage->HtmlBodyPartL();
        TFetchedType type;
        if( htmlBodyPart )
            {
            type = EMessageHtmlBodyPart;
            }
        else
            {
            type = EMessagePlainTextBodyPart;
            }
        if( !MessageStructureKnown(*iMessage) )
            {
            StartWaitedFetchingL(EMessageStructure);
            iFetchingAlready = ETrue;
            }
        else if ( !MessagePartFullyFetchedL( type ) || bodypartNotFound )
            {
            iAsyncProcessComplete = EFalse;
            iFetchingAlready = EFalse;
            iStartAsyncFetchType = type;
            if(iMessage)
                {
                StartFetchingMessagePartL( *iMessage, type );
                }
            }
       }
    iNextOrPrevMessageSelected = EFalse;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::OfferToolbarEventL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::OfferToolbarEventL( TInt aCommand )
    {
    FUNC_LOG;
    switch (aCommand)
        {
        case EFsEmailUiTbCmdReply:
            {
            HandleCommandL(EFsEmailUiCmdActionsReply);
            break;
            }

        case EFsEmailUiTbCmdForward:
            {
            HandleCommandL(EFsEmailUiCmdActionsForward);
            break;
            }
        case EFsEmailUiTbCmdDelete:
            {
            HandleCommandL(EFsEmailUiCmdActionsDelete);
            break;
            }
        case EFsEmailUiTbCmdReplyAll:
            {
            HandleCommandL(EFsEmailUiCmdActionsReplyAll);
            break;
            }
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::ToolbarResourceId
// -----------------------------------------------------------------------------
TInt CFsEmailUiHtmlViewerView::ToolbarResourceId() const
    {
    TInt resourceId( R_FREESTYLE_EMAIL_UI_TOOLBAR_HTML_MESSAGE_VIEWER_NO_REPLY_ALL );
    if (iMessage)
        {
        if (!iActivationData.iEmbeddedMessageMode && 
                TFsEmailUiUtility::CountRecipientsSmart( iAppUi, iMessage ) > 1)
            {            
            resourceId = R_FREESTYLE_EMAIL_UI_TOOLBAR_HTML_MESSAGE_VIEWER;
            }
        }    
    return resourceId;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::GetInitiallyDimmedItemsL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::GetInitiallyDimmedItemsL( const TInt aResourceId,
                RArray<TInt>& aDimmedItems ) const
    {
    if ( R_FREESTYLE_EMAIL_UI_TOOLBAR_HTML_MESSAGE_VIEWER == aResourceId || 
         R_FREESTYLE_EMAIL_UI_TOOLBAR_HTML_MESSAGE_VIEWER_NO_REPLY_ALL == aResourceId )
        {
        if ( iActivationData.iEmbeddedMessageMode )
            {
            aDimmedItems.AppendL(EFsEmailUiTbCmdActions);
            aDimmedItems.AppendL(EFsEmailUiTbCmdDelete);
            }
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::ChildDoDeactivate()
// Deactivate the HTML view
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::ChildDoDeactivate()
    {
    FUNC_LOG;
    CancelFetchings();
   
    if ( iContainer )
        {
        HideContainer();
        iContainer->ResetContent();
        iAppUi.RemoveFromStack(iContainer);
        iContainer->CancelFetch();
        iContainer->ClearCacheAndLoadEmptyContent();
        }
    Toolbar()->SetToolbarObserver( this );
    iMessage = NULL;
    HideNaviPane();
    }

void CFsEmailUiHtmlViewerView::DoExitL()
    {
    FUNC_LOG;
    delete iMessage;
    iMessage = NULL;
    CancelFetchings();
    
    if ( iFlagSelectionHandler )
        {
        iFlagSelectionHandler->Cancel();
        delete iFlagSelectionHandler;
        iFlagSelectionHandler = NULL;
        }
    iAppUi.ReturnFromHtmlViewerL();
    }

// CFsEmailUiHtmlViewerView::NavigateBackL
// Overriden from base class
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::NavigateBackL()
    {
    FUNC_LOG;
    // clean up current message 
    if ( iMessage && iAppUi.DownloadInfoMediator() )
        {
        iAppUi.DownloadInfoMediator()->StopObserving( this, iMessage->GetMessageId() );
        }
    iContainer->StopObserving();
    CFSMailMessage* tmp = PopMessage();
    if( tmp )
        {
        delete tmp;
        tmp = NULL;
        }
    
    if ( iFlagSelectionHandler )
        {
        iFlagSelectionHandler->Cancel();
        delete iFlagSelectionHandler;
        iFlagSelectionHandler = NULL;
        }

    // Return to previous message in the stack if there is still some left
    if ( !iOpenMessages->IsEmpty() )
        {
       
        THtmlViewerActivationData htmlData; 
        if ( iOpenMessages->Head()!= iOpenMessages->Last() )
            {
            htmlData.iActivationDataType = THtmlViewerActivationData::EmbeddedEmailMessage;
            htmlData.iEmbeddedMessageMode = ETrue;
           
            }
        else
            {
            htmlData.iActivationDataType = THtmlViewerActivationData::EmbeddedEmailMessage;
            htmlData.iMailBoxId = iOpenMessages->Head()->GetMailBoxId();
            htmlData.iFolderId  = iOpenMessages->Head()->GetFolderId();
            htmlData.iMessageId = iOpenMessages->Head()->GetMessageId();
            }
        
        TPckgBuf<THtmlViewerActivationData> pckgData( htmlData );
        iAppUi.EnterFsEmailViewL( HtmlViewerId, KStartViewerReturnFromEmbeddedMsg, pckgData );
      
        }
    // In usual case we use the base view implementation
    else
        {
        
        iMessage = NULL;
        
        CancelFetchings();
             
        CFsEmailUiViewBase::NavigateBackL();
        }
    }

void CFsEmailUiHtmlViewerView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;

    if ( aResourceId == R_FSEMAILUI_HTMLVIEWER_MENUPANE )
        {
        
        if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
            {
            // remove help support in pf5250
            aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);      
            }   
        
        // Some commands are blocked when viewing embedded message object
        // or separate HTML file.
        const TBool blockCmds( !iMessage || iActivationData.iEmbeddedMessageMode );
        const TBool blockReplyALLCmd( !iMessage || iActivationData.iEmbeddedMessageMode ||
                TFsEmailUiUtility::CountRecipientsSmart( iAppUi, iMessage ) < 2 );       
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReply, blockCmds );
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReplyAll, blockReplyALLCmd );
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsForward, blockCmds );
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsDelete, blockCmds );
        
        TBool hideNext = !ShowNextMessageMenuInOptions();
        TBool blockNextCmd = !iMessage || iActivationData.iEmbeddedMessageMode || hideNext;
        aMenuPane->SetItemDimmed( EFsEmailUiCmdNextMessage, blockNextCmd );
        TBool hidePrev = !ShowPreviousMessageMenuInOptions();
        TBool blockPrevCmd = !iMessage || iActivationData.iEmbeddedMessageMode || hidePrev;
        aMenuPane->SetItemDimmed( EFsEmailUiCmdPreviousMessage, blockPrevCmd );
        }
    else if ( aResourceId == R_FSEMAILUI_HTMLVIEWER_SUBMENU_ZOOM_LEVEL &&
              iContainer && iContainer->BrowserControlIf() )
        {
        DynInitZoomMenuL( aMenuPane );
        }
    else if ( aResourceId == R_FSEMAILUI_MAILVIEWER_SUBMENU_MORE )
        {
        if( iActivationData.iEmbeddedMessageMode )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsRead, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsUnread, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsMoveMessage, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsFlag, ETrue );
            }
        else
            {
            CFSMailFolder* currentFolder = NULL;
            if ( iMessage )
                {
                TRAP_IGNORE( currentFolder = 
                    iAppUi.GetMailClient()->GetFolderByUidL(
                            iMessage->GetMailBoxId(), 
                            iMessage->GetFolderId() ) );
                }
          
            if ( currentFolder &&
                 currentFolder->GetFolderType() != EFSOutbox )
                {
                // Mark as read/unread options
                TBool messageIsRead( iMessage->IsFlagSet( EFSMsgFlag_Read ) );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsRead, messageIsRead );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsUnread, !messageIsRead );
                
                // Move to another folder option
                aMenuPane->SetItemDimmed( 
                        EFsEmailUiCmdActionsMoveMessage, 
                        !iAppUi.GetActiveMailbox()->HasCapability( EFSMBoxCapaMoveToFolder ) );

                // Follow-up flag option
                aMenuPane->SetItemDimmed( 
                        EFsEmailUiCmdActionsFlag,
                        !( iMailBox && TFsEmailUiUtility::IsFollowUpSupported( *iMailBox ) ) );
                }
            else
                {
                // In case of outbox, all these are dimmed
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsUnread, ETrue );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsRead, ETrue );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsMoveMessage, ETrue );
                aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsFlag, ETrue );
                }
            delete currentFolder;
            }
        aMenuPane->SetItemDimmed( EFsEmailUiCmdMessageDetails, ETrue );
        }
    iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane,
            CFSEmailUiShortcutBinding::EContextHtmlViewer );
    }
    
// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::HandleMrCommandL
// Handle accept/decline/tentative/remove commands given for meeting request
// message directly from list UI.
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::HandleMrCommandL(
    TInt aCommandId,
    TFSMailMsgId aMailboxId,
    TFSMailMsgId aFolderId,
    TFSMailMsgId aMessageId )
    {
    FUNC_LOG;
    ///any init required? 
    UpdateMessagePtrL( aMailboxId, aFolderId, aMessageId );

    if ( aCommandId == EFsEmailUiCmdCalRemoveFromCalendar )
        {
        iAppUi.MrViewerInstanceL()->RemoveMeetingRequestFromCalendarL(
            *iMessage, *this );
        }
    else
        {
        TESMRAttendeeStatus respondStatus;
        if ( aCommandId == EFsEmailUiCmdCalActionsAccept )
            {
            respondStatus = EESMRAttendeeStatusAccept;
            }
        else if ( aCommandId == EFsEmailUiCmdCalActionsTentative )
            {
            respondStatus = EESMRAttendeeStatusTentative;
            }
        else // ( aCommandId == EFsEmailUiCmdCalActionsDecline )
            {
            respondStatus = EESMRAttendeeStatusDecline;
            }
        iAppUi.MrViewerInstanceL()->ResponseToMeetingRequestL(
            respondStatus, *iMessage, *this );
        }
    }


void CFsEmailUiHtmlViewerView::HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType /*aType*/ )
    {
    FUNC_LOG;

    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

void CFsEmailUiHtmlViewerView::LoadContentFromFileL( const TDesC& aFileName )
    {
    FUNC_LOG;

    if ( iContainer )
        {
        iContainer->LoadContentFromFileL( aFileName );
        }
    }

void CFsEmailUiHtmlViewerView::LoadContentFromFileL( RFile& aFile )
    {
    FUNC_LOG;

    if ( iContainer )
        {
        iContainer->LoadContentFromFileL( aFile );
        }
    }

void CFsEmailUiHtmlViewerView::LoadContentFromUrlL( const TDesC& aUrl )
    {
    FUNC_LOG;

    if ( iContainer )
        {
        iContainer->LoadContentFromUrlL( aUrl );
        }
    }

void CFsEmailUiHtmlViewerView::LoadContentFromMailMessageL( CFSMailMessage* aMailMessage )
    {
    FUNC_LOG;

    if ( iContainer )
        {
        iContainer->LoadContentFromMailMessageL( aMailMessage );
        }
    }

void CFsEmailUiHtmlViewerView::DeleteMailL()
    {
    FUNC_LOG;

    if ( !iMessage || iActivationData.iEmbeddedMessageMode )
        {
        return;
        }

    TInt reallyDelete( ETrue );

    if ( iAppUi.GetCRHandler()->WarnBeforeDelete() && 
         !iMessage->IsFlagSet( EFSMsgFlag_CalendarMsg ) )
        {
        reallyDelete = TFsEmailUiUtility::ShowConfirmationQueryL(
                           R_FREESTYLE_EMAIL_UI_DELETE_MESSAGE_CONFIRMATION );
        }

    if ( reallyDelete )
        {
        // Reset container content, so that plugins may really delete files
        if ( iContainer )
            {
            HideContainer();
            iContainer->ResetContent();
            }

        RArray<TFSMailMsgId> msgIds;
        CleanupClosePushL( msgIds );
        msgIds.Append( iMessage->GetMessageId() );
        TFSMailMsgId mailBox = iMessage->GetMailBoxId();
        TFSMailMsgId folderId = iMessage->GetFolderId();
        iAppUi.GetMailClient()->DeleteMessagesByUidL( mailBox, folderId, msgIds );
        CleanupStack::PopAndDestroy( &msgIds );

        // Notify appui of deleted mail item
        SendEventToAppUiL( TFSEventMailDeleted );

        if ( iAppUi.CurrentActiveView()->Id() == HtmlViewerId )
            {
            NavigateBackL();
            }
        }
    }

// ---------------------------------------------------------------------------
// HandleMailBoxEventL
// Mailbox event handler, responds to events sent by the plugin.
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::HandleMailBoxEventL( TFSMailEvent aEvent,
        TFSMailMsgId aMailbox, TAny* aParam1, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    FUNC_LOG;

    if ( /*iFirstStartCompleted &&*/ iMessage && aMailbox.Id() == iAppUi.GetActiveMailboxId().Id() &&
                                     aEvent == TFSEventMailDeleted && aParam1 ) // Safety, in list events that only concern active mailbox are handled
        {
        TFSMailMsgId curMsgId = iMessage->GetMessageId();
        RArray<TFSMailMsgId>* removedEntries = static_cast<RArray<TFSMailMsgId>*>( aParam1 );
        // <cmail> break-keyword should be used only in switch-clauses
        TBool cont = ETrue;

        for ( TInt i = 0 ; i < removedEntries->Count() && cont; i++ )
            {
            if ( curMsgId == ( *removedEntries )[i] )
                {
                ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
                NavigateBackL();
                cont = EFalse;
                }
            }
        }
    if (iMessage && aMailbox.Id() == iAppUi.GetActiveMailboxId().Id() && aEvent == TFSEventNewMail)
        {
        UpdateNaviPaneL();
        }
    // </cmail>
    }

void CFsEmailUiHtmlViewerView::DynInitZoomMenuL( CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    TInt zoomLevelIdx = ZoomLevelIndexL();

    // Set the radio button state to match current zoom level
    if ( zoomLevelIdx >= 0 && zoomLevelIdx < KZoomLevelCount )
        {
        TInt curZoomLevel = zoomLevelIdx + EFsEmailUiCmdZoomSmallest;
        aMenuPane->SetItemButtonState( curZoomLevel, EEikMenuItemSymbolOn );
        }
    }

TInt CFsEmailUiHtmlViewerView::ZoomLevelIndexL()
    {
    FUNC_LOG;
    TInt zoomLevelIdx = iContainer->BrowserControlIf()->BrowserSettingL(
                            TBrCtlDefs::ESettingsCurrentZoomLevelIndex );

    // Behaviour of zooming in Browser Control Interface is different in version 7.1
    // than in previous versions and we need to support both. In older versions there
    // are 4 preset zoom levels while version 7.1 can zoom to any percent.
    RArray<TUint>* zoomLevels = iContainer->BrowserControlIf()->ZoomLevels();

    if ( !zoomLevels || !zoomLevels->Count() || ( *zoomLevels )[0] != KZoomLevels[0] )
        {
        // new browser:
        // BrowserControlIf gives zoom level percentage insted of index to array
        TBool found = EFalse;

        for ( TInt i = 0 ; i < KZoomLevelCount && !found ; ++i )
            {
            if ( zoomLevelIdx == KZoomLevels[i] )
                {
                zoomLevelIdx = i;
                found = ETrue;
                }
            }

        if ( !found )
            {
            zoomLevelIdx = KErrNotFound;
            }
        }

    return zoomLevelIdx;
    }

void CFsEmailUiHtmlViewerView::SetZoomLevelIndexL( TInt aIndex )
    {
    FUNC_LOG;
    TInt newValue = KMinTInt;

    // Behaviour of zooming in Browser Control Interface is different in version 7.1
    // than in previous versions and we need to support both. In older versions there
    // are 4 preset zoom levels while version 7.1 can zoom to any percent.
    RArray<TUint>* zoomLevels = iContainer->BrowserControlIf()->ZoomLevels();

    if ( !zoomLevels || !zoomLevels->Count() || ( *zoomLevels )[0] != KZoomLevels[0] )
        {
        // new browser:
        // BrowserControlIf takes zoom level percentage insted of index to array
        if ( aIndex >= 0 && aIndex < KZoomLevelCount )
            {
            newValue = KZoomLevels[aIndex];
            }
        }
    else
        {
        // old browser
        newValue = aIndex;
        }

    if ( newValue > KMinTInt )
        {
        iContainer->BrowserControlIf()->SetBrowserSettingL(
            TBrCtlDefs::ESettingsCurrentZoomLevelIndex, newValue );
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::HandleEmailAddressCommandL()
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::HandleEmailAddressCommandL( TInt aCommand, const TDesC& aEmailAddress )
    {
    FUNC_LOG;

    if ( !iAppUi.ViewSwitchingOngoing() )
        {
        switch ( aCommand )
            {
            case EFsEmailUiCmdActionsReply:
                {
                if ( iMessage && !iActivationData.iEmbeddedMessageMode )
                    {
                    delete iNewMailTempAddress;
                    iNewMailTempAddress = NULL;
                    iNewMailTempAddress = CFSMailAddress::NewL();
                    iNewMailTempAddress->SetEmailAddress( aEmailAddress );
                    iAppUi.LaunchEditorL( iNewMailTempAddress );
                    }
                }
            break;
            case FsEActionMenuAddToContacts:
                {
                SaveEmailAsContactL( aEmailAddress );
                }
            break;
            case EFsEmailUiCmdActionsCall:
                {
                CallAdressL( aEmailAddress, EFalse );
                }
            break;
            case EFsEmailUiCmdActionsCreateMessage:
                {
                CreateMessageL( aEmailAddress );
                }
            break;
            case EFsEmailUiCmdActionsContactDetails:
                {
                OpenContactDetailsL( aEmailAddress );
                }
            break;
            case EFsEmailUiCmdActionsRemoteLookup:
                {
                LaunchRemoteLookupL( aEmailAddress );
                }
            break;
            default:
                //nothing right now?
                break;
            }
        }
    }

void CFsEmailUiHtmlViewerView::SaveEmailAsContactL(
    const TDesC& aEmailAddress )
    {
    TAddToContactsType type;

    // Query to "update existing" or "Create new"
    // --> EFALSE = user chose "cancel"
    if ( CFsDelayedLoader::InstanceL()->GetContactHandlerL()->
            AddtoContactsQueryL( type ) )
        {
        // Create buffer and strip scheme data such as mailto: and call:
        HBufC* textData = aEmailAddress.AllocLC();
        CFsDelayedLoader::InstanceL()->GetContactHandlerL()->AddToContactL(
            *textData, EContactUpdateEmail, type, this );
        CleanupStack::PopAndDestroy( textData );
        }
    }

// ---------------------------------------------------------------------------
// From MFSEmailUiContactHandlerObserver
// The ownership of the CLS items in the contacts array is transferred to the
// observer, and they must be deleted by the observer.
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::OperationCompleteL(
    TContactHandlerCmd /*aCmd*/, const RPointerArray<CFSEmailUiClsItem>& /*aContacts*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// From MFSEmailUiContactHandlerObserver
// Handles error in contatct handler operation.
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::OperationErrorL(
    TContactHandlerCmd /*aCmd*/, TInt /*aError*/ )
    {
    FUNC_LOG;
    }


TBool CFsEmailUiHtmlViewerView::IsRemoteLookupSupportedL()
    {
    CFSMailBox* mailBox = iAppUi.GetMailClient()->GetMailBoxByUidL( iMessage->GetMailBoxId() );
    CleanupStack::PushL( mailBox );
    TBool isSupported = ( TFsEmailUiUtility::IsRemoteLookupSupported( *mailBox ) );
    CleanupStack::PopAndDestroy( mailBox );
    return isSupported;
    }

void CFsEmailUiHtmlViewerView::CallAdressL( const TDesC& aEmailAddress, TBool aVideoCall )
    {
    HBufC* emailAddress = aEmailAddress.AllocLC();

    if ( aVideoCall )
        {
        CFsDelayedLoader::InstanceL()->GetContactHandlerL()->SetVideoCall(
            ETrue );
        }

    CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCallToContactByEmailL( *emailAddress,
            iAppUi.GetActiveMailbox(), this, EFalse );
    CleanupStack::PopAndDestroy( emailAddress );
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::OpenContactDetailsL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::OpenContactDetailsL( const TDesC& aEmailAddress )
    {
    HBufC* emailAddress = aEmailAddress.AllocLC();
    CFsDelayedLoader::InstanceL()->GetContactHandlerL()->ShowContactDetailsL(
        *emailAddress, EContactUpdateEmail, NULL );

    CleanupStack::PopAndDestroy( emailAddress );
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::LaunchRemoteLookupL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::LaunchRemoteLookupL( const TDesC& aEmailAddress )
    {
    // this method assumes that remote lookup is available with current plugin.
    HBufC* textData = aEmailAddress.AllocLC();
    CFSMailBox* mailBox = iAppUi.GetMailClient()->GetMailBoxByUidL( iMessage->GetMailBoxId() );
    CleanupStack::PushL( mailBox );
    CFsDelayedLoader::InstanceL()->GetContactHandlerL()->LaunchRemoteLookupWithQueryL( *mailBox, *textData );
    CleanupStack::PopAndDestroy( mailBox );
    CleanupStack::PopAndDestroy( textData );
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::LaunchRemoteLookupL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::CreateMessageL( const TDesC& aEmailAddress )
    {
    HBufC* emailAddress = aEmailAddress.AllocLC();
    CFSEmailUiContactHandler* cntHandlerInstance = CFsDelayedLoader::InstanceL()->GetContactHandlerL();
    cntHandlerInstance->FindAndCreateMsgToContactByEmailL( *emailAddress, iAppUi.GetActiveMailbox() );
    CleanupStack::PopAndDestroy( emailAddress );
    }

CFSMailMessage* CFsEmailUiHtmlViewerView::CurrentMessage()
    {
    return iMessage;
    }

CFSEmailUiAttachmentsListModel* CFsEmailUiHtmlViewerView::CurrentAttachmentsListModel()
    {
    return iAttachmentsListModel;
    }

void CFsEmailUiHtmlViewerView::DownloadStatusChangedL( TInt /*aIndex*/ )
    {
    }

void CFsEmailUiHtmlViewerView::DownloadAttachmentL( const TAttachmentData& aAttachment )
    {
    iAttachmentsListModel->StartDownloadL(aAttachment);
    }

void CFsEmailUiHtmlViewerView::DownloadAllAttachmentsL()
    {
    iAttachmentsListModel->DownloadAllAttachmentsL();
    }

void CFsEmailUiHtmlViewerView::CancelAttachmentL( const TAttachmentData& aAttachment )
    {
    TInt reallyCancel = 
        TFsEmailUiUtility::ShowConfirmationQueryL( R_FSE_VIEWER_CANCEL_DOWNLOAD_QUERY );
    if ( reallyCancel )
        {
        iAttachmentsListModel->CancelDownloadL(aAttachment);
        }
    }

void CFsEmailUiHtmlViewerView::OpenAttachmentL( const TAttachmentData& aAttachment )
    {
    if ( iAppUi.DownloadInfoMediator()->IsDownloading( aAttachment.partData.iMessagePartId ) )
        {
        TFsEmailUiUtility::ShowInfoNoteL( R_FSE_VIEWER_NOTE_ATTACHMENT_DOWNLOADING_PROGRESS );
        }
    else if ( aAttachment.downloadProgress != KComplete )
        {
        DownloadAttachmentL( aAttachment );
        }
    else
        {
        TFsEmailUiUtility::OpenAttachmentL( aAttachment.partData );
        }
    }

void CFsEmailUiHtmlViewerView::SaveAttachmentL( const TAttachmentData& aAttachment )
    {
    TFileName fileName;
    if ( TFsEmailUiUtility::ShowSaveFolderDialogL( fileName ) )
        {
        TInt savedCount( 0 );
        TBool downloadRequired = iAttachmentsListModel->SaveAttachmentL( aAttachment, fileName, savedCount );
        if ( savedCount )
            {
            TFsEmailUiUtility::ShowFilesSavedToFolderNoteL( savedCount );        
            }
        }
    }

void CFsEmailUiHtmlViewerView::SaveAllAttachmentsL()
    {
    TFileName fileName;
    if ( TFsEmailUiUtility::ShowSaveFolderDialogL( fileName ) )
        {
        TBool downloadRequired = iAttachmentsListModel->SaveAllAttachmentsL( fileName );
        }
    }

void CFsEmailUiHtmlViewerView::RemoveAttachmentContentL( const TAttachmentData& aAttachment )
    {
    FUNC_LOG;
    TInt reallyClear = TFsEmailUiUtility::ShowConfirmationQueryL( R_FSE_VIEWER_CLEAR_FETCHED_QUERY );
    if ( reallyClear )
        {
        iAttachmentsListModel->RemoveContentL( aAttachment );
        }
    }

void CFsEmailUiHtmlViewerView::OpenAttachmentsListViewL()
    {
    HandleCommandL( EFsEmailUiCmdOpenAttachmentList );
    }

TBool CFsEmailUiHtmlViewerView::IsEmbeddedMsgView()
    {
    return iActivationData.iEmbeddedMessageMode;
    }

TBool CFsEmailUiHtmlViewerView::IsEmbeddedMsgSavingAllowed()
    {
    return iMailBox->HasCapability( 
				EFSMboxCapaSupportsSavingOfEmbeddedMessages );
    }

void CFsEmailUiHtmlViewerView::UpdateDownloadIndicatorL( 
        const TPartData& aPart, 
        const TFSProgress& aEvent )
    {
    FUNC_LOG;
    
    if ( iMessage )
        {
        if ( !iProgressBar )
            {
            iProgressBar = CFreestyleAttachmentDownloadProgressBar::NewL();
            }
        
        for ( TInt i=0; i < iAttachmentsListModel->GetModel().Count(); i++ )
            {
            if ( iAttachmentsListModel->GetModel()[i].partData == aPart )
                {
                TAttachmentData& attachment = CONST_CAST( TAttachmentData&, iAttachmentsListModel->GetModel()[i] );
                
                if ( aEvent.iError )
                    {
                    iProgressBar->HideL();
                    }
                else
                    {
                    switch ( aEvent.iProgressStatus )
                        {
                        case TFSProgress::EFSStatus_Status:
                            {
                            if ( aEvent.iMaxCount > 0 && aEvent.iCounter > 0 )
                                {
                                attachment.downloadProgress = KComplete * aEvent.iCounter / aEvent.iMaxCount;
                                }
                            else
                                {
                                attachment.downloadProgress = KNone;
                                }
                            iProgressBar->UpdateL( attachment );
                            }
                        break;
                        
                        case TFSProgress::EFSStatus_RequestComplete:
                            {
                            attachment.downloadProgress = KComplete;
                            iProgressBar->UpdateL( attachment );
                            }
                        break;
                        
                        case TFSProgress::EFSStatus_RequestCancelled:
                            {
                            iProgressBar->HideL();
                            }
                        break;
                        
                        default:                            
                            break;
                        }
                    }
                break;
                }
            }
        }
    }

void CFsEmailUiHtmlViewerView::RequestResponseL( const TFSProgress& aEvent, const TPartData& aPart )
    {
    FUNC_LOG;
    if ( iAppUi.CurrentActiveView() == this )
        {
        if ( iMessage && ( iMessage->GetMessageId().Id() == aPart.iMessageId.Id() ) )
            {
            UpdateDownloadIndicatorL( aPart, aEvent );
            }
        }
    }


// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::PushMessageL
// Message stack handling. Ownership of message is transferred when succesful.
// -----------------------------------------------------------------------------
// 
void CFsEmailUiHtmlViewerView::PushMessageL( CFSMailMessage* aMessage, TBool aIsEmbedded )
    {
    if ( !iOpenMessages || !iEmbeddedMessages )
        {
        User::Leave( KErrNotReady );
        }

    // We must ensure that push is done succesfully either to both stacks or to neither one.
    if ( aIsEmbedded )
        {
        iEmbeddedMessages->PushL( aMessage );
        }
    else
        {
        iEmbeddedMessages->PushL( NULL );
        }
    TRAPD( err, iOpenMessages->PushL( aMessage ) );
    if ( err )
        {
        iEmbeddedMessages->Pop();
        User::Leave( err );
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::PopMessage
// Message stack handling. Ownership of message is returned.
// -----------------------------------------------------------------------------
// 
CFSMailMessage* CFsEmailUiHtmlViewerView::PopMessage()
    {
    ASSERT( iOpenMessages && iEmbeddedMessages );
    ASSERT( iOpenMessages->Count() == iEmbeddedMessages->Count() );
    
    CFSMailMessage* msg = NULL;
    if ( !iOpenMessages->IsEmpty() )
        {
        msg = iOpenMessages->Pop();
        }
    if ( !iEmbeddedMessages->IsEmpty() )
        {
        iEmbeddedMessages->Pop();
        }
    return msg;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::EraseMessageStack
// Message stack handling. All messages in stack are deallocated
// -----------------------------------------------------------------------------
// 
void CFsEmailUiHtmlViewerView::EraseMessageStack()
    {
    ASSERT( iOpenMessages && iEmbeddedMessages );
    ASSERT( iOpenMessages->Count() == iEmbeddedMessages->Count() );

    iOpenMessages->ResetAndDestroy();
    iEmbeddedMessages->Reset();
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::SetMessageFollowupFlagL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::SetMessageFollowupFlagL()
    {
    FUNC_LOG;
    if ( iMessage && TFsEmailUiUtility::IsFollowUpSupported( *iMailBox ) )
        {
        TFollowUpNewState newState = TFsEmailUiUtility::SetMessageFollowupFlagL( *iMessage );
        if ( newState != EFollowUpNoChanges )
            {
            // Notify appui of changed mail item
            UpdateNaviPaneL();
            SendEventToAppUiL( TFSEventMailChanged );
            }
        }
    }
void CFsEmailUiHtmlViewerView::SendEventToAppUiL( TFSMailEvent aEventType )
    {
    if ( iMessage )
    	{
		RArray<TFSMailMsgId> msgIdArray;
		CleanupClosePushL( msgIdArray );
		msgIdArray.AppendL( iMessage->GetMessageId() );
		TFSMailMsgId folderId = iMessage->GetFolderId();
		iAppUi.EventL( aEventType,
					   iAppUi.GetActiveMailboxId(),
					   &msgIdArray, &folderId, NULL );
		CleanupStack::PopAndDestroy( &msgIdArray );
    	}
    }
// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::ChangeMsgReadStatusL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::ChangeMsgReadStatusL(
    TBool aRead, TBool /*aCmdFromMrui*/ )
    {
    FUNC_LOG;
    if ( iMessage && !iActivationData.iEmbeddedMessageMode )
        {
        if ( aRead )
            {
            // Send flags, local and server
            iMessage->SetFlag( EFSMsgFlag_Read );
            }
        else
            {
            // Send flags, local and server
            iMessage->ResetFlag( EFSMsgFlag_Read );
            }
        iMessage->SaveMessageL(); // Save read status
        SendEventToAppUiL( TFSEventMailChanged );
    
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::ShowNextMessageMenuInOptions
// -----------------------------------------------------------------------------
TBool CFsEmailUiHtmlViewerView::ShowNextMessageMenuInOptions() const
    {
    FUNC_LOG;
    TBool available = EFalse;
    // Next/previous message options are inavailable in the embedded mode
    if ( iMessage && !iActivationData.iEmbeddedMessageMode )    
        {
        TFSMailMsgId currentMsgId = iMessage->GetMessageId();
        TFSMailMsgId nextMsgId;
        TFSMailMsgId nextMsgFolderId;
        available = iAppUi.IsNextMsgAvailable( currentMsgId, nextMsgId, nextMsgFolderId );
        }
    return available;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::ShowNextMessageL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::ShowNextMessageL()
    {
    FUNC_LOG;
    if ( iMessage && !iActivationData.iEmbeddedMessageMode )
        {
        TFSMailMsgId currentMsgId = iMessage->GetMessageId();
        TFSMailMsgId nextMsgId;
        TFSMailMsgId nextMsgFolderId;
        if ( iAppUi.IsNextMsgAvailable( currentMsgId, nextMsgId, nextMsgFolderId ) )
            {
            // Stop timer and cancel fetchings before showing next message
            //iFetchingAnimationTimer->Stop();
            CancelFetchings();
           
            // Change empty msk when moving to next
            ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );

            iAppUi.MoveToNextMsgL( currentMsgId, nextMsgId );
            // Next message is displayed in this view through doactivate, because view is re-activate by mail list
            }
        }
    }
// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::ShowPreviousMessageMenuInOptions
// -----------------------------------------------------------------------------
TBool CFsEmailUiHtmlViewerView::ShowPreviousMessageMenuInOptions() const
    {
    FUNC_LOG;
    TBool available = EFalse;
    // Next/previous message options are inavailable in the embedded mode
    if ( iMessage && !iActivationData.iEmbeddedMessageMode )
        {
        TFSMailMsgId currentMsgId = iMessage->GetMessageId();
        TFSMailMsgId prevMsgId;
        TFSMailMsgId prevMsgFolderId;
        available = iAppUi.IsPreviousMsgAvailable( currentMsgId, prevMsgId, prevMsgFolderId );
        }
    return available;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::ShowPreviousMessageL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::ShowPreviousMessageL()
    {
    FUNC_LOG;
    if ( iMessage && !iActivationData.iEmbeddedMessageMode )
        {
        TFSMailMsgId currentMsgId = iMessage->GetMessageId();
        TFSMailMsgId prevMsgId;
        TFSMailMsgId prevMsgFolderId;
        if ( iAppUi.IsPreviousMsgAvailable( currentMsgId, prevMsgId, prevMsgFolderId ) )
            {
            // Stop timer and cancel fetchings before showing prev message
            CancelFetchings();
           
            // Change empty msk when moving to previous
            ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );

            iAppUi.MoveToPreviousMsgL( currentMsgId, prevMsgId );
            // Previous message is displayed in this view through doactivate, because view is re-activate by mail list
            }
        }
    }


// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::OpenFolderListForMessageMovingL
// -----------------------------------------------------------------------------
TBool CFsEmailUiHtmlViewerView::OpenFolderListForMessageMovingL()
    {
    FUNC_LOG;
    TBool ret = EFalse;
    // Ignore if mailbox doesn't support moving or we are viewing embedded message
    if ( iAppUi.GetActiveMailbox()->HasCapability( EFSMBoxCapaMoveToFolder ) &&
    !iActivationData.iEmbeddedMessageMode )
        {
        // Activate folder selection view and handle moving after callback gets destination
        iMoveToFolderOngoing = ETrue;
        TFolderListActivationData folderListData;
        folderListData.iCallback = this;
        CFSMailFolder* folder = iAppUi.GetMailClient()->GetFolderByUidL( iMessage->GetMailBoxId(), iMessage->GetFolderId() );
        folderListData.iSourceFolderType = TFSFolderType( folder->GetFolderType() );
        delete folder;
        const TPckgBuf<TFolderListActivationData> pkgOut( folderListData );
        iAppUi.EnterFsEmailViewL( FolderListId, KFolderListMoveMessage, pkgOut );
        ret = ETrue;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::FolderSelectedL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::FolderSelectedL(
    TFSMailMsgId aSelectedFolderId, TFSEmailUiCtrlBarResponse aResponse )
    {
    FUNC_LOG;

    if ( iMoveToFolderOngoing )
        {
        iMoveToFolderOngoing = EFalse;
        if ( !iMovingMeetingRequest )
            {
            switch ( aResponse )
                {
                case EFSEmailUiCtrlBarResponseSelect:
                    {
                    iMoveDestinationFolder = aSelectedFolderId;
                    iAsyncCallback->Cancel(); // cancel any outstanding callback just to be safe
                    iAsyncCallback->Set( TCallBack( MoveToFolderAndExitL, this ) );
                    iAsyncCallback->CallBack();
                    }
                    break;
                case EFSEmailUiCtrlBarResponseCancel:
                default:
                    break;
                }
            }
        else
            {
            iMovingMeetingRequest = EFalse;
            switch ( aResponse )
                {
                case EFSEmailUiCtrlBarResponseCancel:
                    iOpResult.iResultCode = KErrCancel;
                    break;
                case EFSEmailUiCtrlBarResponseSelect:
                    {
                    // Do moving here, do not exit, because mrui exists itself.
                    iOpResult.iResultCode = KErrNone;
                    iMoveDestinationFolder = aSelectedFolderId;
                    RArray<TFSMailMsgId> messageIds;
                    CleanupClosePushL( messageIds );
                    messageIds.Append( iActivationData.iMessageId );
                    // Trap is needed because protocol might return KErrNotSupported
                    // if move away from current folder is not supported
                    TRAPD(errMove, iAppUi.GetActiveMailbox()->MoveMessagesL( messageIds,
                            iActivationData.iFolderId, iMoveDestinationFolder ));
                    if ( errMove != KErrNotSupported )
                        {
                        if ( errMove == KErrNone )
                            {
                            TFsEmailUiUtility::DisplayMsgsMovedNoteL( 1, iMoveDestinationFolder, ETrue );
                            }
                        else
                            {
                            User::Leave( errMove );
                            }
                        }
                    CleanupStack::PopAndDestroy( &messageIds );
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::MoveToFolderAndExitL
// -----------------------------------------------------------------------------
TInt CFsEmailUiHtmlViewerView::MoveToFolderAndExitL( TAny* aMailViewerVisualiser )
    {
    FUNC_LOG;
    CFsEmailUiHtmlViewerView* self =
        static_cast<CFsEmailUiHtmlViewerView*>(aMailViewerVisualiser);

    RArray<TFSMailMsgId> messageIds;
    CleanupClosePushL( messageIds );
    messageIds.Append( self->iMessage->GetMessageId() );

    // Trap is needed because protocol might return KErrNotSupported
    // if move away from current folder is not supprted
    TRAPD(errMove, self->iAppUi.GetActiveMailbox()->MoveMessagesL( messageIds,
                                                     self->iMessage->GetFolderId(),
                                                     self->iMoveDestinationFolder ) );
    if ( errMove != KErrNotSupported )
        {
        if ( errMove == KErrNone )
            {
            // move successfull, display note
            TFsEmailUiUtility::DisplayMsgsMovedNoteL( 1, self->iMoveDestinationFolder, ETrue );
            }
        else
            {
            // Leave with any other err code than KErrNone or KErrNotSupported.
            User::Leave( errMove );
            }
        }
    CleanupStack::PopAndDestroy( &messageIds );

    // return to previous view
    self->HandleCommandL( EAknSoftkeyBack );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::MessagePartFullyFetchedL
// -----------------------------------------------------------------------------
TBool CFsEmailUiHtmlViewerView::MessagePartFullyFetchedL( TFetchedType aFetchedContentType ) const
    {
    FUNC_LOG;
    TBool retVal = ETrue;
    if( aFetchedContentType == EMessagePlainTextBodyPart )
        {
        CFSMailMessagePart* textPart = iMessage->PlainTextBodyPartL();
        if( textPart )
            {
            TFSPartFetchState currentPlainTextFetchState = textPart->FetchLoadState();
            if( currentPlainTextFetchState != EFSFull )
                {
                retVal = EFalse;
                }
            }
        delete textPart;
        }
    else if( aFetchedContentType == EMessageHtmlBodyPart )
        {
        CFSMailMessagePart* htmlPart = iMessage->HtmlBodyPartL();
        if( htmlPart )
            {
            TFSPartFetchState currentHtmlTextFetchState = htmlPart->FetchLoadState();
            if( currentHtmlTextFetchState != EFSFull )
                {
                retVal = EFalse;
                }
            }
        delete htmlPart;
        }
    else if( aFetchedContentType == EMessageStructure )
        {
        retVal = MessageStructureKnown( *iMessage );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    return retVal;
    }

//-----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::StartFetchingMessagePartL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::StartFetchingMessagePartL( CFSMailMessage& aMessagePtr,
                                            TFetchedType aFetchedContentType )
    {
    FUNC_LOG;
    if( aFetchedContentType == EMessagePlainTextBodyPart )
        {
        CFSMailMessagePart* textPart = aMessagePtr.PlainTextBodyPartL();
        CleanupStack::PushL( textPart );
        TFSMailMsgId textPartId = textPart->GetPartId();
        iCurrentPlainTextBodyFetchRequestId = textPart->FetchMessagePartL( textPartId, *this, 0 );
        iFetchingPlainTextMessageBody = ETrue;
        CleanupStack::PopAndDestroy( textPart );
        }
    else if( aFetchedContentType == EMessageHtmlBodyPart )
        {
        CFSMailMessagePart* htmlPart = aMessagePtr.HtmlBodyPartL();
        CleanupStack::PushL( htmlPart );
        TFSMailMsgId htmlPartId = htmlPart->GetPartId();
        iCurrentHtmlBodyFetchRequestId = htmlPart->FetchMessagePartL( htmlPartId, *this, 0 );
        iFetchingHtmlMessageBody = ETrue;
        CleanupStack::PopAndDestroy( htmlPart );
        }
    else if( aFetchedContentType == EMessageStructure )
        {
        StartFetchingMessageStructureL( aMessagePtr );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::RequestResponseL
// for MFSMailRequestObserver callback
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::RequestResponseL( TFSProgress aEvent, TInt aRequestId )
    {
    FUNC_LOG;
    
    TBool reloadContent ( EFalse );
    
    if ( aRequestId == iCurrentPlainTextBodyFetchRequestId && iFetchingPlainTextMessageBody )
        {
        if ( aEvent.iError != KErrNone ||
             aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled )
            {
            iAsyncProcessComplete = ETrue;
            iFetchingPlainTextMessageBody = EFalse;
            //iFetchingAnimationTimer->Stop();

            // Clear the "fetching body" text from the end of the message
            //iViewerRichText->SetEmptyStatusLayoutTextL();
            }
        else if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete )
            {
            iAsyncProcessComplete = ETrue;
            iFetchingPlainTextMessageBody = EFalse;
            //iFetchingAnimationTimer->Stop();

            // get message again, there might be new information (in case of POP protocol)
            if( iMessage )
                {
                TFSMailMsgId mailboxId = iMessage->GetMailBoxId();
                TFSMailMsgId folderId = iMessage->GetFolderId();
                TFSMailMsgId messageId = iMessage->GetMessageId();
                UpdateMessagePtrL( mailboxId, folderId, messageId );
                reloadContent = ETrue;
                }
                          
            }
        }

    else if ( aRequestId == iCurrentHtmlBodyFetchRequestId && iFetchingHtmlMessageBody )
        {
        if ( aEvent.iError != KErrNone ||
             aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled )
            {
            iAsyncProcessComplete = ETrue;
            iFetchingHtmlMessageBody = EFalse;
            }
        else if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete )
            {
            iAsyncProcessComplete = ETrue;
            iFetchingHtmlMessageBody = EFalse;           
            
            reloadContent = ETrue;
            
            }
        }
    else if ( aRequestId == iCurrentStructureFetchRequestId && iFetchingMessageStructure )
        {
        if ( aEvent.iError != KErrNone ||
             aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled )
            {
            iAsyncProcessComplete = ETrue;
            iFetchingMessageStructure = EFalse;
            }
        else if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete )
            {
            iAsyncProcessComplete = ETrue;
            iFetchingMessageStructure = EFalse;

            // get message again, there might be new information
            if( iMessage )
                {
                TFSMailMsgId mailboxId = iMessage->GetMailBoxId();
                TFSMailMsgId folderId = iMessage->GetFolderId();
                TFSMailMsgId messageId = iMessage->GetMessageId();
                UpdateMessagePtrL( mailboxId, folderId, messageId );
                reloadContent = ETrue;
                }
            }
        }
    
    if ( reloadContent )
        {
    
        if (  iContainer )
            {
            iContainer->ResetContent();
            }
                
        if( iMessage )
            {
            LoadContentFromMailMessageL( iMessage );  
            SetMskL();
            }
        }
            
    
    if(iAsyncProcessComplete && iWaitDialog && iDialogNotDismissed)
        {
        iWaitDialog->ProcessFinishedL(); // deletes the dialog
        }

    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::DialogDismissedL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::DialogDismissedL( TInt aButtonId )
    {
    FUNC_LOG;
    iDialogNotDismissed = EFalse;
    if( aButtonId == EAknSoftkeyCancel )
        {
        CancelFetchings();
        }

    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::UpdateMessagePtrL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::UpdateMessagePtrL( TFSMailMsgId aNewMailboxId,
                                                        TFSMailMsgId aNewFolderId,
                                                        TFSMailMsgId aNewMessageId )
    {
    FUNC_LOG;
    TBool messageChanged = ETrue;

    if ( iMessage )
        {
        // Check is the message going to be changed or are we just updating the
        // same message object which has been previously shown.
        messageChanged = ( iMessage->GetMessageId() != aNewMessageId );

        // stop observing downloads from the previous message in case the message was changed
        if ( iAppUi.DownloadInfoMediator() && messageChanged )
            {
            iAppUi.DownloadInfoMediator()->StopObserving( this, iMessage->GetMessageId() );
            }
        delete iMessage;
        iMessage = NULL;
        PopMessage();
        }

    iMessage = iAppUi.GetMailClient()->GetMessageByUidL( aNewMailboxId,
                                                         aNewFolderId,
                                                         aNewMessageId,
                                                         EFSMsgDataEnvelope  );
    if ( iMessage )
        {
        // Opened to viewer -> set message as read
        if ( !iMessage->IsFlagSet( EFSMsgFlag_Read ) )
            {
            iMessage->SetFlag( EFSMsgFlag_Read );
            iMessage->SaveMessageL(); // Save read status
            // Notify appui of changed mail item, it seems that protocols don't do this
            SendEventToAppUiL( TFSEventMailChanged );
            }
        // Start observing attachment downloads from the new message
        if ( iAppUi.DownloadInfoMediator() && messageChanged )
            {
            iAppUi.DownloadInfoMediator()->AddObserver( this, iMessage->GetMessageId() );
            }
        }
    
    UpdateToolbarL();
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::CancelFetchings
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::CancelFetchings()
    {
    FUNC_LOG;
    if( iFetchingPlainTextMessageBody )
        {
        TRAP_IGNORE( iAppUi.GetMailClient()->CancelL( iCurrentPlainTextBodyFetchRequestId ) );
        iFetchingPlainTextMessageBody = EFalse;
        }
    if( iFetchingHtmlMessageBody )
        {
        TRAP_IGNORE( iAppUi.GetMailClient()->CancelL( iCurrentHtmlBodyFetchRequestId ) );
        iFetchingHtmlMessageBody = EFalse;
        }
    if( iFetchingMessageStructure )
        {
        TRAP_IGNORE( iAppUi.GetMailClient()->CancelL( iCurrentStructureFetchRequestId ) );
        iFetchingMessageStructure = EFalse;
        }
    iAsyncProcessComplete = ETrue;
    
    //also cancel embedded images download in DownloadInfoMediator
    if ( iMailBox )
        {
        TRAP_IGNORE( iAppUi.DownloadInfoMediator()->CancelAllDownloadsL( iMailBox->GetId() ) );    
        }
    

    //<cmail>
    if(iWaitDialog && iDialogNotDismissed)
        TRAP_IGNORE(iWaitDialog->ProcessFinishedL()); // deletes the dialog
    //</cmail>
    }
// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::StartFetchingMessageStructureL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::StartFetchingMessageStructureL( CFSMailMessage& aMsg )
    {
    FUNC_LOG;
    TFSMailMsgId currentMailboxId = aMsg.GetMailBoxId();
    TFSMailMsgId currentMessageFolderId = aMsg.GetFolderId();
    CFSMailFolder* currentFolder  = iAppUi.GetMailClient()->GetFolderByUidL( currentMailboxId, currentMessageFolderId );
    CleanupStack::PushL( currentFolder );
    RArray<TFSMailMsgId> messageIds;
    CleanupClosePushL( messageIds );
    messageIds.Append( aMsg.GetMessageId() );
    iCurrentStructureFetchRequestId = currentFolder->FetchMessagesL( messageIds, EFSMsgDataStructure, *this );
    iFetchingMessageStructure = ETrue;
    CleanupStack::PopAndDestroy( &messageIds );
    CleanupStack::PopAndDestroy( currentFolder );
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::MessageStructureKnownL
// -----------------------------------------------------------------------------
TBool CFsEmailUiHtmlViewerView::MessageStructureKnown( CFSMailMessage& aMsg ) const
    {
    FUNC_LOG;
    return TFsEmailUiUtility::IsMessageStructureKnown( aMsg );
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::StartWaitedFetchingL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::StartWaitedFetchingL( TFetchedType aFetchedContentType )
    {
    FUNC_LOG;
    iAsyncProcessComplete = EFalse;
    //iFetchingAlready = EFalse;
    iStartAsyncFetchType = aFetchedContentType;
    //<cmail> in cmail we are using different wait dialog, since this wrapper dialog gives
    // problems in red key exit, when its active
    //delete iAsyncWaitNote; iAsyncWaitNote = NULL;
    //iAsyncWaitNote = CAknWaitNoteWrapper::NewL();
    // surpress start delay to prevent situation where wait note gets visible
    // only after the structure has been fetched
    //iAsyncWaitNote->ExecuteL( R_FSE_FETCHING_WAIT_DIALOG, *this, ETrue );
    //</cmail>

    //<cmail>
    /*iWaitDialog = new(ELeave)CAknWaitDialog(
                   (REINTERPRET_CAST(CEikDialog**,&iWaitDialog)), ETrue);
    iWaitDialog->SetCallback(this);
    iDialogNotDismissed = ETrue;
    iWaitDialog->ExecuteLD(R_FSE_FETCHING_WAIT_DIALOG);
    */
        StartFetchingMessagePartL( *iMessage, iStartAsyncFetchType );
      //  iFetchingAlready = ETrue;
    //</cmail>
    }

// ---------------------------------------------------------------------------
// CanProcessCommand
// Callback from meeting request UI. Determines what functionality is provided
// to mrui by this UI
// ---------------------------------------------------------------------------
//
TBool CFsEmailUiHtmlViewerView::CanProcessCommand(
    TESMRIcalViewerOperationType aCommandId  ) const
    {
    FUNC_LOG;
    TBool ret( EFalse );
    TInt numRecipients(0);
    switch ( aCommandId )
        {
        case EESMRCmdMailReply:
            ret = ETrue;
            break;
        case EESMRCmdMailReplyAll:
            //Get # of recipients
            if ( iMessage )
                {
                numRecipients=TFsEmailUiUtility::CountRecepients( iMessage );
                if ( numRecipients == 1 )
                    {
                    //check if the malbox ownmailaddress is same as the recipients email address. If not, then assume that the
                    //email is a distribution list and we need to inc num of Recipients so that "Reply ALL" option appears in UI.
                    if ( iMessage->GetToRecipients().Count() )
                        {
                        if ( iMailBox->OwnMailAddress().GetEmailAddress().Compare(iMessage->GetToRecipients()[0]->GetEmailAddress()) )
                            {
                            numRecipients++;
                            }
                        }
                    if ( iMessage->GetCCRecipients().Count() )
                        {
                        if ( iMailBox->OwnMailAddress().GetEmailAddress().Compare(iMessage->GetCCRecipients()[0]->GetEmailAddress()) )
                            {
                            numRecipients++;
                            }
                        }
                    if ( iMessage->GetBCCRecipients().Count() )
                        {
                        if ( iMailBox->OwnMailAddress().GetEmailAddress().Compare(iMessage->GetBCCRecipients()[0]->GetEmailAddress()) )
                            {
                            numRecipients++;
                            }
                        }
                    }
                }
            ret = ( iMessage && numRecipients > 1 );
            break;
        case EESMRCmdDownloadManager:
            //ret = ShowDownloadManagerMenuInOptions();
            break;
// <cmail>
        case EESMRCmdOpenAttachment:
        case EESMRCmdSaveAttachment:
        case EESMRCmdSaveAllAttachments:
        case EESMRCmdDownloadAttachment:
        case EESMRCmdDownloadAllAttachments:
// </cmail>
        case EESMRCmdOpenAttachmentView:
            ret = ETrue;
            break;
        case EESMRCmdMailComposeMessage:
            ret = ETrue;
            break;
        case EESMRCmdMailMarkUnread:
            {
            ret = EFalse;
            if ( iMessage && iMessage->IsFlagSet(EFSMsgFlag_Read) )
                {
                // Read, unread should be available
                ret = ETrue;
                }
            }
            break;
        case EESMRCmdMailMarkRead:
            {
            ret = EFalse;
            if ( iMessage && !iMessage->IsFlagSet(EFSMsgFlag_Read) )
                {
                // Read, unread should be available
                ret = ETrue;
                }
            }
            break;
        case EESMRCmdMailMoveMessage:
            {
            ret = EFalse;
            if ( iMessage )
                {
                // confirmed pointer exists, check whether mb has capa
                ret = iAppUi.GetActiveMailbox()->HasCapability( EFSMBoxCapaMoveToFolder );
                }
            }
            break;
        case EESMRCmdMailForwardAsMessage:
            ret = ETrue;
            break;
        case EESMRCmdMailFlagMessage:
            ret = TFsEmailUiUtility::IsFollowUpSupported( *iMailBox );
            break;
        case EESMRCmdMailMessageDetails:
            ret = ETrue;
            break;
        case EESMRCmdMailDelete:
            ret = ETrue;
            break;
        case EESMRCmdMailPreviousMessage:
            ret = ShowPreviousMessageMenuInOptions();
            break;
        case EESMRCmdMailNextMessage:
            ret = ShowNextMessageMenuInOptions();
            break;
        default:
            ret = EFalse;
            break;
        }
    return ret;
    }

    
void CFsEmailUiHtmlViewerView::ProcessAsyncCommandL( TESMRIcalViewerOperationType aCommandId,
                                                     const CFSMailMessage& aMessage,
                                                     MESMRIcalViewerObserver* aObserver )
{
FUNC_LOG;
if ( aObserver )
    {
    iMrObserverToInform = aObserver;
    }
// we must cast constness away from message because of flaws in MRUI API
CFSMailMessage* messagePtr = const_cast<CFSMailMessage*>(&aMessage);
if ( messagePtr )
    {
    // Fill in result struct
    iOpResult.iOpType = aCommandId;
    iOpResult.iMessage = messagePtr;
    iOpResult.iResultCode = KErrNotFound;

    switch ( aCommandId )
        {
        case EESMRCmdMailMessageDetails:
            {
            iOpResult.iResultCode = KErrNone;
            TMsgDetailsActivationData msgDetailsData;
            msgDetailsData.iMailBoxId = messagePtr->GetMailBoxId();
            msgDetailsData.iFolderId = messagePtr->GetFolderId();
            msgDetailsData.iMessageId = messagePtr->GetMessageId();
            const TPckgBuf<TMsgDetailsActivationData> pkgOut( msgDetailsData );
            iAppUi.EnterFsEmailViewL( MsgDetailsViewId, KStartMsgDetailsToBeginning,  pkgOut);
            }
            break;
        case EESMRCmdMailDelete:
            {
            if ( iMessage )
                {
                iDeletedMessageFromMrui = iMessage->GetMessageId(); //<cmail>
                DeleteMailL();
                iOpResult.iResultCode = KErrNone; //???? what to pass here
                }

            // Information is returned immediately after delete has completed.
            CompletePendingMrCommand();
            }
            break;
        case EESMRCmdMailMoveMessage:
            {
            if ( iMessage )
                {
                if ( OpenFolderListForMessageMovingL() )
                    {
                    iOpResult.iResultCode = KErrNone;
                    iMovingMeetingRequest = ETrue;
                    }
                }
            }
            break;
// <cmail>
        case EESMRCmdOpenAttachment:
            {
            // Check that message has attachments to display
            if ( messagePtr->IsFlagSet( EFSMsgFlag_Attachments ) )
                {
                iOpResult.iResultCode = KErrNone;
                //OpenAttachmentL();
                CompletePendingMrCommand();
                }
            }
            break;
        case EESMRCmdDownloadAttachment:
        case EESMRCmdDownloadAllAttachments:
            {
            // Check that message has attachments to display
            if ( messagePtr->IsFlagSet( EFSMsgFlag_Attachments ) )
                {
                iOpResult.iResultCode = KErrNone;
                //StartDowloadingAttachmentsL();
                CompletePendingMrCommand();
                }
            }
            break;
        case EESMRCmdSaveAttachment:
        case EESMRCmdSaveAllAttachments:
            {
            // Check that message has attachments to display
            if ( messagePtr->IsFlagSet( EFSMsgFlag_Attachments ) )
                {
                iOpResult.iResultCode = KErrNone;
                SaveAllAttachmentsL();
                CompletePendingMrCommand();
                }
            }
            break;
// </cmail>
        case EESMRCmdOpenAttachmentView:
            {
            // Check that message has attachments to display
            if ( messagePtr->IsFlagSet( EFSMsgFlag_Attachments ) )
                {
                iOpResult.iResultCode = KErrNone;
                TAttachmentListActivationData params;
                params.iMailBoxId = messagePtr->GetMailBoxId();
                params.iFolderId = messagePtr->GetFolderId();
                params.iMessageId = messagePtr->GetMessageId();
                TPckgBuf<TAttachmentListActivationData> buf( params );
                TUid emptyCustomMessageId = { 0 };
                iAppUi.EnterFsEmailViewL( AttachmentMngrViewId, emptyCustomMessageId, buf );
// <cmail>
                CompletePendingMrCommand();
// </cmail>
                }
            }
            break;
        case EESMRCmdDownloadManager:
            {
            // Check that there is something in dwnld manager to show
            //if ( ShowDownloadManagerMenuInOptions() )
              //  {
              //  iOpResult.iResultCode = KErrNone;
               // iAppUi.EnterFsEmailViewL( DownloadManagerViewId );
                //}
            }
            break;
        case EESMRCmdMailComposeMessage:
            {           
            iAppUi.CreateNewMailL();
            iOpResult.iResultCode = KErrNone;
            }
            break;
        case EESMRCmdMailReply:
        case EESMRCmdMailReplyAll:
        case EESMRCmdMailForwardAsMessage:
            {
            // Fill result codes for mrui
            iOpResult.iResultCode = KErrNone;
            // Fill launc params
            TEditorLaunchParams params;
            params.iMailboxId = iAppUi.GetActiveMailboxId();
            params.iActivatedExternally = EFalse;
            params.iMsgId = messagePtr->GetMessageId();
            if ( aCommandId == EESMRCmdMailForwardAsMessage )
                {
                iAppUi.LaunchEditorL( KEditorCmdForward, params );
                }
            else if ( aCommandId == EESMRCmdMailReply )
                {
                iAppUi.LaunchEditorL( KEditorCmdReply, params );
                }
            else if ( aCommandId == EESMRCmdMailReplyAll )
                {
                iAppUi.LaunchEditorL( KEditorCmdReplyAll, params );
                }
            }
            break;
        case EESMRCmdMailPreviousMessage:
            {
            iNextOrPrevMessageSelected = ETrue; // prevent back navigation when operation completed received
            ShowPreviousMessageL();
            iOpResult.iResultCode = KErrNone;
            CompletePendingMrCommand();
            }
            break;
        case EESMRCmdMailNextMessage:
            {
            iNextOrPrevMessageSelected = ETrue; // prevent back navigation when operation completed received
            ShowNextMessageL();
            iOpResult.iResultCode = KErrNone;
            CompletePendingMrCommand();
            }
            break;
        default:
            break;
        }
    }

    // Complete immediately if handling command failed. It makes no harm if following
    // function gets called several times in some case.
    if ( iOpResult.iResultCode < 0 )
        {
        CompletePendingMrCommand();
        }
    }
void CFsEmailUiHtmlViewerView::ProcessSyncCommandL(
    TESMRIcalViewerOperationType aCommandId,
    const CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    if ( &aMessage )
        {
        switch ( aCommandId )
            {
            case EESMRCmdMailMarkUnread:
                {
                ChangeMsgReadStatusL( EFalse, ETrue );
                }
                break;
            case EESMRCmdMailMarkRead:
                {
                ChangeMsgReadStatusL( ETrue, ETrue );
                }
                break;
            case EESMRCmdMailFlagMessage:
                {
                if ( iMessage )
                    {
                    if ( !iFlagSelectionHandler )
                        {
                        // Create when used for the first time
                        iFlagSelectionHandler =
                        CFlagSelectionNoteHandler::NewL( *this );
                        }
                    // Call to LaunchFlagListQueryDialogL will lead
                    // to CFlagSelectionNoteHandler::RunL() for handling the
                    // user's selection.
                    iFlagSelectionHandler->LaunchFlagListQueryDialogL();
                    }
                }
                break;
            default:
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::OperationCompleted()
// From MRUI observer
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::OperationCompleted( TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( aResult.iOpType == EESMRViewLaunch )
        {
        iMrUiActive = EFalse;
        if ( !iNextOrPrevMessageSelected && iAppUi.CurrentActiveView() == this )
            {
            TRAP_IGNORE( NavigateBackL() );
            }
        if ( aResult.iAttendeeStatus == EESMRAttendeeStatusAccept ||
             aResult.iAttendeeStatus == EESMRAttendeeStatusTentative )
            {
            TInt noteTextId = R_FREESTYLE_EMAIL_CONFIRM_NOTE_MEETING_ON_CALENDAR_AFTER_SYNC;
            TRAP_IGNORE( TFsEmailUiUtility::ShowGlobalInfoNoteL( noteTextId ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::OperationError()
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::OperationError( TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    // Show message in standard mail viewer in case launchin MRUI fails for any
    // other reason than viewer being cancelled by calling CancelPendingMrCommand().
    if ( aResult.iOpType == EESMRViewLaunch )
        {
        iMrUiActive = EFalse;
        
        // load mr content to viewer so we don't end up with empty view
        if (  iContainer )
             {
             iContainer->ResetContent();
             }
                 
         if( iMessage )
             {
             TRAP_IGNORE( LoadContentFromMailMessageL( iMessage ) );  
             TRAP_IGNORE( SetMskL() );
             }
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::CompletePendingMrCommand()
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::CompletePendingMrCommand()
    {
    FUNC_LOG;
    if ( iMrObserverToInform )
        {
        iMrObserverToInform->OperationCompleted( iOpResult );
        }
    iMrObserverToInform = NULL;
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::CancelPendingMrCommandL()
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::CancelPendingMrCommandL()
    {
    FUNC_LOG;
    // Be sure not to call cancel if the MRUI is not active. This is because
    // calling cancel after closing a newly fetched MR causes the MRUI to
    // be reopened.
    if ( iMrUiActive && iAppUi.MrViewerInstanceL() )
        {
        iAppUi.MrViewerInstanceL()->CancelOperation();
        }
    iMrUiActive = EFalse;
    iMrObserverToInform = NULL;
    }
    
// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::ClearMailViewer
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::ClearMailViewer()
    {
    FUNC_LOG;
    if ( iContainer )
        {
        iContainer->ResetContent();
        }
    }

// Helper funcitons to get viewed message ID and Folder id
TFSMailMsgId CFsEmailUiHtmlViewerView::ViewedMessageFolderId()
    {
    FUNC_LOG;
    TFSMailMsgId ret;
    if ( iMessage )
        {
        ret = iMessage->GetFolderId();
        }
    return ret;
    }

TFSMailMsgId CFsEmailUiHtmlViewerView::ViewedMessageId()
    {
    FUNC_LOG;
    TFSMailMsgId ret;
    if ( iMessage )
        {
        ret = iMessage->GetMessageId();
        }
    return ret;
    }

void CFsEmailUiHtmlViewerView::PrepareForExit()
    {
    FUNC_LOG;
    delete iNewMailTempAddress;
    iNewMailTempAddress = NULL;

    if ( iFlagSelectionHandler )
        {
        iFlagSelectionHandler->Cancel();
        delete iFlagSelectionHandler;
        iFlagSelectionHandler = NULL;
        }
    if ( iAppUi.DownloadInfoMediator() && iMessage )
        {
        iAppUi.DownloadInfoMediator()->StopObserving( this, iMessage->GetMessageId() );
        }
    delete iOpenMessages;
    iOpenMessages = NULL;
    delete iEmbeddedMessages;
    iEmbeddedMessages = NULL;
    iMessage = NULL;
    CancelFetchings();
   
    delete iMailBox;
    iMailBox = NULL;
    }

TBool CFsEmailUiHtmlViewerView::GetAsyncFetchStatus()
    {
    return ( iFetchingHtmlMessageBody || iFetchingMessageStructure || iFetchingPlainTextMessageBody );
    }
    
// ---------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::UpdateNaviPaneL
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::UpdateNaviPaneL( TBool aForESMR )
    {
    FUNC_LOG;
    // Get navigation decorator for mail viewer
    CAknNavigationDecorator* naviDecorator = iAppUi.NaviDecoratorL( MailViewerId );
    if ( naviDecorator )
        {
        // Check if there's next and previous message available, and set
        // the navipane arrows accordingly
        TFSMailMsgId currentMsgId = iMessage->GetMessageId();
        TFSMailMsgId tmpMsgId;
        TFSMailMsgId tmpMsgFolderId;
        if ( iAppUi.IsNextMsgAvailable( currentMsgId, tmpMsgId, tmpMsgFolderId ) )
            {
            naviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ERightButton, EFalse );
            }
        else
            {
            naviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ERightButton, ETrue );
            }
        if ( iAppUi.IsPreviousMsgAvailable( currentMsgId, tmpMsgId, tmpMsgFolderId ) )
            {
            naviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ELeftButton, EFalse );
            }
        else
            {
            naviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ELeftButton, ETrue );
            }

        // Set this view's navipane to the top of the navipane control stack
        CAknNavigationControlContainer* naviPaneContainer =
            static_cast<CAknNavigationControlContainer*>(
            StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
        naviPaneContainer->PushL( *naviDecorator );
        naviDecorator->SetNaviDecoratorObserver( aForESMR ? NULL : this );
        }

    // Set priority and follow-up flags
    CCustomStatuspaneIndicators* indicators = iAppUi.GetStatusPaneIndicatorContainer();
    if ( indicators)
        {
        indicators->ShowStatusPaneIndicators();

        if ( iMessage->IsFlagSet( EFSMsgFlag_Low ) )
            {
            indicators->SetPriorityFlag( EMsgPriorityLow );
            }
        else if ( iMessage->IsFlagSet( EFSMsgFlag_Important ) )
            {
            indicators->SetPriorityFlag( EMsgPriorityHigh );
            }
        else
            {
            indicators->SetPriorityFlag( EMsgPriorityNormal );
            }

        CCustomStatuspaneIndicators::TFollowUpFlagType flagType = CCustomStatuspaneIndicators::EFollowUpNone;
        if (iMessage->IsFlagSet(EFSMsgFlag_FollowUp))
            {
            flagType = CCustomStatuspaneIndicators::EFollowUp;
            }
        if (iMessage->IsFlagSet(EFSMsgFlag_FollowUpComplete))
            {
            flagType = CCustomStatuspaneIndicators::EFollowUpComplete;
            }
        indicators->SetFollowUpFlag( flagType );
        }

    }

// ---------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::HandleNaviDecoratorEventL
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::HandleNaviDecoratorEventL( TInt aEventID )
    {
    FUNC_LOG;
    CAknNavigationDecorator* naviDecorator = iAppUi.NaviDecoratorL( MailViewerId );
    if ( naviDecorator )
        {
        naviDecorator->SetNaviDecoratorObserver( NULL );
        }
    if( aEventID == MAknNaviDecoratorObserver::EAknNaviDecoratorEventRightTabArrow )
        {
        ShowNextMessageL();
        }
    else
        {
        ShowPreviousMessageL();
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::HideNaviPane
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::HideNaviPane()
    {
    FUNC_LOG;
    CAknNavigationControlContainer* naviPaneContainer = NULL;
    TRAP_IGNORE( naviPaneContainer = static_cast<CAknNavigationControlContainer*>(StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) ) );
    TRAP_IGNORE( naviPaneContainer->PushDefaultL() );

    CCustomStatuspaneIndicators* indicators = iAppUi.GetStatusPaneIndicatorContainer();
    if ( indicators)
        {
        indicators->HideStatusPaneIndicators();
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::IsOpenedInMRViewerL
// ---------------------------------------------------------------------------
//
TBool CFsEmailUiHtmlViewerView::IsOpenedInMRViewerL()
    {
    FUNC_LOG;
    TBool retVal( EFalse );
    CFSMailMessage* msg = iAppUi.GetMailClient()->GetMessageByUidL( 
            iActivationData.iMailBoxId, iActivationData.iFolderId,
            iActivationData.iMessageId, EFSMsgDataEnvelope );
            
    if ( msg )
        {
        CleanupStack::PushL( msg );
        if ( msg->IsFlagSet( EFSMsgFlag_CalendarMsg ) && 
            iAppUi.MrViewerInstanceL() )
            {
            TESMRMeetingRequestMethod mrMethod( EESMRMeetingRequestMethodUnknown );
            mrMethod = iAppUi.MrViewerInstanceL()->ResolveMeetingRequestMethodL( *msg );
            // Opened in MR viewer if request/cancellation or unknown MR
            retVal = 
                mrMethod == EESMRMeetingRequestMethodRequest ||
                mrMethod == EESMRMeetingRequestMethodCancellation ||
                mrMethod == EESMRMeetingRequestMethodUnknown;
            }
        CleanupStack::PopAndDestroy( msg );
        }
    return retVal;
    }


//////////////////////////////////////////////////////////////////////////////////////
// CLASS IMPLEMENTATION CHANGE TO FLAG DIALOG GLOBAL NOTE
///////////////////////////////////////////////////////////////////////////////////
CFlagSelectionNoteHandler* CFlagSelectionNoteHandler::NewL( CFsEmailUiHtmlViewerView& aViewerVisualiser )
    {
    FUNC_LOG;
    CFlagSelectionNoteHandler* self =
         new (ELeave) CFlagSelectionNoteHandler( aViewerVisualiser );
    return self;
    }


CFlagSelectionNoteHandler::~CFlagSelectionNoteHandler()
    {
    FUNC_LOG;
    delete iPrompt;
    delete iGlobalFlagQuery;
    }

void CFlagSelectionNoteHandler::Cancel()
    {
    FUNC_LOG;
    }

void CFlagSelectionNoteHandler::LaunchFlagListQueryDialogL()
    {
    FUNC_LOG;
    iSelection = 0;
    if ( !iPrompt )
        {
        iPrompt = StringLoader::LoadL( R_FREESTYLE_EMAIL_UI_VIEWER_FLAG_DIALOG_HEADER );
        }
    if ( !iGlobalFlagQuery )
        {
        iGlobalFlagQuery = CAknGlobalListQuery::NewL();
        iGlobalFlagQuery->SetHeadingL( *iPrompt ); // Set query heading.
        }

    CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat( 3 );
    CleanupStack::PushL( array );
    // Add follow up text
    HBufC* followUp = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FLAG_FOLLOW_UP );
    array->AppendL( *followUp );
    CleanupStack::PopAndDestroy( followUp );

    // Add flag complete text
    HBufC* completeFlag = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FLAG_COMPLETE );
    array->AppendL( *completeFlag );
    CleanupStack::PopAndDestroy( completeFlag );

    // Add clear flag text
    HBufC* clearFlag = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_FLAG_CLEAR );
    array->AppendL( *clearFlag );
    CleanupStack::PopAndDestroy( clearFlag );

    // Show query
    iGlobalFlagQuery->ShowListQueryL( array, iStatus, iSelection );
    CleanupStack::PopAndDestroy( array );

    // Set active
    SetActive();
    }

void CFlagSelectionNoteHandler::RunL()
    {
    FUNC_LOG;
    // Convert the selected index to enumeration.
    TFollowUpNewState newState = static_cast<TFollowUpNewState>( iStatus.Int() );
    // Update the message based on the new state.
    CFSMailMessage* message = iViewerView.CurrentMessage();
    if ( message )
        {
        TFsEmailUiUtility::SetMessageFollowupStateL( *message, newState );
        }
    }

void CFlagSelectionNoteHandler::DoCancel()
    {
    FUNC_LOG;
    }

TInt CFlagSelectionNoteHandler::RunError( TInt /*aError*/ )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    return err;
    }

CFlagSelectionNoteHandler::CFlagSelectionNoteHandler(  CFsEmailUiHtmlViewerView& aViewerVisualiser )
    : CActive ( EPriorityHigh ),
    iViewerView( aViewerVisualiser )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    iSelection = 0;
    }
