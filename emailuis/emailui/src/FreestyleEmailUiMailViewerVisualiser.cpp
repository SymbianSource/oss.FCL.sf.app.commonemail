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
*  Description : FreestyleEmailUi  list implementation
*
*/



// SYSTEM INCLUDES
#include "emailtrace.h"
#include <AknUtils.h>
#include <freestyleemailui.mbg>
#include <FreestyleEmailUi.rsg>
#include <StringLoader.h>
#include <AknBidiTextUtils.h>

#ifndef FF_CMAIL_INTEGRATION
#include <txtclipboard.h>
#endif // FF_CMAIL_INTEGRATION

//<cmail>
#include "fstextviewer.h"
#include "fsrichtext.h"
///</cmail>
#include <aknlistquerydialog.h>
//<cmail>
#include "fssmileydictionary.h"
///</cmail>
#include <commonphoneparser.h>
#include <browserlauncher.h>
//<cmail>
#include "CFSMailMessagePart.h"
#include "CFSMailClient.h"
//</cmail>
#include <apgcli.h>
//<cmail>
#include "CFSMailCommon.h"
//</cmail>
#include <featmgr.h>
//</cmail>

#include <txtfrmat.h>
#include <aknlists.h> // CAknSinglePopupMenuStyleListBox
#include <baclipb.h> // for clipboard copy
//<cmail>
#include "fstextviewerkeys.h"
#include "fstextviewerwatermark.h"
#include <alf/alfstatic.h>
#include <alf/alfroster.h>
#include <alf/alfevent.h>
#include <alf/alfcontrolgroup.h>
// </cmail>
//<cmail>
#include "AknWaitDialog.h"
//</cmail>
#include <finditemengine.h>
#include <AknGlobalListQuery.h>
#include <AknGlobalNote.h>

// Meeting request
#include <MeetingRequestUids.hrh>
//<cmail>
#include "cesmricalviewer.h"
//</cmail>

// for intranet application opening
#include <AiwServiceHandler.h>
#include <AiwMenu.h>

// for opening normal browser
#include <BrowserOverriddenSettings.h>
#include <BrowserLauncher.h>

// for bookmarks
#include <FavouritesLimits.h>   // KBrowserBookmarks
#include <FavouritesDb.h>

// for fonts
#include <AknFontAccess.h>
#include <AknLayoutFont.h>

// for action menu
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkContactEditorDlg.h>
#include <RPbkViewResourceFile.h>
#include <CPbkDataSaveAppUi.h>
#include <PbkDataSaveAppUi.hrh>
#include <CPbkFieldsInfo.h>

// <cmail>
#include "cfsccontactactionmenu.h"
#include "mfsccontactactionmenumodel.h"
#include "cfsccontactactionservice.h"
#include "cfsccontactactionmenuitem.h"
#include "fsccontactactionmenuuids.hrh"
#include "fsccontactactionmenudefines.h"
// </cmail>

// <cmail>
#include "cfsccontactactionservice.h"
#include <csxhelp/cmail.hlp.hrh>
// </cmail>
#include <aknnotewrappers.h>

#include <SchemeHandler.h> // CSchemeHandler

// <cmail>
#include "FreestyleEmailUiConstants.h"
//</cmail>
// INTERNAL INCLUDES
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiMailViewerControl.h"
#include "FreestyleEmailUiMailListVisualiser.h"
#include "FreestyleEmailUiFileSystemInfo.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailCenRepHandler.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiMsgDetailsVisualiser.h"
#include "FreestyleEmailUiFolderListVisualiser.h"
#include "FreestyleEmailUiAttachmentsListVisualiser.h"
#include "FreestyleEmailUiShortcutBinding.h"
#include "FreestyleEmailUiContactHandler.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiStatusIndicator.h"
#include "FSEmail.pan"
#include "FSDelayedLoader.h"

#include "FreestyleEmailUiMailViewerVisualiser.h"
#include "FreestyleEmailUiMailViewerRichText.h"
#include "FreestyleEmailUiMailViewerConstants.h"

#include <aknnavi.h>
#include <aknnavide.h>
#include <akntoolbar.h>
// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::NewL
// -----------------------------------------------------------------------------
CFSEmailUiMailViewerVisualiser* CFSEmailUiMailViewerVisualiser::NewL( CAlfEnv& aEnv, CFreestyleEmailUiAppUi& aAppUi, CAlfControlGroup& aMailViewerControlGroup )
    {
    FUNC_LOG;
    CFSEmailUiMailViewerVisualiser* self = CFSEmailUiMailViewerVisualiser::NewLC( aEnv, aAppUi, aMailViewerControlGroup );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::NewLC
// -----------------------------------------------------------------------------
CFSEmailUiMailViewerVisualiser* CFSEmailUiMailViewerVisualiser::NewLC( CAlfEnv& aEnv, CFreestyleEmailUiAppUi& aAppUi, CAlfControlGroup& aMailViewerControlGroup )
    {
    FUNC_LOG;
    CFSEmailUiMailViewerVisualiser* self = new (ELeave) CFSEmailUiMailViewerVisualiser( aEnv, aAppUi, aMailViewerControlGroup );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ConstructL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL( R_FSEMAILUI_MAIL_VIEWER_VIEW );
    iMessage = NULL;
    iNextOrPevMessageSelected = EFalse;
    iFirstStartCompleted = EFalse;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::DoFirstStartL()
// Purpose of this function is to do first start only when viewer is
// really needed to be shown. Implemented to make app start up faster.
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::DoFirstStartL()
    {
    FUNC_LOG;
    iOpenMessages = new (ELeave) CStack<CFSMailMessage, ETrue>();
    iEmbeddedMessages = new (ELeave) CStack<CFSMailMessage, EFalse>();
    iSmDictionary = CFsSmileyDictionary::NewL();
    iIBServiceHandler = CAiwServiceHandler::NewL();
    iIBServiceHandler->AttachMenuL( R_FSEMAILUI_MAILVIEWER_SUBMENU_ACTIONS, R_INTRANET_AIW_INTEREST_MENU );
    iIBServiceHandler->AttachL( R_INTRANET_AIW_INTEREST_BASE );

    // Initialize browser laucher api
    iFetchingAnimationTimer = CFSEmailUiGenericTimer::NewL( this );
    iAsyncCallback = new (ELeave) CAsyncCallBack( CActive::EPriorityLow );
    iDownloadProgressControlGroup = &iEnv.NewControlGroupL( KDownloadIndicatorControlGroup );
    iDownloadProgressIndicator = CFSEmailUiStatusIndicator::NewL( iEnv, *iDownloadProgressControlGroup,
                                                                  &iAppUi );

    iViewerRichText = CFSEmailUiMailViewerRichText::NewL( iAppUi );

    //here is set menu for msk
    MenuBar()->SetContextMenuTitleResourceId( R_FSEMAILUI_MAILVIEWER_MSK_MENUBAR );

    iFirstStartCompleted = ETrue;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CFSEmailUiMailViewerVisualiser
// -----------------------------------------------------------------------------
CFSEmailUiMailViewerVisualiser::CFSEmailUiMailViewerVisualiser( CAlfEnv& aEnv,
    CFreestyleEmailUiAppUi& aAppUi, CAlfControlGroup& aMailViewerControlGroup )
    : CFsEmailUiViewBase( aMailViewerControlGroup, aAppUi ),
    iMoveToFolderOngoing( EFalse ),
    iFetchingMessageStructure( EFalse ),
    iFetchingPlainTextMessageBody( EFalse ),
    iFetchingHtmlMessageBody( EFalse ),
    iAsyncProcessComplete( ETrue ),
    iEnv( aEnv )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::~CFSEmailUiMailViewerVisualiser
// -----------------------------------------------------------------------------
CFSEmailUiMailViewerVisualiser::~CFSEmailUiMailViewerVisualiser()
    {
    FUNC_LOG;
        //<cmail> notewrapper is replaced by iWaitDialog
    //delete iAsyncWaitNote;
        //</cmail>
    if( iIBServiceHandler )
        {
        iIBServiceHandler->Reset();
        delete iIBServiceHandler;
        }

    if ( iAsyncCallback )
        {
        iAsyncCallback->Cancel();
        delete iAsyncCallback;
        }

    iDownloadProgressControlGroup = NULL; // owned by AlfEnv
    }

void CFSEmailUiMailViewerVisualiser::PrepareForExit()
    {
    FUNC_LOG;
    delete iSmDictionary;
    iSmDictionary = NULL;
    delete iNewMailTempAddress;
    iNewMailTempAddress = NULL;

    // Text viewer control need to be removed control group and deleted here,
    // because akn physics (used by text viewer control) object's destructor
    // ends up calling CoeEnv->AppUi, so AppUi must be alive when deleting
    // text viewer control
    if ( iTextViewerControl )
        {
        ControlGroup().Remove( iTextViewerControl );
        }

    delete iTextViewer;
    iTextViewer = NULL;

    delete iTextViewerControl;
    iTextViewerControl = NULL;

    delete iViewerRichText;
    iViewerRichText = NULL;

    if ( iFlagSelectionHanler )
        {
        iFlagSelectionHanler->Cancel();
        delete iFlagSelectionHanler;
        iFlagSelectionHanler = NULL;
        }
    if ( iFetchingAnimationTimer )
        {
        iFetchingAnimationTimer->Stop();
        delete iFetchingAnimationTimer;
        iFetchingAnimationTimer = NULL;
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

    delete iMailBox;
    iMailBox = NULL;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::HandleForegroundEventL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::HandleForegroundEventL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) //Safety
        {
        // Resize the download indicator after returning from the screensaver
        iDownloadProgressIndicator->HandleForegroundEventL();
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::RequestResponseL
// for MFSEmailDownloadInformationObserver callback
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::RequestResponseL( const TFSProgress& aEvent, const TPartData& /*aPart*/ )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && iAppUi.CurrentActiveView() == this )
        {
        UpdateDownloadIndicatorL( aEvent.iProgressStatus );

        // <cmail> Action menu is dismissed if it's opened on an attachment
        // and the download has completed (to prevent canceling after completion).
        SViewerHeadingHotSpotData currentHeaderHotSpotData;
        CFindItemEngine::SFoundItem currentBodyHotSpotData;

        THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL(
            currentHeaderHotSpotData, currentBodyHotSpotData );

        if( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete &&
            hotspotType == EHeaderHotspot &&
            (currentHeaderHotSpotData.iType == ETypeAttachment ||
            currentHeaderHotSpotData.iType == ETypeAttachments) )
            {
            CFSEmailUiActionMenu::Dismiss( ETrue );

            // close actions menu if open
            CEikMenuBar* menu = MenuBar();

            if( menu->IsDisplayed() )
                {
                menu->MenuPane()->CloseCascadeMenu();
                }
            }
        // </cmail>
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::RequestResponseL
// for MFSMailRequestObserver callback
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::RequestResponseL( TFSProgress aEvent, TInt aRequestId )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        if ( aRequestId == iCurrentPlainTextBodyFetchRequestId && iFetchingPlainTextMessageBody )
            {
            if ( aEvent.iError != KErrNone ||
                 aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled )
                {
                iAsyncProcessComplete = ETrue;
                iFetchingPlainTextMessageBody = EFalse;
                iFetchingAnimationTimer->Stop();

                // Clear the "fetching body" text from the end of the message
                iViewerRichText->SetEmptyStatusLayoutTextL();
                }
            else if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete )
                {
                iAsyncProcessComplete = ETrue;
                iFetchingPlainTextMessageBody = EFalse;
                iFetchingAnimationTimer->Stop();

                // get message again, there might be new information (in case of POP protocol)
                TFSMailMsgId mailboxId = iMessage->GetMailBoxId();
                TFSMailMsgId folderId = iMessage->GetFolderId();
                TFSMailMsgId messageId = iMessage->GetMessageId();
                UpdateMessagePtrL( mailboxId, folderId, messageId );

                // Refresh viewer to show newly fetched content.
                // Prevent screen flickering by stopping automatic screen refresh during the update
                iEnv.SetRefreshMode( EAlfRefreshModeManual );
                // Make sure we don't leave and forget the refresh mode as manual.
                TRAP_IGNORE(
                    // Try to preserve the scroll position on the same line.
                    iAppUi.Display().Roster().Hide( ControlGroup() );
                    TInt firstLine = iTextViewer->GetFirstDisplayedLine();
                    RefreshL();
                    iTextViewer->FocusLineL( firstLine, ETrue );
                    iAppUi.Display().Roster().ShowL( ControlGroup() );
                    );
                iEnv.SetRefreshMode( EAlfRefreshModeAutomatic );
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
                LaunchHtmlViewerL();
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
                TFSMailMsgId mailboxId = iMessage->GetMailBoxId();
                TFSMailMsgId folderId = iMessage->GetFolderId();
                TFSMailMsgId messageId = iMessage->GetMessageId();

                UpdateMessagePtrL( mailboxId, folderId, messageId );
                if ( iMessage )
                    {
                    // Stop automatic screen refresh temporarily to prevent flickering
                    iEnv.SetRefreshMode( EAlfRefreshModeManual );
                    // we must not leave before the update mode has been restored
                    TRAP_IGNORE(
                        iAppUi.Display().Roster().Hide( ControlGroup() );
                        RefreshL( ETrue );
                        iAppUi.Display().Roster().ShowL( ControlGroup() );
                        );
                    iEnv.SetRefreshMode( EAlfRefreshModeAutomatic );
                    }
                }
            }
        }
    //<cmail>
    if(iAsyncProcessComplete && iWaitDialog && iDialogNotDismissed)
        iWaitDialog->ProcessFinishedL(); // deletes the dialog
    //</cmail>
    }


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::FolderSelectedL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::FolderSelectedL(
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
                    messageIds.Append( iMessage->GetMessageId() );
                    // Trap is needed because protocol might return KErrNotSupported
                    // if move away from current folder is not supprted
                    TRAPD(errMove, iAppUi.GetActiveMailbox()->MoveMessagesL( messageIds,
                                                      iMessage->GetFolderId(), iMoveDestinationFolder ));
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

// ---------------------------------------------------------------------------
// From MFSEmailUiContactHandlerObserver
// The ownership of the CLS items in the contacts array is transferred to the
// observer, and they must be deleted by the observer.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::OperationCompleteL(
    TContactHandlerCmd /*aCmd*/, const RPointerArray<CFSEmailUiClsItem>& /*aContacts*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// From MFSEmailUiContactHandlerObserver
// Handles error in contatct handler operation.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::OperationErrorL(
    TContactHandlerCmd /*aCmd*/, TInt /*aError*/ )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::MoveToFolderAndExitL
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerVisualiser::MoveToFolderAndExitL( TAny* aMailViewerVisualiser )
    {
    FUNC_LOG;
    CFSEmailUiMailViewerVisualiser* self =
        static_cast<CFSEmailUiMailViewerVisualiser*>(aMailViewerVisualiser);

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
// CFSEmailUiMailViewerVisualiser::OpenFolderListForMessageMovingL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::OpenFolderListForMessageMovingL()
    {
    FUNC_LOG;
    TBool ret = EFalse;
    // Ignore if mailbox doesn't support moving or we are viewing embedded message
    if ( iAppUi.GetActiveMailbox()->HasCapability( EFSMBoxCapaMoveToFolder ) &&
            !iEmbeddedMessageMode )
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
// CFSEmailUiMailViewerVisualiser::ViewerControl
// -----------------------------------------------------------------------------
CAlfControl* CFSEmailUiMailViewerVisualiser::ViewerControl()
    {
    FUNC_LOG;
    return iTextViewerControl;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::Id
// -----------------------------------------------------------------------------
TUid CFSEmailUiMailViewerVisualiser::Id() const
    {
    FUNC_LOG;
    return MailViewerId;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ChildDoActivateL
// -----------------------------------------------------------------------------
// <cmail> Toolbar
/*void CFSEmailUiMailViewerVisualiser::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )*/
// </cmail> Toolbar
void CFSEmailUiMailViewerVisualiser::ChildDoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    FUNC_LOG;

    // Read the activation parameters
    TMsgViewerActivationData activationData;
    if ( &aCustomMessage && aCustomMessage.Length() )
        {
        TPckgBuf<TMsgViewerActivationData> pkgBuf;
        pkgBuf.Copy( aCustomMessage );
        activationData = pkgBuf();
        }

    // Take ownership of the passed message pointer. No pointer should be passed
    // in other than embedded message cases.
    // No leaving functions may be called before this step.
    if ( aCustomMessageId != KStartViewerWithEmbeddedMsgPtr && activationData.iEmbeddedMessage )
        {
        delete activationData.iEmbeddedMessage;
        activationData.iEmbeddedMessage = NULL;
        __ASSERT_DEBUG( EFalse, Panic(EFSEmailUiUnexpectedValue) );
        }

    CleanupStack::PushL( activationData.iEmbeddedMessage );

    if ( !iFirstStartCompleted )
        {
        DoFirstStartL();
        }
    if ( aCustomMessageId == KStartViewerWithEmbeddedMsgPtr )
        {
        PushMessageL( activationData.iEmbeddedMessage, ETrue );
        }
    CleanupStack::Pop( activationData.iEmbeddedMessage ); // ownership transferred

    // Clear MSK to prevent unintentional presses before viewer is fully constructed
    ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );

    // If returning from view launched by MRUI, just complete the ongoing async process.
    // This will cause MRUI to be reopened and no other activation actions are needed.
    if ( iMrObserverToInform && aCustomMessageId == KStartViewerReturnToPreviousMsg )
        {
        CompletePendingMrCommand();
        }
    // Otherwise go on with the standard view activation
    else
        {
        // Cancel any pending asynchronous MR operation. There may be one at least if we
        // have navigated from MRUI to Donwload manager and from there to another message
        // or if external view activation happens while MRUI is open.
        CancelPendingMrCommandL();

        // Get message object unless returning to previously shown message.
        // In KStartViewerReturnToPreviousMsg the UI assumes that iMessage is alive.
        if ( aCustomMessageId == KStartViewerWithMsgId )
            {
            // Inform base view if view has been forward navigated to get correct back
            // navigation functionality in case of external activation.
            ViewEntered( aPrevViewId );
            UpdateMessagePtrL( activationData.iMailBoxId, activationData.iFolderId, activationData.iMessageId );
            }
        else if ( aCustomMessageId == KStartViewerWithEmbeddedMsgPtr ||
                  aCustomMessageId == KStartViewerReturnFromEmbeddedMsg )
            {
            if ( !iOpenMessages || iOpenMessages->IsEmpty() )
                {
                User::Leave( KErrNotFound );
                }
            UpdateMessagePtr( iOpenMessages->Head() );
            }

        // Regardless of the activation mode, we should now have a valid message pointer.
        // Otherwise we cannot go on.
        User::LeaveIfNull( iMessage );

        iEmbeddedMessageMode = (iEmbeddedMessages->Head() != NULL);

        // Update mailbox unless returning to previously open message
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

        // Check if MSG should be shown in the Meeting Request viewer
        TBool msgSentToMrUi( EFalse );
        if ( iMessage->IsFlagSet( EFSMsgFlag_CalendarMsg ) &&
             iAppUi.MrViewerInstanceL() &&
             !iEmbeddedMessageMode )
            {
            // Resolve meeting requst method
            TESMRMeetingRequestMethod mrMethod( EESMRMeetingRequestMethodUnknown );
            mrMethod = iAppUi.MrViewerInstanceL()->ResolveMeetingRequestMethodL( *iMessage );
            if ( mrMethod == EESMRMeetingRequestMethodRequest ||      // Request is shown in mrui
                 mrMethod == EESMRMeetingRequestMethodCancellation || // Cancellation is shown in mrui
                 mrMethod == EESMRMeetingRequestMethodUnknown )       // Unknown MR needs to be to resolved in MRUI (Usually IMAP)
                {
                // Setting MSK empty. This blocks unwanted MSK keypresses before MrViewer is initialized.
                ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );

                msgSentToMrUi = ETrue;
                ClearMailViewer(); // clear prev message to avoid it becoming visible before MRUI opens
                iMrUiActive = ETrue;
                // Same navi pane shown in MR viewer
                UpdateNaviPaneL( ETrue );
                SetMailboxNameToStatusPaneL();
                iAppUi.MrViewerInstanceL()->ExecuteViewL( *iMessage, *this );
                }
            }

        // MSG is not sent to MRUI, display in internal mail viewer
        // Message is either normal email message or response to meeting request
        if ( !msgSentToMrUi )
            {
            iMrUiActive = EFalse;

            // Update the viewer if new message has been opened or layout has changed while we were in some other view
            TBool changeMsg = (aCustomMessageId != KStartViewerReturnToPreviousMsg);
            if ( changeMsg || iLayoutChangedWhileNotActive )
                {
                RefreshL( changeMsg );
                }
            else // update MSK anyway as the attachment status may have changed
                {
                iShowMskDownloadOption = ShowMskDownloadOptionL();
                SetMskL();
                }

            SetMailboxNameToStatusPaneL();

            // Update navipane content
            UpdateNaviPaneL();
            }

        // Set email indicator off.. user has checked the new emails
        TFsEmailUiUtility::ToggleEmailIconL( EFalse );
        }

    iNextOrPevMessageSelected = EFalse;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::OfferToolbarEventL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::OfferToolbarEventL( TInt aCommand )
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
// CFSEmailUiMailViewerVisualiser::ToolbarResourceId
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerVisualiser::ToolbarResourceId() const
    {
    return R_FREESTYLE_EMAIL_UI_TOOLBAR_MESSAGE_VIEWER;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ChildDoDeactivate
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::ChildDoDeactivate()
    {
    FUNC_LOG;
    CancelFetchings();
    if ( iFetchingAnimationTimer )
        {
        iFetchingAnimationTimer->Stop();
        }
    if ( iDownloadProgressIndicator )
        {
        iDownloadProgressIndicator->HideIndicator();
        }

    // Hide this view's navipane
    HideNaviPane();

    // set view itself as toolbar observer again (e.g. MRGUI could have been changed it)
    Toolbar()->SetToolbarObserver( this );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::HideNaviPane
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::HideNaviPane()
    {
    FUNC_LOG;
    CAknNavigationControlContainer* naviPaneContainer = NULL;
    TRAP_IGNORE( naviPaneContainer = static_cast<CAknNavigationControlContainer*>( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) ) );
    TRAP_IGNORE( naviPaneContainer->PushDefaultL() );

    CCustomStatuspaneIndicators* indicators = iAppUi.GetStatusPaneIndicatorContainer();
    if ( indicators)
        {
        indicators->HideStatusPaneIndicators();
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ActivateControlGroup
// Overriden to make any running download progress bar visible after
// control group has been swithced
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::ActivateControlGroup( TInt aDelay /*= 0*/ )
    {
    CFsEmailUiViewBase::ActivateControlGroup( aDelay );
    TRAP_IGNORE( UpdateDownloadIndicatorL() );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::HandleMrCommandL
// Handle accept/decline/tentative/remove commands given for meeting request
// message directly from list UI.
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::HandleMrCommandL(
    TInt aCommandId,
    TFSMailMsgId aMailboxId,
    TFSMailMsgId aFolderId,
    TFSMailMsgId aMessageId )
    {
    FUNC_LOG;

    // All items should be run
    if ( !iFirstStartCompleted )
        {
        DoFirstStartL();
        }

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

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::HandleTextViewerEventL
// From viewer observer for CFsTextViewer callbacks
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::HandleTextViewerEventL( TFsTextViewerEvent aEvent )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        if ( aEvent == EFsTextViewerHotspotClicked )
            {
            // Get currently focused hotspot data
            SViewerHeadingHotSpotData currentHeaderHotSpotData;
            CFindItemEngine::SFoundItem currentBodyHotSpotData;
            THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL(
                currentHeaderHotSpotData, currentBodyHotSpotData );

            // Some header hotspots have some immediate action defined, so
            // in case of header hotspot, try to handle the action first
            TBool actionTaken( EFalse );
            if ( hotspotType == EHeaderHotspot )
                {
                actionTaken = HandleHeaderHotspotActionL( currentHeaderHotSpotData.iType );
                }

            // If the current item is valid hotspot but it didn't have any
            // immediate action defined, launch item specific action menu
            if( ( hotspotType == EHeaderHotspot || hotspotType == EBodyHotspot ) &&
                !actionTaken )
                {
                LaunchActionMenuL();
                }
            }
        else if ( aEvent == EFsTextViewerHotspotLongTap )
            {
            LaunchActionMenuL();
            }
        else if ( aEvent == EFsTextViewerScrollDown )
            {
            // check if user has scroll to the bottom for start fetching rest of the message.
            // if whole message has already been fetched, no actions are needed
            StartFetchingRemaininBodyLinesIfAtBottomL();

            // Hide action menu icon if current hotspot does not have action menu.
            // Seems that this needs to be checked also here because "hotspot changed" events
            // are not received reliably when scrolling down.
            SetActionMenuIconVisbilityL();
            }

        else if ( aEvent == EFsTextViewerHotspotChanged )
            {
            // Hide action menu icon if current hotspot does not have action menu.
            SetActionMenuIconVisbilityL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::HandleHeaderHotspotActionL
// Handle appropriate command for the selected header hotspot, if the currently
// focused item has some action specified. Returns ETrue, if current item has
// some action defined (and the action is initiated), EFalse if not.
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::HandleHeaderHotspotActionL(
        TViewerHeadingHotspotType aHotspotType )
    {
    TBool actionTaken( EFalse );

    switch( aHotspotType )
        {
        case ETypeToNMoreRecipients:
        case ETypeCcNMoreRecipients:
        case ETypeBccNMoreRecipients:
            {
            HandleCommandL( EFsEmailUiCmdViewAll );
            actionTaken = ETrue;
            }
            break;
        case ETypeAttachment:
            {
            HandleCommandL( EFsEmailUiCmdOpenAttachment );
            actionTaken = ETrue;
            }
            break;
        case ETypeAttachments:
            {
            HandleCommandL( EFsEmailUiCmdOpenAttachmentList );
            actionTaken = ETrue;
            }
            break;
        case ETypeHtml:
            {
            HandleCommandL( EFsEmailUiCmdActionsOpenHtmlViewer );
            actionTaken = ETrue;
            }
            break;
        }

    return actionTaken;
    }

// Helper funcitons to get viewed message ID and Folder id
TFSMailMsgId CFSEmailUiMailViewerVisualiser::ViewedMessageFolderId()
    {
    FUNC_LOG;
    TFSMailMsgId ret;
    if ( iMessage )
        {
        ret = iMessage->GetFolderId();
        }
    return ret;
    }

TFSMailMsgId CFSEmailUiMailViewerVisualiser::ViewedMessageId()
    {
    FUNC_LOG;
    TFSMailMsgId ret;
    if ( iMessage )
        {
        ret = iMessage->GetMessageId();
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::RefreshL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::RefreshL( TBool aFirstRefresh /*= EFalse*/ )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        // Show "Opening" wait note if the viewed message is a large one
        //<cmail>
        //TInt waitNoteId = KErrNotFound;
        //</cmail>
        if ( TFsEmailUiUtility::IsMessageBodyLargeL( iMessage ) )
            {
            //<cmail> using normal wait note instead of using global
            //waitNoteId = TFsEmailUiUtility::ShowGlobalWaitNoteLC( R_FSE_WAIT_OPENING_TEXT );
            iWaitDialogOpening = new(ELeave)CAknWaitDialog(
                                (REINTERPRET_CAST(CEikDialog**,&iWaitDialogOpening)));
            HBufC* noteText = StringLoader::LoadLC( R_FSE_WAIT_OPENING_TEXT );
            iWaitDialogOpening->SetTextL(*noteText);
            //iWaitDialogOpening->DrawableWindow()->SetOrdinalPosition(0);
            CleanupStack::PopAndDestroy(noteText);
            iWaitDialogOpening->ExecuteLD(R_FSE_WAIT_DIALOG_NO_CANCEL);
            //</cmail>
            }

        // Do the update
        UpdateMailViewerL();
        AddBackgroundPicturesL();

        // scroll to Sent line after updating if requested
        if ( aFirstRefresh )
            {
            iTextViewer->FocusLineL( iViewerRichText->SentTextLine(), ETrue );

            iAsyncCallback->Cancel(); // just in case
            iAsyncCallback->Set( TCallBack( DoPostRefresh, this ) );
            iAsyncCallback->CallBack();
            }

        // Close the wait note if it was opened
        if ( iWaitDialogOpening )//<cmail>
            {
            //<cmail>
            //CleanupStack::PopAndDestroy( (TAny*)waitNoteId );
            iWaitDialogOpening->ProcessFinishedL(); //it destroys the waitnote also
            //</cmail>
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::DoPostActivation
// Callback function for deferred activation actions
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerVisualiser::DoPostRefresh( TAny* aSelfPtr )
    {
    FUNC_LOG;
    CFSEmailUiMailViewerVisualiser* self =
        static_cast<CFSEmailUiMailViewerVisualiser*>( aSelfPtr );
    TRAPD( err, self->PostRefreshL() );
    return err;
    }

void CFSEmailUiMailViewerVisualiser::PostRefreshL()
    {
    FUNC_LOG;
    // Focus attahcment line if available
    TInt attachmentTextLine = iViewerRichText->AttachmentTextLine();
    TInt viewHtmlTextLine = iViewerRichText->ViewHtmlTextLine();
    if ( attachmentTextLine > 0 )
        {
        iTextViewer->FocusLineL( attachmentTextLine, EFalse );
        iViewerRichText->SetHotspotByIndexL( iViewerRichText->AttachmentHotSpotIndex() );
        }
    else if ( viewHtmlTextLine > 0 )
        {
        iTextViewer->FocusLineL( viewHtmlTextLine, EFalse );
        iViewerRichText->SetHotspotByIndexL( iViewerRichText->ViewHtmlHotSpotIndex() );
        }
    SetActionMenuIconVisbilityL();

    // Set initial value for downloaded status for one attachment case
    iShowMskDownloadOption = ShowMskDownloadOptionL();
    SetMskL();

    // check if we know whole message, starts fetching if structure is unknown
    // or body doesn't found
    CheckMessageStructureL();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CheckMessageStructureL
// Starts fetching message structure if it is unknown.
// If the structure is known but message body is not fetched yet, start fetching it.
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::CheckMessageStructureL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && iMessage )
        {
        if ( !MessageStructureKnown( *iMessage ) )
            { // fetch structure
            StartWaitedFetchingL( EMessageStructure );
            }
        else if ( StartFetchingBodyAfterOpeningL() )
            {
            iViewerRichText->AppendFetchingMoreTextL();
            StartFetchingMessagePartL( *iMessage, EMessagePlainTextBodyPart );
            iFetchingAnimationTimer->Start( KAnimationRefreshTimeIntervalInMilliseconds );
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::UpdateDownloadIndicatorL
// Show or hide download progress indicator popup and update its contents if
// necessary.
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::UpdateDownloadIndicatorL( TFSProgress::TFSProgressStatus aStatus
                                                               /*= TFSProgress::EFSStatus_Waiting*/ )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && iMessage && iTextViewer ) // Safety
        {
        TFSMailMsgId messId = iMessage->GetMessageId();
        HBufC* progressString = NULL;
        if ( iAppUi.DownloadInfoMediator() && iAppUi.DownloadInfoMediator()->IsAnyAttachmentDownloads( messId ) )
            {
            TInt percentage = iAppUi.DownloadInfoMediator()->GetDownloadPercentageL( messId );
            progressString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_LIST_DOWNLOADING,
                                                          percentage );
            }
        else if ( aStatus == TFSProgress::EFSStatus_RequestComplete )
            {
            progressString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_LIST_DOWNLOADED_100 );
            }
        else if ( aStatus == TFSProgress::EFSStatus_RequestCancelled )
            {
            progressString = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_LIST_DOWNLOADING_CANCELLED );
            }
        else
            {
            progressString = KNullDesC().AllocLC();
            }

        if ( progressString->Length() )
            {
            if ( iDownloadProgressIndicator->IsVisible() )
                {
                // pop-up already visible, just reset the text
                iDownloadProgressIndicator->SetContentsL( progressString );
                }
            else
                {
                // pop-up should be launched

                // Create fetching status layout first to ensure there's empty space in the end of the message
                iViewerRichText->CreateStatusLayoutL();

                iDownloadProgressIndicator->ShowIndicatorL( CFSEmailUiStatusIndicator::EIndicationDownloadProgress,
                                                            progressString,
                                                            CFSEmailUiStatusIndicator::EIndicationNoAutomaticHiding );
                iShowMskDownloadOption = EFalse;
                }
            }
        CleanupStack::PopAndDestroy( progressString );

        // Hide the progress popup after small delay if the download was completed or cancelled
        if ( iAppUi.DownloadInfoMediator() && !iAppUi.DownloadInfoMediator()->IsAnyAttachmentDownloads( messId ) &&
             iDownloadProgressIndicator->IsVisible() )
            {
            iShowMskDownloadOption = ShowMskDownloadOptionL();
            iDownloadProgressIndicator->HideIndicator( KDownloadProgreeNoteHideDelay );
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::DynInitMenuPaneL
// For dynamically dimm/undimm menu options according to currect message and focus in the viewer
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    if ( !iMessage || !iTextViewer )
        {
        User::Leave( KErrNotReady );
        }

    if ( aResourceId == R_FSEMAILUI_MAILVIEWER_MENUPANE )
        {
        
        if ( FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ) )
           {
           // remove help support in pf5250
           aMenuPane->SetItemDimmed( EFsEmailUiCmdHelp, ETrue);      
           }
        
        // Folder independent options

        // hide/show MessageReader
        TBool hideMessageReader = iEmbeddedMessageMode || !iAppUi.MessageReaderSupportsFreestyle();
        aMenuPane->SetItemDimmed( EFsEmailUiCmdReadEmail, hideMessageReader );

        // hide/show actions sub menu
        TBool hideActions = !ShowActionsMenuInOptionsL();
        aMenuPane->SetItemDimmed( EFsEmailUiCmdMailActions, hideActions );

        // hide/show next and previous message options
        TBool hideNext = !ShowNextMessageMenuInOptions();
        aMenuPane->SetItemDimmed( EFsEmailUiCmdNextMessage, hideNext );
        TBool hidePrev = !ShowPreviousMessageMenuInOptions();
        aMenuPane->SetItemDimmed( EFsEmailUiCmdPreviousMessage, hidePrev );

        // hide/show copy to clipboard
        TBool hideCopyToClipBoard = !IsCopyToClipBoardAvailableL();
        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCopyToClipboard, hideCopyToClipBoard );


        // Folder dependent options
        TFSMailMsgId currentMessageFolderId = iMessage->GetFolderId();
        CFSMailFolder* currentFolder = NULL;
        if ( !iEmbeddedMessageMode )
            {
            TRAP_IGNORE( currentFolder = iAppUi.GetMailClient()->GetFolderByUidL(
                    iMessage->GetMailBoxId(), currentMessageFolderId ) );
            }
        CleanupStack::PushL( currentFolder );
        if ( !currentFolder || currentFolder->GetFolderType() == EFSOutbox )
            { // outbox folder or embedded message
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsForward, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReply, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReplyAll, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsForward2, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsMoveToDrafts, !currentFolder );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsDelete, !currentFolder );
            }
        else
            { // other folders
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsMoveToDrafts, ETrue );

            // hide reply all if there's no multiple recipients
            TInt numRecipients(0);
            if ( iMessage )
                {
                //Get # of recipients
                numRecipients =TFsEmailUiUtility::CountRecepients( iMessage );
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
                    if( iMessage->GetBCCRecipients().Count() )
                        {
                        if( iMailBox->OwnMailAddress().GetEmailAddress().Compare(iMessage->GetBCCRecipients()[0]->GetEmailAddress()) )
                            {
                            numRecipients++;
                            }
                        }
                    }
                }

            TBool hideReplyAll = ( numRecipients <= 1 );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsReplyAll, hideReplyAll );

            // In Sent folder, option forward is shown above options reply and reply all
            TBool forwardAboveReply = ( currentFolder->GetFolderType() == EFSSentFolder );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsForward, !forwardAboveReply );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsForward2, forwardAboveReply );
            }
        CleanupStack::PopAndDestroy( currentFolder );
        }

    if ( aResourceId == R_FSEMAILUI_MAILVIEWER_SUBMENU_MORE )
        {
        TFSMailMsgId currentMessageFolderId = iMessage->GetFolderId();
        CFSMailFolder* currentFolder = NULL;
        if ( !iEmbeddedMessageMode )
            {
            TRAP_IGNORE( currentFolder = iAppUi.GetMailClient()->GetFolderByUidL(
                    iMessage->GetMailBoxId(), currentMessageFolderId ) );
            }
        CleanupStack::PushL( currentFolder );
        if ( !currentFolder || currentFolder->GetFolderType() == EFSOutbox )
            {  // outbox folder or embedded message
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsUnread, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsRead, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsMoveMessage, ETrue );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsFlag, ETrue );
// <cmail> Prevent Download Manager opening with attachments
//          aMenuPane->SetItemDimmed( EFsEmailUiCmdDownloadManager, ETrue );
// </cmail> Prevent Download Manager opening with attachments
            }
        else // other folders
            {
            // show read/unread according to current state
            TBool messageIsRead = iMessage->IsFlagSet( EFSMsgFlag_Read );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsRead, messageIsRead );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdMarkAsUnread, !messageIsRead );

            // show/hide move message
            TBool hideMove = !iAppUi.GetActiveMailbox()->HasCapability( EFSMBoxCapaMoveToFolder );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsMoveMessage, hideMove );

            // show/hide flag for followup
            TBool hideFlag = !TFsEmailUiUtility::IsFollowUpSupported( *iMailBox );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsFlag, hideFlag );

            // hide/show download manager option
// <cmail> Prevent Download Manager opening with attachments
//          TBool hideDownloadMan = !ShowDownloadManagerMenuInOptions();
//          aMenuPane->SetItemDimmed( EFsEmailUiCmdDownloadManager, hideDownloadMan );
// </cmail>
            }
        CleanupStack::PopAndDestroy( currentFolder );
        }

    if ( aResourceId == R_FSEMAILUI_MAILVIEWER_SUBMENU_ACTIONS )
        {
        // All menu items are hidden by default.
        // Undimm items that should be visible in action menu.
        SViewerHeadingHotSpotData currentHeaderHotSpotData;
        CFindItemEngine::SFoundItem currentBodyHotSpotData;
        THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL(
            currentHeaderHotSpotData, currentBodyHotSpotData );

        TBool remoteLookupAvailable =
            TFsEmailUiUtility::IsRemoteLookupSupported( *iMailBox );

        if ( ( hotspotType == EHeaderHotspot && (
            currentHeaderHotSpotData.iType == ETypeFromAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeToAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeCcAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeBccAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeEmailAddress )  )
            ||
            ( hotspotType == EBodyHotspot &&
              currentBodyHotSpotData.iItemType ==
                CFindItemEngine::EFindItemSearchMailAddressBin ) )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCall, EFalse );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCreateMessage, EFalse );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCreateMail, EFalse );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsContactDetails, EFalse );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsAddContact2, EFalse );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsRemoteLookup, !remoteLookupAvailable );
            }
        else if ( hotspotType == EHeaderHotspot &&
                  currentHeaderHotSpotData.iType == ETypeAttachment )
            {
            TBool hideOpen = !ShowOpenAttachmentOptionL();
            aMenuPane->SetItemDimmed( EFsEmailUiCmdOpenAttachment, hideOpen );
            TBool hideDownload = !ShowDownloadOptionL();
            aMenuPane->SetItemDimmed( EFsEmailUiCmdDownload, hideDownload );
            TBool hideCancel = !ShowCancelDownloadOption();
            aMenuPane->SetItemDimmed( EFsEmailUiCmdCancelDownload, hideCancel );
            TBool hideSave = !ShowSaveAttachmentOptionL();
            aMenuPane->SetItemDimmed( EFsEmailUiCmdSave, hideSave );

            TBool hideRemove = hideOpen;
            aMenuPane->SetItemDimmed( EFsEmailUiCmdClearFetchedAttachment, hideRemove );
            }
        else if ( hotspotType == EHeaderHotspot &&
                  currentHeaderHotSpotData.iType == ETypeAttachments )
            {
            TBool hideOpen = !ShowOpenAttachmentOptionL();
            aMenuPane->SetItemDimmed( EFsEmailUiCmdOpenAttachmentList, hideOpen );
            TBool hideDownload = !ShowDownloadOptionL();
            aMenuPane->SetItemDimmed( EFsEmailUiCmdDownloadAll, hideDownload );
            TBool hideCancel = !ShowCancelDownloadOption();
            aMenuPane->SetItemDimmed( EFsEmailUiCmdCancelAllDownloads, hideCancel );
            TBool hideSave = !ShowSaveAttachmentOptionL();
            aMenuPane->SetItemDimmed( EFsEmailUiCmdSaveAll, hideSave );
            }
        else if ( hotspotType == EBodyHotspot &&
                  currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchPhoneNumberBin )
            {
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCall, EFalse );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsAddContact, EFalse );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCreateMessage, EFalse );
            aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsRemoteLookup, !remoteLookupAvailable );
            }
        else if ( hotspotType == EBodyHotspot &&
                  ( currentBodyHotSpotData.iItemType ==
                    CFindItemEngine::EFindItemSearchURLBin ||
                    currentBodyHotSpotData.iItemType ==
                    CFindItemEngine::EFindItemSearchScheme ) )
            {
            // Handle action menu for different search scheme, e.g mailto: and call:
            TInt schemaLinkType = ResolveBodyTextSchemaUrlTypeL( currentBodyHotSpotData );
            switch ( schemaLinkType )
                {
                case EFocusOnEMailInBodyText:
                    {
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCall, EFalse );
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCreateMessage, EFalse );
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsContactDetails, EFalse );
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsAddContact2, EFalse );
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsRemoteLookup, !remoteLookupAvailable );
                    }
                    break;
                case EFocusOnNumberWithinMessage:
                    {
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCall, EFalse );
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsAddContact, EFalse );
                    // Drop out "Create message" selection for SIP addresses
                    HBufC* schemeText = iViewerRichText->GetHotspotTextLC( currentBodyHotSpotData );
                    if ( schemeText && schemeText->FindC( KVoipPrefix ) != 0 )
                        {
                        aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsCreateMessage, EFalse );
                        }
                    CleanupStack::PopAndDestroy( schemeText );
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsRemoteLookup, !remoteLookupAvailable );
                    }
                    break;
                default:
                    {
                    aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsOpenWeb, EFalse );

                    // Handle intranet browsing item by using AIW service
                    // handler's InitializeMenuPaneL-function, it will remove
                    // the IB menu item if IB is not available, or make the
                    // item visible if IB is available
                    if ( iIBServiceHandler && iIBServiceHandler->IsAiwMenu( aResourceId ) )
                        {
                        TAiwGenericParam param( EGenericParamURL );
                        CAiwGenericParamList* list = CAiwGenericParamList::NewLC();
                        list->AppendL( param );
                        iIBServiceHandler->InitializeMenuPaneL( *aMenuPane, aResourceId,
                                EFsEmailUiCmdActionsOpenInIntranetMenu, *list, ETrue );
                        CleanupStack::PopAndDestroy( list );
                        }

                    aMenuPane->SetItemDimmed( EFsEmailUiCmdActionsAddBookmark, EFalse );
                    }
                    break;
                }
            }
        }

    iAppUi.ShortcutBinding().AppendShortcutHintsL( *aMenuPane,
        CFSEmailUiShortcutBinding::EContextMailViewer );
    }


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::HandleCommandL
// Handles menu selections
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;
    if ( !iAppUi.ViewSwitchingOngoing() )
        {
        switch ( aCommand )
            {
            case EAknSoftkeyBack:
                {
                // Set empty MSK when navigating back
                ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
                // Cancel MR viewer launching if Back is pressed in the middle
                // of launching process.
                CancelPendingMrCommandL();
                NavigateBackL();
                }
                break;
            // top level options
            case EFsEmailUiCmdActionsMoveToDrafts:
                {
                TFsEmailUiUtility::MoveMessageToDraftsL(
                    *iAppUi.GetActiveMailbox(), *iMessage );
                }
                break;
            case EFsEmailUiCmdCalActionsReplyAsMail:
            case EFsEmailUiCmdActionsReply:
                {
                if ( !iEmbeddedMessageMode )
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
                if ( !iEmbeddedMessageMode )
                    {
                    TEditorLaunchParams params;
                    params.iMailboxId = iAppUi.GetActiveMailboxId();
                    params.iMsgId = iMessage->GetMessageId();
                    params.iActivatedExternally = EFalse;
                    iAppUi.LaunchEditorL( KEditorCmdReplyAll, params );
                    }
                }
                break;
            case EFsEmailUiCmdCalActionsForwardAsMail:
            case EFsEmailUiCmdActionsForward:
            case EFsEmailUiCmdActionsForward2:
                {
                if ( !iEmbeddedMessageMode )
                    {
                    TEditorLaunchParams params;
                    params.iMailboxId = iAppUi.GetActiveMailboxId();
                    params.iMsgId = iMessage->GetMessageId();
                    params.iActivatedExternally = EFalse;
                    iAppUi.LaunchEditorL( KEditorCmdForward, params );
                    }
                }
                break;
            case EFsEmailUiCmdActionsDelete:
                {
                DeleteMailL( *iMessage, ETrue );
                }
                break;
            case EFsEmailUiCmdActionsCopyToClipboard:
                {
                CopyCurrentHotspotToClipBoardL();
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
            case EFsEmailUiCmdHelp:
                {
                TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
                }
                break;
            case EFsEmailUiCmdExit:
                {
                iAppUi.Exit();
                }
                break;
            case EFsEmailUiCmdReadEmail:
                {
                iAppUi.StartReadingEmailsL();
                }
                break;

            // "actions" -sub menu options
            case EFsEmailUiCmdActionsCall:
                {
                CallHotSpotAddressL();
                }
                break;
            case EFsEmailUiCmdActionsCreateMessage:
                {
                CreateMessageL();
                }
                break;
            case EFsEmailUiCmdActionsCreateMail:
                {
                ComposeMailL();
                }
                break;
            case EFsEmailUiCmdActionsContactDetails:
                {
                OpenContactDetailsL();
                }
                break;
            case EFsEmailUiCmdActionsAddContact:
            case EFsEmailUiCmdActionsAddContact2:
                {
                SViewerHeadingHotSpotData currentHeaderHotSpotData;
                CFindItemEngine::SFoundItem currentBodyHotSpotData;
                THotspotType hotspotType =
                    iViewerRichText->FindCurrentHotSpotL(
                        currentHeaderHotSpotData, currentBodyHotSpotData );
                if ( hotspotType == EHeaderHotspot )
                    {
                    HBufC* emailAddress =
                        iViewerRichText->GetHeaderHotspotEmailAddressLC(
                            currentHeaderHotSpotData );
                    SaveEmailAsContactL( *emailAddress );
                    CleanupStack::PopAndDestroy( emailAddress );
                    }
                else if ( hotspotType == EBodyHotspot )
                    {
                    HBufC* hotspotText = iViewerRichText->GetHotspotTextLC(
                        currentBodyHotSpotData );
                    if ( currentBodyHotSpotData.iItemType ==
                        CFindItemEngine::EFindItemSearchPhoneNumberBin )
                        {
                        SavePhoneNumberAsContactL( *hotspotText );
                        }
                    else if ( currentBodyHotSpotData.iItemType ==
                        CFindItemEngine::EFindItemSearchMailAddressBin )
                        {
                        SaveEmailAsContactL( *hotspotText );
                        }
                    else if ( currentBodyHotSpotData.iItemType ==
                            CFindItemEngine::EFindItemSearchScheme)
                        {
                        if ( hotspotText->FindC( KMailtoPrefix ) == 0 )
                            {
                            // Save email address, prefix is stripped in SaveEmailAsContactL
                            SaveEmailAsContactL( *hotspotText );
                            }
                        else if ( hotspotText->FindC( KCallPrefix ) == 0 ||
                                  hotspotText->FindC( KTelPrefix ) == 0  ||
                                  hotspotText->FindC( KSmsPrefix ) == 0 ||
                                  hotspotText->FindC( KMmsPrefix ) == 0 )
                            {
                            // Save phone nukmber, prefix is stripped in SavePhoneNumberAsContactL
                            SavePhoneNumberAsContactL( *hotspotText );
                            }
                        }
                    CleanupStack::PopAndDestroy( hotspotText );
                    }
                }
                break;
            case EFsEmailUiCmdActionsRemoteLookup:
                {
                SViewerHeadingHotSpotData currentHeaderHotSpotData;
                CFindItemEngine::SFoundItem currentBodyHotSpotData;
                THotspotType hotspotType =
                    iViewerRichText->FindCurrentHotSpotL(
                        currentHeaderHotSpotData, currentBodyHotSpotData );
                if ( hotspotType == EHeaderHotspot )
                    {
                    HBufC* emailAddress =
                        iViewerRichText->GetHeaderHotspotEmailAddressLC(
                            currentHeaderHotSpotData );
                    LaunchRemoteLookupL( *emailAddress );
                    CleanupStack::PopAndDestroy( emailAddress );
                    }
                else if ( hotspotType == EBodyHotspot )
                    {
                    HBufC* hotspotText = iViewerRichText->GetHotspotTextLC(
                        currentBodyHotSpotData );
                    LaunchRemoteLookupL( *hotspotText );
                    CleanupStack::PopAndDestroy( hotspotText );
                    }
                }
                break;
            case EFsEmailUiCmdActionsOpenHtmlViewer:
                {
                LaunchHtmlViewerL();
                }
                break;
            case EFsEmailUiCmdActionsOpenWeb:
                {
                CFindItemEngine::SFoundItem currentBodyHotSpotData;
                TBool bodyHotSpotFound =
                    iViewerRichText->FindCurrentBodyHotSpotL(
                        currentBodyHotSpotData );
                OpenHotSpotUrlInBrowserL( currentBodyHotSpotData );
                }
                break;
            case EFsEmailUiCmdActionsOpenInIntranetMenu:
                {
                OpenLinkInIntranetL( ETrue );
                }
                break;
            case EFsEmailUiCmdActionsAddBookmark:
                {
                CFindItemEngine::SFoundItem currentBodyHotSpotData;
                TBool bodyHotSpotFound =
                    iViewerRichText->FindCurrentBodyHotSpotL(
                        currentBodyHotSpotData );
                AddToBookmarksL( currentBodyHotSpotData );
                }
                break;
            case EAknSoftkeyOpen:
            case EFsEmailUiCmdOpenAttachment:
                {
                OpenAttachmentL();
                }
                break;
            case EFsEmailUiCmdOpenAttachmentList:
                {
                OpenAttachmentsViewL();
                }
                break;
            case EFsEmailUiCmdDownload:
            case EFsEmailUiCmdDownloadAll:
                {
                StartDowloadingAttachmentsL();
                }
                break;
            case EFsEmailUiCmdCancelDownload:
            case EFsEmailUiCmdCancelAllDownloads:
                {
                CancelDowloadingAttachmentsL();
                }
                break;
            case EFsEmailUiCmdSave:
            case EFsEmailUiCmdSaveAll:
                {
                SaveAllAttachmentsL();
                }
                break;
            case EFsEmailUiCmdClearFetchedAttachment:
                {
                RemoveFetchedAttachmentL();
                }
                break;

            // "more" -sub menu options
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
            case EFsEmailUiCmdCompose:
                {
                iAppUi.CreateNewMailL();
                }
                break;
            case EFsEmailUiCmdComposeTo:
                {
                ComposeMailL();
                }
                break;
            case EFsEmailUiCmdMessageDetails:
            case EFsEmailUiCmdViewAll:
                {
                TUid activationCmdId = KStartMsgDetailsToBeginning;
                if ( aCommand == EFsEmailUiCmdViewAll )
                    {
                    SViewerHeadingHotSpotData currentHeaderHotSpotData;
                    iViewerRichText->FindCurrentHeaderHotSpotL( currentHeaderHotSpotData );
                    if( currentHeaderHotSpotData.iType == ETypeToNMoreRecipients )
                        {
                        activationCmdId = KStartMsgDetailsToTo;
                        }
                    else if( currentHeaderHotSpotData.iType == ETypeCcNMoreRecipients )
                        {
                        activationCmdId = KStartMsgDetailsToCc;
                        }
                    else if( currentHeaderHotSpotData.iType == ETypeBccNMoreRecipients )
                        {
                        activationCmdId = KStartMsgDetailsToBcc;
                        }
                    }

                TMsgDetailsActivationData msgDetailsData;
                msgDetailsData.iMailBoxId = iMessage->GetMailBoxId();
                msgDetailsData.iFolderId = iMessage->GetFolderId();
                msgDetailsData.iMessageId = iMessage->GetMessageId();
                const TPckgBuf<TMsgDetailsActivationData> pkgOut( msgDetailsData );
                iAppUi.EnterFsEmailViewL(
                    MsgDetailsViewId, activationCmdId,  pkgOut );
                }
                break;
// <cmail> Prevent Download Manager opening with attachments
//          case EFsEmailUiCmdDownloadManager:
//              {
//              iAppUi.EnterFsEmailViewL( DownloadManagerViewId );
//              }
//              break;
// </cmail>
            case EFsEmailUiCmdMarkAsReadUnreadToggle:
                {
                if ( iMessage && !iEmbeddedMessageMode )
                    {
                    if ( iMessage->IsFlagSet( EFSMsgFlag_Read ) )
                        {
                        HandleCommandL( EFsEmailUiCmdMarkAsUnread );
                        }
                    else
                        {
                        HandleCommandL( EFsEmailUiCmdMarkAsRead );
                        }
                    }
                }
                break;

            // Moving to top or to bottom of the text. Zero seems to be
            // treated as illegal value in FocusLineL (it is skipped),
            // so assuming that line range is [1, total lines count].
            // It is not documented anywhere though.
            case EFsEmailUiCmdGoToTop:
                {
                // <cmail>
                // Do nothing if focus is already in top
                if ( iTextViewer->GetFirstDisplayedLine() > 1 )
                    {
                    iTextViewer->FocusLineL( 1 );
                    }
                }
                // </cmail>
                break;
            case EFsEmailUiCmdGoToBottom:
                {
                iTextViewer->FocusLineL( iTextViewer->GetTotalLines() );
                }
                break;

            // The Generic text viewer API supports setting only one key for scroll up
            // and one key for scroll down. To have multiple scroll up/down keys,
            // we have set artificial keys for the generic viewer and simulate those
            // when one of the shortcut keys is pressed.
            // Another reason to use these artificial keys is that the TextViewer identifies
            // keys by iCodes. This doesn't fit our concept at least in product where iCode
            // of certain keys contains the value of the character found behind Fn on that key,
            // and the Fn mappings vary between variants.
            // Similar mechanism is used inside AknFep with key codes EKeyF19...EKeyF24,
            // so collisions with these must be avoided.
            case EFsEmailUiCmdScrollUp:
                {
                TKeyEvent simEvent = { KKeyCodeArtificialScrollUp, EStdKeyNull, 0, 0 };
                iCoeEnv->SimulateKeyEventL( simEvent, EEventKey );
                }
                break;
            case EFsEmailUiCmdScrollDown:
                {
                TKeyEvent simEvent = { KKeyCodeArtificialScrollDown, EStdKeyNull, 0, 0 };
                iCoeEnv->SimulateKeyEventL( simEvent, EEventKey );
                }
                break;
            case EFsEmailUiCmdPageUp:
                {
                TKeyEvent simEvent = { EKeyPageUp, EStdKeyNull, 0, 0 };
                iCoeEnv->SimulateKeyEventL( simEvent, EEventKey );
                }
                break;
            case EFsEmailUiCmdPageDown:
                {
                TKeyEvent simEvent = { EKeyPageDown, EStdKeyNull, 0, 0 };
                iCoeEnv->SimulateKeyEventL( simEvent, EEventKey );
                // Check if we reached the end of truncated message body
                StartFetchingRemaininBodyLinesIfAtBottomL();
                }
                break;

            default:
                break;
            }
        }
    }


void CFSEmailUiMailViewerVisualiser::ComposeMailL()
    {
    SViewerHeadingHotSpotData currentHeaderHotSpotData;
    CFindItemEngine::SFoundItem currentBodyHotSpotData;
    THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL( currentHeaderHotSpotData, currentBodyHotSpotData );
    if ( hotspotType == EHeaderHotspot )
       {
       // Regular email address
       if ( currentHeaderHotSpotData.iType == ETypeEmailAddress )
           {
           HBufC* emailAddress = iViewerRichText->GetHeaderHotspotEmailAddressLC( currentHeaderHotSpotData );
           if ( emailAddress && emailAddress->Length() )
               {
               delete iNewMailTempAddress;
               iNewMailTempAddress = NULL;
               iNewMailTempAddress = CFSMailAddress::NewL();
               iNewMailTempAddress->SetEmailAddress( *emailAddress );
               iAppUi.LaunchEditorL( iNewMailTempAddress );
               }
           CleanupStack::PopAndDestroy( emailAddress );
           }
       // Sender addr with display name
       else if ( currentHeaderHotSpotData.iType == ETypeFromAddressDisplayName )
           {
           CFSMailAddress* senderAddress = iMessage->GetSender(); // not owned
           iAppUi.LaunchEditorL( senderAddress );
           }
       // To addr with display name
       else if ( currentHeaderHotSpotData.iType == ETypeToAddressDisplayName )
           {
           RPointerArray<CFSMailAddress>& toArray = iMessage->GetToRecipients(); // not owned
           CFSMailAddress* toAddress = toArray[ currentHeaderHotSpotData.iDisplayNameArrayIndex ];
           iAppUi.LaunchEditorL( toAddress );
           }
       // CC addr with display name
       else if ( currentHeaderHotSpotData.iType == ETypeCcAddressDisplayName )
           {
           RPointerArray<CFSMailAddress>& ccArray = iMessage->GetCCRecipients(); // not owned
           CFSMailAddress* ccAddress = ccArray[ currentHeaderHotSpotData.iDisplayNameArrayIndex ];
           iAppUi.LaunchEditorL( ccAddress );
           }
       // BCC addr with display name
       else if ( currentHeaderHotSpotData.iType == ETypeBccAddressDisplayName )
           {
           RPointerArray<CFSMailAddress>& bccArray = iMessage->GetBCCRecipients(); // not owned
           CFSMailAddress* bccAddress = bccArray[ currentHeaderHotSpotData.iDisplayNameArrayIndex ];
           iAppUi.LaunchEditorL( bccAddress );
           }
       }
    // Email address hotspot in body text clicked
    else if ( hotspotType == EBodyHotspot && ( currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchMailAddressBin ||
             currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchScheme ) )
       {
       WriteEmailToHotSpotAddressL( currentBodyHotSpotData );
       }
    }




void CFSEmailUiMailViewerVisualiser::ProcessAsyncCommandL( TESMRIcalViewerOperationType aCommandId,
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
                    iOpResult.iResultCode = DeleteMailL( *iMessage, EFalse, ETrue );
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
                    OpenAttachmentL();
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
                    StartDowloadingAttachmentsL();
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
                if ( ShowDownloadManagerMenuInOptions() )
                    {
                    iOpResult.iResultCode = KErrNone;
                    iAppUi.EnterFsEmailViewL( DownloadManagerViewId );
                    }
                }
                break;
            case EESMRCmdMailComposeMessage:
                {
                iOpResult.iResultCode = KErrNone;
                iAppUi.CreateNewMailL();
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
                iNextOrPevMessageSelected = ETrue; // prevent back navigation when operation completed received
                ShowPreviousMessageL();
                iOpResult.iResultCode = KErrNone;
                CompletePendingMrCommand();
                }
                break;
            case EESMRCmdMailNextMessage:
                {
                iNextOrPevMessageSelected = ETrue; // prevent back navigation when operation completed received
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


void CFSEmailUiMailViewerVisualiser::ProcessSyncCommandL(
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
                    if ( !iFlagSelectionHanler )
                        {
                        // Create when used for the first time
                        iFlagSelectionHanler =
                            CFlagSelectionGlobalNoteHandler::NewL( *this );
                        }
                    // Call to LaunchFlagListQueryDialogL will lead
                    // to FlagselectionCompleteL
                    iFlagSelectionHanler->LaunchFlagListQueryDialogL();
                    }
                }
                break;
            default:
                break;
            }
        }
    }


// ---------------------------------------------------------------------------
// CanProcessCommand
// Callback from meeting request UI. Determines what functionality is provided
// to mrui by this UI
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMailViewerVisualiser::CanProcessCommand(
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
            ret = ShowDownloadManagerMenuInOptions();
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

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CompletePendingMrCommand()
// -----------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::CompletePendingMrCommand()
    {
    FUNC_LOG;
    if ( iMrObserverToInform )
        {
        iMrObserverToInform->OperationCompleted( iOpResult );
        }
    iMrObserverToInform = NULL;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CancelPendingMrCommandL()
// -----------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::CancelPendingMrCommandL()
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
// CFSEmailUiMailViewerVisualiser::OperationCompleted()
// From MRUI observer
// -----------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::OperationCompleted( TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    if ( aResult.iOpType == EESMRViewLaunch )
        {
        iMrUiActive = EFalse;
        if ( !iNextOrPevMessageSelected && iAppUi.CurrentActiveView() == this )
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
// CFSEmailUiMailViewerVisualiser::OperationError()
// -----------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::OperationError( TIcalViewerOperationResult aResult )
    {
    FUNC_LOG;
    // Show message in standard mail viewer in case launchin MRUI fails for any
    // other reason than viewer being cancelled by calling CancelPendingMrCommand().
    if ( aResult.iOpType == EESMRViewLaunch )
        {
        iMrUiActive = EFalse;
        if ( iAppUi.CurrentActiveView() == this && aResult.iResultCode != KErrCancel )
            {
            TRAP_IGNORE(
                SetMailboxNameToStatusPaneL();
                iAppUi.Display().Roster().Hide( ControlGroup() );
                RefreshL( ETrue );
                iAppUi.Display().Roster().ShowL( ControlGroup() );
                UpdateDownloadIndicatorL();
                );
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::SetMskL()
// -----------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::SetMskL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && iTextViewer ) // Safety
        {
        TInt ignoreThis( 0 );
        TBool visibleHotsSpot =
            iTextViewer->GetVisibleFocusedHotspotLine( ignoreThis );
        if ( visibleHotsSpot )
            {
            SViewerHeadingHotSpotData currentHeaderHotSpotData;
            CFindItemEngine::SFoundItem currentBodyHotSpotData;
            THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL(
                currentHeaderHotSpotData, currentBodyHotSpotData );

            if ( hotspotType == EHeaderHotspot )
                {
                if ( currentHeaderHotSpotData.iType ==
                        ETypeToAddressDisplayName ||
                     currentHeaderHotSpotData.iType ==
                        ETypeCcAddressDisplayName ||
                     currentHeaderHotSpotData.iType ==
                        ETypeBccAddressDisplayName ||
                     currentHeaderHotSpotData.iType ==
                        ETypeEmailAddress )
                    {
                    ChangeMskCommandL( R_FSE_QTN_MSK_COMPOSE );
                    }
                else if ( currentHeaderHotSpotData.iType ==
                    ETypeFromAddressDisplayName )
                    {
                    if ( !iEmbeddedMessageMode )
                        {
                        ChangeMskCommandL( R_FSE_QTN_MSK_REPLY );
                        }
                    else
                        {
                        // Reply option is not available in the embedded mode
                        ChangeMskCommandL( R_FSE_QTN_MSK_COMPOSE );
                        }
                    }
                else if ( currentHeaderHotSpotData.iType == ETypeHtml )
                    {
                    ChangeMskCommandL( R_FSE_QTN_MSK_OPENHTMLVIEWER );
                    }
                else if ( currentHeaderHotSpotData.iType == ETypeAttachment )
                    {
                    if ( iShowMskDownloadOption )
                        { // attachment not yet downloaded
                        ChangeMskCommandL( R_FSE_QTN_MSK_DOWNLOAD );
                        }
                    else if ( ShowCancelDownloadOption() )
                        { // attachment is being downloaded
                        ChangeMskCommandL( R_FSE_QTN_MSK_OPEN_BLOCKED );
                        }
                    else // attachment is already downloaded
                        {
                        ChangeMskCommandL( R_FSE_QTN_MSK_VIEWATTACHMENT );
                        }
                    }
                else if ( currentHeaderHotSpotData.iType == ETypeAttachments )
                    {
                    ChangeMskCommandL( R_FSE_QTN_MSK_VIEWATTACHMENTS );
                    }
                else if ( currentHeaderHotSpotData.iType ==
                            ETypeToNMoreRecipients ||
                          currentHeaderHotSpotData.iType ==
                            ETypeCcNMoreRecipients ||
                          currentHeaderHotSpotData.iType ==
                            ETypeBccNMoreRecipients )
                    {
                    ChangeMskCommandL( R_FSE_QTN_MSK_VIEWALL );
                    }
                }
             else if ( hotspotType == EBodyHotspot )
                {
                if ( currentBodyHotSpotData.iItemType ==
                    CFindItemEngine::EFindItemSearchMailAddressBin )
                    {
                    ChangeMskCommandL( R_FSE_QTN_MSK_COMPOSE );
                    }
                else if ( currentBodyHotSpotData.iItemType ==
                    CFindItemEngine::EFindItemSearchPhoneNumberBin )
                    {
                    ChangeMskCommandL( R_FSE_QTN_MSK_CALL );
                    }
                else if ( currentBodyHotSpotData.iItemType ==
                            CFindItemEngine::EFindItemSearchURLBin ||
                          currentBodyHotSpotData.iItemType ==
                            CFindItemEngine::EFindItemSearchScheme )
                    {
                    TInt schemaLinkType = ResolveBodyTextSchemaUrlTypeL( currentBodyHotSpotData );
                    switch ( schemaLinkType )
                        {
                        case EFocusOnEMailInBodyText:
                            ChangeMskCommandL( R_FSE_QTN_MSK_COMPOSE );
                            break;
                        case EFocusOnNumberWithinMessage:
                            ChangeMskCommandL( R_FSE_QTN_MSK_CALL );
                            break;
                        default:
                            ChangeMskCommandL( R_FSE_QTN_MSK_OPENURL );
                            break;
                        }
                    }
                }
             else // if hotspot is not known
                {
                //it will set context menu for email (reply, forward, etc.)
                ChangeMskCommandL( R_FSE_QTN_MSK_BODY_MENU );
                }
            }
        else
            {
        //it will set context menu for email (reply, forward, etc.)
            ChangeMskCommandL( R_FSE_QTN_MSK_BODY_MENU );
            }
        }
    }

// CFSEmailUiMailViewerVisualiser::OfferEventL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::OfferEventL(const TAlfEvent& aEvent)
    {
    FUNC_LOG;
    // Consume most keys to be safe. Let only few keys to flow to
    // text viewer because text viewer does some unexcepted actions
    // with some keys events (at least left arrow and 0 keys)
    TBool consumed( ETrue );

    // Handle key presses if we are ready to process them
    if ( iAsyncProcessComplete && !iAppUi.ViewSwitchingOngoing() && aEvent.IsKeyEvent() )
        {
        // Check keyboard shortcuts on key down event since half-QWERTY keyboard
        // has shortcuts on keys which do not send key event at all
        if ( aEvent.Code() == EEventKeyDown )
            {
            TInt shortcutCommand =
                iAppUi.ShortcutBinding().CommandForShortcutKey( aEvent.KeyEvent(),
                    CFSEmailUiShortcutBinding::EContextMailViewer );
            if ( shortcutCommand != KErrNotFound )
                {
                HandleCommandL( shortcutCommand );
                }
            }

        // Update MSK label when key released to avoid constant updating
        // on fast scrolling
        else if ( aEvent.Code() == EEventKeyUp )
            {
            SetMskL();
            }

        // Handle key events
        else if ( aEvent.Code() == EEventKey )
            {
            // Swap right and left controls in mirrored layout
            TInt scanCode = aEvent.KeyEvent().iScanCode;
            if ( AknLayoutUtils::LayoutMirrored() )
                {
                if ( scanCode == EStdKeyRightArrow ) scanCode = EStdKeyLeftArrow;
                else if ( scanCode == EStdKeyLeftArrow ) scanCode = EStdKeyRightArrow;
                }

            switch ( scanCode )
                {
                case EStdKeyRightArrow:
                    {
                    // Open action menu if it is available to current hotspot item
                    LaunchActionMenuL();
                    break;
                    }
                case EStdKeyYes:
                    {
                    // call current header hotspot name or current body hotspot number
                    CallHotSpotAddressL();
                    break;
                    }
                case EStdKeyDownArrow:
                case EStdKeyUpArrow:
                case EStdKeyNull: // simulated keys events
                    {
                    // These keys are passed on to TextViewer
                    consumed = EFalse;
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            }
        }

    return consumed;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ClearMailViewer
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::ClearMailViewer()
    {
    FUNC_LOG;
    iAppUi.Display().Roster().Hide( ControlGroup() );
    if ( iTextViewer )
        {
        if ( iTextViewerControl )
            {
            ControlGroup().Remove( iTextViewerControl );
            }
        if ( iControl )
            {
            ControlGroup().Remove( iControl );
            }
        delete iTextViewer; iTextViewer = NULL;
        delete iTextViewerControl; iTextViewerControl = NULL;
        delete iControl; iControl = NULL;
        }
    if ( iViewerRichText )
        {
        iViewerRichText->Reset();
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::UpdateMailViewerL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::UpdateMailViewerL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        ClearMailViewer();

        // Create text viewer and store control pointer
        iTextViewer = CFsTextViewer::NewL( iEnv );
        iTextViewerControl = iTextViewer->GetControl();
        iTextViewer->SetObserver( this );

        // Set the artificial shortcut keys used to command the viewer to scroll up/down.
        // These are simulated when corresponding shortcut command is received from the
        // standard shortcut binder of FsEmailUI.
        CFsTextViewerKeys* keys = CFsTextViewerKeys::NewL();
        keys->SetKeyPgUp( EKeyPageUp );
        keys->SetKeyPgDown( EKeyPageDown );
        keys->SetKeyScrollUp( KKeyCodeArtificialScrollUp );
        keys->SetKeyScrollDown( KKeyCodeArtificialScrollDown );
        iTextViewer->SetCustomKeys( keys );

        iTextViewer->SetMirroring( AknLayoutUtils::LayoutMirrored() );

        iViewerRichText->RefreshRichTextL( *iTextViewer, *iMessage, *iMailBox );

        // These need to be called before setting text to the viewer.
        iTextViewer->SetLeftMargin( iAppUi.LayoutHandler()->ViewerLeftMarginInPixels() );
        iTextViewer->SetRightMargin( iAppUi.LayoutHandler()->ViewerRightMarginInPixels() );

        SetActionButtonIconAndHighLight();

        iTextViewer->SetTextL( &iViewerRichText->RichText(), iSmDictionary );

        // ownerships are transfered to AlfEnviroment.
        //<cmail> Compared to S60 3.2.3 in S60 5.0 Alf offers the key events in
        // opposite order.
        // Create control for capturing left and right navigation events
        iControl = CFreestyleEmailUiMailViewerControl::NewL( iEnv, *this );
        ControlGroup().AppendL( iControl );
        ControlGroup().AppendL( iTextViewerControl );
        //</cmail>

        iLayoutChangedWhileNotActive = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::SetActionButtonIconAndHighLight
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::SetActionButtonIconAndHighLight()
    {
    FUNC_LOG;
    CFreestyleEmailUiTextureManager* textureMgr = iAppUi.FsTextureManager();
    CFSEmailUiLayoutHandler* layoutMgr = iAppUi.LayoutHandler();

    // get textures from texture manager
    CAlfTexture& menuTexture = textureMgr->TextureByIndex( EListControlMenuIcon );
    CAlfTexture& selectortexture = textureMgr->TextureByIndex( EViewerHighlightTexture );

    iTextViewer->SetLiteLineBg( selectortexture, layoutMgr->ViewerSelectorOpacity() );

    iTextViewer->SetActionButton( menuTexture, 1  );
    iTextViewer->SetActionButtonMargin( layoutMgr->ViewerActionMenuIconMargin() );
    TSize btnSize = layoutMgr->ViewerActionMenuIconSize();
    iTextViewer->SetActionButtonSize( btnSize.iWidth, btnSize.iHeight );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::LaunchActionMenuL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::LaunchActionMenuL()
    {
    FUNC_LOG;
    // check that current hotspot is visible
    TInt ignoreThis; // not used here. Only return value is used not this mandatory reference parameter
    TBool visibleHotsSpot = iTextViewer->GetVisibleFocusedHotspotLine( ignoreThis );
    if( visibleHotsSpot )
        {
        // Remove items
        CFSEmailUiActionMenu::RemoveAllL();

        // check which hotspot is active.
        SViewerHeadingHotSpotData currentHeaderHotSpotData;
        CFindItemEngine::SFoundItem currentBodyHotSpotData;
        THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL( currentHeaderHotSpotData, currentBodyHotSpotData );

        TActionMenuType currentActionMenuType = ENoActionMenuFocused;
        if( hotspotType == EHeaderHotspot && (
            currentHeaderHotSpotData.iType == ETypeFromAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeToAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeCcAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeBccAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeEmailAddress ) )
            {
            currentActionMenuType = EFocusOnNameInAddressField;
            }
        else if( hotspotType == EHeaderHotspot && (
            currentHeaderHotSpotData.iType == ETypeAttachment ) )
            {
            currentActionMenuType = EFocusOnAttachmentName;
            }
        else if( hotspotType == EHeaderHotspot && (
            currentHeaderHotSpotData.iType == ETypeAttachments ) )
            {
            currentActionMenuType = EFocusOnAttachmentsText;
            }
        else if( hotspotType == EBodyHotspot &&
                currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchPhoneNumberBin )
            {
            currentActionMenuType = EFocusOnNumberWithinMessage;
            }
        else if( hotspotType == EBodyHotspot && ( currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchURLBin ||
                 currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchScheme ) )
            {
            TInt schemaLinkType = ResolveBodyTextSchemaUrlTypeL( currentBodyHotSpotData );
            switch ( schemaLinkType )
                {
                case EFocusOnEMailInBodyText:
                    currentActionMenuType = EFocusOnEMailInBodyText;
                    break;
                case EFocusOnNumberWithinMessage:
                    currentActionMenuType = EFocusOnNumberWithinMessage;
                    break;
                default:
                    currentActionMenuType = EFocusOnHyperLinkWithinMessage;
                    break;

                }
            }
        else if( hotspotType == EBodyHotspot &&
                currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchMailAddressBin )
            {
            currentActionMenuType = EFocusOnEMailInBodyText;
            }


        if( currentActionMenuType != ENoActionMenuFocused )
            {
            CreateActionMenuItemsL( currentActionMenuType );
            TActionMenuCustomItemId menuResult = CFSEmailUiActionMenu::ExecuteL( EFscCenter );
            if ( menuResult != FsEActionMenuCasItemSelectedAndExecuted &&
                    menuResult != FsEActionMenuDismissed    )
                {
                HandleActionMenuCommandL( menuResult, currentActionMenuType );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ResolveBodyTextSchemaUrlTypeL
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerVisualiser::ResolveBodyTextSchemaUrlTypeL( CFindItemEngine::SFoundItem aHotSpot )
    {
    FUNC_LOG;
    TInt ret( EFocusOnHyperLinkWithinMessage );
    HBufC* schemeText = iViewerRichText->GetHotspotTextLC( aHotSpot );
    if ( schemeText->FindC( KMailtoPrefix ) == 0 )
        {
        ret = EFocusOnEMailInBodyText;
        }
    else if ( schemeText->FindC( KCallPrefix ) == 0 ||
              schemeText->FindC( KTelPrefix ) == 0 ||
              schemeText->FindC( KSmsPrefix ) == 0 ||
              schemeText->FindC( KMmsPrefix ) == 0 ||
              schemeText->FindC( KVoipPrefix ) == 0 )
        {
        ret = EFocusOnNumberWithinMessage;
        }
    else
        {
        ret = EFocusOnHyperLinkWithinMessage;
        }
    CleanupStack::PopAndDestroy( schemeText );
    return ret;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::SetActionMenuIconVisbilityL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::SetActionMenuIconVisbilityL()
    {
    FUNC_LOG;
    if ( iTextViewer )
        {
        if ( ShowActionsMenuInOptionsL() )
            {
            iTextViewer->ShowActionButtonForCurrentHighlightedHotspotL();
            }
        else
            {
            iTextViewer->HideActionButtonFromCurrentHighlightedHotspot();
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CreateActionMenuItemsL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::CreateActionMenuItemsL(
    TActionMenuType aActionMenuType )
    {
    FUNC_LOG;
    // See the ui spec for all the right action menus according to
    // currently selected hotspot
    RArray<TActionMenuCustomItemId> uids;
    CleanupClosePushL( uids );
    if ( aActionMenuType == EFocusOnNumberWithinMessage )
        {
        uids.Append( FsEActionMenuCall );
        uids.Append( FsEActionMenuAddToContacts );
        // Drop FsEActionMenuCreateMessage from sip: addresses
        // First check which hotspot is active.
        SViewerHeadingHotSpotData currentHeaderHotSpotData;
        CFindItemEngine::SFoundItem currentBodyHotSpotData;
        THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL(
            currentHeaderHotSpotData, currentBodyHotSpotData );
        HBufC* schemeText = iViewerRichText->GetHotspotTextLC( currentBodyHotSpotData );
        if ( schemeText &&  schemeText->FindC( KVoipPrefix ) != 0 )
            {
            uids.Append( FsEActionMenuCreateMessage );
            }
        CleanupStack::PopAndDestroy( schemeText );
        if ( TFsEmailUiUtility::IsRemoteLookupSupported( *iMailBox ) )
            {
            uids.Append( FsEActionMenuRemoteLookup );
            }
        }
    else if ( aActionMenuType == EFocusOnNameInAddressField ||
              aActionMenuType == EFocusOnEMailInBodyText )
        {
        uids.Append( FsEActionMenuCall );
        uids.Append( FsEActionMenuCreateMessage );
        uids.Append( FsEActionMenuCreateEmail );
        uids.Append( FsEActionMenuContactDetails );
        uids.Append( FsEActionMenuAddToContacts );
        if ( TFsEmailUiUtility::IsRemoteLookupSupported( *iMailBox ) )
            {
            uids.Append( FsEActionMenuRemoteLookup );
            }
        }
    else if ( aActionMenuType == EFocusOnHyperLinkWithinMessage )
        {
        uids.Append( FsEActionMenuOpenInWeb );
        uids.Append( FsEActionMenuOpenInIntranet );
        uids.Append( FsEActionMenuBookmark );
        }
    else if ( aActionMenuType == EFocusOnAttachmentName )
        {
        if ( ShowOpenAttachmentOptionL() )
            {
            uids.Append( FsEActionAttachmentOpen );
            }
        if ( ShowDownloadOptionL() )
            {
            uids.Append( FsEActionAttachmentDownload );
            }
        if ( ShowCancelDownloadOption() )
            {
            uids.Append( FsEActionAttachmentCancelDownload );
            }
        if ( ShowSaveAttachmentOptionL() )
            {
            uids.Append( FsEActionAttachmentSave );
            }
        if ( ShowOpenAttachmentOptionL() )
            {
            uids.Append( FsEActionAttachmentClearFetchedContent );
            }
        }
    else if ( aActionMenuType == EFocusOnAttachmentsText )
        {
        if ( ShowOpenAttachmentOptionL() )
            {
            uids.Append( FsEActionAttachmentViewAll );
            }
        if ( ShowDownloadOptionL() )
            {
            uids.Append( FsEActionAttachmentDownloadAll );
            }
        if ( ShowCancelDownloadOption() )
            {
            uids.Append( FsEActionAttachmentCancelAllDownloads );
            }
        if ( ShowSaveAttachmentOptionL() )
            {
            uids.Append( FsEActionAttachmentSaveAll );
            }
        }

    TInt uidsCount = uids.Count();
    for ( TInt i = 0; i <  uidsCount; i++ )
        {
        CFSEmailUiActionMenu::AddCustomItemL( uids[i] );
        }
    CleanupStack::PopAndDestroy( &uids );
    }


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::HandleActionMenuCommandL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::HandleActionMenuCommandL(
    TActionMenuCustomItemId aSelectedActionMenuItem,
    TActionMenuType aActionMenuType )
    {
    FUNC_LOG;
    // check which hotspot is active.
    SViewerHeadingHotSpotData currentHeaderHotSpotData;
    CFindItemEngine::SFoundItem currentBodyHotSpotData;
    THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL(
        currentHeaderHotSpotData, currentBodyHotSpotData );

    if ( aActionMenuType == EFocusOnNumberWithinMessage )
        {
        switch ( aSelectedActionMenuItem  )
            {
            case FsEActionMenuCall: // Call
                {
                CallToBodyHotSpotNumberL( currentBodyHotSpotData );
                }
                break;
            case FsEActionMenuAddToContacts: // Add to Contacts
                {
                HBufC* hotspotText = iViewerRichText->GetHotspotTextLC(
                    currentBodyHotSpotData );
                SavePhoneNumberAsContactL( *hotspotText );
                CleanupStack::PopAndDestroy( hotspotText );
                }
                break;
            case FsEActionMenuCreateMessage: // Create message
                {
                CreateMessageL();
                }
                break;
            case FsEActionMenuRemoteLookup: // Remote lookup
                {
                HBufC* hotspotText = iViewerRichText->GetHotspotTextLC(
                    currentBodyHotSpotData );
                LaunchRemoteLookupL( *hotspotText );
                CleanupStack::PopAndDestroy( hotspotText );
                }
                break;
            }
        }
    else if ( aActionMenuType == EFocusOnNameInAddressField )
        {
        switch( aSelectedActionMenuItem )
            {
            case FsEActionMenuCall: // Call
                {
                CallHotSpotAddressL();
                }
                break;
            case FsEActionMenuCreateMessage: // Create message
                {
                CreateMessageL();
                }
                break;
            case FsEActionMenuCreateEmail: // Create message
                {
                ComposeMailL();
                }
                break;
            case FsEActionMenuContactDetails: // Contact details
                {
                OpenContactDetailsL();
                }
                break;
            case FsEActionMenuAddToContacts: // Add to Contacts
                {
                HBufC* emailAddress =
                    iViewerRichText->GetHeaderHotspotEmailAddressLC(
                        currentHeaderHotSpotData );
                SaveEmailAsContactL( *emailAddress );
                CleanupStack::PopAndDestroy( emailAddress );
                }
                break;
            case FsEActionMenuRemoteLookup: // Remote lookup
                {
                HBufC* emailAddress =
                    iViewerRichText->GetHeaderHotspotEmailAddressLC(
                        currentHeaderHotSpotData );
                LaunchRemoteLookupL( *emailAddress );
                CleanupStack::PopAndDestroy( emailAddress );
                }
                break;
            }
        }
    else if ( aActionMenuType == EFocusOnHyperLinkWithinMessage )
        {
        switch ( aSelectedActionMenuItem )
            {
            case FsEActionMenuOpenInWeb: // Open in web
                {
                OpenHotSpotUrlInBrowserL( currentBodyHotSpotData );
                }
                break;
            case FsEActionMenuOpenInIntranet: // Open in intranet
                {
                OpenLinkInIntranetL( EFalse );
                }
                break;
            case FsEActionMenuBookmark: // Bookmark
                {
                AddToBookmarksL( currentBodyHotSpotData );
                }
                break;
            }
        }
    else if ( aActionMenuType == EFocusOnAttachmentName ||
              aActionMenuType == EFocusOnAttachmentsText )
        {
        switch ( aSelectedActionMenuItem )
            {
            case FsEActionAttachmentOpen: // Open single attachment
                {
                OpenAttachmentL();
                }
                break;
            case FsEActionAttachmentViewAll: // Open attachments list
                {
                OpenAttachmentsViewL();
                }
                break;
            case FsEActionAttachmentDownload: // Dowload one
            case FsEActionAttachmentDownloadAll : /// all attachments
                {
                StartDowloadingAttachmentsL();
                }
                break;
            case FsEActionAttachmentCancelDownload: // Cancel one / all downloads
            case FsEActionAttachmentCancelAllDownloads:
                {
                CancelDowloadingAttachmentsL();
                }
                break;
            case FsEActionAttachmentSave: // Save one / all attachments
            case FsEActionAttachmentSaveAll:
                {
                SaveAllAttachmentsL();
                }
                break;
            case FsEActionAttachmentClearFetchedContent:
                {
                RemoveFetchedAttachmentL();
                }
                break;
            }
        }
    else if ( aActionMenuType == EFocusOnEMailInBodyText )
        {
        switch ( aSelectedActionMenuItem )
            {
            case FsEActionMenuCall: // Call
                {
                CallHotSpotAddressL();
                }
                break;
            case FsEActionMenuCreateMessage: // Create message
                {
                CreateMessageL();
                }
                break;
            case FsEActionMenuCreateEmail: // Create message
                {
                ComposeMailL();
                }
                break;
            case FsEActionMenuContactDetails: // Contact details
                {
                OpenContactDetailsL();
                }
                break;
            case FsEActionMenuAddToContacts: // Add to Contacts
                {
                HBufC* hotspotText = iViewerRichText->GetHotspotTextLC(
                    currentBodyHotSpotData );
                if ( currentBodyHotSpotData.iItemType ==
                    CFindItemEngine::EFindItemSearchPhoneNumberBin )
                    {
                    SavePhoneNumberAsContactL( *hotspotText );
                    }
                else if ( currentBodyHotSpotData.iItemType ==
                    CFindItemEngine::EFindItemSearchMailAddressBin )
                    {
                    SaveEmailAsContactL( *hotspotText );
                    }
                else if ( currentBodyHotSpotData.iItemType ==
                    CFindItemEngine::EFindItemSearchScheme )
                    {
                    if ( hotspotText->FindC( KMailtoPrefix ) == 0 )
                        {
                        // Save email as contact, address mailto: strip is done in SaveEmailAsContactL
                        SaveEmailAsContactL( *hotspotText );
                        }
                    }
                CleanupStack::PopAndDestroy( hotspotText );
                }
                break;
            case FsEActionMenuRemoteLookup: // Remote lookup
                {
                HBufC* hotspotText = iViewerRichText->GetHotspotTextLC(
                    currentBodyHotSpotData );
                LaunchRemoteLookupL( *hotspotText );
                CleanupStack::PopAndDestroy( hotspotText );
                }
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::OpenContactDetailsL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::OpenContactDetailsL()
    {
    FUNC_LOG;
    SViewerHeadingHotSpotData currentHeaderHotSpotData;
    CFindItemEngine::SFoundItem currentBodyHotSpotData;
    THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL( currentHeaderHotSpotData, currentBodyHotSpotData );

    HBufC* emailAddress = NULL;
    if ( hotspotType == EHeaderHotspot )
        {
        emailAddress = iViewerRichText->GetHeaderHotspotEmailAddressLC( currentHeaderHotSpotData );
        }
    else if ( hotspotType == EBodyHotspot )
        {
        emailAddress = iViewerRichText->GetHotspotTextLC( currentBodyHotSpotData );
        // Delete "mailto:" prefix if found
        if ( emailAddress->FindC( KMailtoPrefix ) == 0 )
            {
            emailAddress->Des().Delete( 0, KMailtoPrefix().Length() );
            }
        }
    else
        {
        return;
        }

    CFsDelayedLoader::InstanceL()->GetContactHandlerL()->ShowContactDetailsL(
            *emailAddress, EContactUpdateEmail, NULL );

    CleanupStack::PopAndDestroy( emailAddress );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::LaunchHtmlViewerL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::LaunchHtmlViewerL()
    {
    FUNC_LOG;
    if ( MessagePartFullyFetchedL( EMessageHtmlBodyPart ) )
        {
        THtmlViewerActivationData htmlData;
        htmlData.iActivationDataType = THtmlViewerActivationData::EMailMessage;
        htmlData.iMailBoxId = iMessage->GetMailBoxId();
        htmlData.iFolderId = iMessage->GetFolderId();
        htmlData.iMessageId = iMessage->GetMessageId();
        htmlData.iEmbeddedMessageMode = iEmbeddedMessageMode;
        TPckgBuf<THtmlViewerActivationData> pckgData( htmlData );

        iAppUi.EnterFsEmailViewL( HtmlViewerId, KHtmlViewerOpenNew, pckgData );
        }
    else
        {
        StartWaitedFetchingL( EMessageHtmlBodyPart );
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::LaunchRemoteLookupL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::LaunchRemoteLookupL( const TDesC& aDes ) const
    {
    FUNC_LOG;
    // this method assumes that remote lookup is available with current plugin.
    HBufC* textData = HBufC::NewLC( aDes.Length() );
    textData->Des().Append( aDes );
    if ( textData->FindC( KMailtoPrefix ) == 0 )
        {
        textData->Des().Delete( 0, KMailtoPrefix().Length() );
        }
    else if ( textData->FindC( KCallPrefix ) == 0 )
        {
        textData->Des().Delete( 0, KCallPrefix().Length() );
        }
    else if ( textData->FindC( KTelPrefix ) == 0 )
        {
        textData->Des().Delete( 0, KTelPrefix().Length() );
        }
    else if ( textData->FindC( KSmsPrefix ) == 0 )
        {
        textData->Des().Delete( 0, KSmsPrefix().Length() );
        }
    else if ( textData->FindC( KMmsPrefix ) == 0 )
        {
        textData->Des().Delete( 0, KMmsPrefix().Length() );
        }
    else if ( textData->FindC( KVoipPrefix ) == 0 )
        {
        textData->Des().Delete( 0, KVoipPrefix().Length() );
        }

    CFsDelayedLoader::InstanceL()->GetContactHandlerL()->LaunchRemoteLookupWithQueryL( *iMailBox, *textData );
    CleanupStack::PopAndDestroy( textData );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::AddToBookmarksL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::AddToBookmarksL(
                const CFindItemEngine::SFoundItem& aBodyHotspotData ) const
    {
    FUNC_LOG;
    HBufC* hotspotText =
        iViewerRichText->GetHotspotTextLC( aBodyHotspotData );

    RFavouritesSession fSession;
    User::LeaveIfError( fSession.Connect() );
    CleanupClosePushL( fSession );

    RFavouritesDb favoris;
    User::LeaveIfError( favoris.Open( fSession, KBrowserBookmarks ) );
    CleanupClosePushL( favoris );

    CFavouritesItem *favorisItem = CFavouritesItem::NewLC();
    favorisItem->SetType( CFavouritesItem::EItem );
    favorisItem->SetUrlL( *hotspotText );
    // Should be less than KFavouritesMaxName ( 50 )
    if ( hotspotText->Length() > KFavouritesMaxName )
        {
        favorisItem->SetNameL( hotspotText->Left( KFavouritesMaxName ) );
        }
    else
        {
        favorisItem->SetNameL( *hotspotText );
        }

    favorisItem->SetParentFolder( KFavouritesRootUid );

    TInt error = favoris.Add( *favorisItem, ETrue );

    CleanupStack::PopAndDestroy( favorisItem );
    CleanupStack::PopAndDestroy( &favoris );
    CleanupStack::PopAndDestroy( &fSession );

    CleanupStack::PopAndDestroy( hotspotText );

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

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CallHotSpotAddressL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::CallHotSpotAddressL()
    {
    FUNC_LOG;
    SViewerHeadingHotSpotData currentHeaderHotSpotData;
    CFindItemEngine::SFoundItem currentBodyHotSpotData;
    THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL( currentHeaderHotSpotData, currentBodyHotSpotData );

   if( hotspotType == EHeaderHotspot &&  currentHeaderHotSpotData.iType == ETypeFromAddressDisplayName )
        {
        HBufC* emailAddress = iViewerRichText->GetHeaderHotspotEmailAddressLC( currentHeaderHotSpotData );
           CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCallToContactByEmailL( *emailAddress,
                    iAppUi.GetActiveMailbox(), this, ETrue /*use call sender text*/);
           CleanupStack::PopAndDestroy( emailAddress );
        }
   else if( ( hotspotType == EHeaderHotspot &&
            ( currentHeaderHotSpotData.iType == ETypeToAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeCcAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeBccAddressDisplayName ||
            currentHeaderHotSpotData.iType == ETypeEmailAddress )
            )
        )
        {
        HBufC* emailAddress = iViewerRichText->GetHeaderHotspotEmailAddressLC( currentHeaderHotSpotData );
        CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCallToContactByEmailL( *emailAddress,
                 iAppUi.GetActiveMailbox(), this, EFalse /*use call sender text*/);
        CleanupStack::PopAndDestroy( emailAddress );
        }
    else if( hotspotType == EBodyHotspot &&
                currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchMailAddressBin  )
        {
        HBufC* emailAddress = iViewerRichText->GetHotspotTextLC( currentBodyHotSpotData );
        CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCallToContactByEmailL( *emailAddress,
                 iAppUi.GetActiveMailbox(), this, EFalse );
        CleanupStack::PopAndDestroy( emailAddress );
        }
    else if( hotspotType == EBodyHotspot &&
                currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchPhoneNumberBin )
        {
        CallToBodyHotSpotNumberL( currentBodyHotSpotData );
        }
    else if ( currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchScheme )
        {
        // Get scheme text and make sure there is "mailto:" text or "call:" text
        HBufC* schemeText = iViewerRichText->GetHotspotTextLC( currentBodyHotSpotData );
        if ( schemeText &&  schemeText->FindC( KMailtoPrefix ) == 0 )
            {
            schemeText->Des().Delete( 0, KMailtoPrefix().Length() );
            CFsDelayedLoader::InstanceL()->GetContactHandlerL()->FindAndCallToContactByEmailL( *schemeText,
                 iAppUi.GetActiveMailbox(), this, EFalse);
            }
        else if ( schemeText->FindC( KCallPrefix ) == 0 ||
                  schemeText->FindC( KTelPrefix ) == 0 ||
                  schemeText->FindC( KSmsPrefix ) == 0 ||
                  schemeText->FindC( KMmsPrefix ) == 0 ||
                  schemeText->FindC( KVoipPrefix ) == 0 )
            {
            CallToBodyHotSpotNumberL( currentBodyHotSpotData );
            }
        CleanupStack::PopAndDestroy( schemeText );
        }

    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CreateMessageL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::CreateMessageL() const
    {
    FUNC_LOG;
    SViewerHeadingHotSpotData currentHeaderHotSpotData;
    CFindItemEngine::SFoundItem currentBodyHotSpotData;
    THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL( currentHeaderHotSpotData, currentBodyHotSpotData );

    if( hotspotType == EHeaderHotspot )
        {
        HBufC* emailAddress = iViewerRichText->GetHeaderHotspotEmailAddressLC( currentHeaderHotSpotData );
        CFSEmailUiContactHandler* cntHandlerInstance = CFsDelayedLoader::InstanceL()->GetContactHandlerL();
        cntHandlerInstance->FindAndCreateMsgToContactByEmailL( *emailAddress, iAppUi.GetActiveMailbox() );
        CleanupStack::PopAndDestroy( emailAddress );
        }
    else if( hotspotType == EBodyHotspot )
        {
        // Sending based on email address, goes to contact handler
        if ( currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchMailAddressBin )
            {
            HBufC* emailAddress = iViewerRichText->GetHotspotTextLC( currentBodyHotSpotData );
            CFSEmailUiContactHandler* cntHandlerInstance = CFsDelayedLoader::InstanceL()->GetContactHandlerL();
            cntHandlerInstance->FindAndCreateMsgToContactByEmailL( *emailAddress, iAppUi.GetActiveMailbox() );
            CleanupStack::PopAndDestroy( emailAddress );
            }
        // Sending based on found number, goes to utility class
        else if ( currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchPhoneNumberBin )
            {
            HBufC* hotspotText = iViewerRichText->GetHotspotTextLC( currentBodyHotSpotData );
            TFsEmailUiUtility::ShowCreateMessageQueryL( *hotspotText );
            CleanupStack::PopAndDestroy( hotspotText );
            }
        else if ( currentBodyHotSpotData.iItemType == CFindItemEngine::EFindItemSearchScheme )
            {
            // Get scheme text and make sure there is "mailto:" text or "call:" text
            HBufC* schemeText = iViewerRichText->GetHotspotTextLC( currentBodyHotSpotData );
            if ( schemeText && ( schemeText->FindC( KMailtoPrefix ) == 0 ||
                 schemeText->FindC( KVoipPrefix ) == 0) )
                {
                schemeText->Des().Delete( 0, KMailtoPrefix().Length() );
                CFSEmailUiContactHandler* cntHandlerInstance = CFsDelayedLoader::InstanceL()->GetContactHandlerL();
                cntHandlerInstance->FindAndCreateMsgToContactByEmailL( *schemeText, iAppUi.GetActiveMailbox() );
                }
            else if ( schemeText && schemeText->FindC( KCallPrefix ) == 0 )
                {
                schemeText->Des().Delete( 0, KCallPrefix().Length() );
                TFsEmailUiUtility::ShowCreateMessageQueryL( *schemeText );
                }
            else if ( schemeText && schemeText->FindC( KTelPrefix ) == 0 )
                {
                schemeText->Des().Delete( 0, KTelPrefix().Length() );
                TFsEmailUiUtility::ShowCreateMessageQueryL( *schemeText );
                }
            else if ( schemeText && schemeText->FindC( KSmsPrefix ) == 0 )
                {
                schemeText->Des().Delete( 0, KSmsPrefix().Length() );
                TFsEmailUiUtility::ShowCreateMessageQueryL( *schemeText );
                }
            else if ( schemeText && schemeText->FindC( KMmsPrefix ) == 0 )
                {
                schemeText->Des().Delete( 0, KMmsPrefix().Length() );
                TFsEmailUiUtility::ShowCreateMessageQueryL( *schemeText );
                }
            CleanupStack::PopAndDestroy( schemeText );
            }
        }
    }

TBool CFSEmailUiMailViewerVisualiser::IsCopyToClipBoardAvailableL() const
    {
    FUNC_LOG;
    TBool available = EFalse;
    TInt ignoreThis( 0 );
    TBool visibleHotsSpot =
        iTextViewer->GetVisibleFocusedHotspotLine( ignoreThis );

    // Copy to clipboard is hidden if there's no visible hotspot focus
    if ( visibleHotsSpot )
        {
        // Check the type of focused hotspot
        SViewerHeadingHotSpotData currentHeaderHotSpotData;
        CFindItemEngine::SFoundItem currentBodyHotSpotData;
        THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL(
            currentHeaderHotSpotData, currentBodyHotSpotData );

        if ( hotspotType == EBodyHotspot )
            {
            // copying is available for all body hotspots
            available = ETrue;
            }
        else if ( hotspotType == EHeaderHotspot )
            {
            // in header, copying is available for email addresses
            switch ( currentHeaderHotSpotData.iType )
                {
                case ETypeFromAddressDisplayName:
                case ETypeToAddressDisplayName:
                case ETypeCcAddressDisplayName:
                case ETypeBccAddressDisplayName:
                case ETypeEmailAddress:
                    available = ETrue;
                    break;
                default:
                    available = EFalse;
                    break;
                }
            }
        else // hotspotType == ENoHotspot
            {
            available = EFalse;
            }
        }

    return available;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CopyCurrentHotspotToClipBoardL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::CopyCurrentHotspotToClipBoardL() const
    {
    FUNC_LOG;
    TInt ignoreThis( 0 );
    TBool visibleHotsSpot =
        iTextViewer->GetVisibleFocusedHotspotLine( ignoreThis );

    // Copy to clipboard is disabled if there's no visible hotspot focus
    if ( visibleHotsSpot )
        {
        HBufC* clipBoardText = NULL;
        SViewerHeadingHotSpotData currentHeaderHotSpotData;
        CFindItemEngine::SFoundItem currentBodyHotSpotData;
        THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL( currentHeaderHotSpotData, currentBodyHotSpotData );

        if ( hotspotType == EHeaderHotspot )
            {
            switch ( currentHeaderHotSpotData.iType )
                {
                case ETypeFromAddressDisplayName:
                case ETypeToAddressDisplayName:
                case ETypeCcAddressDisplayName:
                case ETypeBccAddressDisplayName:
                    {
                    // Get email address attachned to display name, cropped display name has no use
                    clipBoardText = iViewerRichText->GetEmailAddressLC( currentHeaderHotSpotData );
                    }
                    break;
                case ETypeEmailAddress:
                    {
                    clipBoardText = iViewerRichText->GetHotspotTextLC( currentHeaderHotSpotData );
                    }
                    break;
                }
            }
        else if ( hotspotType == EBodyHotspot )
            {
            clipBoardText = iViewerRichText->GetHotspotTextLC( currentBodyHotSpotData );
            }

        if ( clipBoardText )
            {
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
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowOpenAttachmentOptionL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::ShowOpenAttachmentOptionL()
    {
    FUNC_LOG;
    TInt ret = EFalse;

    RPointerArray<CFSMailMessagePart> attachments;
    CleanupResetAndDestroyClosePushL( attachments );
    iMessage->AttachmentListL( attachments );

    // The Open option is always available when focus on the multiple
    // attachments item.
    if ( attachments.Count() > 1 )
        {
        ret = ETrue;
        }
    // When focus on single attachment item, the Open is shown only when the
    // attachment is fully downloaded
    else if ( attachments.Count() == 1 )
        {
        TPartData partData = MailData();
        partData.iMessagePartId = attachments[0]->GetPartId();
        if ( iAppUi.DownloadInfoMediator() && !iAppUi.DownloadInfoMediator()->IsDownloadableL( partData ) &&
             !iAppUi.DownloadInfoMediator()->IsDownloading( partData.iMessagePartId ) )
            {
            ret = ETrue;
            }
        }
    else
        {
        ret = EFalse;
        }

    CleanupStack::PopAndDestroy( &attachments );
    return ret;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowSaveAttachmentOptionL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::ShowSaveAttachmentOptionL()
    {
    FUNC_LOG;
    // Save (all) is displayed if there are no ongoing downloads.
    TBool saveAvailable = !ShowCancelDownloadOption();

    // When viewing an embedded message object, saving needs to be disabled
    // in case there are any attachments of message type. This is due to
    // limitations in the Activesync plugin.
    if ( saveAvailable && iEmbeddedMessageMode )
        {
        RPointerArray<CFSMailMessagePart> attachments;
        CleanupResetAndDestroyClosePushL( attachments );
        iMessage->AttachmentListL( attachments );

        for ( TInt i = 0 ; i < attachments.Count() && saveAvailable ; ++i )
            {
            const TDesC& attName = attachments[i]->AttachmentNameL();
            const TDesC& attMime = attachments[i]->GetContentType();
            TFileType attType = TFsEmailUiUtility::GetFileType( attName, attMime );
            if ( attType == EMessageType )
                {
                saveAvailable = EFalse;
                }
            }

        CleanupStack::PopAndDestroy( &attachments );
        }
    return saveAvailable;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowDownloadOptionL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::ShowDownloadOptionL()
    {
    FUNC_LOG;
    TBool retVal = EFalse;

    TPartData partData = MailData();

    RPointerArray<CFSMailMessagePart> attachments;
    CleanupResetAndDestroyClosePushL( attachments );
    iMessage->AttachmentListL( attachments );
    TInt attachmentsCount = attachments.Count();
    for ( TInt i = 0; i < attachmentsCount; i++ )
        {
        partData.iMessagePartId = attachments[i]->GetPartId();
        if ( iAppUi.DownloadInfoMediator() && iAppUi.DownloadInfoMediator()->IsDownloadableL( partData ) )
            {
            retVal = ETrue;
            break;
            }
        }
    CleanupStack::PopAndDestroy( &attachments );
    return retVal;
    }


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowDownloadOptionL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::ShowMskDownloadOptionL()
    {
    FUNC_LOG;
    // Check status only if download status might have changed
    TBool ret( EFalse );
    if ( iMessage && iMessage->IsFlagSet(EFSMsgFlag_Attachments) )
        {
        TPartData partData = MailData();
        RPointerArray<CFSMailMessagePart> attachments;
        CleanupResetAndDestroyClosePushL( attachments );
        iMessage->AttachmentListL( attachments );
        // Safety, this function should not be called for any other amount than 1
        if ( attachments.Count() == 1 )
            {
            partData.iMessagePartId = attachments[0]->GetPartId();
            if ( iAppUi.DownloadInfoMediator() && iAppUi.DownloadInfoMediator()->IsDownloadableL( partData ) )
                {
                ret = ETrue;
                }
            }
        CleanupStack::PopAndDestroy( &attachments );
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowCancelDownloadOption
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::ShowCancelDownloadOption()
    {
    FUNC_LOG;
    TBool ret( EFalse );
    if ( iMessage && iAppUi.DownloadInfoMediator() )
        {
        ret = iAppUi.DownloadInfoMediator()->IsAnyAttachmentDownloads(
                iMessage->GetMessageId() );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::OpenAttachmentL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::OpenAttachmentL()
    {
    FUNC_LOG;
    if ( !iMessage )
        {
        User::Leave( KErrNotReady );
        }

    RPointerArray<CFSMailMessagePart> attachments;
    CleanupResetAndDestroyClosePushL( attachments );
    iMessage->AttachmentListL( attachments );

    // start downloading if not yet downloaded.
    if ( ShowDownloadOptionL() )
        {
        StartDowloadingAttachmentsL();
        }
    else if ( ShowCancelDownloadOption() ) // show download ongoing info
        {
        TFsEmailUiUtility::ShowInfoNoteL(
            R_FSE_VIEWER_NOTE_ATTACHMENT_DOWNLOADING_PROGRESS );
        }
    else // open if attachment is already downloaded
        {
        TInt attachmentsCount = attachments.Count();
        if ( attachmentsCount )
            {
            // <cmail>
            TFsEmailUiUtility::OpenAttachmentL( *attachments[attachmentsCount-1] );
            // </cmail>
            }
        }

    CleanupStack::PopAndDestroy( &attachments );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::OpenAttachmentsViewL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::OpenAttachmentsViewL()
    {
    FUNC_LOG;
    // give pointer to current for attachments list view
    TAttachmentListActivationData params;
    params.iMailBoxId = iMessage->GetMailBoxId();
    params.iFolderId = iMessage->GetFolderId();
    params.iMessageId = iMessage->GetMessageId();
    params.iEmbeddedMsgMode = iEmbeddedMessageMode;
    // use package buffer to pass the params
    TPckgBuf<TAttachmentListActivationData> buf( params );
    TUid emptyCustomMessageId = { 0 };
    iAppUi.EnterFsEmailViewL(
        AttachmentMngrViewId, emptyCustomMessageId, buf );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::StartDowloadingAttachmentsL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::StartDowloadingAttachmentsL()
    {
    FUNC_LOG;
    TPartData partData = MailData();

    RPointerArray<CFSMailMessagePart> attachments;
    CleanupResetAndDestroyClosePushL( attachments );
    iMessage->AttachmentListL( attachments );
    TInt attachmentsCount = attachments.Count();
    for( TInt i = 0; i < attachmentsCount; i++)
        {
        if ( attachments[i]->FetchLoadState() == EFSNone ||
             attachments[i]->FetchLoadState() == EFSPartial )
            {
            partData.iMessagePartId = attachments[i]->GetPartId();
            if ( iAppUi.DownloadInfoMediator() )
                {
                iAppUi.DownloadInfoMediator()->DownloadL( partData );
                }
            }
        }
    CleanupStack::PopAndDestroy( &attachments );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CancelDowloadingAttachmentsL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::CancelDowloadingAttachmentsL()
    {
    FUNC_LOG;
    RPointerArray<CFSMailMessagePart> attachments;
    CleanupResetAndDestroyClosePushL( attachments );
    iMessage->AttachmentListL( attachments );
    TInt attachmentsCount = attachments.Count();

    // ask confirmation for cancel
    TInt promptTextId = R_FSE_VIEWER_CANCEL_DOWNLOAD_QUERY;
    if ( attachmentsCount > 1 )
        {
        promptTextId = R_FSE_VIEWER_CANCEL_DOWNLOADS_QUERY;
        }

    if ( TFsEmailUiUtility::ShowConfirmationQueryL(promptTextId) && iAppUi.DownloadInfoMediator() )
        {
        for ( TInt i = 0 ; i < attachmentsCount ; i++ )
            {
            TFSPartFetchState fetchState = attachments[i]->FetchLoadState();
            if ( fetchState == EFSPartial || fetchState == EFSNone )
                {
                iAppUi.DownloadInfoMediator()->CancelDownloadL( attachments[i]->GetPartId() );
                }
            }
        }
    CleanupStack::PopAndDestroy( &attachments );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::SaveAllAttachmentsL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::SaveAllAttachmentsL()
    {
    FUNC_LOG;
    TFileName fileName;
    if ( TFsEmailUiUtility::ShowSaveFolderDialogL( fileName ) )
        {
        // start downloading attachemts which are not downloaded.
        // Save all attachment which all already downloaded
        TPartData partData = MailData();

        RPointerArray<CFSMailMessagePart> attachments;
        CleanupResetAndDestroyClosePushL( attachments );
        iMessage->AttachmentListL( attachments );
        TInt attachmentsCount = attachments.Count();

        // <cmail> remove the MRUI part of the message so we
        // don't save unnecessary .ics file
        CFSMailMessagePart* calendarPart = iMessage->FindBodyPartL( KFSMailContentTypeTextCalendar );
        CleanupStack::PushL( calendarPart );
        // </cmail>

        TInt savedCount( 0 );
        for ( TInt i = 0; i < attachmentsCount; i++ )
            {
            partData.iMessagePartId = attachments[i]->GetPartId();
            if ( iAppUi.DownloadInfoMediator() && iAppUi.DownloadInfoMediator()->IsDownloadableL( partData ) )
                { // start download and let mediator do the saving
                iAppUi.DownloadInfoMediator()->DownloadAndSaveL( partData, fileName );
                }
            else // save immediately.
                {
                // <cmail>
                if( (calendarPart && partData.iMessagePartId != calendarPart->GetPartId() ||
                    !calendarPart) && TFsEmailUiUtility::OkToSaveFileL( fileName, *attachments[i] ) )
                    {
                    attachments[i]->CopyContentFileL( fileName );
                    savedCount++;
                    }
                // </cmail>
                }
            }
        if ( savedCount )
            {
            TFsEmailUiUtility::ShowFilesSavedToFolderNoteL( savedCount );
            }
        // <cmail>
        CleanupStack::PopAndDestroy( calendarPart );
        // </cmail>
        CleanupStack::PopAndDestroy( &attachments );
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::RemoveFetchedAttachmentL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::RemoveFetchedAttachmentL()
    {
    FUNC_LOG;
    TInt reallyClear =
        TFsEmailUiUtility::ShowConfirmationQueryL( R_FSE_VIEWER_CLEAR_FETCHED_QUERY );
    if ( reallyClear )
        {
        RPointerArray<CFSMailMessagePart> attachments;
        CleanupResetAndDestroyClosePushL( attachments );
        iMessage->AttachmentListL( attachments );
        TInt attachmentsCount = attachments.Count();
        for ( TInt i = 0; i < attachmentsCount; i++ )
            {
            attachments[i]->RemoveContentL();
            }
        CleanupStack::PopAndDestroy( &attachments );
        iShowMskDownloadOption = ShowMskDownloadOptionL();
        SetMskL();
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::MailData
// -----------------------------------------------------------------------------
TPartData CFSEmailUiMailViewerVisualiser::MailData()
    {
    FUNC_LOG;
    TPartData newPartData;
    newPartData.iMailBoxId = iMessage->GetMailBoxId();
    newPartData.iFolderId = iMessage->GetFolderId();
    newPartData.iMessageId = iMessage->GetMessageId();
    return newPartData;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::StartFetchingBodyAfterOpeningL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::StartFetchingBodyAfterOpeningL() const
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    CFSMailMessagePart* textPart = iMessage->PlainTextBodyPartL();
    if ( textPart )
        {
        TFSPartFetchState currentPlainTextFetchState = textPart->FetchLoadState();
        if ( currentPlainTextFetchState == EFSNone )
            {
            retVal = ETrue;
            }
        }
    delete textPart;
    return retVal;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::MessagePartFullyFetchedL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::MessagePartFullyFetchedL( TFetchedContentType aFetchedContentType ) const
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

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::StartFetchingMessagePartL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::StartFetchingMessagePartL( CFSMailMessage& aMessagePtr,
                                            TFetchedContentType aFetchedContentType )
    {
    FUNC_LOG;
    if( aFetchedContentType == EMessagePlainTextBodyPart )
        {
        CFSMailMessagePart* textPart = aMessagePtr.PlainTextBodyPartL();
        CleanupStack::PushL( textPart );
        TFSMailMsgId textPartId = textPart->GetPartId();
        iFetchingPlainTextMessageBody = ETrue;
        iCurrentPlainTextBodyFetchRequestId = textPart->FetchMessagePartL( textPartId, *this, 0 );
        CleanupStack::PopAndDestroy( textPart );
        }
    else if( aFetchedContentType == EMessageHtmlBodyPart )
        {
        CFSMailMessagePart* htmlPart = aMessagePtr.HtmlBodyPartL();
        CleanupStack::PushL( htmlPart );
        TFSMailMsgId htmlPartId = htmlPart->GetPartId();
        iFetchingHtmlMessageBody = ETrue;
        iCurrentHtmlBodyFetchRequestId = htmlPart->FetchMessagePartL( htmlPartId, *this, 0 );
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
// CFSEmailUiMailViewerVisualiser::CancelFetchings
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::CancelFetchings()
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
    //<cmail>
    if(iWaitDialog && iDialogNotDismissed)
        TRAP_IGNORE(iWaitDialog->ProcessFinishedL()); // deletes the dialog
    //</cmail>
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::MessageStructureKnownL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::MessageStructureKnown( CFSMailMessage& aMsg ) const
    {
    FUNC_LOG;
    return TFsEmailUiUtility::IsMessageStructureKnown( aMsg );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::StartFetchingMessageStructureL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::StartFetchingMessageStructureL( CFSMailMessage& aMsg )
    {
    FUNC_LOG;
    TFSMailMsgId currentMailboxId = aMsg.GetMailBoxId();
    TFSMailMsgId currentMessageFolderId = aMsg.GetFolderId();
    CFSMailFolder* currentFolder  = iAppUi.GetMailClient()->GetFolderByUidL( currentMailboxId, currentMessageFolderId );
    CleanupStack::PushL( currentFolder );
    RArray<TFSMailMsgId> messageIds;
    CleanupClosePushL( messageIds );
    messageIds.Append( aMsg.GetMessageId() );
    iFetchingMessageStructure = ETrue;
    iCurrentStructureFetchRequestId = currentFolder->FetchMessagesL( messageIds, EFSMsgDataStructure, *this );
    CleanupStack::PopAndDestroy( &messageIds );
    CleanupStack::PopAndDestroy( currentFolder );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::StartWaitedFetchingL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::StartWaitedFetchingL( TFetchedContentType aFetchedContentType )
    {
    FUNC_LOG;
    iAsyncProcessComplete = EFalse;
    iFetchingAlready = EFalse;
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
    iWaitDialog = new(ELeave)CAknWaitDialog(
                   (REINTERPRET_CAST(CEikDialog**,&iWaitDialog)), ETrue);
    iWaitDialog->SetCallback(this);
    iDialogNotDismissed = ETrue;
    iWaitDialog->ExecuteLD(R_FSE_FETCHING_WAIT_DIALOG);
        StartFetchingMessagePartL( *iMessage, iStartAsyncFetchType );
        iFetchingAlready = ETrue;
    //</cmail>
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::StartFetchingRemaininBodyLinesIfAtBottomL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::StartFetchingRemaininBodyLinesIfAtBottomL()
    {
    FUNC_LOG;
    TInt totalLines = iTextViewer->GetTotalLines();
    TInt currentLastDisplayedLine = iTextViewer->GetLastDisplayedLine();

    if ( totalLines == currentLastDisplayedLine )
        {
        // check if there is something to be fetched
        if ( !MessagePartFullyFetchedL( EMessagePlainTextBodyPart ) && !iFetchingPlainTextMessageBody )
            {
            iViewerRichText->AppendFetchingMoreTextL();
            // ensure the status layout in the bottom of the screen is visible
            iTextViewer->FocusLineL( iTextViewer->GetTotalLines() );
            // start fetching
            StartFetchingMessagePartL( *iMessage, EMessagePlainTextBodyPart );
            iFetchingAnimationTimer->Start( KAnimationRefreshTimeIntervalInMilliseconds );
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::TimerEventL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer )
    {
    FUNC_LOG;
    if ( aTriggeredTimer == iFetchingAnimationTimer )
        {
        iViewerRichText->AppendFetchingMoreTextL();
        iFetchingAnimationTimer->Start( KAnimationRefreshTimeIntervalInMilliseconds );
        }
    }

// MAknBackgroundProcess methods
//
// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::StepL
// -----------------------------------------------------------------------------
//<cmail>
/*
void CFSEmailUiMailViewerVisualiser::StepL()
    {
    FUNC_LOG;
    if( !iFetchingAlready )
        {
        StartFetchingMessagePartL( *iMessage, iStartAsyncFetchType );
        iFetchingAlready = ETrue;
        }
    }*/
//</cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::IsProcessDone
// -----------------------------------------------------------------------------
//<cmail>
/*
TBool CFSEmailUiMailViewerVisualiser::IsProcessDone() const
    {
    FUNC_LOG;
    return iAsyncProcessComplete;
    }*/
//</cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ProcessFinished
// -----------------------------------------------------------------------------
//<cmail>
//void CFSEmailUiMailViewerVisualiser::ProcessFinished() {/* nothing here */ }
//</cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::DialogDismissedL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::DialogDismissedL( TInt aButtonId )
    {
    FUNC_LOG;
    iDialogNotDismissed = EFalse;
    if( aButtonId == EAknSoftkeyCancel )
        {
        CancelFetchings();
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CycleError
// -----------------------------------------------------------------------------
//<cmail>
//TInt CFSEmailUiMailViewerVisualiser::CycleError( TInt /*aError*/ )
/*
    {
    FUNC_LOG;
    iAsyncProcessComplete = ETrue;
    CancelFetchings();
    return KErrNone;
    }*/
//</cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::AddBackgroundPicturesL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::AddBackgroundPicturesL()
    {
    CAlfTexture& headerTexture = iAppUi.FsTextureManager()->TextureByIndex( EViewerTextureHeaderBackGround );
    CAlfTexture& backgroundTexture = iAppUi.FsTextureManager()->TextureByIndex( EBackgroundTextureMailList );

    // add header texture
    // -1 is here for to bottom space icon
    // // <cmail> S60 Skin support
    //  iTextViewer->SetPartBgImageL( 0, iViewerRichText->HeaderLengthInCharacters()-2, headerTexture );
    // </cmail>

    // add bg texture
    //<cmail> S60 skin support
    //iTextViewer->SetBackgroundImageL( backgroundTexture );
    //</cmail>
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::SetMessageFollowupFlagL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::SetMessageFollowupFlagL()
    {
    FUNC_LOG;
    if ( iMessage &&
         iTextViewer &&
         iViewerRichText &&
         TFsEmailUiUtility::IsFollowUpSupported( *iMailBox ) &&
         !iEmbeddedMessageMode )
        {
        TFollowUpNewState newState = TFsEmailUiUtility::SetMessageFollowupFlagL( *iMessage );

        switch ( newState )
            {
            case EFollowUp:
                {
                iViewerRichText->UpdateIconL( CFSEmailUiMailViewerRichText::EViewerIconFollowUp );
                }
                break;
            case EFollowUpComplete:
                {
                iViewerRichText->UpdateIconL( CFSEmailUiMailViewerRichText::EViewerIconFollowUpComplete );
                }
                break;
            case EFollowUpClear:
                {
                iViewerRichText->UpdateIconL( CFSEmailUiMailViewerRichText::EViewerIconFollowUpNone );
                }
                break;
            case EFollowUpNoChanges:
            default:
                {
                // do nothing.
                }
                break;
            }

        if ( newState != EFollowUpNoChanges )
            {
            // Update icon in viewer
            TInt iconLine = iViewerRichText->FollowupIconLine();
            iTextViewer->ReloadPicturesL( iconLine );
            // Notify appui of changed mail item
            SendEventToAppUiL( TFSEventMailChanged );
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::DeleteMailL
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerVisualiser::DeleteMailL( CFSMailMessage& aMessagePtr, TBool aReturnPreviousView, TBool aDisableNote )
    {
    FUNC_LOG;
    TInt ret( KErrCancel );

    if ( iEmbeddedMessageMode )
        {
        // embedded messages can't be deleted
        ret = KErrNotSupported;
        }
    else
        {
        TInt okToDelete( ETrue );
        if ( !aDisableNote )
            {
            if(iAppUi.GetCRHandler()->WarnBeforeDelete())
                {
                okToDelete = TFsEmailUiUtility::ShowConfirmationQueryL( R_FREESTYLE_EMAIL_UI_DELETE_MESSAGE_CONFIRMATION );
                }
            }
        if ( okToDelete )
            {
            CFSMailMessage* confirmedMsgPtr = NULL;
            TRAP_IGNORE( confirmedMsgPtr = iAppUi.GetMailClient()->GetMessageByUidL( iAppUi.GetActiveMailboxId(),
                                                                                     aMessagePtr.GetFolderId(),
                                                                                     aMessagePtr.GetMessageId() ,
                                                                                     EFSMsgDataEnvelope ) );
            if ( confirmedMsgPtr )
                {
                CleanupStack::PushL( confirmedMsgPtr );
                RArray<TFSMailMsgId> msgIds;
                CleanupClosePushL( msgIds );
                msgIds.Append( confirmedMsgPtr->GetMessageId() );
                TFSMailMsgId mailBox = iAppUi.GetActiveMailbox()->GetId();
                iAppUi.GetMailClient()->DeleteMessagesByUidL( mailBox, confirmedMsgPtr->GetFolderId(), msgIds );
                // Notify appui of deleted mail item
                SendEventToAppUiL( TFSEventMailDeleted );
                CleanupStack::PopAndDestroy( &msgIds );
                CleanupStack::PopAndDestroy( confirmedMsgPtr );
                }

            // return to previous view
            if ( aReturnPreviousView )
                {
                // return only if we are in mail viewer
                if ( iAppUi.CurrentActiveView()->Id() == MailViewerId )
                    {
                    HandleCommandL( EAknSoftkeyBack );
                    }
                }
            }
        if ( okToDelete )
            {
            ret = KErrNone;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowActionsMenuInOptionsL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::ShowActionsMenuInOptionsL() const
    {
    FUNC_LOG;
    TBool retVal = ETrue;

    SViewerHeadingHotSpotData currentHeaderHotSpotData;
    CFindItemEngine::SFoundItem currentBodyHotSpotData;
    THotspotType hotspotType = iViewerRichText->FindCurrentHotSpotL( currentHeaderHotSpotData, currentBodyHotSpotData );

    if( hotspotType == ENoHotspot || ( hotspotType == EHeaderHotspot && (
        currentHeaderHotSpotData.iType == ETypeToNMoreRecipients ||
        currentHeaderHotSpotData.iType == ETypeCcNMoreRecipients ||
        currentHeaderHotSpotData.iType == ETypeBccNMoreRecipients ||
        currentHeaderHotSpotData.iType == ETypeHtml ) ) )
        {
        retVal = EFalse;
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowDownloadManagerMenuInOptions
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::ShowDownloadManagerMenuInOptions() const
    {
    FUNC_LOG;
    TBool ret(EFalse);
/* <cmail> Download Manager blocked
    if ( iAppUi.DownloadInfoMediator() )
        {
        ret = iAppUi.DownloadInfoMediator()->IsAnyAttachmentDownloads();
        }
</cmail> */
    return ret;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowNextMessageMenuInOptions
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::ShowNextMessageMenuInOptions() const
    {
    FUNC_LOG;
    TBool available = EFalse;
    // Next/previous message options are inavailable in the embedded mode
    if ( iMessage && !iEmbeddedMessageMode )
        {
        TFSMailMsgId currentMsgId = iMessage->GetMessageId();
        TFSMailMsgId nextMsgId;
        TFSMailMsgId nextMsgFolderId;
        available = iAppUi.IsNextMsgAvailable( currentMsgId, nextMsgId, nextMsgFolderId );
        }
    return available;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowPreviousMessageMenuInOptions
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerVisualiser::ShowPreviousMessageMenuInOptions() const
    {
    FUNC_LOG;
    TBool available = EFalse;
    // Next/previous message options are inavailable in the embedded mode
    if ( iMessage && !iEmbeddedMessageMode )
        {
        TFSMailMsgId currentMsgId = iMessage->GetMessageId();
        TFSMailMsgId prevMsgId;
        TFSMailMsgId prevMsgFolderId;
        available = iAppUi.IsPreviousMsgAvailable( currentMsgId, prevMsgId, prevMsgFolderId );
        }
    return available;
    }


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowNextMessageL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::ShowNextMessageL()
    {
    FUNC_LOG;
    if ( iMessage && !iEmbeddedMessageMode )
        {
        TFSMailMsgId currentMsgId = iMessage->GetMessageId();
        TFSMailMsgId nextMsgId;
        TFSMailMsgId nextMsgFolderId;
        if ( iAppUi.IsNextMsgAvailable( currentMsgId, nextMsgId, nextMsgFolderId ) )
            {
            // Stop timer and cancel fetchings before showing next message
            iFetchingAnimationTimer->Stop();
            CancelFetchings();

            // Change empty msk when moving to next
            ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );

            iAppUi.MoveToNextMsgL( currentMsgId, nextMsgId );
            // Next message is displayed in this view through doactivate, because view is re-activate by mail list
            }
        }
    }
// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ShowPreviousMessageL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::ShowPreviousMessageL()
    {
    FUNC_LOG;
    if ( iMessage && !iEmbeddedMessageMode )
        {
        TFSMailMsgId currentMsgId = iMessage->GetMessageId();
        TFSMailMsgId prevMsgId;
        TFSMailMsgId prevMsgFolderId;
        if ( iAppUi.IsPreviousMsgAvailable( currentMsgId, prevMsgId, prevMsgFolderId ) )
            {
            // Stop timer and cancel fetchings before showing prev message
            iFetchingAnimationTimer->Stop();
            CancelFetchings();

            // Change empty msk when moving to previous
            ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );

            iAppUi.MoveToPreviousMsgL( currentMsgId, prevMsgId );
            // Previous message is displayed in this view through doactivate, because view is re-activate by mail list
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::UpdateMessagePtrL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::UpdateMessagePtrL( TFSMailMsgId aNewMailboxId,
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

        UpdateMessagePtr( iMessage );
        // Update the open messages stack.
        EraseMessageStack(); // takes care of deallocating any previous messages
        PushMessageL( iMessage, EFalse ); // this is non-embedded message
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
// CFSEmailUiMailViewerVisualiser::UpdateMessagePtr
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::UpdateMessagePtr( CFSMailMessage* aNewMessagePtr )
    {
    ASSERT( aNewMessagePtr );

    TBool messageChanged = ETrue;

    // Stop observing any previous message and update iMessage pointer
    if ( iMessage )
        {
        // Check is the message going to be changed or are we just updating the
        // same message object which has been previously shown.
        messageChanged = ( iMessage->GetMessageId() != aNewMessagePtr->GetMessageId() );

        // stop observing downloads from the previous message in case the message was changed
        if ( iAppUi.DownloadInfoMediator() && messageChanged )
            {
            iAppUi.DownloadInfoMediator()->StopObserving( this, iMessage->GetMessageId() );
            }
        }
    iMessage = aNewMessagePtr;

    // Start observing attachment downloads from the new message
    if ( iAppUi.DownloadInfoMediator() && messageChanged )
        {
        iAppUi.DownloadInfoMediator()->AddObserver( this, iMessage->GetMessageId() );
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::PushMessageL
// Message stack handling. Ownership of message is transferred when succesful.
// -----------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::PushMessageL( CFSMailMessage* aMessage, TBool aIsEmbedded )
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
// CFSEmailUiMailViewerVisualiser::PopMessage
// Message stack handling. Ownership of message is returned.
// -----------------------------------------------------------------------------
//
CFSMailMessage* CFSEmailUiMailViewerVisualiser::PopMessage()
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
// CFSEmailUiMailViewerVisualiser::EraseMessageStack
// Message stack handling. All messages in stack are deallocated
// -----------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::EraseMessageStack()
    {
    ASSERT( iOpenMessages && iEmbeddedMessages );
    ASSERT( iOpenMessages->Count() == iEmbeddedMessages->Count() );

    iOpenMessages->ResetAndDestroy();
    iEmbeddedMessages->Reset();
    }
// CFSEmailUiMailViewerVisualiser::NavigateBackL
// Overriden from base class
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::NavigateBackL()
    {
    FUNC_LOG;
    // clean up current message
    if ( iMessage && iAppUi.DownloadInfoMediator() )
        {
        iAppUi.DownloadInfoMediator()->StopObserving( this, iMessage->GetMessageId() );
        }
    delete iMessage;
    iMessage = NULL;
    PopMessage();
    // Return to previous message in the stack if there is still some left
    if ( !iOpenMessages->IsEmpty() )
        {
        iAppUi.EnterFsEmailViewL( MailViewerId, KStartViewerReturnFromEmbeddedMsg, KNullDesC8 );
        }
    // In usual case we use the base view implementation
    else
        {
        CFsEmailUiViewBase::NavigateBackL();
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::HandleDynamicVariantSwitchL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::HandleDynamicVariantSwitchL( CFsEmailUiViewBase::TDynamicSwitchType /*aType*/ )
    {
    FUNC_LOG;
    // Set scroll offset to match screen height
    if ( iFirstStartCompleted && iTextViewer && iMessage )
        {
        // <cmail>
        TInt firstLine = iTextViewer->GetFirstDisplayedLine();        // get first line number
        TInt firstChar = iTextViewer->GetFirstCharInLine(firstLine);  // get first char offset
        // </cmail>

        iAppUi.Display().Roster().Hide( ControlGroup() );
        RefreshL();

        // <cmail>
        TInt newLine = iTextViewer->GetLineNumber(firstChar);   // get the new line number for this offset
        if(newLine<0) newLine = firstLine;                       // in case of error go to the prev. line number
        iTextViewer->FocusLineL(newLine,ETrue);                  // focus on the found line number
        // </cmail>
        // <cmail>
        SetActionMenuIconVisbilityL();
            // </cmail>
            iAppUi.Display().Roster().ShowL( ControlGroup() );
        iDownloadProgressIndicator->NotifyLayoutChange();
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::HandleDynamicVariantSwitchOnBackgroundL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::HandleDynamicVariantSwitchOnBackgroundL( CFsEmailUiViewBase::TDynamicSwitchType /*aType*/ )
    {
    FUNC_LOG;
    iLayoutChangedWhileNotActive = ETrue;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::OpenLinkInIntranetL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::OpenLinkInIntranetL( TBool aMenuSelection )
    {
    FUNC_LOG;
    CFindItemEngine::SFoundItem currentBodyHotSpotData;
    TBool bodyHotSpotFound = iViewerRichText->FindCurrentBodyHotSpotL( currentBodyHotSpotData );
    if ( bodyHotSpotFound )
        {
        HBufC* intranetUrl = iViewerRichText->GetHotspotTextLC( currentBodyHotSpotData );
        TInt len = intranetUrl->Length();
        HBufC8* eightBitUrl = HBufC8::NewLC( len );
        eightBitUrl->Des().Copy( *intranetUrl );

        TAiwGenericParam param( EGenericParamURL, TAiwVariant(*eightBitUrl) );
        CAiwGenericParamList& paramList = iIBServiceHandler->InParamListL();
        paramList.AppendL( param );
        if ( aMenuSelection )
            {
            iIBServiceHandler->ExecuteMenuCmdL( EFsEmailUiCmdActionsOpenInIntranetMenu,
                paramList, iIBServiceHandler->OutParamListL() );
            }
        else
            {
            iIBServiceHandler->ExecuteServiceCmdL( KAiwCmdView,
                paramList, iIBServiceHandler->OutParamListL() );
            }
        CleanupStack::PopAndDestroy( eightBitUrl );
        CleanupStack::PopAndDestroy( intranetUrl );
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::ChangeMsgReadStatusL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::ChangeMsgReadStatusL(
    TBool aRead, TBool aCmdFromMrui )
    {
    FUNC_LOG;
    if ( iMessage && !iEmbeddedMessageMode )
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

        if ( !aCmdFromMrui && iViewerRichText && iTextViewer )
            {
            iViewerRichText->UpdateIconL(
                CFSEmailUiMailViewerRichText::EViewerEmailStatus );
            TInt iconLine = iViewerRichText->EmailStatusIconLine();
            iTextViewer->ReloadPicturesL( iconLine );
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::OpenHotSpotUrlInBrowserL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::OpenHotSpotUrlInBrowserL(  CFindItemEngine::SFoundItem& aHotSpot  )
    {
    FUNC_LOG;
    if ( aHotSpot.iItemType == CFindItemEngine::EFindItemSearchURLBin ||
         aHotSpot.iItemType == CFindItemEngine::EFindItemSearchScheme ) // Scheme is needed also
        {
        // use browser launcher API here
        HBufC* urlToOpen = iViewerRichText->GetHotspotTextLC( aHotSpot );
        // Check if the address is rtsp url and send it media player
        if ( aHotSpot.iItemType == CFindItemEngine::EFindItemSearchScheme &&
             urlToOpen->FindC( KRtspUrlPrefix ) == 0 )
            {
            RApaLsSession appArcSession;
            User::LeaveIfError( appArcSession.Connect() );
            TThreadId id;
            appArcSession.StartDocument( urlToOpen->Des(), KUidMediaPlayer , id );
            appArcSession.Close();
            }
        else
            {
            // Check wheter the url text has some prefix (http, https, ftp, etc.).
            // If no prefix found, try with the default prefix http.
            TInt urlPos = urlToOpen->Find( KUrlPrefixIdentifier );
            if ( urlPos == KErrNotFound )
                {
                HBufC* newBuf = urlToOpen->ReAllocL( urlToOpen->Length() + KHttpUrlPrefix().Length() );
                CleanupStack::Pop( urlToOpen );
                urlToOpen = newBuf;
                CleanupStack::PushL( urlToOpen );
                TPtr urlToOpenPtr = urlToOpen->Des();
                urlToOpenPtr.Insert( 0, KHttpUrlPrefix );
                }

            // Use scheme handler to launch the browser as a stand alone application
            CSchemeHandler* handler = CSchemeHandler::NewL( *urlToOpen );
            CleanupStack::PushL( handler );
            handler->HandleUrlStandaloneL();
            CleanupStack::PopAndDestroy( handler );
            }

        CleanupStack::PopAndDestroy( urlToOpen );
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::CallToBodyHotSpotNumberL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::CallToBodyHotSpotNumberL( CFindItemEngine::SFoundItem& aHotSpot )
    {
    FUNC_LOG;

    if ( aHotSpot.iItemType == CFindItemEngine::EFindItemSearchPhoneNumberBin )
        {
        HBufC* phoneNumber = iViewerRichText->GetHotspotTextLC( aHotSpot );
        TInt answer = TFsEmailUiUtility::ShowConfirmationQueryL(
                R_FREESTYLE_EMAIL_UI_VIEWER_CALL_HOTSPOT, *phoneNumber );
        if ( answer )
            {
            TPtr numberPtr = phoneNumber->Des();
            CommonPhoneParser::ParsePhoneNumber( numberPtr,
                                  CommonPhoneParser::EPhoneClientNumber);

            CFsDelayedLoader::InstanceL()->GetContactHandlerL()->MakeAiwCallL( NULL, *phoneNumber );
            }
        CleanupStack::PopAndDestroy( phoneNumber );
        }
    else if ( aHotSpot.iItemType == CFindItemEngine::EFindItemSearchScheme )
        {
        TBool voipCall( EFalse );
        HBufC* schemeText = iViewerRichText->GetHotspotTextLC( aHotSpot );
        // Strip scheme prefix text if found
        if ( schemeText && schemeText->FindC( KCallPrefix ) == 0 )
            {
            schemeText->Des().Delete( 0, KCallPrefix().Length() );
            }
        else if ( schemeText && schemeText->FindC( KTelPrefix ) == 0 )
            {
            schemeText->Des().Delete( 0, KTelPrefix().Length() );
            }
        else if ( schemeText && schemeText->FindC( KSmsPrefix ) == 0 )
            {
            schemeText->Des().Delete( 0, KSmsPrefix().Length() );
            }
        else if ( schemeText && schemeText->FindC( KMmsPrefix ) == 0 )
            {
            schemeText->Des().Delete( 0, KMmsPrefix().Length() );
            }
        else if ( schemeText && schemeText->FindC( KVoipPrefix ) == 0 )
            {
            voipCall = ETrue;
            schemeText->Des().Delete( 0, KVoipPrefix().Length() );
            }

        TInt answer = TFsEmailUiUtility::ShowConfirmationQueryL(
                R_FREESTYLE_EMAIL_UI_VIEWER_CALL_HOTSPOT, *schemeText );
        if ( answer )
            {
            if ( !voipCall )
                {
                // use common phone parser for other than voip numbers
                TPtr numberPtr = schemeText->Des();
                CommonPhoneParser::ParsePhoneNumber( numberPtr,
                                 CommonPhoneParser::EPhoneClientNumber );
                }
            CFsDelayedLoader::InstanceL()->GetContactHandlerL()->MakeAiwCallL( NULL, *schemeText, voipCall );
            }
        CleanupStack::PopAndDestroy( schemeText );
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerVisualiser::WriteEmailToHotSpotAddressL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerVisualiser::WriteEmailToHotSpotAddressL( CFindItemEngine::SFoundItem& aHotSpot )
    {
    FUNC_LOG;
    if ( aHotSpot.iItemType == CFindItemEngine::EFindItemSearchMailAddressBin )
        {
        delete iNewMailTempAddress; iNewMailTempAddress = NULL;
        iNewMailTempAddress = CFSMailAddress::NewL();
        HBufC* mailAddress = iViewerRichText->GetHotspotTextLC( aHotSpot );
        iNewMailTempAddress->SetEmailAddress( *mailAddress );
        CleanupStack::PopAndDestroy( mailAddress );
        iAppUi.LaunchEditorL( iNewMailTempAddress );
        }
    else if ( aHotSpot.iItemType == CFindItemEngine::EFindItemSearchScheme )
        {
        HBufC* schemeText = iViewerRichText->GetHotspotTextLC( aHotSpot );
        if ( schemeText && schemeText->FindC( KMailtoPrefix ) == 0)
            {
            schemeText->Des().Delete( 0, KMailtoPrefix().Length() );
            delete iNewMailTempAddress; iNewMailTempAddress = NULL;
            iNewMailTempAddress = CFSMailAddress::NewL();
            iNewMailTempAddress->SetEmailAddress( *schemeText );
            iAppUi.LaunchEditorL( iNewMailTempAddress );
            }
        CleanupStack::PopAndDestroy( schemeText );
        }
    }

void CFSEmailUiMailViewerVisualiser::SaveEmailAsContactL(
    const TDesC& aEmailAddress )
    {
    FUNC_LOG;
    TAddToContactsType type;
    // Query to "update existing" or "Create new"
    // --> EFALSE = user chose "cancel"
    if ( CFsDelayedLoader::InstanceL()->GetContactHandlerL()->
        AddtoContactsQueryL( type ) )
        {
        // Create buffer and strip scheme data such as mailto: and call:
        HBufC* textData = aEmailAddress.AllocLC();
        if ( textData->FindC( KMailtoPrefix ) == 0 )
            {
            textData->Des().Delete( 0, KMailtoPrefix().Length() );
            }
        CFsDelayedLoader::InstanceL()->GetContactHandlerL()->AddToContactL(
                *textData, EContactUpdateEmail, type, this );
        CleanupStack::PopAndDestroy( textData );
        }
    }

void CFSEmailUiMailViewerVisualiser::SavePhoneNumberAsContactL(
    const TDesC& aPhoneNumber )
    {
    FUNC_LOG;
    TAddToContactsType type;
    // Query to "update existing" or "Create new"
    // --> EFALSE = user chose "cancel"
    if ( CFsDelayedLoader::InstanceL()->GetContactHandlerL()->
        AddtoContactsQueryL( type ) )
        {
        // Create buffer and strip scheme data such as mailto: and call:
        HBufC* textData = aPhoneNumber.AllocLC();
        // Strip scheme prefix text if found
        if ( textData && textData->FindC( KCallPrefix ) == 0 )
            {
            textData->Des().Delete( 0, KCallPrefix().Length() );
            }
        else if ( textData && textData->FindC( KTelPrefix ) == 0 )
            {
            textData->Des().Delete( 0, KTelPrefix().Length() );
            }
        else if ( textData && textData->FindC( KSmsPrefix ) == 0 )
            {
            textData->Des().Delete( 0, KSmsPrefix().Length() );
            }
        else if ( textData && textData->FindC( KMmsPrefix ) == 0 )
            {
            textData->Des().Delete( 0, KMmsPrefix().Length() );
            }
        CFsDelayedLoader::InstanceL()->GetContactHandlerL()->AddToContactL(
            *textData, EContactUpdateNumber, type, this );
        CleanupStack::PopAndDestroy( textData );
        }
    }


void CFSEmailUiMailViewerVisualiser::FlagselectionCompleteL( TInt aSelection )
    {
    FUNC_LOG;
    // Set message follow up flag
    if ( iMessage && aSelection != KErrNotFound )
        {
        switch ( aSelection )
            {
            case EFollowUp:
                {
                iMessage->SetFlag( EFSMsgFlag_FollowUp );
                iMessage->ResetFlag( EFSMsgFlag_FollowUpComplete );
                iMessage->SaveMessageL();
                }
                break;
            case EFollowUpComplete:
                {
                iMessage->SetFlag( EFSMsgFlag_FollowUpComplete );
                iMessage->ResetFlag( EFSMsgFlag_FollowUp );
                iMessage->SaveMessageL();
                }
                break;
            case EFollowUpClear:
                {
                iMessage->ResetFlag( EFSMsgFlag_FollowUp | EFSMsgFlag_FollowUpComplete );
                iMessage->SaveMessageL();
                }
                break;
            default:
                break;
            }
        }
    }


void CFSEmailUiMailViewerVisualiser::SetMailboxNameToStatusPaneL()
    {
    FUNC_LOG;
    iAppUi.SetActiveMailboxNameToStatusPaneL();
    }

void CFSEmailUiMailViewerVisualiser::GetParentLayoutsL( RPointerArray<CAlfVisual>& aLayoutArray ) const
    {
    if ( iTextViewer )
        {
        aLayoutArray.AppendL( iTextViewer->RootLayout() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::UpdateNaviPaneL( TBool aForESMR )
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
        
        indicators->DrawNow();
        }

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::HandleNaviDecoratorEventL( TInt aEventID )
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

// ---------------------------------------------------------------------------
// HandleMailBoxEventL
// Mailbox event handler, responds to events sent by the plugin.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMailViewerVisualiser::HandleMailBoxEventL( TFSMailEvent aEvent,
    TFSMailMsgId aMailbox, TAny* aParam1, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && iMessage && aMailbox.Id() == iAppUi.GetActiveMailboxId().Id() &&
         aEvent == TFSEventMailDeleted && aParam1 ) // Safety, in list events that only concern active mailbox are handled
        {
        TFSMailMsgId curMsgId = iMessage->GetMessageId();
        RArray<TFSMailMsgId>* removedEntries = static_cast<RArray<TFSMailMsgId>*>(aParam1);
        for ( TInt i=0 ; i < removedEntries->Count() ; i++ )
            {
            //<cmail>
            if ( (curMsgId == (*removedEntries)[i]) &&
                    (iDeletedMessageFromMrui != curMsgId) )
                {
                //</cmail>
                ChangeMskCommandL( R_FSE_QTN_MSK_EMPTY );
                NavigateBackL();
                break;
                }
            }
        }
    }

void CFSEmailUiMailViewerVisualiser::SendEventToAppUiL( TFSMailEvent aEventType )
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

//////////////////////////////////////////////////////////////////////////////////////
// CLASS IMPLEMENTATION CHANGE TO FLAG DIALOG GLOBAL NOTE
///////////////////////////////////////////////////////////////////////////////////
CFlagSelectionGlobalNoteHandler* CFlagSelectionGlobalNoteHandler::NewL( CFSEmailUiMailViewerVisualiser& aViewerVisualiser )
    {
    FUNC_LOG;
    CFlagSelectionGlobalNoteHandler* self =
         new (ELeave) CFlagSelectionGlobalNoteHandler( aViewerVisualiser );
    return self;
    }


CFlagSelectionGlobalNoteHandler::~CFlagSelectionGlobalNoteHandler()
    {
    FUNC_LOG;
    delete iPrompt;
    delete iGlobalFlagQuery;
    }

void CFlagSelectionGlobalNoteHandler::Cancel()
    {
    FUNC_LOG;
    }

void CFlagSelectionGlobalNoteHandler::LaunchFlagListQueryDialogL()
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

void CFlagSelectionGlobalNoteHandler::RunL()
    {
    FUNC_LOG;
    TInt status = iStatus.Int();
    iViewerVisualiser.FlagselectionCompleteL( status );
    }

void CFlagSelectionGlobalNoteHandler::DoCancel()
    {
    FUNC_LOG;
    }

TInt CFlagSelectionGlobalNoteHandler::RunError( TInt /*aError*/ )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    return err;
    }

CFlagSelectionGlobalNoteHandler::CFlagSelectionGlobalNoteHandler(  CFSEmailUiMailViewerVisualiser& aViewerVisualiser )
    : CActive ( EPriorityHigh ),
    iViewerVisualiser( aViewerVisualiser )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    iSelection = 0;
    }

