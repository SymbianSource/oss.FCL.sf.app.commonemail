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
#include <apgcli.h>
#include <apmstd.h>
#include <favouritesdb.h>
//<cmail>
#include <featmgr.h>
#include "cfsmailmessage.h"
#include "cfsmailclient.h"
#include <FreestyleEmailUi.rsg>
#include <schemehandler.h> // CSchemeHandler
#include <brctlinterface.h>
#include <csxhelp/cmail.hlp.hrh>
#include <baclipb.h> // for clipboard copy

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <txtclipboard.h>
#endif

// Meeting request
#include <MeetingRequestUids.hrh>
#include <layoutmetadata.cdl.h>
#include "cesmricalviewer.h"
#include "FreestyleEmailUiHtmlViewerView.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiHtmlViewerContainer.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "freestyleemailcenrephandler.h"
#include "FreestyleEmailUiAttachmentsListModel.h"
#include "FreestyleEmailUiConstants.h"
#include "FSEmail.pan"
#include "cfsmailcommon.h"

#include "FreestyleEmailUiMailViewerConstants.h"
#include "FSDelayedLoader.h"
#include "FreestyleMessageHeaderURL.h"
#include "FreestyleEmailUiAknStatusIndicator.h"

#include <eiksoftkeyimage.h>

CFsEmailUiHtmlViewerView* CFsEmailUiHtmlViewerView::NewL(
    CAlfEnv& aEnv,
    CFreestyleEmailUiAppUi& aAppUi,
    CAlfControlGroup& aControlGroup )
    {
    FUNC_LOG;
    CFsEmailUiHtmlViewerView* self = new ( ELeave ) CFsEmailUiHtmlViewerView(
        aEnv, aAppUi, aControlGroup );
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

    if ( iContainer )
        {
        iContainer->PrepareForExit();
        }

    delete iNewMailTempAddress;
    if ( iContainer )
        {
        iContainer->CancelFetch();
        }
    delete iContainer;
    delete iAttachmentsListModel;

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
    BaseConstructL( R_FSEMAILUI_HTML_VIEW_FULLSCREEN );
    iAsyncCallback = new (ELeave) CAsyncCallBack( CActive::EPriorityLow );
    iOpenMessages = new (ELeave) CStack<CFSMailMessage, ETrue>();
    iEmbeddedMessages = new (ELeave) CStack<CFSMailMessage, EFalse>();
    iNextOrPrevMessageSelected = EFalse;
    iForwardingMessage = EFalse;
    iMessageIsDeleted = EFalse;
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
                        HBufC* url = iContainer->BrowserControlIf()->PageInfoLC(TBrCtlDefs::EPageInfoFocusedNodeUrl);
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
                        CleanupStack::PopAndDestroy( url );
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
                TIMESTAMP( "Reply selected from html viewer" );
                if ( iMessage && !iActivationData.iEmbeddedMessageMode )
                    {
                    TEditorLaunchParams params;

                    params.iMailboxId = iAppUi.GetActiveMailboxId();
                    params.iMsgId = iMessage->GetMessageId();
                    params.iActivatedExternally = EFalse;
                    iAppUi.LaunchEditorL( KEditorCmdReply, params );
                    }
                }
            break;
            case EFsEmailUiCmdActionsReplyAll:
                {
                TIMESTAMP( "Reply to all selected from html viewer" );
                if ( iMessage && !iActivationData.iEmbeddedMessageMode )
                    {
                    TEditorLaunchParams params;
                    params.iMailboxId = iAppUi.GetActiveMailboxId();
                    params.iMsgId = iMessage->GetMessageId();
                    params.iActivatedExternally = EFalse;
                    iAppUi.LaunchEditorL( KEditorCmdReplyAll, params );
                    }
                }
            break;
            case EFsEmailUiCmdActionsForward:
                {
                TIMESTAMP( "Forward selected from html viewer" );
                if ( iMessage && !iActivationData.iEmbeddedMessageMode )
                    {
                    TEditorLaunchParams params;
                    params.iMailboxId = iAppUi.GetActiveMailboxId();
                    params.iMsgId = iMessage->GetMessageId();
                    params.iActivatedExternally = EFalse;
                    iForwardingMessage = ETrue;
                    iAppUi.LaunchEditorL( KEditorCmdForward, params );
                    }
                }
            break;
            case EFsEmailUiCmdActionsDelete:
                {
                TIMESTAMP( "Delete selected from html viewer" );
                DeleteMailL();
                }
            break;
            case EFsEmailUiCmdZoomIn:
                {
                iContainer->ZoomInL();
                }
            break;
            case EFsEmailUiCmdZoomOut:
                {
                iContainer->ZoomOutL();
                }
            break;
            case EFsEmailUiCmdHelp:
                {
                TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
                }
            break;
            case EFsEmailUiCmdExit:
                {
                TIMESTAMP( "Exit selected from html viewer" );
                // <cmail>
                iContainer->PrepareForExit();
                // </cmail>
                iAppUi.Exit();
                }
            break;
            case EFsEmailUiCmdOpenAttachmentList:
                {
                if( iMessage )
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
                /* -- pinch zoom only --
                if ( aCommand >= EFsEmailUiCmdZoomSmall )
                    {
                    iContainer->SetZoomLevelL( aCommand - EFsEmailUiCmdZoomSmall );
                    }
                */
                }
            break;
            }
        }
    TIMESTAMP( "Html viewer selected operation done" );
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
        iContainer->HideDownloadStatus();
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

// ---------------------------------------------------------------------------
//  hide or show Container ( used for activation of the view )
//
void CFsEmailUiHtmlViewerView::FadeOut(TBool aDirectionOut )
	{
	FUNC_LOG;
	iContainer->MakeVisible(!aDirectionOut);
	}


// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::DoActivateL()
// Activate an Compose view
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::ChildDoActivateL( const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    FUNC_LOG;

    TBool msgBodyStructurePresent ( EFalse );
    TBool msgBodyContentPresent ( EFalse );

    if( aCustomMessageId != KHtmlViewerReturnToPrevious)
     {
     ViewEntered( aPrevViewId );
     }

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
        iContainer->ResetContent();
        iAppUi.RemoveFromStack( iContainer );
        iContainer->CancelFetch();
        iContainer->ClearCacheAndLoadEmptyContent();
        }

    if ( !iContainer )
        {
        iContainer = CFsEmailUiHtmlViewerContainer::NewL( iAppUi, *this );
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
                    iContainer->DisplayStatusIndicatorL();
                    iAppUi.GetMailClient()->PrepareMrDescriptionL( iActivationData.iMailBoxId,
                                                                   iActivationData.iMessageId );

                    // Setting MSK empty. This blocks unwanted MSK keypresses before MrViewer is initialized.
                    ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
                    iMrUiActive = ETrue;
                    iAppUi.MrViewerInstanceL()->ExecuteViewL( *iMessage, *this );
                    }
                else
                    {
                    // Attachment list model is not created when opening mrViewer, so attachment options menu is
                    // not working with MR messages
                    iAttachmentsListModel = CFSEmailUiAttachmentsListModel::NewL( iAppUi, *this );
                    TPartData msgPartData( iActivationData.iMailBoxId, iActivationData.iFolderId, iActivationData.iMessageId );
                    iAttachmentsListModel->UpdateListL( msgPartData );
                    CheckMessageBodyL( *iMessage, msgBodyStructurePresent, msgBodyContentPresent );
                    LoadContentFromMailMessageL( iMessage, ETrue );
                    }
                }
            else
                {
                User::Leave( KErrNotFound );
                }

            if ( aCustomMessageId != KStartViewerReturnToPreviousMsg && iMessage )
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

            CheckMessageBodyL( *iOpenMessages->Head(), msgBodyStructurePresent, msgBodyContentPresent );
            LoadContentFromMailMessageL( iOpenMessages->Head(), ETrue );

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

    if ( !openedInMrViewer && iMessage)
        {
        CFSMailMessagePart* htmlBodyPart = iMessage->HtmlBodyPartL();
        TFetchedType type;
        if ( htmlBodyPart )
            {
            type = EMessageHtmlBodyPart;
            delete htmlBodyPart;
            }
        else
            {
            type = EMessagePlainTextBodyPart;
            }

        if ( !MessageStructureKnown(*iMessage) )
            {
            iContainer->DisplayStatusIndicatorL();
            StartWaitedFetchingL(EMessageStructure);
            iFetchingAlready = ETrue;
            }
        else
            {
            if ( msgBodyStructurePresent && !msgBodyContentPresent )
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
       }
    iNextOrPrevMessageSelected = EFalse;
    iForwardingMessage = EFalse;

    //update rect only if it has changed from previous time.
    if ( iContainer->Rect() != ContainerRect()  )
        {
        iContainer->SetRect( ContainerRect() );
        }

    TIMESTAMP( "Html viewer opened" );
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::ChildDoDeactivate()
// Deactivate the HTML view
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::ChildDoDeactivate()
    {
    FUNC_LOG;
    // Don't cancel fetching the message parts when forwarding the message:
    // it would also cancel the fetchings initiated by the forwarding,
    // causing the forwarding to abort.
    if ( !iForwardingMessage )
        {
        CancelFetchings();
        }

    if ( iContainer )
        {
        HideContainer();
        iContainer->ResetContent(ETrue);
        iAppUi.RemoveFromStack(iContainer);
        iContainer->CancelFetch();
        if ( !iAppUi.AppUiExitOngoing() )
            {
            // if app ui is exiting, a call to this function causes a KERN-EXEC 3 crash in iBrCtlInterface->ClearCache();
                iContainer->ClearCacheAndLoadEmptyContent();
            }
        }
    iMessage = NULL;
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
    if( iContainer )
        {
        iContainer->StopObserving();
        iContainer->ResetContent();
        }
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
            htmlData.iActivationDataType = THtmlViewerActivationData::EMailMessage;
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

        if ( iContainer )
            {
            HideContainer();
            iContainer->ResetContent();
            }
        }
    }

// ---------------------------------------------------------------------------
// Wrapper for implementation in view base class
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::ChangeMskCommandL( TInt /*aLabelResourceId*/ )
    {
    CEikButtonGroupContainer* cba( Cba() );
    if ( cba )
			{
      cba->SetCommandSetL( R_FREESTYLE_EMAUIL_UI_SK_OPTIONS_BACK_FULLSCREEN );
      CEikCba* eikCba( static_cast< CEikCba* >( cba->ButtonGroup() ) );
      TFileName filename;
			TFsEmailUiUtility::GetFullIconFileNameL( filename );
		  if (iActivationData.iEmbeddedMessageMode)
		  	{
	      eikCba->UpdateMSKIconL( KAknsIIDQsnIconColors,  filename,
	          EMbmFreestyleemailuiQgn_graf_cmail_blank,
	          EMbmFreestyleemailuiQgn_graf_cmail_blank_mask, ETrue );
		  	}
		  else
		    {
	      eikCba->UpdateMSKIconL( KAknsIIDQsnIconColors,  filename,
	          EMbmFreestyleemailuiQgn_prop_cmail_action_delete,
	          EMbmFreestyleemailuiQgn_prop_cmail_action_delete_mask, ETrue );
	      }
      }
    }

// ---------------------------------------------------------------------------
// Sets status bar layout
// ---------------------------------------------------------------------------
//
TBool CFsEmailUiHtmlViewerView::IsStatusPaneVisible() const
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Sets status bar layout
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::SetStatusBarLayout()
    {
    if( StatusPane()->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_IDLE_FLAT )
       {
       TRAP_IGNORE( StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_IDLE_FLAT ) );
       }
    }

void CFsEmailUiHtmlViewerView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;

    if ( aResourceId == R_FSEMAILUI_HTMLVIEWER_MENUPANE )
        {
        // Pinch zoom only
        aMenuPane->SetItemDimmed( EFsEmailUiCmdZoomLevel, ETrue );

        if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
            {
            // remove help support in pf5250
            aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);
            }

        const TBool toolbarExists( EFalse );

        // Some commands are blocked when viewing embedded message object
        // or separate HTML file.
        const TBool blockCmds( !iMessage || iActivationData.iEmbeddedMessageMode || toolbarExists );
        const TBool blockReplyALLCmd( toolbarExists || !iMessage || iActivationData.iEmbeddedMessageMode ||
                TFsEmailUiUtility::CountRecipientsSmart( iAppUi, iMessage ) < 2 );
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReply, blockCmds );
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReplyAll, blockReplyALLCmd );
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsForward, blockCmds );
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsDelete, blockCmds );

        if( iActivationData.iEmbeddedMessageMode )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsRead, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsUnread, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsMoveMessage, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsFlag, ETrue );
            }

        TBool hideNext = !ShowNextMessageMenuInOptions();
        TBool blockNextCmd = !iMessage || iActivationData.iEmbeddedMessageMode || hideNext;
        TInt menuPos;
        if( aMenuPane->MenuItemExists( EFsEmailUiCmdNextMessage, menuPos ) )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdNextMessage, blockNextCmd );
            }
        TBool hidePrev = !ShowPreviousMessageMenuInOptions();
        TBool blockPrevCmd = !iMessage || iActivationData.iEmbeddedMessageMode || hidePrev;
        if( aMenuPane->MenuItemExists( EFsEmailUiCmdPreviousMessage, menuPos ) )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdPreviousMessage, blockPrevCmd );
            }

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
    else if ( aResourceId == R_FSEMAILUI_HTMLVIEWER_SUBMENU_ZOOM_LEVEL &&
              iContainer && iContainer->BrowserControlIf() )
        {
        DynInitZoomMenuL( aMenuPane );
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

    if ( aCommandId == EFsEmailUiCmdCalRemoveFromCalendar && iMessage )
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
        if( iMessage )
            {
            iAppUi.MrViewerInstanceL()->ResponseToMeetingRequestL(
                respondStatus, *iMessage, *this );
            }
        }
    }


void CFsEmailUiHtmlViewerView::HandleViewRectChange()
    {
    CFsEmailUiViewBase::HandleViewRectChange();

     if ( iContainer )
         {
         iContainer->SetRect( ContainerRect() );
         }
    }

void CFsEmailUiHtmlViewerView::HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType aType )
    {
    FUNC_LOG;

    if ( aType == EScreenLayoutChanged )
        {
        SetStatusBarLayout();
        }

    if ( iContainer )
        {
        iContainer->SetRect( ContainerRect() );
        iContainer->HandleResourceChange( aType );
        }
    }

TRect CFsEmailUiHtmlViewerView::ContainerRect() const
    {
    TRect rect( ClientRect() );
    return rect;
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

void CFsEmailUiHtmlViewerView::LoadContentFromMailMessageL( CFSMailMessage* aMailMessage, TBool aResetScrollPos )
    {
    FUNC_LOG;

    if ( iContainer )
        {
        iContainer->LoadContentFromMailMessageL( aMailMessage, aResetScrollPos );
        }
    }

TInt CFsEmailUiHtmlViewerView::DeleteMail( TAny* aSelf )
    {
    FUNC_LOG;
    CFsEmailUiHtmlViewerView* self =
        static_cast<CFsEmailUiHtmlViewerView*>( aSelf );
    TRAP_IGNORE( self->DeleteMailL( EFalse ) );
    return KErrNone;
    }

void CFsEmailUiHtmlViewerView::DeleteMailL( TBool aSilentDelete )
    {
    FUNC_LOG;

    if ( !iMessage || iActivationData.iEmbeddedMessageMode )
        {
        return;
        }

    TFSMailMsgId currentMsgId = iMessage->GetMessageId();
    if ( iLastDeletedMessageID == currentMsgId )
        {
        return;
        }

    TInt reallyDelete( ETrue );

    if ( iAppUi.GetCRHandler()->WarnBeforeDelete() &&
         !aSilentDelete )
        {
        reallyDelete = TFsEmailUiUtility::ShowConfirmationQueryL(
                           R_FREESTYLE_EMAIL_UI_DELETE_MESSAGE_CONFIRMATION );
        }

    if ( reallyDelete )
        {
        CancelFetchings();

        // Reset container content, so that plugins may really delete files
        if ( iContainer )
            {
            HideContainer();
            iContainer->ResetContent();
            }

        RArray<TFSMailMsgId> msgIds;
        CleanupClosePushL( msgIds );

        msgIds.Append( currentMsgId );
        TFSMailMsgId mailBox = iMessage->GetMailBoxId();
        TFSMailMsgId folderId = iMessage->GetFolderId();

        //Get the id and check if there is a previous message available
        TFSMailMsgId prevMsgId;
        TFSMailMsgId prevMsgFolderId;

        //Get the previous message if it exists
        TBool available = iAppUi.IsPreviousMsgAvailable( currentMsgId,
                                                   prevMsgId,
                                                   prevMsgFolderId );

        //Delete the message
        iLastDeletedMessageID = iMessage->GetMessageId();
        iAppUi.GetMailClient()->DeleteMessagesByUidL( mailBox, folderId, msgIds );
        CleanupStack::PopAndDestroy( &msgIds );

        // Notify appui of deleted mail item
        SendEventToAppUiL( TFSEventMailDeletedFromViewer );


        if ( iAppUi.CurrentActiveView()->Id() == HtmlViewerId )
            {
            //Open the previous message or navigate back to list viewer
            if ( available )
                {
				iMessageIsDeleted = ETrue;
                TRAPD( err, iAppUi.MoveToPreviousMsgAfterDeleteL( prevMsgId ) );
                iMessageIsDeleted = EFalse;

                User::LeaveIfError( err );
                }
            else
                {
                ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
                NavigateBackL();
                }
			}
        //inform user that mail is deleted
        if (!aSilentDelete)
        	TFsEmailUiUtility::ShowDiscreetInfoNoteL( R_FREESTYLE_EMAIL_MAIL_DELETED );
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
                                     (aEvent == TFSEventMailDeleted || aEvent == TFSEventMailDeletedFromViewer) && aParam1 ) // Safety, in list events that only concern active mailbox are handled
        {
        TFSMailMsgId curMsgId = iMessage->GetMessageId();
        RArray<TFSMailMsgId>* removedEntries = static_cast<RArray<TFSMailMsgId>*>( aParam1 );
        TBool cont = ETrue;
        for ( TInt i = 0 ; i < removedEntries->Count() && cont; i++ )
            {
            if ( ( curMsgId == ( *removedEntries )[i] ) &&
                    (iDeletedMessageFromMrui != curMsgId) )
                {
                cont = EFalse;
                ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
                if( aEvent == TFSEventMailDeleted  && !iMessageIsDeleted )
                    {   //Delete event came from server; close the viewer.
                	HandleCommandL( EAknSoftkeyBack );
                	// The message we are viewing was deleted => stop here
                	return;
                    }
                }
            }
        }

    if ( iContainer && iMessage && aMailbox.Id() == iAppUi.GetActiveMailboxId().Id() )
        {
        if ( aEvent == TFSEventNewMail ||
             aEvent == TFSEventMailDeleted ||
             aEvent == TFSEventMailChanged )
            {
            iContainer->MailListModelUpdatedL();
            }

        // DSW fix for FAMZ-82YJQ2
        // Dismiss the download status dialog after sync has been finished or cancelled

        if ( aEvent == TFSEventMailboxSyncStateChanged )
            {
            TSSMailSyncState* newSyncState = static_cast<TSSMailSyncState*>( aParam1 );
            if ( newSyncState && ( *newSyncState == FinishedSuccessfully ||
                                   *newSyncState == SyncCancelled ||
                                   *newSyncState == Idle ||
                                   *newSyncState == SyncError ))
                {
                iContainer->HideDownloadStatus();
                }
            }
        }
    }

void CFsEmailUiHtmlViewerView::DynInitZoomMenuL( CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    /* -- Pinch zoom only --
    TInt zoomLevelIdx = iContainer->ZoomLevelL();

    // Set the radio button state to match current zoom level
    if ( zoomLevelIdx >= 0 && zoomLevelIdx < iContainer->MaxZoomLevel() )
        {
        TInt curZoomLevel = zoomLevelIdx + EFsEmailUiCmdZoomSmall;
        aMenuPane->SetItemButtonState( curZoomLevel, EEikMenuItemSymbolOn );
        }
     */
    aMenuPane->SetItemDimmed( EFsEmailUiCmdZoomSmall, ETrue );
    aMenuPane->SetItemDimmed( EFsEmailUiCmdZoomNormal, ETrue );
    aMenuPane->SetItemDimmed( EFsEmailUiCmdZoomLarge, ETrue );
    aMenuPane->SetItemDimmed( EFsEmailUiCmdZoom150Percent, ETrue );
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
                if ( iMessage )
                    {
                    if ( iActivationData.iEmbeddedMessageMode )
                        {
                        iCreateNewMsgFromEmbeddedMsg = ETrue;
                        }
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
            case EFsEmailUiCmdActionsAddContact:
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
            case EFsEmailUiCmdActionsCopyToClipboard:
                {
                CopyCurrentToClipBoardL( aEmailAddress );
                }
            break;
            default:
                //nothing right now?
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::HandleWebAddressCommandL()
// -----------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::HandleWebAddressCommandL( TInt aCommand, const TDesC& aUrl )
    {
    FUNC_LOG;
    switch ( aCommand )
        {
        case EFsEmailUiCmdActionsOpenWeb:
            {
            OpenLinkInBrowserL( aUrl );
            break;
            }
        case EFsEmailUiCmdActionsAddBookmark:
            {
            SaveWebAddressToFavouritesL( aUrl );
            break;
            }
        case EFsEmailUiCmdActionsCopyWWWAddressToClipboard:
            {
            CopyCurrentToClipBoardL( aUrl );
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
    iAttachmentsListModel->CancelDownloadL(aAttachment);
    }

void CFsEmailUiHtmlViewerView::CancelAllAttachmentsL()
    {
    FUNC_LOG;
    iAttachmentsListModel->CancelAllDownloadsL();
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
        if ( iContainer && !iContainer->AttachmentDownloadStatusVisible() )
            {
            iContainer->ShowAttachmentDownloadStatusL( TFSProgress::EFSStatus_Status, aAttachment );
            }
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
        if ( downloadRequired && iContainer && !iContainer->AttachmentDownloadStatusVisible() )
            {
            iContainer->ShowAttachmentDownloadStatusL( TFSProgress::EFSStatus_Status, aAttachment );
            }

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
        if ( downloadRequired && iContainer && !iContainer->AttachmentDownloadStatusVisible() )
            {
            for ( TInt i = 0; i < iAttachmentsListModel->GetModel().Count(); i++ )
                {
                const TAttachmentData& attachment = iAttachmentsListModel->GetModel()[i];
                if ( attachment.downloadProgress < KComplete )
                    {
                    iContainer->ShowAttachmentDownloadStatusL( TFSProgress::EFSStatus_Status, attachment );
                    break;
                    }
                }
            }
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

    if ( !iMessage || !iContainer || ( aEvent.iError != KErrNone ) )
        return;

    TAttachmentData* attachment = NULL;
    const RArray<TAttachmentData>& attachments = iAttachmentsListModel->GetModel();
    for ( TInt i=0; i < attachments.Count(); i++ )
        {
        if ( attachments[i].partData == aPart )
            {
            attachment = CONST_CAST( TAttachmentData*, &iAttachmentsListModel->GetModel()[i] );
            break;
            }
        }

    if ( attachment )
        {
        switch ( aEvent.iProgressStatus )
            {
            case TFSProgress::EFSStatus_Status:
                {
                if ( aEvent.iMaxCount > 0 && aEvent.iCounter > 0 )
                    {
                    attachment->downloadProgress = KComplete * aEvent.iCounter / aEvent.iMaxCount;
                    }
                else
                    {
                    attachment->downloadProgress = KNone;
                    }
                }
            break;

            case TFSProgress::EFSStatus_RequestComplete:
                {
                attachment->downloadProgress = KComplete;
                }
            break;

            default:
                break;
            }

        iContainer->ShowAttachmentDownloadStatusL( aEvent.iProgressStatus, *attachment );
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
            UpdateEmailHeaderIndicators();
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
            if (iContainer)
                {
                iContainer->PrepareForMessageNavigation();
                }

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
            if (iContainer)
                {
                iContainer->PrepareForMessageNavigation();
                }

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
                    iAsyncCallback->SetPriority( CActive::EPriorityLow );
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
    if( aFetchedContentType == EMessagePlainTextBodyPart && iMessage )
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
    else if( aFetchedContentType == EMessageHtmlBodyPart && iMessage )
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
    else if( aFetchedContentType == EMessageStructure && iMessage )
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
    iContainer->DisplayStatusIndicatorL();
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

    if ( iContainer )
        {
        iContainer->HideDownloadStatus();
        }

    if ( reloadContent )
        {
        if ( iContainer )
            {
            iContainer->ResetContent( EFalse, EFalse );
            if ( iMessage )
                {
                LoadContentFromMailMessageL( iMessage , EFalse );
                SetMskL();
                }
            }
        }

    if ( iAsyncProcessComplete && iWaitDialog && iDialogNotDismissed )
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
    if( iMessage )
        {
        StartFetchingMessagePartL( *iMessage, iStartAsyncFetchType );
        }
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
        //Changed to EFalse to fix defect EJZG-83CDRX
        case EESMRCmdMailMessageDetails:
            ret = EFalse;
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
                DeleteMailL(ETrue);
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
            if( !iAttachmentsListModel )
                {
                iAttachmentsListModel = CFSEmailUiAttachmentsListModel::NewL( iAppUi, *this );
                iAttachmentsListModel->UpdateListL( iOpenMessages->Head() );
                }
            // Check that message has attachments to display
            if ( messagePtr->IsFlagSet( EFSMsgFlag_Attachments ) )
                {
                iOpResult.iResultCode = KErrNone;
                DownloadAllAttachmentsL();
                CompletePendingMrCommand();
                }
            }
            break;
        case EESMRCmdSaveAttachment:
        case EESMRCmdSaveAllAttachments:
            {
            if( !iAttachmentsListModel )
                {
                iAttachmentsListModel = CFSEmailUiAttachmentsListModel::NewL( iAppUi, *this );
                iAttachmentsListModel->UpdateListL( iOpenMessages->Head() );
                }
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

void CFsEmailUiHtmlViewerView::ReloadPageL(  )
    {
    FUNC_LOG;
    if( iContainer )
        {
        iContainer->ReloadPageL();
        }
    }

TBool CFsEmailUiHtmlViewerView::GetAsyncFetchStatus()
    {
    return ( iFetchingHtmlMessageBody || iFetchingMessageStructure || iFetchingPlainTextMessageBody );
    }

// ---------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::UpdateEmailHeaderIndicatorsL
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerView::UpdateEmailHeaderIndicators()
    {
    FUNC_LOG;
    // Reload mail header.
    if ( iContainer )
        {
        iContainer->RefreshCurrentMailHeader();
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

// -----------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::CopyToClipBoardL
// -----------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::CopyCurrentToClipBoardL( const TDesC& aArgument ) const
    {
    FUNC_LOG;

    // Making sure that we are copying something to the clipboard
    if ( aArgument.Length() > 0 )
        {
        HBufC* clipBoardText = aArgument.AllocLC();
        CClipboard* cb = CClipboard::NewForWritingLC( CCoeEnv::Static()->FsSession() );
        cb->StreamDictionary().At( KClipboardUidTypePlainText );
        CPlainText* plainText = CPlainText::NewL();
        CleanupStack::PushL( plainText );
        plainText->InsertL( 0 , *clipBoardText );
        plainText->CopyToStoreL( cb->Store(), cb->StreamDictionary(), 0, plainText->DocumentLength() );
        CleanupStack::PopAndDestroy( plainText );
        cb->CommitL();
        CleanupStack::PopAndDestroy( cb );
        CleanupStack::PopAndDestroy( clipBoardText );
        }
    }

// --------------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::OpenLinkInBrowserL
// --------------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::OpenLinkInBrowserL( const TDesC& aUrl ) const
    {
    FUNC_LOG;

    // Create session
    RApaLsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );

    // Gets the default application UID for for the given MIME type
    TUid uid;
    TDataType dataType( _L8( "text/html" ) );
    session.AppForDataType( dataType, uid );
    
    if (uid != TUid::Uid(0) ) //only open link when default browser exist.
    	{
        TApaTaskList taskList( iEikonEnv->WsSession() );
        TApaTask task = taskList.FindApp( uid );
        if ( task.Exists() )
            {
            HBufC8 * param8 = HBufC8::NewLC( aUrl.Length() );
            param8->Des().Append( aUrl );
            task.SendMessage( TUid::Uid( 0 ), *param8 );
            CleanupStack::PopAndDestroy( param8 );
            }
        else
            {
            // Runs the default application using the dataType
            TThreadId threadId;
            User::LeaveIfError( session.StartDocument( aUrl , dataType, threadId ) );
            }
    	}
    CleanupStack::PopAndDestroy( &session );
    }

// --------------------------------------------------------------------------------
// CFsEmailUiHtmlViewerView::SaveWebAddressToFavouritesL
// --------------------------------------------------------------------------------
void CFsEmailUiHtmlViewerView::SaveWebAddressToFavouritesL( const TDesC& aUrl ) const
    {
    FUNC_LOG;
    HBufC* url = aUrl.AllocLC();

    RFavouritesSession fSession;
    User::LeaveIfError( fSession.Connect() );
    CleanupClosePushL( fSession );

    RFavouritesDb favourites;
    User::LeaveIfError( favourites.Open( fSession, KBrowserBookmarks ) );
    CleanupClosePushL( favourites );

    CFavouritesItem *favouritesItem = CFavouritesItem::NewLC();
    favouritesItem->SetType( CFavouritesItem::EItem );
    favouritesItem->SetUrlL( aUrl );
    // Should be less than KFavouritesMaxName ( 50 )
    if ( url->Length() > KFavouritesMaxName )
        {
        favouritesItem->SetNameL( url->Left( KFavouritesMaxName ) );
        }
    else
        {
        favouritesItem->SetNameL( *url );
        }

    favouritesItem->SetParentFolder( KFavouritesRootUid );

    TInt error = favourites.Add( *favouritesItem, ETrue );

    CleanupStack::PopAndDestroy( favouritesItem );
    CleanupStack::PopAndDestroy( &favourites );
    CleanupStack::PopAndDestroy( &fSession );

    CleanupStack::PopAndDestroy( url );

    if ( error == KErrNone )
        {
        TFsEmailUiUtility::ShowInfoNoteL(
            R_FREESTYLE_EMAIL_UI_VIEWER_BOOKMARK_ADDED, ETrue );
        }
    else // Error in bookmark creation, show could not complete message
        {
        TFsEmailUiUtility::ShowErrorNoteL(
            R_FREESTYLE_EMAIL_ERROR_GENERAL_UNABLE_TO_COMPLETE, ETrue );
        }
    }

/*
 * Delaying the fetch for MfE till the user scrolls to the bottom of
 * the message.
 */
void CFsEmailUiHtmlViewerView::StartFetchingMessageL()
    {
	if( iMessage )
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


        if ( !MessagePartFullyFetchedL( type ))
           {
           iAsyncProcessComplete = EFalse;
           iStartAsyncFetchType = type;
           //check to make sure we don't kick off fetch twice for the cases where bodypart
           //not found(OZ)
           if(iMessage && !GetAsyncFetchStatus())
               {
               StartFetchingMessagePartL( *iMessage, type );
               }
           }
       }
    }

void CFsEmailUiHtmlViewerView::CheckMessageBodyL( CFSMailMessage& /*aMessage*/, TBool& aMessageBodyStructurePresent, TBool& aMessageBodyContentPresent)
    {
    if( iMessage )
        {
        CFSMailMessagePart* bodyPart = iMessage->HtmlBodyPartL();
        if ( !bodyPart )
            {
            bodyPart = iMessage->PlainTextBodyPartL();
            }

        if ( bodyPart )
            {
            aMessageBodyStructurePresent = ETrue;
            CleanupStack::PushL( bodyPart );

            if ( bodyPart->FetchedContentSize() == 0 && bodyPart->ContentSize() != 0 )
                {
                aMessageBodyContentPresent = EFalse;
                }
            else
                {
                aMessageBodyContentPresent = ETrue;
                }

            CleanupStack::PopAndDestroy( bodyPart );
            }
        else
            {
            aMessageBodyStructurePresent = EFalse;
            aMessageBodyContentPresent = EFalse;
            }
        }
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

CFlagSelectionNoteHandler::CFlagSelectionNoteHandler( CFsEmailUiHtmlViewerView& aViewerVisualiser )
    : CActive ( EPriorityHigh ),
    iViewerView( aViewerVisualiser )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    iSelection = 0;
    }
