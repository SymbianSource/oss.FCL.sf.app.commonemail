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
* Description:  Implementation for plain text formatter for ES MR entries
*
*/

#include "emailtrace.h"
#include "cesmrmailplaitextformatter.h"

#include <esmrtasks.rsg>
#include <cmrmailboxutils.h>
#include <StringLoader.h>
#include <calentry.h>
#include <caluser.h>
#include <avkon.rsg>
#include <utf.h>
#include <data_caging_path_literals.hrh>
#include "cesmrcaluserutil.h"
#include "esmrhelper.h"
#include <bautils.h>

// Unnamed namespace for local definitions
namespace  {

// Resource file name definition
_LIT( KResourceFileName,"esmrtasks.rsc" );

// Definition for new line
_LIT( KNewLine,"\n" );

_LIT( KWhiteSpace, " " );

// Definition for first position
const TInt KFirstPos( 0 );

// Denition for amount of needed newlines
const TInt KNewLinesNeeded = 5;

// Definition for time string length (13:15)
const TInt KTimeStringLength = 20;

// Definition for date stinng length (31.12.2007)
const TInt KDateStringLength = 20;

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::CESMRMailPlainTextFormatter
// ---------------------------------------------------------------------------
//
inline CESMRMailPlainTextFormatter::CESMRMailPlainTextFormatter(
        CMRMailboxUtils& aMailboxUtils ) :
    iMailboxUtils(aMailboxUtils)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::~CESMRMailPlainTextFormatter
// ---------------------------------------------------------------------------
//
CESMRMailPlainTextFormatter::~CESMRMailPlainTextFormatter()
    {
    FUNC_LOG;
    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        }
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::NewL
// ---------------------------------------------------------------------------
//
CESMRMailPlainTextFormatter* CESMRMailPlainTextFormatter::NewL(
        CMRMailboxUtils& aMailboxUtils )
    {
    FUNC_LOG;
    CESMRMailPlainTextFormatter* self = NewLC( aMailboxUtils );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::NewLC
// ---------------------------------------------------------------------------
//
CESMRMailPlainTextFormatter* CESMRMailPlainTextFormatter::NewLC(
        CMRMailboxUtils& aMailboxUtils )
    {
    FUNC_LOG;
    CESMRMailPlainTextFormatter* self =
        new (ELeave) CESMRMailPlainTextFormatter(aMailboxUtils);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRMailPlainTextFormatter::ConstructL()
    {
    FUNC_LOG;
    CCoeEnv* coeEnv = CCoeEnv::Static();// codescanner::eikonenvstatic

    TFileName resourceFile;
    ESMRHelper::LocateResourceFile(
            KResourceFileName,
            KDC_RESOURCE_FILES_DIR,
            resourceFile,
            &coeEnv->FsSession());

    // Find the resource file for the nearest language
    BaflUtils::NearestLanguageFile(coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::Body16LC
// ---------------------------------------------------------------------------
//
HBufC* CESMRMailPlainTextFormatter::Body16LC(
        CCalEntry& aEntry )
    {
    FUNC_LOG;
    HBufC* dateFormatString = StringLoader::LoadLC( R_QTN_DATE_USUAL );

    HBufC* timeFormatString = StringLoader::LoadLC( R_QTN_TIME_USUAL );

    TTime mrStartTime = aEntry.StartTimeL().TimeLocalL();
    TTime mrEndTime   = aEntry.EndTimeL().TimeLocalL();

    TBuf<KTimeStringLength> startTimeStr, endTimeStr;
    TBuf<KDateStringLength> startDateStr, endDateStr;

    mrStartTime.FormatL( startTimeStr, *timeFormatString );
    mrStartTime.FormatL( startDateStr, *dateFormatString );
    mrEndTime.FormatL( endTimeStr, *timeFormatString );
    mrEndTime.FormatL( endDateStr, *dateFormatString );

    CleanupStack::PopAndDestroy( 2, dateFormatString );

    HBufC* subject =
        StringLoader::LoadLC(R_QTN_MEET_REQ_PLAIN_TEXT_SUBJECT);

    HBufC* location =
        StringLoader::LoadLC(R_QTN_MEET_REQ_PLAIN_TEXT_LOCATION);

    HBufC* startTime =
        StringLoader::LoadLC(R_QTN_MEET_REQ_PLAIN_TEXT_START_TIME);

    HBufC* endTime =
        StringLoader::LoadLC(R_QTN_MEET_REQ_PLAIN_TEXT_END_TIME);

    HBufC* description =
        StringLoader::LoadLC(R_QTN_MEET_REQ_PLAIN_TEXT_DESCRIPTION);

    HBufC* details =
        HBufC::NewLC( subject->Length() +
                      location->Length() +
                      startTime->Length() +
                      endTime->Length() +
                      description->Length() +
                      aEntry.SummaryL().Length() +
                      aEntry.LocationL().Length() +
                      aEntry.DescriptionL().Length() +
                      KNewLine().Length() * KNewLinesNeeded +
                      KTimeStringLength * 2 + // start and end time
                      KDateStringLength * 2 + // start and end date
                      KWhiteSpace().Length() * 2 ); // start and end date times

    TPtr detailsPtr( details->Des() );

    detailsPtr.Append( *subject );
    detailsPtr.Append( aEntry.SummaryL() );
    detailsPtr.Append( KNewLine );

    detailsPtr.Append( *location );
    detailsPtr.Append( aEntry.LocationL() );
    detailsPtr.Append( KNewLine );

    detailsPtr.Append( *startTime );
    detailsPtr.Append( startDateStr );
    detailsPtr.Append( KWhiteSpace );
    detailsPtr.Append( startTimeStr );
    detailsPtr.Append( KNewLine );

    detailsPtr.Append( *endTime );
    detailsPtr.Append( endDateStr );
    detailsPtr.Append( KWhiteSpace );
    detailsPtr.Append( endTimeStr );
    detailsPtr.Append( KNewLine );

    detailsPtr.Append( *description );
    detailsPtr.Append( aEntry.DescriptionL() );
    detailsPtr.Append( KNewLine );

    CleanupStack::Pop( details );

    // subject, location, startTime, endTime, description
    CleanupStack::PopAndDestroy( 5, subject );

    CleanupStack::PushL( details );
    return details;
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::Body8LC
// ---------------------------------------------------------------------------
//
HBufC8* CESMRMailPlainTextFormatter::Body8LC(
        CCalEntry& aEntry )
    {
    FUNC_LOG;
    HBufC* buf16 = Body16LC( aEntry );
    HBufC8* buf8 = HBufC8::NewLC( buf16->Length() );
    TPtr8 ptr8( buf8->Des() );

    TInt err = CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr8, *buf16 );
    User::LeaveIfError( err );
    CleanupStack::Pop( buf8 );
    CleanupStack::PopAndDestroy( buf16 );
    CleanupStack::PushL( buf8 );
    return buf8;
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::Subject16LC
// ---------------------------------------------------------------------------
//
HBufC* CESMRMailPlainTextFormatter::Subject16LC(
            CCalEntry& aEntry,
            TBool aIsForwarded,
            TBool aIsUpdate )
    {
    FUNC_LOG;
    CESMRCalUserUtil* caluserUtil = CESMRCalUserUtil::NewLC( aEntry );
    TESMRRole role = caluserUtil->PhoneOwnerRoleL();
    CleanupStack::PopAndDestroy( caluserUtil );

    // create prefix for subject:
    HBufC* prefix = SubjectLinePrefix16LC(
                            aEntry,
                            aIsForwarded,
                            aIsUpdate ); //if returns NULL no need to CleanupStack::Pop later

    // user's email address is attached to subject if user is
    // responding to meeting request:
    HBufC* responseAddress = KNullDesC().AllocLC();
    if ( !aIsForwarded )
        {
        if ( EESMRRoleRequiredAttendee == role ||
             EESMRRoleOptionalAttendee == role ||
             EESMRRoleNonParticipant == role  )
             {
             CleanupStack::PopAndDestroy( responseAddress );
             responseAddress = NULL;

             CCalUser* phoneOwner = aEntry.PhoneOwnerL();
             responseAddress = phoneOwner->Address().AllocLC();
             }
        }

    // count the subject line buffer length
    TInt subjectLineLength( aEntry.SummaryL().Length() );

    if (prefix)
        {
        subjectLineLength += prefix->Length();
        }

    if (responseAddress)
        {
        subjectLineLength += responseAddress->Length();
        }

    // create and fill the subject buffer
    HBufC* subjectLine =
        HBufC::NewLC( subjectLineLength );
    TPtr ptrSubject( subjectLine->Des() );

     if (responseAddress)
        {
        ptrSubject.Append(*responseAddress);
        }

     if (prefix)
        {
        ptrSubject.Copy( *prefix );
        }

    ptrSubject.Append( aEntry.SummaryL() );

    CleanupStack::Pop( subjectLine );

    if (prefix)
        {
        CleanupStack::PopAndDestroy( 2, prefix ); // responseAddress
        }
    else
        {
        CleanupStack::PopAndDestroy( responseAddress );
        }
    CleanupStack::PushL( subjectLine );
    return subjectLine;
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::Subject8LC
// ---------------------------------------------------------------------------
//
HBufC8* CESMRMailPlainTextFormatter::Subject8LC(
            CCalEntry& aEntry,
            TBool aIsForwarded,
            TBool aIsUpdate  )
    {
    FUNC_LOG;
    HBufC* buf16 = Subject16LC( aEntry, aIsForwarded, aIsUpdate );
    HBufC8* buf8 = HBufC8::NewLC( buf16->Length() );
    TPtr8 ptr8(buf8->Des() );

    TInt err = CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr8, *buf16 );
    User::LeaveIfError( err );
    CleanupStack::Pop( buf8 );
    CleanupStack::PopAndDestroy( buf16 );
    CleanupStack::PushL( buf8 );
    return buf8;
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::SubjectLinePrefix16LC
// ---------------------------------------------------------------------------
//
HBufC* CESMRMailPlainTextFormatter::SubjectLinePrefix16LC(
        CCalEntry& aEntry,
        TBool aIsForwarded,
        TBool aIsUpdate  )
    {
    FUNC_LOG;
    HBufC* prefix = NULL;

    CESMRCalUserUtil* caluserUtil = CESMRCalUserUtil::NewLC( aEntry );
    TESMRRole role = caluserUtil->PhoneOwnerRoleL();
    CleanupStack::PopAndDestroy( caluserUtil );
    caluserUtil = NULL;

    if ( EESMRRoleOrganizer == role )
        {
        CCalEntry::TMethod method = aEntry.MethodL();
        if ( CCalEntry::EMethodCancel == method )
            {
            prefix =
                StringLoader::LoadLC( R_QTN_MEET_REQ_PLAIN_TEXT_CANCELLED );
            }
        else if ( CCalEntry::EMethodRequest == method  &&
                  (aIsUpdate || aIsForwarded) )
            {
            prefix =
                StringLoader::LoadLC( R_QTN_MEET_REQ_PLAIN_TEXT_UPDATE );
            }
        else
            {
            prefix =
                KNullDesC().AllocLC();
            }
        }

    else if ( EESMRRoleRequiredAttendee == role ||
              EESMRRoleOptionalAttendee == role ||
              EESMRRoleNonParticipant == role )
        {
        CCalAttendee* attendee =
            iMailboxUtils.ThisAttendeeL( aEntry );

        if ( attendee )
            {
            switch( attendee->StatusL() )
                {
                case CCalAttendee::EAccepted:
                    prefix =
                            StringLoader::LoadLC(
                                R_QTN_MEET_REQ_PLAIN_TEXT_ACCEPTED );
                    break;
                case CCalAttendee::ETentative:
                    prefix =
                            StringLoader::LoadLC(
                                R_QTN_MEET_REQ_PLAIN_TEXT_TENTATIVE );
                    break;
                case CCalAttendee::EDeclined:
                    prefix =
                            StringLoader::LoadLC(
                                R_QTN_MEET_REQ_PLAIN_TEXT_DECLINED );
                    break;
                default:
                    prefix =
                        KNullDesC().AllocLC();
                    break;
                }
            }
        else
            {
            prefix =
                KNullDesC().AllocLC();
            }
        }
    return prefix;
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::UpdatedStringLC
// ---------------------------------------------------------------------------
//
HBufC* CESMRMailPlainTextFormatter::UpdatedStringLC()
    {
    FUNC_LOG;
    HBufC* updatedString =
            StringLoader::LoadLC( R_QTN_MEET_REQ_PLAIN_TEXT_UPDATE );

    return updatedString;
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::CanceledStringLC
// ---------------------------------------------------------------------------
//
HBufC* CESMRMailPlainTextFormatter::CanceledStringLC()
    {
    FUNC_LOG;
    HBufC* canceledString =
            StringLoader::LoadLC( R_QTN_MEET_REQ_PLAIN_TEXT_CANCELLED );

    return canceledString;
    }

// ---------------------------------------------------------------------------
// CESMRMailPlainTextFormatter::ReplyStringLC
// ---------------------------------------------------------------------------
//
HBufC* CESMRMailPlainTextFormatter::ReplyStringLC( CCalEntry& aEntry )
    {
    FUNC_LOG;
    HBufC* replyString = NULL;
    HBufC* replyStringPrefix =
            StringLoader::LoadLC( R_QTN_MEET_REQ_VIEWER_SUBJECT_PREFIX_REPL );
    
    TPtrC subject( aEntry.SummaryL() );
    
    TInt pos = subject.Find( *replyStringPrefix );
    if ( KErrNotFound == pos || KFirstPos != pos )
        {
        // There is no reply prefix already or reply prefix is not at the 
        // beginning --> We need to add it to description

        // No need to put into cleanupstack because calling only non leaving 
        // functions before exiting
        replyString = HBufC::NewL( 
                            replyStringPrefix->Length() + subject.Length() );
        
        TPtr ptrReply( replyString->Des() );
        
        ptrReply.Copy( *replyStringPrefix );
        ptrReply.Append( subject );
        }
    else
        {
        replyString = subject.AllocL();
        }
    
    CleanupStack::PopAndDestroy( replyStringPrefix );
    CleanupStack::PushL( replyString );
    
    return replyString;
    }

// EOF
