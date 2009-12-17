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
* Description:  Implementation for ESMR task factory
*
*/

#include "emailtrace.h"
#include "cesmrtaskfactory.h"

#include "cesmrcombinedtask.h"
#include "cesmrstoremrtask.h"
#include "cesmrsendmrrespmailtask.h"
#include "cesmrdeletefromdbmrtask.h"
#include "cesmrsendmrrespfsmailtask.h"
#include "cesmrsendmrfsmailtask.h"
#include "cesmrforwardasfsmailtask.h"
#include "cesmrsendmrfsmailreplytask.h"

#include "mesmrmeetingrequestentry.h"
#include "cesmrconfirmationquery.h"
#include "cesmrlistquery.h"
#include "cesmrresponsedialog.h"
#include "tesmrinputparams.h"
#include "esmrconfig.hrh"

//<cmail>
#include "CFSMailCommon.h"
#include "CFSMailClient.h"
#include "CFSMailMessage.h"
#include "CFSMailBox.h"
//</cmail>

namespace {

#ifdef _DEBUG

// Panic literal for ESMRTaskFactory
_LIT( KESMRTaskFactoryPanicTxt, "ESMRTaskFactory" );

/** Panic code definitions */
enum TESMRTaskFactoryPanic
    {
    EESMRTaskFactoryInvalidTask // Trying to create invalid task
    };


// ---------------------------------------------------------------------------
// Panic wrapper method
// ---------------------------------------------------------------------------
//
void Panic( TESMRTaskFactoryPanic aPanic )
    {

    User::Panic( KESMRTaskFactoryPanicTxt, aPanic );
    }

#endif // _DEBUG

/**
 * Checks whether reply mail is required by the plug-in or not.
 * @param aEntry Regerence to MR entry
 */
TBool ReplyMailRequiredL(
        MESMRMeetingRequestEntry& aEntry )
    {
    TBool replyNeeded( EFalse );

    TESMRInputParams esmrParams;
    if ( aEntry.IsOpenedFromMail() &&
         aEntry.StartupParameters(esmrParams) )
        {
        TFSMailMsgId mailboxId(
                esmrParams.iMailMessage->GetMailBoxId() );

        CFSMailBox* mailbox =
                esmrParams.iMailClient->GetMailBoxByUidL( mailboxId );
        CleanupStack::PushL( mailbox );

        if ( mailbox->HasCapability(EFSMBoxCapaMRRequiresReplyEmail ) )
            {
            replyNeeded = ETrue;
            }
        CleanupStack::PopAndDestroy( mailbox );
        mailbox = NULL;
        }

    return replyNeeded;
    }

// ---------------------------------------------------------------------------
// Queries response query from user.
// ---------------------------------------------------------------------------
//
HBufC* QuerySendResponseQueryFromUserLC(
        TESMRCommand aCommand,
        TESMRResponseType& aResponseType,
        MESMRMeetingRequestEntry& aEntry )
    {
    HBufC* responseMessage = NULL;
    TInt ret = EESMRResponsePlain;

    TESMRAttendeeStatus status(
            EESMRAttendeeStatusDecline );

    // convert the command to status:
    switch ( aCommand )
        {
        case EESMRCmdAcceptMR:
            {
            status = EESMRAttendeeStatusAccept;
            break;
            }
        case EESMRCmdTentativeMR:
            {
            status = EESMRAttendeeStatusTentative;
            break;
            }
        case EESMRCmdDeclineMR:
            {
            status = EESMRAttendeeStatusDecline;
            break;
            }
        default:
            {
            // never should come here.
            User::Leave(KErrGeneral);
            break;
            }
        }

    TBool replyRequired( ReplyMailRequiredL(aEntry) );

    if ( replyRequired )
        {
        ret = CESMRListQuery::ExecuteEditBeforeSendWithSendOptOnlyL( status );
        }
    else
        {
        ret = CESMRListQuery::ExecuteEditBeforeSendL( status );
        }

    // User has selected cancel from dialog. Interrupt task execution,
    // leave will be trapped in cesmecontroller (esmrviewer module)
    if ( ret == KErrCancel )
        {
        User::Leave( KErrCancel );
        }

    aResponseType = static_cast<TESMRResponseType>(ret);

    // set the response status to cal entry:
    aEntry.ConstructReplyL(status);

    if ( EESMRResponseMessage == aResponseType )
        {
        CESMRResponseDialog* respDlg =
                CESMRResponseDialog::NewL( responseMessage );
        CleanupStack::PushL( respDlg );

        TBool dialogRetValue( respDlg->ExecuteDlgLD() );
        // Dialog has deleted itself --> Only pop from cleanup stack
        CleanupStack::Pop( respDlg );

        if ( !dialogRetValue )
            {
            User::Leave( KErrCancel );
            }
        }
    if ( responseMessage )
        {
        CleanupStack::PushL( responseMessage );
        }
    else
        {
        responseMessage = KNullDesC().AllocLC();
        }
    return responseMessage;
    }

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTaskFactory::CESMRTaskFactory
// ---------------------------------------------------------------------------
//
inline CESMRTaskFactory::CESMRTaskFactory(
            MESMRCalDbMgr& aCalDbMgr,
            CMRMailboxUtils& aMRMailboxUtils )
:   iCalDbMgr( aCalDbMgr ),
    iMRMailboxUtils(aMRMailboxUtils)
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
// CESMRTaskFactory::~CESMRTaskFactory
// ---------------------------------------------------------------------------
//
CESMRTaskFactory::~CESMRTaskFactory()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRTaskFactory::TaskFactory
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRTaskFactory* CESMRTaskFactory::NewL(
            MESMRCalDbMgr& aCalDbMgr,
            CMRMailboxUtils& aMRMailboxUtils )
    {
    FUNC_LOG;

    CESMRTaskFactory* self =
        new (ELeave) CESMRTaskFactory(aCalDbMgr, aMRMailboxUtils );


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTaskFactory::CreateTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CESMRTaskFactory::CreateTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;

    MESMRTask* task = NULL;

    switch (aCommand)
        {
        case EESMRCmdAcceptMR:
        case EESMRCmdTentativeMR:
        case EESMRCmdDeclineMR:
            task = CreateSendMRResponseViaMailTaskL( aCommand, aEntry );
            break;

        case EESMRCmdSendMR:
        case EESMRCmdSendMRUpdate:
            task = CreateSendMRTaskL( aCommand, aEntry );
            break;

        case EESMRCmdDeleteMR:
        case EESMRCmdRemoveFromCalendar:
        case EESMRCmdMailDelete:
            task = CreateDeleteMRTaskL( aCommand, aEntry );
            break;

        case EESMRCmdSaveMR:
            task = CreateStoreMRToLocalDBTaskL( aCommand, aEntry );
            break;

        case EESMRCmdForwardAsMeeting:
            task = CreateForwardAsMeetingTaskL( aCommand, aEntry );
            break;

        case EESMRCmdForwardAsMail:
            task = CreateForwardAsMailTaskL( aEntry );
            break;
        case EESMRCmdReplyAll:
            task = CreateReplyAsMailTaskL( aEntry, ETrue );
            break;

        case EESMRCmdReply:
            task = CreateReplyAsMailTaskL( aEntry, EFalse );
            break;

        default:
            __ASSERT_DEBUG(EFalse, Panic(EESMRTaskFactoryInvalidTask) );
            User::Leave( KErrNotSupported );
            break;
        }


    return task;
    }

// ---------------------------------------------------------------------------
// CESMRTaskFactory::CreateSendMRResponseViaMailTask
// ---------------------------------------------------------------------------
//
MESMRTask* CESMRTaskFactory::CreateSendMRResponseViaMailTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;

    CESMRCombinedTask* task = NULL;
    TESMRRole role = aEntry.RoleL();
    
    TBool syncObjectPresent( aEntry.IsSyncObjectPresent() );
    TBool isStored( aEntry.IsStoredL() );

    if ( EESMRRoleRequiredAttendee == role  ||
         EESMRRoleOptionalAttendee == role  ||
         EESMRRoleNonParticipant == role )
        {
        task = CESMRCombinedTask::NewL
                                    (   iCalDbMgr,
                                        aEntry,
                                        iMRMailboxUtils,
                                        CESMRCombinedTask::EESMRTrap );

        CleanupStack::PushL( task );

        TESMRResponseType responseType( EESMRResponsePlain );
        HBufC* responseMessage = QuerySendResponseQueryFromUserLC(
                aCommand,
                responseType,
                aEntry );
        
        if ( !syncObjectPresent && EESMRCmdDeclineMR != aCommand)
            {
            // Entry is stored or deleted from calendar DB if
            // sync object is not present
            // Store MR task
            task->AppendTaskL(
                        CESMRStoreMRTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        iMRMailboxUtils ) );
            }

        if ( EESMRResponseDontSend != responseType  ||
             aEntry.IsSyncObjectPresent() )
            {
            // Send MR response via mail freestyle task
            task->AppendTaskL(
                        CESMRSendMRRespFSMailTask::NewL(
                                aCommand,
                                iCalDbMgr,
                                aEntry,
                                iMRMailboxUtils,
                                responseType,
                                *responseMessage ) );
            }
        CleanupStack::PopAndDestroy( responseMessage );
        
        if ( !syncObjectPresent && EESMRCmdDeclineMR == aCommand && 
                isStored )
            {
            // Entry is deleted from calendar DB if
            // sync object is not present and entry exits in database
            // Declined --> Delete MR from cal DB task
            task->AppendTaskL(
                        CESMRDeleteMRFromDbTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        iMRMailboxUtils ) );
            }

        CleanupStack::Pop( task );
        }
    else
        {
        // Only (opt-)attendees and non-participants can send
        // responses
        User::Leave( KErrNotSupported );
        }


    return task;
    }

// ---------------------------------------------------------------------------
// CESMRTaskFactory::CreateSendMRTask
// ---------------------------------------------------------------------------
//

MESMRTask* CESMRTaskFactory::CreateSendMRTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;

    CESMRCombinedTask* task = CESMRCombinedTask::NewL
                                (   iCalDbMgr,
                                    aEntry,
                                    iMRMailboxUtils );

    CleanupStack::PushL( task );

    if ( !aEntry.IsForwardedL() )
        {
        // Store MR task
        task->AppendTaskL(
                    CESMRStoreMRTask::NewL(
                    iCalDbMgr,
                    aEntry,
                    iMRMailboxUtils ) );
        }

    // Note: forwarding is allowed also for entries that occurred in past
    if ( aEntry.IsForwardedL() || !aEntry.OccursInPastL() )
        {
        if ( aCommand == EESMRCmdSendMR ||
             aCommand == EESMRCmdSendMRUpdate )
            {
            // Send MR response via mail task
            task->AppendTaskL(
                        CESMRSendMRFSMailTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        iMRMailboxUtils,
                        aCommand ) );
            }
        }

    CleanupStack::Pop( task );


    return task;
    }

// ---------------------------------------------------------------------------
// CESMRTaskFactory::CreateDeleteMRTask
// ---------------------------------------------------------------------------
//
MESMRTask* CESMRTaskFactory::CreateDeleteMRTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;

    MESMRTask* task = NULL;

    if ( EESMRRoleOrganizer == aEntry.RoleL() )
        {
        task = CreateOrganizerDeleteMRTaskL( aCommand, aEntry );
        }
    else
        {
        task = CreateAttendeeDeleteMRTaskL( aCommand, aEntry );
        }


    return task;
    }

// ---------------------------------------------------------------------------
// CESMRTaskFactory::CreateStoreMRToLocalDBTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CESMRTaskFactory::CreateStoreMRToLocalDBTaskL(
        TESMRCommand /*aCommand*/,
        MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    return CESMRStoreMRTask::NewL(
                iCalDbMgr,
                aEntry,
                iMRMailboxUtils );
    }

// ---------------------------------------------------------------------------
// CESMRTaskFactory::CreateForwardAsMeetingTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CESMRTaskFactory::CreateForwardAsMeetingTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry  )
    {
    FUNC_LOG;
    MESMRTask* task = NULL;

    // Send MR response via mail task is used for forwarding as meeting request

    if ( aCommand == EESMRCmdForwardAsMeeting )
        {
        task =  CESMRSendMRFSMailTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        iMRMailboxUtils,
                        aCommand );
        }
    return task;
    }



// ---------------------------------------------------------------------------
// CESMRTaskFactory::CreateForwardAsMeetingTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CESMRTaskFactory::CreateForwardAsMailTaskL(
            MESMRMeetingRequestEntry& aEntry  )
    {
    FUNC_LOG;
    // Send MR response via mail task
    return CESMRForwardAsFSMailTask::NewL(
                iCalDbMgr,
                aEntry,
                iMRMailboxUtils );
    }

// ---------------------------------------------------------------------------
// CESMRTaskFactory::CreateReplyAsMailTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CESMRTaskFactory::CreateReplyAsMailTaskL(
            MESMRMeetingRequestEntry& aEntry, TBool aReplyAll  )
    {
    FUNC_LOG;
    // Send MR reply via mail task
    return CESMRSendMRFSMailReplyTask::NewL(
                iCalDbMgr,
                aEntry,
                iMRMailboxUtils,
                aReplyAll);

    }


// ---------------------------------------------------------------------------
// CESMRTaskFactory::CreateOrganizerDeleteMRTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CESMRTaskFactory::CreateOrganizerDeleteMRTaskL(
        TESMRCommand aCommand,
        MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;

    CESMRCombinedTask* task = CESMRCombinedTask::NewL
                                (   iCalDbMgr,
                                    aEntry,
                                    iMRMailboxUtils,
                                    CESMRCombinedTask::EESMRTrap );

    CleanupStack::PushL( task );

    if ( EESMRCmdDeleteMR == aCommand )
        {
        // SYNC_SOLUTION requires the modified entry to be stored into
        // calendar db for deletion purposes.
        task->AppendTaskL(
                CESMRStoreMRTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        iMRMailboxUtils ) );

        TUint attendeeFlags(
                EESMRRoleRequiredAttendee | EESMRRoleOptionalAttendee);
        if ( aEntry.AttendeeCountL( attendeeFlags ) && aEntry.IsSentL()
				&& !aEntry.OccursInPastL() )
            {
            // Meeting request contains attendees --> Cancellation message
            // needs to be sent
            if ( CESMRConfirmationQuery::ExecuteL(
             CESMRConfirmationQuery::EESMRSendCancellationInfoToParticipants) )
                {
                task->AppendTaskL(
                        CESMRSendMRFSMailTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        iMRMailboxUtils,
                        aCommand ) );
                }
            }
        }

    // Delete MR from cal DB task
    task->AppendTaskL(
                CESMRDeleteMRFromDbTask::NewL(
                iCalDbMgr,
                aEntry,
                iMRMailboxUtils ) );

    CleanupStack::Pop( task );


    return task;
    }

// ---------------------------------------------------------------------------
// CESMRTaskFactory::CreateAttendeeDeleteMRTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CESMRTaskFactory::CreateAttendeeDeleteMRTaskL(
        TESMRCommand aCommand,
        MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;

    TBool pluginRequiresReplyMail( ReplyMailRequiredL(aEntry) );
    
    TBool deleteTaskCreated(EFalse);

    CESMRCombinedTask* task = CESMRCombinedTask::NewL
                                (   iCalDbMgr,
                                    aEntry,
                                    iMRMailboxUtils,
                                    CESMRCombinedTask::EESMRTrap );

    CleanupStack::PushL( task );

    if ( !aEntry.IsOpenedFromMail() && !aEntry.IsSyncObjectPresent() )
        {
        // When deleting from the calendar --> Store first modifying entry
        // And then delete that from calendar DB.
        //
        // When opened from mail --> We do not need to store
        // modifying entry into calendar database.
        //
        task->AppendTaskL(
                CESMRStoreMRTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        iMRMailboxUtils ) );
        }


    if ( EESMRCmdDeleteMR == aCommand   ||
         EESMRCmdMailDelete == aCommand   ||
         (pluginRequiresReplyMail && EESMRCmdRemoveFromCalendar == aCommand ) )
        {
        TBool sendDecline(EFalse);

        if ( pluginRequiresReplyMail )
            {
            sendDecline = ETrue;
            }
        else
            {
            sendDecline = CESMRConfirmationQuery::ExecuteL(
                    CESMRConfirmationQuery::EESMRSendDecline );
            }

        if( sendDecline )
            {
            // Send MR response via mail task. No response message is appended
            // to sent email

            TESMRCommand command( EESMRCmdDeclineMR );
            if ( pluginRequiresReplyMail && EESMRCmdRemoveFromCalendar == aCommand  )
                {
                command = EESMRCmdRemoveFromCalendar;
                }

            task->AppendTaskL(
                        CESMRSendMRRespFSMailTask::NewL(
                                command,
                                iCalDbMgr,
                                aEntry,
                                iMRMailboxUtils,
                                EESMRResponsePlain,
                                KNullDesC()) );
            }
        }

    // Delete MR from cal DB task if plugin does not handle it
    if ( !pluginRequiresReplyMail && !deleteTaskCreated)
        {
        task->AppendTaskL(
            CESMRDeleteMRFromDbTask::NewL(
            iCalDbMgr,
            aEntry,
            iMRMailboxUtils ) );
        }

    CleanupStack::Pop( task );


    return task;
    }

// EOF

