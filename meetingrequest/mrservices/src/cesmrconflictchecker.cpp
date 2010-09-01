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
*  Version     : %version: e002sa33#9 %
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
#include <calinstanceiterator.h>
#include <ct/rcpointerarray.h>
#include <calrrule.h>

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
 * Removes and deletes the entries, which do not conflict with
 * entry. Entry is considered to be conflicting entry if starts or
 * ends between aEntry's start and end time.
 *
 * @param aEntries Reference to entries, which may conflict
 * @param aEntry Reference to entry, which confilcts are resolved.
 */
void RemoveAndDeleteNonConflictingInstancesL(
    RPointerArray<CCalInstance>& instanceArray,
    TCalTime aStartTime,
    TCalTime aEndTime,
    const TDesC8& aUid,
    TInstanceFilter aFilterType )
    {
    FUNC_LOG;
    TInt index(0);

    TTime startTimeLocal = aStartTime.TimeLocalL();
    TTime endTimeLocal = aEndTime.TimeLocalL();

    while( index < instanceArray.Count() )
        {
        TBool conflictingInstance( ETrue );
        CCalInstance* instance = instanceArray[index];

        TTime entryStartTimeLocal = instance->StartTimeL().TimeLocalL();
        TTime entryEndTimeLocal = instance->EndTimeL().TimeLocalL();

        TPtrC8 uid( instance->Entry().UidL() );
        if ( ERemoveSameUID == aFilterType &&
             0 == aUid.CompareF(uid) )
            {
            conflictingInstance = EFalse;
            }
        else if ( entryStartTimeLocal >= startTimeLocal &&
                  entryStartTimeLocal < endTimeLocal )
            {
            // Entry starts during another entry
            index++;
            }
        else if ( entryEndTimeLocal > startTimeLocal &&
                entryEndTimeLocal <= endTimeLocal )
            {
            // Entry ends during another entry
            index++;
            }
        else if ( entryStartTimeLocal < startTimeLocal &&
                  entryEndTimeLocal > endTimeLocal )
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

        aConflictingEntries.AppendL( entry );
        CleanupStack::Pop( entry );
        }
    }

/**
 * Checks if entry is repeating.
 * @return ETrue if entry is repeating
 */
TBool IsRepeatingMeetingL( const CCalEntry& aEntry,
                           MESMRCalDbMgr& aDb )
    {
    FUNC_LOG;

    TBool recurrent( EFalse );

    CCalInstance* instance = aDb.FindInstanceL( aEntry );

    if ( instance )
        {
        CleanupStack::PushL( instance );
        recurrent = ESMREntryHelper::IsRepeatingMeetingL( instance->Entry() );
        CleanupStack::PopAndDestroy( instance );
        }
    else
        {
        recurrent = ESMREntryHelper::IsRepeatingMeetingL( aEntry );
        }

    return recurrent;
    }

/**
 * Finds conflicts for entry
 */
void FindConflictsForEntryL(
        const CCalEntry& aEntry,
        RPointerArray< CCalInstance >& aInstances,
        MESMRCalDbMgr& aDb )
    {
    FUNC_LOG;

    // Get instance views of all calendar
    RPointerArray<CCalInstanceView> allCalenInstanceView =
            aDb.NormalDbAllCalenInstanceView();

    // Check if there is any conflict in each calendar
    for( TInt i = 0; i < allCalenInstanceView.Count(); i++ )
        {
        CalCommon::TCalTimeRange timeRange =
                    ResolveFetchTimeRangeL(
                                    aEntry.StartTimeL().TimeUtcL(),
                                    aEntry.EndTimeL().TimeUtcL(),
                                    allCalenInstanceView[i] );

        allCalenInstanceView[i]->FindInstanceL(
                aInstances,
                CalCommon::EIncludeAppts,
                timeRange );
        }

    RemoveAndDeleteNonConflictingInstancesL(
            aInstances,
            aEntry.StartTimeL(),
            aEntry.EndTimeL(),
            aEntry.UidL(),
            ERemoveSameUID );
    }

/**
 * Moves instances from an array to another.
 */
void MoveInstancesL(
        RPointerArray< CCalInstance >& aFrom,
        RPointerArray< CCalInstance >& aTo )
    {
    FUNC_LOG;

    aTo.ReserveL( aTo.Count() + aFrom.Count() );
    while ( aFrom.Count() )
        {
        aTo.AppendL( aFrom[ 0 ] );
        aFrom.Remove( 0 );
        }
    }

/**
 * Finds conflicts based on Daily recurrence.
 *
 */
void FindConflictsForDailyRRuleL(
        CCalEntry& aEntry,
        RPointerArray< CCalInstance >& aInstances,
        MESMRCalDbMgr& aDb,
        TBool aFindAllConflicts )
    {
    FUNC_LOG;

    TCalRRule rRule;
    aEntry.GetRRuleL( rRule );
    RCPointerArray< CCalInstance > tmpInstanceArray;
    CleanupClosePushL( tmpInstanceArray );

    // Entry start and end time
    TTime start( aEntry.StartTimeL().TimeUtcL() );
    TTime end( aEntry.EndTimeL().TimeUtcL() );

    if ( rRule.Count() )
        {
        for ( TInt i = 0; i < rRule.Count(); ++i )
            {
            // Set each occurence start and end time on entry
            TTimeIntervalDays interval( i * rRule.Interval() );
            TCalTime startTime;
            startTime.SetTimeUtcL( start + interval );
            TCalTime endTime;
            endTime.SetTimeUtcL( end + interval );
            aEntry.SetStartAndEndTimeL(
                    startTime,
                    endTime );

            // Find conflicts for this occurence
            FindConflictsForEntryL( aEntry, tmpInstanceArray, aDb );

            if ( tmpInstanceArray.Count() )
                {
                MoveInstancesL( tmpInstanceArray, aInstances );
                if ( !aFindAllConflicts )
                    {
                    break;
                    }
                }
            }
        }
    else if ( rRule.Until().TimeUtcL() != Time::NullTTime() )
        {
        TTime until( rRule.Until().TimeUtcL() );
        TTime start( aEntry.StartTimeL().TimeUtcL() );
        TTime end( aEntry.EndTimeL().TimeUtcL() );
        TTimeIntervalDays interval( rRule.Interval() );

        // Loop while start time is before until time
        while ( start <= until )
            {
            // Set start and end time for occurence
            TCalTime startTime;
            startTime.SetTimeUtcL( start );
            TCalTime endTime;
            endTime.SetTimeUtcL( end );
            aEntry.SetStartAndEndTimeL(
                    startTime,
                    endTime );

            // Find conflicts
            FindConflictsForEntryL( aEntry, tmpInstanceArray, aDb );

            if ( tmpInstanceArray.Count() )
                {
                MoveInstancesL( tmpInstanceArray, aInstances );
                if ( !aFindAllConflicts )
                    {
                    break;
                    }
                }

            // Move to next occurence
            start += interval;
            end += interval;
            }
        }

    CleanupStack::PopAndDestroy( &tmpInstanceArray );
    }

/**
 * Finds conflicts based on Weekly recurrence.
 */
void FindConflictsForWeeklyRRuleL(
        CCalEntry& aEntry,
        RPointerArray< CCalInstance >& aInstances,
        MESMRCalDbMgr& aDb,
        TBool aFindAllConflicts )
    {
    FUNC_LOG;

    TCalRRule rRule;
    aEntry.GetRRuleL( rRule );

    // Tmp array for conflic instances
    RCPointerArray< CCalInstance > tmpInstanceArray;
    CleanupClosePushL( tmpInstanceArray );

    // Array of recurrence days
    RArray< TDay > days;
    CleanupClosePushL( days );
    rRule.GetByDayL( days );

    // Recurrence start time
    TTime start( aEntry.StartTimeL().TimeUtcL() );
    // Recurrence end time
    TTime end( aEntry.EndTimeL().TimeUtcL() );
    const TTimeIntervalDays KWeek( 7 );
    TDay startDayOfWeek( start.DayNoInWeek() );

    // Instance duration
    TTimeIntervalMinutes duration;
    end.MinutesFrom( start, duration );

    if ( rRule.Count() )
        {
        for ( TInt i = 0; i < rRule.Count(); ++i )
            {
            // Calculate weekly start time
            TTimeIntervalDays interval( i* KWeek.Int() );
            TDateTime date( TTime( start + interval ).DateTime() );
            date.SetDay( date.Day() - startDayOfWeek );
            TTime weekStartTime( date );

            for ( TInt j = 0; j < days.Count(); ++j )
                {
                // Iterate through days of week
                TTime entryStartTime( weekStartTime + TTimeIntervalDays( days[ j ] ) );

                if ( start <= entryStartTime )
                    {
                    // Start time is in recurrence range
                    // Calcualte end time
                    TCalTime startCalTime;
                    startCalTime.SetTimeUtcL( entryStartTime );
                    TCalTime endCalTime;
                    endCalTime.SetTimeUtcL( entryStartTime + duration );
                    aEntry.SetStartAndEndTimeL( startCalTime, endCalTime );

                    // Find conflicts of for entry and move them to result array
                    FindConflictsForEntryL( aEntry, tmpInstanceArray, aDb );

                    if ( tmpInstanceArray.Count() )
                        {
                        MoveInstancesL( tmpInstanceArray, aInstances );
                        if ( !aFindAllConflicts )
                            {
                            break;
                            }
                        }
                    }
                }

            if ( !aFindAllConflicts && aInstances.Count() )
                {
                break;
                }
            }
        }
    else if ( rRule.Until().TimeUtcL() != Time::NullTTime() )
        {
        TDateTime date( start.DateTime() );
        date.SetDay( date.Day() - startDayOfWeek );
        TTime weekStartTime( date );
        TTime until( rRule.Until().TimeUtcL() );

        while ( weekStartTime < until )
            {
            for ( TInt j = 0; j < days.Count(); ++j )
                {
                // Iterate through days of week
                TTime entryStartTime( weekStartTime + TTimeIntervalDays( days[ j ] ) );

                if ( start <= entryStartTime )
                    {
                    // Start time is in recurrence range
                    // Calculate end time
                    TCalTime startCalTime;
                    startCalTime.SetTimeUtcL( entryStartTime );
                    TCalTime endCalTime;
                    endCalTime.SetTimeUtcL( entryStartTime + duration );
                    aEntry.SetStartAndEndTimeL( startCalTime, endCalTime );

                    // Find conflicts of for entry and move them to result array
                    FindConflictsForEntryL( aEntry, tmpInstanceArray, aDb );

                    if ( tmpInstanceArray.Count() )
                        {
                        MoveInstancesL( tmpInstanceArray, aInstances );
                        if ( !aFindAllConflicts )
                            {
                            break;
                            }
                        }
                    }
                }

            if ( !aFindAllConflicts && aInstances.Count() )
                {
                break;
                }
            else
                {
                weekStartTime += KWeek;
                }
            }
        }

    CleanupStack::PopAndDestroy( &days );
    CleanupStack::PopAndDestroy( &tmpInstanceArray );
    }

/**
 * Finds conflict for recurrent entry
 */
void FindConflictsForRepeatingMeetingL(
        const CCalEntry& aEntry,
        RPointerArray< CCalInstance >& aInstances,
        MESMRCalDbMgr& aDb,
        TBool aFindAllConflicts )
    {
    FUNC_LOG;

    CCalInstance* instance = aDb.FindInstanceL( aEntry );

    if ( instance ) // Instance is stored
        {
        CleanupStack::PushL( instance );
        RCPointerArray< CCalInstance > tmpInstanceArray;
        CleanupClosePushL( tmpInstanceArray );

        CCalEntry& parent = instance->Entry();
        CCalInstanceView* instanceView = aDb.InstanceViewL( parent );
        CCalInstanceIterator* iterator = instanceView->FindInstanceByUidL(
                        parent.UidL(),
                        parent.StartTimeL() );
        CleanupStack::PushL( iterator );
        CCalEntry* entry = ESMRHelper::CopyEntryLC(
                            parent,
                            parent.MethodL(),
                            ESMRHelper::ECopyFull );

        while ( iterator->HasMore() )
            {
            CCalInstance* next = iterator->NextL();
            CleanupStack::PushL( next );
            entry->SetStartAndEndTimeL( next->StartTimeL(), next->EndTimeL() );
            CleanupStack::PopAndDestroy( next );
            FindConflictsForEntryL( *entry,
                                    tmpInstanceArray,
                                    aDb );

            if ( tmpInstanceArray.Count() )
                {
                MoveInstancesL( tmpInstanceArray, aInstances );

                if ( !aFindAllConflicts )
                    {
                    break;
                    }
                }
            }

        CleanupStack::PopAndDestroy( entry );

        CleanupStack::PopAndDestroy( iterator );

        if ( aFindAllConflicts
             || !aInstances.Count() )
            {
            // Find conflicts also for parent entry
            FindConflictsForEntryL( parent, tmpInstanceArray, aDb );
            MoveInstancesL( tmpInstanceArray, aInstances );
            }

        CleanupStack::PopAndDestroy( &tmpInstanceArray );
        CleanupStack::PopAndDestroy( instance );
        }
    else // Entry is not stored yet
        {
        CCalEntry* entry = ESMRHelper::CopyEntryLC(
                aEntry,
                aEntry.MethodL(),
                ESMRHelper::ECopyFull );

        TCalRRule rRule;
        if ( aEntry.GetRRuleL( rRule ) )
            {
            switch ( rRule.Type() )
                {
                case TCalRRule::EDaily:
                    {
                    FindConflictsForDailyRRuleL(
                            *entry,
                            aInstances,
                            aDb,
                            aFindAllConflicts );
                    break;
                    }
                case TCalRRule::EWeekly:
                    {
                    FindConflictsForWeeklyRRuleL(
                            *entry,
                            aInstances,
                            aDb,
                            aFindAllConflicts );
                    break;
                    }
                default:
                    break;
                }
            }
        else
            {
            // Entry has RDates set
            RCPointerArray< CCalInstance > tmpInstanceArray;
            CleanupClosePushL( tmpInstanceArray );

            RArray< TCalTime > rDates;
            CleanupClosePushL( rDates );
            aEntry.GetRDatesL( rDates );

            // Get entry start time, end time and duration
            TTime start( aEntry.StartTimeL().TimeUtcL() );
            TTime end( aEntry.EndTimeL().TimeUtcL() );
            TTimeIntervalMinutes duration(0);
            end.MinutesFrom(
                    start,
                    duration );

            for ( TInt i = 0; i < rDates.Count(); ++i )
                {
                // Set start and end times for entry
                TCalTime startTime( rDates[ i ] );
                TCalTime endTime;
                endTime.SetTimeUtcL( startTime.TimeUtcL() + duration );
                entry->SetStartAndEndTimeL( startTime, endTime );

                // Find conflicts
                FindConflictsForEntryL( *entry, tmpInstanceArray, aDb );

                if ( tmpInstanceArray.Count() )
                    {
                    MoveInstancesL( tmpInstanceArray, aInstances );
                    if ( !aFindAllConflicts )
                        {
                        break;
                        }
                    }
                }

            CleanupStack::PopAndDestroy( &rDates );

            if ( aFindAllConflicts
                 || aInstances.Count() == 0 )
                {
                // Find conflicts for parent entry
                FindConflictsForEntryL( aEntry, tmpInstanceArray, aDb );
                MoveInstancesL( tmpInstanceArray, aInstances );
                }

            CleanupStack::PopAndDestroy( &tmpInstanceArray );
            }

        CleanupStack::PopAndDestroy( entry );
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
EXPORT_C CESMRConflictChecker::~CESMRConflictChecker()
    {
    FUNC_LOG;
    // Not implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRConflictChecker::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRConflictChecker* CESMRConflictChecker::NewL(
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
EXPORT_C void CESMRConflictChecker::FindConflictsL(
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

    RCPointerArray<CCalInstance> instanceArray;
    CleanupClosePushL( instanceArray );

    if ( IsRepeatingMeetingL( aEntry, iDbMgr ) )
        {
        FindConflictsForRepeatingMeetingL(
                aEntry,
                instanceArray,
                iDbMgr,
                EFalse );
        }
    else
        {
        FindConflictsForEntryL(
                aEntry,
                instanceArray,
                iDbMgr );
        }

    CreateEntriesFromInstancesL( instanceArray, aConflicts );
    CleanupStack::PopAndDestroy( &instanceArray);
    }

// ---------------------------------------------------------------------------
// CESMRConflictChecker::FindInstancesForEntry
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRConflictChecker::FindInstancesForEntryL(
        TTime aStart,
        TTime aEnd,
        const CCalEntry& aEntry,
        TCalCollectionId aColId,
        RPointerArray<CCalEntry>& aInstances )
    {
    FUNC_LOG;
    CCalInstanceView* instanceView = iDbMgr.InstanceViewL( aEntry );

    RCPointerArray<CCalInstance> instanceArray;
    CleanupClosePushL( instanceArray );

    // First we need the parent entry of the series ...
    CCalInstance* entryInstance = instanceView->FindInstanceL(
    		aEntry.LocalUidL(),
    		aEntry.StartTimeL() ); //Ownership gained
    CleanupStack::PushL( entryInstance );

    CCalInstance* parentEntryInstance = instanceView->FindInstanceL(
    		aEntry.LocalUidL(),
    		entryInstance->Entry().StartTimeL() );
    CleanupStack::PopAndDestroy( entryInstance );
    CleanupStack::PushL( parentEntryInstance );

    // ... And the parent entry instances start time
    TCalTime firstIntanceStartTime( parentEntryInstance->StartTimeL() );

    CleanupStack::Pop( parentEntryInstance ); // ownership given to instanceArray
    instanceArray.Append( parentEntryInstance );

    // Let's get all instances which have same uid and collection id
    // as the aEntry to an iterator.
    CCalInstanceIterator* instanceIterator = instanceView->FindInstanceByUidL(
    		aColId,
    		aEntry.UidL(),
    		firstIntanceStartTime );
    CleanupStack::PushL( instanceIterator );

    TInt count( instanceIterator->Count() );

    for( TInt i = 0; i < count; ++i )
    	{
		CCalInstance* instance = NULL;
		TRAPD( err, instance = instanceIterator->NextL() ); //Ownership gained
		if( !err && instance )
			{
			instanceArray.Append( instance );
			}
    	}

    CleanupStack::PopAndDestroy( instanceIterator );

    // Now the instanceArray has all instances of the aEntry,
    // let's remove this instance == instance of aEntry, from the array
	TInt i( 0 );
    while( i < instanceArray.Count() )
    	{
		CCalInstance* instance = instanceArray[i];

        TBool thisInstance(
                instance->StartTimeL().TimeLocalL() ==
                aEntry.StartTimeL().TimeLocalL() &&
                instance->EndTimeL().TimeLocalL() ==
                aEntry.EndTimeL().TimeLocalL() );

        if( thisInstance )
        	{
			delete instance;
			instanceArray.Remove( i );
        	}
        else
        	{
			++i;
        	}
    	}

    TCalTime start;
    start.SetTimeLocalL( aStart );

    TCalTime end;
    end.SetTimeLocalL( aEnd );

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

