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
*  Description : Compose view class
*
*/

// INCLUDE FILES
#include "emailtrace.h"
#include <aknnotewrappers.h>
#include <NpdApi.h>
#include <StringLoader.h>
#include <txtrich.h>
#include <AknWaitDialog.h>
#include <MsgAttachmentUtils.h>
#include <featmgr.h>
#include "cfsmailbox.h"
#include "cfsmailmessage.h"
#include "cfsmailaddress.h"
#include "cfsmailclient.h"
#include "cfsmailcommon.h"
#include <csxhelp/cmail.hlp.hrh>
#include <akntoolbar.h>
#include <akntoolbarextension.h>
#include <AknUtils.h>
#include <FreestyleEmailUi.rsg>
#include <freestyleemailui.mbg>
#include <aknstyluspopupmenu.h>

#include "ncscomposeview.h"
#include "ncscomposeviewcontainer.h"
#include "ncsconstants.h"
#include "ncsutility.h"
#include "ncsemailaddressobject.h"
#include "freestyleemailcenrephandler.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiSendAttachmentsListControl.h"
#include "FreestyleEmailUiSendAttachmentsListVisualiser.h"
#include "FreestyleEmailUiSendAttachmentsListModel.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiConstants.h"
#include "FSAutoSaver.h"
#include "FSEmail.pan"


// line separators that should not appear in Subject
_LIT(KLineSeparators, "\u2029");
const TChar KReplacementChar = ' ';


#pragma mark +++COMPOSE VIEW - CONSTRUCTORS AND DESTRUCTORS

// -----------------------------------------------------------------------------
// Safe pointer deletion
//
// -----------------------------------------------------------------------------
//
template <class T> void SafeDelete( T*& aPtr )
    {
    delete aPtr;
    aPtr = NULL;
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::CNcsComposeView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CNcsComposeView::CNcsComposeView( CFreestyleEmailUiAppUi& aAppUi,
    CAlfEnv& aEnv, CAlfControlGroup& aSendAttachmentControlGroup,
    CFSMailClient& aMailClient, CMsvSession& aMsvSession )
    : CFsEmailUiViewBase( aSendAttachmentControlGroup, aAppUi ),
     iMailClient( aMailClient ), iOrigMessage( NULL ), iNewMessage( NULL ),
     iMsvSession( aMsvSession ), iEnv( aEnv ),
     iFakeSyncGoingOn(EFalse), iFetchDialogCancelled(EFalse),
     iExecutingDoExitL( EFalse )
    {
    FUNC_LOG;

    iAttachmentListSaveDraft = EFalse;

    }

// -----------------------------------------------------------------------------
// CNcsComposeView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNcsComposeView* CNcsComposeView::NewL( 
        CFSMailClient& aMailClient, CAlfEnv& aEnv,
        CFreestyleEmailUiAppUi* aAppUi, CAlfControlGroup& aControlGroup,
        CMsvSession& aMsvSession )
    {
    FUNC_LOG;

    CNcsComposeView* self = 
        CNcsComposeView::NewLC( aMailClient, aEnv, aAppUi,
                                aControlGroup, aMsvSession );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNcsComposeView* CNcsComposeView::NewLC(
    CFSMailClient& aMailClient,
    CAlfEnv& aEnv,
    CFreestyleEmailUiAppUi* aAppUi,
    CAlfControlGroup& aControlGroup,
    CMsvSession& aMsvSession )
    {
    FUNC_LOG;

    CNcsComposeView* self = new ( ELeave ) CNcsComposeView( 
            *aAppUi, aEnv, aControlGroup, aMailClient, aMsvSession );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNcsComposeView::ConstructL()
    {
    FUNC_LOG;

    BaseConstructL( R_NCS_COMPOSE_VIEW );
    iFirstStartCompleted = EFalse;
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::DoFirstStartL()
// Purpose of this function is to do first start only when msg details is
// really needed to be shown. Implemented to make app startuo faster.
// -----------------------------------------------------------------------------
//
void CNcsComposeView::DoFirstStartL()
    {
    FUNC_LOG;
    // Attachment list for compose screen, owned by AlfEnv
    CAlfControlGroup& attListControlGroup = 
        iEnv.NewControlGroupL( KSendAttachmentManagerDisplayGroup );
    CFSEmailUiSendAttachmentsListVisualiser* sendAttachmentVisualiser =
        CFSEmailUiSendAttachmentsListVisualiser::NewLC(
            iEnv, &iAppUi, attListControlGroup );
    iAppUi.AddViewL( sendAttachmentVisualiser );
    CleanupStack::Pop( sendAttachmentVisualiser );

    iCrHandler = CFSEmailCRHandler::InstanceL(); // singleton, not owned

    iEnv.AddActionObserverL( this );

    iAutoSaver = CFsAutoSaver::NewL( iEnv, KAutoSaveTimeDelayMs );
    iAutoSaver->Enable( EFalse );
    iFirstStartCompleted = ETrue;
    iCheckQuery = EFalse;
    iAsyncCallback = new (ELeave) CAsyncCallBack( CActive::EPriorityLow );
    // msk context menu added into composer
    MenuBar()->SetContextMenuTitleResourceId( R_NCS_COMPOSE_BODY_MSK_MENUBAR );
    iActiveHelper = CActiveHelper::NewL( this );

    iAsyncAttachmentAdd = 
        new (ELeave) CAsyncCallBack( CActive::EPriorityStandard );
    
    // Initializing the default stylus long tap popup menu
	if( !iStylusPopUpMenu )
		{
		TPoint point( 0, 0 );
		iStylusPopUpMenu = CAknStylusPopUpMenu::NewL( this, point );
		TResourceReader reader;
		iCoeEnv->CreateResourceReaderLC( reader,
				R_STYLUS_POPUP_MENU_COMPOSE_VIEW_ATTACHMENT );
		iStylusPopUpMenu->ConstructFromResourceL( reader );
		CleanupStack::PopAndDestroy(); // reader
		}
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::~CNcsComposeView()
// Destructor.
// -----------------------------------------------------------------------------
//
CNcsComposeView::~CNcsComposeView()
    {
    FUNC_LOG;
	delete iAutoSaver;
	if ( iAsyncCallback )
	    {
	    iAsyncCallback->Cancel();
	    delete iAsyncCallback;
	    }
    delete iFetchLogic;
    if( iActiveHelper )
        {
        iActiveHelper->Cancel();
        delete iActiveHelper;
        }
    if ( iAsyncAttachmentAdd )
        {
        iAsyncAttachmentAdd->Cancel();
        delete iAsyncAttachmentAdd;
        }
    delete iStylusPopUpMenu;
    }

void CNcsComposeView::PrepareForExit()
    {
    FUNC_LOG;
    if ( iCheckQuery && iDlg )
        {
        TKeyEvent check = { EKeyEscape, EStdKeyNull, 0, 0 };
        TRAP_IGNORE( iDlg->OfferKeyEventL( check, EEventKey ) );
        iDlg = NULL;

        iAsyncCallback->Cancel(); // cancel any outstanding callback
        iAsyncCallback->Set( TCallBack( AsyncExit, this ) );
        iAsyncCallback->CallBack();
        }
    else if( iFakeSyncGoingOn || iExecutingDoExitL )
        {
        // low priority active obj so that iwait in plugins finish
        iActiveHelper->Cancel();
        iActiveHelper->Start();
        }
    else
        {
        DoSafeExit( ESaveDraft );
        }

    if( iFetchLogic )
        {
        iFetchLogic->CancelFetchings();
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::Id()
// Returns View's ID.
// -----------------------------------------------------------------------------
//
TUid CNcsComposeView::Id() const
    {
    FUNC_LOG;
    return MailEditorId;
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::ChildDoActivateL()
// Activate the Compose view
// -----------------------------------------------------------------------------
//
void CNcsComposeView::ChildDoActivateL( const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    FUNC_LOG;
    if ( !iFirstStartCompleted )
        {
        DoFirstStartL();
        }
    Toolbar()->SetDimmed( ETrue );
    // Set status pane indicators
    iStatusPaneIndicators = iAppUi.GetStatusPaneIndicatorContainer();
    iStatusPaneIndicators->ShowStatusPaneIndicators();

    // Notify base class about view being entered unless 
    // returned from another view
    if ( aCustomMessageId.iUid != KEditorCmdReturnToPrevious )
        {
        ViewEntered( aPrevViewId );
        }

    // Cleanup the previous message before opening a new one
    SaveAndCleanPreviousMessage();

    // Get the launch parameters
    if ( aCustomMessageId.iUid == KEditorCmdReturnToPrevious )
        {
        // Use previous launch parameters when returning to previously 
        // opened message
        iMailBox = iMailClient.GetMailBoxByUidL( iLaunchParams.iMailboxId );
        }
    else if ( aCustomMessage.Length() > 0 )
        {

        TPckgBuf<TEditorLaunchParams> buf( iLaunchParams );
        buf.Copy( aCustomMessage );
        iLaunchParams = buf();
        iMailBox = iMailClient.GetMailBoxByUidL( iLaunchParams.iMailboxId );

        }
    else
        {
        iLaunchParams.iActivatedExternally = ETrue;
        }

    // Inform the AppUi about external view activation to fix the view stack.
    // Normally this happens based on the previous view AppUid in the BaseView
    // but in case contacts action menu was used within FsEmail, this doesn' 
    // work. It doesn't hurt to call this again even if it was already called
    // once from BaseView.
    if ( iLaunchParams.iActivatedExternally )
        {
        iAppUi.ViewActivatedExternallyL( Id() );
        }

    // Try to get the mailbox by other means if using launch parameters failed
    if ( !iMailBox )
        {
        TInt error( KErrNone );

        // can we get it from MSV Id
        // MSV id is passed to us when account is defined email key settings
        if ( aCustomMessageId != TUid::Null() )
            {
            TRAP( error, iMailBox = TFsEmailUiUtility::GetMailboxForMtmIdL( 
                    iMailClient, iMsvSession, aCustomMessageId.iUid ) );
            }

        error = KErrNone;

        // try to use mailbox set as default in MCE
        if ( !iMailBox )
            {
            TRAP( error, iMailBox = TFsEmailUiUtility::GetMceDefaultMailboxL(
                    iMailClient, iMsvSession ) );
            }

        // use first mail account from the list
        if ( !iMailBox )
            {
            TFSMailMsgId id;
            RPointerArray<CFSMailBox> mailboxes;
            CleanupResetAndDestroyClosePushL( mailboxes );
            iMailClient.ListMailBoxes( id, mailboxes );
            if ( mailboxes.Count() > 0 )
                {
                iMailBox = 
                    iMailClient.GetMailBoxByUidL( mailboxes[0]->GetId() );
                }
            CleanupStack::PopAndDestroy( &mailboxes );
            }

        // could not get mailbox so leave
        if ( !iMailBox )
            {
            User::Leave( KErrGeneral );
            }

        aCustomMessageId = TUid::Uid( KEditorCmdCreateNew );
        }


    iMailSent = EFalse;
    iMailSendFailed = EFalse;
    iCustomMessageId = aCustomMessageId;

    CreateContainerL();

    TRAPD( err, HandleActivationCommandL( aCustomMessageId ) );
    if ( !err )
        {
        err = iMailFetchingErrCode;
        }

    if ( err == KErrCancel || 
         err == KErrCouldNotConnect || 
         err == KErrConnectionTerminated )
        {
        // Just close composer without any notes if operation was cancelled.
        // For connection errors, DownloadInfoMediator already shows error 
        // message, so no need to show another one.
        ExitComposer();
        }
    else if ( err )
        {
        // some other error => show generic error note and close composer
        if( !iAppUi.AppUiExitOngoing() )
            TFsEmailUiUtility::ShowErrorNoteL( 
                R_FREESTYLE_EMAIL_ERROR_GENERAL_UNABLE_TO_COMPLETE, ETrue );
        ExitComposer();
        }
    else
        {
        // everything went fine => do rest of the generic initialization
        InitUiGeneralL();
        iAppUi.StartEndKeyCapture();
        iContainer->SetMskL();
        iViewReady = ETrue;
        Toolbar()->SetDimmed( EFalse );
        RefreshToolbar();
        }
        
    // if there is a embedded app in FSEmail.
    if( iAppUi.EmbeddedApp() )
        {
        // Set email editor started from embedded app flag to true 
        // so that we can switch view correct when sent email.
        iAppUi.SetEditorStartedFromEmbeddedApp( ETrue );
        
        RWsSession rwsSession;
        User::LeaveIfError( rwsSession.Connect() );
        CleanupClosePushL( rwsSession );
        
        // Simulate a back key to exit embedded app 
        // so that email editor could show on the top level.
        TKeyEvent KeyEvent = TKeyEvent();
        KeyEvent.iCode = EKeyCBA2;
        rwsSession.SimulateKeyEvent( KeyEvent );

        rwsSession.Close();
        CleanupStack::PopAndDestroy( &rwsSession );
        }
        
    TIMESTAMP( "Editor launched" );
    }

// -----------------------------------------------------------------------------
// Initialises toolbar items.
// -----------------------------------------------------------------------------
//
void CNcsComposeView::DynInitToolbarL( TInt aResourceId,
    CAknToolbar* aToolbar )
    {
    FUNC_LOG;
    if ( aResourceId == EFsEmailUiTbCmdExpandActions && aToolbar )
        {
        CAknToolbarExtension* ext = aToolbar->ToolbarExtension();
        if ( iContainer )
            {
            // Set correct state for show/hide cc field button.
            CAknButton* ccButton = Button( EFsEmailUiTbCmdCcField, ext );
            if ( ccButton )
                {
                TInt index = iContainer->IsCcFieldVisible() ? 1 : 0;
                ccButton->SetCurrentState( index, EFalse );

                // Command is dimmed, if there is content in the field.
                ccButton->SetDimmed( iContainer->GetCcFieldLength() > 0 );
                }

            // Set correct state for show/hide bcc field button.
            CAknButton* bccButton = Button( EFsEmailUiTbCmdBccField, ext );
            if ( bccButton )
                {
                TInt index = iContainer->IsBccFieldVisible() ? 1 : 0;
                bccButton->SetCurrentState( index, EFalse );

                // Command is dimmed, if there is content in the field.
                bccButton->SetDimmed( iContainer->GetBccFieldLength() > 0 );
                }
            }
        else
            {
            ext->HideItemL( EFsEmailUiTbCmdCcField, ETrue );
            ext->HideItemL( EFsEmailUiTbCmdBccField, ETrue );
            }

        if ( iNewMessage )
            {
            // Set correct state for low priority button.
            TBool lowPriority = iNewMessage->IsFlagSet( EFSMsgFlag_Low );
            CAknButton* lowPriorityButton = static_cast<CAknButton*>(
                ext->ControlOrNull( EFsEmailUiTbCmdLowPriority ) );
            if ( lowPriorityButton )
                {
                TInt index = lowPriority ? 1 : 0;
                lowPriorityButton->SetCurrentState( index, EFalse ); 
                }

            // Set correct state for high priority button.
            TBool highPriority = iNewMessage->IsFlagSet( EFSMsgFlag_Important );
            CAknButton* highPriorityButton = static_cast<CAknButton*>(
                ext->ControlOrNull( EFsEmailUiTbCmdHighPriority ) );
            if ( highPriorityButton )
                {
                TInt index = highPriority ? 1 : 0;
                highPriorityButton->SetCurrentState( index, EFalse );
                }

            // Set correct state for follow up button.
            TBool followUp = iNewMessage->IsFlagSet( EFSMsgFlag_FollowUp );
            CAknButton* followUpButton = static_cast<CAknButton*>(
                ext->ControlOrNull( EFsEmailUiTbCmdFollowUp ) );
            if ( followUpButton )
                {
                TInt index = followUp ? 1 : 0;
                followUpButton->SetCurrentState( index, EFalse );

                // Hide follow up button, if follow up is not supported.
                ext->HideItemL( EFsEmailUiTbCmdFollowUp,
                    !TFsEmailUiUtility::IsFollowUpSupported( *iMailBox ) );
                }
            }
        else
            {
            ext->HideItemL( EFsEmailUiTbCmdLowPriority, ETrue );
            ext->HideItemL( EFsEmailUiTbCmdHighPriority, ETrue );
            ext->HideItemL( EFsEmailUiTbCmdFollowUp, ETrue );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::OfferToolbarEventL
// -----------------------------------------------------------------------------
void CNcsComposeView::OfferToolbarEventL( TInt aCommand )
    {
    FUNC_LOG;
    TBool attachmentAddition = EFalse;
    switch ( aCommand )
        {
        case EFsEmailUiTbCmdSend:
            if ( iContainer && iContainer->AreAddressFieldsEmpty() )
                {
                RefreshToolbar();
                }
            else
                {
                HandleCommandL( ENcsCmdSend );
                }
            break;
        case EFsEmailUiTbCmdCcField:
            {
            CAknButton* ccFieldButton = Button( EFsEmailUiTbCmdCcField );
            if ( ccFieldButton )
                {
                if ( ccFieldButton->StateIndex() )
                    {
                    HandleCommandL( ENcsCmdShowCc );
                    }
                else
                    {
                    HandleCommandL( ENcsCmdHideCc );
                    }
                }
            break;
            }
        case EFsEmailUiTbCmdBccField:
            {
            CAknButton* bccFieldButton = Button( EFsEmailUiTbCmdBccField );
            if ( bccFieldButton )
                {
                if ( bccFieldButton->StateIndex()  )
                    {
                    HandleCommandL( ENcsCmdShowBcc );
                    }
                else
                    {
                    HandleCommandL( ENcsCmdHideBcc );
                    }
                }
            break;
            }
        case EFsEmailUiTbCmdLowPriority:
            {
            CAknButton* lowPriorityButton = Button( EFsEmailUiTbCmdLowPriority );
            if ( lowPriorityButton )
                {
                if ( lowPriorityButton->StateIndex() )
                    {
                    CAknButton* highPriorityButton = Button( 
                        EFsEmailUiTbCmdHighPriority );
                    if ( highPriorityButton )
                        {
                        highPriorityButton->SetCurrentState( 0, ETrue );
                        }
                    HandleCommandL( ENcsCmdPriorityLow );
                    }
                else
                    {
                    HandleCommandL( ENcsCmdPriorityNormal );
                    }
                }
            break;
            }
        case EFsEmailUiTbCmdHighPriority:
            {
            CAknButton* highPriorityButton = Button( EFsEmailUiTbCmdHighPriority );
            if ( highPriorityButton )
                {
                if ( highPriorityButton->StateIndex() )
                    {
                    CAknButton* lowPriorityButton = Button( 
                        EFsEmailUiTbCmdLowPriority );
                    if ( lowPriorityButton )
                        {
                        lowPriorityButton->SetCurrentState( 0, ETrue );
                        }
                    HandleCommandL( ENcsCmdPriorityHigh );
                    }
                else
                    {
                    HandleCommandL( ENcsCmdPriorityNormal );
                    }
                }
            break;
            }
        case EFsEmailUiTbCmdFollowUp:
            {
            CAknButton* button = Button( EFsEmailUiTbCmdFollowUp );
            if ( button && iNewMessage )
                {
                if ( button->StateIndex() )
                    {
                    iNewMessage->SetFlag( EFSMsgFlag_FollowUp );
                    iNewMessage->ResetFlag( EFSMsgFlag_FollowUpComplete );
                    iStatusPaneIndicators->SetFollowUpFlag( 
                        CCustomStatuspaneIndicators::EFollowUp );
                    }
                else
                    {
                    iNewMessage->ResetFlag( EFSMsgFlag_FollowUp 
                        | EFSMsgFlag_FollowUpComplete );
                    iStatusPaneIndicators->SetFollowUpFlag( 
                        CCustomStatuspaneIndicators::EFollowUpNone );
                    }
                }
            break;
            }
        case EFsEmailUiTbCmdInsertAudio:
            iAttachmentAddType = MsgAttachmentUtils::EAudio;
            attachmentAddition = ETrue;
            break;
        case EFsEmailUiTbCmdInsertImage:
            iAttachmentAddType = MsgAttachmentUtils::EImage;
            attachmentAddition = ETrue;
            break;
        case EFsEmailUiTbCmdInsertVideo:
            iAttachmentAddType = MsgAttachmentUtils::EVideo;
            attachmentAddition = ETrue;
            break;
        case EFsEmailUiTbCmdInsertNote:
            iAttachmentAddType = MsgAttachmentUtils::ENote;
            attachmentAddition = ETrue;
            break;
        case EFsEmailUiTbCmdInsertPresentation:
            iAttachmentAddType = MsgAttachmentUtils::ESVG;
            attachmentAddition = ETrue;
            break;
        case EFsEmailUiTbCmdInsertOther:
            iAttachmentAddType = MsgAttachmentUtils::EUnknown;
            attachmentAddition = ETrue;
            break;
        default:
            break;
        }
    if ( attachmentAddition )
        {
        iAsyncAttachmentAdd->Cancel(); // cancel any outstanding callback
        iAsyncAttachmentAdd->Set( TCallBack( AsyncAddAttachment, this ) );
        iAsyncAttachmentAdd->CallBack();
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::ToolbarResourceId()
// -----------------------------------------------------------------------------
//
TInt CNcsComposeView::ToolbarResourceId() const
    {
    FUNC_LOG;
    return R_FREESTYLE_EMAIL_UI_TOOLBAR_MESSAGE_EDITOR;
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::GetInitiallyDimmedItemsL()
// -----------------------------------------------------------------------------
//
void CNcsComposeView::GetInitiallyDimmedItemsL( const TInt aResourceId,
        RArray<TInt>& aDimmedItems ) const
    {
    FUNC_LOG;
    aDimmedItems.Reset();
    switch ( aResourceId )
        {
        case R_FREESTYLE_EMAIL_UI_TOOLBAR_MESSAGE_EDITOR:
            aDimmedItems.AppendL( EFsEmailUiTbCmdSend );
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::RefreshToolbar()
// -----------------------------------------------------------------------------
//
void CNcsComposeView::RefreshToolbar()
    {
    FUNC_LOG;
    if ( iContainer )
        {
        // DimAllOptions if remotesearch is in progress, 
        // because it takes you into a different view
        TBool dimAllOptions = iContainer->IsRemoteSearchInprogress();
        SetToolbarItemDimmed( EFsEmailUiTbCmdExpandActions, dimAllOptions );
        SetToolbarItemDimmed( EFsEmailUiTbCmdExpandInsert, dimAllOptions );
        SetToolbarItemDimmed( EFsEmailUiTbCmdSend, dimAllOptions 
            || iContainer->AreAddressFieldsEmpty() );
        }
    }


// -----------------------------------------------------------------------------
// CNcsComposeView::ChildDoDeactivate()
// Deactivate the Compose view
// -----------------------------------------------------------------------------
//
void CNcsComposeView::ChildDoDeactivate()
    {
    FUNC_LOG;

    iAppUi.StopEndKeyCapture();

    SafeDelete( iFetchLogic );

    // Normally Comopser state has been already cleaned by now but in special
    // cases (like external view activation while in Composer) this may not be
    // the case. In those cases the message needs to be saved before the view
    // is deactivated.
    if ( !iAppUi.AppUiExitOngoing() )
        {
        SaveAndCleanPreviousMessage();
        }

    }

// -----------------------------------------------------------------------------
// CNcsComposeView::DynInitMenuPaneL()
// -----------------------------------------------------------------------------
//
void CNcsComposeView::DynInitMenuPaneL( TInt aResourceId,
                                        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;

    if ( !iViewReady )
        {
        User::Leave( KErrNcsComposeViewNotReady );
        }

    __ASSERT_DEBUG( iContainer, Panic( ENcsBasicUi ) );
    __ASSERT_DEBUG( iMailBox, Panic( ENcsBasicUi ) );
    __ASSERT_DEBUG( iNewMessage, Panic( ENcsBasicUi ) );

    //we are forbiding to change MSK label, because popup will be opened 
    iContainer->SwitchChangeMskOff( ETrue );
    
    if ( aResourceId == R_NCS_COMPOSE_MENU )
        {
	    if ( FeatureManager::FeatureSupported( 
	            KFeatureIdFfCmailIntegration ) )
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
		   }     
        
    	// Disable send if there are no addresses in any of the AIF fields,
    	if ( iContainer->AreAddressFieldsEmpty() )
		    {
	    	aMenuPane->DeleteMenuItem( ENcsCmdSend );
		    }

	    // If no attachments, delete the remove attachments menu item
		CFreestyleEmailUiSendAttachmentsListControl* attachmentControl;
		attachmentControl = AttachmentsListControl();
		TInt count = attachmentControl->Model()->Count();

        // if mail has remote attachments then attachments can't be 
		// removed via options menu
		if ( count <= 0 || 
		     AttachmentsListControl()->Model()->HasReadOnlyAttachments() )
	    	{
    		aMenuPane->DeleteMenuItem( ENcsCmdRemoveAttachment );
    		aMenuPane->DeleteMenuItem( ENcsCmdRemoveAllAttachments );
	    	}
        else if ( count == 1 )
	     	{
    		aMenuPane->DeleteMenuItem( ENcsCmdRemoveAllAttachments );
	     	}
        else
	     	{
    		aMenuPane->DeleteMenuItem( ENcsCmdRemoveAttachment );
	     	}

        if ( iContainer->GetCcFieldLength() > 0 &&
             iContainer->GetBccFieldLength() > 0 )
        	{
            aMenuPane->DeleteMenuItem( ENcsCmdMore );
        	}

        if ( !TFsEmailUiUtility::IsFollowUpSupported( *iMailBox ) )
        	{
    		aMenuPane->DeleteMenuItem( ENcsCmdFlag );
        	}
        }

    if ( aResourceId == R_NCS_EXTRA_RECEPIENT_FIELDS_MENU )
        {
		// Select which View->CC menu items to show
		if ( iContainer->IsCcFieldVisible() )
            {
            aMenuPane->DeleteMenuItem( ENcsCmdShowCc );
            // if there is text in Cc field do not show "hide Cc" menu item
            if ( iContainer->GetCcFieldLength() > 0 )
            	{
            	aMenuPane->DeleteMenuItem( ENcsCmdHideCc );
            	}
            }
        else
            {
        	aMenuPane->DeleteMenuItem( ENcsCmdHideCc );
            }

		// Select which View->BCC menu items to show
        if ( iContainer->IsBccFieldVisible() )
            {
            aMenuPane->DeleteMenuItem( ENcsCmdShowBcc );
            // if there is text in Bcc field do not show "hide Bcc" menu item
            if ( iContainer->GetBccFieldLength() > 0 )
            	{
            	aMenuPane->DeleteMenuItem( ENcsCmdHideBcc );
            	}
            }
        else
            {
            aMenuPane->DeleteMenuItem( ENcsCmdHideBcc );
            }
        }

	if ( aResourceId == R_NCS_PRIORITY_MENU )
    	{

        if ( iNewMessage->IsFlagSet( EFSMsgFlag_Important ) )
            {
            aMenuPane->DeleteMenuItem( ENcsCmdPriorityHigh );
            }
        else if ( iNewMessage->IsFlagSet( EFSMsgFlag_Low ) )
            {
            aMenuPane->DeleteMenuItem( ENcsCmdPriorityLow );
            }
        else
            {
            aMenuPane->DeleteMenuItem( ENcsCmdPriorityNormal );
            }
    	}

    }

// -----------------------------------------------------------------------------
// CNcsComposeView::HandleCommandL()
// Takes care of Command handling.
// -----------------------------------------------------------------------------
//
void CNcsComposeView::HandleCommandL( TInt aCommand )
    {
    FUNC_LOG;

    // Attempt to handle commands only after view is fully activated and exit
    // has not yet been initiated.
    if ( iViewReady && !iAppUi.ViewSwitchingOngoing() )
        {
        __ASSERT_DEBUG( iContainer, Panic( ENcsBasicUi ) );
        __ASSERT_DEBUG( iNewMessage, Panic( ENcsBasicUi ) );
        __ASSERT_DEBUG( iCrHandler, Panic( ENcsBasicUi ) );

        switch( aCommand )
            {
            case ENcsCmdSend:
                {
                // Show query if the Subject field is empty
                if ( iContainer->IsSubjectFieldEmpty() )
                    {
                    TInt answer = TFsEmailUiUtility::ShowConfirmationQueryL(
                            R_FREESTYLE_EMAIL_QUERY_NO_SUBJECT );
                    if ( !answer )
                        {
                        // user didn't want to send yet
                        // -> go back to the message
                        return;
                        }
                    }

                TRAPD( error, DoSendL() );
                if ( !error )
                    {
                    // Sending successful
                    HBufC* confMessage = NULL;
                    if ( !TFsEmailUiUtility::IsOfflineModeL() )
                        {
                        // when sync status is currently ONLINE
                        confMessage = StringLoader::LoadLC( 
                                R_FREESTYLE_EMAIL_CONFIRM_NOTE_SENDING_QUEUED );
                        }
                    else
                        {
                        // when sync status is currently OFFLINE
                        confMessage = StringLoader::LoadLC( 
                                R_FREESTYLE_EMAIL_CONFIRM_NOTE_QUEUED_UNTIL_ONLINE );
                        }
                    CAknConfirmationNote* note = 
                        new (ELeave) CAknConfirmationNote( ETrue ); //waiting
                    note->SetTimeout( CAknNoteDialog::ELongTimeout );
                    note->ExecuteLD( *confMessage );
                    CleanupStack::PopAndDestroy( confMessage );
                    DoSafeExit(); // Exit after successful sending
                    }
                else if ( error == KErrBadName )
                	{
                  	// sending failed because recipient address was invalid
                	TFsEmailUiUtility::ShowErrorNoteL( 
                        R_FREESTYLE_EMAIL_CONFIRM_NOTE_INVALID_EMAIL_ADDRESS,
                        ETrue );
                	}
                else
                	{
                	// sending failed for some other reason
                    TFsEmailUiUtility::ShowErrorNoteL( 
                        R_FREESTYLE_EMAIL_CONFIRM_NOTE_MESSAGE_NOT_SENT,
                        ETrue );
                    // Exit after sending failed for other reason for other
                    // reason than KErrBadName
                	DoSafeExit();
                	}
                }
                break;
            case ENcsCmdAddAttachment:
           	 	{
                if ( AknLayoutUtils::PenEnabled() )
                    {
                    // show the toolbar
                    UpdateToolbarL();
                    if ( Toolbar() && Toolbar()->ToolbarExtension() )
                        {
                        Toolbar()->ToolbarExtension()->SetShown( ETrue );
                        }
                    }
                else
                    {
                    iAttachmentAddType = MsgAttachmentUtils::EUnknown;
                    iAsyncAttachmentAdd->Cancel();
                    iAsyncAttachmentAdd->Set( 
                            TCallBack( AsyncAddAttachment, this ) );
                    iAsyncAttachmentAdd->CallBack();
                    iContainer->SetMskL();
                    }                
           	 	}
                break;
            case EFsEmailUiCmdOpenAttachment:
            case EFsEmailUiCmdOpenAttachmentList:
                {
                DoOpenAttachmentListL();
                }
                break;
            case ENcsCmdRemoveAttachment:
            case EFsEmailUiCmdRemoveAttachment:
                {
                TInt index( iContainer->FocusedAttachmentLabelIndex() );
                CFreestyleEmailUiSendAttachmentsListControl* ctrl =
                    AttachmentsListControl();
                if ( ctrl && KNoAttachmentLabelFocused != index )
                    {
                    ctrl->RemoveAttachmentFromListL( index );
                    }
                SetAttachmentLabelContentL();
                }
                break;
            case ENcsCmdRemoveAllAttachments:
           	 	{
           	 	CFreestyleEmailUiSendAttachmentsListControl* ctrl =
                    AttachmentsListControl();
           	 	if ( ctrl )
           	 	    {
           	 	    ctrl->RemoveAllAttachmentsL();
           	 	    }
           	 	SetAttachmentLabelContentL();
            	}
                break;
            case ENcsCmdPriorityHigh:
            	{
            	iNewMessage->ResetFlag( EFSMsgFlag_Low );
            	iNewMessage->SetFlag( EFSMsgFlag_Important );
            	iNewMessage->SaveMessageL();
            	iStatusPaneIndicators->SetPriorityFlag( EMsgPriorityHigh );
            	}
                break;
        	case ENcsCmdPriorityNormal:
    	    	{
            	iNewMessage->ResetFlag( EFSMsgFlag_Low );
            	iNewMessage->ResetFlag( EFSMsgFlag_Important );
            	iNewMessage->SaveMessageL();
            	iStatusPaneIndicators->SetPriorityFlag( EMsgPriorityNormal );
    	    	}
                break;
        	case ENcsCmdPriorityLow:
    	    	{
            	iNewMessage->ResetFlag( EFSMsgFlag_Important );
            	iNewMessage->SetFlag( EFSMsgFlag_Low );
            	iNewMessage->SaveMessageL();
            	iStatusPaneIndicators->SetPriorityFlag( EMsgPriorityLow );
    	    	}
                break;
            case ENcsCmdShowCc:
                {
                iContainer->SetCcFieldVisibleL( ETrue );
                iCrHandler->SetEditorCCVisible( 1 );
                }
                break;
            case ENcsCmdHideCc:
                {
                iContainer->SetCcFieldVisibleL( EFalse );
                iCrHandler->SetEditorCCVisible( 0 );
                }
                break;
            case ENcsCmdShowBcc:
                {
                iContainer->SetBccFieldVisibleL( ETrue );
                iCrHandler->SetEditorBCVisible( 1 );
                }
                break;
            case ENcsCmdHideBcc:
                {
                iContainer->SetBccFieldVisibleL( EFalse );
                iCrHandler->SetEditorBCVisible( 0 );
                }
                break;
            case ENcsCmdFlag:
    	        {
    	        RunFollowUpFlagDlgL();
    	        }
                break;
            case ENcsCmdQuickText:
    	        {
    	        DoQuickTextL();
    	        }
                break;
            case ENcsCmdInsertContact:
            case ENcsInsertContact:
            	{
            	iContainer->AppendAddressesL();
            	}
                break;
            case ENcsCmdSaveDraft:
            	{

                iContainer->FixSemicolonL();


            	TRAPD( saveDraftError, DoSaveDraftL( EFalse ) );
                if ( saveDraftError == KErrNone )
                	{
                    // Saving successful
                	if( !iAppUi.AppUiExitOngoing() )
                	TFsEmailUiUtility::ShowInfoNoteL( 
                        R_FREESTYLE_EMAIL_CONFIRM_NOTE_SAVED_TO_DRAFTS,
                        ETrue );
                    }
            	else
                    {
                    // error occured in saving -> show an error message
                    if( !iAppUi.AppUiExitOngoing() )
                        TFsEmailUiUtility::ShowErrorNoteL( 
                            R_FREESTYLE_EMAIL_ERROR_GENERAL_UNABLE_TO_COMPLETE,
                            ETrue );
                    }

            	DoSafeExit( ENoSave );
            	}
                break;
            case EAknCmdHelp:
            	{
            	TFsEmailUiUtility::LaunchHelpL( KFSE_HLP_LAUNCHER_GRID );
    	        }
            	break;
            case EAknSoftkeySelect:
                {
                iContainer->DoPopupSelectL();
                }
                break;
       	    case EAknSoftkeyCancel:
        	    {
    	 		iContainer->ClosePopupContactListL();
                }
                break;
            case EAknSoftkeyClose:
                {
                DoSafeExit();
                }
                break;
            case ENcsCmdExit:
                {
                TIMESTAMP( "Exit selected from editor" );
                iAppUi.Exit();
                }
                break;
            default:
                {
                // empty implementation
                // these commands are passed next to FEP for processing
                }
                break;
            }
        }

    }

// -----------------------------------------------------------------------------
// CNcsComposeView::DoSendL()
// Send message
// -----------------------------------------------------------------------------
//
void CNcsComposeView::DoSendL()
    {
    FUNC_LOG;

    // check that addresses exist
    if ( iContainer->AreAddressFieldsEmpty() )
        {
        User::Leave( KErrNotFound );
        }

	CommitL();

    TRAPD(r, iMailBox->SendMessageL( *iNewMessage ) );
    if ( KErrNone != r )
        {
        iMailSendFailed = ETrue;
        User::Leave( r );
        }

    // prevent sending failure if original message is
    // removed from server while replying/forwarding
	TRAPD( flagError, SetReplyForwardFlagL() );

    if ( KErrNone != flagError && KErrNotFound != flagError )
        {
        iMailSendFailed = ETrue;
        User::Leave( flagError );
        }

    iMailSent = ETrue;

    }


// -----------------------------------------------------------------------------
// CNcsComposeView::DoSaveDraftL()
// Saves message to Drafts
// Displays dialog asking user choice if preferred
// Pressing cancel in the query dialog causes Leave with code KErrCancel
// -----------------------------------------------------------------------------
//
TBool CNcsComposeView::DoSaveDraftL( TBool aAskUser )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iNewMessage, Panic( ENcsBasicUi ) );

    TBool result( ETrue );

    if ( aAskUser )
        {
    	TInt selectedOption( -1 );
    	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat( 2 );
    	CleanupStack::PushL( array );

    	HBufC* saveItem = StringLoader::LoadLC( R_NCS_DRAFT_SAVE );
        array->AppendL( *saveItem );
		CleanupStack::PopAndDestroy( saveItem );

    	HBufC* deleteItem = StringLoader::LoadLC( R_NCS_DRAFT_DELETE );
        array->AppendL( *deleteItem );
		CleanupStack::PopAndDestroy( deleteItem );

		iDlg = new (ELeave) CAknListQueryDialog( &selectedOption );
    	iDlg->PrepareLC( R_DRAFT_QUERY_DIALOG );
    	iDlg->SetItemTextArray( array );
    	iDlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

        iCheckQuery = ETrue;
        //we are forbiding to change MSK label, cause popup will be opend 
        iContainer->SwitchChangeMskOff( ETrue );
        TInt ret = iDlg->RunLD();
        iDlg = NULL;

        iCheckQuery = EFalse; 
        iContainer->SwitchChangeMskOff( EFalse );

    	if ( !ret )
    	    {
    	    selectedOption = KErrCancel;
    	    }

    	switch ( selectedOption )
    		{
    		case 0:
    			{
				TRAPD( saveError, SaveToDraftsL( ETrue ) );

				if ( saveError == KErrNone )
				    {
				    // User wanted to save to Drafts and saving was successful
				    if( !iAppUi.AppUiExitOngoing() )
				        TFsEmailUiUtility::ShowInfoNoteL( 
                            R_FREESTYLE_EMAIL_CONFIRM_NOTE_SAVED_TO_DRAFTS, 
                            ETrue );
				    }
				else
				    {

				    // error occured in saving -> show an error message
				    if( !iAppUi.AppUiExitOngoing() )
				        TFsEmailUiUtility::ShowErrorNoteL( 
                            R_FREESTYLE_EMAIL_ERROR_GENERAL_UNABLE_TO_COMPLETE,
                            ETrue );

				    // let's return EFalse so after completing this query
				    // so the focus would stay in compose view (same behaviour
				    // as if Cancel was pressed )
				    result = EFalse;
				    }
				}
                break;
    		case 1:
    			{
    			TIMESTAMP( "Delete draft message selected in editor" ); 
				TRAPD( error, SaveToDraftsL( ETrue ) );
    			if ( KErrNone != error )
    			    {
    			    }
    			// for delete messages fake sync calls
    	        iFakeSyncGoingOn = ETrue;
                error = NcsUtility::DeleteMessage( iMailClient,
            		iMailBox->GetId(),
            		iNewMessage->GetFolderId(),
            		iNewMessage->GetMessageId() );

    			if ( KErrNone == error && 
    			     iMailBox->HasCapability( 
    			             EFSMBoxCapaSupportsDeletedItemsFolder ) )
    			    {
                    error = NcsUtility::DeleteMessage( iMailClient,
                		iMailBox->GetId(),
                		iMailBox->GetStandardFolderId( EFSDeleted ),
                		iNewMessage->GetMessageId() );
    			    }
    		     iFakeSyncGoingOn = EFalse;

                if( !iAppUi.AppUiExitOngoing() )
                    {
                    if ( KErrNone != error )
                        {
                        // error occured in saving -> show an error message
                        TFsEmailUiUtility::ShowErrorNoteL( 
                            R_FREESTYLE_EMAIL_ERROR_GENERAL_UNABLE_TO_COMPLETE,
                            ETrue );
                        }
                    else
                        {
                        // Simulate delete event so that drafts folder is
                        // updated correctly.
                        RArray<TFSMailMsgId> messageIds;
                        CleanupClosePushL( messageIds );
                        messageIds.AppendL( iNewMessage->GetMessageId() );
                        TFSMailMsgId folderId = iNewMessage->GetFolderId();
                        TFSMailMsgId mailboxId = iNewMessage->GetMailBoxId();
                        iAppUi.EventL( TFSEventMailDeleted,
                            mailboxId, &messageIds, &folderId, NULL );
                        CleanupStack::PopAndDestroy( &messageIds );
                        }
                    }
                }
                break;
    		default:
    			{
    			result = EFalse;
    			}
                break;
    		}
    	CleanupStack::PopAndDestroy( array );
        }
    else  // no question from user
        {
        SaveToDraftsL( ETrue );
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::NewMessage()
// Returns pointer to currently composed mail message
// -----------------------------------------------------------------------------
//
CFSMailMessage* CNcsComposeView::NewMessage()
    {
    FUNC_LOG;
	return iNewMessage;
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::AsyncAddAttachmentL()
// Open file dialog and add selected file as an attachment
// -----------------------------------------------------------------------------
//
TInt CNcsComposeView::AsyncAddAttachment( TAny* aSelfPtr )
    {
    FUNC_LOG;

    CNcsComposeView* self = static_cast<CNcsComposeView*>( aSelfPtr );

    // get pointer to attachment list view
	CFreestyleEmailUiSendAttachmentsListControl* attachmentControl;
	attachmentControl = self->AttachmentsListControl();

   	// show file dialog and get file name
	TBool ok = EFalse;
	TRAPD( error, ok = attachmentControl->AppendAttachmentToListL(
                self->iAttachmentAddType) );
	if ( ok && error == KErrNone )
	    {
	    TRAPD( error, self->SetAttachmentLabelContentL() );
	    return error;
	    }
	return error;
    }

// -----------------------------------------------------------------------------
// RunFollowUpFlagDlgL()
// Query user for followup flag
// -----------------------------------------------------------------------------
//
void CNcsComposeView::RunFollowUpFlagDlgL()
    {
    FUNC_LOG;
    if ( iNewMessage && TFsEmailUiUtility::IsFollowUpSupported( *iMailBox ) )
        {
        TFollowUpNewState newState =
            TFsEmailUiUtility::SetMessageFollowupFlagL( 
                    *iNewMessage, EFalse );

        switch ( newState )
            {
            case EFollowUp:
                {
                iStatusPaneIndicators->SetFollowUpFlag( 
                        CCustomStatuspaneIndicators::EFollowUp );
                }
                break;
            case EFollowUpComplete:
                {
                iStatusPaneIndicators->SetFollowUpFlag( 
                        CCustomStatuspaneIndicators::EFollowUpComplete );
                }
                break;
            case EFollowUpClear:
                {
                iStatusPaneIndicators->SetFollowUpFlag( 
                        CCustomStatuspaneIndicators::EFollowUpNone );
                }
                break;
            default:
                {
                // do nothing.
                }
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::SetAttachmentLabelContentL()
// Open file dialog and add selected file as an attachment
// -----------------------------------------------------------------------------
//
void CNcsComposeView::SetAttachmentLabelContentL()
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && iContainer )
        {
        CFreestyleEmailUiSendAttachmentsListControl* 
            attachmentControl = AttachmentsListControl();
        if ( !( attachmentControl && attachmentControl->Model() ) )
            {
            return;
            }
        
        // read number of attachments
        TInt count = attachmentControl->Model()->Count();
        if ( count > 0 )
            {
            // Fill in the name and size descriptor arrays
            CDesCArrayFlat* nameArray = new (ELeave) CDesCArrayFlat( count );
            CleanupStack::PushL( nameArray );
            CDesCArrayFlat* sizeArray = new (ELeave) CDesCArrayFlat( count );
            CleanupStack::PushL( sizeArray );

            for ( TInt i(0); i<count; ++i )
                {
                CFSEmailUiSendAttachmentsListModelItem* item =
                    static_cast<CFSEmailUiSendAttachmentsListModelItem*>
                    ( attachmentControl->Model()->Item( i ) );
                if ( item )
                    {
                    nameArray->AppendL( item->FileName() );
                    HBufC* sizeDesc = 
                        TFsEmailUiUtility::CreateSizeDescLC( 
                                item->FileSize() );
                    sizeArray->AppendL( *sizeDesc );
                    CleanupStack::PopAndDestroy( sizeDesc );
                    }
                }
            iContainer->SetAttachmentLabelTextsLD( nameArray, sizeArray );
            iContainer->SetFocusToAttachmentField();
            
            CleanupStack::Pop( sizeArray );
            CleanupStack::Pop( nameArray );
            }
        else
            {
            iContainer->SetAttachmentLabelTextsLD( NULL, NULL );
            iContainer->HideAttachmentLabel();
            }
        }
    }


// -----------------------------------------------------------------------------
// CNcsComposeView::AttachmentsListControl()
//
// -----------------------------------------------------------------------------
//
CFreestyleEmailUiSendAttachmentsListControl* 
CNcsComposeView::AttachmentsListControl()
	{
    FUNC_LOG;
    CFreestyleEmailUiSendAttachmentsListControl* attachmentControl( NULL );
    if ( iFirstStartCompleted ) // Safety
        {
        CFSEmailUiSendAttachmentsListVisualiser* attachmentView =
            static_cast<CFSEmailUiSendAttachmentsListVisualiser*>(
                iAvkonViewAppUi->View( SendAttachmentMngrViewId ) );
        if ( attachmentView )
            {
            attachmentControl = 
                static_cast<CFreestyleEmailUiSendAttachmentsListControl*>(
                    attachmentView->ViewerControl() );
            }
        }
	return attachmentControl;
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::ProcessCommandL()
// -----------------------------------------------------------------------------
//
void CNcsComposeView::ProcessCommandL( TInt aCommand )
    {
    FUNC_LOG;

    // Block all the commands while we are exiting
    if ( !iExecutingDoExitL )
        {
        CFsEmailUiViewBase::ProcessCommandL( aCommand );
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::HandleActivationCommandL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeView::HandleActivationCommandL( TUid aCustomMessageId )
    {
    FUNC_LOG;
    // Called from doactivate in which case first start is completed
    iMailFetchingErrCode = KErrNone;

    if ( aCustomMessageId == TUid::Uid( KEditorCmdCreateNew ) )
        {
        iOrigMessage = NULL;
        iFakeSyncGoingOn = ETrue;
        iNewMessage = iMailBox->CreateMessageToSend();
        iFakeSyncGoingOn = EFalse;
        if ( !iNewMessage )
            {
            User::Leave( KErrGeneral );
            }
        iFakeSyncGoingOn = ETrue;
        TFsEmailUiUtility::MoveMessageToDraftsL( *iMailBox, *iNewMessage );
        iFakeSyncGoingOn = EFalse;
        
        if ( iNewMessage->GetContentType() != 
             KFSMailContentTypeMultipartMixed )
            {
            iNewMessage->SetContentType( KFSMailContentTypeMultipartMixed );
            iNewMessage->SaveMessageL();
            }
        
        TFsEmailUiUtility::CreatePlainTextPartL( 
                *iNewMessage, iNewMessageTextPart );
        IncludeMessageTextL( ETrue );
        AttachmentsListControl()->Model()->Clear();
        iContainer->SelectAllToFieldTextL();
        }
    else if ( aCustomMessageId == TUid::Uid( KEditorCmdReply ) ||
              aCustomMessageId == TUid::Uid( KEditorCmdReplyAll ) )
        {

        iOrigMessage = iMailClient.GetMessageByUidL( 
                iLaunchParams.iMailboxId, iLaunchParams.iFolderId,
                iLaunchParams.iMsgId, EFSMsgDataStructure );
        if ( !iOrigMessage )
            {
            User::Leave( KErrNotFound );
            }

        SafeDelete( iFetchLogic );
        iFetchLogic = CFsComposerFetchLogic::NewL( 
                iMailClient, iLaunchParams.iMailboxId,
                iOrigMessage->GetFolderId(), iLaunchParams.iMsgId, 
                *this, iAppUi );
        ShowFetchingWaitNoteL();
        iFetchLogic->RunReplyLogicL();

        // now fetch logic object runs its tasks and gives callback
        // to CNcsComposeView::FetchLogicComplete when it is finished
        }
    else if ( aCustomMessageId == TUid::Uid( KEditorCmdForward ) )
        {

        iOrigMessage = iMailClient.GetMessageByUidL( 
                iLaunchParams.iMailboxId, iLaunchParams.iFolderId, 
                iLaunchParams.iMsgId, EFSMsgDataStructure );
        if ( !iOrigMessage )
            {
            User::Leave( KErrNotFound );
            }

        SafeDelete( iFetchLogic );
        iFetchLogic = CFsComposerFetchLogic::NewL( 
                iMailClient, iLaunchParams.iMailboxId,
                iOrigMessage->GetFolderId(), iLaunchParams.iMsgId, 
                *this, iAppUi );
        ShowFetchingWaitNoteL();
        iFetchLogic->RunForwardLogicL();

        // now fetch logic object runs its tasks and gives callback
        // to CNcsComposeView::FetchLogicComplete when it is finished
        }
    else if ( aCustomMessageId == TUid::Uid( KEditorCmdOpen ) ||
              aCustomMessageId == TUid::Uid( KEditorCmdReturnToPrevious ) )
        {
        // for now, we handle returning from attachments list just like 
        // any message opening
        iNewMessage = iMailClient.GetMessageByUidL( 
                iLaunchParams.iMailboxId, iLaunchParams.iFolderId, 
                iLaunchParams.iMsgId, EFSMsgDataStructure );
        if ( !iNewMessage )
            {
            User::Leave( KErrNotFound );
            }

        // Show "Opening" wait note if the message body is large
        TInt waitNoteId = KErrNotFound;
        if ( TFsEmailUiUtility::IsMessageBodyLargeL(iNewMessage) )
            {
            waitNoteId = TFsEmailUiUtility::ShowGlobalWaitNoteLC( 
                    R_FSE_WAIT_OPENING_TEXT );
            }

        iOrigMessage = NULL;
        iNewMessage->SetContentType( KFSMailContentTypeMultipartMixed );
        iNewMessage->SaveMessageL();
        TFsEmailUiUtility::CreatePlainTextPartL( 
                *iNewMessage, iNewMessageTextPart );
        InitFieldsL();
        IncludeMessageTextL();
        AttachmentsListControl()->Model()->Clear();
        GetAttachmentsFromMailL();
        SetAttachmentLabelContentL();
        iAttachmentListSaveDraft = ETrue;

        // Close the "Opening" wait note if it was shown
        if ( waitNoteId != KErrNotFound )
            {
            CleanupStack::PopAndDestroy( (TAny*)waitNoteId );
            }
        }
    else if ( aCustomMessageId == TUid::Uid( KEditorCmdInternalMailto ) )
        {
        FUNC_LOG;

        iOrigMessage = NULL;
        iFakeSyncGoingOn = ETrue;
        iNewMessage = iMailBox->CreateMessageToSend();
        iFakeSyncGoingOn = EFalse;
        if ( !iNewMessage )
            {
            User::Leave( KErrGeneral );
            }
        iNewMessage->SetContentType( KFSMailContentTypeMultipartMixed );
        iNewMessage->SaveMessageL();

        TFsEmailUiUtility::CreatePlainTextPartL( 
                *iNewMessage, iNewMessageTextPart );

        IncludeMessageTextL( ETrue );

        CFSMailAddress* toAddress = 
            static_cast<CFSMailAddress*>( iLaunchParams.iExtra ); // not owned
        RPointerArray<CNcsEmailAddressObject> toRecipients;
        CleanupResetAndDestroyClosePushL( toRecipients );
        CNcsEmailAddressObject* ncsToAddress = 
            NcsUtility::CreateNcsAddressL( *toAddress );
        CleanupStack::PushL( ncsToAddress );
        toRecipients.AppendL( ncsToAddress );
        CleanupStack::Pop( ncsToAddress ); // owned by toRecipients now
        iContainer->SetToFieldAddressesL( toRecipients );
        CleanupStack::PopAndDestroy( &toRecipients );

        iContainer->SetFocusToMessageFieldL();
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::DoQuickTextL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeView::DoQuickTextL()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iContainer, Panic( ENcsBasicUi ) );

	HBufC* title = 
        iEikonEnv->AllocReadResourceLC( R_NCS_QUICK_TEXT_TITLE_TEXT );
	HBufC* buf( 0 );
	TRAPD( err, buf = CNotepadApi::FetchTemplateL( title ) );
	if ( err == KLeaveExit )
	    { 
		// If end key was pressed the dialog leaves with the above error code.
	    // In that case we must leave as well or the application will become
	    // stuck as the exit will be incomplete.
	    User::Leave( err );
	    }
		
	if ( buf && err == KErrNone ) // Safety check, 0 if user cancel
		{
		CleanupStack::PushL( buf );
		iContainer->AddQuickTextL( *buf );
		CleanupStack::PopAndDestroy( buf );
		}
	CleanupStack::PopAndDestroy( title );

    }

// -----------------------------------------------------------------------------
// CNcsComposeView::HandleDynamicVariantSwitchL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeView::HandleDynamicVariantSwitchL(
    CFsEmailUiViewBase::TDynamicSwitchType aType )
	{
    FUNC_LOG;

    if ( iFirstStartCompleted && iContainer )
        {
        if ( aType == CFsEmailUiViewBase::EScreenLayoutChanged )
            {
            iContainer->HandleLayoutChangeL();
            }
        else if ( aType == CFsEmailUiViewBase::ESkinChanged )
            {
            iContainer->HandleSkinChangeL();
            }
        else
            {
            }
        }

	}

// -----------------------------------------------------------------------------
// CNcsComposeView::InitReplyFieldsL()
// Initialises the reply fields from the reply message created by the plug-in.
// -----------------------------------------------------------------------------
//
void CNcsComposeView::InitReplyFieldsL( TBool /*aReplyAll*/ )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iContainer, Panic( ENcsBasicUi ) );

    // Get TO recipients
    RPointerArray<CFSMailAddress>& toRecipients = 
        iNewMessage->GetToRecipients();
    RPointerArray<CNcsEmailAddressObject> recipients;
    CleanupResetAndDestroyClosePushL( recipients );
    NcsUtility::ConvertAddressArrayL( toRecipients, recipients );
    iContainer->SetToFieldAddressesL( recipients );
    CleanupStack::PopAndDestroy( &recipients );

    // Get CC recipients
    RPointerArray<CFSMailAddress>& ccRecipients = 
        iNewMessage->GetCCRecipients();
    NcsUtility::ConvertAddressArrayL( ccRecipients, recipients );
    iContainer->SetCcFieldAddressesL( recipients );
    recipients.ResetAndDestroy();

    // Get BCC recipients
    RPointerArray<CFSMailAddress>& bccRecipients = 
        iNewMessage->GetBCCRecipients();
    NcsUtility::ConvertAddressArrayL( bccRecipients, recipients );
    iContainer->SetBccFieldAddressesL( recipients );
    recipients.ResetAndDestroy();

    // Get subject line from original message
    HBufC* prefix = StringLoader::LoadLC( R_NCS_ENGINE_EMAIL_REPLY_PREFIX );
    HBufC* formattedSubject = NcsUtility::FormatSubjectLineL(
        iOrigMessage->GetSubject(), *prefix );
    CleanupStack::PushL( formattedSubject );
    iContainer->SetSubjectL( *formattedSubject );
    CleanupStack::PopAndDestroy( formattedSubject );
    CleanupStack::PopAndDestroy( prefix );

    // clear attachments
    AttachmentsListControl()->Model()->Clear();

    }

// -----------------------------------------------------------------------------
// CNcsComposeView::RemoveOwnAddress()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeView::RemoveOwnAddress( 
        RPointerArray<CNcsEmailAddressObject>& aAddressList )
    {
    FUNC_LOG;
    TInt index = 0;
    TDesC& ownAddress( iMailBox->OwnMailAddress().GetEmailAddress() );
	while ( index < aAddressList.Count() )
        {
        if ( !aAddressList[index]->EmailAddress().CompareC(ownAddress) )
            {
            // The entry will be removed from the arry.
            aAddressList.Remove(index);
            break;
            }
        index++;
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::IncludeMessageTextL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeView::IncludeMessageTextL( 
        TBool aEnsureSpaceInBegin /*= EFalse*/ )
	{
    FUNC_LOG;

    __ASSERT_DEBUG( iContainer, Panic( ENcsBasicUi ) );

    HBufC* body = NULL;
    HBufC* rawBody = GetMessageBodyL();
    CleanupStack::PushL( rawBody );

    // Ensure there's free space in the beginning of the message if required
    if ( aEnsureSpaceInBegin && rawBody->Length() )
        {
        TText firstChar = (*rawBody)[0];
        _LIT( KNewLines, "\r\n\x2028\x2029" );
        if ( KNewLines().Locate( firstChar ) == KErrNotFound )
            {
            // First character is not a new line character. Insert one.
            body = HBufC::NewL( rawBody->Length() + KIMSLineFeed().Length() );
            TPtr ptr = body->Des();
            ptr.Append( KIMSLineFeed );
            ptr.Append( *rawBody );
            CleanupStack::PopAndDestroy( rawBody );
            rawBody = NULL;
            CleanupStack::PushL( body );
            }
        }
    // If no modifications were needed, then just set body pointer to point
    // the rawBody
    if ( !body )
        {
        body = rawBody;
        rawBody = NULL;
        }
    // Now we have possibly decorated message text in body pointer and
    // in cleanup stack

    // Divide the contents into normal body and the read-only quote fields
    // Convert bytes length to words length
    TInt readOnlyLength = iNewMessageTextPart->ReadOnlyPartSize() / 2;
    if ( body )
    	{
		TInt modifiableLength = body->Length() - readOnlyLength;
	
		// Remove one newline from the end of the modifiable body if there's
		// read-only quote present. This is because the field boundary appears
		// as newline on the screen. This newline is added back when saving
		// the message.
		TInt lfLength = KIMSLineFeed().Length();
		if ( readOnlyLength && modifiableLength >= lfLength &&
			 body->Mid( modifiableLength-lfLength, lfLength ) == 
			 KIMSLineFeed )
			{
			modifiableLength -= lfLength;
			}
	
		iContainer->SetBodyContentL( body->Left( modifiableLength ),
			body->Right( readOnlyLength ) );
    	}

    CleanupStack::PopAndDestroy( body );
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::InitForwardFieldsL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeView::InitForwardFieldsL()
	{
    FUNC_LOG;

    __ASSERT_DEBUG( iContainer, Panic( ENcsBasicUi ) );

	// set subject
	HBufC* prefix = StringLoader::LoadLC( R_NCS_ENGINE_EMAIL_FORWARD_PREFIX );
	HBufC* formattedSubject = NcsUtility::FormatSubjectLineL( 
	        iOrigMessage->GetSubject(), *prefix );
	CleanupStack::PushL( formattedSubject );
	iContainer->SetSubjectL( *formattedSubject );
	CleanupStack::PopAndDestroy( formattedSubject );
	CleanupStack::PopAndDestroy( prefix );

    // attachments
	AttachmentsListControl()->Model()->Clear();
	// <cmail>
	TInt error = KErrNone;
	TRAP( error, GetAttachmentsFromMailL() );
	// </cmail>
	SetAttachmentLabelContentL();

	}

// -----------------------------------------------------------------------------
// CNcsComposeView::SetPriority()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeView::SetPriority()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iContainer, Panic( ENcsBasicUi ) );

    if ( iNewMessage && iNewMessage->IsFlagSet( EFSMsgFlag_Important ) )
        {
        iStatusPaneIndicators->SetPriorityFlag( EMsgPriorityHigh );
        }
    else if ( iNewMessage && iNewMessage->IsFlagSet( EFSMsgFlag_Low ) )
        {
        iStatusPaneIndicators->SetPriorityFlag( EMsgPriorityLow );
        }
    }

// -----------------------------------------------------------------------------
// Sets follow-up flags to status pane indicators
// -----------------------------------------------------------------------------
//
void CNcsComposeView::SetFollowUp()
    {
    if ( iNewMessage && iStatusPaneIndicators )
        {
        if ( iNewMessage->IsFlagSet( EFSMsgFlag_FollowUp ) )
            {
            iStatusPaneIndicators->SetFollowUpFlag(
                CCustomStatuspaneIndicators::EFollowUp );
            }
        else if ( iNewMessage->IsFlagSet( EFSMsgFlag_FollowUpComplete ) )
            {
            iStatusPaneIndicators->SetFollowUpFlag(
                CCustomStatuspaneIndicators::EFollowUpComplete );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::InitFieldsL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeView::InitFieldsL()
	{
    FUNC_LOG;

    __ASSERT_DEBUG( iContainer, Panic( ENcsBasicUi ) );

	// to
 	RPointerArray<CFSMailAddress>& toRecipients = 
        iNewMessage->GetToRecipients();
	RPointerArray<CNcsEmailAddressObject> ncsToRecipients;
	CleanupResetAndDestroyClosePushL( ncsToRecipients );
	NcsUtility::ConvertAddressArrayL( toRecipients, ncsToRecipients );
 	iContainer->SetToFieldAddressesL( ncsToRecipients );
    CleanupStack::PopAndDestroy( &ncsToRecipients );

	// cc
 	RPointerArray<CFSMailAddress>& ccRecipients = 
        iNewMessage->GetCCRecipients();
	RPointerArray<CNcsEmailAddressObject> ncsCcRecipients;
    CleanupResetAndDestroyClosePushL( ncsCcRecipients );
	NcsUtility::ConvertAddressArrayL( ccRecipients, ncsCcRecipients );
 	iContainer->SetCcFieldAddressesL( ncsCcRecipients );
    CleanupStack::PopAndDestroy( &ncsCcRecipients );

	// bcc
 	RPointerArray<CFSMailAddress>& bccRecipients = 
        iNewMessage->GetBCCRecipients();
	RPointerArray<CNcsEmailAddressObject> ncsBccRecipients;
    CleanupResetAndDestroyClosePushL( ncsBccRecipients );
	NcsUtility::ConvertAddressArrayL( bccRecipients, ncsBccRecipients );
 	iContainer->SetBccFieldAddressesL( ncsBccRecipients );
    CleanupStack::PopAndDestroy( &ncsBccRecipients );

	// set subject line
	iContainer->SetSubjectL( iNewMessage->GetSubject() );

	}

// -----------------------------------------------------------------------------
// CNcsComposeView::GetMessageBodyL()
//
// -----------------------------------------------------------------------------
//
HBufC* CNcsComposeView::GetMessageBodyL()
	{
    FUNC_LOG;

    __ASSERT_DEBUG( iNewMessageTextPart, Panic( ENcsBasicUi ) );

    TInt messageSize = iNewMessageTextPart->FetchedContentSize();

    HBufC* data = NULL;

    if ( messageSize > 0 )
        {
        // fetch message body if there is something to fetch
        data = HBufC::NewLC( messageSize );
        TPtr dataPtr = data->Des();
        iNewMessageTextPart->GetContentToBufferL( dataPtr, 0 );
        CleanupStack::Pop( data );
        }
    else
        {
        data = KNullDesC().AllocL();
        }

	return data;
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::CommitL()
//
// -----------------------------------------------------------------------------
//
void CNcsComposeView::CommitL( 
        TBool aParseAddresses, TFieldToCommit aFieldToCommit )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iNewMessage, Panic( ENcsBasicUi ) );
	__ASSERT_DEBUG( iNewMessageTextPart, Panic( ENcsBasicUi ) );

	TBool commitToField = EFalse;
	TBool commitCcField = EFalse;
	TBool commitBccField = EFalse;
	TBool commitSubjectField = EFalse;
	TBool commitBodyField = EFalse;

	switch ( aFieldToCommit )
	    {
	    case EAllFields:
	        commitToField = ETrue;
	        commitCcField = ETrue;
	        commitBccField = ETrue;
	        commitSubjectField = ETrue;
	        commitBodyField = ETrue;
	        break;
	    case EToField:
            commitToField = ETrue;
	        break;
	    case ECcField:
            commitCcField = ETrue;
	        break;
	    case EBccField:
            commitBccField = ETrue;
	        break;
        case EBodyField:
            commitBodyField = ETrue;
            break;
        case ESubjectField:
            commitSubjectField = ETrue;
            break;
	    default:
	        break;
	    }

	if ( commitToField )
		{
	    // get addresses from UI to MSG object
	    RPointerArray<CFSMailAddress> toAddresses;
	    CleanupResetAndDestroyClosePushL( toAddresses );
	    NcsUtility::ConvertAddressArrayL( 
	            iContainer->GetToFieldAddressesL( aParseAddresses ), 
	            toAddresses );
	    iNewMessage->GetToRecipients().ResetAndDestroy();
	    for ( TInt i = 0 ; i < toAddresses.Count() ; i++ )
	    	{
	    	iNewMessage->AppendToRecipient( toAddresses[i] );
	        // Ownership of the message pointer was transferred from our array
	    	// to iNewMessage
	        toAddresses[i] = NULL;
	    	}
	    CleanupStack::PopAndDestroy( &toAddresses );
		}

	if ( commitCcField )
		{
	    RPointerArray<CFSMailAddress> ccAddresses;
	    CleanupResetAndDestroyClosePushL( ccAddresses );
	    NcsUtility::ConvertAddressArrayL( 
	            iContainer->GetCcFieldAddressesL( aParseAddresses ), 
	            ccAddresses );
	    iNewMessage->GetCCRecipients().ResetAndDestroy();
	    for ( TInt i = 0 ; i < ccAddresses.Count() ; i++ )
	    	{
	    	iNewMessage->AppendCCRecipient( ccAddresses[i] );
	        // Ownership of the message pointer was transferred from 
	    	// our array to iNewMessage
	        ccAddresses[i] = NULL;
	    	}
	    CleanupStack::PopAndDestroy( &ccAddresses );
		}

	if ( commitBccField )
		{
	    RPointerArray<CFSMailAddress> bccAddresses;
	    CleanupResetAndDestroyClosePushL( bccAddresses );
	    NcsUtility::ConvertAddressArrayL( 
	            iContainer->GetBccFieldAddressesL( aParseAddresses ), 
	            bccAddresses );
	    iNewMessage->GetBCCRecipients().ResetAndDestroy();
	    for ( TInt i = 0; i < bccAddresses.Count() ; i++ )
	    	{
	    	iNewMessage->AppendBCCRecipient( bccAddresses[i] );
	        // Ownership of the message pointer was transferred from 
	    	// our array to iNewMessage
	        bccAddresses[i] = NULL;
	    	}
	    CleanupStack::PopAndDestroy( &bccAddresses );
		}

	if ( commitSubjectField )
	    {
	    // get subject from UI to MSG object
	    HBufC* subject = iContainer->GetSubjectLC();
	    TPtr ptr = subject->Des();
		// replace new line characters with spaces as Subject is normally
	    // one line only
	    AknTextUtils::ReplaceCharacters( 
	            ptr, KLineSeparators, KReplacementChar );
	    iNewMessage->SetSubject( *subject );
	    CleanupStack::PopAndDestroy( subject );
	    }

    if ( commitBodyField )
        {
        HBufC* body = iContainer->GetBodyContentLC();
        TPtr bodyPtr = body->Des(); // this TPtr is needed only because of
        // incorrect argument type in FW API, can be removed when API fixed
        iNewMessageTextPart->SetContent( bodyPtr );
        CleanupStack::PopAndDestroy( body );

        TInt error = KErrNone;
        if ( iNewMessageTextPart )
            {
            TRAP( error, iNewMessageTextPart->SaveL() );
            }
        }

	// Save message if at least one field committed
    if ( iNewMessage || commitToField || commitCcField || commitBccField
        || commitSubjectField || commitBodyField )
        {
        TInt error = KErrNone;
        if(iNewMessage) // Coverity error fix:
            {
            TRAP( error, iNewMessage->SaveMessageL() );
            }
        RefreshToolbar();
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::DoSafeExit
// -----------------------------------------------------------------------------
void CNcsComposeView::DoSafeExit( TExitMode aMode )
    {
    FUNC_LOG;

    if ( !iExecutingDoExitL )
        {
        iExecutingDoExitL = ETrue;
        TRAP_IGNORE( DoExitL( aMode ) );
        iExecutingDoExitL = EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CNcsComposeView::DoExitL
// -----------------------------------------------------------------------------
void CNcsComposeView::DoExitL( TExitMode aMode )
    {
    FUNC_LOG;

    TBool emptyMessage = ETrue;

    if ( iAutoSaver && iContainer )
        {
        iAutoSaver->Enable( EFalse );

        // check for some user input in some header field or in message body
    	TInt attaCount = AttachmentsListControl()->Model()->Count();
        emptyMessage =  iContainer->AreAddressFieldsEmpty() &&
            iContainer->IsSubjectFieldEmpty() &&
            iContainer->GetMessageFieldLength() < 1 &&
            attaCount <= 0;
	    }

    // Consider saving the draft if the message is not completely empty
    if ( !emptyMessage )
        {
        if ( aMode == ESaveDraftQuery && !iMailSent && !iMailSendFailed )
            {
            TBool actionSucceeded = EFalse;

            // DoSaveDraftL returns EFalse if the user query was cancelled
            // or saving to Drafts failed
            TRAPD( saveDraftError, actionSucceeded = DoSaveDraftL( ETrue ) );
            if ( saveDraftError == KErrNone )
                {
                if ( !actionSucceeded )
                    {
                    // return to normal action 
                    // (cancel was pressed in the query)
                    iAutoSaver->Enable( ETrue );
                    return;
                    }
                }
            else
                {
                }
            }
        else if ( aMode == ESaveDraft )
            {
            DoSaveDraftL( EFalse );
            }
        }
    else if ( iNewMessage ) // message is empty 
        {
        // delete the draft if it has been already saved earlier
        TInt err = NcsUtility::DeleteMessage( iMailClient, iMailBox->GetId(),
            iNewMessage->GetFolderId(), iNewMessage->GetMessageId() );
        if ( !err && iMailBox->HasCapability( 
                EFSMBoxCapaSupportsDeletedItemsFolder ) )
            {
            err = NcsUtility::DeleteMessage( iMailClient, iMailBox->GetId(),
                iMailBox->GetStandardFolderId( EFSDeleted ),
                iNewMessage->GetMessageId() );
            }

        if ( !err && !iAppUi.AppUiExitOngoing() )
            {
            // Simulate delete event so that drafts folder is
            // updated correctly.
            RArray<TFSMailMsgId> messageIds;
            CleanupClosePushL( messageIds );
            messageIds.AppendL( iNewMessage->GetMessageId() );
            TFSMailMsgId folderId = iNewMessage->GetFolderId();
            TFSMailMsgId mailboxId = iNewMessage->GetMailBoxId();
            iAppUi.EventL( TFSEventMailDeleted,
                mailboxId, &messageIds, &folderId, NULL );
            CleanupStack::PopAndDestroy( &messageIds );
            }
        }

    // Clear attachment control now that message has been sent
    // othewise this will kind of leak memory by leaving attachments
    // in the list, even if they have been sent.
    CFreestyleEmailUiSendAttachmentsListControl* attachmentControl;
    attachmentControl = AttachmentsListControl();
    if ( attachmentControl && attachmentControl->Model() )
        {
        attachmentControl->Model()->Clear();
        }
    ExitComposer();
    TIMESTAMP( "Editor exited" );
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::DoOpenAttachmentList
// Opens the currently focused attachment
// -----------------------------------------------------------------------------
void CNcsComposeView::DoOpenAttachmentListL()
	{
    FUNC_LOG;

    CFreestyleEmailUiSendAttachmentsListControl* attachmentControl
   		= AttachmentsListControl();

    TInt index( iContainer ? iContainer->FocusedAttachmentLabelIndex() 
            : KNoAttachmentLabelFocused );
    if ( KNoAttachmentLabelFocused != index && 
         attachmentControl && attachmentControl->Model() )
        {
        CFSEmailUiSendAttachmentsListModelItem* item =
            static_cast<CFSEmailUiSendAttachmentsListModelItem*>(
                    attachmentControl->Model()->Item( index ) );
            
        if ( item && !item->IsRemote() ) // cannot open remote attachments
            {
            CFSMailMessagePart* msgPart = 
                iNewMessage->ChildPartL( item->MailMsgPartId() );
            CleanupStack::PushL( msgPart );
            TFsEmailUiUtility::OpenAttachmentL( *msgPart );
            CleanupStack::PopAndDestroy( msgPart );
            }
        }
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::LaunchStylusPopupMenu
// Show stylus popup menu for the attachments line
// -----------------------------------------------------------------------------
//
void CNcsComposeView::LaunchStylusPopupMenu( const TPoint& aPenEventScreenLocation )
    {
    FUNC_LOG;
    
    CFSEmailUiSendAttachmentsListModel* attachmentModel =
        ( AttachmentsListControl() ? 
                AttachmentsListControl()->Model() : NULL ); 
    TInt count( attachmentModel ? attachmentModel->Count() : 0 );
    TBool attachmentsInModel( count > 0 );
    CFSEmailUiSendAttachmentsListModelItem* item( NULL );
   
    if ( attachmentsInModel )
        {
        item = static_cast<CFSEmailUiSendAttachmentsListModelItem*>( 
                attachmentModel->Item( 
                    iContainer->FocusedAttachmentLabelIndex() ) );
        if ( !item || item->IsRemote() )
            {
            // Only non-remote attachments can be opened
            iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdOpenAttachment, ETrue );
            }
        }

    if ( !item || item->IsReadOnly() )
        {
        // Only non-readonly attachments can be removed
        iStylusPopUpMenu->SetItemDimmed( EFsEmailUiCmdRemoveAttachment, ETrue );
        }

    // Set the position for the popup
	iStylusPopUpMenu->SetPosition( aPenEventScreenLocation );

	// Display the popup.
	iStylusPopUpMenu->ShowMenu();
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::GetAttachmentsFromMail
// -----------------------------------------------------------------------------
void CNcsComposeView::GetAttachmentsFromMailL()
	{
    FUNC_LOG;

    TInt error( KErrNone );

	RPointerArray<CFSMailMessagePart> attachments;
    CleanupResetAndDestroyClosePushL( attachments );
	TRAP( error, iNewMessage->AttachmentListL( attachments ) );
	User::LeaveIfError( error );

	for ( TInt i=0; i<attachments.Count(); ++i )
		{
        TFileType fileType = TFsEmailUiUtility::GetFileType( 
                attachments[i]->AttachmentNameL(),
                attachments[i]->GetContentType() );

        TBool isReadOnly = ( attachments[i]->ReadOnlyPartSize() > 0 );
        TBool isRemote = EFalse;

		if ( attachments[i]->FetchedContentSize() < 
		     attachments[i]->ContentSize() )
			{
			if ( iMailBox->HasCapability( EFSMBoxCapaSmartForward ) )
			    {
			    isRemote = ETrue;
			    }
			else
			    {
	            __ASSERT_DEBUG( EFalse, Panic(EFSEmailUiUnexpectedValue) );
	            User::Leave( KErrNotFound );
	            continue;
			    }
			}

		AttachmentsListControl()->AppendFileToModelL(
		        attachments[i]->GetPartId(),
		        attachments[i]->AttachmentNameL(),
		        attachments[i]->ContentSize(),
		        fileType,
		        isReadOnly,
		        isRemote );
		}

	CleanupStack::PopAndDestroy( &attachments );

	}

// -----------------------------------------------------------------------------
// CNcsComposeView::FileExistsInModel
// -----------------------------------------------------------------------------
TBool CNcsComposeView::FileExistsInModel( TFSMailMsgId aAttachmentId )
	{
    FUNC_LOG;

	TInt count( AttachmentsListControl()->Model()->Count() );

	for ( TInt i=0; i < count; ++i )
		{
		CFSEmailUiSendAttachmentsListModelItem* item =
			static_cast<CFSEmailUiSendAttachmentsListModelItem*>(
                AttachmentsListControl()->Model()->Item(i) );
		if ( aAttachmentId.Id() == item->MailMsgPartId().Id() )
			{
			return ETrue;
			}
		}

	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::GenerateReplyHeaderLC
// -----------------------------------------------------------------------------
HBufC* CNcsComposeView::GenerateReplyHeaderLC()
	{
    FUNC_LOG;

    __ASSERT_DEBUG( iOrigMessage, Panic( ENcsBasicUi ) );

	// separator line
	HBufC* separator = 
        StringLoader::LoadLC( R_NCS_ENGINE_EMAIL_MESSAGE_LINE_SEPARATOR ); //1

	// from line
	RPointerArray<CNcsEmailAddressObject> senderArray;
	CleanupResetAndDestroyClosePushL( senderArray );
    CFSMailAddress* sender = iOrigMessage->GetSender();
    if ( sender )
        {
    	CNcsEmailAddressObject* ncsSender = 
            NcsUtility::CreateNcsAddressL( *sender );
    	CleanupStack::PushL( ncsSender );
	    senderArray.AppendL( ncsSender );
	    CleanupStack::Pop( ncsSender ); // now owned by senderArray
        }
	HBufC* fromLine = 
        NcsUtility::GenerateFromLineToMessageBodyL( senderArray );
	CleanupStack::PopAndDestroy( &senderArray );
	CleanupStack::PushL( fromLine ); //2

	// sent line
	HBufC* sentLine = 
        NcsUtility::GenerateSentLineToMessageBodyL( *iOrigMessage );
	CleanupStack::PushL( sentLine ); //3

	// to line
	RPointerArray<CNcsEmailAddressObject> ncsToRecipients;
	CleanupResetAndDestroyClosePushL( ncsToRecipients );
	RPointerArray<CFSMailAddress>& toRecipients = 
        iOrigMessage->GetToRecipients();
	NcsUtility::ConvertAddressArrayL( toRecipients, ncsToRecipients );
	HBufC* toLine = NcsUtility::GenerateAddressLineToMessageBodyL( 
                ncsToRecipients, NcsUtility::ERecipientTypeTo );
    CleanupStack::PopAndDestroy( &ncsToRecipients );
	CleanupStack::PushL( toLine ); //4

	// cc line
	RPointerArray<CFSMailAddress>& ccRecipients = 
        iOrigMessage->GetCCRecipients();
	TBool hasCcLine = ( ccRecipients.Count() > 0 );
    HBufC* ccLine = NULL;
	if ( hasCcLine )
		{
	    RPointerArray<CNcsEmailAddressObject> ncsCcRecipients;
	    CleanupResetAndDestroyClosePushL( ncsCcRecipients );
		NcsUtility::ConvertAddressArrayL( ccRecipients, ncsCcRecipients );
		ccLine = NcsUtility::GenerateAddressLineToMessageBodyL( 
		        ncsCcRecipients, NcsUtility::ERecipientTypeCc );
	    CleanupStack::PopAndDestroy( &ncsCcRecipients );
		CleanupStack::PushL( ccLine ); //5
		}

	// subject line
	HBufC* subjectLine = NcsUtility::GenerateSubjectLineToMessageBodyL( 
	        iOrigMessage->GetSubject() );
	CleanupStack::PushL( subjectLine ); //6

    // Body
    HBufC* body = NULL;
    TBool hasBody = EFalse;
    CFSMailMessagePart* textBodyPart = iOrigMessage->PlainTextBodyPartL();
    if ( textBodyPart )
        {
        // Plain text body part present, no need
        // to generate it from HTML body part
        delete textBodyPart;
        }
    else
        {
        // Generate body part for reply
        CFSMailMessagePart* htmlBodyPart = iOrigMessage->HtmlBodyPartL();
        if ( htmlBodyPart )
            {
            CleanupStack::PushL( htmlBodyPart );

            HBufC* htmlData = 
                HBufC::NewLC( htmlBodyPart->FetchedContentSize() );
            TPtr pointer = htmlData->Des();
            htmlBodyPart->GetContentToBufferL( pointer, 0 );

            body = TFsEmailUiUtility::ConvertHtmlToTxtL( *htmlData );

            CleanupStack::PopAndDestroy( htmlData );
            CleanupStack::PopAndDestroy( htmlBodyPart );

            CleanupStack::PushL( body ); //7

            hasBody = ETrue;
            }
        }
	// calculate total length
	TInt length = 0;
	length += separator->Length();
	length += KIMSLineFeed().Length();
	length += fromLine->Length();
	length += KIMSLineFeed().Length();
	length += sentLine->Length();
	length += KIMSLineFeed().Length();
	length += toLine->Length();
	if ( hasCcLine )
		{
		length += KIMSLineFeed().Length();
		length += ccLine->Length();
		}
	length += KIMSLineFeed().Length();
	length += subjectLine->Length();
	length += KIMSLineFeed().Length();
    if ( hasBody )
        {
        length += body->Length();
        }
	length += KIMSLineFeed().Length();
	length += KIMSLineFeed().Length();

	// create buffer and write contents
	HBufC* header = HBufC::NewL( length );
	TPtr ptr = header->Des();
	ptr.Append( *separator );
	ptr.Append( KIMSLineFeed );
	ptr.Append( *fromLine );
	ptr.Append( KIMSLineFeed );
	ptr.Append( *sentLine );
	ptr.Append( KIMSLineFeed );
	ptr.Append( *toLine );
	if ( hasCcLine )
		{
		ptr.Append( KIMSLineFeed );
		ptr.Append( *ccLine );
		}
	ptr.Append( KIMSLineFeed );
	ptr.Append( *subjectLine );
	ptr.Append( KIMSLineFeed );
    if ( hasBody )
        {
        ptr.Append( *body );
        }
	ptr.Append( KIMSLineFeed );
	ptr.Append( KIMSLineFeed );

	// leave header on the cleanup stack
    if ( hasBody )
        {
        CleanupStack::PopAndDestroy( body ); //-7
        }
	CleanupStack::PopAndDestroy( subjectLine ); //-6
	if ( hasCcLine )
		{
		CleanupStack::PopAndDestroy( ccLine ); //-5
		}
	CleanupStack::PopAndDestroy( toLine ); //-4
	CleanupStack::PopAndDestroy( sentLine ); //-3
	CleanupStack::PopAndDestroy( fromLine ); //-2
	CleanupStack::PopAndDestroy( separator ); //-1

	CleanupStack::PushL( header );

	return header;
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::GenerateSmartTagLC
// -----------------------------------------------------------------------------
HBufC* CNcsComposeView::GenerateSmartTagLC()
	{
    FUNC_LOG;

	HBufC* tag = StringLoader::LoadLC( R_FSE_EDITOR_SMART_TAG );

	return tag;
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::InitReplyOrForwardUiL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::InitReplyOrForwardUiL()
    {
    FUNC_LOG;
    // Show "Opening" wait note if the message body is large
    TInt waitNoteId = KErrNotFound;
    if ( TFsEmailUiUtility::IsMessageBodyLargeL(iOrigMessage) )
        {
        waitNoteId = TFsEmailUiUtility::ShowGlobalWaitNoteLC( 
                R_FSE_WAIT_OPENING_TEXT );
        }

    if ( iCustomMessageId == TUid::Uid( KEditorCmdReply ) )
        {
        GenerateReplyMessageL( EFalse );
        InitReplyUiL( EFalse );
        }
    else if ( iCustomMessageId == TUid::Uid( KEditorCmdReplyAll ) )
        {
        GenerateReplyMessageL( ETrue );
        InitReplyUiL( ETrue );
        }
    else if ( iCustomMessageId == TUid::Uid( KEditorCmdForward ) )
        {
        GenerateForwardMessageL();
        InitForwardUiL();
        }
    else
        {
        // This shouldn't ever happen. Panic in debug builds.
        ASSERT( EFalse );
        }

    // Close the "Opening" wait note if it was shown
    if ( waitNoteId != KErrNotFound )
        {
        CleanupStack::PopAndDestroy( (TAny*)waitNoteId );
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::GenerateReplyMessageL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::GenerateReplyMessageL( TBool aReplyAll )
	{
    FUNC_LOG;

    __ASSERT_DEBUG( iMailBox, Panic( ENcsBasicUi ) );

	TReplyForwardParams params;
	params.iHeader = GenerateReplyHeaderLC();
	params.iSmartTag = GenerateSmartTagLC();
	TPckgBuf<TReplyForwardParams> buf( params );
	HBufC* temp = HBufC::NewLC( buf.Length() );
	temp->Des().Copy( buf );

	//<cmail> this is actually a waited async method
	iFakeSyncGoingOn = ETrue;
    iNewMessage = iMailBox->CreateReplyMessage( 
            iLaunchParams.iMsgId, aReplyAll, *temp );
    iFakeSyncGoingOn = EFalse;
    //</cmail>
   	// Compose screen does not send cal messages, make sure
	// that cal event flag is not left active by some protocol
	if ( iNewMessage )
		{
		iNewMessage->ResetFlag( EFSMsgFlag_CalendarMsg );
		}
    CleanupStack::PopAndDestroy( temp );
    CleanupStack::PopAndDestroy( params.iSmartTag );
    CleanupStack::PopAndDestroy( params.iHeader );
    if ( !iNewMessage )
    	{
    	User::Leave( KErrGeneral );
    	}
    iFakeSyncGoingOn = ETrue; //<cmail>
    TRAPD( error, TFsEmailUiUtility::MoveMessageToDraftsL( 
            *iMailBox, *iNewMessage ) );
    iFakeSyncGoingOn = EFalse; //</cmail>
    User::LeaveIfError( error );

    if ( iNewMessage->GetContentType() != KFSMailContentTypeMultipartMixed )
        {
        iNewMessage->SetContentType( KFSMailContentTypeMultipartMixed );
        iNewMessage->SaveMessageL();
	    }
	
	TFsEmailUiUtility::CreatePlainTextPartL( *iNewMessage, iNewMessageTextPart );

	}

// -----------------------------------------------------------------------------
// CNcsComposeView::GenerateForwardMessageL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::GenerateForwardMessageL()
	{
    FUNC_LOG;

    __ASSERT_DEBUG( iMailBox, Panic( ENcsBasicUi ) );

	TReplyForwardParams params;
	params.iHeader = GenerateReplyHeaderLC();
	params.iSmartTag = GenerateSmartTagLC();
	TPckgBuf<TReplyForwardParams> buf( params );
	HBufC* temp = HBufC::NewLC( buf.Length() );
	temp->Des().Copy( buf );

	//<cmail> this is actually a waited async call
	iFakeSyncGoingOn = ETrue;
    iNewMessage = iMailBox->CreateForwardMessage( iLaunchParams.iMsgId, *temp );
    iFakeSyncGoingOn = EFalse;
    //</cmail>
 	// Compose screen does not send cal messages, make sure
	// that cal event flag is not left active by some protocol
	if ( iNewMessage )
		{
		iNewMessage->ResetFlag( EFSMsgFlag_CalendarMsg );
		}
    CleanupStack::PopAndDestroy( temp );
    CleanupStack::PopAndDestroy( params.iSmartTag );
    CleanupStack::PopAndDestroy( params.iHeader );
    if ( !iNewMessage )
    	{
    	User::Leave( KErrGeneral );
    	}
    iFakeSyncGoingOn = ETrue; //<cmail>
    TRAPD( error, TFsEmailUiUtility::MoveMessageToDraftsL( 
            *iMailBox, *iNewMessage ) );
    iFakeSyncGoingOn = EFalse; //<cmail>
    User::LeaveIfError( error );

    if ( iNewMessage->GetContentType() != KFSMailContentTypeMultipartMixed )
        {
        iNewMessage->SetContentType( KFSMailContentTypeMultipartMixed );
        iNewMessage->SaveMessageL();
	    }
    
	TFsEmailUiUtility::CreatePlainTextPartL( 
	        *iNewMessage, iNewMessageTextPart );

    }

// -----------------------------------------------------------------------------
// CNcsComposeView::InitReplyUiL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::InitReplyUiL( TBool aReplyAll )
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety check
        {
        InitReplyFieldsL( aReplyAll );
        IncludeMessageTextL( ETrue );
        iContainer->SetFocusToMessageFieldL();
        }
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::InitUiGeneralL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::InitUiGeneralL()
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety check
        {
        AppUi()->AddToStackL( iContainer );
        iContainer->SetMenuBar( Cba() );
        iContainer->UpdateScrollBarL();

        // Set title pane text
        const TDesC& mbName = iMailBox->GetName();
        iAppUi.SetTitlePaneTextL( mbName );

        // Set priority flag
        SetPriority();

        // Set follow-up flag
        SetFollowUp();
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::InitForwardUiL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::InitForwardUiL()
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety check
        {
        InitForwardFieldsL();
        IncludeMessageTextL( ETrue );
        iContainer->SetFocusToToField();
        iContainer->SelectAllToFieldTextL();
        }
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::SetReplyForwardFlag
// -----------------------------------------------------------------------------
//
void CNcsComposeView::SetReplyForwardFlagL()
	{
    FUNC_LOG;

	if ( iFirstStartCompleted && iOrigMessage )
		{
		if ( iCustomMessageId == TUid::Uid( KEditorCmdReply ) ||
             iCustomMessageId == TUid::Uid( KEditorCmdReplyAll ) )
			{
			iOrigMessage->SetFlag( EFSMsgFlag_Answered );
			iOrigMessage->SaveMessageL();
			}
		else if ( iCustomMessageId == TUid::Uid( KEditorCmdForward ) )
			{
			iOrigMessage->SetFlag( EFSMsgFlag_Forwarded );
			iOrigMessage->SaveMessageL();
			}
		}

	}

// -----------------------------------------------------------------------------
// CNcsComposeView::MailBoxSupportsSmartReply
// -----------------------------------------------------------------------------
//
TBool CNcsComposeView::MailBoxSupportsSmartReply()
	{
    FUNC_LOG;

    __ASSERT_DEBUG( iMailBox, Panic( ENcsBasicUi ) );

	return iMailBox->HasCapability( EFSMBoxCapaSmartReply );
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::MailBoxSupportsSmartForward
// -----------------------------------------------------------------------------
//
TBool CNcsComposeView::MailBoxSupportsSmartForward()
	{
    FUNC_LOG;
    __ASSERT_DEBUG( iMailBox, Panic( ENcsBasicUi ) );

	return iMailBox->HasCapability( EFSMBoxCapaSmartForward );
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::HasUnfetchedAttachmentsL
// -----------------------------------------------------------------------------
//
TBool CNcsComposeView::HasUnfetchedAttachmentsL( CFSMailMessage& aMsg )
	{
    FUNC_LOG;

    __ASSERT_DEBUG( &aMsg, Panic( ENcsBasicUi ) );

    TBool ret = EFalse;

	RPointerArray<CFSMailMessagePart> attachments;
	CleanupResetAndDestroyClosePushL( attachments );
	aMsg.AttachmentListL( attachments );
	for ( TInt i=0; i<attachments.Count(); i++ )
		{
		if ( EFSFull != attachments[i]->FetchLoadState() )
			{
			ret = ETrue;
			break;
			}
		}
	CleanupStack::PopAndDestroy( &attachments );

	return ret;
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::HandleActionL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::HandleActionL( const TAlfActionCommand& aActionCommand )
    {
    FUNC_LOG;
    if ( iFirstStartCompleted && iNewMessage && iMailBox 
         && aActionCommand.Id() == KCmdEditorAutoSave )
        {
        CommitL( EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::SaveToDraftsL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::SaveToDraftsL( TBool aParseAddresses )
	{
    FUNC_LOG;
    if ( iFirstStartCompleted ) // Safety
        {
        __ASSERT_DEBUG( iMailBox, Panic( ENcsBasicUi ) );
        __ASSERT_DEBUG( iNewMessage, Panic( ENcsBasicUi ) );

        iFakeSyncGoingOn = ETrue;
        TRAPD( error, CommitL( aParseAddresses ) );
        iFakeSyncGoingOn = EFalse;
        User::LeaveIfError( error );

        iFakeSyncGoingOn = ETrue;
        TRAP( error, TFsEmailUiUtility::MoveMessageToDraftsL(
            *iMailBox, *iNewMessage ) );
        iFakeSyncGoingOn = EFalse;
        User::LeaveIfError( error );

        if ( !iAppUi.AppUiExitOngoing() )
            {
            // Simulate a new mail event to ensure that saved message becomes
            // visible in the mail list. 
            // All protocols do not send event automatically in this case.
            RArray<TFSMailMsgId> messageIdArray;
            CleanupClosePushL( messageIdArray );
            messageIdArray.Append( iNewMessage->GetMessageId() );
            TFSMailMsgId folderId = iNewMessage->GetFolderId();
            TFSMailMsgId mailboxId = iNewMessage->GetMailBoxId();
            iAppUi.EventL(
                TFSEventNewMail, mailboxId, &messageIdArray, &folderId, NULL );
            // Update displayed mail info if the mail was created previously
            iAppUi.EventL(
                TFSEventMailChanged, mailboxId, &messageIdArray, 
                &folderId, NULL );
            CleanupStack::PopAndDestroy( &messageIdArray );
            }
        }
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::FetchLogicComplete
// -----------------------------------------------------------------------------
//
TBool CNcsComposeView::FetchLogicComplete( 
        TComposerFetchState /*aState*/, TInt aError )
	{
    FUNC_LOG;
    TBool result = EFalse;
    if ( iFirstStartCompleted ) // Safety
        {
        if ( !aError )
            {
            TRAP( aError, InitReplyOrForwardUiL() );
            iAutoSaver->Enable( ETrue );
            }
        else
            {
            // something went wrong
            DoSafeExit( ENoSave );
            iMailFetchingErrCode = aError;
            result = ETrue;
            }
        }
    if ( iFetchWaitDialog && !iFetchDialogCancelled )
        {
        TRAP_IGNORE( iFetchWaitDialog->ProcessFinishedL() );
        iFetchWaitDialog = NULL;
        }
    return result;
	}

// -----------------------------------------------------------------------------
// CNcsComposeView::ExitComposer
// -----------------------------------------------------------------------------
//
void CNcsComposeView::ExitComposer()
    {
    FUNC_LOG;

    if ( iStatusPaneIndicators )
        {
        iStatusPaneIndicators->HideStatusPaneIndicators();
        }
    
    // lower flag to indicate that view is no longer 
    // able to handle user commands
    iViewReady = EFalse;

	// view switching and alfred operations should be avoided
	// when Appui exit has been initialised
	if ( !iAppUi.AppUiExitOngoing() )
		{
        // change to previous view
        TRAP_IGNORE( NavigateBackL() );
        }

    // Cleanup message contents to prevent ChildDoDeactivate() 
	// from saving the message to drafts again
    ResetComposer();

    SafeDelete( iFetchLogic );
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::CreateContainerL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::CreateContainerL()
    {
    FUNC_LOG;
    TInt flags = NULL;
    if ( iCrHandler->EditorCCVisible() )
        {
        flags |= CNcsComposeViewContainer::ECcFieldVisible;
        }
    if ( iCrHandler->EditorBCVisible() )
        {
        flags |= CNcsComposeViewContainer::EBccFieldVisible;
        }
    TRect rect = ClientRect();
    iContainer = CNcsComposeViewContainer::NewL( *this, rect, *iMailBox,
        *iAutoSaver, flags );
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::ResetComposer
// -----------------------------------------------------------------------------
//
void CNcsComposeView::ResetComposer()
    {
    FUNC_LOG;

    delete iMailBox;
    iMailBox = NULL;
    delete iOrigMessage;
    iOrigMessage = NULL;
    delete iNewMessageTextPart;
    iNewMessageTextPart = NULL;
    delete iNewMessage;
    iNewMessage = NULL;
    iDlg = NULL;

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }

    }

// -----------------------------------------------------------------------------
// CNcsComposeView::SaveAndCleanPreviousMessage
// -----------------------------------------------------------------------------
//
void CNcsComposeView::SaveAndCleanPreviousMessage()
    {
    FUNC_LOG;

    if ( iFirstStartCompleted && iContainer && iNewMessage && iMailBox )
        {

        TBool noAddrOrNoMesBody = iContainer->AreAddressFieldsEmpty() &&
            iContainer->IsSubjectFieldEmpty() &&
            (iContainer->GetMessageFieldLength() < 1);

        // save to Drafts if there's some user input in some header field 
        // or in message body
        if ( !noAddrOrNoMesBody )
            {
            TRAPD( error, DoSaveDraftL( EFalse ) );
            if ( error )
                {
                }
            }

        // store the message IDs to launch parameters to be able to return 
        // to same message if desired
        iLaunchParams.iMsgId = iNewMessage->GetMessageId();
        iLaunchParams.iFolderId = iNewMessage->GetFolderId();
        iLaunchParams.iMailboxId = iMailBox->GetId();
        }
    ResetComposer();
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::AsyncExit
// -----------------------------------------------------------------------------
//
TInt CNcsComposeView::AsyncExit( TAny* aSelfPtr )
    {
    FUNC_LOG;
    CNcsComposeView* self =
        static_cast<CNcsComposeView*>( aSelfPtr );

    TRAPD( err,self->AsyncExitL() );
    return err;
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::AsyncExitL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::AsyncExitL()
    {
    FUNC_LOG;
    if ( iFakeSyncGoingOn || iExecutingDoExitL )
        {
        // if some sync method is still going on, we continue waiting
        iActiveHelper->Cancel();
        iActiveHelper->Start();
        }
    else
        {
        ExitComposer();
        // for some strange reason composer view is stuck and only option 
        // was to call ProcessCommandL with EAknCmdExit
        ProcessCommandL( EAknCmdExit );
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::HandleContainerChangeRequiringToolbarRefresh
// -----------------------------------------------------------------------------
//
void CNcsComposeView::HandleContainerChangeRequiringToolbarRefresh()
    {
    RefreshToolbar();
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::ShowFetchingWaitNoteL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::ShowFetchingWaitNoteL()
    {
    FUNC_LOG;
    if ( !iAppUi.AppUiExitOngoing() )
        {
        if ( !iFetchWaitDialog )
            {
            iFetchWaitDialog = new (ELeave) CAknWaitDialog(
                reinterpret_cast<CEikDialog**>( &iFetchWaitDialog ), EFalse);
            iFetchWaitDialog->SetCallback( this );
            iFetchDialogCancelled = EFalse;
            iFetchWaitDialog->ExecuteLD( R_FSE_FETCHING_WAIT_DIALOG );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsComposeView::DialogDismissedL
// -----------------------------------------------------------------------------
//
void CNcsComposeView::DialogDismissedL( TInt aButtonId )
    {
    FUNC_LOG;
    if ( aButtonId == EAknSoftkeyCancel && iFetchLogic )
        {
        iFetchDialogCancelled = ETrue;
        iFetchLogic->CancelFetchings();
        }
    }

TBool CNcsComposeView::IsPreparedForExit()
    {
    return !( iFakeSyncGoingOn ||iExecutingDoExitL );
    }

// ---------------------------------------------------------------------------
// Returns the speficied button from the toolbar extension, or NULL, 
// if the button is not found.
// ---------------------------------------------------------------------------
//
CAknButton* CNcsComposeView::Button( TInt aCmdId, 
    CAknToolbarExtension* aExtension )
    {
    CAknButton* button = NULL;

    // Get toolbar extension.
    CAknToolbarExtension* extension = aExtension;
    if ( !aExtension )
        {
        CAknToolbar* toolbar = Toolbar();
        if ( toolbar )
            {
            extension = toolbar->ToolbarExtension();
            }
        }

    // Get specified button from the extension.
    if ( extension )
        {
        button = static_cast<CAknButton*>( extension->ControlOrNull( aCmdId ) );
        }

    return button;
    }

// ---------------------------------------------------------------------------
// CActiveHelper::NewL()
// ---------------------------------------------------------------------------
//
CActiveHelper* CActiveHelper::NewL( CNcsComposeView* aSession )
    {
    CActiveHelper* self = new(ELeave) CActiveHelper( aSession );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CActiveHelper::~CActiveHelper()
// ---------------------------------------------------------------------------
//
CActiveHelper::~CActiveHelper( )
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CActiveHelper::Start()
// ---------------------------------------------------------------------------
//
void CActiveHelper::Start()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CActiveHelper::CActiveHelper()
// ---------------------------------------------------------------------------
//
CActiveHelper::CActiveHelper( CNcsComposeView* aSession ) 
    : CActive( EPriorityLow )
    {
    iComposeView = aSession;
    }

// ---------------------------------------------------------------------------
// CActiveHelper::RunL()
// ---------------------------------------------------------------------------
//
void CActiveHelper::RunL()
    {
    if( iComposeView )
        iComposeView->AsyncExitL();
    }

// ---------------------------------------------------------------------------
// CActiveHelper::DoCancel()
// ---------------------------------------------------------------------------
//
void CActiveHelper::DoCancel()
    {
    // Empty
    }

// ---------------------------------------------------------------------------
// CActiveHelper::ConstructL()
// ---------------------------------------------------------------------------
//
void CActiveHelper::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

