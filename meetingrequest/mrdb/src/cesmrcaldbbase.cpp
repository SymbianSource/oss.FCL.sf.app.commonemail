/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CESMRCalDbBase.
*
*/


//INCLUDE FILES
#include "emailtrace.h"
#include "cesmrcaldbbase.h"
#include "esmrhelper.h"
#include <ct/rcpointerarray.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calentry.h>
#include <calinstance.h>
#include <caluser.h>
#include <calrrule.h>
#include <caltime.h>

// CONSTANTS

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicNonEmptyParamArray = 1
    };

_LIT( KPanicMsg, "CESMRCalDbBase" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

#endif // _DEBUG

}  // namespace


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CESMRCalDbBase::CESMRCalDbBase
//
// Constructor.
// ----------------------------------------------------------------------------
//
CESMRCalDbBase::CESMRCalDbBase(
    MESMRCalDbObserver& aDbObserver,
    MCalProgressCallBack& aCmdObserver )
    : iDbStatus( MESMRCalDbObserver::EUninitialized ),
    iDbObserver( aDbObserver ),
      iCmdObserver( aCmdObserver )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::~CESMRCalDbBase
//
// Destructor.
// ----------------------------------------------------------------------------
//
CESMRCalDbBase::~CESMRCalDbBase()
    {
    FUNC_LOG;
    // subclasses take care of deleting entry & instance view if they
    // create them.
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::EntryView
// ----------------------------------------------------------------------------
//
const CCalEntryView* CESMRCalDbBase::EntryView() const
    {
    FUNC_LOG;
    return iCalEntryView;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::InstanceView
// ----------------------------------------------------------------------------
//
const CCalInstanceView* CESMRCalDbBase::InstanceView() const
    {
    FUNC_LOG;
    return iCalInstanceView;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::Session
// ----------------------------------------------------------------------------
//
const CCalSession* CESMRCalDbBase::Session() const
    {
    FUNC_LOG;
    return iCalSession;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::EntryView
// ----------------------------------------------------------------------------
//
CCalEntryView* CESMRCalDbBase::EntryView()
    {
    FUNC_LOG;
    return iCalEntryView;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::InstanceView
// ----------------------------------------------------------------------------
//
CCalInstanceView* CESMRCalDbBase::InstanceView()
    {
    FUNC_LOG;
    return iCalInstanceView;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::Session
// ----------------------------------------------------------------------------
//
CCalSession* CESMRCalDbBase::Session()
    {
    FUNC_LOG;
    return iCalSession;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::DbStatus
// ----------------------------------------------------------------------------
//
MESMRCalDbObserver::TDbStatus CESMRCalDbBase::DbStatus()
    {
    FUNC_LOG;
    return iDbStatus;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::ResetDbL
// ----------------------------------------------------------------------------
//
void CESMRCalDbBase::ResetDbL()
    {
    FUNC_LOG;
    // By default database reseting is not supported
    User::Leave( KErrNotSupported );
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::FetchWithRangeL
// According to UI spec. we only remove a repeating entry if all occurences
// are within given timerange. This implementation however considers
// originating and modifying entries separately (i.e. modifying entry may
// be removed without originating entry being removed ). It is done like this
// because 1) it's faster and 2) they are semantically different requests.
// ----------------------------------------------------------------------------
//
void CESMRCalDbBase::FetchWithRangeL(
    RPointerArray<CCalEntry>& aCalEntryArray,
    const CalCommon::TCalTimeRange& aCalTimeRange,
    TBool aFetchFullCopy )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( aCalEntryArray.Count() == 0,
                    Panic( EPanicNonEmptyParamArray ) );

    if ( !iCalInstanceView || !iCalEntryView )
        {
        User::Leave( KErrNotSupported );
        }

    RCPointerArray<CCalInstance> tmpFindArray;
    CleanupClosePushL( tmpFindArray );

    iCalInstanceView->FindInstanceL( tmpFindArray,
                                     CalCommon::EIncludeAppts,
                                     aCalTimeRange );

    TInt count( tmpFindArray.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        const CCalEntry& entry( tmpFindArray[i]->Entry() );

        // This is used to ensure that if entry has already been added we don't
        // bother to try again:
        TInt alreadyExistsIndex( aCalEntryArray.FindInOrder( &entry,
                TLinearOrder<CCalEntry>( CESMRCalDbBase::Compare ) ) );

        if ( alreadyExistsIndex < 0 &&
             IsCompletelyWithinRangeL( entry, aCalTimeRange ) )
            {
            // we create a full or a skeleton copy, based on given argument:
            ESMRHelper::TCopyFields copyType( aFetchFullCopy ?
                ESMRHelper::ECopyFull : ESMRHelper::ECopySkeleton );
            CCalEntry* copy = ESMRHelper::CopyEntryLC(
                                    entry,
                                    entry.MethodL(),
                                    copyType );

            aCalEntryArray.InsertInOrderL(
                    copy,
                    TLinearOrder<CCalEntry>( CESMRCalDbBase::Compare ) );

            CleanupStack::Pop( copy ); // ownership transferred to array
            }
        }

    CleanupStack::PopAndDestroy( &tmpFindArray );

    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::IsCompletelyWithinRangeL
// To optimize performance we return from the method immediatelly when EFalse
// has been confirmed.
// ----------------------------------------------------------------------------
//
TBool CESMRCalDbBase::IsCompletelyWithinRangeL(
    const CCalEntry& aEntry,
    const CalCommon::TCalTimeRange& aCalTimeRange )
    {
    FUNC_LOG;

    // 1. Check occurrences specified with a recurrence rule:

    TCalRRule rule;
    if ( aEntry.GetRRuleL( rule ) )
        {
        // In this implementation we don't check if end time of the last
        // occurrence exceeds the time range, we only check the start/until of
        // the rule (according to the standard the start time of the rule equals
        // the start time of the first occurrence).
        // Furthermore ex-dates are not considered at all.

        if ( rule.DtStart().TimeUtcL() < aCalTimeRange.StartTime().TimeUtcL() ||
             rule.Until().TimeUtcL() > aCalTimeRange.EndTime().TimeUtcL() )
            {
            return EFalse;
            }
        }

    // 2. Check occurrences specified with r-dates:

    RArray<TCalTime> rDateList;
    CleanupClosePushL( rDateList );
    aEntry.GetRDatesL( rDateList );
    TInt count( rDateList.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        // In this implementation we don't check if end time of r-date
        // occurence exceeds the time range.

        if ( rDateList[i].TimeUtcL() < aCalTimeRange.StartTime().TimeUtcL() ||
             rDateList[i].TimeUtcL() > aCalTimeRange.EndTime().TimeUtcL() )
            {
            CleanupStack::PopAndDestroy( &rDateList );

            return EFalse;
            }
        }
    CleanupStack::PopAndDestroy( &rDateList ); 
    
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::Compare
// ----------------------------------------------------------------------------
//
TInt CESMRCalDbBase::Compare(
    const CCalEntry& aFirst,
    const CCalEntry& aSecond )
    {
    FUNC_LOG;
    TInt retVal( 0 ); // default value is that items are equal
    TRAP_IGNORE( retVal = CompareL( aFirst, aSecond ) );
    return retVal;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::CompareL
// ----------------------------------------------------------------------------
//
TInt CESMRCalDbBase::CompareL( // codescanner::intleaves
    const CCalEntry& aFirst,
    const CCalEntry& aSecond )
    {
    FUNC_LOG;
    TInt result( aFirst.UidL().Compare( aSecond.UidL() ) );
    if ( result == 0 )
        { // UIDs were identical, how about recurrence id's then:
        TTime firstRec( aFirst.RecurrenceIdL().TimeUtcL() );
        TTime secondRec( aSecond.RecurrenceIdL().TimeUtcL() );
        if ( firstRec > secondRec )
            {
            result = 1;
            }
        else if ( firstRec < secondRec )
            {
            result = -1;
            }
        else
            {
            result = 0;
            }
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::Progress
// ----------------------------------------------------------------------------
//
void CESMRCalDbBase::Progress( TInt /*aPercentageCompleted*/ )
    {
    FUNC_LOG;
    // No interest currently
    }

// ----------------------------------------------------------------------------
// CESMRCalDbBase::NotifyProgress
// ----------------------------------------------------------------------------
//
TBool CESMRCalDbBase::NotifyProgress()
    {
    FUNC_LOG;
    // No interest currently
    return EFalse;
    }

// End of file

