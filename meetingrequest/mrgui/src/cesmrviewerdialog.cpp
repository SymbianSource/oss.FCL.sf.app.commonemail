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
* Description:  ESMR policy implementation
*
*/

#include "emailtrace.h"
#include "cesmrviewerdialog.h"

#include <data_caging_path_literals.hrh>
#include <esmrgui.rsg>
#include <esmrpolicies.rsg>
#include <eikenv.h>
#include <eikappui.h>
#include <avkon.hrh>
#include <MAgnEntryUi.h>
#include <eikmenup.h>
#include <calentry.h>
#include <apgtask.h>
#include <apgcli.h>
#include <AknDef.h>
#include <AknUtils.h>
#include <StringLoader.h>
#include <akntitle.h>
#include <eikspane.h>
#include <e32keys.h>
#include <AiwServiceHandler.h>
#include <featmgr.h>
#include <bldvariant.hrh>
#include <akntoolbar.h>
#include <aknappui.h>
#include <aknViewAppUi.h>
#include <aknview.h>
#include <eikcolib.h>
#include <aknbutton.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <msvuids.h>
//</cmail>
#include <featmgr.h>
//</cmail>
#include <cmrmailboxutils.h>

#include "cesmrpolicy.h"
#include "esmrdef.h"
#include "cesmrpolicymanager.h"
#include "cesmrurlparserplugin.h"
#include "esmricalvieweropcodes.hrh"
#include "tesmrinputparams.h"
#include "cesmrattachmentinfo.h"
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

#include "CFSMailBox.h"

/// Unnamed namespace for local definitions
namespace  { // codescanner::namespace

#ifdef _DEBUG

// Literal for viewer dialog
_LIT( KESMRViewerDlgPanicTxt, "ESMRViewerDlg" );

enum TESMRViewerDlgPanic
    {
    EESMRViewerDlgInvalidCommand,
    EESMRViewerDlgnvalidSeriesResult,
    EESMRViewerDlgInvalidEntry
    };

void Panic( TESMRViewerDlgPanic aPanic )
    {

    User::Panic( KESMRViewerDlgPanicTxt, aPanic );
    }

#endif

// Definition for default buffer length
const TInt KCalDefaultBufferLen = 40;

// Calendar command parameter
_LIT16( KCalCmdDATE, "GRAPHICDAY" );

// Definition for time format ("YYYY MM DD HH MM SS MMMMMM" - 26)
_LIT( KCalCmdTimeFormat, "%F %Y %M %D %H %T %S %C" );

// Definition for calendar application UID
const TUid KCalendarAppUID = { KUidCalendarApplication };

// Defs for prefix
_LIT( KMailtoMatchPattern, "mailto:*" ); // these are never localized
const TInt KMailtoLength = 7; // "mailto:" length


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Set empty buttons to toolbar.
// ---------------------------------------------------------------------------
//
void SetEmptyButtonsL( CAknToolbar* aToolbar )
    {
    aToolbar->AddItemL( CAknButton::NewL(), EAknCtButton, EESMRCmdUndefined, 0, 0 );
    aToolbar->AddItemL( CAknButton::NewL(), EAknCtButton, EESMRCmdUndefined, 0, 1 );
    aToolbar->AddItemL( CAknButton::NewL(), EAknCtButton, EESMRCmdUndefined, 0, 2 );
    }

// ---------------------------------------------------------------------------
// Set buttons for responding to meeting request to toolbar.
// ---------------------------------------------------------------------------
//
void SetResponseButtonsL( CAknToolbar* aToolbar )
    {
    CAknButton* buttonAccept = CAknButton::NewLC();
    buttonAccept->ConstructFromResourceL( R_ACCEPT_BUTTON );
    aToolbar->AddItemL( buttonAccept, EAknCtButton, EESMRCmdAcceptMR, 0, 0 );
    CleanupStack::Pop( buttonAccept );
    
    CAknButton* buttonTentative = CAknButton::NewLC();
    buttonTentative->ConstructFromResourceL( R_TENTATIVE_BUTTON );
    aToolbar->AddItemL( buttonTentative, EAknCtButton, EESMRCmdTentativeMR, 0, 1 );
    CleanupStack::Pop( buttonTentative );

    CAknButton* buttonDecline = CAknButton::NewLC();
    buttonDecline->ConstructFromResourceL( R_DECLINE_BUTTON );
    aToolbar->AddItemL( buttonDecline, EAknCtButton, EESMRCmdDeclineMR, 0, 2 );
    CleanupStack::Pop( buttonDecline );
    }

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
        MESMRCalEntry& aEntry,
        MAgnEntryUiCallback& aCallback  ) :
    iEntry( aEntry ),
    iCallback( aCallback ),
    iClearToolbar( EFalse )
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

    if ( iIdleNaviEventRunner )
        {
        iIdleNaviEventRunner->Cancel();
        delete iIdleNaviEventRunner;
        }
    
    iESMRStatic.Close();

    if ( iFeatureManagerInitialized )
        {
        FeatureManager::UnInitializeLib();
        }

    if ( iServiceHandler )
        {
        iServiceHandler->DetachMenu( R_MRVIEWER_MENUBAR, R_PS_AIW_INTEREST );
        delete iServiceHandler;
        }

    delete iESMRSendUI;

    delete iLocationPluginHandler;
    
    delete iFeatures;
    
    TRAP_IGNORE( ClearToolbarL() );

    // iView is deleted by framework because it uses the
    // custom control mechanism.
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRViewerDialog* CESMRViewerDialog::NewL(
        CESMRPolicy* aPolicy,
        MESMRCalEntry& aEntry,
        MAgnEntryUiCallback& aCallback )
    {
    FUNC_LOG;
    CESMRViewerDialog* self =
        new (ELeave) CESMRViewerDialog( aEntry, aCallback );
    CleanupStack::PushL( self );
    self->ConstructL( aPolicy );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::ConstructL(
        CESMRPolicy* aPolicy )
    {
    FUNC_LOG;
    
    iIdleNaviEventRunner = CIdle::NewL( CActive::EPriorityStandard );
    
    iPolicy = aPolicy;
    iESMRStatic.ConnectL();

    CAknDialog::ConstructL( R_MRVIEWER_MENUBAR );

    TRect clientRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, clientRect );

    TBool responseReady(EFalse);
    MESMRMeetingRequestEntry* mrEntry = NULL;
    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == iEntry.Type() )
        {
        // This is meeting request
        mrEntry = static_cast<MESMRMeetingRequestEntry*>( &iEntry );
        responseReady = mrEntry->IsSentL();
        }

    iESMRSendUI = CESMRSendUI::NewL(EESMRCmdSendAs);

    FeatureManager::InitializeLibL();
    iFeatureManagerInitialized = ETrue;

    CESMRViewerFieldStorage* storage =
            CESMRViewerFieldStorage::NewL(
                    aPolicy,
                    this,
                    responseReady,
                    *this );

    // storage ownership is transferred to CESMRView
    iView = CESMRView::NewL(storage, iEntry, clientRect );

    if ( iEntry.Entry().SummaryL().Length() == 0 )
        {
        // if no title, set unnamed text:
        HBufC* title = StringLoader::LoadLC ( R_QTN_MEET_REQ_CONFLICT_UNNAMED,
                                              iCoeEnv );
        iView->SetTitleL( *title );
        CleanupStack::PopAndDestroy( title );
        }
    else
        {
        iView->SetTitleL( iEntry.Entry().SummaryL() );
        }

    TInt titleStringId = -1;
    switch ( iEntry.Type() )
        {
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
        case MESMRCalEntry::EESMRCalEntryMeetingRequest: // Fall through
        default:
        	{
            break;
        	}
        }
    if ( titleStringId != -1 )
        {
        CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
        CAknTitlePane* tp = 
			static_cast<CAknTitlePane*>( 
					sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)) );
        HBufC* titleText = StringLoader::LoadLC( titleStringId, iCoeEnv );
        tp->SetTextL( *titleText );
        CleanupStack::PopAndDestroy( titleText );
        }


    iFeatures = CESMRFeatureSettings::NewL();
    if ( iFeatures->FeatureSupported(
            CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
        {
        iMenuBar->SetContextMenuTitleResourceId( R_MR_VIEWER_CONTEXT_MENU );
        }
    
    // start to observe navigation decorator
    CEikStatusPane* sp =
        static_cast<CEikAppUiFactory*>( 
                iEikonEnv->AppUiFactory() )->StatusPane();

    CAknNavigationControlContainer* naviPane =
        static_cast<CAknNavigationControlContainer*>( 
                sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    if ( naviPane )
        {
        iESMRNaviDecorator = naviPane->Top();

        if ( iESMRNaviDecorator )
            {
            iESMRNaviDecorator->SetNaviDecoratorObserver( this );
            }
        }
        
    
    //start service handler and add the interests of this class
    iServiceHandler = CAiwServiceHandler::NewL();
    iServiceHandler->AttachL( R_PS_AIW_INTEREST );
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
            case EStdKeyLeftArrow:
                if ( aType == EEventKey )
                    {
                    response = EKeyWasConsumed;
                    if ( iCallback.IsCommandAvailable(
                            EESMRCmdMailPreviousMessage ) )
                        {
                        ProcessCommandL( EESMRCmdMailPreviousMessage );
                        }
                    }
                break;
            case EStdKeyRightArrow:
                if ( aType == EEventKey )
                    {
                    response = EKeyWasConsumed;
                    if ( iCallback.IsCommandAvailable(
                            EESMRCmdMailNextMessage ) )
                        {
                        ProcessCommandL( EESMRCmdMailNextMessage );
                        }
                    }
                break;
            default:
                response = CAknDialog::OfferKeyEventL( aEvent, aType );
                break;
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
        case EESMRCmdPrint: // Fall through
        case EESMRCmdPrint_Reserved1: // Fall through
        case EESMRCmdPrint_Reserved2: // Fall through
        case EESMRCmdPrint_Reserved3: // Fall through
        case EESMRCmdPrint_Reserved4:
            {
            this->MakeVisible(EFalse); 
            HandlePrintCommandL(aCommand);
            this->MakeVisible(ETrue); 
            break;
            }
        case EESMRCmdSendAs:
            {
            SendCalendarEntryL(aCommand);
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
        case EESMRCmdCalEntryUIDelete:
            {
            if (!(iCallback.ProcessCommandWithResultL(aCommand) == KErrCancel))
                {
                TryExitL( EAknSoftkeyClose );
                }
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
        case EESMRCmdEdit: // Fall through
        case EESMRCmdEditLocal: // Fall through
        case EESMRCmdDeleteMR: // Fall through
        case EESMRCmdCalEntryUIEdit:
            {
            TBool closeDialog = HandleCommandForRecurrentEventL( aCommand );
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
            iView->ExternalizeL(); // no forced validation
            TInt res = iCallback.ProcessCommandWithResultL( aCommand );
            if ( res != KErrCancel )
                {
                TryExitL( EAknSoftkeyClose );
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
            {
            TInt res = iCallback.ProcessCommandWithResultL( aCommand );
            if ( res != KErrCancel )
                {
                TryExitL( aCommand );
                }
            break;
            }
        case EESMRCmdForward:
            {
            if ( iEntry.Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
                {
                // Because this is MR entry, it is safe to cast it.
                MESMRMeetingRequestEntry* mrEntry =
                    static_cast<MESMRMeetingRequestEntry*>( &iEntry );
                __ASSERT_DEBUG( mrEntry, Panic( EESMRViewerDlgInvalidEntry ) );
                
                TInt command = SupportsForwardingAsMeetingL( mrEntry, ETrue )
                    ? EESMRCmdForwardAsMeeting : EESMRCmdForwardAsMail;
                
                CAknToolbar* currentToolbar = static_cast<CEikAppUiFactory*>( 
                        iEikonEnv->AppUiFactory() )->CurrentFixedToolbar();
                if ( currentToolbar )
                    {
                    currentToolbar->SetToolbarVisibility( EFalse );
                    }
                TInt res = iCallback.ProcessCommandWithResultL( command );
                if ( currentToolbar )
                    {
                    currentToolbar->SetToolbarVisibility( ETrue );
                    }
                if ( res != KErrCancel )
                    {
                    TryExitL( aCommand );
                    }
                }
            break;
            }
        case EESMRCmdDownloadManager:
            {
            TInt res = iCallback.ProcessCommandWithResultL( aCommand );
            if ( res != KErrCancel )
                {
                TryExitL( EAknSoftkeyClose );
                }
            break;
            }
        case EESMRCmdOpenAttachment: // Fall through
        case EESMRCmdOpenAttachmentView: // Fall through
        case EESMRCmdDownloadAttachment: // Fall through
        case EESMRCmdDownloadAllAttachments: // Fall through
        case EESMRCmdSaveAttachment: // Fall through
        case EESMRCmdSaveAllAttachments: // Fall through
            {
            TInt res = iCallback.ProcessCommandWithResultL(
                    aCommand );
            if ( res != KErrCancel )
                {
                iExitTriggered = ETrue;
                }
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
            if ( iEntry.Type() == MESMRCalEntry::EESMRCalEntryTodo )
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
                LocationPluginHandlerL().HandleCommandL( aCommand, iEntry );
                iView->SetControlFocusedL( EESMRFieldLocation );
                }
            break;
			}
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
                iMenuBar->TryDisplayContextMenuBarL();
                }
            break;
            }
        case EESMRCmdEdit:
        case EESMRCmdEditLocal:
        case EESMRCmdCalEntryUIEdit:
            {
            res = HandleCommandForRecurrentEventL( aButtonId );
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
            
        case EESMRCmdOpenAttachmentView:
        case EESMRCmdMskOpenEmailAttachment:
       	case EESMRCmdOpenAttachment:
        case EESMRCmdDownloadAttachment: 
            {
            ProcessCommandL( aButtonId );
            break;
            }            

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
            {
            iClearToolbar = ETrue;
            res = HandleMRExitL( aButtonId );
            break;
            }            
        case EESMRCmdRemoveFromCalendar:
        	{
        	res = iCallback.ProcessCommandWithResultL ( aButtonId );
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
    MakeMrGuiToolbarButtonsL();
    return ExecuteLD( R_MRVIEWER_DIALOG );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::Response
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDialog::Response( TInt aCommand )
    {
    FUNC_LOG;
    iExitTriggered = EFalse;
    // Documentation says that this function might leave and also might
    // return system might error code.
    TInt res = KErrNone;
    TRAPD( error, res = iCallback.ProcessCommandWithResultL( aCommand ) );
    if ( res != KErrCancel && error == KErrNone )
        {
        iExitTriggered = ETrue;
        return ETrue;
        }
    return EFalse;
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
        CEikonEnv::Static()->HandleError( error );// codescanner::eikonenvstatic
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
// CESMRViewerDialog::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    if ( iServiceHandler )
        {
        if ( iServiceHandler->HandleSubmenuL( *aMenuPane ) )
            {
            return;
            }
        }

    switch ( aResourceId )
        {
        case  R_ACTION_MENU:
            {
            iESMRStatic.ContactMenuHandlerL().InitOptionsMenuL( aMenuPane );
            break;
            }
        case R_MR_VIEWER_MENU:
            {
    	    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
    		   {
    		   // remove help support in pf5250
    		   aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
    		   }
    	    
            HandlePolicyMenuL(
                    aResourceId,
                    aMenuPane );
    
            // Insert send ui menu for all other entry types than meeting request
            if ( iEntry.Type() != MESMRCalEntry::EESMRCalEntryMeetingRequest )
                {
                TryInsertSendMenuL(aMenuPane);
                }
            
            if ( iEntry.Entry().AttendeesL().Count() <= 1 )
                {
                aMenuPane->SetItemDimmed( EESMRCmdReplyAll, ETrue );
                }
    
            HandleDynamicMenuItemsL( aResourceId, aMenuPane );

            if ( FeatureManager::FeatureSupported( 
                    KFeatureIdPrintingFrameworkCalendarPlugin ) )
                {
                // Initiliaze menu
                iServiceHandler->InitializeMenuPaneL(
                        *aMenuPane,
                        aResourceId,
                        EESMRCmdPrint,
                        iServiceHandler->InParamListL() );
                }
            else
                {
                aMenuPane->SetItemDimmed( EESMRCmdPrint, ETrue );            
                }     
                
            TBool canCopyToClipboard =  iView->CanProcessEditorCommandL( EESMRCmdClipboardCopy );
            if ( !canCopyToClipboard )
                {
                aMenuPane->SetItemDimmed( EESMRCmdClipboardCopy, ETrue );            
                }     
                
            break;
            }

        case R_MR_VIEWER_LOCATION_MENU:
            {
            if ( iFeatures->FeatureSupported(
                    CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
                {
                TBool showOnMap =
                    LocationPluginHandlerL().IsCommandAvailableL( EESMRCmdShowOnMap,
                                                                  iEntry );
                
                aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, !showOnMap );
                aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap, showOnMap );
                
                aMenuPane->SetItemDimmed( EESMRCmdEdit,
                                          !iPolicy->IsDefaultCommand( EESMRCmdEdit ) );
                aMenuPane->SetItemDimmed( EESMRCmdEditLocal,
                                          !iPolicy->IsDefaultCommand( EESMRCmdEditLocal ) );
                aMenuPane->SetItemDimmed( EESMRCmdCalEntryUIEdit,
                                          !iPolicy->IsDefaultCommand( EESMRCmdCalEntryUIEdit ) );
                }
            
            break;
            }
        default:
            {
            break;
            }
        }

    // Handles email submenu
    HandleEmailSubmenuL( aResourceId, aMenuPane );
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
	    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
		   }
        if ( iFeatures->FeatureSupported(
                CESMRFeatureSettings::EESMRUIMnFwIntegration ) )
            {
            if ( !iView->IsComponentVisible( EESMRFieldLocation ))
                {
                aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap, ETrue );
                aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, ETrue );
                }
            else 
                {
                TBool showOnMap = LocationPluginHandlerL().
                    IsCommandAvailableL( EESMRCmdShowOnMap,
                                         iEntry );
                aMenuPane->SetItemDimmed( EESMRCmdSearchFromMap, showOnMap );
                aMenuPane->SetItemDimmed( EESMRCmdShowOnMap, !showOnMap );
                }
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
    CCalEntry& entry = iEntry.Entry();
    TTime startTime = entry.StartTimeL().TimeLocalL();

    startTime.FormatL(tailPtr, KCalCmdTimeFormat() );
    tailPtr.Insert( 0, KCalCmdDATE );
    CleanupStack::Pop( aTailBuffer );
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::HandleCommandForRecurrentEventL
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDialog::HandleCommandForRecurrentEventL( TInt aCommand )
    {
    FUNC_LOG;
    TBool closeDialog( EFalse );

    switch ( aCommand )
        {
        case EESMRCmdEdit: //Fall through
        case EESMRCmdEditLocal: //Fall through
        case EESMRCmdCalEntryUIEdit:
            {
            this->MakeVisible(EFalse); 
            if ( iEntry.IsRecurrentEventL() )
                {
                // Check is the currently viewed entry a modifying entry.
                // MESMRCalEntry should provide this method.
                // if the entry is an exception to series, no query
                // should be asked in which mode the editor should be launched
                // in this occurance or series -mode.
                if ( iEntry.Entry().RecurrenceIdL().TimeUtcL() !=
                            Time::NullTTime()   )
                    {
                    iEntry.SetModifyingRuleL( MESMRCalEntry::EESMRThisOnly );
                    }
                else
                    {
                    SetRecurrenceModRuleL(
                        iEntry,
                        CESMRListQuery::EESMROpenThisOccurenceOrSeriesQuery );
                    }
                }

            MESMRMeetingRequestEntry* entry = NULL;
            if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == iEntry.Type() )
                {
                entry = static_cast<MESMRMeetingRequestEntry*>(&iEntry);

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
            this->MakeVisible(ETrue); 
            break;
            }
        case EESMRCmdDeleteMR:
            {
            if ( iEntry.Type () == MESMRCalEntry::EESMRCalEntryMeetingRequest )
                {
                TBool okToDelete = ETrue;

                if ( iEntry.IsRecurrentEventL() )
                    {
                    SetRecurrenceModRuleL(
					 iEntry,
					 CESMRListQuery::EESMRDeleteThisOccurenceOrSeriesQuery );
                    }
                else
                    {
                    okToDelete = CESMRConfirmationQuery::ExecuteL(
								CESMRConfirmationQuery::EESMRDeleteMR );

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
        iESMRSendUI->SendAsVCalendarL(aCommandId, iEntry.Entry() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::HandlePrintCommandL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::HandlePrintCommandL(TInt aCommand)
    {
    FUNC_LOG;
    CAiwGenericParamList& inParams = iServiceHandler->InParamListL();

    // Param date

    TCalTime startTime = iEntry.Entry().StartTimeL();

    TAiwGenericParam dateParam( EGenericParamDateTime );
    TTime activeDay = startTime.TimeUtcL();

    TAiwGenericParam calendarParam( EGenericParamCalendarItem );
    calendarParam.Value().Set( TUid::Uid(iEntry.Entry().LocalUidL()) );
    inParams.AppendL( calendarParam );

    // Append date param
    dateParam.Value().Set( activeDay );
    inParams.AppendL( dateParam );

    const TUid uid( TUid::Uid( KUidCalendarApplication ) );
    TAiwGenericParam uidParam( EGenericParamApplication );
    uidParam.Value().Set( uid );
    inParams.AppendL( uidParam );

    // Execute service command with given parameters
    iServiceHandler->ExecuteMenuCmdL( aCommand,
                                      inParams,
                                      iServiceHandler->OutParamListL(),
                                      0,
                                      NULL );
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
    // Insert send ui menu for all other entry types than meeting request
    if ( iEntry.Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
        {
        if ( R_MRVIEWER_EMAIL_MENU == aResourceId )
            {
            TInt menuItemCount( aMenuPane->NumberOfItemsInPane() );
            for ( TInt i(0); i < menuItemCount; ++i )
                {
                CEikMenuPaneItem::SData& item(
                        aMenuPane->ItemDataByIndexL( i ) );
                aMenuPane->SetItemDimmed(
                    item.iCommandId,
                    !iCallback.IsCommandAvailable(item.iCommandId) );
                }
            }
        else if ( R_MR_VIEWER_MENU == aResourceId )
            {
    	    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
    		   {
    		   // remove help support in pf5250
    		   aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
    		   }
    	                
            TBool emailMenuDimmed( ETrue );
            // Try insert email specific menu
            if ( EESMRAppESEmail == iPolicy->AllowedApp() )
                {
                emailMenuDimmed = EFalse;
                }

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

            aMenuPane->SetItemDimmed(
                    EESMRCmdMailEmailMoreMenu,
                    emailMenuDimmed );
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
    if ( aResourceId == R_MR_VIEWER_MENU )
        {
	    if (FeatureManager::FeatureSupported( KFeatureIdFfCmailIntegration ))
		   {
		   // remove help support in pf5250
		   aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue);      
		   }
	            
        if ( iEntry.Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
            {
            MESMRMeetingRequestEntry* mrEntry =
                    (MESMRMeetingRequestEntry*)( &iEntry );
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
                            TPtrC addr = DigMailboxAndRemovePrefixL();
                            
                            if( iESMRStatic.MailBoxL( addr ).HasCapability( 
                                    EFSMBoxCapaRemoveFromCalendar ) &&
                                iEntry.IsStoredL() )
                                {
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
                                    !iPolicy->IsDefaultCommand( 
                                    		item.iCommandId) );
                            break;
                            }
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
                                !iPolicy->IsDefaultCommand( item.iCommandId) );
                        }
                    }
                if ( EESMRRoleOrganizer == mrEntry->RoleL())
                    {
                    //dim tracking view menuitem if no attendees
                    //or if current cfsmailbox doesn't support attendee status
                    if( EESMRCmdViewTrack == item.iCommandId )
                        {
                        TInt count(0);

                        if ( SupportsAttendeeStatusL() )
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
                    !iPolicy->IsDefaultCommand( item.iCommandId) );
                }

            if ( iEntry.Type() == MESMRCalEntry::EESMRCalEntryTodo )
                {
                if (iPolicy->IsDefaultCommand(EESMRCmdTodoMarkAsDone))
                    {
                    TBool dim(EFalse);
                    if( iEntry.Entry().StatusL() == CCalEntry::ETodoCompleted )
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
TBool CESMRViewerDialog::SupportsAttendeeStatusL( )
    {
    FUNC_LOG;
    TBool supportsAttendeeStatus(EFalse);
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

        if (extension)
            {
            supportsAttendeeStatus = 
                extension->CFSMailBoxCapabilityL(
                        MESMRBuilderExtension::EMRCFSAttendeeStatus);
            }
        }

    CleanupStack::PopAndDestroy( plugin );

    return supportsAttendeeStatus;
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
        case EESMRCmdMailMessageDetails: // Fall through
        case EESMRCmdForward:
            {
            queryAvailable = EFalse;
            break;
            }
        default:
            break;
        }
             
    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == iEntry.Type() )
        {
        MESMRMeetingRequestEntry* mrEntry =  static_cast<MESMRMeetingRequestEntry*>( &iEntry );

        if ( mrEntry->IsOpenedFromMail() &&
             queryAvailable &&
             EESMRRoleOrganizer != mrEntry->RoleL() &&
             mrEntry->IsMeetingCancelledL() &&
             EESMRAttendeeStatusDecline != mrEntry->AttendeeStatusL() &&
             CESMRConfirmationQuery::ExecuteL( CESMRConfirmationQuery::EESMRRemoveAppointment) )
            {
            deleteEntry = ETrue;
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
// CESMRViewerDialog::SupportsForwardingAsMeetingL
// ---------------------------------------------------------------------------
//
TBool CESMRViewerDialog::SupportsForwardingAsMeetingL( 
        MESMRMeetingRequestEntry* aEntry,
        TBool aForceResetDefaultMRMailbox )
    {
    FUNC_LOG;
    TBool sendingAsMRSupported( EFalse );
    TESMRMailPlugin currentPlugin = aEntry->CurrentPluginL();

    if ( EESMRIntelliSync != currentPlugin )
        {
        if ( aEntry->IsOpenedFromMail() )
            {
            // Set default MR mail box to the one used for this MR
            // unless if it already is set
            CMRMailboxUtils* mbUtils = CMRMailboxUtils::NewL();
            CleanupStack::PushL( mbUtils );
            TInt currentEntryId = AccountIdL( aEntry->CurrentMailBoxIdL() );
            CMRMailboxUtils::TMailboxInfo mrMailboxInfo;
            mbUtils->GetDefaultMRMailBoxL( mrMailboxInfo );
            if ( mrMailboxInfo.iEntryId != currentEntryId )
                {
                mbUtils->SetDefaultMRMailBoxL( currentEntryId );
                }
            CleanupStack::PopAndDestroy( mbUtils );
            
            // When opened from email, we need to make sure that
            // meeting request sending is possible
            CESMRFieldBuilderInterface* plugin = 
                CESMRFieldBuilderInterface::CreatePluginL(
                        TUid::Uid( KESMRUIFieldBuilderPluginImplUId ) );
            CleanupDeletePushL( plugin );

            if ( plugin )
                {
                TUid uid = {0};
                MESMRBuilderExtension* extension =
                        static_cast<MESMRBuilderExtension*>(
                                plugin->ExtensionL( uid ) );

                if ( extension )
                    {
                    sendingAsMRSupported =
                            extension->MRCanBeOriginateedL(
                                    aForceResetDefaultMRMailbox );
                    }
                }
            CleanupStack::PopAndDestroy( plugin );
            plugin = NULL;
            }
        else
            {
            sendingAsMRSupported = ETrue;
            }
        }

    return sendingAsMRSupported;
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
// CESMRViewerDialog::LocationPluginHandlerL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::HandleFieldEventL( const MESMRFieldEvent& aEvent )
    {
    FUNC_LOG;
    if ( aEvent.Type() == MESMRFieldEvent::EESMRFieldCommandEvent )
        {
        TInt* command = static_cast< TInt* >( aEvent.Param( 0 ) );
        
        switch ( *command )
            {
            case EESMRCmdRestoreMiddleSoftKey:
                {
                SetDefaultMiddleSoftKeyL();
                ButtonGroupContainer().DrawDeferred();
                break;
                }
           	case EESMRCmdOpenAttachment:// Fall through
            case EESMRCmdOpenAttachmentView:// Fall through
            case EESMRCmdDownloadAttachment: // Fall through
            case EESMRCmdDownloadAllAttachments: // Fall through
            case EESMRCmdSaveAttachment: // Fall through
            case EESMRCmdSaveAllAttachments: // Fall through
            case EESMRCmdMskOpenEmailAttachment:
                {
                ProcessCommandL( *command );
                
                break;
                }
            default:
                {
                break;
                }
            }
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
    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == iEntry.Type() )
        {
        // If 'Edit' command is not allowed then we change the MSK to 'Select'
        if ( !iPolicy->IsDefaultCommand( EESMRCmdEdit ) )
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
    else if ( iPolicy->IsDefaultCommand( EESMRCmdCalEntryUIEdit ) )
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

// ---------------------------------------------------------------------------
// CESMRViewerDialog::DigMailboxAndRemovePrefixL
// ---------------------------------------------------------------------------
//
TPtrC CESMRViewerDialog::DigMailboxAndRemovePrefixL()
    {
    CCalUser* calUser = iEntry.Entry().PhoneOwnerL();
    TPtrC addrWithoutPrefix = calUser->Address();
    TInt pos = KErrNotFound;
        
    do
        {
        pos = addrWithoutPrefix.MatchF( KMailtoMatchPattern );
        if ( pos != KErrNotFound )
        {
            addrWithoutPrefix.Set( addrWithoutPrefix.Mid( KMailtoLength ) );
            }
        }
    while ( pos != KErrNotFound );
    
    return addrWithoutPrefix;
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
// CESMRViewerDialog::HandleNaviDecoratorEventL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::HandleNaviDecoratorEventL( TInt aEventID )
    {
    FUNC_LOG;
    if ( iESMRNaviDecorator )
        {
        iESMRNaviDecorator->SetNaviDecoratorObserver( NULL );
        }
    if ( iIdleNaviEventRunner && !iIdleNaviEventRunner->IsActive() )
        {
        if( aEventID == 
        MAknNaviDecoratorObserver::EAknNaviDecoratorEventRightTabArrow )
            {
            iIdleNaviEventRunner->Start( 
                    TCallBack( RunCmdMailNextMessageIdle, this ) );
            }
        else
            {
            iIdleNaviEventRunner->Start( 
                    TCallBack( RunCmdMailPreviousMessageIdle, this ) );
            }
        }
    }


// ---------------------------------------------------------------------------
// CESMRViewerDialog::HandleSessionEventL
// ---------------------------------------------------------------------------
// 
void CESMRViewerDialog::HandleSessionEventL( 
    TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/,
    TAny* /*aArg2*/, TAny* /*aArg3*/ )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CESMRViewerDialog::RunCmdMailNextMessageIdle [static]
// ---------------------------------------------------------------------------
// 
TInt CESMRViewerDialog::RunCmdMailNextMessageIdle( TAny* aObjPtr )
    {
    CESMRViewerDialog* self = 
        reinterpret_cast<CESMRViewerDialog*>( aObjPtr );
    if ( self )
        {
        TRAP_IGNORE( self->ProcessCommandL( EESMRCmdMailNextMessage ) );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::RunCmdMailPreviousMessageIdle [static]
// ---------------------------------------------------------------------------
// 
TInt CESMRViewerDialog::RunCmdMailPreviousMessageIdle( TAny* aObjPtr )
    {
    CESMRViewerDialog* self = 
            reinterpret_cast<CESMRViewerDialog*>( aObjPtr );
    if ( self )
        {
        TRAP_IGNORE( self->ProcessCommandL( EESMRCmdMailPreviousMessage ) );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::MakeMrGuiToolbarButtonsL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::MakeMrGuiToolbarButtonsL()
    {
    CAknToolbar* currentToolbar = static_cast<CEikAppUiFactory*>( 
        iEikonEnv->AppUiFactory() )->CurrentFixedToolbar();
    if ( currentToolbar )
        {
        iOldObserver = currentToolbar->ToolbarObserver();
        currentToolbar->SetToolbarObserver( this );

        TBool setResponseButtons = EFalse;
        if ( iEntry.Type() == MESMRCalEntry::EESMRCalEntryMeetingRequest )
            {
            CCalEntry::TMethod method = iEntry.Entry().MethodL();
            if ( method != CCalEntry::EMethodCancel )
                {
                setResponseButtons = ETrue;
                }
            }

        if ( setResponseButtons )
            {
            SetResponseButtonsL( currentToolbar );
            }
        else
            {
            SetEmptyButtonsL( currentToolbar );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::ClearToolbarL
// ---------------------------------------------------------------------------
//
void CESMRViewerDialog::ClearToolbarL()
    {
    if ( iClearToolbar )
        {
        // creating empty toolbar buttons
        CAknToolbar* currentToolbar = static_cast<CEikAppUiFactory*>(
            iEikonEnv->AppUiFactory() )->CurrentFixedToolbar();
        if ( currentToolbar )
            {
            SetEmptyButtonsL( currentToolbar );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerDialog::AccountIdL
// ---------------------------------------------------------------------------
//
TInt CESMRViewerDialog::AccountIdL( const TFSMailMsgId& aMailboxId )
    {
    FUNC_LOG;

    CMsvSession* msvSession = CMsvSession::OpenSyncL( *this );
    CleanupStack::PushL( msvSession );
    CMsvEntry* rootEntry = msvSession->GetEntryL( KMsvRootIndexEntryIdValue );
    CleanupStack::PushL( rootEntry );

    rootEntry->SetSortTypeL(
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );

    TInt accountId = 0;
    TBool found = EFalse;
    const TInt count = rootEntry->Count();
    for ( TInt ii = 0; ii < count && !found; ++ii )
        {
        const TMsvEntry& entry = (*rootEntry)[ii];
        if ( entry.iType == KUidMsvServiceEntry )
            {
            if ( entry.iMtmData1 == aMailboxId.PluginId().iUid &&
                entry.iMtmData2 == aMailboxId.Id() )
                {
                accountId = entry.Id();
                found = ETrue;
                }
            }
        }

    if ( !found )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( rootEntry );
    CleanupStack::PopAndDestroy( msvSession );
    return accountId;
    }
