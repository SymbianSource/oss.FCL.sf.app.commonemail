/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ESMR policy implementation
*
*/

#include "cesmrviewerdialog.h"
#include "cesmrpolicy.h"
#include "esmrdef.h"
#include "cesmrpolicymanager.h"
#include "cesmrurlparserplugin.h"
#include "esmricalvieweropcodes.hrh"
#include "tesmrinputparams.h"
#include "cesmrview.h"
#include "esmrgui.hrh"
#include "cesmrfield.h"
#include "cesmrviewerfieldstorage.h"
#include "esmrentryhelper.h"
#include "mesmrmeetingrequestentry.h"
#include "esmrinternaluid.h"
#include "cesmrlistquery.h"
#include "cesmrconfirmationquery.h"
#include "cesmrsendui.h"
#include "cesmrcontactmenuhandler.h"
#include "cesmrfieldbuilderinterface.h"
#include "cesmrtrackingviewdialog.h"
#include "mesmruibase.h"
#include "cesmruifactory.h"
#include "cesmrlocationpluginhandler.h"
#include "cesmrfeaturesettings.h"
#include "esmrconfig.hrh"
#include "mesmrfieldevent.h"
#include "cmrtoolbar.h"
#include "cesmrlistquery.h"
#include "mmrinfoprovider.h"
#include "cmrfocusstrategyviewer.h"
#include "cesmrtitlepanehandler.h"

#include <data_caging_path_literals.hrh>
#include <esmrgui.rsg>
#include <esmrpolicies.rsg>
#include <aknnotewrappers.h>
#include <eikenv.h>
#include <eikappui.h>
#include <avkon.hrh>
#include <MAgnEntryUi.h>
#include <eikmenup.h>
#include <calentry.h>
#include <apgtask.h>
#include <apmstd.h>
#include <apgcli.h>
#include <AknDef.h>
#include <AknUtils.h>
#include <StringLoader.h>
#include <eikspane.h>
#include <e32keys.h>
#include <CalenInterimUtils2.h>
#include <w32std.h>

// DEBUG
#include "emailtrace.h"
#include "cesmrcaldbmgr.h"

/// Unnamed namespace for local definitions
namespace  { // codescanner::namespace

#ifdef _DEBUG

enum TESMRViewerDlgPanic
    {
    EESMRViewerDlgInvalidCommand,
    EESMRViewerDlgnvalidSeriesResult,
    EESMRViewerDlgInvalidEntry
    };

void Panic( TESMRViewerDlgPanic aPanic )
    {
    // Literal for viewer dialog
    _LIT( KESMRViewerDlgPanicTxt, "ESMRViewerDlg" );
    User::Panic( KESMRViewerDlgPanicTxt, aPanic );
    }

#endif

// Definition for default buffer length
const TInt KCalDefaultBufferLen = 40;

// VGA resolution screen size in pixels
const TInt KVGAOneSide( 640 );
const TInt KVGAOtherSide( 480 );

// Calendar command parameter
_LIT16( KCalCmdDATE, "GRAPHICDAY" );

// Definition for time format ("YYYY MM DD HH MM SS MMMMMM" - 26)
_LIT( KCalCmdTimeFormat, "%F %Y %M %D %H %T %S %C" );

// Definition for calendar application UID
const TUid KCalendarAppUID = { KUidCalendarApplication };


/**
 * Sets recurrence modification rule to entry. Rule is queried
 * from the user. Invalid result will cause panic.
 * @param aEntry Reference to meeting request.
 */
void SetRecurrenceModRuleL(
        MESMRCalEntry& aEntry,
        CESMRListQuery::TESMRListQueryType aQueryType )
    {
    FUNC_LOG;
    if ( aEntry.IsRecurrentEventL() &&
         aEntry.IsStoredL() )
        {
        TInt result =
            CESMRListQuery::ExecuteL( aQueryType );

        if( KErrCancel == result )
            {
            // User has cancelled selecting opening the instance
            User::Leave( KErrCancel );
            }

        TESMRThisOccurenceOrSeriesQuery recurrenceModRule =
            static_cast<TESMRThisOccurenceOrSeriesQuery>( result );

        switch( recurrenceModRule )
            {
            case EESMRThisOccurence:
                {
                aEntry.SetModifyingRuleL(
                        MESMRMeetingRequestEntry::EESMRThisOnly );
                break;
                }
            case EESMRSeries:
                {
                aEntry.SetModifyingRuleL(
                        MESMRMeetingRequestEntry::EESMRAllInSeries );
                break;
                }
            default:
            	{
                __ASSERT_DEBUG( EFalse,
                		Panic(EESMRViewerDlgnvalidSeriesResult) );
                break;
            	}
            }
        }
    else
        {
        aEntry.SetModifyingRuleL(
                MESMRMeetingRequestEntry::EESMRThisOnly );
        }
    }
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerDialog::CESMRViewerDialog
// ---------------------------------------------------------------------------
//
CESMRViewerDialog::CESMRViewerDialog(
        MMRInfoProvider& aInfoProvider,
        MAgnEntryUiCallback& aCallback  ) :
    iCallback( aCallback ),
    iInfoProvider( aInfoProvider )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::~CESMRViewerDialog
// ---------------------------------------------------------------------------
//
CESMRViewerDialog::~CESMRViewerDialog()
    {
    FUNC_LOG;
    iESMRStatic.Close();

    if ( iTitlePane )
        {
        // Returns the previous title back to titlepane
        iTitlePane->Rollback();
        delete iTitlePane;
        }

    delete iESMRSendUI;
    delete iLocationPluginHandler;
    delete iFeatures;

    // iView is deleted by framework because it uses the
    // custom control mechanism.
    delete iToolbar;
    delete iFocusStrategy;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRViewerDialog* CESMRViewerDialog::NewL(
        MMRInfoProvider& aInfoProvider,
        MAgnEntryUiCallback& aCallback )
    {
    FUNC_LOG;
    CESMRViewerDialog* self =
        new (ELeave) CESMRViewerDialog( aInfoProvider, aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::ConstructL()
    {
    FUNC_LOG;
    iESMRStatic.ConnectL();

    CAknDialog::ConstructL( R_MRVIEWER_MENUBAR );

    // Class disables avkon toolbar by default, which
    // we want in case of non-MR
    iToolbar = CMRToolbar::NewL();

    TBool responseReady(EFalse);
    MESMRCalEntry* calEntry = iInfoProvider.EntryL();
    MESMRMeetingRequestEntry* mrEntry = NULL;
    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == calEntry->Type() )
        {
        // This is meeting request
        mrEntry = static_cast<MESMRMeetingRequestEntry*>( calEntry );
        responseReady = mrEntry->IsSentL();

        // Change the status pane layout to remove navi pane
        CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
        // If open the MR viewer from Cmail side, the status pane
        // is not visible, we need to show it in viewer dialog
        if ( sp && !sp->IsVisible() )
            {
            sp->MakeVisible( ETrue );
            }
        // Change the type of statuspane to usual_flat.
        if ( sp )
            {
            sp->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT );
            }
        }
    
    TRect clientRect;
    AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane,
            clientRect );

    iESMRSendUI = CESMRSendUI::NewL(EESMRCmdSendAs);

    CESMRViewerFieldStorage* storage =
            CESMRViewerFieldStorage::NewL(
                    iInfoProvider.PolicyProvider().CurrentPolicy(),
                    this,
                    responseReady,
                    *this );

    // Create focus strategy
    iFocusStrategy = CMRFocusStrategyViewer::NewL( *storage );
    // Give the strategy to the iView

    // storage ownership is transferred to CESMRView
    iView = CESMRView::NewL(
            storage,
            *calEntry,
            clientRect,
            *iFocusStrategy,
            *iToolbar,
            this );

    iView->SetViewMode( EESMRViewMR );

    if ( calEntry->Entry().SummaryL().Length() == 0 )
        {
        // if no title, set unnamed text:
        HBufC* title = StringLoader::LoadLC ( R_QTN_MEET_REQ_CONFLICT_UNNAMED,
                                              iCoeEnv );
        iView->SetTitleL( *title );
        CleanupStack::PopAndDestroy( title );
        }
    else
        {
        iView->SetTitleL( calEntry->Entry().SummaryL() );
        }

    TInt titleStringId = -1;
    switch ( calEntry->Type() )
        {
        case MESMRCalEntry::EESMRCalEntryMeetingRequest:
        	{
            titleStringId = R_QTN_MEET_REQ_TITLE_MEETING;
            break;
        	}
        case MESMRCalEntry::EESMRCalEntryMeeting:
        	{
            titleStringId = R_QTN_CALENDAR_TITLE_MEETING;
            break;
        	}
        case MESMRCalEntry::EESMRCalEntryTodo:
        	{
            titleStringId = R_QTN_CALENDAR_TITLE_TODO;
            break;
        	}
        case MESMRCalEntry::EESMRCalEntryMemo:
        	{
            titleStringId = R_QTN_CALENDAR_TITLE_MEMO;
            break;
        	}
        case MESMRCalEntry::EESMRCalEntryAnniversary:
        	{
            titleStringId = R_QTN_CALENDAR_TITLE_ANNIVERSARY;
            break;
        	}
        case MESMRCalEntry::EESMRCalEntryReminder: // Fall through
        default:
        	{
            break;
        	}
        }
    if ( titleStringId != -1 )
        {
        if( !iTitlePane )
            {
            iTitlePane = CESMRTitlePaneHandler::NewL( *iEikonEnv );
            }

        HBufC* titleText = StringLoader::LoadLC( titleStringId, iCoeEnv );
        iTitlePane->SetNewTitle( titleText );
        CleanupStack::PopAndDestroy( titleText );
        }

    iFeatures = CESMRFeatureSettings::NewL();
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ActivateL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::ActivateL()
    {
    FUNC_LOG;
    iView->InternalizeL();
    CAknDialog::ActivateL();

    // Needs to be constructed here
    ConstructToolbarL();

    switch ( iInfoProvider.EntryL()->Type() )
    	{
    	case MESMRCalEntry::EESMRCalEntryMeetingRequest:
    		{
    		iView->InitialScrollL();

            MESMRMeetingRequestEntry* entry =
                static_cast<MESMRMeetingRequestEntry*>(
                        iInfoProvider.EntryL() );

    		if ( entry->IsOpenedFromMail() )
    		    {
                iView->SetNaviArrowStatus(
                                 iCallback.IsCommandAvailable(
                                        EESMRCmdMailPreviousMessage ),
                                 iCallback.IsCommandAvailable(
                                        EESMRCmdMailNextMessage ) );
    		    }

    		}

        case MESMRCalEntry::EESMRCalEntryMeeting:
        case MESMRCalEntry::EESMRCalEntryMemo:
        case MESMRCalEntry::EESMRCalEntryAnniversary:
        case MESMRCalEntry::EESMRCalEntryTodo:
    	case MESMRCalEntry::EESMRCalEntryReminder: // Fall through
    	default:
    		break;
    	}
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::PreLayoutDynInitL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::PreLayoutDynInitL()
    {
    FUNC_LOG;
    SetDefaultMiddleSoftKeyL();
    ButtonGroupContainer().MakeCommandVisibleByPosition(
            CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            ETrue );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRViewerDialog::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType)
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );
    if ( aEvent.iCode != EKeyEscape && !MenuShowing() )
    	{
    	response = iView->OfferKeyEventL( aEvent, aType );
    	}

    if ( response == EKeyWasNotConsumed )
        {
        switch (aEvent.iScanCode)
            {
            case '4':
                {
                response = EKeyWasConsumed;
                ProcessCommandL(EESMRCmdMailPreviousMessage);
                break;
                }
            case '6':
                {
                response = EKeyWasConsumed;
                ProcessCommandL(EESMRCmdMailNextMessage);
                break;
                }
			case EStdKeyDelete:
			case EStdKeyBackspace:
				{
				if( CCalenInterimUtils2::IsMeetingRequestL( 
						iInfoProvider.EntryL()->Entry() ) )
					{
					response = EKeyWasConsumed;
					ProcessCommandL( EESMRCmdDeleteMR );
					}
				else
					{
					response = EKeyWasConsumed;
					ProcessCommandL( EESMRCmdCalEntryUIDelete );
					}
				break;
				}
            default:
                {
                response = CAknDialog::OfferKeyEventL( aEvent, aType );
                break;
                }
            }
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::ProcessCommandL( TInt aCommand )
    {
    FUNC_LOG;

    TRAPD( err, DoProcessCommandL( aCommand ) );
    if ( err != KErrNone &&
         err != KErrCancel &&
         err != KErrArgument )
        {
        User::Leave(err);
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::DoProcessCommandL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::DoProcessCommandL( TInt aCommand )
    {
    FUNC_LOG;
    CAknDialog::ProcessCommandL( aCommand );
    switch ( aCommand )
        {
        case EESMRCmdSendAs:
            {
            SendCalendarEntryL(aCommand);
            break;
            }

        case EESMRCmdSendMR: //Fall through
        case EESMRCmdSendMRUpdate:
            {
            TInt ret( KErrGeneral );
            ret = iCallback.ProcessCommandWithResultL ( aCommand );

            if ( ret == KErrNone )
                {
                TryExitL ( EAknSoftkeyClose );
                }
            break;
            }
        // pass-through all calentryui related command to call back
        case EESMRCmdCalEntryUISend: // Fall through
        case EESMRCmdCalEntryUIAddParticipants: // Fall through
        case EESMRCmdCalEntryUISave: // Fall through
            {
            User::LeaveIfError(
            	iCallback.ProcessCommandWithResultL( aCommand ));
            break;
            }

        // EEikBidCancel is called when Red End key is pressed or
        // application is killed via Swap Window
        case EEikBidCancel: // Fall through
        case EAknSoftkeyClose: // Fall through
        case EAknCmdExit:
        	{
            TryExitL( aCommand );
            break;
        	}
        case EAknSoftkeySelect: // Fall through
            {
            iView->ProcessEditorCommandL( aCommand );
            break;
            }
        case EESMRCmdEdit:              // Fall through
        case EESMRCmdEditLocal:         // Fall through
        case EESMRCmdDeleteMR:          // Fall through
        case EESMRCmdCalEntryUIEdit:    // Fall through
        case EESMRCmdCalEntryUIDelete:
            {
            TBool closeDialog = HandleCommandForEventL( aCommand );
            if ( closeDialog )
                {
                TryExitL( EAknSoftkeyClose );
                }
            break;
            }

        case EESMRCmdReply: // Fall through
        case EESMRCmdReplyAll:
            {
            User::LeaveIfError(
            		iCallback.ProcessCommandWithResultL( aCommand ));
            break;
            }

        case EAknCmdHelp:
            {
            iView->LaunchViewerHelpL();
            break;
            }
        case EESMRCmdViewTrack:
            {
            User::LeaveIfError(
            	iCallback.ProcessCommandWithResultL( aCommand ));
            TryExitL( EAknSoftkeyClose );
            break;
            }
        case EESMRCmdAcceptMR: // Fall through
        case EESMRCmdTentativeMR: // Fall through
        case EESMRCmdDeclineMR: // Fall through
            {
            if ( !UserWantToHandleAttachmentsL() )
                {
                iView->ExternalizeL(); // no forced validation
                TInt res = iCallback.ProcessCommandWithResultL( aCommand );
                if ( res != KErrCancel )
                    {
                    TryExitL( EAknSoftkeyClose );
                    }
                }
            break;
            }
        case EESMRCmdRemoveFromCalendar:
            {
            iView->ExternalizeL(); // no forced validation
            TryExitL( EESMRCmdRemoveFromCalendar );
            break;
            }
        case EESMRCmdViewCalendar:
            {
            OpenInDayModeL();
            break;
            }

            // Email commands
        case EESMRCmdMailDelete:
            {
            TBool executeCmd( CESMRConfirmationQuery::ExecuteL(
                    CESMRConfirmationQuery::EESMRDeleteMR ) );

            if ( executeCmd )
                {
                TInt res = iCallback.ProcessCommandWithResultL( aCommand );
                if ( res != KErrCancel )
                    {
                    TryExitL( EAknSoftkeyClose );
                    }
                }
            break;
            }
        case EESMRCmdClipboardCopy:
            {
            iView->ProcessEditorCommandL( aCommand );
            break;
            }
        case EESMRCmdMailPreviousMessage: // Fall through
        case EESMRCmdMailNextMessage: // Fall through
        case EESMRCmdMailForwardAsMessage: // Fall through
        case EESMRCmdMailMoveMessage: // Fall through
        case EESMRCmdMailCopyMessage: // Fall through
        case EESMRCmdMailMoveMessageToDrafts: // Fall through
        case EESMRCmdMailComposeMessage: // Fall through
        case EESMRCmdMailMessageDetails:
        case EESMRCmdForwardAsMeeting: // Fall through
        case EESMRCmdForwardAsMail:
            {
            if( aCommand == EESMRCmdForwardAsMeeting &&
                    iInfoProvider.EntryL()->Entry().AttachmentCountL() > 0 )
                {
                // Show confirmation note if current mailbox does not
                // support attachments and attachments exist in the entry
                if( !SupportsMailBoxCapabilityL(
                                MESMRBuilderExtension::
                                    EMRCFSSupportsAttachmentsInMR ) )
                    {
                    if ( !CESMRConfirmationQuery::ExecuteL(
                                       CESMRConfirmationQuery::
                                           EESMRAttachmentsNotSupported ) )
                        {
                        break;
                        }
                    }
                }

            if( aCommand == EESMRCmdForwardAsMail &&
            		iInfoProvider.EntryL()->IsRecurrentEventL() )
            	{
				iInfoProvider.EntryL()->SetModifyingRuleL(
				                  MESMRMeetingRequestEntry::EESMRAllInSeries );
            	}

            TInt res = iCallback.ProcessCommandWithResultL( aCommand );
            if ( res != KErrCancel )
                {
                TryExitL( aCommand );
                }
            break;
            }

// <cmail>
        case EESMRCmdDownloadManager:
            {
            TInt res = iCallback.ProcessCommandWithResultL( aCommand );
            if ( res != KErrCancel )
                {
                TryExitL( EAknSoftkeyClose );
                }
// </cmail>
            break;
            }

// <cmail>
        case EESMRCmdOpenAttachment: // Fall through
        case EESMRCmdOpenAttachmentView: // Fall through
        case EESMRCmdDownloadAttachment: // Fall through
        case EESMRCmdDownloadAllAttachments: // Fall through
        case EESMRCmdSaveAttachment: // Fall through
        case EESMRCmdSaveAllAttachments: // Fall through
// </cmail>
            {
            TInt res = iCallback.ProcessCommandWithResultL(
                    aCommand );
            if ( res != KErrCancel )
                {
                iExitTriggered = ETrue;
                }
            break;
            }

        /*
         * Attachment field viewer context menu commands
         */
        case EESMRViewerOpenAttachment:
        case EESMRViewerSaveAttachment:
        case EESMRViewerSaveAllAttachments:
        case EESMRViewerCancelAttachmentDownload:
            {
            iView->ProcessEditorCommandL( aCommand );
            break;
            }

        case EESMRCmdMailFlagMessage: // Fall through
        case EESMRCmdMailMarkUnread: // Fall through
        case EESMRCmdMailMarkRead:
            {
            TInt res = iCallback.ProcessCommandWithResultL( aCommand );
            break;
            }
        case EESMRCmdTodoMarkAsDone: // Fall through
        case EESMRCmdTodoMarkAsNotDone:
            {
            if ( iInfoProvider.EntryL()->Type() == MESMRCalEntry::EESMRCalEntryTodo )
                {
                User::LeaveIfError(
                	iCallback.ProcessCommandWithResultL( aCommand ));
                }
			break;
            }
        case EESMRCmdSearchFromMap:
        case EESMRCmdShowOnMap:
            {
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
                {
                MESMRCalEntry* entry = iInfoProvider.EntryL();
                LocationPluginHandlerL().HandleCommandL( aCommand, *entry );
                iView->SetControlFocusedL( EESMRFieldLocation );
                }
            break;
			}
        case EMRCmdHideAttachmentIndicator:
            {
            iView->ProcessEditorCommandL( EMRCmdHideAttachmentIndicator );
            }
            break;
        default:
        	{
            if ( aCommand >= EESMRCmdActionMenuFirst &&
            	 aCommand < EESMRCmdActionMenuLast )
                {
                iESMRStatic.ContactMenuHandlerL().ExecuteOptionsMenuL(aCommand);
                }
            else if ( aCommand == EESMRCmdActionMenuLast )
                {
                TInt res = iCallback.ProcessCommandWithResultL(
										EESMRCmdOpenAttachmentView );
                if ( res != KErrCancel )
                    {
                    iExitTriggered = ETrue;
                    }
                }
            else
                {
                __ASSERT_DEBUG( EFalse, Panic( EESMRViewerDlgInvalidCommand ) );
                }
            break;
        	}
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDialog::OkToExitL(TInt aButtonId)
    {
    FUNC_LOG;
    TBool res( EFalse );
    // show options
    switch ( aButtonId )
        {
        case EAknSoftkeyOptions:
            {
            CAknDialog::DisplayMenuL();
            break;
            }
        case EAknSoftkeyContextOptions:
            {
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
                {
                SetContextMenu();
                ShowContextMenuL();
                }
            break;
            }
        case EESMRCmdEdit:
        case EESMRCmdEditLocal:
        case EESMRCmdCalEntryUIEdit:
            {
            res = HandleCommandForEventL( aButtonId );
            break;
            }
        case EAknSoftkeySelect:
            {
            iView->ProcessEditorCommandL( aButtonId );

            if ( iExitTriggered )
                {
                res = ETrue;
                }
            break;
            }
// <cmail>
        case EESMRCmdOpenAttachmentView:
        case EESMRCmdMskOpenEmailAttachment:
       	case EESMRCmdOpenAttachment:
        case EESMRCmdDownloadAttachment:
            {
            ProcessCommandL( aButtonId );
            break;
            }
// </cmail>
        case EESMRCmdShowAllAttendees:
        case EESMRCmdShowOnMap:
            {
            iView->ProcessEditorCommandL( aButtonId );
            break;
            }
        case EEikBidCancel:
            {
            res = ETrue;
            break;
            }
        case EAknSoftkeyClose: // Fall through
        case EAknCmdExit: // Fall through
        case EESMRCmdMailPreviousMessage: // Fall through
        case EESMRCmdMailNextMessage: // Fall through
        case EESMRCmdMailForwardAsMessage: // Fall through
        case EESMRCmdMailMoveMessage: // Fall through
        case EESMRCmdMailCopyMessage: // Fall through
        case EESMRCmdMailMoveMessageToDrafts: // Fall through
        case EESMRCmdMailComposeMessage: // Fall through
        case EESMRCmdMailMessageDetails:
        case EESMRCmdRemoveFromCalendar:
            {
            res = HandleMRExitL( aButtonId );
            break;
            }

        default:
            {
            if(iExitTriggered)
                {
                res = ETrue;
                }
            break;
            }
        }

    return res;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::CreateCustomControlL
// ---------------------------------------------------------------------------
//
SEikControlInfo CESMRViewerDialog::CreateCustomControlL( TInt aType ) // codescanner::LFunctionCantLeave
    {
    FUNC_LOG;
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iFlags = 0;
    controlInfo.iTrailerTextId = 0;

    if ( aType == EEsMrViewerType )
        {
        controlInfo.iControl = iView;
        }

    return controlInfo;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ExecuteViewLD
// ---------------------------------------------------------------------------
//
TInt CESMRViewerDialog::ExecuteViewLD()
    {
    FUNC_LOG;
    return ExecuteLD( R_MRVIEWER_DIALOG );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::Response
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDialog::Response( TInt aCommand )
    {
    FUNC_LOG;

    TBool retValue( EFalse );
    iExitTriggered = EFalse;

    if ( KErrCancel != aCommand )
        {
        TBool handleAttachments( EFalse );
        TRAPD( error, handleAttachments = UserWantToHandleAttachmentsL() );

        if ( !handleAttachments )
            {
            // Documentation says that this function might leave and also might
            // return system wide error code.
            TInt res = KErrNone;
            TRAP( error, res = iCallback.ProcessCommandWithResultL( aCommand ) );
            if ( res != KErrCancel && error == KErrNone )
                {
                iExitTriggered = ETrue;
                retValue = ETrue;
                }
            }
        }

    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ExitDialog
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::ExitDialog()
    {
    FUNC_LOG;
    // If closing does not work what can we do
    TRAPD( error, ProcessCommandL(EAknSoftkeyClose) );
    if ( error != KErrNone )
        {
        iCoeEnv->HandleError( error );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ChangeReadyResponseL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::ChangeReadyResponseL()
    {
    FUNC_LOG;
    iView->ProcessEditorCommandL( EESMRCmdEdit );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::DynInitToolbarL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::DynInitToolbarL (
        TInt /*aResourceId*/,
        CAknToolbar* /*aToolbar*/ )
	{

	}

// ---------------------------------------------------------------------------
// CESMRViewerDialog::OfferToolbarEventL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::OfferToolbarEventL( TInt aCommand )
	{
	ProcessCommandL( aCommand );
	}

// ---------------------------------------------------------------------------
// CESMRViewerDialog::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    MESMRCalEntry* entry = iInfoProvider.EntryL();

    TMRMenuStyle menuStyle( EMROptionsMenu );

    switch ( aResourceId )
        {
        case  R_ACTION_MENU:
            {
            CESMRContactMenuHandler& handler = iESMRStatic.ContactMenuHandlerL();
            if ( handler.OptionsMenuAvailable() )
                {
                handler.InitOptionsMenuL( aMenuPane );
                }
            break;
            }
        case R_MR_VIEWER_MENU:
            {
            HandlePolicyMenuL(
                    aResourceId,
                    aMenuPane );

            // Insert send ui menu for all other entry types than meeting request
            if ( entry->Type() != MESMRCalEntry::EESMRCalEntryMeetingRequest )
                {
                TryInsertSendMenuL(aMenuPane);
                }

            if ( entry->Entry().AttendeesL().Count() <= 1 )
                {
                aMenuPane->SetItemDimmed( EESMRCmdReplyAll, ETrue );
                }

            HandleDynamicMenuItemsL( aResourceId, aMenuPane );

            break;
            }
        case R_FORWARD_OPTIONS:
            {
            HandleForwardMenuL( aResourceId, aMenuPane );
            break;
            }
        case R_MR_VIEWER_LOCATION_MENU:
            {
            menuStyle = EMRContextMenu;
            InitLocationMenuL( aMenuPane );
            break;
            }
        case R_MR_VIEWER_ATTACHMENT_MENU:
            {
            menuStyle = EMRContextMenu;
            break;
            }
        default:
            {
            break;
            }
        }

    // Handles email submenu
    HandleEmailSubmenuL( aResourceId, aMenuPane );

    // Handles field specific context menu
    iView->DynInitMenuPaneL( menuStyle, aResourceId, aMenuPane );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::HandleDynamicMenuItemsL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::HandleDynamicMenuItemsL(
            TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    if ( aResourceId == R_MR_VIEWER_MENU )
        {
        // Map and Navigation Fw support
        if ( iFeatures->FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration ) &&
             iView->IsControlVisible( EESMRFieldLocation ))
            {
            MESMRCalEntry* entry = iInfoProvider.EntryL();
            TBool showOnMap = LocationPluginHandlerL().
                IsCommandAvailableL( EESMRCmdShowOnMap, *entry );
            aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap, showOnMap );
            aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, !showOnMap );
            }
        else
            {
            aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap, ETrue );
            aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, ETrue );
            }

        // Handle Action Menu item
        TBool dim = !iESMRStatic.ContactMenuHandlerL().OptionsMenuAvailable();
        aMenuPane->SetItemDimmed( EESMRCmdActionMenu, dim );
        aMenuPane->SetItemDimmed( EESMRCmdClipboardCopy, dim );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::TryInsertSendMenuL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::TryInsertSendMenuL(CEikMenuPane* aMenuPane)
    {
    FUNC_LOG;
    TInt index(KErrNotFound);

	// Insert send menu to the next position from "editing options"
	aMenuPane->ItemAndPos(EESMRCmdClipboardCopy, index);
	if (index == KErrNotFound)
		{
		aMenuPane->ItemAndPos(EESMRCmdCalEntryUIDelete, index);
		if (index == KErrNotFound)
			{
			aMenuPane->ItemAndPos(EESMRCmdDeleteMR, index);
			}
		}
	index++;
	iESMRSendUI->DisplaySendMenuItemL(*aMenuPane, index);
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::SetContextMenu
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::SetContextMenu()
    {
    TESMREntryFieldId fieldId = iView->ClickedOrFocusedField();

    switch( fieldId )
        {
        case EESMRFieldViewerAttachments:
            {
            iContextMenuResourceId = R_MR_VIEWER_ATTACHMENT_CONTEXT_MENU;
            }
            break;

        default:
            iContextMenuResourceId = R_MR_VIEWER_CONTEXT_MENU;
            break;
        }

    iMenuBar->SetContextMenuTitleResourceId( iContextMenuResourceId );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::OpenInDayModeL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::OpenInDayModeL()
    {
    FUNC_LOG;

    TApaTaskList taskList( iCoeEnv->WsSession() );
    TApaTask task = taskList.FindApp(KCalendarAppUID );

    HBufC16* tail = NULL;
    CommandTailL( tail );
    CleanupStack::PushL( tail );

    // copy the 16-bit data into 8-bit buffer
    HBufC8* paramBuf = HBufC8::NewLC( tail->Length() *2 );
    TPtr8 tailBuf = paramBuf->Des();

    tailBuf.Copy(
    	reinterpret_cast<const TUint8*>( tail->Ptr() ), tail->Length() *2 );

    if( task.Exists() )  // Calendar already open
        {
		const TUid dummyUID = { 0x0 };
		task.SendMessage( dummyUID, tailBuf );
		task.BringToForeground();
        }
    else  // Launch Calendar into day view
        {
		TApaAppInfo appInfo;
		RApaLsSession lsSession;
		User::LeaveIfError(lsSession.Connect());
		CleanupClosePushL( lsSession );

		if( lsSession.GetAppInfo( appInfo, KCalendarAppUID ) == KErrNone )
			{
			CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
			cmdLine->SetExecutableNameL( appInfo.iFullName );
			cmdLine->SetCommandL( EApaCommandRun );
			cmdLine->SetTailEndL( tailBuf );

			lsSession.StartApp( *cmdLine );
			CleanupStack::PopAndDestroy( cmdLine );
			}

		// Close lsSession
		CleanupStack::PopAndDestroy();  // codescanner::cleanup
        }
	CleanupStack::PopAndDestroy( paramBuf );
	CleanupStack::PopAndDestroy( tail );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::CommandTailL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::CommandTailL(HBufC16*& aTailBuffer)
    {
    FUNC_LOG;
    // "DATE YYYY MM DD HH MM SS MMMMMM"
    aTailBuffer = HBufC::NewLC( KCalDefaultBufferLen );
    TPtr tailPtr = aTailBuffer->Des();

    // get the start time of entry
    CCalEntry& entry = iInfoProvider.EntryL()->Entry();
    TTime startTime = entry.StartTimeL().TimeLocalL();

    startTime.FormatL(tailPtr, KCalCmdTimeFormat() );
    tailPtr.Insert( 0, KCalCmdDATE );
    CleanupStack::Pop( aTailBuffer );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::HandleCommandForEventL
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDialog::HandleCommandForEventL( TInt aCommand )
    {
    FUNC_LOG;
    TBool closeDialog( EFalse );

    MESMRCalEntry* calEntry( iInfoProvider.EntryL() );

    switch ( aCommand )
        {
        case EESMRCmdEdit:              //Fall through
        case EESMRCmdEditLocal:         //Fall through
        case EESMRCmdCalEntryUIEdit:
            {
            if ( calEntry->IsRecurrentEventL() )
                {
                // Check is the currently viewed entry a modifying entry.
                // MESMRCalEntry should provide this method.
                // if the entry is an exception to series, no query
                // should be asked in which mode the editor should be launched
                // in this occurance or series -mode.
                if ( calEntry->Entry().RecurrenceIdL().TimeUtcL() !=
                            Time::NullTTime()   )
                    {
                    calEntry->SetModifyingRuleL( MESMRCalEntry::EESMRThisOnly );
                    }
                else
                    {
                    SetRecurrenceModRuleL(
                        *calEntry,
                        CESMRListQuery::EESMROpenThisOccurenceOrSeriesQuery );
                    }
                }

            MESMRMeetingRequestEntry* entry = NULL;
            if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == calEntry->Type() )
                {
                entry = static_cast<MESMRMeetingRequestEntry*>(calEntry);

                TESMRRole role = entry->RoleL();
                if ( role == EESMRRoleOrganizer )
                    {
                    User::LeaveIfError(
                    	iCallback.ProcessCommandWithResultL( EESMRCmdEdit ));
                    closeDialog = ETrue;
                    }
                else
                    {
                    User::LeaveIfError(
                      iCallback.ProcessCommandWithResultL( EESMRCmdEditLocal ));
                    closeDialog = ETrue;
                    }
                }
            else
                {
                User::LeaveIfError(
                  iCallback.ProcessCommandWithResultL( EESMRCmdCalEntryUIEdit));
                closeDialog = ETrue;
                }
            break;
            }
        case EESMRCmdDeleteMR:          // Fall through
        case EESMRCmdCalEntryUIDelete:
            {
            TBool okToDelete( ETrue );
            MESMRCalEntry* calEntry = iInfoProvider.EntryL();
            
            if ( calEntry->IsRecurrentEventL() && calEntry->IsStoredL() )
                {
                if( CCalenInterimUtils2::IsMeetingRequestL( calEntry->Entry() ) )
                	{
                    // Static cast is safe here. We know for sure that entry is MR
                    MESMRMeetingRequestEntry* mrEntry =
                            static_cast<MESMRMeetingRequestEntry*>( calEntry );
                    
                    if( mrEntry->IsOpenedFromMail() )
                    	{
                        okToDelete = CESMRConfirmationQuery::ExecuteL(
                                                CESMRConfirmationQuery::EESMRDeleteMR );
                    	}
                    else
                    	{
                    	SetRecurrenceModRuleL(
                    			*calEntry,
                        		CESMRListQuery::EESMRDeleteThisOccurenceOrSeriesQuery );
                    	}
                	}
                else
                	{
    				SetRecurrenceModRuleL(
    					*calEntry,
    					CESMRListQuery::EESMRDeleteThisOccurenceOrSeriesQuery );                	
                	}
                }
            else
                {
                if( CCalenInterimUtils2::IsMeetingRequestL( calEntry->Entry() ) )
                    {
                    okToDelete = CESMRConfirmationQuery::ExecuteL(
                            CESMRConfirmationQuery::EESMRDeleteMR );
                    }
                else
                    {
                    okToDelete = CESMRConfirmationQuery::ExecuteL(
                             CESMRConfirmationQuery::EESMRDeleteEntry );
                    }
                }

            if ( okToDelete )
                {
                // When deleting we do not need to externalize entry
                TInt res = iCallback.ProcessCommandWithResultL( aCommand );
                if ( res != KErrCancel )
                    {
                    closeDialog = ETrue;
                    }
                }
            break;
            }
        default:
        	{
        	break;
        	}
        }

    return closeDialog;
    }


// ---------------------------------------------------------------------------
// CESMRViewerDialog::SendCalendarEntryL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::SendCalendarEntryL(TInt aCommandId)
    {
    FUNC_LOG;
    // we don't need MenuPane in CCalSend but we need it for the API
    CEikMenuPane* pane=NULL;

    // Show menu to user
    // CCalenSend handles selection internally, so we don't
    // get anything in return
    iESMRSendUI->DisplaySendCascadeMenuL(*pane);

    // Try to send
    if (iESMRSendUI->CanSendL(aCommandId))
        {
        MESMRCalEntry* calEntry = iInfoProvider.EntryL();
        iESMRSendUI->SendAsVCalendarL(aCommandId, calEntry->Entry() );
        }
    }


// ---------------------------------------------------------------------------
// CESMRViewerDialog::HandleEmailSubmenuL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::HandleEmailSubmenuL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;

    MESMRCalEntry* calEntry = iInfoProvider.EntryL();

    // Insert send ui menu for all other entry types than meeting request
    if ( calEntry->Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
        {
        if ( R_MR_VIEWER_MENU == aResourceId )
            {
            const CESMRPolicy& currentPolicy(
                    iInfoProvider.PolicyProvider().CurrentPolicy() );

            aMenuPane->SetItemDimmed(
                    EESMRCmdMailDelete,
                    !iCallback.IsCommandAvailable(
                            EESMRCmdMailDelete) );
            aMenuPane->SetItemDimmed(
                    EESMRCmdMailMessageDetails,
                    !iCallback.IsCommandAvailable(
                            EESMRCmdMailMessageDetails ) );

            aMenuPane->SetItemDimmed(
                    EESMRCmdMailPreviousMessage,
                    !iCallback.IsCommandAvailable(
                            EESMRCmdMailPreviousMessage ) );

            aMenuPane->SetItemDimmed(
                    EESMRCmdMailNextMessage,
                    !iCallback.IsCommandAvailable(
                            EESMRCmdMailNextMessage ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::HandlePolicyMenuL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::HandlePolicyMenuL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    MESMRCalEntry* calEntry = iInfoProvider.EntryL();
    const CESMRPolicy& currentPolicy(
            iInfoProvider.PolicyProvider().CurrentPolicy() );

    if ( aResourceId == R_MR_VIEWER_MENU )
        {
        if ( calEntry->Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
            {
            MESMRMeetingRequestEntry* mrEntry =
                    (MESMRMeetingRequestEntry*)( calEntry );
            CCalEntry::TMethod method(
                    mrEntry->Entry().MethodL() );

            TInt itemsInMenu( aMenuPane->NumberOfItemsInPane() );
            for ( TInt i(0); i < itemsInMenu; ++i )
                {
                CEikMenuPaneItem::SData& item(
                        aMenuPane->ItemDataByIndexL( i ) );

                if ( CCalEntry::EMethodCancel == method &&
                        EESMRRoleOrganizer != mrEntry->RoleL())
                    {
                    switch ( item.iCommandId )
                        {
                        case EESMRCmdAcceptMR: // Fall through
                        case EESMRCmdTentativeMR: // Fall through
                        case EESMRCmdDeclineMR: // Fall through
                        case EESMRCmdDeleteMR:
                            {
                            aMenuPane->SetItemDimmed(
                                    item.iCommandId,
                                    ETrue );
                            break;
                            }
                        case EESMRCmdRemoveFromCalendar:
                            {
                            if( SupportsMailBoxCapabilityL(
                                    MESMRBuilderExtension::
                                        EMRCFSRemoveFromCalendar ) )
                                {
                                // If Sync is enable and the mr is stored, we should show 
                                // "Remove from calendar" when viewing a past or cancelled MR
                                if ( mrEntry->IsOpenedFromMail() && mrEntry->IsStoredL() )
                                    {
                                    HBufC* removeString = 
                                            StringLoader::LoadLC( R_QTN_MEET_REQ_OPTIONS_REMOVE_FROM_CAL );
                                    aMenuPane->SetItemTextL( item.iCommandId, *removeString ); 
                                    CleanupStack::PopAndDestroy(); // removeString
                                    }
                                aMenuPane->SetItemDimmed(
                                        item.iCommandId,
                                        EFalse );
                                }
                            else
                                {
                                aMenuPane->SetItemDimmed(
                                        item.iCommandId,
                                        ETrue );
                                }
                            break;
                            }
                        default:
                            {
                            aMenuPane->SetItemDimmed(
                                    item.iCommandId,
                                    !currentPolicy.IsDefaultCommand(
                                    		item.iCommandId) );
                            break;
                            }
                        }
                    }
                else
                    {
                    if( mrEntry->OccursInPastL() ||
            			mrEntry->IsEntryOutOfDateL() ||
            			mrEntry->IsMeetingCancelledL() )
                    	{
                    	if( item.iCommandId == EESMRCmdAcceptMR ||
                    			item.iCommandId == EESMRCmdTentativeMR ||
                    			item.iCommandId == EESMRCmdDeclineMR )
                    		{
                    		aMenuPane->SetItemDimmed(
                    				item.iCommandId,
                    				ETrue );
                    		}
                        else if ( EESMRCmdRemoveFromCalendar == item.iCommandId &&
                        		  mrEntry->IsOpenedFromMail() )
                            {
                            // If Sync is enable and the mr is stored, we should show 
                            // "Remove from calendar" when viewing a past or cancelled MR
                            if ( mrEntry->IsStoredL() )
                                {
                                HBufC* removeString = 
                                        StringLoader::LoadLC( R_QTN_MEET_REQ_OPTIONS_REMOVE_FROM_CAL );
                                aMenuPane->SetItemTextL( item.iCommandId, *removeString ); 
                                CleanupStack::PopAndDestroy(); // removeString
                                }
                            aMenuPane->SetItemDimmed(
                                    EESMRCmdRemoveFromCalendar,
                                    EFalse );
                            }
                		else
                			{
                            aMenuPane->SetItemDimmed(
                                    item.iCommandId,
                                    !currentPolicy.IsDefaultCommand( item.iCommandId) );
                			}
                    	}
            		else
            			{
            			if ( EESMRCmdRemoveFromCalendar == item.iCommandId )
            				{
            				aMenuPane->SetItemDimmed(
            						EESMRCmdRemoveFromCalendar,
            						ETrue );
            				}
            			else
            				{
            				aMenuPane->SetItemDimmed(
            						item.iCommandId,
            						!currentPolicy.IsDefaultCommand( item.iCommandId) );
            				}
                        }
                    }
                if ( EESMRRoleOrganizer == mrEntry->RoleL())
                    {
                    //dim tracking view menuitem if no attendees
                    //or if current cfsmailbox doesn't support attendee status
                    if( EESMRCmdViewTrack == item.iCommandId )
                        {
                        TInt count(0);

                        if ( SupportsMailBoxCapabilityL(
                                MESMRBuilderExtension::
                                EMRCFSAttendeeStatus ) )
                            {
                            count = mrEntry->AttendeeCountL(EESMRRoleRequiredAttendee)+
                                    mrEntry->AttendeeCountL(EESMRRoleOptionalAttendee);
                            }

                        if (!count)
                            {
                            aMenuPane->SetItemDimmed( item.iCommandId, ETrue );
                            }
                        }
                    }
                }
            }
        else
            {
            TInt itemsInMenu( aMenuPane->NumberOfItemsInPane() );
            for ( TInt i(0); i < itemsInMenu; ++i )
                {
                CEikMenuPaneItem::SData& item = aMenuPane->ItemDataByIndexL(i);

                aMenuPane->SetItemDimmed(
                    item.iCommandId,
                    !currentPolicy.IsDefaultCommand( item.iCommandId) );
                }

            if ( calEntry->Type() == MESMRCalEntry::EESMRCalEntryTodo )
                {
                if (currentPolicy.IsDefaultCommand( EESMRCmdTodoMarkAsDone ) )
                    {
                    TBool dim(EFalse);
                    if( calEntry->Entry().StatusL() == CCalEntry::ETodoCompleted )
                        {
                        dim = ETrue;
                        }
                    aMenuPane->SetItemDimmed( EESMRCmdTodoMarkAsDone, dim );
                    aMenuPane->SetItemDimmed( EESMRCmdTodoMarkAsNotDone, !dim );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::SupportsAttendeeStatusL
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDialog::SupportsMailBoxCapabilityL(
        MESMRBuilderExtension::TMRCFSMailBoxCapability aCapa )
    {
    FUNC_LOG;
    TBool supportsCapability( EFalse );
    CESMRFieldBuilderInterface* plugin = NULL;
    TRAPD( error,
            plugin = CESMRFieldBuilderInterface::CreatePluginL(
                        TUid::Uid(KESMRUIFieldBuilderPluginImplUId) ) );
    CleanupStack::PushL( plugin );

    if (error == KErrNone && plugin)
        {
        TUid uid = {0};
        MESMRBuilderExtension* extension =
                static_cast<MESMRBuilderExtension*>( plugin->ExtensionL(uid) );

        MESMRCalEntry* calEntry = iInfoProvider.EntryL();

        if ( extension && MESMRCalEntry::EESMRCalEntryMeetingRequest == calEntry->Type())
            {
            // Static cast is safe here. We know for sure that entry is MR
            MESMRMeetingRequestEntry* mrEntry =
                    static_cast<MESMRMeetingRequestEntry*>( calEntry );

            supportsCapability = extension->CFSMailBoxCapabilityL(
                    mrEntry->Entry().PhoneOwnerL()->Address(),
                    aCapa );
            }
        }

    CleanupStack::PopAndDestroy( plugin );

    return supportsCapability;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::HandleMRExitL
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDialog::HandleMRExitL( TInt aCommand )
    {
    FUNC_LOG;
    TBool deleteEntry( EFalse );

    TBool queryAvailable( ETrue );

    // disable EESMRRemoveAppointment query if exiting dialog
    // through email-based option menu command,
    switch ( aCommand )
        {
        case EESMRCmdMailPreviousMessage: // Fall through
        case EESMRCmdMailNextMessage: // Fall through
        case EESMRCmdMailForwardAsMessage: // Fall through
        case EESMRCmdMailMoveMessage: // Fall through
        case EESMRCmdMailCopyMessage: // Fall through
        case EESMRCmdMailMoveMessageToDrafts: // Fall through
        case EESMRCmdMailComposeMessage: // Fall through
        case EESMRCmdMailMessageDetails:
        case EESMRCmdForwardAsMeeting: // Fall through
        case EESMRCmdForwardAsMail:
            {
            queryAvailable = EFalse;
            break;
            }
        case EAknSoftkeyClose:
        	{
        	queryAvailable = EFalse;
        	break;
        	}
        default:
            break;
        }

    MESMRCalEntry* calEntry = iInfoProvider.EntryL();

    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == calEntry->Type() )
        {
        // Static cast is safe here. We know for sure that entry is MR
        MESMRMeetingRequestEntry* mrEntry =
                static_cast<MESMRMeetingRequestEntry*>( calEntry );

        // Restore the status pane layout
        CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
        sp->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);

        if ( mrEntry->IsOpenedFromMail() )
            {


            if( queryAvailable &&
                EESMRRoleOrganizer != mrEntry->RoleL() &&
                ( mrEntry->IsMeetingCancelledL() ||
                		mrEntry->IsEntryOutOfDateL() ||
                		mrEntry->OccursInPastL() ) &&
                EESMRAttendeeStatusDecline != mrEntry->AttendeeStatusL() &&
                CESMRConfirmationQuery::ExecuteL( CESMRConfirmationQuery::EESMRRemoveAppointment)  )
                {
                deleteEntry = ETrue;
                }
            }
        }

    if ( deleteEntry )
        {
        iView->ExternalizeL();
        TInt res = iCallback.ProcessCommandWithResultL( EESMRCmdRemoveFromCalendar );
        if ( res == KErrCancel )
            {
            return EFalse;
            }
        }

    if ( EESMRCmdRemoveFromCalendar == aCommand )
        {
        return deleteEntry;
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::HandleForwardMenuL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::HandleForwardMenuL(
        TInt /*aResourceId*/,
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    MESMRCalEntry* calEntry = iInfoProvider.EntryL();
    if ( calEntry->Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
        {
        // Because this is MR entry, it is safe to cast it.
        MESMRMeetingRequestEntry* mrEntry =
                (MESMRMeetingRequestEntry*)( calEntry );

        __ASSERT_DEBUG( mrEntry, Panic( EESMRViewerDlgInvalidEntry) );

        TBool mrOriginatingEnabled( EFalse  );
        TESMRMailPlugin currentPlugin = mrEntry->CurrentPluginL();

        if ( EESMRIntelliSync == currentPlugin )
            {
            mrOriginatingEnabled = EFalse;
            }
        else if ( mrEntry->IsOpenedFromMail() )
            {
            // When opened from email, we need to make sure that
            // meeting request sending is possible
            CESMRFieldBuilderInterface* plugin = NULL;
            TRAPD( error, plugin =
                CESMRFieldBuilderInterface::CreatePluginL(
                        TUid::Uid(KESMRUIFieldBuilderPluginImplUId)) );

            CleanupDeletePushL( plugin );

            if (error == KErrNone && plugin)
                {
                TUid uid = {0};
                MESMRBuilderExtension* extension =
                        static_cast<MESMRBuilderExtension*>(
                                plugin->ExtensionL(uid) );

                if (extension)
                    {
                    mrOriginatingEnabled =
                            extension->MRCanBeOriginateedL();
                    }
                }
            CleanupStack::PopAndDestroy( plugin );
            plugin = NULL;
            }
        else
            {
            mrOriginatingEnabled = ETrue;
            }

        aMenuPane->SetItemDimmed(
                EESMRCmdForwardAsMeeting,
                !mrOriginatingEnabled );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::LocationPluginHandlerL
// ---------------------------------------------------------------------------
//
CESMRLocationPluginHandler& CESMRViewerDialog::LocationPluginHandlerL()
    {
    FUNC_LOG;
    if ( !iLocationPluginHandler )
        {
        iLocationPluginHandler = CESMRLocationPluginHandler::NewL( *this );
        }

    return *iLocationPluginHandler;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::HandleFieldEventL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::HandleFieldEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;

    MESMRFieldEvent::TEventType type( aEvent.Type() );

    if ( MESMRFieldEvent::EESMRFieldCommandEvent == type )
        {
        ProcessCommandEventL( aEvent );
        }
    else if ( MESMRFieldEvent::EESMRFieldChangeEvent == type )
        {
        ProcessFieldEventL( aEvent );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::SetDefaultMiddleSoftKeyL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::SetDefaultMiddleSoftKeyL()
    {
    FUNC_LOG;
    TInt resourceId = KErrNotFound;
    MESMRCalEntry* calEntry = iInfoProvider.EntryL();
    const CESMRPolicy& currentPolicy(
            iInfoProvider.PolicyProvider().CurrentPolicy() );

    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == calEntry->Type() )
        {
        // If 'Edit' command is not allowed then we change the MSK to 'Select'
        if ( !currentPolicy.IsDefaultCommand( EESMRCmdEdit ) )
            {
            resourceId = R_MR_SELECT_SOFTKEY;
            // Disable middle softkey by default
            // Focused field enables it if MSK has select functionality
            ButtonGroupContainer().MakeCommandVisibleByPosition(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    EFalse );
            }
        else
            {
            resourceId = R_MR_EDIT_SOFTKEY;
            }

        }
    else if ( currentPolicy.IsDefaultCommand( EESMRCmdCalEntryUIEdit ) )
        {
        // Set correct edit command in MSK. Dialog resource has EESMRCmdEdit
        resourceId = R_MR_CAL_EDIT_SOFTKEY;
        }

    if ( resourceId != KErrNotFound )
        {
        ButtonGroupContainer().SetCommandL(
                CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                resourceId );
        }
    }

// -----------------------------------------------------------------------------
// CESMRViewerDialog::ShowContextMenuL
// -----------------------------------------------------------------------------
//
void CESMRViewerDialog::ShowContextMenuL()
    {
    FUNC_LOG;

    if ( iMenuBar )
        {
        iMenuBar->TryDisplayContextMenuBarL();
        }
    }

 // ---------------------------------------------------------------------------
 // CESMRViewerDialog::HandleNaviArrowEventL
 // ---------------------------------------------------------------------------
 //
 void CESMRViewerDialog::HandleNaviArrowEventL( const TInt aCommand )
     {
     FUNC_LOG;
     ProcessCommandL( aCommand );
     }

 // ---------------------------------------------------------------------------
 // CESMRViewerDialog::UserWantToHandleAttachmentsL
 // ---------------------------------------------------------------------------
 //
 TBool CESMRViewerDialog::UserWantToHandleAttachmentsL()
     {
     TBool retValue( EFalse );

     MESMRCalEntry* calEntry = iInfoProvider.EntryL();
     MESMRMeetingRequestEntry* mrEntry = NULL;
     if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == calEntry->Type() )
         {
         // This is meeting request
         mrEntry = static_cast<MESMRMeetingRequestEntry*>( calEntry );

         TBool openedFromEmail( mrEntry->IsOpenedFromMail() );

         // If entry is opened from mail, contains remote attachments and
         // mailbox is MfE, we show the query.
         if ( openedFromEmail && mrEntry->ContainsRemoteAttachmentsL() &&
        		 mrEntry->CurrentPluginL() == EESMRActiveSync )
             {
             retValue =
                     CESMRConfirmationQuery::ExecuteL(
                                 CESMRConfirmationQuery::EESMRAttachments );
             }
         }

     return retValue;
     }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::InitLocationMenuL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::InitLocationMenuL( CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;

    aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap,ETrue );
    aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, ETrue );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ConstructToolbarL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::ConstructToolbarL()
    {
    FUNC_LOG;
    if( iInfoProvider.EntryL()->Type() ==
            MESMRCalEntry::EESMRCalEntryMeetingRequest )
        {
        MESMRMeetingRequestEntry* entry =
            static_cast<MESMRMeetingRequestEntry*>(
                    iInfoProvider.EntryL() );

        if( !entry->OccursInPastL() &&
                !entry->IsEntryOutOfDateL() &&
                    !entry->IsMeetingCancelledL() )
            {
            if ( entry->RoleL()== EESMRRoleRequiredAttendee ||
                        entry->RoleL()== EESMRRoleOptionalAttendee )
                {
                TSize screenSize =
                        iEikonEnv->ScreenDevice()->SizeInPixels();

                TBool isVGA( EFalse );
                if( ( screenSize.iHeight == KVGAOneSide ||
                        screenSize.iWidth == KVGAOneSide ) &&
                            ( screenSize.iHeight == KVGAOtherSide ||
                              screenSize.iWidth == KVGAOtherSide ) &&
                              screenSize.iHeight != screenSize.iWidth )
                    {
                    isVGA = ETrue;
                    }

                // Toolbar is disabled in VGA mode
                if( !isVGA )
                    {
                    // Toolbar is always shown in MR viewer when user
                    // is attendee
                    iToolbar->SetObserver( this );
                    iToolbar->InitializeToolbarL(
                            CMRToolbar::EMRViewerAttendee );
                    iToolbar->ShowToolbar( ETrue );

                    // Toolbar created, relayouting needed
                    SizeChanged();
                    // help the relayout of the viewer
                    iView->ListPane()->RecordFields();
                    DrawDeferred();
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ProcessCommandEventL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::ProcessCommandEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;

    TInt* command = static_cast< TInt* >( aEvent.Param( 0 ) );

    switch ( *command )
        {
        case EESMRCmdRestoreMiddleSoftKey:
            {
            SetDefaultMiddleSoftKeyL();
            ButtonGroupContainer().DrawDeferred();
            break;
            }
        case EESMRCmdEdit: // Fall through
        case EESMRCmdViewTrack: // Fall through
        case EESMRCmdOpenAttachment:// Fall through
        case EESMRCmdOpenAttachmentView:// Fall through
        case EESMRCmdDownloadAttachment: // Fall through
        case EESMRCmdDownloadAllAttachments: // Fall through
        case EESMRCmdSaveAttachment: // Fall through
        case EESMRCmdSaveAllAttachments: // Fall through
        case EESMRCmdMskOpenEmailAttachment: // Fall through
        case EESMRCmdSearchFromMap: // Fall through
        case EESMRCmdShowOnMap:
        case EMRCmdHideAttachmentIndicator:
            {
            ProcessCommandL( *command );
            break;
            }
       case EMRLaunchAttachmentContextMenu:
       case EAknSoftkeyContextOptions:
            {
            SetContextMenu();
            ShowContextMenuL();
            break;
            }
       case EESMRCmdAcceptMR:
       case EESMRCmdTentativeMR:
       case EESMRCmdDeclineMR:
       case EESMRCmdRemoveFromCalendar:
           {
           ProcessCommandL( *command );
           }
           break;
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ProcessFieldEventL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::ProcessFieldEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    iView->ProcessEventL( aEvent );
    }

// EOF
