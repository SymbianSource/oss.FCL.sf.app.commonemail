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
* Description:  ESMRIcalViewer utility methods
*
*/


#include "emailtrace.h"
#include "esmricalviewerutils.h"
#include "cfsmailmessagepart.h"
#include "cfsmailclient.h"
#include "cfsmailmessage.h"
#include "cfsmailbox.h"

#include <calentry.h>
#include <caluser.h>
#include <cmrmailboxutils.h>

// Unnamed namespace for local definitions
namespace {

const TInt KZero( 0 );
_LIT( KTextCalendar, "text/calendar");

}//namespace

// ---------------------------------------------------------------------------
// ESMRIcalViewerUtils::EmailOperation
// ---------------------------------------------------------------------------
//
TESMRIcalViewerOperationType ESMRIcalViewerUtils::EmailOperation(
        TInt aCommandId )
    {
    FUNC_LOG;
    TESMRIcalViewerOperationType command( EESMRCmdOpenAttachment );

    switch ( aCommandId )
        {
        case EESMRCmdAcceptMR://fallthrough
        case EESMRCmdTentativeMR://fallthrough
        case EESMRCmdDeclineMR:
            {
            command = EESMRCmdOpenAttachmentView;
            }
        break;
        default:
            {
            command = static_cast<TESMRIcalViewerOperationType>( aCommandId );
            }
        break;
        }

    return command;
    }

// ---------------------------------------------------------------------------
// ESMRIcalViewerUtils::ContainsAttachments
// ---------------------------------------------------------------------------
//
TBool ESMRIcalViewerUtils::ContainsAttachments(
        TESMRInputParams& /*aParams*/ )
    {
    FUNC_LOG;
    TBool retValue( EFalse );

    // TODO: This needs to be updated
    /*if ( aParams.iAttachmentInfo &&
         aParams.iAttachmentInfo->AttachmentCount() )
        {
        retValue = ETrue;
        } */

    return retValue;
    }

// ---------------------------------------------------------------------------
// ESMRIcalViewerUtils::IsAsyncEmailCommand
// ---------------------------------------------------------------------------
//
TBool ESMRIcalViewerUtils::IsAsyncEmailCommand(
        TESMRIcalViewerOperationType aCommand )
    {
    FUNC_LOG;
    TBool retValue( ETrue );

    if ( EESMRCmdMailMarkUnread == aCommand ||
         EESMRCmdMailMarkRead == aCommand ||
         EESMRCmdMailFlagMessage == aCommand )
        {
        retValue = EFalse;
        }

    return retValue;
    }

// ---------------------------------------------------------------------------
// ESMRIcalViewerUtils::MRResponseCommandWithoutAttachmentCheck
// ---------------------------------------------------------------------------
//
TESMRCommand ESMRIcalViewerUtils::MRResponseCommandWithoutAttachmentCheck(
        TInt aCommand )
    {
    FUNC_LOG;
    TESMRCommand command( EESMRAcceptWithoutAttachmentCheck );

    switch ( aCommand )
        {
        case EESMRCmdAcceptMR:
            {
            command = EESMRAcceptWithoutAttachmentCheck;
            break;
            }
        case EESMRCmdTentativeMR:
            {
            command = EESMRTentativeWithoutAttachmentCheck;
            break;
            }
        case EESMRCmdDeclineMR:
            {
            command = EESMRDeclineWithoutAttachmentCheck;
            break;
            }
        default:
            break;
        }

    return command;
    }

// ---------------------------------------------------------------------------
// ESMRIcalViewerUtils::LocateChildPartLC
// ---------------------------------------------------------------------------
//
CFSMailMessagePart* ESMRIcalViewerUtils::LocateChildPartLC(
        CFSMailMessagePart& aParentPart,
        const TDesC& aMimeType )
    {
    FUNC_LOG;
    CFSMailMessagePart* child = NULL;

    TBool partFound( EFalse );

    RPointerArray<CFSMailMessagePart> messageParts;
    CleanupStack::PushL(
            TCleanupItem(
                ESMRIcalViewerUtils::MessagePartPointerArrayCleanup,
                &messageParts    ) );

    aParentPart.ChildPartsL( messageParts );

    TInt msgPartCount( messageParts.Count() );
    for ( TInt i(0); (i < msgPartCount) && !partFound; ++i )
        {
        CFSMailMessagePart* part = messageParts[i];

        HBufC* contentType = part->GetContentType().AllocLC();
        TPtr ptrContentType( contentType->Des() );
        ptrContentType.LowerCase();

        // Check part's content type
        if ( KErrNotFound != ptrContentType.Find( aMimeType ) )
            {
            partFound = ETrue;
            child = messageParts[i];
            messageParts.Remove(i);
            }
        CleanupStack::PopAndDestroy( contentType );
        contentType = NULL;
        }

    CleanupStack::PopAndDestroy(); // messageParts

    CleanupStack::PushL( child );
    return child;
    }

// ---------------------------------------------------------------------------
// ESMRIcalViewerUtils::LocateCalendarPartL
// ---------------------------------------------------------------------------
//
CFSMailMessagePart* ESMRIcalViewerUtils::LocateCalendarPartL( CFSMailMessage& aMessage )
    {
    FUNC_LOG;

    CFSMailMessagePart* calendarPart( NULL );
    CFSMailMessagePart* multipart( NULL );

    // 1st, try to find 'TEXT/CALENDAR' part from 'MULTIPART/ALTERNATIVE' part of message
    multipart = ESMRIcalViewerUtils::LocateChildPartLC(
        aMessage,
        KFSMailContentTypeMultipartAlternative );
    if ( multipart )
        {
        // multipart found
        calendarPart = ESMRIcalViewerUtils::LocateChildPartLC( *multipart, KTextCalendar );
        if ( calendarPart )
            {
            // calendar part under multipart found
            CleanupStack::Pop( calendarPart );
            CleanupStack::PopAndDestroy( multipart );
            multipart = NULL;
            CleanupStack::PushL( calendarPart );
            }
        else
            {
            // no calendar part under multipart
            CleanupStack::PopAndDestroy( 2 );
            multipart = NULL;
            calendarPart = NULL;
            }
        }
    else
        {
        CleanupStack::PopAndDestroy( multipart );
        }

    if ( !calendarPart )
        {
        // If there's no 'MULTIPART/ALTERNATIVE' or it doesn't have
        // 'TEXT/CALENDAR' entry, try looking from message root
        calendarPart = ESMRIcalViewerUtils::LocateChildPartLC(
            aMessage,
            KTextCalendar );
        }
    CleanupStack::Pop();
    return calendarPart;
    }

// ---------------------------------------------------------------------------
// ESMRIcalViewerUtils::MessagePartPointerArrayCleanup
// ---------------------------------------------------------------------------
//
void ESMRIcalViewerUtils::MessagePartPointerArrayCleanup( TAny* aArray )
    {
    FUNC_LOG;
    RPointerArray<CFSMailMessagePart>* messagePartArray =
        static_cast<RPointerArray<CFSMailMessagePart>*>( aArray );

    messagePartArray->ResetAndDestroy();
    messagePartArray->Close();
    }

// ---------------------------------------------------------------------------
// ESMRIcalViewerUtils::UserResponsedToMRL
// ---------------------------------------------------------------------------
//
TESMRAttendeeStatus ESMRIcalViewerUtils::UserResponsedToMRL(
            CCalEntry& aEntry )
    {
    FUNC_LOG;
    TESMRAttendeeStatus attendeeStatus( EESMRAttendeeStatusUnknown );

    CCalEntry::TStatus status( aEntry.StatusL() );

    if ( status == CCalEntry::EConfirmed  )
        {
        CMRMailboxUtils* mbUtils =
                CMRMailboxUtils::NewL();

        CleanupStack::PushL( mbUtils );

        CCalAttendee* attendee = mbUtils->ThisAttendeeL( aEntry );

        if ( attendee )
            {
            CCalAttendee::TCalStatus status( attendee->StatusL() );

            switch( status )
                {
                case CCalAttendee::EAccepted:
                    {
                    attendeeStatus = EESMRAttendeeStatusAccept;
                    break;
                    }
                case CCalAttendee::ETentative:
                    {
                    attendeeStatus = EESMRAttendeeStatusTentative;
                    break;
                    }
                case CCalAttendee::EDeclined:
                    {
                    attendeeStatus = EESMRAttendeeStatusDecline;
                    break;
                    }
                default:
                    {
                    attendeeStatus = EESMRAttendeeStatusUnknown;
                    break;
                    }
                }
            }

        CleanupStack::PopAndDestroy( mbUtils );
        mbUtils = NULL;
        }

    return attendeeStatus;
    }

// ---------------------------------------------------------------------------
// ESMRIcalViewerUtils::MailboxOwnerRoleL
// ---------------------------------------------------------------------------
//
TESMRRole ESMRIcalViewerUtils::MailboxOwnerRoleL(
        CCalEntry& aEntry,
        CFSMailClient& aMailClient,
        CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    TESMRRole role( EESMRRoleUndef );

    TFSMailMsgId mailboxId( aMessage.GetMailBoxId() );
    CFSMailBox* mailbox = aMailClient.GetMailBoxByUidL( mailboxId );

    CleanupStack::PushL( mailbox );

    TPtrC mailboxOwnerAddName(
            mailbox->OwnMailAddress().GetEmailAddress() );

    CCalUser* organizer = aEntry.OrganizerL();

    if ( organizer &&  KZero == organizer->Address().CompareF(mailboxOwnerAddName) )
        {
        role = EESMRRoleOrganizer;
        }
    else
        {
        RPointerArray<CCalAttendee>& attendees = aEntry.AttendeesL();
        TInt attendeeCount( attendees.Count() );

        for( TInt i(0); i < attendeeCount; ++i )
            {
            CCalAttendee* attendee = attendees[i];
            if ( KZero == attendee->Address().CompareF(mailboxOwnerAddName) )
                {
                switch( attendee->RoleL() )
                    {
                    case CCalAttendee::EChair:
                    case CCalAttendee::EReqParticipant:
                        {
                        role = EESMRRoleRequiredAttendee;
                        }
                    break;
                    case CCalAttendee::EOptParticipant:
                        {
                        role = EESMRRoleOptionalAttendee;
                        }
                    break;
                    case CCalAttendee::ENonParticipant:
                        {
                        role = EESMRRoleNonParticipant;
                        }
                    break;
                    default:
                        {
                        role = EESMRRoleUndef;
                        }
                    break;
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy( mailbox );

    return role;
    }

// ---------------------------------------------------------------------------
// ESMRIcalViewerUtils::IsMailboxOwnerAttendeeL
// ---------------------------------------------------------------------------
//
TBool ESMRIcalViewerUtils::IsMailboxOwnerAttendeeL(
        CCalEntry& aEntry,
        CFSMailClient& aMailClient,
        CFSMailMessage& aMessage )
    {
    FUNC_LOG;
    TBool retValue( EFalse );

    TESMRRole role(
            ESMRIcalViewerUtils::MailboxOwnerRoleL(
                    aEntry,
                    aMailClient,
                    aMessage ) );

    if ( EESMRRoleRequiredAttendee == role ||
            EESMRRoleOptionalAttendee == role ||
            EESMRRoleNonParticipant == role )
        {
        retValue = ETrue;
        }

    return retValue;
    }

// EOF

