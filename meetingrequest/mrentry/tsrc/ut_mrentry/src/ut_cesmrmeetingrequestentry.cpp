/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MR Entry unit and module tests
*  Version     : %version: e002sa33#3 % 
*
*  Copyright ? 2008-2008 Nokia.  All rights reserved.
*/


#include "ut_cesmrmeetingrequestentry.h"


// SYSTEM INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <aknutils.h>

#include <caleninterimutils2.h> 
#include <calentry.h>
#include <calsession.h>
#include <caluser.h>
#include <calrrule.h>
#include <calalarm.h>
#include <calrrule.h>
#include <cmrmailboxutils.h>
#include <calinstanceview.h>
#include <calinstance.h>
#include <mesmrcalentry.h>
#include <gulicon.h>

// USER INCLUDES
#include "cesmrmeetingrequestentry.h"
#include "cesmrcaldbmgr.h"
#include "esmrentryhelper.h"



// CONSTANTS

// LOCAL DEFINITIONS

namespace {

// Default Calendar database path "Calendar_test");//
_LIT( KCalendarDatabaseFilePath, "c:Calendar_test" );


_LIT( KUser1, "foo1@bar.com" );
_LIT( KUser2, "foo2@bar.com" );
_LIT( KUser3, "foo3@bar.com" );
_LIT( KUser4, "foo4@bar.com" );
_LIT( KUser5, "foo5@bar.com" );
_LIT( KUser6, "foo6@bar.com" );
_LIT( KOrg,   "foo7@bar.com");
_LIT( KAtOrg,   "cayenne2@digia.com");


CCalEntry* CreateEntryL( CCalEntry::TType aType, 
        TCalTime aStartCalTime, 
        TCalTime aEndCalTime )
    {    
    CCalenInterimUtils2* utils = CCalenInterimUtils2::NewL();
    CleanupStack::PushL( utils );
    // Create unique ID.
    HBufC8* guid = utils->GlobalUidL();
    CleanupStack::PushL(guid);
    CCalEntry* entry = CCalEntry::NewL( aType, guid, CCalEntry::EMethodNone, 0 );
    CleanupStack::Pop( guid );
    CleanupStack::PushL( entry );
    
    //TEST DATA
    TDateTime start = aStartCalTime.TimeLocalL().DateTime();
    TDateTime end = aEndCalTime.TimeLocalL().DateTime();
    
    entry->SetStartAndEndTimeL( aStartCalTime, aEndCalTime );
    CleanupStack::Pop( entry );
    CleanupStack::PopAndDestroy( utils );
   
    return entry;
    }

CCalAttendee* CreateAttendeeLC( const TDesC& aAddress, CCalAttendee::TCalRole aRole )
    {
    CCalAttendee* attendee = CCalAttendee::NewL(aAddress);
    CleanupStack::PushL(attendee);
    attendee->SetRoleL(aRole);

    return attendee;    
    }

CCalAttendee* SetAttendeeLC( CCalEntry& aEntry, const TDesC& aAddress, CCalAttendee::TCalRole aRole )
    {
    CCalAttendee* attendee = CreateAttendeeLC( aAddress, aRole );
    aEntry.AddAttendeeL( attendee );

    return attendee;
    }


} //namespace

// - Construction ------------------------------------------------------------

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntryImpl::NewL
// ---------------------------------------------------------------------------
//
UT_CESMRMeetingRequestEntry* UT_CESMRMeetingRequestEntry::NewL()
    {
    UT_CESMRMeetingRequestEntry* self = UT_CESMRMeetingRequestEntry::NewLC();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::NewLC
// ---------------------------------------------------------------------------
//
UT_CESMRMeetingRequestEntry* UT_CESMRMeetingRequestEntry::NewLC()
    {
    UT_CESMRMeetingRequestEntry* self = new( ELeave ) UT_CESMRMeetingRequestEntry();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::~UT_CESMRMeetingRequestEntry
// ---------------------------------------------------------------------------
//
UT_CESMRMeetingRequestEntry::~UT_CESMRMeetingRequestEntry()
    {
    delete iMRMailboxUtils; iMRMailboxUtils = NULL;
    delete iEntry; iEntry = NULL;
    delete iCtrlSyncher; iCtrlSyncher = NULL;
    delete iCalDbMgr; iCalDbMgr = NULL;
    delete iCalSession; iCalSession = NULL;
    }

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::UT_CESMRMeetingRequestEntry
// ---------------------------------------------------------------------------
//
UT_CESMRMeetingRequestEntry::UT_CESMRMeetingRequestEntry()
    {
    }

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::ConstructL
// ---------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
 
    }


// - Utility functions -----------------------------------------------------------

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::SetupL
// ---------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::SetupL()
    {
    // Creating mailbox utils
    iMRMailboxUtils = CMRMailboxUtils::NewL( NULL );
    
    // Creating calSession to calendar database
    iCalSession = CCalSession::NewL();
    
    TRAP_IGNORE( iCalSession->DeleteCalFileL( KCalendarDatabaseFilePath ));
    
    TRAPD( err, iCalSession->OpenL( KCalendarDatabaseFilePath ));
    if ( err == KErrNotFound )
        {
        iCalSession->CreateCalFileL( KCalendarDatabaseFilePath );
        iCalSession->OpenL( KCalendarDatabaseFilePath );
        }
    else
        {
        User::LeaveIfError( err );
        }
    
    iCalDbMgr = CESMRCalDbMgr::NewL( *iCalSession, *this );
    
    iConflictsExists = EFalse;
    
    ExecuteL();
    }

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::Teardown
// ---------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::Teardown()
    {
    delete iMrEntry; iMrEntry = NULL;
    delete iMRMailboxUtils; iMRMailboxUtils = NULL;
    delete iEntry; iEntry = NULL;
    delete iCtrlSyncher; iCtrlSyncher = NULL;
    delete iCalDbMgr; iCalDbMgr = NULL;
    delete iCalSession; iCalSession = NULL;
    }

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::CreateCalEntryL
// ---------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::CreateCalEntryL()
    {
    TTime time; 
    time.HomeTime();
    TCalTime initialCalTime; initialCalTime.SetTimeLocalL( time );
    TTimeIntervalHours hour( 1 );
    TCalTime initialCalEndTime; time += hour; initialCalEndTime.SetTimeLocalL( time );

    iEntry = CreateEntryL( CCalEntry::EAppt, initialCalTime, initialCalEndTime );
 
    // Location
    _LIT( KLocation, "Meeting Room data 123" );
    iEntry->SetLocationL( KLocation() );
    // Method for 'remove from cal' option test
    //            iEntry->SetMethodL( CCalEntry::EMethodCancel );

    // Description
    _LIT( KDesc, "Description field data" );
    iEntry->SetDescriptionL( KDesc() );
    
    // Subject
    _LIT( KSummary, "Summary field data" );
    iEntry->SetSummaryL( KSummary() );   
    
    // Setting alarm
    CCalAlarm* alarm = CCalAlarm::NewL();
    TTimeIntervalMinutes offset(60);
    alarm->SetTimeOffset( offset );
    iEntry->SetAlarmL( alarm );
    delete alarm; alarm = NULL;            
    }

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::CalEntryL
// ---------------------------------------------------------------------------
//
CCalEntry* UT_CESMRMeetingRequestEntry::CCalEntryL()
    {
    TTime time; 
    time.HomeTime();
    TCalTime initialCalTime; initialCalTime.SetTimeLocalL( time );
    TTimeIntervalHours hour( 1 );
    TCalTime initialCalEndTime; time += hour; initialCalEndTime.SetTimeLocalL( time );

    CCalEntry* entry = CreateEntryL( CCalEntry::EAppt, initialCalTime, initialCalEndTime );
 
    // Location
    _LIT( KLocation, "Meeting Room data 123" );
    entry->SetLocationL( KLocation() );
   
    // Description
    _LIT( KDesc, "Description field data" );
    entry->SetDescriptionL( KDesc() );
    
    // Subject
    _LIT( KSummary, "Summary field data" );
    entry->SetSummaryL( KSummary() );   
    
    // Setting alarm
    CCalAlarm* alarm = CCalAlarm::NewL();
    TTimeIntervalMinutes offset(60);
    alarm->SetTimeOffset( offset );
    entry->SetAlarmL( alarm );
    delete alarm; alarm = NULL;   
    
    return entry;
    }


// -----------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::HandleCalEngStatus
// -----------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::HandleCalEngStatus( TMRUtilsCalEngStatus /*aStatus*/ )
    {
    if ( iCtrlSyncher->CanStopNow() )
        {
        iCtrlSyncher->AsyncStop();
        }
    }

// -----------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::HandleOperation
// -----------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::HandleOperation(
        TInt /*aType*/,
        TInt /*aPercentageCompleted*/,
        TInt /*aStatus*/ )
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::ExecuteL
// -----------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::ExecuteL()
    {
    iExecutionError = KErrNone;

    iCtrlSyncher = new (ELeave) CActiveSchedulerWait;

    iCtrlSyncher->Start();
    }

// -----------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::CreateMREntryL
// -----------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::CreateMREntryL()
    {
    iMrEntry = CESMRMeetingRequestEntry::NewL( 
            *iEntry,
            *iMRMailboxUtils,
            *iCalDbMgr,
            iConflictsExists,
            NULL );
    }

// - Test methods -----------------------------------------------------------


// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::T_Global_CESMRMeetingRequestEntry_NewL
// ---------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::T_Global_CESMRMeetingRequestEntry_NewL()
    {
    CreateCalEntryL(); // Creating iEntry
    
    iMrEntry = CESMRMeetingRequestEntry::NewL( 
            *iEntry,
            *iMRMailboxUtils,
            *iCalDbMgr,
            iConflictsExists,
            NULL );
    
    EUNIT_ASSERT_DESC( iMrEntry, "CESMRMeetingRequestEntry::NewL failed" );
    }

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::T_ValidateEntryL_1
// ---------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::T_ValidateEntryL_1()
    {
    // entry1 = Normal once occuring entry
    CCalEntry* entry1 = CCalEntryL();

    // Saving entry to caldb
    MESMRCalDbMgr& mgr = *iCalDbMgr; 
    mgr.StoreEntryL( *entry1, ETrue );

    iEntry = entry1; //ownership transfered to iEntry
    
    CreateMREntryL();
    CCalEntry* validatedEntry1 = iMrEntry->ValidateEntryL();
    
    TCalLocalUid entry1uid = entry1->LocalUidL();
    TCalLocalUid validateuid = validatedEntry1->LocalUidL();

/*
    EUNIT_ASSERT_DESC( validateuid == entry1uid, 
            "Entry1 local uid should be the same as validated entrys!" );
*/
    
    EUNIT_ASSERT_DESC( validatedEntry1->UidL() == entry1->UidL(), 
            "Entry1 local uid should be the same as validated entrys!" );
    
    EUNIT_ASSERT_DESC( validatedEntry1->RecurrenceIdL().TimeLocalL() == 
                        entry1->RecurrenceIdL().TimeLocalL(), 
                        "Entry1 recurrence id should be the same as validated entrys!" );
    
    
    delete validatedEntry1; validatedEntry1 = NULL;
    }

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::T_ValidateEntryL_2
// ---------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::T_ValidateEntryL_2()
    {
    // entry2 = recurring entry
    CCalEntry* entry2 = CCalEntryL();
    
    // Adjusting entry2 start and end time 
    TCalTime newStartTime;
    TCalTime newEndTime;
    TTime newStart = entry2->StartTimeL().TimeLocalL() + TTimeIntervalDays(10);
    TTime newEnd = entry2->EndTimeL().TimeLocalL() + TTimeIntervalDays(10);
    newStartTime.SetTimeLocalL( newStart );
    newEndTime.SetTimeLocalL( newEnd );
    entry2->SetStartAndEndTimeL( newStartTime , newEndTime );
    
    // Adding recurrence to entry2
    TCalTime meetingStartTime = entry2->StartTimeL();
    TCalRRule rRule;
    rRule.SetType( TCalRRule::EDaily );    
    rRule.SetCount( 5 );
    rRule.SetDtStart( meetingStartTime );    
    entry2->SetRRuleL( rRule );
    
    // Saving entries to caldb
    MESMRCalDbMgr& mgr = *iCalDbMgr; 
    mgr.StoreEntryL( *entry2, ETrue );

    // If all went well, entry2 has been saved to db correctly, so let's use that
    iEntry = entry2; // ownership transfered

    // Creating iMrEntry based on this modifying entry
    CreateMREntryL();
    
    iMrEntry->SetModifyingRuleL( MESMRCalEntry::EESMRThisOnly  );
   
    CCalEntry* validatedEntry2 = iMrEntry->ValidateEntryL();

    // Verification
    if( validatedEntry2->EntryTypeL() != iEntry->EntryTypeL() )
        {
        EUNIT_FAIL_TEST( "Validated entrys type has changed!" );
        }
    else if( validatedEntry2->UidL().Compare( iEntry->UidL() ))
        {
        EUNIT_FAIL_TEST( "Validated entrys uid is no longer the original one!" );
        }
    else if ( validatedEntry2->LocalUidL() != 0 )
        {
        EUNIT_FAIL_TEST( "Validated entrys local uid is not zero!" );
        }
    else if ( validatedEntry2->SequenceNumberL() != 0 )
        {
        EUNIT_FAIL_TEST( "Validated entrys sequence number is not zero!" );
        }
    else if( validatedEntry2->SummaryL().Compare( iEntry->SummaryL() ))
        {
        EUNIT_FAIL_TEST( "Validated entrys summary is no longer the original one!" );
        }
    else if ( validatedEntry2->RecurrenceRangeL() != CalCommon::EThisOnly )
        {
        EUNIT_FAIL_TEST( "Validated entrys recurrence range is not EThisOnly!" );
        }
    else if ( validatedEntry2->StartTimeL().TimeLocalL() != iEntry->StartTimeL().TimeLocalL() )
        {
        EUNIT_FAIL_TEST( "Validated entrys start time has changed from original!" );
        }
    else
        {
        EUNIT_ASSERT_DESC( validatedEntry2, "Enrty validated correctly" )
        }

    delete validatedEntry2; validatedEntry2 = NULL;
    }


// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::T_EntryAttendeeInfoL
// ---------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::T_EntryAttendeeInfoL()
    {
    CreateCalEntryL(); // Creating iEntry 

    CCalAttendee* organizer = CreateAttendeeLC( KAtOrg(), CCalAttendee::EChair );
    iEntry->SetOrganizerL( organizer );
    iEntry->SetPhoneOwnerL( organizer );
    CleanupStack::Pop( organizer );
    
    SetAttendeeLC( *iEntry, KUser1(), CCalAttendee::EReqParticipant );
    CleanupStack::Pop();
    
    SetAttendeeLC( *iEntry, KUser2(), CCalAttendee::EReqParticipant );
    CleanupStack::Pop();
    
    SetAttendeeLC( *iEntry, KUser3(), CCalAttendee::EReqParticipant );
    CleanupStack::Pop();
    
    SetAttendeeLC( *iEntry, KUser4(), CCalAttendee::EOptParticipant );
    CleanupStack::Pop();
    
    SetAttendeeLC( *iEntry, KUser5(), CCalAttendee::EOptParticipant );
    CleanupStack::Pop();
    
    SetAttendeeLC( *iEntry, KUser6(), CCalAttendee::EOptParticipant );
    CleanupStack::Pop();            
    
    CCalUser* phoneOwner = iEntry->PhoneOwnerL();
    CCalUser* org = iEntry->OrganizerL();
 
    CreateMREntryL(); // Creating iMrEntry
    // Test data
    TESMRRole role = iMrEntry->RoleL();

    // When entry is organizer, attendee info should leave with KErrNotSupported
    //EUNIT_ASSERT_SPECIFIC_LEAVE( iMrEntry->EntryAttendeeInfoL(), KErrNotSupported );
    delete iMrEntry; iMrEntry = NULL;
    
    // PhoneOwner is set different than organizer, so that AttendeeInfo gives valid value
    CCalAttendee* attendee = SetAttendeeLC( *iEntry, KUser1(), CCalAttendee::EReqParticipant );
    iEntry->SetPhoneOwnerL( attendee );
    CleanupStack::Pop( attendee );
    
    // Setting entry to occure in the past so that return value is EESMREntryInfoOccursInPast
    TCalTime newStartTime;
    TCalTime newEndTime;
    TTime newStart = iEntry->StartTimeL().TimeLocalL() - TTimeIntervalDays(10);
    TTime newEnd = iEntry->EndTimeL().TimeLocalL() - TTimeIntervalDays(10);
    newStartTime.SetTimeLocalL( newStart );
    newEndTime.SetTimeLocalL( newEnd );
    iEntry->SetStartAndEndTimeL( newStartTime , newEndTime );
        
    CreateMREntryL(); // Creating iMrEntry
    // Test data
    role = iMrEntry->RoleL();
   
    MESMRMeetingRequestEntry::TESMREntryInfo attendeeInfo = iMrEntry->EntryAttendeeInfoL();

    EUNIT_ASSERT_DESC( attendeeInfo == MESMRMeetingRequestEntry::EESMREntryInfoOccursInPast, 
            "Entrys attendee info should be EESMREntryInfoOccursInPast" );
    
    }

// ---------------------------------------------------------------------------
// UT_CESMRMeetingRequestEntry::T_GetRecurrenceL
// ---------------------------------------------------------------------------
//
void UT_CESMRMeetingRequestEntry::T_GetRecurrenceL()
    {
    CreateCalEntryL(); // Creating iEntry
    
    // Setting recurrence
    TCalTime meetingStartTime = iEntry->StartTimeL();
    TCalRRule rRule;
    rRule.SetType( TCalRRule::EDaily );    
    rRule.SetCount( 5 );
    //rRule.SetInterval( 1 );    
    rRule.SetDtStart( meetingStartTime );    
    iEntry->SetRRuleL( rRule );
    // Entry is repeating 5 times every day from today onwards
    
    CreateMREntryL(); // Creating iMrEntry

    TESMRRecurrenceValue recurrence;
    TTime until;
    iMrEntry->GetRecurrenceL( recurrence, until );
        
    TTime recurrenceUntil = meetingStartTime.TimeUtcL() + TTimeIntervalDays(4); 

    // Test data
    TDateTime start = meetingStartTime.TimeLocalL().DateTime();
    TDateTime givenUntil = recurrenceUntil.DateTime();
    TDateTime fetchedUntil = until.DateTime();
    
    EUNIT_ASSERT_DESC( recurrence == ERecurrenceDaily, "Entry should be recurring daily" );
    EUNIT_ASSERT_DESC( until == recurrenceUntil, "Entrys until time is not 5 days from now" );
    }


// - EUnit test table --------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    UT_CESMRMeetingRequestEntry,
    "MRMeetingRequestEntry unit and module tests.",
    "UNIT" )
       
EUNIT_TEST(
    "Class creation (NewL)",
    "CESMRMeetingRequestEntry",
    "NewL",
    "FUNCTIONALITY",
    SetupL, T_Global_CESMRMeetingRequestEntry_NewL, Teardown )

EUNIT_TEST(
    "Validate Entry - 1",
    "CESMRMeetingRequestEntry",
    "ValidateEntryL",
    "FUNCTIONALITY",
    SetupL, T_ValidateEntryL_1, Teardown )
    
EUNIT_TEST(
    "Validate Entry - 2",
    "CESMRMeetingRequestEntry",
    "ValidateEntryL",
    "FUNCTIONALITY",
    SetupL, T_ValidateEntryL_2, Teardown )
    
EUNIT_TEST(
    "Entry Attendee Info",
    "CESMRMeetingRequestEntry",
    "EntryAttendeeInfoL",
    "FUNCTIONALITY",
    SetupL, T_EntryAttendeeInfoL, Teardown )

EUNIT_TEST(
    "Get recurrence",
    "CESMRMeetingRequestEntry",
    "GetRecurrenceL",
    "FUNCTIONALITY",
    SetupL, T_GetRecurrenceL, Teardown )
    
EUNIT_END_TEST_TABLE
