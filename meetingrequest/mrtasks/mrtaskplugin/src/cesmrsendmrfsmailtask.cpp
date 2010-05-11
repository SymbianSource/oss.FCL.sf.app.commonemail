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


#include "cesmrsendmrfsmailtask.h"

#include "cesmrcaldbmgr.h"
#include "mesmrmeetingrequestentry.h"
#include "cesmrfsemailmanager.h"
#include "cesmrmailplaitextformatter.h"
#include "cesmrcalimportexporter.h"
#include "cesmrcaluserutil.h"
#include "cesmrguilistquery.h"
#include "esmrhelper.h"
#include "cesmrfsmailboxutils.h"

#include <coemain.h>
#include <utf.h>
#include <bautils.h>
#include <caluser.h>
#include <cmrmailboxutils.h>
#include <calentry.h>
#include <calentryview.h>
#include <calattachment.h>

#include "emailtrace.h"


// Unnamed namespace for local definitions
namespace { // codescanner::namespace

/**
 * Adds updated label into summary field.
 */
void UpdateSummaryL(
        CCalEntry& aEntry,
        CESMRMailPlainTextFormatter& aPlainTextFormatter )
    {
    FUNC_LOG;
    TPtrC currentSummary( aEntry.SummaryL() );

    CCalEntry::TMethod method( aEntry.MethodL() );
    if ( CCalEntry::EMethodRequest == method && aEntry.SequenceNumberL() )
        {
        // This is update
        HBufC* updateStr = aPlainTextFormatter.UpdatedStringLC();
        HBufC* summary = HBufC::NewLC(
                            updateStr->Length() + currentSummary.Length() );

        if ( currentSummary.Find(*updateStr) == KErrNotFound )
            {
            TPtr summaryPtr( summary->Des() );
            summaryPtr.Append( *updateStr );
            summaryPtr.Append( currentSummary );
            aEntry.SetSummaryL( summaryPtr );
            }
        CleanupStack::PopAndDestroy( summary );
        summary = NULL;

        CleanupStack::PopAndDestroy( updateStr );
        updateStr = NULL;
        }
    else if ( CCalEntry::EMethodCancel == method )
        {
        // This is cancellation
        HBufC* cancelStr = aPlainTextFormatter.CanceledStringLC();
        HBufC* summary = HBufC::NewLC(
                cancelStr->Length() + currentSummary.Length() );

        if ( currentSummary.Find(*cancelStr) == KErrNotFound )
            {
            TPtr summaryPtr( summary->Des() );
            summaryPtr.Append( *cancelStr );
            summaryPtr.Append( currentSummary );
            aEntry.SetSummaryL( summaryPtr );
            }
        CleanupStack::PopAndDestroy( summary );
        summary = NULL;

        CleanupStack::PopAndDestroy( cancelStr );
        cancelStr = NULL;
        }
    }

/**
 * Cleanup operation for RPointerArray.
 *
 * @param aArray Pointer to RPointerArray.
 */
void CalAttendeePointerArrayCleanup( TAny* aArray )
    {
    RPointerArray<CCalAttendee>* attendeeArray =
        static_cast<RPointerArray<CCalAttendee>*>( aArray );

        attendeeArray->ResetAndDestroy();
        attendeeArray->Close();
    }

#ifdef _DEBUG

// Literal for panics
_LIT( ESMRSendMRRespMailPanicTxt, "ESMRSendMRFSEMailTask" );

// Panic codes
enum TESMRSendMRFSMailTaskPanic
    {
    EESMRSendMRFSMailInvalidMethod, // Entry has invalid method
    EESMRSendMRFSAttendeesMissing,  // There are no any attendees in entry to be sent
    EESMRSendMRNoEntriesToSend,     // There are no entries to be sent
    EESMRSendMRPhoneOwnerNotSet     // Phone owner is not set
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


// Definition for temporary calendar file
//<cmail> remove hard coded paths
//_LIT( KPath, "c:\\temp\\" ); //codescanner::driveletters
//_LIT( KFileAndPath, "c:\\temp\\meeting.ics" ); //codescanner::driveletters
//_LIT( KCancellationFile, "c:\\temp\\cancel.ics" ); //codescanner::driveletters
_LIT( KFile, "temp\\meeting.ics" );
_LIT( KCancelFile, "temp\\cancel.ics" );
//</cmail>

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
// CESMRSendMRFSMailTask::CESMRSendMRFSMailTask
// ---------------------------------------------------------------------------
//
CESMRSendMRFSMailTask::CESMRSendMRFSMailTask(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TESMRCommand aCommand )
:   CESMRTaskBase( aCalDbMgr, aEntry, aMRMailboxUtils ),
    iCommand( aCommand )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::~CESMRSendMRFSMailTask
// ---------------------------------------------------------------------------
//
CESMRSendMRFSMailTask::~CESMRSendMRFSMailTask()
    {
    FUNC_LOG;
    iEntriesToSend.ResetAndDestroy();
    delete iCaluserUtil;
    delete iEmailMgr;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::NewL
// ---------------------------------------------------------------------------
//
CESMRSendMRFSMailTask* CESMRSendMRFSMailTask::NewL(
        MESMRCalDbMgr& aCalDbMgr,
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        TESMRCommand aCommand )
    {
    FUNC_LOG;

    CESMRSendMRFSMailTask* self =
        new (ELeave) CESMRSendMRFSMailTask(
                aCalDbMgr,
                aEntry,
                aMRMailboxUtils,
                aCommand );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailTask::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    iEmailMgr = CESMRFSEMailManager::NewL( MailboxUtils() );
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::ExecuteTaskL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailTask::ExecuteTaskL()
    {
    FUNC_LOG;
    CCalEntry* entry = ESMREntry().ValidateEntryL();
    CleanupStack::PushL( entry );    

    iCaluserUtil = CESMRCalUserUtil::NewL( *entry );

    CCalEntry::TMethod method( entry->MethodL() );

    // Checking input paramters
    if( !(CCalEntry::EMethodRequest == method ||
          CCalEntry::EMethodCancel == method) ||
          CCalEntry::EAppt != entry->EntryTypeL() )
        {
        DoLeaveIfErrorL( KErrArgument );
        }

    // Get entries to be sent
    GetEntriesToBeSentL();
    ResolveSendTypeL();

    if ( NeedToSendInvitationL() )
        {
        TInt entryCount( iEntriesToSend.Count() );
        for (TInt i(0); i < entryCount; ++i )
            {
            iEntryToSend = iEntriesToSend[i];
            ConstructMailL();
            TInt ret = iEmailMgr->SendMessageL();
            User::LeaveIfError(ret);
            }
        }

    if ( EESMRCmdSendMRUpdate == iCommand &&
         NeedToSendCancellationL() )
        {
        delete iEmailMgr; iEmailMgr = NULL;
        iEmailMgr = CESMRFSEMailManager::NewL( MailboxUtils() );

        TInt entryCount( iEntriesToSend.Count() );
        for (TInt i(0); i < entryCount; ++i )
            {
            // Sending cancellation to removed attendees
            iEntryToSend = iEntriesToSend[i];
            iEntryToSend->SetMethodL( CCalEntry::EMethodCancel );

            ConstructMailL();
            TInt ret = iEmailMgr->SendMessageL();
            User::LeaveIfError(ret);
            }
        }
    
    CleanupStack::PopAndDestroy( entry );
    }

// ---------------------------------------------------------------------------
// CESMRSendMRRespFSMailTask::ConstructMailL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailTask::ConstructMailL()
    {
    FUNC_LOG;

    // create text formatter:
    CESMRMailPlainTextFormatter* textFormatter =
            CESMRMailPlainTextFormatter::NewLC( MailboxUtils() );

    // Prepare email manager for sending
    HBufC* mbOwnerAddr = ResolveUsedMailboxUserAddressLC();
    TPtrC emailAddr( ESMRHelper::AddressWithoutMailtoPrefix( *mbOwnerAddr ) );
    iEmailMgr->PrepareForSendingL( emailAddr );

    TBool isForwarded( ESMREntry().IsForwardedL() );
    if ( isForwarded )
        {
        CCalUser* organizer = iEntryToSend->OrganizerL();
        CCalUser* forwardOrganizer =
                CCalUser::NewL(
                        organizer->Address(),
                        emailAddr );
        CleanupStack::PushL( forwardOrganizer );
        forwardOrganizer->SetCommonNameL( organizer->CommonName() );

        // Ownership is transferred to entry
        iEntryToSend->SetOrganizerL( forwardOrganizer );
        CleanupStack::Pop( forwardOrganizer );

        iEmailMgr->SetSenderL(
                forwardOrganizer->Address(),forwardOrganizer->CommonName() );

        iEmailMgr->SetReplyToAddressL(
                forwardOrganizer->Address(), forwardOrganizer->CommonName() );

        organizer = NULL;
        forwardOrganizer = NULL;
        }

    CleanupStack::PopAndDestroy( mbOwnerAddr );

    // set the email subject:
    HBufC* subject = NULL;
    if ( CCalEntry::EMethodRequest == iEntryToSend->MethodL() )
        {
        subject = textFormatter->Subject16LC(
                *iEntryToSend,
                ESMREntry().IsForwardedL(),
                EESMRCmdSendMRUpdate == iCommand  );
        }
    else
        {
        subject = textFormatter->Subject16LC(
                *iEntryToSend,
                ESMREntry().IsForwardedL(),
                EFalse );
        }

    iEmailMgr->SetSubjectL( *subject );
    CleanupStack::PopAndDestroy( subject );
    subject = NULL;

    AddAttendeesL();

    // Set text/plain part:
    HBufC* body = textFormatter->Body16LC( *iEntryToSend );
    iEmailMgr->CreateTextPlainPartL( *body );
    CleanupStack::PopAndDestroy(body);
    body = NULL;

    //  export the calendar entry in iCal format:
    UpdateSummaryL( *iEntryToSend, *textFormatter );

    TInt priority( iEntryToSend->PriorityL() );
    SetCorrectAttendeesToEntryL();
    CESMRCalImportExporter* calExporter = CESMRCalImportExporter::NewLC();
    HBufC* calendarPart = calExporter->ExportToICal16LC( *iEntryToSend );

    // Set text/calendar part:
    CESMRFSEMailManager::TESMRMethod method(
            CESMRFSEMailManager::EESMRMethodRequest);

    CCalEntry::TMethod entryMethod( iEntryToSend->MethodL() );
    switch ( entryMethod )
        {
        case CCalEntry::EMethodRequest:
            {
            method = CESMRFSEMailManager::EESMRMethodRequest;
            }
            break;
        case CCalEntry::EMethodCancel:
            {
            method = CESMRFSEMailManager::EESMRMethodCancel;
            }
            break;
        case CCalEntry::EMethodReply://fallthrough, EMethodReply not needed
        default:
            break;
        }

    if ( CCalEntry::EMethodCancel == entryMethod )
        {
        TFileName fileName(KCancelFile);
        User::LeaveIfError(ESMRHelper::CreateAndAppendPrivateDirToFileName(fileName));
        SaveICalToFileL( *calendarPart, fileName );
        iEmailMgr->CreateTextCalendarPartL(method, fileName);
        }
    else
        {
        TFileName fileName(KFile);
        User::LeaveIfError(ESMRHelper::CreateAndAppendPrivateDirToFileName(fileName));
        SaveICalToFileL( *calendarPart, fileName );
        iEmailMgr->CreateTextCalendarPartL(method, fileName);

        AddAttachmentsL();
        }

    CleanupStack::PopAndDestroy(3, textFormatter);
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::SaveICalToFileL
// ---------------------------------------------------------------------------
//
TInt CESMRSendMRFSMailTask::SaveICalToFileL( const TDesC& aICal,
                                             const TDesC& aFilename  )
    {
    FUNC_LOG;
    //create 8 bit buffer for temp file content
    TInt length = aICal.Length() * 2;
    HBufC8* buffer = HBufC8::NewLC( length );
    TPtr8 des = buffer->Des();

    // covert the 16 bit iCal to 8 bit iCal:
    TInt err = CnvUtfConverter::ConvertFromUnicodeToUtf8(des, aICal);

    RFs& fs( CCoeEnv::Static()->FsSession() );

    // ensure the path exists:
    //<cmail>remove hardcoded paths
    //BaflUtils::EnsurePathExistsL(fs, KPath);
    //</cmail>

    // delete previous file
    fs.Delete( aFilename );

    // save the iCal to file system:
    RFile file;
    User::LeaveIfError(file.Create(fs, aFilename, EFileWrite));
    CleanupClosePushL(file);
    User::LeaveIfError(file.Write(*buffer));

    // clean up:
    CleanupStack::PopAndDestroy(2, buffer); // file, fs, buffer

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::AddAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailTask::AddAttendeesL()
    {
    FUNC_LOG;

    RArray<CCalAttendee*> requiredAttendees;
    CleanupClosePushL( requiredAttendees );

    RArray<CCalAttendee*> optionalAttendees;
    CleanupClosePushL( optionalAttendees );

    GetAttendeesToBeSentL( requiredAttendees, optionalAttendees );

    // Add recipients and cc-recipients
    TInt attendeeCount( requiredAttendees.Count() );
    TInt optAttendeeCount( optionalAttendees.Count() );

    __ASSERT_DEBUG( attendeeCount || optAttendeeCount, Panic(EESMRSendMRFSAttendeesMissing) );

    if ( !attendeeCount && !optAttendeeCount)
        {
        DoLeaveIfErrorL( KErrArgument );
        }

    for (TInt i(0); i < attendeeCount; ++i)
        {
        TPtrC emailAddr( requiredAttendees[i]->Address() );


        iEmailMgr->AppendToRecipientL(
                emailAddr,
                requiredAttendees[i]->CommonName() );
        }

    for (TInt i(0); i < optAttendeeCount; ++i)
        {
        TPtrC emailAddr( optionalAttendees[i]->Address() );


        iEmailMgr->AppendCCRecipientL(
                emailAddr,
                optionalAttendees[i]->CommonName() );
        }

    CleanupStack::PopAndDestroy(2, &requiredAttendees );

    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::ResolveUsedMailboxUserAddressLC
// ---------------------------------------------------------------------------
//
HBufC* CESMRSendMRFSMailTask::ResolveUsedMailboxUserAddressLC()
    {
    FUNC_LOG;

    HBufC* mailboxUserAddress = NULL;
    MESMRMeetingRequestEntry& mrEntry( ESMREntry());
    if ( mrEntry.IsForwardedL() )
        {
        // Entry is forwarded --> Use default mailbox
        CMRMailboxUtils::TMailboxInfo mbInfo;
        CMRMailboxUtils& mbUtils( MailboxUtils() );
        mbUtils.GetDefaultMRMailBoxL( mbInfo );
        mailboxUserAddress = mbInfo.iEmailAddress.AllocLC();
        }
    else
        {
        mailboxUserAddress = mrEntry.CalendarOwnerAddressL().AllocLC();
        }

    return mailboxUserAddress;
    }


// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::ResolveSendTypeL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailTask::ResolveSendTypeL()
    {
    FUNC_LOG;

    if ( EESMRCmdSendMRUpdate == iCommand )
        {
        RArray<CCalAttendee*> addedAttendees;
        CleanupClosePushL( addedAttendees );

        RArray<CCalAttendee*> removedAttendees;
        CleanupClosePushL( removedAttendees );

        ESMREntry().GetAddedAttendeesL(
                addedAttendees,
                EESMRRoleRequiredAttendee | EESMRRoleOptionalAttendee );
        ESMREntry().GetRemovedAttendeesL(
                removedAttendees,
                EESMRRoleRequiredAttendee | EESMRRoleOptionalAttendee );

        if ( addedAttendees.Count() || removedAttendees.Count() )
            {
            TInt sendType = CESMRGUIListQuery::ExecuteL(
                    CESMRGUIListQuery::EESMRSendUpdateToAllQuery );

            if ( KErrCancel == sendType )
                {
                User::Leave( KErrCancel );
                }

            iSendType = static_cast<TESMRSendType>(sendType );
            }

        CleanupStack::PopAndDestroy( &removedAttendees );
        CleanupStack::PopAndDestroy( &addedAttendees );
        }

    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::NeedToSendInvitationL
// ---------------------------------------------------------------------------
//
TBool CESMRSendMRFSMailTask::NeedToSendInvitationL()
    {
    FUNC_LOG;

    TBool retValue( ETrue );

    RArray<CCalAttendee*> addedAttendees;
    CleanupClosePushL( addedAttendees );

    if ( EESMRCmdSendMRUpdate == iCommand &&
         ESentToAddedAndRemoved == iSendType )
        {
        ESMREntry().GetAddedAttendeesL(
                addedAttendees,
                EESMRRoleRequiredAttendee | EESMRRoleOptionalAttendee );

        if ( !addedAttendees.Count() )
            {
            retValue = EFalse;
            }
        }

    CleanupStack::PopAndDestroy( &addedAttendees );


    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::NeedToSendCancellationL
// ---------------------------------------------------------------------------
//
TBool CESMRSendMRFSMailTask::NeedToSendCancellationL()
    {
    FUNC_LOG;

    TBool retValue( EFalse );

    RArray<CCalAttendee*> removedAttendees;
    CleanupClosePushL( removedAttendees );

    if ( EESMRCmdSendMRUpdate == iCommand  )
        {
        ESMREntry().GetRemovedAttendeesL(
                removedAttendees,
                EESMRRoleRequiredAttendee | EESMRRoleOptionalAttendee );

        if ( removedAttendees.Count() )
            {
            retValue = ETrue;
            }
        }

    CleanupStack::PopAndDestroy( &removedAttendees );


    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::NeedToSendCancellationL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailTask::SetCorrectAttendeesToEntryL()
    {
    FUNC_LOG;
    if ( EESMRCmdSendMRUpdate == iCommand )
        {
        TESMRMailPlugin plugin( ESMREntry().CurrentPluginL() );

        CCalEntry::TMethod method( iEntryToSend->MethodL() );

        RArray<CCalAttendee*> requiredAttendees;
        CleanupClosePushL( requiredAttendees );

        RArray<CCalAttendee*> optionalAttendees;
        CleanupClosePushL( optionalAttendees );

        GetAttendeesToBeSentL( requiredAttendees, optionalAttendees );

        RPointerArray<CCalAttendee> attendees;
        CleanupStack::PushL(
                    TCleanupItem(
                        CalAttendeePointerArrayCleanup,
                        &attendees    ) );


        TInt attendeeCount( requiredAttendees.Count() );
        for ( TInt i(0); i < attendeeCount; ++i )
            {
            // Make copy of the attendees
            CCalAttendee* attendee = ESMRHelper::CopyAttendeeL(*requiredAttendees[i] );
            User::LeaveIfError(
                    attendees.Append( attendee  ) );
            }

        TInt optionalAttendeeCount( optionalAttendees.Count() );
        for ( TInt i(0); i < optionalAttendeeCount; ++i )
            {
            // Make copy of the attendees
            CCalAttendee* attendee = ESMRHelper::CopyAttendeeL(*optionalAttendees[i] );
            User::LeaveIfError(
                    attendees.Append( attendee  ) );
            }

        // Clean attendee array
        while ( iEntryToSend->AttendeesL().Count() )
            {
            iEntryToSend->DeleteAttendeeL( 0 );
            }

        while ( attendees.Count() )
            {
            CCalAttendee* attendee = attendees[0];
            iEntryToSend->AddAttendeeL( attendee );
            attendees.Remove(0);
            }

        CleanupStack::PopAndDestroy( &attendees );
        CleanupStack::PopAndDestroy( &optionalAttendees );
        CleanupStack::PopAndDestroy( &requiredAttendees );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::NeedToSendCancellationL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailTask::GetAttendeesToBeSentL(
        RArray<CCalAttendee*>& aRequiredAttendees,
        RArray<CCalAttendee*>& aOptionalAttendees )
    {
    FUNC_LOG;
    CCalEntry::TMethod method( iEntryToSend->MethodL() );

    if ( EESMRCmdSendMRUpdate == iCommand &&
         ESentToAddedAndRemoved == iSendType &&
         CCalEntry::EMethodRequest == method )
        {
        ESMREntry().GetAddedAttendeesL(
                aRequiredAttendees,
                EESMRRoleRequiredAttendee );
        ESMREntry().GetAddedAttendeesL(
                aOptionalAttendees,
                EESMRRoleOptionalAttendee );
        }
    else if (EESMRCmdSendMRUpdate == iCommand &&
            CCalEntry::EMethodCancel == method )
        {
        ESMREntry().GetRemovedAttendeesL(
                aRequiredAttendees,
                EESMRRoleRequiredAttendee );
        ESMREntry().GetRemovedAttendeesL(
                aOptionalAttendees,
                EESMRRoleOptionalAttendee );
        }
    else
        {
        iCaluserUtil->GetAttendeesL(
                aRequiredAttendees,
                EESMRRoleRequiredAttendee );
        iCaluserUtil->GetAttendeesL(
                aOptionalAttendees,
                EESMRRoleOptionalAttendee );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::GetEntriesToBeSentL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailTask::GetEntriesToBeSentL()
    {
    FUNC_LOG;
    // Clear the entry array
    iEntriesToSend.ResetAndDestroy();

    if ( EESMRCmdSendMRUpdate == iCommand )
        {
        // We are sending update and we need to send all the entries
        CCalEntry* mainEntry = ESMREntry().ValidateEntryL();
        CleanupStack::PushL( mainEntry );

        if ( MESMRCalEntry::EESMRAllInSeries == ESMREntry().RecurrenceModRule() )
            {
            // Update child entries sequence number
            ESMREntry().UpdateChildEntriesSeqNumbersL();

            // We are sending update to series
            // Also the modifying entries need to be sent
            // Otherwise the modifying entries may disappear from the
            // attendees calendar.
            CalDbMgr().EntryViewL( *mainEntry )->FetchL(
                    mainEntry->UidL(),
                    iEntriesToSend );
            // set the phone owner for the series
            CESMRFsMailboxUtils* fsMbUtils =CESMRFsMailboxUtils::NewL( MailboxUtils() );
            CleanupStack::PushL( fsMbUtils );
            for (TInt i =0 ; i < iEntriesToSend.Count(); i++)
            	{
            	fsMbUtils->SetPhoneOwnerL( *iEntriesToSend[i] );
            	}
            CleanupStack::PopAndDestroy( fsMbUtils );
            fsMbUtils = NULL;
            CleanupStack::PopAndDestroy( mainEntry );
            }
        else
            {
            // Ownership transferred
            TInt err = iEntriesToSend.Append( mainEntry );
            User::LeaveIfError( err );

            CleanupStack::Pop( mainEntry );
            }

        mainEntry = NULL;
        }
    else
        {
        // We are sending either:
        // a) Single entry
        // b) Modified instance from series
        // c) First invitation to series
        // d) forwarding entry
        TInt err = iEntriesToSend.Append( ESMREntry().ValidateEntryL() );
        User::LeaveIfError( err );
        }

    __ASSERT_DEBUG( iEntriesToSend.Count(), Panic(EESMRSendMRNoEntriesToSend ) );
    }

// ---------------------------------------------------------------------------
// CESMRSendMRFSMailTask::AddAttachmentsL
// ---------------------------------------------------------------------------
//
void CESMRSendMRFSMailTask::AddAttachmentsL()
    {
    FUNC_LOG;

    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );

    User::LeaveIfError( fsSession.ShareProtected() );

    TInt attachmentCount( iEntryToSend->AttachmentCountL() );

    for ( TInt i(0); i < attachmentCount; ++i )
        {
        CCalAttachment* attachment = iEntryToSend->AttachmentL( i );

        if ( CCalAttachment::EFile == attachment->Type() )
            {
            // Adding file attachment to MR email message
            CCalAttachmentFile* fileAttachment = attachment->FileAttachment();

            RFile file;
            CleanupClosePushL( file );
            fileAttachment->FetchFileHandleL( file );

            iEmailMgr->SetAttachmentL(
                    file,
                    attachment->MimeType() );

            TESMRMailPlugin plugin( ESMREntry().CurrentPluginL() );
            if ( EESMRImapPop == plugin )
                {
                CleanupStack::Pop( &file );
                }
            else
                {
                CleanupStack::PopAndDestroy( &file );
                }
            }
        }

    CleanupStack::PopAndDestroy( &fsSession );
    }

// EOF
