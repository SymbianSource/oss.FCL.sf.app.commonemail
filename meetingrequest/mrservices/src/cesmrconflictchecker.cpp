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
*  Version     : %version: e002sa33#7 %
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

    // Get instance views of all calendar
    RPointerArray<CCalInstanceView> allCalenInstanceView = 
                                    iDbMgr.NormalDbAllCalenInstanceView();
    
    RCPointerArray<CCalInstance> instanceArray;
    CleanupClosePushL( instanceArray );
    
    // Check if there is any conflict in each calendar
    for( TInt i = 0; i < allCalenInstanceView.Count(); i++ )
        {
        CalCommon::TCalTimeRange timeRange =
                    ResolveFetchTimeRangeL( 
                                    aEntry.StartTimeL().TimeUtcL(),
                                    aEntry.EndTimeL().TimeUtcL(), 
                                    allCalenInstanceView[i] );
        
        allCalenInstanceView[i]->FindInstanceL( instanceArray,
                                                CalCommon::EIncludeAppts,
                                                timeRange );


        }
    
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

