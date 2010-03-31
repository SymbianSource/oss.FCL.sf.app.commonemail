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
* Description:  Implementation for Meeting request task factory
*
*/

#include "emailtrace.h"
#include "cmrtaskplugin.h"

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
#include "cesmrcaldbmgr.h"
//<cmail>
#include "CFSMailCommon.h"
#include "CFSMailClient.h"
#include "CFSMailMessage.h"
#include "CFSMailBox.h"
//</cmail>


namespace {

#ifdef _DEBUG

// Panic literal for ESMRTaskFactory
_LIT( KESMRTaskFactoryPanicTxt, "MRTaskPlugin" );

/** Panic code definitions */
enum TESMRTaskFactoryPanic
    {
    EESMRTaskFactoryInvalidTask, // Trying to create invalid task
    EESMRTaskFactoryIllegalTask, // Task can not be performed for given entry.
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
    if ( ret != KErrCancel )
        {
        if ( aEntry.IsRecurrentEventL() )
            {
            aEntry.SetModifyingRuleL( MESMRCalEntry::EESMRAllInSeries );
            }
        }
    else
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
			aEntry.Entry().SetStatusL( CCalEntry::ETentative );
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
// CMRTaskPlugin::CMRTaskPlugin
// ---------------------------------------------------------------------------
//
inline CMRTaskPlugin::CMRTaskPlugin( MESMRCalDbMgr& aCalDbMgr )
    : iCalDbMgr( aCalDbMgr )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRTaskPlugin::~CMRTaskPlugin
// ---------------------------------------------------------------------------
//
CMRTaskPlugin::~CMRTaskPlugin()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRTaskPlugin::TaskFactory
// ---------------------------------------------------------------------------
//
CMRTaskPlugin* CMRTaskPlugin::NewL( TAny* aCalDbMgr )
    {
    FUNC_LOG;

    MESMRCalDbMgr* calDbMgr = static_cast< MESMRCalDbMgr* >( aCalDbMgr );
    CMRTaskPlugin* self = new(ELeave) CMRTaskPlugin( *calDbMgr );

    return self;
    }

// ---------------------------------------------------------------------------
// CMRTaskPlugin::CreateTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRTaskPlugin::CreateTaskL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    MESMRTask* task = NULL;

    MESMRMeetingRequestEntry& mrEntry =
        static_cast< MESMRMeetingRequestEntry& >( aEntry );
    
    switch (aCommand)
        {
        case EESMRCmdAcceptMR:
        case EESMRCmdTentativeMR:
        case EESMRCmdDeclineMR:
            task = CreateSendMRResponseViaMailTaskL( aCommand, mrEntry );
            break;

        case EESMRCmdSendMR:
        case EESMRCmdSendMRUpdate:
            task = CreateSendMRTaskL( aCommand, mrEntry );
            break;

        case EESMRCmdDeleteMR:
        case EESMRCmdRemoveFromCalendar:
        case EESMRCmdMailDelete:
            task = CreateDeleteMRTaskL( aCommand, mrEntry );
            break;

        case EESMRCmdSaveMR:
            task = CreateStoreMRToLocalDBTaskL( aCommand, mrEntry );
            break;

        case EESMRCmdForwardAsMeeting:
            task = CreateForwardAsMeetingTaskL( aCommand, mrEntry );
            break;

        case EESMRCmdForwardAsMail:
            task = CreateForwardAsMailTaskL( mrEntry );
            break;
        case EESMRCmdReplyAll:
            task = CreateReplyAsMailTaskL( mrEntry, ETrue );
            break;

        case EESMRCmdReply:
            task = CreateReplyAsMailTaskL( mrEntry, EFalse );
            break;

        case EESMRCmdCalendarChange:
            task = CreateMoveMRToCurrentDBTaskL( mrEntry );
            break;
            
        default:
            __ASSERT_DEBUG(EFalse, Panic(EESMRTaskFactoryInvalidTask) );
            User::Leave( KErrNotSupported );
            break;
        }


    return task;
    }

// ---------------------------------------------------------------------------
// CMRTaskPlugin::CreateSendMRResponseViaMailTask
// ---------------------------------------------------------------------------
//
MESMRTask* CMRTaskPlugin::CreateSendMRResponseViaMailTaskL(
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
        task = CESMRCombinedTask::NewL( aEntry,
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
            AppendTaskL( *task,
                         CESMRStoreMRTask::NewL(
                         iCalDbMgr,
                         aEntry,
                         aEntry.MailboxUtils() ) );
            }

        if ( EESMRResponseDontSend != responseType  ||
             aEntry.IsSyncObjectPresent() )
            {
            // Send MR response via mail freestyle task
            AppendTaskL( *task,
                        CESMRSendMRRespFSMailTask::NewL(
                                aCommand,
                                iCalDbMgr,
                                aEntry,
                                aEntry.MailboxUtils(),
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
            AppendTaskL( *task,
                        CESMRDeleteMRFromDbTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        aEntry.MailboxUtils() ) );
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
// CMRTaskPlugin::CreateSendMRTask
// ---------------------------------------------------------------------------
//

MESMRTask* CMRTaskPlugin::CreateSendMRTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;

    CESMRCombinedTask* task = CESMRCombinedTask::NewL( aEntry );

    CleanupStack::PushL( task );

    if ( !aEntry.IsForwardedL() )
        {
        // Store MR task
        AppendTaskL( *task,
                    CESMRStoreMRTask::NewL(
                    iCalDbMgr,
                    aEntry,
                    aEntry.MailboxUtils() ) );
        }

    if ( !aEntry.OccursInPastL() )
        {
        if ( aCommand == EESMRCmdSendMR ||
             aCommand == EESMRCmdSendMRUpdate )
            {
            // Send MR response via mail task
            AppendTaskL( *task, 
                        CESMRSendMRFSMailTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        aEntry.MailboxUtils(),
                        aCommand ) );
            }
        }

    CleanupStack::Pop( task );


    return task;
    }

// ---------------------------------------------------------------------------
// CMRTaskPlugin::CreateDeleteMRTask
// ---------------------------------------------------------------------------
//
MESMRTask* CMRTaskPlugin::CreateDeleteMRTaskL(
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
// CMRTaskPlugin::CreateStoreMRToLocalDBTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRTaskPlugin::CreateStoreMRToLocalDBTaskL(
        TESMRCommand /*aCommand*/,
        MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    return CESMRStoreMRTask::NewL(
                iCalDbMgr,
                aEntry,
                aEntry.MailboxUtils() );
    }

// ---------------------------------------------------------------------------
// CMRTaskPlugin::CreateForwardAsMeetingTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRTaskPlugin::CreateForwardAsMeetingTaskL(
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
                        aEntry.MailboxUtils(),
                        aCommand );
        }
    return task;
    }



// ---------------------------------------------------------------------------
// CMRTaskPlugin::CreateForwardAsMeetingTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRTaskPlugin::CreateForwardAsMailTaskL(
            MESMRMeetingRequestEntry& aEntry  )
    {
    FUNC_LOG;
    // Send MR response via mail task
    return CESMRForwardAsFSMailTask::NewL(
                iCalDbMgr,
                aEntry,
                aEntry.MailboxUtils() );
    }

// ---------------------------------------------------------------------------
// CMRTaskPlugin::CreateReplyAsMailTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRTaskPlugin::CreateReplyAsMailTaskL(
            MESMRMeetingRequestEntry& aEntry, TBool aReplyAll  )
    {
    FUNC_LOG;
    // Send MR reply via mail task
    return CESMRSendMRFSMailReplyTask::NewL(
                iCalDbMgr,
                aEntry,
                aEntry.MailboxUtils(),
                aReplyAll);

    }


// ---------------------------------------------------------------------------
// CMRTaskPlugin::CreateOrganizerDeleteMRTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRTaskPlugin::CreateOrganizerDeleteMRTaskL(
        TESMRCommand aCommand,
        MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;

    CESMRCombinedTask* task = CESMRCombinedTask::NewL(
                                    aEntry,
                                    CESMRCombinedTask::EESMRTrap );

    CleanupStack::PushL( task );

    if ( EESMRCmdDeleteMR == aCommand )
        {
        // SYNC_SOLUTION requires the modified entry to be stored into
        // calendar db for deletion purposes.
        AppendTaskL( *task,
                CESMRStoreMRTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        aEntry.MailboxUtils() ) );

        TUint attendeeFlags(
                EESMRRoleRequiredAttendee | EESMRRoleOptionalAttendee);
        if ( aEntry.AttendeeCountL( attendeeFlags ) && aEntry.IsSentL()
        		&& !aEntry.OccursInPastL() )
        	{
        	// if enrey doesn't need to send canellation, don't query
        	if( aEntry.SendCanellationAvailable())
        		{
        		// Meeting request contains attendees --> Cancellation message
        		// needs to be sent
        		if ( CESMRConfirmationQuery::ExecuteL(
        				CESMRConfirmationQuery::EESMRSendCancellationInfoToParticipants) )
        			{
        			AppendTaskL( *task,
        					CESMRSendMRFSMailTask::NewL(
        							iCalDbMgr,
        							aEntry,
        							aEntry.MailboxUtils(),
        							aCommand ) );
        			}
        		}
        	aEntry.SetSendCanellationAvailable( ETrue );
        	}
        }

    // Delete MR from cal DB task
    AppendTaskL( *task,
                CESMRDeleteMRFromDbTask::NewL(
                iCalDbMgr,
                aEntry,
                aEntry.MailboxUtils() ) );

    CleanupStack::Pop( task );


    return task;
    }

// ---------------------------------------------------------------------------
// CMRTaskPlugin::CreateAttendeeDeleteMRTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRTaskPlugin::CreateAttendeeDeleteMRTaskL(
        TESMRCommand aCommand,
        MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;

    TBool pluginRequiresReplyMail( ReplyMailRequiredL(aEntry) );

    CESMRCombinedTask* task = CESMRCombinedTask::NewL(
                                    aEntry,
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
        AppendTaskL( *task,
                CESMRStoreMRTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        aEntry.MailboxUtils() ) );
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

            AppendTaskL( *task,
                        CESMRSendMRRespFSMailTask::NewL(
                                command,
                                iCalDbMgr,
                                aEntry,
                                aEntry.MailboxUtils(),
                                EESMRResponsePlain,
                                KNullDesC() ) );
            }
        }

    // Delete MR from cal DB task if plugin does not handle it
    if ( !pluginRequiresReplyMail )
        {
        AppendTaskL( *task,
            CESMRDeleteMRFromDbTask::NewL(
            iCalDbMgr,
            aEntry,
            aEntry.MailboxUtils() ) );
        }

    CleanupStack::Pop( task );


    return task;
    }

// ---------------------------------------------------------------------------
// CMRTaskPlugin::CreateMoveMRToCurrentDBTaskL
// ---------------------------------------------------------------------------
//
MESMRTask* CMRTaskPlugin::CreateMoveMRToCurrentDBTaskL(
            MESMRMeetingRequestEntry& aEntry )
    {
    // Check first that entry can be moved
    if ( aEntry.IsSyncObjectPresent() )
        {
        __ASSERT_DEBUG( EFalse, Panic( EESMRTaskFactoryIllegalTask ) );
        User::Leave( KErrNotSupported );
        }
    
    // Create combined task
    CESMRCombinedTask* task = CESMRCombinedTask::NewL(
            aEntry,
            CESMRCombinedTask::EESMRTrap );
    
    CleanupStack::PushL( task );
    
    if ( !aEntry.IsOpenedFromMail() )
        {
        // When deleting from the calendar --> Store first modifying entry
        // And then delete that from calendar DB.
        //
        // When opened from mail --> We do not need to store
        // modifying entry into calendar database.
        //
        AppendTaskL(
                *task,
                CESMRStoreMRTask::NewL(
                        iCalDbMgr,
                        aEntry,
                        aEntry.MailboxUtils() ) );
        }
    
    // Delete entry from old db
    AppendTaskL(
            *task,
            CESMRDeleteMRFromDbTask::NewL(
                    iCalDbMgr,
                    aEntry,
                    aEntry.MailboxUtils() ) );
    
    // Store entry to current db
    AppendTaskL(
            *task,
            CESMRStoreMRTask::NewL(
                    iCalDbMgr,
                    aEntry,
                    aEntry.MailboxUtils() ) );
    
    CleanupStack::Pop( task );
    return task;
    }

// EOF

