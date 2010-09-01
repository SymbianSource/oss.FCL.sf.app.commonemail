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
* Description:  Conflict checker unit and module tests
*  Version     : %version: e002sa33#4 % 
*
*  Copyright ? 2008-2008 Nokia.  All rights reserved.
*/


#include "UT_CESMRConflictChecker.h"

// SYSTEM INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <aknutils.h>
#include <calsession.h>
#include <calinstance.h>
#include <calinstanceview.h>

// USER INCLUDES
#include "cesmrconflictchecker.h"
#include "cesmrcaldbmgr.h"

// CONSTANTS

// LOCAL DEFINITIONS

namespace {

_LIT(KTimeFormat,"%F%Y/%M/%D,%H:%T:%S");

// Conflicting entry's start time (yyyymmdd::hhmmss).
_LIT( KConflictingCalendarEntryStartTime, "20091100:140000" );
//-TODO:luo _LIT( KConflictingCalendarEntryStartTime, "20090000:150000" );
// Conflicting entry's end time (yyyymmdd::hhmmss).
_LIT( KConflictingCalendarEntryEndTime, "20091100:150000");
//-TODO:luo _LIT( KConflictingCalendarEntryEndTime, "20090000:170000");
// Calendar entry's location for conflicting entries.
_LIT( KCalendarEntryTrueValue, "1" );

// Recurrent event start and end time.
_LIT( KRecurrentCalendarEntryStartTime, "20091100:140000" );
_LIT( KRecurrentCalendarEntryEndTime, "20091100:150000" );

// Time range for fetching recurrent event instances.
_LIT( KRecurrentEntryTimeRangeStartTime, "20091100:000000" );
_LIT( KRecurrentEntryTimeRangeEndTime, "20091106:000000" );

// Calendar database filename.
_LIT( KCalendarDatabaseFilePath, "c:calendar3");//"c:Calendar_test_conflictchecker" );
// Calendar entry UID.
_LIT8( KCalendarEntryUid, "UT_ConflictChecker" );
}

// - Construction ------------------------------------------------------------

// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::NewL
// ---------------------------------------------------------------------------
//
UT_CESMRConflictChecker* UT_CESMRConflictChecker::NewL()
    {
    UT_CESMRConflictChecker* self = UT_CESMRConflictChecker::NewLC();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::NewLC
// ---------------------------------------------------------------------------
//
UT_CESMRConflictChecker* UT_CESMRConflictChecker::NewLC()
    {
    UT_CESMRConflictChecker* self = new( ELeave ) UT_CESMRConflictChecker();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::~UT_CESMRConflictChecker
// ---------------------------------------------------------------------------
//
UT_CESMRConflictChecker::~UT_CESMRConflictChecker()
    {
    delete iWait;
    }

// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::UT_CESMRConflictChecker
// ---------------------------------------------------------------------------
//
UT_CESMRConflictChecker::UT_CESMRConflictChecker()
    {
    }

// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::ConstructL
// ---------------------------------------------------------------------------
//
void UT_CESMRConflictChecker::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iWait = new ( ELeave) CActiveSchedulerWait();
    }

// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::HandleCalEngStatus
// ---------------------------------------------------------------------------
//
void UT_CESMRConflictChecker::HandleCalEngStatus( TMRUtilsCalEngStatus aStatus )
    {
    iWait->AsyncStop();
    }

// - Test methods -----------------------------------------------------------


// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::SetupL
// ---------------------------------------------------------------------------
//
void UT_CESMRConflictChecker::SetupL()
    {
    __UHEAP_MARK;
    iCalSession = CCalSession::NewL();
    iCalSession->OpenL( KCalendarDatabaseFilePath );
    iCalDbManager = CESMRCalDbMgr::NewL( *iCalSession, *this );
    iWait->Start();
    iConflictChecker = CESMRConflictChecker::NewL( *iCalDbManager );
    }
    
// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::Teardown
// ---------------------------------------------------------------------------
//
void UT_CESMRConflictChecker::Teardown()
    {
    delete iConflictChecker;
    delete iCalDbManager;
    delete iCalSession;
    __UHEAP_MARKEND;
    }

// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::T_Global_NewL
// ---------------------------------------------------------------------------
//
void UT_CESMRConflictChecker::T_Global_NewL()
    {
    delete iConflictChecker;
    iConflictChecker = NULL;
    
    __UHEAP_MARK;
    iConflictChecker = CESMRConflictChecker::NewL( *iCalDbManager );
    delete iConflictChecker;
    __UHEAP_MARKEND;
    
    iConflictChecker = NULL;
    }

// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::T_Global_FindConflictsL
// ---------------------------------------------------------------------------
//
void UT_CESMRConflictChecker::T_Global_FindConflictsL()
    {
    // Create reference calendar entry.
    HBufC8* entryUidBufC = KCalendarEntryUid().AllocLC();
    CCalEntry* calEntry = CCalEntry::NewL( CCalEntry::EAppt, entryUidBufC, CCalEntry::EMethodRequest, 0 );
    CleanupStack::Pop( entryUidBufC );
    entryUidBufC = NULL;
    
    TTime startTime, endTime;
    TCalTime startCalTime, endCalTime;
    
    startTime.Set( KConflictingCalendarEntryStartTime );
    endTime.Set( KConflictingCalendarEntryEndTime );

    TBuf<32> sTime;
    TBuf<32> eTime;
    startTime.FormatL( sTime, KTimeFormat );    
    endTime.FormatL(  eTime, KTimeFormat );
    
    startCalTime.SetTimeLocalL( startTime );
    endCalTime.SetTimeLocalL( endTime );
    
    calEntry->SetStartAndEndTimeL( startCalTime, endCalTime );
    
    // Fetch conflicting entries.
    RPointerArray<CCalEntry> conflictList;
    CleanupClosePushL( conflictList );
    
    iConflictChecker->FindConflictsL( *calEntry, conflictList );
    delete calEntry;
    calEntry = NULL;
    
    // Check that correct number of entries is returned.
    EUNIT_ASSERT_DESC( conflictList.Count() == 4,
            "Incorrect number of calendar entries." );
    
    // Verify list of conflicting entries.
    TBool testFailed = EFalse;
    while ( conflictList.Count() )
        {
        // If entry's location is something else than '1'.
        if ( conflictList[ 0 ]->LocationL().Compare( KCalendarEntryTrueValue ) )
            {
            testFailed = ETrue;
            }
        
        // Remove entry from array.
        delete conflictList[ 0 ];
        conflictList.Remove( 0 );
        }
    
    // Check that correct entries were returned.
    EUNIT_ASSERT_DESC( !testFailed,
            "Incorrect calendar entry/ies returned.");
    
    CleanupStack::PopAndDestroy(); // conflictList
    }

// ---------------------------------------------------------------------------
// UT_CESMRConflictChecker::T_Global_FindInstancesForEntryL
// ---------------------------------------------------------------------------
//
void UT_CESMRConflictChecker::T_Global_FindInstancesForEntryL()
    {
    // First retrieve one instace of recurrent event.
    RPointerArray<CCalInstance> instanceList;
    MESMRCalDbMgr& dbMgr = *iCalDbManager;
    CCalInstanceView* instanceView = dbMgr.NormalDbInstanceView();
    CalCommon::TCalViewFilter calViewFilter( CalCommon::EIncludeAppts );
    
    TTime startTime, endTime;
    TCalTime startCalTime, endCalTime;
    
    startTime.Set( KRecurrentCalendarEntryStartTime );
    endTime.Set( KRecurrentCalendarEntryEndTime );

    TBuf<32> sTime;
    TBuf<32> eTime;
    startTime.FormatL( sTime, KTimeFormat );    
    endTime.FormatL(  eTime, KTimeFormat );
    
    startCalTime.SetTimeLocalL( startTime );
    endCalTime.SetTimeLocalL( endTime );
    
    instanceView->FindInstanceL(
            instanceList,
            calViewFilter,
            CalCommon::TCalTimeRange( startCalTime, endCalTime ) );
    
    EUNIT_ASSERT_DESC( instanceList.Count() == 2,
            "Incorrect number of instances returned.");
    
    CCalEntry& calEntry = instanceList[0]->Entry();
    
    // Set time range.
    startTime.Set( KRecurrentEntryTimeRangeStartTime );
    endTime.Set( KRecurrentEntryTimeRangeEndTime );
    
    startCalTime.SetTimeLocalL( startTime );
    endCalTime.SetTimeLocalL( endTime );
    
    // Fetch recurrent event instances.
    RPointerArray<CCalEntry> entryList;
    iConflictChecker->FindInstancesForEntryL( startTime, endTime, calEntry, entryList );
    
    EUNIT_ASSERT_DESC( entryList.Count() == 6,
            "Incorrect number of calendar entry instances returned.");
    
    // Clear entry/instance lists.
    while ( instanceList.Count() )
        {
        delete instanceList[ 0 ];
        instanceList.Remove( 0 );
        }
    while ( entryList.Count() )
        {
        delete entryList[ 0 ];
        entryList.Remove( 0 );
        }
    instanceList.Close();
    entryList.Close();
    }

// - EUnit test table --------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    UT_CESMRConflictChecker,
    "CESMRConflictChecker unit and module tests.",
    "UNIT" )

EUNIT_TEST(
    "NewL",
    "CESMRConflictChecker",
    "NewL",
    "FUNCTIONALITY",
    SetupL, T_Global_NewL, Teardown )
    
EUNIT_TEST(
    "FindConflictsL",
    "CESMRConflictChecker",
    "FindConflictsL",
    "FUNCTIONALITY",
    SetupL, T_Global_FindConflictsL, Teardown )
    
EUNIT_TEST(
    "FindInstancesForEntryL",
    "CESMRConflictChecker",
    "FindInstancesForEntryL",
    "FUNCTIONALITY",
    SetupL, T_Global_FindInstancesForEntryL, Teardown )
    
EUNIT_END_TEST_TABLE
