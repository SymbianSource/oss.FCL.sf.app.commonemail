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
*  Description : ESMR conflict checker implementation
*  Version     : %version: tr1sido#7 %
*
*/

#include "emailtrace.h"
#include "cesmrconflictchecker.h"
#include "cesmrcaldbmgr.h"
#include "esmrhelper.h"
#include "esmrentryhelper.h"
#include "esmrinternaluid.h"

#include <calcommon.h>
#include <calinstance.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <ct/rcpointerarray.h>

/// Unnamed namespace for local definitions
namespace { // codescanner::namespace

// Definition for zero
const TInt KZero(0);

/** Defines instace filter type */
enum TInstanceFilter
    {
    EIncludeSameUID,
    ERemoveSameUID
    };

#ifdef _DEBUG

// Literal for panic texts
_LIT( KESMRConflictCheckerTxt, "ESMRConflictChecker" );

/** Enumeration for panic codes */
enum TESMRConflictCheckerPanic
    {
    // Input entry is not appointment
    EESMRConflictCheckerInvalidEntryType
    };

/**
 * Thows system wide panic.
 * @param aPanic Panic code.
 */
void Panic( TESMRConflictCheckerPanic aPanic )
    {

    User::Panic( KESMRConflictCheckerTxt, aPanic );
    }

#else

/**
 * Thows system wide leave.
 * @param aError Leave code.
 */
void Leave( TInt aError )
    {

    User::Leave( aError );
    }

#endif

/**
 * Resolves time range for possible conflicting entries. Instance view
 * is used for resolving the time range.
 *
 * @param aStart Start time
 * @param aEnd End time
 * @param aInstanceView Pointer to calendar db instance view
 * @return Time range for possible conflicting entries
 */
CalCommon::TCalTimeRange ResolveFetchTimeRangeL(
        TTime aStart,
        TTime aEnd,
        CCalInstanceView* aInstanceView )
    {
    FUNC_LOG;
    const CalCommon::TCalViewFilter instanceFilter = 
            CalCommon::EIncludeAppts;

    TDateTime start = aStart.DateTime();
    TDateTime end   = aEnd.DateTime();

    start.SetHour( KZero );
    start.SetMinute( KZero );
    start.SetSecond( KZero );
    start.SetMicroSecond( KZero );

    end.SetHour( KZero );
    end.SetMinute( KZero );
    end.SetSecond( KZero );
    end.SetMicroSecond( KZero );
    TTime endTime = end;
    
    if ( TTime(start) == endTime || 
         endTime < aEnd )
        {
        
        endTime += TTimeIntervalDays(1);
        end = endTime.DateTime();
        }

    TCalTime rangeStart;
    rangeStart.SetTimeLocalL( start );

    TCalTime instanceTime = aInstanceView->PreviousInstanceL(
            instanceFilter,
            rangeStart );

    if (  Time::NullTTime() != instanceTime.TimeUtcL()  )
        {
        rangeStart = instanceTime;
        }

    TCalTime rangeEnd;
    rangeEnd.SetTimeLocalL( end );

    instanceTime = aInstanceView->NextInstanceL(
            instanceFilter,
            rangeEnd );

    if ( instanceTime.TimeLocalL() != Time::NullTTime() )
        {
        rangeEnd = instanceTime;
        }

    return CalCommon::TCalTimeRange( rangeStart, rangeEnd );
    }

/**
 * Removes and deletes the entries, which do not confilct with
 * entry. Entry is considered to be conflicting entry if starts or
 * ends between aEntry's start and end time.
 *
 * @param aEntries Reference to entries, which may conflict
 * @param aEntry Reference to entry, which confilcts are resolved.
 */
void RemoveAndDeleteNonConflictingInstancesL(
    RPointerArray<CCalInstance>& instanceArray,    
    // const CCalEntry& aEntry,
    TCalTime aStartTime,
    TCalTime aEndTime,
    const TDesC8& aUid,
    TInstanceFilter aFilterType )
    {
    FUNC_LOG;
    TInt index(0);

    TTime startTimeUtc = aStartTime.TimeUtcL();
    TTime endTimeUtc = aEndTime.TimeUtcL();
    
    while( index < instanceArray.Count() )
        {
        TBool conflictingInstance( ETrue );
        CCalInstance* instance = instanceArray[index];
        
        TTime entryStartTimeUtc = instance->StartTimeL().TimeUtcL();
        TTime entryEndTimeUtc = instance->EndTimeL().TimeUtcL();            

        TPtrC8 uid( instance->Entry().UidL() );
        if ( ERemoveSameUID == aFilterType && 
             0 == aUid.CompareF(uid) )
            {
            conflictingInstance = EFalse;
            }
        else if ( entryStartTimeUtc >= startTimeUtc &&
                  entryStartTimeUtc < endTimeUtc )
            {
            // Entry starts during another entry
            index++;
            }
        else if ( entryEndTimeUtc > startTimeUtc &&
                entryEndTimeUtc <= endTimeUtc )
            {
            // Entry ends during another entry
            index++;
            }
        else if ( entryStartTimeUtc > startTimeUtc &&
                  entryEndTimeUtc < endTimeUtc )
            {
            // Antry starts and ends during another entry
            index++;
            }
        else if ( entryStartTimeUtc < startTimeUtc &&
                  entryEndTimeUtc > endTimeUtc )
            {
            // Antry starts and ends during another entry
            index++;
            }
        else
            {
            conflictingInstance = EFalse;            
            }
        
        // Remove non-conflicting instance from instance array
        if ( !conflictingInstance )
            {
            instanceArray.Remove(index);
            delete instance; 
            instance = NULL;
            }
        }
    }

/**
 * Creates calendar entries from the conflicting instances
 * and stores the created entries into entry array.
 * Ownership of the created entries is transferred to caller.
 * @param aInstanceArray Reference to instance array
 * @param aConflictingEntries Reference to entry array.
 */
void CreateEntriesFromInstancesL(
        RPointerArray<CCalInstance>& instanceArray,
        RPointerArray<CCalEntry>& aConflictingEntries )
    {
    FUNC_LOG;
    TInt instanceCount( instanceArray.Count() );    
    for ( TInt i(0); i < instanceCount; ++i )
        {        
        CCalEntry& parent( instanceArray[i]->Entry() );
        CCalEntry* entry = ESMRHelper::CopyEntryLC( parent, 
                                                    parent.MethodL(),
                                                    ESMRHelper::ECopyFull );
        
        entry->SetStartAndEndTimeL( instanceArray[i]->StartTimeL(),
                                    instanceArray[i]->EndTimeL() );

        User::LeaveIfError( aConflictingEntries.Append( entry ) );
        CleanupStack::Pop( entry );
        }
    }

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRConflictChecker::CESMRConflictChecker
// ---------------------------------------------------------------------------
//
inline CESMRConflictChecker::CESMRConflictChecker(
        MESMRCalDbMgr& aDbMgr ) :
        iDbMgr( aDbMgr )
    {
    FUNC_LOG;
    // Not implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRConflictChecker::~CESMRConflictChecker
// ---------------------------------------------------------------------------
//
CESMRConflictChecker::~CESMRConflictChecker()
    {
    FUNC_LOG;
    // Not implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRConflictChecker::NewL
// ---------------------------------------------------------------------------
//
CESMRConflictChecker* CESMRConflictChecker::NewL(
        MESMRCalDbMgr& aDbMgr )
    {
    FUNC_LOG;
    CESMRConflictChecker* self =
            new (ELeave) CESMRConflictChecker(aDbMgr);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRConflictChecker::FindConflictsL
// ---------------------------------------------------------------------------
//
void CESMRConflictChecker::FindConflictsL(
            const CCalEntry& aEntry,
            RPointerArray<CCalEntry>& aConflicts )
    {
    FUNC_LOG;

    // Checking input parameters
#ifdef _DEBUG

    __ASSERT_DEBUG(
            CCalEntry::EAppt == aEntry.EntryTypeL(),
            Panic(EESMRConflictCheckerInvalidEntryType) );

#else

    if ( CCalEntry::EAppt != aEntry.EntryTypeL() )
        {
        Leave( KErrArgument );
        }

#endif

    CCalInstanceView* instanceView = iDbMgr.NormalDbInstanceView();
    
    CalCommon::TCalTimeRange timeRange =
            ResolveFetchTimeRangeL( 
                    aEntry.StartTimeL().TimeUtcL(),
                    aEntry.EndTimeL().TimeUtcL(), 
                    instanceView );

    RCPointerArray<CCalInstance> instanceArray;
    CleanupClosePushL( instanceArray );

    instanceView->FindInstanceL( instanceArray,
                                 CalCommon::EIncludeAppts,
                                 timeRange );

    RemoveAndDeleteNonConflictingInstancesL(
            instanceArray,
            aEntry.StartTimeL(),
            aEntry.EndTimeL(),
            aEntry.UidL(),
            ERemoveSameUID );

    CreateEntriesFromInstancesL( instanceArray, aConflicts );
    
    CleanupStack::PopAndDestroy(); // instanceArray
    
    }

// ---------------------------------------------------------------------------
// CESMRConflictChecker::FindInstancesForEntry
// ---------------------------------------------------------------------------
//
void CESMRConflictChecker::FindInstancesForEntryL(
        TTime aStart, 
        TTime aEnd,
        const CCalEntry& aEntry,
        RPointerArray<CCalEntry>& aInstances )
    {
    FUNC_LOG;
    TDateTime startDt = aStart.DateTime();
    startDt.SetHour( KZero );
    startDt.SetMinute( KZero );
    startDt.SetSecond( KZero );
    
    TDateTime endDt = aEnd.DateTime();
    endDt.SetHour( KMaxHoursInDay );
    endDt.SetMinute( KMaxMinutesInHour );
    endDt.SetSecond( KMaxSecondsInMinute );
    
    aStart = startDt;
    aEnd = endDt;
    
    CCalInstanceView* instanceView = iDbMgr.NormalDbInstanceView();
    
    CalCommon::TCalTimeRange timeRange =
            ResolveFetchTimeRangeL( 
                    aStart,
                    aEnd, 
                    instanceView );

    RCPointerArray<CCalInstance> instanceArray;
    CleanupClosePushL( instanceArray );

    instanceView->FindInstanceL( instanceArray,
                                 CalCommon::EIncludeAppts,
                                 timeRange );
    
    TInt i(0);
    while( i < instanceArray.Count() )
        {
        CCalInstance* instance = instanceArray[i];
        CCalEntry& instanceEntry( instance->Entry() );
        
        TBool sameUid( 
                0 == instanceEntry.UidL().CompareF( aEntry.UidL() ) );
        
        TBool thisInstance( 
                instance->StartTimeL().TimeLocalL() == 
                aEntry.StartTimeL().TimeLocalL() &&
                instance->EndTimeL().TimeLocalL() == 
                aEntry.EndTimeL().TimeLocalL() );      
        
        if ( !sameUid || thisInstance )
            {
            // This is either same entry as aEntry or then
            // this entry does not belong to this series 
            delete instance;
            instanceArray.Remove( i );            
            }
        else
            {
            ++i;
            }        
        }
    
    TCalTime start;
    start.SetTimeUtcL( aStart );
    
    TCalTime end;
    end.SetTimeUtcL( aEnd );
    
    RemoveAndDeleteNonConflictingInstancesL(
            instanceArray,
            start,
            end,
            aEntry.UidL(),
            EIncludeSameUID );
    
    CreateEntriesFromInstancesL( instanceArray, aInstances );
    
    CleanupStack::PopAndDestroy(); // instanceArray    
    }

// EOF

