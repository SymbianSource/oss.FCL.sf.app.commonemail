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
#include "cesmrentryprocessor.h"
#include "mesmrmeetingrequestentry.h"
#include "mesmrtaskextension.h"
//<cmail>
#include "cesmruifactory.h"
//</cmail>
#include "mesmruibase.h"
#include "tesmrinputparams.h"
#include "esmrconfig.hrh"
#include "cesmrconfirmationquery.h"
#include "cesmrutils.h"
#include "cesmrattachmentinfo.h"
#include "cesmrcaluserutil.h"
// <cmail> Removed profiling. </cmail>

// Logging utilities

// From System
//<cmail>
#include <CMRUtils.h>
#include "esmrutilsapiext.h"
//</cmail>
#include <msvstd.h>
#include <msvids.h>
//<cmail>
#include "FreestyleEmailUiConstants.h"
//</cmail>
#include <eikappui.h>
#include <eikenv.h>

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
    };

void Panic(TMRViewerCtrlPanicCode aPanicCode)
    {
    User::Panic( KMRViewerCtrlPanicTxt, aPanicCode );
    }

#endif

/**
 * Tests if entry contains attachments or not.
 * @param aEntry Reference to meeting request entry.
 * @return ETrue if entry contains attachments
 */
TBool ContainsAttachmentsL(
        MESMRMeetingRequestEntry& aEntry )
    {
    TBool retValue( EFalse );

    TESMRInputParams esmrInputParams;
    if ( aEntry.StartupParameters(esmrInputParams)  )
        {
        if ( esmrInputParams.iAttachmentInfo &&
                esmrInputParams.iAttachmentInfo->AttachmentCount() )
            {
            retValue = ETrue;
            }
        }
    return retValue;
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
    delete iESMRUtils;
    delete iCtrlSyncher;
    delete iGuiFactory;
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
        static_cast<CESMREntryProcessor*>(
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
// CESMRViewerController::ProcessCommandWithResultInternalL
// -----------------------------------------------------------------------------
//
TInt CESMRViewerController::ProcessCommandWithResultInternalL(
        TInt aCommandId )
    {
    FUNC_LOG;


    TESMRCommand command = static_cast<TESMRCommand>(aCommandId);
    TBool needToProcessOutputParams( ETrue );
    MESMRMeetingRequestEntry& entry = iEntryProcessor->ESMREntryL();

    switch( aCommandId )
        {
        case EESMRCmdSendMR:            // Send Meeting request
        case EESMRCmdSendMRUpdate:      // Send MR update
            {
            entry.ConfirmEntryL();
            iTaskExt->SendAndStoreMRL(
                            command,
                            entry );
            }
            break;

        case EESMRCmdAcceptMR:          // Accept received MR
        case EESMRCmdTentativeMR:       // Tentatively accept MR
        case EESMRCmdDeclineMR:         // Decline MR

            {
            if ( entry.IsRecurrentEventL() )
                {
                entry.SetModifyingRuleL(MESMRCalEntry::EESMRAllInSeries);
                }
            
            entry.ConfirmEntryL();
            iTaskExt->SendAndStoreResponseL(command,entry );
                        
            if ( entry.IsOpenedFromMail() )
               	{
               	// Triggering mail delete command also
               	needToProcessOutputParams = EFalse;
               	iCallback.ProcessCommandL( EESMRCmdMailDelete );
               	}
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

        case EESMRCmdSaveMR:            // Saves MR
            {
            iTaskExt->StoreMRToLocalDBL(
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

        case EESMRCmdEdit:              // Edit MR
        case EESMRCmdEditLocal:         // Locally edit meeting request
            {
            iEntryProcessor->SwitchToEditL();
            iPolicyMgr->PushPolicyL();
            LaunchUIL();

            // No need to process parameters because the view is closed
            // and editing view has processed to parameters
            needToProcessOutputParams = EFalse;
            iPolicyMgr->PopPolicy();
            iEntryProcessor->SwitchToViewL();
            }
            break;
        case EESMRCmdViewTrack:
            {
            TESMRViewMode viewMode = iEntryProcessor->ScenarioData().iViewMode;
            iEntryProcessor->SwitchToTrackL();
            iPolicyMgr->PushPolicyL();
            LaunchUIL();

            //go back to original view depending on where we came from
            if (viewMode == EESMREditMR)
                {
                iEntryProcessor->SwitchToEditL();
                }
            else
                {
                iEntryProcessor->SwitchToViewL();
                }

            LaunchUIL();
            iPolicyMgr->PopPolicy();
            }
            break;
        case EESMRCmdForwardAsMeeting:   // Forward MR as meeting
            {
            iEntryProcessor->SwitchToForwardL();
            iPolicyMgr->PushPolicyL();
            LaunchUIL();
            iPolicyMgr->PopPolicy();
            iEntryProcessor->SwitchToViewL();
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
// <cmail
        case EESMRCmdDownloadAttachment:
        case EESMRCmdDownloadAllAttachments:
        case EESMRCmdSaveAttachment:
        case EESMRCmdSaveAllAttachments:
// </cmail>
        case EESMRCmdDownloadManager:
            {
            needToProcessOutputParams = EFalse;
            iCallback.ProcessCommandL( aCommandId );
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
// CESMRViewerController::LaunchUIL
// -----------------------------------------------------------------------------
//
void CESMRViewerController::LaunchUIL()
    {
    FUNC_LOG;
    MESMRMeetingRequestEntry& entry = iEntryProcessor->ESMREntryL();

    iPolicyMgr->ResolvePolicyL(
        iEntryProcessor->ScenarioData(),
        entry,
        NULL ); // Passing NULL --> Default policy resolver is used

    MESMRUiBase* uibase =
            iGuiFactory->CreateUIL(
            &iPolicyMgr->CurrentPolicy(),
            entry,
            *this );

    TInt retValue = uibase->ExecuteViewLD();
    TBool openedFromMail( entry.IsOpenedFromMail() );
    
    if ( retValue == EAknCmdExit  )
       {
       if ( openedFromMail )
           {
           // Need to communicate with FS Email how this is
           // indicated to FS EMail UI application
           }
       else
           {
           CEikAppUi* appUi =
               CEikonEnv::Static()->EikAppUi(); // codescanner::eikonenvstatic
           // Exit application
           static_cast<MEikCommandObserver*>(appUi)->ProcessCommandL(EAknCmdExit);
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

//  End of File

