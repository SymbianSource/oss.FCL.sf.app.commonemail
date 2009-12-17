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
#include "cesmrmeetingrequestsender.h"

#include <cmrmailboxutils.h>
#include <coemain.h>
#include <f32file.h>
#include <calentry.h>
#include <caluser.h>
#include <utf.h>
#include "mesmrmeetingrequestentry.h"
#include "cesmrcalimportexporter.h"
#include "esmrentryhelper.h"
#include "esmrhelper.h"
#include "cesmrmailplaitextformatter.h"

// Unnamed namespace for local definitions
namespace {

const TInt KMRBufferSize = 16 * 1024;

_LIT8( KMethodRequest, "REQUEST" );
_LIT8( KMethodResponse, "RESPONSE" );
_LIT8( KMethodCancel, "CANCEL" );
_LIT8( KNewLine, "\n" );

//<cmail> remove hardcoded paths
//_LIT( KNCFileName, "c:\\temp\\mail.tmp" ); //codescanner::driveletters
_LIT( KNCFile, "temp\\mail.tmp" );
//</cmail>
_LIT8( KStart, "EHLO hello\n");
_LIT8( KEnd, ".\nQUIT\n" );

_LIT8( KMailFromStart, "MAIL FROM:<");
_LIT8( KRecipientTo, "RCPT TO:<");
_LIT8( KAddressEnd, ">\n");
_LIT8( KDataStart, "DATA\n" );
_LIT8( KTo, "To:");


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructs file for nc application. NC application can further
// send the file to recipients.
// ---------------------------------------------------------------------------
//
void StoreToFileL(
        MESMRMeetingRequestEntry& aEntry,
        CMRMailboxUtils& aMailboxUtils,
        const TDesC& aFreePlainText )
    {
    FUNC_LOG;
    HBufC8* mrBuffer = HBufC8::NewLC( KMRBufferSize );
    TPtr8 mrBufPtr( mrBuffer->Des() );
    CESMRMailPlainTextFormatter* mailPlainTextFormatter =
        CESMRMailPlainTextFormatter::NewLC( aMailboxUtils );

    mrBufPtr.Append(KStart);

    CCalEntry& entry = aEntry.Entry();

    CCalUser* sender = entry.PhoneOwnerL();

    if ( !sender )
        {
        User::Leave( KErrArgument);
        }

    mrBufPtr.Append( KMailFromStart );
    mrBufPtr.Append(
        ESMRHelper::AddressWithoutMailtoPrefix( sender->Address() ) );
    mrBufPtr.Append( KAddressEnd );

    if ( entry.MethodL() == CCalEntry::EMethodReply )
        {
        CCalUser* organizer = NULL;
        TRAP_IGNORE( organizer = entry.OrganizerL() );

        mrBufPtr.Append(KRecipientTo() );
        mrBufPtr.Append(
            ESMRHelper::AddressWithoutMailtoPrefix( organizer->Address() ) );
        mrBufPtr.Append(KAddressEnd());
        }
    else
        {
        RPointerArray<CCalAttendee>& attendees = entry.AttendeesL();
        TInt attendeeCount( attendees.Count() );
        for (TInt i(0); i < attendeeCount; ++i )
            {
            mrBufPtr.Append(KRecipientTo() );
            mrBufPtr.Append(
                ESMRHelper::AddressWithoutMailtoPrefix( attendees[i]->Address() ) );
            mrBufPtr.Append(KAddressEnd());
            }
        }

    mrBufPtr.Append( KDataStart );

    _LIT(KDateFormat,"%*E%,%D%*N %1 %2");
    _LIT( KDateStart, "Date: " );

    TTime time; time.UniversalTime();
    TDateTime dTime = time.DateTime();
    TBuf<30> dateString;
    time.FormatL(dateString, KDateFormat );
    mrBufPtr.Append( KDateStart );
    mrBufPtr.Append( dateString );

    _LIT8( KFormat, " %d %02d:%02d:%02d +0000\n" );
    mrBufPtr.AppendFormat( KFormat, dTime.Year(), dTime.Hour(), dTime.Minute(), dTime.Second() );

    if ( entry.MethodL() == CCalEntry::EMethodReply )
        {
        // Leavescan fix
        CCalUser* organizer = NULL;
        TRAP_IGNORE( organizer = entry.OrganizerL() );

        mrBufPtr.Append( KTo );
        mrBufPtr.Append(
            ESMRHelper::AddressWithoutMailtoPrefix( organizer->Address() ) );
        mrBufPtr.Append( KNewLine );
        }
    else
        {mrBufPtr.Append( KTo );
        RPointerArray<CCalAttendee>& attendees = entry.AttendeesL();
        TInt attendeeCount( attendees.Count() );
        for (TInt i(0); i < attendeeCount; ++i )
            {

            _LIT8( KComma, ",");
            _LIT8( KStart, "<");
            _LIT8( KEnd, "<");
            mrBufPtr.Append( KStart );
            mrBufPtr.Append(
                ESMRHelper::AddressWithoutMailtoPrefix( attendees[i]->Address() ) );
            mrBufPtr.Append( KEnd );
            mrBufPtr.Append( KComma );
            }
            mrBufPtr.Append( KNewLine );
        }

    _LIT8( KReplyTo, "Reply-To: ");
    mrBufPtr.Append( KReplyTo );
        mrBufPtr.Append(
        ESMRHelper::AddressWithoutMailtoPrefix( sender->Address() ) );
    mrBufPtr.Append( KNewLine );

    _LIT8( KFrom, "From: " );
    mrBufPtr.Append( KFrom );
    mrBufPtr.Append(
        ESMRHelper::AddressWithoutMailtoPrefix( sender->Address() ) );
    mrBufPtr.Append( KNewLine );

    _LIT8( KSubject, "Subject: " );
    mrBufPtr.Append( KSubject );
    HBufC8* subject = mailPlainTextFormatter->Subject8LC(aEntry);
    mrBufPtr.Append( *subject );
    CleanupStack::PopAndDestroy( subject );
    mrBufPtr.Append( KNewLine );

    _LIT8(KMimeVer, "MIME-Version: 1.0\n" );
    mrBufPtr.Append( KMimeVer );

    _LIT8( KMailContentType, "Content-Type: multipart/alternative;\n        boundary=\"EPOC32-cqFgWS+P6_g42_fH-ySFynWD'-TgP+tX+tHKMDhNp-fmZFHj\"\n" );
    mrBufPtr.Append( KMailContentType );

    _LIT8( XParams, "X-imss-version: 2.043\nX-imss-result: Passed\nX-imss-scores: Clean:45.41865 C:2 M:3 S:5 R:5\nX-imss-settings: Baseline:2 C:1 M:1 S:1 R:1 (0.1500 0.1500)\n\nThis is a MIME Message\n\n" );
    mrBufPtr.Append( XParams );

    _LIT8( KPlainBoundary, "--EPOC32-cqFgWS+P6_g42_fH-ySFynWD'-TgP+tX+tHKMDhNp-fmZFHj\nContent-Type: text/plain; charset=ISO-8859-1\nContent-Disposition: inline\nContent-Transfer-Encoding: quoted-printable\n\n" );
    mrBufPtr.Append( KPlainBoundary );

    HBufC8* bodyText = mailPlainTextFormatter->Body8LC( aEntry );
    mrBufPtr.Append( *bodyText );
    CleanupStack::PopAndDestroy( bodyText );

    if ( aFreePlainText.Length() )
        {
        _LIT8( KFreeTextSeparatorTxt, "------------------------------------------------------------------------\n" );
        mrBufPtr.Append( KFreeTextSeparatorTxt );

        HBufC8* freeTxt8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aFreePlainText );
        mrBufPtr.Append( *freeTxt8 );
        mrBufPtr.Append( KNewLine );
        mrBufPtr.Append( KNewLine );

        delete freeTxt8;
        }

    _LIT( KMRBoundary, "--EPOC32-cqFgWS+P6_g42_fH-ySFynWD'-TgP+tX+tHKMDhNp-fmZFHj\nContent-Type: text/calendar;\n        method=");
    mrBufPtr.Append( KMRBoundary );

    if ( entry.MethodL() == CCalEntry::EMethodRequest )
        {
        mrBufPtr.Append( KMethodRequest );
        }
    else if ( entry.MethodL() == CCalEntry::EMethodCancel )
        {
        mrBufPtr.Append( KMethodCancel );
        }
    else
        {
        mrBufPtr.Append( KMethodResponse );
        }
    mrBufPtr.Append( KNewLine );

    _LIT( KMRBoundaryEnd, "        name=\"meeting.ics\"\nContent-Transfer-Encoding: 8bit\n\n" );
    mrBufPtr.Append( KMRBoundaryEnd );


    CESMRCalImportExporter* calExporter = CESMRCalImportExporter::NewLC();

    HBufC8* iCal = calExporter->ExportToICal8LC( entry );
    mrBufPtr.Append( *iCal );

    CleanupStack::PopAndDestroy( iCal );
    CleanupStack::PopAndDestroy(calExporter);

    mrBufPtr.Append( KNewLine );


    _LIT(KBoundaryEnd, "--EPOC32-cqFgWS+P6_g42_fH-ySFynWD'-TgP+tX+tHKMDhNp-fmZFHj--\n\n\n" );
    mrBufPtr.Append( KBoundaryEnd );
    mrBufPtr.Append( KEnd);

    RFile ncFile;
    RFs& fs = CCoeEnv::Static()->FsSession();
    //<cmail> remove hard coded paths
    //TInt err = fs.MkDirAll( KNCFileName() );
    TFileName fileName(KNCFile);
    User::LeaveIfError(ESMRHelper::CreateAndAppendPrivateDirToFileName(fileName));
    TInt err = ncFile.Replace( fs, fileName, EFileWrite );
    //</cmail>
    if (KErrAlreadyExists != err )
        {
        User::LeaveIfError( err );
        }

    CleanupClosePushL( ncFile );
    User::LeaveIfError( ncFile.Write(mrBufPtr) );
    CleanupStack::PopAndDestroy( &ncFile );
    // mrBuffer, mailPlainTextFormatter
    CleanupStack::PopAndDestroy( 2, mrBuffer );
    }

}  // namespace


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRMeetingRequestSender::CESMRMeetingRequestSender
// ---------------------------------------------------------------------------
//
inline CESMRMeetingRequestSender::CESMRMeetingRequestSender(
        CMRMailboxUtils& aMailboxUtils) :
        iMailboxUtils( aMailboxUtils )

    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestSender::~CESMRMeetingRequestSender
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRMeetingRequestSender::~CESMRMeetingRequestSender()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestSender::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRMeetingRequestSender*
    CESMRMeetingRequestSender::NewL(
        CMRMailboxUtils& aMailboxUtils )
    {
    CESMRMeetingRequestSender* self =
        new (ELeave) CESMRMeetingRequestSender( aMailboxUtils );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestSender::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestSender::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }


// ---------------------------------------------------------------------------
// CESMRMeetingRequestSender::CreateAndSendMeetingRequestL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestSender::CreateAndSendMeetingRequestL(
                MESMRMeetingRequestEntry& aEntry )
    {
    FUNC_LOG;
    StoreToFileL( aEntry, iMailboxUtils, KNullDesC() );
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestSender::CreateAndSendMeetingRequestL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestSender::CreateAndSendMeetingRequestL(
                MESMRMeetingRequestEntry& aEntry,
                const TDesC& aFreePlainText )
    {
    FUNC_LOG;
    StoreToFileL( aEntry, iMailboxUtils, aFreePlainText );
    }

