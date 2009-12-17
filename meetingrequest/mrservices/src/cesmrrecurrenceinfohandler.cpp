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
* Description: This file implements CESMRRecurrenceInfoHandler.
*
*/


//<cmail>
#include "emailtrace.h"
#include "cesmrrecurrenceinfohandler.h"
#include "esmrdef.h"
//</cmail>

#include "mesmrmeetingrequestentry.h"

#include <calentry.h>
#include <calrrule.h>
#include <calcommon.h>

/// Unnamed namespace for local definitions
namespace {

// Definition for 0
const TInt KZero = 0;

// Definition for 1
const TInt KOne = 1;

// Definition for 2
const TInt KTwo = 2;

// Definition for days in week
const TInt KDaysInWeek = 7;

// Definition for days in two weeks
const TInt KDaysInTwoWeeks = 14;

// Definition for montsh in year
const TInt KMonthsInYear = 12;

/**
 * Compares two times and returns ETrue if they have same date components
 * @param aLhs Left hand side component
 * @param aRhs Right hand side component
 */
TBool IsSameDay(
        const TDateTime& aLhs,
        const TDateTime& aRhs )
    {
    TBool retValue(EFalse);

    if ( aLhs.Day() == aRhs.Day() &&
         aLhs.Month() == aRhs.Month() &&
         aLhs.Year() == aRhs.Year()   )
        {
        retValue = ETrue;
        }

    return retValue;
    }

/**
 * Checks if instance is included in exception list.
 * @param aInstanceTime Reference to instance time.
 * @param aExDateList Reference to exception list.
 */
TBool IsTimeIncluded(
        const TCalTime& aInstanceTime,
        const RArray<TCalTime>& aExDateList )
    {
    FUNC_LOG;
    TBool included( EFalse );
    TInt exceptionCount( aExDateList.Count() );
    if ( exceptionCount )
        {
        for (TInt i(0); (i < exceptionCount) && !included ; i++ )
            {
            const TCalTime& exceptionTime( aExDateList[i] );
            TDateTime exDate = exceptionTime.TimeLocalL().DateTime();

            if ( aInstanceTime.TimeLocalL() == exceptionTime.TimeLocalL() )
                {
                included = ETrue;
                }
            }
        }
    return included;
    }

/**
 * Calculates time for next instance for recurrent event.
 * @param aRecurrenceValue Defines the used recurrence.
 * @param aPrevInstanceStartTime Start time of the previous instance.
 */
TTime TimeForNextInstaceStartTime(
        TESMRRecurrenceValue aRecurrenceValue,
        TCalTime& aPrevInstanceStartTime )
    {
    TTime nextStartTime = aPrevInstanceStartTime.TimeLocalL();

    switch ( aRecurrenceValue )
        {
        case ERecurrenceDaily:
            {
            nextStartTime += TTimeIntervalDays( KOne );
            }
            break;
        case ERecurrenceWeekly:
            {
            nextStartTime += TTimeIntervalDays( KDaysInWeek );
            }
            break;
        case ERecurrenceEverySecondWeek:
            {
            nextStartTime += TTimeIntervalDays( KDaysInTwoWeeks );
            }
            break;
        case ERecurrenceMonthly:
            {
            nextStartTime += TTimeIntervalMonths( KOne );
            }
            break;
        case ERecurrenceYearly:
            {
            nextStartTime += TTimeIntervalYears( KOne );
            }
            break;
        default:
            {
            nextStartTime = aPrevInstanceStartTime.TimeLocalL();
            }
            break;
        }

    return nextStartTime;
    }


void CalculateUntilForUnknownRecurrenceL(
        TTime& aUntil,
        TCalRRule& aRule )
    {
    aUntil = aRule.Until().TimeLocalL();
    if ( Time::NullTTime() == aUntil )
        {
        aUntil = aRule.DtStart().TimeLocalL();
        TInt factor( aRule.Count() * aRule.Interval() );

        switch ( aRule.Type() )
            {
            case TCalRRule::EDaily:
                {
                aUntil += TTimeIntervalDays(factor );
                }
                break;
            case TCalRRule::EWeekly:
                {
                aUntil += TTimeIntervalDays(factor * KDaysInWeek );
                }
                break;
            case TCalRRule::EMonthly:
                {
                aUntil += TTimeIntervalMonths(factor );
                }
                break;
            case TCalRRule::EYearly:
                {
                aUntil += TTimeIntervalYears(factor );
                }
                break;
            }
        }
    }

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::CESMRRecurrenceInfoHandler
// ---------------------------------------------------------------------------
//
inline CESMRRecurrenceInfoHandler::CESMRRecurrenceInfoHandler(
        CCalEntry& aEntry ) :
        iEntry( aEntry )
    {
    FUNC_LOG;
    // Not implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::~CESMRRecurrenceInfoHandler
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRRecurrenceInfoHandler::~CESMRRecurrenceInfoHandler()
    {
    FUNC_LOG;
    // Not implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRRecurrenceInfoHandler* CESMRRecurrenceInfoHandler::NewL(
        CCalEntry& aEntry )
    {
    FUNC_LOG;

    CESMRRecurrenceInfoHandler* self = NewLC( aEntry );
    CleanupStack::Pop( self );


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRRecurrenceInfoHandler* CESMRRecurrenceInfoHandler::NewLC(
        CCalEntry& aEntry )
    {
    FUNC_LOG;

    CESMRRecurrenceInfoHandler* self =
            new (ELeave) CESMRRecurrenceInfoHandler(aEntry);
    CleanupStack::PushL( self );
    self->ConstructL();


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRRecurrenceInfoHandler::ConstructL()
    {
    FUNC_LOG;
    // Not implementation yet
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::SetRecurrenceL
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRRecurrenceInfoHandler::SetRecurrenceL(
        TESMRRecurrenceValue aRecurrence,
        TTime aUntil )
    {
    FUNC_LOG;

    // Recurrence until time needs to be in Utc mode, because comparison in
    // NeedToSetRecurrenceL is based on UTC times
    TCalTime until;
    until.SetTimeLocalL( aUntil );
    TTime recUntilUtc = until.TimeUtcL();

    // Check input parameters
    if ( ERecurrenceNot != aRecurrence &&
         Time::NullTTime() == aUntil )
        {
        // No until parameter set --> Leave
        User::Leave( KErrArgument );
        }
    else if ( ERecurrenceNot == aRecurrence )
        {
        // Clear all (recurrence, exceptions) repeating properties
        iEntry.ClearRepeatingPropertiesL();
        }
    else if ( NeedToSetRecurrenceL(aRecurrence, recUntilUtc) )
        {
        // Calculate 'correct' until time
        // Date component is taken from input parameter and
        // time component from meeting's end time.
        TDateTime until = aUntil.DateTime();
        TDateTime endTime = iEntry.EndTimeL().TimeLocalL().DateTime();
        until.SetHour( endTime.Hour() );
        until.SetMinute( endTime.Minute() );
        until.SetSecond( endTime.Second() );

        TCalTime rEndTime;
        rEndTime.SetTimeLocalL( TTime(until) );

        TCalTime meetingStartTime = iEntry.StartTimeL();

        TCalRRule rRule;
        rRule.SetDtStart( meetingStartTime );

        // Clear recurrence properties before setting the correct one
        iEntry.ClearRepeatingPropertiesL();

        switch ( aRecurrence )
            {
            case ERecurrenceDaily:
                {
                // Set daily recurrence
                rRule.SetType( TCalRRule::EDaily );

                TTimeIntervalDays daysBetween =
                    rEndTime.TimeLocalL().DaysFrom(
                            meetingStartTime.TimeLocalL() );

                // First occurence and days between
                rRule.SetCount( daysBetween.Int() + KOne);
                rRule.SetInterval( KOne );
                }
                break;

            case ERecurrenceWeekly:
                {
                // Set weekly recurrence.
                rRule.SetType( TCalRRule::EWeekly );
                TTimeIntervalDays weeksBetween =
                    rEndTime.TimeLocalL().DaysFrom(
                            meetingStartTime.TimeLocalL() );

                weeksBetween = weeksBetween.Int() / KDaysInWeek;

                RArray<TDay> dayArray;
                CleanupClosePushL( dayArray );
                dayArray.Append( rRule.DtStart().TimeLocalL().DayNoInWeek() );
                rRule.SetByDay( dayArray );
                CleanupStack::PopAndDestroy( &dayArray );

                // First occurence and weeks between
                rRule.SetCount(
                        weeksBetween.Int() + KOne);
                rRule.SetInterval( KOne );
                }
                break;

            case ERecurrenceEverySecondWeek:
                {
                // Set bi-weekly recurrence. This is weekly recurrence
                // with 2 weeks interval.
                rRule.SetType( TCalRRule::EWeekly );
                TTimeIntervalDays fortNightBetween =
                    rEndTime.TimeLocalL().DaysFrom(
                            meetingStartTime.TimeLocalL() );

                fortNightBetween =
                        fortNightBetween.Int() / KDaysInTwoWeeks;

                RArray<TDay> dayArray;
                CleanupClosePushL( dayArray );
                dayArray.Append( rRule.DtStart().TimeLocalL().DayNoInWeek() );
                rRule.SetByDay( dayArray );
                CleanupStack::PopAndDestroy( &dayArray );

                // First occurence and fortnights between
                rRule.SetCount(
                        fortNightBetween.Int() + KOne);
                rRule.SetInterval( KTwo );
                }
                break;

            case ERecurrenceMonthly:
                {
                // Set monthly recurrence.
                rRule.SetType( TCalRRule::EMonthly );

                RArray<TInt> dateArray;
                CleanupClosePushL( dateArray );
                dateArray.Append(
                    rRule.DtStart().TimeLocalL().DayNoInMonth() );
                rRule.SetByMonthDay( dateArray );
                CleanupStack::PopAndDestroy( &dateArray );

                TTimeIntervalMonths monthsBetween =
                    rEndTime.TimeLocalL().MonthsFrom(
                            meetingStartTime.TimeLocalL() );

                // First occurence and months between
                rRule.SetCount( monthsBetween.Int() + KOne);
                rRule.SetInterval( KOne );
                }
                break;

            case ERecurrenceYearly:
                {
                // Set yearly recurrence.
                rRule.SetType( TCalRRule::EYearly );

                TTimeIntervalYears yearsBetween =
                    rEndTime.TimeLocalL().YearsFrom(
                            meetingStartTime.TimeLocalL() );

                // First occurence and months between
                rRule.SetCount( yearsBetween.Int() + KOne);
                rRule.SetInterval( KOne );
                }
            default:
                break;
            }
        iEntry.SetRRuleL( rRule );
        }

    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::GetRecurrenceL
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRRecurrenceInfoHandler::GetRecurrenceL(
        TESMRRecurrenceValue& aRecurrence,
        TTime& aUntil) const
    {
    FUNC_LOG;

    TESMRRecurrenceValue recurrenceValue(ERecurrenceNot);

    // Let's get RDates of the entry also to see if entry is repeating
    RArray<TCalTime> rdates;
    CleanupClosePushL( rdates );
    iEntry.GetRDatesL( rdates );

    TBool hasRdates = rdates.Count() > 0;

    TCalRRule rRule;
    if ( iEntry.GetRRuleL(rRule) || hasRdates )
        {
        // Entry has recurrence
        recurrenceValue = ERecurrenceUnknown;

        TCalRRule::TType rType = rRule.Type();
        TInt interval          = rRule.Interval();

        switch ( rType )
            {
            case TCalRRule::EInvalid:
                {
                // Recurrence type has not yet been defined
                // --> Consider it then unknown.
                recurrenceValue = ERecurrenceUnknown;
                }
                break;

            case TCalRRule::EDaily:
                {
                HandleDailyRecurrenceL( recurrenceValue, rRule );
                }
                break;

            case TCalRRule::EWeekly:
                {
                HandleWeeklyRecurrenceL( recurrenceValue, rRule );
                }
                break;

            case TCalRRule::EMonthly:
                {
                HandleMonthlyRecurrenceL( recurrenceValue, rRule );
                }
                break;

            case TCalRRule::EYearly:
                {
                // Recurrence is based on a number of years
                if ( KOne == interval)
                    {
                    recurrenceValue = ERecurrenceYearly;
                    }
                }
                break;
            default:
                break;
            }

        if ( hasRdates )
            {
            // Getting the last RDates occurance time == Until time
            aUntil = rdates[ rdates.Count() - 1 ].TimeUtcL();
            }
        else
            {
            CalculateRecurrenceUntilDateL(
                    recurrenceValue,
                    aUntil,
                    rRule,
                    iEntry );
            }
        }

    CleanupStack::PopAndDestroy( &rdates );
    aRecurrence = recurrenceValue;

    }


EXPORT_C void CESMRRecurrenceInfoHandler::RemoveInstanceL(
        TCalTime aInstanceTime )
    {
    FUNC_LOG;

    RArray<TCalTime> exDateList;
    CleanupClosePushL( exDateList );

    TDateTime instanceException = aInstanceTime.TimeLocalL().DateTime();
    instanceException.SetHour( KZero );
    instanceException.SetMinute( KZero );
    instanceException.SetSecond( KZero );
    instanceException.SetMicroSecond( KZero );

    TCalTime exceptionTime;
    exceptionTime.SetTimeUtcL( TTime(instanceException) );

    iEntry.GetExceptionDatesL( exDateList );
    exDateList.Append( exceptionTime );
    iEntry.SetExceptionDatesL( exDateList );

    CleanupStack::PopAndDestroy( &exDateList );

    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::AddExceptionL
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRRecurrenceInfoHandler::AddExceptionL(
        TCalTime aNewInstanceTime,
         TCalTime aOrginalInstanceTime )
    {
    FUNC_LOG;

    // Entry's time has been modified and exception
    // needs to be added to parent entry
    RArray<TCalTime> rDateList;
    CleanupClosePushL( rDateList );

    RArray<TCalTime> exDateList;
    CleanupClosePushL( exDateList );

    // Only this instance is edited --> Set RRULE to entry
    iEntry.GetRDatesL( rDateList );
    iEntry.GetExceptionDatesL( exDateList );

    rDateList.Append( aNewInstanceTime );
    exDateList.Append( aOrginalInstanceTime );

    iEntry.SetRDatesL( rDateList );
    iEntry.SetExceptionDatesL( exDateList );

    CleanupStack::PopAndDestroy( &exDateList );
    CleanupStack::PopAndDestroy( &rDateList );

    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::CopyRecurrenceInformationToL
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRRecurrenceInfoHandler::CopyRecurrenceInformationToL(
        CCalEntry& aDestination )
    {
    FUNC_LOG;

    aDestination.ClearRepeatingPropertiesL();

    TCalRRule rrule;
    if ( iEntry.GetRRuleL(rrule) )
        {
        aDestination.SetRRuleL( rrule );

        RArray<TCalTime> rDateList;
        CleanupClosePushL( rDateList );

        RArray<TCalTime> exDateList;
        CleanupClosePushL( exDateList );

        iEntry.GetRDatesL( rDateList );
        iEntry.GetExceptionDatesL( exDateList );

        aDestination.SetRDatesL( rDateList );
        aDestination.SetExceptionDatesL( exDateList );

        CleanupStack::PopAndDestroy( &exDateList );
        CleanupStack::PopAndDestroy( &rDateList );
        }

    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::CopyRecurrenceInformationFromL
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRRecurrenceInfoHandler::CopyRecurrenceInformationFromL(
        const CCalEntry& aSource )
    {
    FUNC_LOG;

    iEntry.ClearRepeatingPropertiesL();

    TCalRRule rrule;
    if ( aSource.GetRRuleL(rrule) )
        {
        iEntry.SetRRuleL( rrule );

        RArray<TCalTime> rDateList;
        CleanupClosePushL( rDateList );

        RArray<TCalTime> exDateList;
        CleanupClosePushL( exDateList );

        aSource.GetRDatesL( rDateList );
        aSource.GetExceptionDatesL( exDateList );

        iEntry.SetRDatesL( rDateList );
        iEntry.SetExceptionDatesL( exDateList );

        CleanupStack::PopAndDestroy( &exDateList );
        CleanupStack::PopAndDestroy( &rDateList );
        }

    }


// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::GetFirstInstanceTimeL
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRRecurrenceInfoHandler::GetFirstInstanceTimeL(
        TCalTime& aStart,
        TCalTime& aEnd )
    {
    FUNC_LOG;

    TESMRRecurrenceValue recurrence( ERecurrenceNot );
    TTime until( Time::NullTTime() );
    GetRecurrenceL( recurrence, until );

    if ( ERecurrenceNot == recurrence )
        {
        aStart = iEntry.StartTimeL();
        aEnd = iEntry.EndTimeL();
        }
    else if ( ERecurrenceUnknown != recurrence )
        {
        aStart = iEntry.StartTimeL();
        aEnd = iEntry.EndTimeL();

        TTimeIntervalMinutes diff;
        aEnd.TimeLocalL().MinutesFrom(
                aStart.TimeLocalL(),
                diff );

        RArray<TCalTime> exDateList;
        CleanupClosePushL( exDateList );
        iEntry.GetExceptionDatesL( exDateList );

        TInt exceptionCount( exDateList.Count() );
        if ( exceptionCount )
            {
            TBool timeIncludedInExceptionList(
                    IsTimeIncluded( aStart, exDateList ) );

            while( timeIncludedInExceptionList )
                {
                TTime nextInstanceTime =
                        TimeForNextInstaceStartTime(
                                recurrence,
                                aStart );

                aStart.SetTimeLocalL( nextInstanceTime);
                timeIncludedInExceptionList =
                        IsTimeIncluded( aStart, exDateList );
                }
            }

        TCalTime untilCalTime;
        untilCalTime.SetTimeUtcL( until );
        
        TDateTime nextTime = aStart.TimeLocalL().DateTime();
        TDateTime untilTime = untilCalTime.TimeLocalL().DateTime();
        
        if ( aStart.TimeUtcL() > untilCalTime.TimeUtcL() )
            {
            User::Leave( KErrCorrupt );
            }

        TTime end = aStart.TimeLocalL() + diff;
        aEnd.SetTimeLocalL( end );

        CleanupStack::PopAndDestroy( &exDateList );
        }
    else
        {
        // Unknown recurrence type
        aStart = iEntry.StartTimeL();
        aEnd = iEntry.EndTimeL();
        }

    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::CalculateRecurrenceUntilDateL
//
// ---------------------------------------------------------------------------
//
void CESMRRecurrenceInfoHandler::CalculateRecurrenceUntilDateL(
            TESMRRecurrenceValue aRecurrenceType,
            TTime& aUntil,
            TCalRRule& aRule,
            const CCalEntry& aEntry  ) const
    {
    FUNC_LOG;
    TCalRRule::TType rType = aRule.Type();
    TInt count             = aRule.Count();
    TCalTime until         = aRule.Until();

    if ( count == KZero &&
         until.TimeUtcL() == Time::NullTTime() )
        {
        User::Leave( KErrNotFound );
        }

    if ( !count )
        {
        aUntil = until.TimeUtcL();
        }
    else
        {
        aUntil = aEntry.StartTimeL().TimeUtcL();
        switch( aRecurrenceType )
            {
            case ERecurrenceDaily:
                {
                --count;
                aUntil += TTimeIntervalDays(count);
                break;
                }
            case ERecurrenceWeekly:
                {
                --count;
                aUntil += TTimeIntervalDays(count * KDaysInWeek);
                break;
                }
            case ERecurrenceEverySecondWeek:
                {
                --count;
                aUntil += TTimeIntervalDays(count * KDaysInTwoWeeks);
                break;
                }
            case ERecurrenceMonthly:
                {
                --count;
                aUntil += TTimeIntervalMonths(count);
                break;
                }
            case ERecurrenceYearly:
                {
                --count;
                aUntil += TTimeIntervalYears(count);
                break;
                }
            default:
                {
                // Check if until date can be calculated
                CalculateUntilForUnknownRecurrenceL( aUntil, aRule );
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::HandleDailyRecurrenceL
// ---------------------------------------------------------------------------
//
void CESMRRecurrenceInfoHandler::HandleDailyRecurrenceL(
        TESMRRecurrenceValue& aRecurrenceValue,
        TCalRRule& aRule ) const
    {
    FUNC_LOG;
    TInt interval( aRule.Interval() );
    aRecurrenceValue = ERecurrenceUnknown;

    // Recurrence is based on a number of days
    if ( KOne == interval )
        {
        // Recurrence occurs daily
        aRecurrenceValue = ERecurrenceDaily;
        }
    else if ( KDaysInWeek ==  interval)
        {
        // Interval is seven days
        // --> recurrence occurs weekly
        aRecurrenceValue = ERecurrenceWeekly;
        }
    else if ( KDaysInTwoWeeks == interval )
        {
        // Interval is fortnight -->
        // recurrence occurs bi-weekly
        aRecurrenceValue = ERecurrenceEverySecondWeek;
        }
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::HandleWeeklyRecurrenceL
// ---------------------------------------------------------------------------
//
void CESMRRecurrenceInfoHandler::HandleWeeklyRecurrenceL(
        TESMRRecurrenceValue& aRecurrenceValue,
        TCalRRule& aRule ) const
    {
    FUNC_LOG;
    TInt interval( aRule.Interval() );

    RArray<TDay> byDays;
    CleanupClosePushL( byDays );
    aRule.GetByDayL(byDays);

    if ( byDays.Count() > KOne )
        {
        // Entry is repeated more than once every week or
        // every second week.
        aRecurrenceValue = ERecurrenceUnknown;
        }
    else if ( KOne == interval)
        {
        aRecurrenceValue = ERecurrenceWeekly;
        }
    else if ( KTwo == interval )
        {
        aRecurrenceValue = ERecurrenceEverySecondWeek;
        }

    CleanupStack::PopAndDestroy( &byDays );
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::HandleMonthlyRecurrenceL
// ---------------------------------------------------------------------------
//
void CESMRRecurrenceInfoHandler::HandleMonthlyRecurrenceL(
        TESMRRecurrenceValue& aRecurrenceValue,
        TCalRRule& aRule ) const
    {
    FUNC_LOG;
    TInt interval( aRule.Interval() );

    aRecurrenceValue = ERecurrenceUnknown;

    // Recurrence is based on a number of months
    // Recurrence is based on a number of weeks
    if ( KOne == interval)
        {
        aRecurrenceValue = ERecurrenceMonthly;
        }
    else if ( KMonthsInYear == interval )
        {
        aRecurrenceValue = ERecurrenceYearly;
        }
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::HandleYearlyRecurrenceL
// ---------------------------------------------------------------------------
//
void CESMRRecurrenceInfoHandler::HandleYearlyRecurrenceL(
        TESMRRecurrenceValue& aRecurrenceValue,
        TCalRRule& aRule ) const
    {
    FUNC_LOG;
    TInt interval( aRule.Interval() );

    // Recurrence is based on a number of years
    if ( KOne == interval)
        {
        aRecurrenceValue = ERecurrenceYearly;
        }
    else
        {
        aRecurrenceValue = ERecurrenceUnknown;
        }
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceInfoHandler::NeedToSetRecurrence
//
// ---------------------------------------------------------------------------
//
TBool CESMRRecurrenceInfoHandler::NeedToSetRecurrenceL(
        TESMRRecurrenceValue aRecurrence,
        TTime aUntil ) const
    {
    FUNC_LOG;
    TBool retValue(ETrue);

    TESMRRecurrenceValue oldRecurrence;
    TTime oldUntil;
    GetRecurrenceL( oldRecurrence, oldUntil );

    if( oldRecurrence == aRecurrence &&
        IsSameDay( aUntil.DateTime(), oldUntil.DateTime() )     )
        {
        // Recurrence is not changed --> No need to set
        retValue = EFalse;
        }

    return retValue;
    }

// EOF

