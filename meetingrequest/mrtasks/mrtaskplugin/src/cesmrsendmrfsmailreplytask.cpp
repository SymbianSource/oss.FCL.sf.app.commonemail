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
* Description:  Implementation for ESMR send MR via fs email task
*
*/

#include "emailtrace.h"
#include "cesmrsendmrfsmailreplytask.h"

#include <utf.h>
#include <bautils.h>
#include <caluser.h>
#include <cmrmailboxutils.h>
#include <calentry.h>
//<cmail>
#include "CFSMailMessage.h"
#include <coemain.h>
#include <coeaui.h>
#include "FreestyleEmailUiConstants.h"
//</cmail>

#include "cesmrcaldbmgr.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrfsemailmanager.h"
#include "cesmrmailplaitextformatter.h"
#include "cesmrcalimportexporter.h"
#include "cesmrcaluserutil.h"
#include "tesmrinputparams.h"
#include "esmrhelper.h"

// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

// Literal for panics
_LIT( ESMRSendMRRespMailPanicTxt, "ESMRSendMRFSEMailTask" );

// Panic codes
enum TESMRSendMRFSMailTaskPanic
    {
    EESMRSendMRFSAttendeesMissing,
    EESMRCannotBeTriggeredFromMail
    };

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Panic wrapper method
// ---------------------------------------------------------------------------
//
void Panic( TESMRSendMRFSMailTaskPanic aPanic )
    {

    User::Panic( ESMRSendMRRespMailPanicTxt, aPanic );
    }

#endif // _DEBUG

// ---------------------------------------------------------------------------
// Leave wrapper method
// ---------------------------------------------------------------------------
//
inline void DoLeaveIfErrorL( TInt aLeaveCode )
    {
    if ( KErrNone != aLeaveCode )
        {

        User::Leave( aLeaveCode );
        }
    }
} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailReplyTask::CESMRSendMRFSMailReplyTask
// ---------------------------------------------------------------------------
//
CESMRSendMRFSMailReplyTask::CESMRSendMRFSMailReplyTask(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TBool aReplyAll)
:   CESMRTaskBase( aCalDbMgr, aEntry, aMRMailboxUtils ),
    iReplyAll(aReplyAll)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailReplyTask::~CESMRSendMRFSMailReplyTask
// ---------------------------------------------------------------------------
//
CESMRSendMRFSMailReplyTask::~CESMRSendMRFSMailReplyTask()
    {
    FUNC_LOG;
    delete iEntryToSend;
    delete iCaluserUtil;
    delete iEmailMgr;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailReplyTask::NewL
// ---------------------------------------------------------------------------
//
CESMRSendMRFSMailReplyTask* CESMRSendMRFSMailReplyTask::NewL(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TBool aReplyAll)
    {
    FUNC_LOG;

    CESMRSendMRFSMailReplyTask* self =
        new (ELeave) CESMRSendMRFSMailReplyTask(
                aCalDbMgr,
                aEntry,
                aMRMailboxUtils,
                aReplyAll);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailReplyTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailReplyTask::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    iEmailMgr = CESMRFSEMailManager::NewL( MailboxUtils() );
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailReplyTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailReplyTask::ExecuteTaskL()
    {
    FUNC_LOG;

    iEntryToSend = ESMREntry().ValidateEntryL();
    iCaluserUtil = CESMRCalUserUtil::NewL( *iEntryToSend );

    ConstructMailL();

    iEmailMgr->StoreMessageToDraftsFolderL();

    LaunchFSEmailEditorL(iReplyAll);

    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::ConstructMailL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailReplyTask::ConstructMailL()
    {
    FUNC_LOG;

    // create text formatter:
    CESMRMailPlainTextFormatter* textFormatter =
            CESMRMailPlainTextFormatter::NewLC( MailboxUtils() );

    TESMRInputParams startupparams;
    TBool isEmail = ESMREntry().StartupParameters(startupparams);

    // This command cannot be triggered from email
    __ASSERT_DEBUG( !isEmail, Panic(EESMRCannotBeTriggeredFromMail) );

    if(isEmail)
        {
        // This command cannot be triggered from email
        User::Leave( KErrNotSupported );
        }
    else
        {
        //launched from calendar, mail id available for mr

        // Prepare email manager for sending
        CCalUser* mailboxUser = iEntryToSend->PhoneOwnerL();
        iEmailMgr->PrepareForSendingL(
                ESMRHelper::AddressWithoutMailtoPrefix(
                        mailboxUser->Address() ) );

        // set the email subject:
        HBufC* subject =
            textFormatter->ReplyStringLC( *iEntryToSend );
        
        iEmailMgr->SetSubjectL(*subject);
        CleanupStack::PopAndDestroy(subject);

        CCalUser* organizer = iEntryToSend->OrganizerL();

        //Add organizer as sender
        iEmailMgr->SetSenderL(
                            organizer->Address(),
                            organizer->CommonName() );

        if(iReplyAll)
            {
            //Add also required and optional attendees
            AddAttendeesL();
            }
        }

    CleanupStack::PopAndDestroy(textFormatter);

    }


// ---------------------------------------------------------------------------
// CESMRSendMRFSMailReplyTask::AddAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailReplyTask::AddAttendeesL()
    {
    FUNC_LOG;

    RArray<CCalAttendee*> requiredAttendees;
    CleanupClosePushL( requiredAttendees );

    RArray<CCalAttendee*> optionalAttendees;
    CleanupClosePushL( optionalAttendees );

    iCaluserUtil->GetAttendeesL(
            requiredAttendees,
            EESMRRoleRequiredAttendee );
    iCaluserUtil->GetAttendeesL(
            optionalAttendees,
            EESMRRoleOptionalAttendee );

    // Add recipients and cc-recipients
    TInt attendeeCount( requiredAttendees.Count() );

    __ASSERT_DEBUG( attendeeCount, Panic(EESMRSendMRFSAttendeesMissing) );

    if ( !attendeeCount )
        {
        DoLeaveIfErrorL( KErrArgument );
        }

    for (TInt i(0); i < attendeeCount; ++i)
        {
        iEmailMgr->AppendToRecipientL(
                requiredAttendees[i]->Address(),
                requiredAttendees[i]->CommonName() );
        }

    TInt optAttendeeCount( optionalAttendees.Count() );
    for (TInt i(0); i < optAttendeeCount; ++i)
        {
        iEmailMgr->AppendToRecipientL(
                optionalAttendees[i]->Address(),
                optionalAttendees[i]->CommonName() );
        }

    CleanupStack::PopAndDestroy(2, &requiredAttendees );

    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailReplyTask::ResolveUsedMailboxUserAddressLC
// ---------------------------------------------------------------------------
//
HBufC* CESMRSendMRFSMailReplyTask::ResolveUsedMailboxUserAddressLC()
    {
    FUNC_LOG;

    HBufC* mailboxUserAddress = NULL;
    MESMRMeetingRequestEntry& mrEntry( ESMREntry());
    if ( mrEntry.IsForwardedL() )
        {
        // Entry is forwarder --> Use default mailbox
        CMRMailboxUtils::TMailboxInfo mbInfo;
        CMRMailboxUtils& mbUtils( MailboxUtils() );
        mbUtils.GetDefaultMRMailBoxL( mbInfo );
        mailboxUserAddress = mbInfo.iEmailAddress.AllocLC();
        }
    else
        {
        mailboxUserAddress = mrEntry.CalendarOwnerAddressL().AllocLC(); ;
        }

    return mailboxUserAddress;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailReplyTask::LaunchFSEmailEditorL
// ---------------------------------------------------------------------------
//
TInt CESMRSendMRFSMailReplyTask::LaunchFSEmailEditorL(TBool aReplAll)
    {
    FUNC_LOG;
    CCoeEnv* coe = CCoeEnv::Static();

    CCoeAppUi* appui = coe->AppUi();

    // fill params
    TEditorLaunchParams params;
    params.iActivatedExternally = ETrue;
    params.iMailboxId = iEmailMgr->MailboxId();
    params.iFolderId = iEmailMgr->MessageFolderId();
    params.iMsgId = iEmailMgr->MessageId();

    TVwsViewId viewId(
            KFSEmailUiUid,
            MailEditorId );

    TUid editorCommand;

    if(aReplAll)
        {
        editorCommand = TUid::Uid( KEditorCmdReplyAll );
        }
    else
        {
        editorCommand = TUid::Uid( KEditorCmdReply );
        }

    TPckgC<TEditorLaunchParams> buf( params );

    appui->ActivateViewL(
            viewId,
            editorCommand,
            buf );

    return KErrNone;
    }

//EOF

