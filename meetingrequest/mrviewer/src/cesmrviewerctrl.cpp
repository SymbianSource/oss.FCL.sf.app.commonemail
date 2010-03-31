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
* Description:  ESMR esmviewer controller implementation
*
*/

// INCLUDE FILES
//<cmail>
#include "emailtrace.h"
#include "cesmrpolicy.h"
#include "esmrdef.h"
#include "cesmrpolicymanager.h"
//</cmail>

#include "cesmrviewerctrl.h"
#include "cmrentryprocessor.h"
#include "mesmrmeetingrequestentry.h"
#include "mesmrtaskextension.h"
#include "esmrentryhelper.h"
#include "esmrhelper.h"
#include "cesmruifactory.h"

#include "mesmruibase.h"
#include "tesmrinputparams.h"
#include "tmroutputparams.h"
#include "esmrconfig.hrh"
#include "cesmrconfirmationquery.h"
#include "cesmrutils.h"
#include "cesmrcaluserutil.h"
#include "cesmrlistquery.h"
#include "cesmrcaldbmgr.h"

#include "cesmrtaskfactory.h"
#include "cesmrtaskextensionimpl.h"

// Logging utilities

// From System
//<cmail>
#include <cmrutils.h>
#include "esmrutilsapiext.h"
//</cmail>
#include <msvstd.h>
#include <msvids.h>
//<cmail>
#include "FreestyleEmailUiConstants.h"
//</cmail>
#include <eikappui.h>
#include <eikenv.h>
#include <calentry.h>
#include <caleninterimutils2.h>
#include <calcommon.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace { // codescanner::namespace

#if defined(_DEBUG)

// Definition for MR VIEWER ctrl panic text
_LIT(KMRViewerCtrlPanicTxt, "ESMRViewerCtrl" );

// MR VIEWER ctrl panic codes
enum TMRViewerCtrlPanicCode
    {
    EESMRViewerCtrlSyncher = 0, // Syncher is not created,
    EESMRViewerCtrlProcessor,
    EESMRViewerCtrlTaskExt,
    EESMRViewerCtrlPolicyMgr,
    EESMRViewerCtrlInvalidModifyingRule,
    EESMRViewerCtrlInvalidEntryType
    };

void Panic(TMRViewerCtrlPanicCode aPanicCode)
    {
    User::Panic( KMRViewerCtrlPanicTxt, aPanicCode );
    }

#endif

/**
 * Maps calendar entry type to MR entry type
 * @param aEntryType to be mapped to MR entry type
 * @return MR event type
 */
TESMRCalendarEventType MapType( const CCalEntry& aEntry )
    {
    TESMRCalendarEventType type( EESMREventTypeNone );

    switch ( aEntry.EntryTypeL() )
        {
        case CCalEntry::EAppt:
            {
            if( CCalenInterimUtils2::IsMeetingRequestL( 
                    ( const_cast<CCalEntry&>( aEntry ) ) ) )
                {
                type = EESMREventTypeMeetingRequest;
                }
            else
                {
                type = EESMREventTypeAppt;
                }
            break;
            }
        case CCalEntry::ETodo:
            {
            type = EESMREventTypeETodo;
            break;
            }
        case CCalEntry::EEvent:
            {
            type = EESMREventTypeEEvent;
            break;
            }
        case CCalEntry::EReminder:
            {
            type = EESMREventTypeEReminder;
            break;
            }
        case CCalEntry::EAnniv:
            {
            type = EESMREventTypeEAnniv;
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EESMRViewerCtrlInvalidEntryType ) );
            }
        }

    return type;
    }

/**
 * Creates calendar entry
 * @param required parameters for creating an entry
 * @return CCalEntry Created entry.
 */
CCalEntry* CreateEntryL(
        CCalEntry::TType aType,
        const TDesC8& aUid,
        CCalEntry::TMethod aMethod,
        CalCommon::TRecurrenceRange aRange )
    {
    HBufC8* guid = aUid.AllocLC();
    CCalEntry* entry = CCalEntry::NewL( aType, guid, aMethod, aRange );
    CleanupStack::Pop( guid );

    return entry;
    }

/**
 * Converts to-do specific priority to normal priority
 * @param Calendar entry, which priority is converted
 * @return The converted priority
 */
TInt ConvertTodoPriorityToNormalL( const CCalEntry& entry )
    {
    FUNC_LOG;
    TInt priority( 0 );
    if( entry.PriorityL() == EFSCalenTodoPriorityHigh )
        {
        priority = EFSCalenMRPriorityHigh;
        }
    
    else if( entry.PriorityL() == EFSCalenTodoPriorityNormal )
        {
        priority = EFSCalenMRPriorityNormal;
        }
    
    else if( entry.PriorityL() == EFSCalenTodoPriorityLow )
        {
        priority = EFSCalenMRPriorityLow;
        }
    
    else
        {
        // Priority unknown, let's set it to normal then
        priority = EFSCalenMRPriorityNormal;
        }

    return priority;
    }

/**
 * Converts normal priority to to-do specific priority
 * @param Calendar entry, which priority is converted
 * @return The converted priority
 */
TInt ConvertNormalPriorityToTodoL( const CCalEntry& entry )
    {
    FUNC_LOG;
    TInt priority( 0 );
    if( entry.PriorityL() == EFSCalenMRPriorityHigh )
        {
        priority = EFSCalenTodoPriorityHigh;
        }
    
    else if( entry.PriorityL() == EFSCalenMRPriorityNormal )
        {
        priority = EFSCalenTodoPriorityNormal;
        }
    
    else if( entry.PriorityL() == EFSCalenMRPriorityLow )
        {
        priority = EFSCalenTodoPriorityLow;
        }
    
    else
        {
        // Priority unknown, let's set it to normal then
        priority = EFSCalenMRPriorityNormal;
        }

    return priority;
    }

}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CESMRViewerController::CESMRViewerController
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CESMRViewerController::CESMRViewerController(
    RPointerArray<CCalEntry>& aEntries,
    const MAgnEntryUi::TAgnEntryUiInParams& aParams,
    MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
    MAgnEntryUiCallback& aCallback )
    : iInParams( aParams ),
      iOutParams( aOutParams ),
      iCallback( aCallback ),
      iEntries( aEntries )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::~CESMRViewerController
// -----------------------------------------------------------------------------
//
EXPORT_C CESMRViewerController::~CESMRViewerController()
    {
    FUNC_LOG;    
    delete iCtrlSyncher;
    delete iGuiFactory;
    delete iCalEntry;
    delete iESMRUtils;
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CESMRViewerController* CESMRViewerController::NewL(
    const TDesC8& aMtmuid,
    RPointerArray<CCalEntry>& aEntries,
    const MAgnEntryUi::TAgnEntryUiInParams& aParams,
    MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
    MAgnEntryUiCallback& aCallback )
    {
    FUNC_LOG;

    CESMRViewerController* self =
        new( ELeave ) CESMRViewerController( aEntries,
                                             aParams,
                                             aOutParams,
                                             aCallback );

    CleanupStack::PushL( self );
    self->ConstructL(aMtmuid);
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRViewerController::ConstructL(const TDesC8& /*aMtmUid */)
    {
    FUNC_LOG;
    // This will create ESMR UTILS. Initialization is asynchronous.
    // Competion is signaled via CESMRViewerController::HandleCalEngStatus.
    iESMRUtils = CESMRUtils::NewL( *this );
    iESMRUtils->SessionSetupL(
            iInParams.iCalSession,
            iInParams.iMsgSession );

    iESMRUtils->CreateExtensionsL(
            ESMREntryHelper::EventTypeL( *(iEntries[0]) ) );

    iGuiFactory = CESMRUiFactory::NewL();
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C void CESMRViewerController::ExecuteL()
    {
    FUNC_LOG;

    iExecutionError = KErrNone;

    // Initialization waiter is needed because MR VIEWER user expects
    // MR VIEWER operation to be synchronous.
    iCtrlSyncher = new (ELeave) CActiveSchedulerWait;

    // MR UTILS calls CESMRViewerController::HandleCalEngStatus after init is
    // complete. CESMRViewerController::HandleCalEngStatus also sets
    // iExectionError if there occurs error during MR UTILS init.
    iCtrlSyncher->Start();

    User::LeaveIfError( iExecutionError );

    iEntryProcessor =
        static_cast<CMREntryProcessor*>(
                iESMRUtils->ExtensionL(
                        TUid::Uid(KESMRMREntryProcessorUid) ) );

    iPolicyMgr =
        static_cast<CESMRPolicyManager*>(
                iESMRUtils->ExtensionL(
                        TUid::Uid(KESMRPolicyMgrUid) ) );

    iTaskExt =
        static_cast<MESMRTaskExtension*>(
                iESMRUtils->ExtensionL(
                        TUid::Uid(KESMRTaskExtensionUid) ) );

    // Check that all extensions are found
    if ( !iEntryProcessor || !iPolicyMgr || !iTaskExt )
        {
        // All extensions are not found --> Leave
        User::Leave( KErrNotSupported );
        }

    iEntryProcessor->ProcessL(
            &iInParams,
            &iEntries );

    LaunchCorrectOperationModeL();

    }

// -----------------------------------------------------------------------------
// CESMRViewerController::IsCommandAvailable
// -----------------------------------------------------------------------------
//
TBool CESMRViewerController::IsCommandAvailable( TInt aCommandId )
    {
    FUNC_LOG;


    TBool retValue( EFalse );

    if ( KFSEmailUiUid == iInParams.iCallingApp )
        {
        retValue = iCallback.IsCommandAvailable( aCommandId );
        }


    return retValue;
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::ProcessCommandWithResultL
// -----------------------------------------------------------------------------
//
TInt CESMRViewerController::ProcessCommandWithResultL( TInt aCommandId )
    {
    FUNC_LOG;

    TInt result = ProcessCommandWithResultInternalL(aCommandId);


    return result;
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CESMRViewerController::ProcessCommandL( TInt aCommandId )
    {
    FUNC_LOG;

    ProcessCommandWithResultL( aCommandId );

    }

// -----------------------------------------------------------------------------
// CESMRViewerController::HandleCalEngStatus
// -----------------------------------------------------------------------------
//
void CESMRViewerController::HandleCalEngStatus( TMRUtilsCalEngStatus aStatus )
    {
    FUNC_LOG;


    __ASSERT_DEBUG( iCtrlSyncher, Panic(EESMRViewerCtrlSyncher) );

    if ( ENotReady == aStatus )
        {
        // cal db is not ready
        iExecutionError = KErrNotReady;
        }
    else if (ENotAvailable == aStatus )
        {
        // cal db cannot be found
        iExecutionError = KErrNotFound;
        }

// <cmail>
    if ( iCtrlSyncher &&
            iCtrlSyncher->IsStarted() &&
            iCtrlSyncher->CanStopNow() )
// </cmail>
        {
        // Exection continues in CESMRViewerController::ExecuteL
        iCtrlSyncher->AsyncStop();
        }

    }

// -----------------------------------------------------------------------------
// CESMRViewerController::HandleOperation
// -----------------------------------------------------------------------------
//
void CESMRViewerController::HandleOperation(
        TInt /*aType*/,
        TInt /*aPercentageCompleted*/,
        TInt /*aStatus*/ )
    {
    FUNC_LOG;

    }

// -----------------------------------------------------------------------------
// CESMRViewerController::PolicyProvider
// -----------------------------------------------------------------------------
//
MMRPolicyProvider& CESMRViewerController::PolicyProvider() const
    {
    return *iPolicyMgr;
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::EntryL
// -----------------------------------------------------------------------------
//
MESMRCalEntry* CESMRViewerController::EntryL()
    {
    return &iEntryProcessor->ESMREntryL();
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::ProcessCommandWithResultInternalL
// -----------------------------------------------------------------------------
//
TInt CESMRViewerController::ProcessCommandWithResultInternalL(
        TInt aCommandId )
    {
    FUNC_LOG;

    TInt ret = ProcessDefaultCommandL( aCommandId );

    if ( ret == KErrNotSupported )
        {
        ret = ProcessMeetingRequestCommandL( aCommandId );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::ProcessDefaultCommandL
// -----------------------------------------------------------------------------
//
TInt CESMRViewerController::ProcessDefaultCommandL( TInt aCommandId )
    {
    FUNC_LOG;

    TESMRCommand command = static_cast<TESMRCommand>(aCommandId);
    TBool needToProcessOutputParams( ETrue );

    MESMRCalEntry& entry = iEntryProcessor->ESMREntryL();
    TInt ret = KErrNone;

    MESMRCalDbMgr& dbMgr = entry.GetDBMgr();
    switch( aCommandId )
        {
        case EESMRCmdSaveMR:            // Saves Entry
        case EESMRCmdCalEntryUISave:
            {
            // If we are editing an existing entry.
            if ( entry.IsStoredL() )
                {
                // If entry is type changed.
                if ( entry.IsEntryTypeChangedL() )
                    {
                    // If entry is type changed, delete the old entry first
                    // from db ... 

                    CCalEntry* lastStoredEntry = 
                            dbMgr.FetchEntryL( 
                                    entry.Entry().UidL(), 
                                    entry.Entry().RecurrenceIdL() );
                    ASSERT( lastStoredEntry );
                    CleanupStack::PushL( lastStoredEntry );
                    
                    if( CCalenInterimUtils2::IsMeetingRequestL( 
                                    *lastStoredEntry ) )
                        {
                        // Last stored entry was MR. If MR is sent, it needs to be 
                        // also cancelled before deleting
                        CancelAndDeleteMRInEntryTypeChangeL( 
                                *lastStoredEntry,dbMgr );
                        }
                    else
                        {
                        // Use right taskExtension                        
                        CESMRTaskFactory* taskFactory = CESMRTaskFactory::NewL(
                        		MapType( *lastStoredEntry ),
                        		dbMgr );
                        CleanupStack::PushL( taskFactory );                        
                        
                        MESMRTaskExtension* taskExtension =
                            CESMRTaskExtenstionImpl::NewL( *taskFactory );
                        CleanupDeletePushL( taskExtension );
                        
                        taskExtension->DeleteEntryFromLocalDBL(
                                EESMRCmdCalEntryUIDelete,
                                entry );
                        
                        CleanupStack::PopAndDestroy( taskExtension );                         
                        CleanupStack::PopAndDestroy( taskFactory );                                                
                        }
                    
                    // ... and then store the new (edited) entry to db.
                    iTaskExt->StoreEntryToLocalDBL(
                            command,
                            entry );
                    
                    CleanupStack::PopAndDestroy( lastStoredEntry );
                    }
                
                // If we are saving entry that previously existed in 
                // different db.
                else if ( dbMgr.EntryViewL( entry.Entry() ) != NULL
                          && dbMgr.EntryViewL( entry.Entry() ) != dbMgr.EntryView() )
                    {
                    iTaskExt->MoveEntryToCurrentDBL( command, entry );
                    }
                
                // And if none of the above conditions apply, we are just editing 
                // an entry and want to save it.
                else
                    {
                    iTaskExt->StoreEntryToLocalDBL(
                                            command,
                                            entry );
                    }
                }
            
            // Store new entry to db.
            else
                {
                iTaskExt->StoreEntryToLocalDBL(
                        command,
                        entry );
                }
            }
            break;

        case EESMRCmdCalEntryUIDelete:           // Deletes Entry
            {
            iTaskExt->DeleteEntryFromLocalDBL(
                    command,
                    entry );

            }
            break;

        case EESMRCmdEdit:              // Edit MR
        case EESMRCmdEditLocal:         // Locally edit meeting request
        case EESMRCmdCalEntryUIEdit:    // Edit calendar entries
            {
            iEntryProcessor->SwitchProcessorToModeL( EMRProcessorModeEdit );
            iPolicyMgr->PushPolicyL();
            LaunchUIL();

            // No need to process parameters because the view is closed
            // and editing view has processed to parameters
            needToProcessOutputParams = EFalse;
            iPolicyMgr->PopPolicy();

            iEntryProcessor->SwitchProcessorToModeL( EMRProcessorModeView );
            }
            break;

        case EESMRCmdCalEntryUISend:      // Send calendar entry item
        case EESMRCmdCalEntryUIAddParticipants:  // Add participants to meeting item
            {
            ASSERT( EFalse ); // TODO: implement when tasks are ready
            }
            break;

        case EESMRCmdEditorInitializationComplete:
            {
            if ( iCallback.IsCommandAvailable( EESMRCmdEditorInitializationComplete ) )
                {
                iCallback.ProcessCommandL( EESMRCmdEditorInitializationComplete );
                }
            break;
            }

        case EMRCommandSwitchToMR:
        case EMRCommandSwitchToMeeting:
        case EMRCommandSwitchToMemo:
        case EMRCommandSwitchToAnniversary:
        case EMRCommandSwitchToTodo:
            {
            ChangeEntryTypeL( aCommandId );
            needToProcessOutputParams = EFalse;
            }
            break;

        /*
        case EMRLaunchAttachmentsView:
        case EMRViewAttachmentInCorrespondingApp:
            {
            iCallback.ProcessCommandL( command );
            }
            break;
*/
        case EESMRCmdTodoMarkAsDone:
            {
            iTaskExt->MarkTodoAsDoneL( command,
                    iEntryProcessor->ESMREntryL() );
            }
            break;

        case EESMRCmdTodoMarkAsNotDone:
            {
            iTaskExt->MarkTodoAsNotDoneL( command,
                    iEntryProcessor->ESMREntryL() );
            }
            break;

        default:
            ret = KErrNotSupported;
            needToProcessOutputParams = EFalse;
            break;
        }

    if ( needToProcessOutputParams )
        {
        iEntryProcessor->ProcessOutputParametersL(
                iOutParams,
                command );
        }


    return ret;
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::ProcessMeetingRequestCommandL
// -----------------------------------------------------------------------------
//
TInt CESMRViewerController::ProcessMeetingRequestCommandL( TInt aCommandId )
    {
    FUNC_LOG;

    TESMRCommand command = static_cast<TESMRCommand>(aCommandId);
    TBool needToProcessOutputParams( ETrue );

    MESMRCalEntry* tmpEntry = &iEntryProcessor->ESMREntryL();

    ASSERT( tmpEntry->Type() == EESMREventTypeMeetingRequest );

    if ( tmpEntry->Type() != EESMREventTypeMeetingRequest )
        {
        User::Leave( KErrNotSupported );
        }

    MESMRMeetingRequestEntry& entry( *static_cast<MESMRMeetingRequestEntry*>( tmpEntry ) );
    MESMRCalDbMgr& dbMgr = entry.GetDBMgr();

    switch( aCommandId )
        {
        case EESMRCmdSendMR:            // Send Meeting request
        case EESMRCmdSendMRUpdate:      // Send MR update
            {
            entry.ConfirmEntryL();

            if( entry.IsStoredL() )
                {
                if( entry.IsEntryTypeChangedL() ||
                        ( ( dbMgr.EntryViewL( entry.Entry() ) != NULL ) &&
                        ( dbMgr.EntryViewL( entry.Entry() ) != dbMgr.EntryView() ) ) )
                    {
                    if(!entry.IsEntryTypeChangedL())
                    	{
                    	entry.SetSendCanellationAvailable( EFalse );
                    	}
                    iTaskExt->DeleteEntryFromLocalDBL(
                            EESMRCmdDeleteMR,
                            entry );
                    }
                }
            iTaskExt->SendAndStoreMRL(
                            command,
                            entry );
            }
            break;

        case EESMRCmdAcceptMR:          // Accept received MR
        case EESMRCmdTentativeMR:       // Tentatively accept MR
        case EESMRCmdDeclineMR:         // Decline MR
            {
			entry.ConfirmEntryL();
			iTaskExt->SendAndStoreResponseL(
					command,
					entry );
            }
            break;

        case EESMRAcceptWithoutAttachmentCheck:
        case EESMRTentativeWithoutAttachmentCheck:
        case EESMRDeclineWithoutAttachmentCheck:
            {
            entry.ConfirmEntryL();
            iTaskExt->SendAndStoreResponseL(
                    command,
                    entry );
            }
            break;

        case EESMRCmdDeleteMR:           // Deletes MR
        case EESMRCmdRemoveFromCalendar: // Removes MR from calendar
        case EESMRCmdMailDelete:         // Mail specific delete command
            {
            if ( EESMRCmdDeleteMR == command ||
                 EESMRCmdMailDelete == command  )
                {
                entry.MarkMeetingCancelledL();
                }

            iTaskExt->DeleteAndSendMRL(
                    command,
                    entry );

            if ( entry.IsOpenedFromMail() )
                {
                // Triggering mail delete command also
                needToProcessOutputParams = EFalse;
                iCallback.ProcessCommandL( aCommandId );
                }
            }
            break;

        case EESMRCmdViewTrack:
            {
            TESMRViewMode viewMode = iEntryProcessor->ScenarioData().iViewMode;
            // iEntryProcessor->SwitchToTrackL();
            iEntryProcessor->SwitchProcessorToModeL( EMRProcessorModeTrack );
            iPolicyMgr->PushPolicyL();
            LaunchUIL();

            //go back to original view depending on where we came from
            if (viewMode == EESMREditMR)
                {
                //iEntryProcessor->SwitchToEditL();
                iEntryProcessor->SwitchProcessorToModeL( EMRProcessorModeEdit );
                }
            else
                {
                // iEntryProcessor->SwitchToViewL();
                iEntryProcessor->SwitchProcessorToModeL( EMRProcessorModeView );
                }

            LaunchUIL();
            iPolicyMgr->PopPolicy();
            }
            break;
        case EESMRCmdForwardAsMeeting:   // Forward MR as meeting
            {
            // iEntryProcessor->SwitchToForwardL();
            iEntryProcessor->SwitchProcessorToModeL( EMRProcessorModeForward );
            iPolicyMgr->PushPolicyL();
            LaunchUIL();
            iPolicyMgr->PopPolicy();
            // iEntryProcessor->SwitchToViewL();
            iEntryProcessor->SwitchProcessorToModeL( EMRProcessorModeView );
            }
            break;

        case EESMRCmdForwardAsMail:      // Forward MR as mail
            {
            if ( entry.IsOpenedFromMail() )
                {
                needToProcessOutputParams = EFalse;
                iCallback.ProcessCommandL(
                        EESMRCmdMailForwardAsMessage );
                }
            else
                {
                iTaskExt->ForwardMRAsEmailL(
                        command,
                        entry );
                }
            }
            break;

        case EESMRCmdReply:      // Forward MR as mail
        case EESMRCmdReplyAll:   //Flow through
            {
            if ( entry.IsOpenedFromMail() )
                {
                TInt emailCommand( EESMRCmdMailReply );
                if ( EESMRCmdReplyAll == aCommandId )
                    {
                    emailCommand = EESMRCmdMailReplyAll;
                    }
                needToProcessOutputParams = EFalse;
                iCallback.ProcessCommandL(
                        emailCommand );
                }
            else
                {
                iTaskExt->ReplyAsEmailL(
                        command,
                        entry );
                }
            }
            break;

        // Email commands
        case EESMRCmdMailMarkUnread:
        case EESMRCmdMailMarkRead:
        case EESMRCmdMailForwardAsMessage:
        case EESMRCmdMailMoveMessage:
        case EESMRCmdMailCopyMessage:
        case EESMRCmdMailFlagMessage:
        case EESMRCmdMailComposeMessage:
        case EESMRCmdMailMessageDetails:
        case EESMRCmdMailPreviousMessage:
        case EESMRCmdMailNextMessage:
        case EESMRCmdMailMoveMessageToDrafts:
        case EESMRCmdOpenAttachment:
        case EESMRCmdOpenAttachmentView:
        case EESMRCmdDownloadAttachment:
        case EESMRCmdDownloadAllAttachments:
        case EESMRCmdSaveAttachment:
        case EESMRCmdSaveAllAttachments:
        case EESMRCmdDownloadManager:
            {
            needToProcessOutputParams = EFalse;
            iCallback.ProcessCommandL( aCommandId );
            }
            break;

        default:
            User::Leave( KErrNotSupported );
        }

    if ( needToProcessOutputParams )
        {
        iEntryProcessor->ProcessOutputParametersL(
                iOutParams,
                command );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::ChangeEntryTypeL
// -----------------------------------------------------------------------------
//
void CESMRViewerController::ChangeEntryTypeL( TInt aCommandId )
    {
    TESMRCalendarEventType type = EESMREventTypeNone;

    switch ( aCommandId )
        {
        case EMRCommandSwitchToMR:
            {
            type = EESMREventTypeMeetingRequest;
            break;
            }

        case EMRCommandSwitchToMeeting:
            {
            type = EESMREventTypeAppt;
            break;
            }

        case EMRCommandSwitchToMemo:
            {
            type = EESMREventTypeEEvent;
            break;
            }

        case EMRCommandSwitchToAnniversary:
            {
            type = EESMREventTypeEAnniv;
            break;
            }

        case EMRCommandSwitchToTodo:
            {
            type = EESMREventTypeETodo;
            break;
            }

        default:
            {
            break;
            }
        }    
   
    // Original entry is needed for creating new comparative entry
    CCalEntry* originalEntry =
            ESMRHelper::CopyEntryL(
                    iEntryProcessor->ESMREntryL().OriginalEntry(),
                    iEntryProcessor->ESMREntryL().OriginalEntry().MethodL(),
                    ESMRHelper::ECopyFull );
    
    CleanupStack::PushL( originalEntry );
    
    // Create a new entry by cloning the existing entry. 
    // This also does the type conversion and judges what
    // entry information will be copied from the old entry
    // to the new entry
    CCalEntry* newEntry =
        iEntryProcessor->ESMREntryL().CloneEntryLC(
                MESMRCalEntry::TESMRCalEntryType( type ) );
    
    iESMRUtils->CreateExtensionsL( type );
    
    iEntryProcessor =
            static_cast<CMREntryProcessor*>(
                    iESMRUtils->ExtensionL(
                            TUid::Uid( KESMRMREntryProcessorUid ) ) );

    iPolicyMgr =
        static_cast<CESMRPolicyManager*>(
                iESMRUtils->ExtensionL(
                        TUid::Uid( KESMRPolicyMgrUid ) ) );

    iTaskExt =
        static_cast<MESMRTaskExtension*>(
                iESMRUtils->ExtensionL(
                        TUid::Uid( KESMRTaskExtensionUid ) ) );
    
    // Process entry
    iEntryProcessor->ResetL();
    iEntryProcessor->ProcessL( &iInParams, *newEntry, EFalse, ETrue );

    if ( iInParams.iEditorMode == MAgnEntryUi::EViewEntry )
        {
        // If user has been viewing original entry, switch processor to editor.
        iEntryProcessor->SwitchProcessorToModeL( EMRProcessorModeEdit );
        }

    // Resolve new policy
    iPolicyMgr->ResolvePolicyL( iEntryProcessor->ScenarioData(),
                                iEntryProcessor->ESMREntryL(),
                                NULL );

    if ( iCalEntry )
        {
        delete iCalEntry;
        }

    iCalEntry = newEntry;
    CleanupStack::Pop( newEntry );
    newEntry = NULL;

    // Comparative entry needs to be updated also, otherwise
    // type changed entry's saving won't function correctly
    UpdateComparativeEntryInTypeChangeL( *originalEntry );
    
    iEntryProcessor->ESMREntryL().SetTypeChanged( ETrue );
    
    CleanupStack::PopAndDestroy( originalEntry );
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::LaunchUIL
// -----------------------------------------------------------------------------
//
void CESMRViewerController::LaunchUIL()
    {
    FUNC_LOG;
    iPolicyMgr->ResolvePolicyL(
        iEntryProcessor->ScenarioData(),
        iEntryProcessor->ESMREntryL(),
        NULL ); // Passing NULL --> Default policy resolver is used

    MESMRUiBase* uibase =
            iGuiFactory->CreateUIL(
            *this,
            *this );

    TInt retValue = uibase->ExecuteViewLD();

    if ( retValue == EAknCmdExit  )
        {
        MESMRCalEntry* calEntry = &iEntryProcessor->ESMREntryL();
        
        if( CCalenInterimUtils2::IsMeetingRequestL( calEntry->Entry() ) )
            {
            // Cast safe, after we have identified the entry as mr 
            MESMRMeetingRequestEntry& mrEntry( 
                    *static_cast< MESMRMeetingRequestEntry* >( calEntry ) );
            
            if( mrEntry.IsOpenedFromMail() )
                {
                CEikAppUi* appUi =
                    CEikonEnv::Static()->EikAppUi(); // codescanner::eikonenvstatic
                // Exit application
                static_cast<MEikCommandObserver*>( appUi )->ProcessCommandL( 
                        EAknCmdExit );
                }
            else
                {
                // Let's inform via output parameters that 
                // exit is desired.
                TMROutputParams* outputParams = 
                   reinterpret_cast< TMROutputParams* >( iOutParams.iSpare );
                outputParams->iCommand = EMRDialogOptionsMenuExit;
                }
            }
        else
            {
            // Let's inform via output parameters that 
            // exit is desired.
            TMROutputParams* outputParams = 
               reinterpret_cast< TMROutputParams* >( iOutParams.iSpare );
            outputParams->iCommand = EMRDialogOptionsMenuExit;
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::LaunchCorrectOperationModeL
// -----------------------------------------------------------------------------
//
void CESMRViewerController::LaunchCorrectOperationModeL()
    {
    FUNC_LOG;
    TESMRInputParams* esmrInputParams = (TESMRInputParams*)(iInParams.iSpare);

    if ( esmrInputParams &&
            EESMRCmdUndefined != esmrInputParams->iCommand )
        {
        ProcessCommandWithResultL( esmrInputParams->iCommand );
        }
    else
        {
        LaunchUIL();
        }
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::UpdateComparativeEntryInTypeChangeL
// -----------------------------------------------------------------------------
//
void CESMRViewerController::UpdateComparativeEntryInTypeChangeL(
        const CCalEntry& aOriginalEntry  )
    {
    /* 
     * This is needed because if user creates an entry and only type changes
     * the entry, but does  not input any data, we cannot save the entry.
     */

    CCalEntry& esmrEntry = iEntryProcessor->ESMREntryL().Entry();

    TESMRCalendarEventType type = MapType( aOriginalEntry );
    
    // Let's create new comparative entry based on the current 
    // type changed entry
    CCalEntry* newComparativeEntry = CreateEntryL(         
            iCalEntry->EntryTypeL(),
            iCalEntry->UidL(),
            CCalEntry::EMethodNone,
            CalCommon::EThisOnly );

    CleanupStack::PushL( newComparativeEntry );
    
    newComparativeEntry->CopyFromL( *iCalEntry );

    // New comparative entry needs still data from original entry.
    // Use case:
    // - User opens entry to editor
    // - User changes values
    // - User changes entry type
    // -> We need to determine if entry values have changed from the original
    newComparativeEntry->SetSummaryL(
            aOriginalEntry.SummaryL() );
    newComparativeEntry->SetLocationL(
            aOriginalEntry.LocationL() );
    newComparativeEntry->SetDescriptionL(
            aOriginalEntry.DescriptionL() );
    newComparativeEntry->SetReplicationStatusL(
            aOriginalEntry.ReplicationStatusL() );
    
    // If original and new entries are both appointments,
    // then also start and end times can be copied from original entry
    // to comparative entry
    if( aOriginalEntry.EntryTypeL() == CCalEntry::EAppt &&
            esmrEntry.EntryTypeL() == CCalEntry::EAppt )
        {
        newComparativeEntry->SetStartAndEndTimeL(
                aOriginalEntry.StartTimeL(),
                aOriginalEntry.EndTimeL() );
        }

    // Time stamp must be the same as in esmrEntry
    newComparativeEntry->SetDTStampL( esmrEntry.DTStampL() );

    // If neither the original entry, nor the new entry
    // are To-do's, we can also copy the priority from
    // the original entry, since priority value is then compatible
    // between the old and the new entry types
    if( aOriginalEntry.EntryTypeL() != CCalEntry::ETodo && 
            iCalEntry->EntryTypeL() != CCalEntry::ETodo )
        {
        newComparativeEntry->SetPriorityL( 
                aOriginalEntry.PriorityL() );
        }
    // One of the entries, original or new, are to-do's,
    // so the priority value is not compatible between them.
    // we need to convert that
    else
        {
        if( aOriginalEntry.EntryTypeL() == CCalEntry::ETodo )
            {
            newComparativeEntry->SetPriorityL( 
                    ConvertTodoPriorityToNormalL( aOriginalEntry ) );
            }
        else if( iCalEntry->EntryTypeL() == CCalEntry::ETodo )
            {
            newComparativeEntry->SetPriorityL( 
                    ConvertNormalPriorityToTodoL( aOriginalEntry ) );
            }
        }
    
    // If new entry is meeting request, let's also
    // copy the same phone owner to the comparative entry
    if( iCalEntry->MethodL() == CCalEntry::EMethodRequest )
        {
        CCalUser* temp = esmrEntry.OrganizerL(); // Ownership not transfered
        CCalUser* organizer = ESMRHelper::CopyUserL( *temp );

        newComparativeEntry->SetOrganizerL( organizer );
        newComparativeEntry->SetPhoneOwnerL(
                newComparativeEntry->OrganizerL() );
        }
    
    // Attachment information is also needed from original entry
    if( aOriginalEntry.AttachmentCountL() == 0 )
        {
        // Original entry did not have attachments, so let's 
        // remove possible attachments from comparative entry also
        TInt count = newComparativeEntry->AttachmentCountL();
        
        // Attachments need to be deleted in reversed order
        for( TInt i = ( count - 1 ); i == 0; --i )
            {
            newComparativeEntry->DeleteAttachmentL( 
                    *( newComparativeEntry->AttachmentL( i ) ) );
            }
        
        // If original entry does have attachments, we do nothing ->
        // If original entry has attachments, it has been already saved 
        // and now type changed, which means that it needs to be saved 
        // anyway.
        }

    iEntryProcessor->ESMREntryL().UpdateComparativeEntry(
            newComparativeEntry ); // Takes ownership

    CleanupStack::Pop( newComparativeEntry );
    }

// -----------------------------------------------------------------------------
// CESMRViewerController::CancelAndDeleteMRInEntryTypeChangeL
// -----------------------------------------------------------------------------
//
void CESMRViewerController::CancelAndDeleteMRInEntryTypeChangeL( 
        CCalEntry& aEntry, MESMRCalDbMgr& aDbMgr )
	{
	FUNC_LOG;
	// We need new task factor, processor and task extension
	// for this temporary MR action.
    CESMRTaskFactory* taskFactory = CESMRTaskFactory::NewL(
            EESMREventTypeMeetingRequest,
            aDbMgr );
    CleanupStack::PushL( taskFactory );
    
    CMREntryProcessor* entryProcessor = CMREntryProcessor::NewL(
            EESMREventTypeMeetingRequest,
            aDbMgr );
    CleanupStack::PushL( entryProcessor );
    
    MESMRTaskExtension* taskExtension =
        CESMRTaskExtenstionImpl::NewL( *taskFactory );
    CleanupDeletePushL( taskExtension );
    
    entryProcessor->ProcessL( &iInParams, aEntry, EFalse, ETrue );
    MESMRCalEntry& entry = entryProcessor->ESMREntryL();

    // Cast safe, because entry is always MR
    MESMRMeetingRequestEntry& mrEntry( 
            static_cast<MESMRMeetingRequestEntry&>( entry ) );
    
    if ( EESMRRoleOrganizer == mrEntry.RoleL() )
        {
        CCalEntry::TStatus status( mrEntry.Entry().StatusL() );
        if ( CCalEntry::ENullStatus != status )
            {
            // We are changing entry type from MR to some other type
            // And we have sent the MR invitation to attendees.
            // We provide possibility for user to send
            // cancellation message to attendees.
            mrEntry.MarkMeetingCancelledL();

            taskExtension->DeleteAndSendMRL(
                    EESMRCmdDeleteMR,
                    mrEntry );
            }
        else
            {
            taskExtension->DeleteEntryFromLocalDBL(
                    EESMRCmdDeleteMR,
                    mrEntry );
            }
        }
    
    CleanupStack::PopAndDestroy( taskExtension );
    CleanupStack::PopAndDestroy( entryProcessor );
    CleanupStack::PopAndDestroy( taskFactory );
	}

//  End of File
