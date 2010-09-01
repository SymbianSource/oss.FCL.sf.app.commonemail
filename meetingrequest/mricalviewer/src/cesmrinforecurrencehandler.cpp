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
* Description:  ESMR mrinfo recurrence handler implementation
*
*/

#include "emailtrace.h"
#include "cesmrinforecurrencehandler.h"
#include "tesmrinputparams.h"
#include "esmrconfig.hrh"

//<cmail>
#include "mmrinfoobject.h"
#include "mmrrecurrencerule.h"
#include "esmrdef.h"
//</cmail>
#include <calentry.h>
#include <calrrule.h>

// Unnamed namespace for local definitions
namespace {

/**
 * Converts MRINFO day into Symbian date.
 * @param aDay MRINFO day
 * @return Symbian date
 */
TDay MRInfoDay( MRRecurrenceRule::TMRRecurrentDay aDay )
    {
    TDay day( EMonday );

    switch( aDay )
        {
        case MRRecurrenceRule::EMRRecurrenceMonday:
            {
            day = EMonday;
            break;
            }
        case MRRecurrenceRule::EMRRecurrenceTuesday:
            {
            day = ETuesday;
            break;
            }
        case MRRecurrenceRule::EMRRecurrenceWednesday:
            {
            day = EWednesday;
            break;
            }
        case MRRecurrenceRule::EMRRecurrenceThursday:
            {
            day =  EThursday;
            break;
            }
        case MRRecurrenceRule::EMRRecurrenceFriday:
            {
            day = EFriday;
            break;
            }
        case MRRecurrenceRule::EMRRecurrenceSaturday:
            {
            day = ESaturday;
            break;
            }
        case MRRecurrenceRule::EMRRecurrenceSunday:
            {
            day = ESunday;
            break;
            }
        default:
           break;
        }

    return day;
    }

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::CESMRInfoRecurrenceHandler
// ---------------------------------------------------------------------------
//
inline CESMRInfoRecurrenceHandler::CESMRInfoRecurrenceHandler()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::~CESMRInfoRecurrenceHandler
// ---------------------------------------------------------------------------
//
CESMRInfoRecurrenceHandler::~CESMRInfoRecurrenceHandler()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::NewL
// ---------------------------------------------------------------------------
//
CESMRInfoRecurrenceHandler* CESMRInfoRecurrenceHandler::NewL()
    {
    FUNC_LOG;
    CESMRInfoRecurrenceHandler* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::NewLC
// ---------------------------------------------------------------------------
//
CESMRInfoRecurrenceHandler* CESMRInfoRecurrenceHandler::NewLC()
    {
    FUNC_LOG;
    CESMRInfoRecurrenceHandler* self =
        new (ELeave) CESMRInfoRecurrenceHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRInfoRecurrenceHandler::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::ParseRecurrenceInforationL
// ---------------------------------------------------------------------------
//
void CESMRInfoRecurrenceHandler::ParseRecurrenceInforationL(
        CCalEntry& aCalEntry,
        const MMRInfoObject& aInfoObject )
    {
    FUNC_LOG;

    TTime until;

    aCalEntry.ClearRepeatingPropertiesL();
    GetRecurrenceL(
            aCalEntry,
            aInfoObject );

    }

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::GetRecurrenceL
//
// ---------------------------------------------------------------------------
//
void CESMRInfoRecurrenceHandler::GetRecurrenceL(
        CCalEntry& aCalEntry,
        const MMRInfoObject& aInfoObject)
    {
    FUNC_LOG;

    const MRRecurrenceRule& recurrenceRule =
            aInfoObject.RecurrenceRuleL();

    MRRecurrenceRule::TMRRecurrenceType rType(
            recurrenceRule.Type() );

    if ( MRRecurrenceRule::EMRRecurrenceInvalid != rType )
        {
        switch ( rType )
            {
            case MRRecurrenceRule::EMRRecurrenceDaily:
                {
                HandleDailyRecurrenceL(
                        aCalEntry,
                        aInfoObject );
                }
                break;

            case MRRecurrenceRule::EMRRecurrenceWeekly:
                {
                HandleWeeklyRecurrenceL(
                        aCalEntry,
                        aInfoObject );
                }
                break;

            case MRRecurrenceRule::EMRRecurrenceMonthly://fallthrough
            case MRRecurrenceRule::EMRRecurrenceMonthlyByDay:
                {
                HandleMonthlyRecurrenceL(
                        aCalEntry,
                        aInfoObject );
                }
                break;

            case MRRecurrenceRule::EMRRecurrenceYearly:
                {
                HandleYearlyRecurrenceL(
                        aCalEntry,
                        aInfoObject );
                }
                break;
            default:
               break;
            }
        }

    }

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::HandleDailyRecurrenceL
// ---------------------------------------------------------------------------
//
void CESMRInfoRecurrenceHandler::HandleDailyRecurrenceL(
        CCalEntry& aCalEntry,
        const MMRInfoObject& aInfoObject )
    {
    FUNC_LOG;
    const MRRecurrenceRule& recurrenceRule =
            aInfoObject.RecurrenceRuleL();

    TCalRRule rRule;
    rRule.SetType( TCalRRule::EDaily );

    TCalTime meetingStartTime = aCalEntry.StartTimeL();
    rRule.SetDtStart( meetingStartTime );


    TTime until( recurrenceRule.RecurrentUntil() );
    if ( Time::NullTTime() != until )
        {
        TDateTime untilDt = until.DateTime();

        TDateTime endTime =
            aCalEntry.EndTimeL().TimeLocalL().DateTime();
        untilDt.SetHour( endTime.Hour() );
        untilDt.SetMinute( endTime.Minute() );
        untilDt.SetSecond( endTime.Second() );

        TCalTime rEndTime;
        rEndTime.SetTimeLocalL( TTime(untilDt) );

        rRule.SetUntil( rEndTime );
        }
    else
        {
        TInt interval( recurrenceRule.RecurrentInterval() );
        TInt count( recurrenceRule.RecurrentCount() );

        rRule.SetCount( count );
        rRule.SetInterval( interval );
        }

    aCalEntry.SetRRuleL( rRule );
    }

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::HandleWeeklyRecurrenceL
// ---------------------------------------------------------------------------
//
void CESMRInfoRecurrenceHandler::HandleWeeklyRecurrenceL(
        CCalEntry& aCalEntry,
        const MMRInfoObject& aInfoObject )
    {
    FUNC_LOG;
    const MRRecurrenceRule& recurrenceRule =
            aInfoObject.RecurrenceRuleL();

    TCalRRule rRule;
    rRule.SetType( TCalRRule::EWeekly );
    TCalTime meetingStartTime = aCalEntry.StartTimeL();
    rRule.SetDtStart( meetingStartTime );
    TInt interval( recurrenceRule.RecurrentInterval() );

    TTime until( recurrenceRule.RecurrentUntil() );
    if ( Time::NullTTime() != until )
        {
        TDateTime untilDt = until.DateTime();

        TDateTime endTime =
            aCalEntry.EndTimeL().TimeLocalL().DateTime();
        untilDt.SetHour( endTime.Hour() );
        untilDt.SetMinute( endTime.Minute() );
        untilDt.SetSecond( endTime.Second() );

        TCalTime rEndTime;
        rEndTime.SetTimeLocalL( TTime(untilDt) );

        rRule.SetUntil( rEndTime );
        
        rRule.SetInterval( interval );
        }
    else
        {
        TInt count( recurrenceRule.RecurrentCount() );

        rRule.SetCount( count );
        rRule.SetInterval( interval );

        }
    const RArray<MRRecurrenceRule::TMRRecurrentDay>&
    recurrenceDays(
            recurrenceRule.RecurrentWeekDays() );

    if ( recurrenceDays.Count() )
        {
        RArray<TDay> dDays;
        CleanupClosePushL( dDays );
        for ( TInt i(0); i < recurrenceDays.Count(); ++i )
            {
            dDays.Append( MRInfoDay( recurrenceDays[i] ) );
            }

        rRule.SetByDay( dDays );
        CleanupStack::PopAndDestroy( &dDays );
        }

    aCalEntry.SetRRuleL( rRule );
    }

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::HandleMonthlyRecurrenceL
// ---------------------------------------------------------------------------
//
void CESMRInfoRecurrenceHandler::HandleMonthlyRecurrenceL(
        CCalEntry& aCalEntry,
        const MMRInfoObject& aInfoObject )
    {
    FUNC_LOG;
    const MRRecurrenceRule& recurrenceRule =
            aInfoObject.RecurrenceRuleL();

    TCalRRule rRule;
    rRule.SetType( TCalRRule::EMonthly );
    TCalTime meetingStartTime = aCalEntry.StartTimeL();
    rRule.SetDtStart( meetingStartTime );

    TInt interval( recurrenceRule.RecurrentInterval() );

    TTime until( recurrenceRule.RecurrentUntil() );
    if ( Time::NullTTime() != until )
        {
        TDateTime untilDt = until.DateTime();

        TDateTime endTime =
            aCalEntry.EndTimeL().TimeLocalL().DateTime();
        untilDt.SetHour( endTime.Hour() );
        untilDt.SetMinute( endTime.Minute() );
        untilDt.SetSecond( endTime.Second() );

        TCalTime rEndTime;
        rEndTime.SetTimeLocalL( TTime(untilDt) );

        rRule.SetUntil( rEndTime );
        }
    else
        {
        TInt count( recurrenceRule.RecurrentCount() );
        rRule.SetCount( count );
        rRule.SetInterval( interval );
        }

    const RArray<MRRecurrenceRule::TRecurrentDaysofMonth>&
    recurrenceDays(
            recurrenceRule.RecurrentDaysofMonth() );

    const RArray<TInt>& recurrenceDaysInMonth(
            recurrenceRule.RecurrentMonthDays() );

    if ( recurrenceDays.Count() )
        {
        RArray<TCalRRule::TDayOfMonth> dDays;
        CleanupClosePushL( dDays );
        for ( TInt i(0); i < recurrenceDays.Count(); ++i )
            {
            TCalRRule::TDayOfMonth dayOfMonth(
                    MRInfoDay( recurrenceDays[i].iDayOfWeek ),
                    static_cast<TInt8>( recurrenceDays[i].iWeekOfMonth ) );
            }

        rRule.SetByDay( dDays );
        CleanupStack::PopAndDestroy( &dDays );
        }
    else if ( recurrenceDaysInMonth.Count() )
        {
        rRule.SetByMonthDay( recurrenceDaysInMonth );
        }

    aCalEntry.SetRRuleL( rRule );
    }

// ---------------------------------------------------------------------------
// CESMRInfoRecurrenceHandler::HandleYearlyRecurrenceL
// ---------------------------------------------------------------------------
//
void CESMRInfoRecurrenceHandler::HandleYearlyRecurrenceL(
        CCalEntry& aCalEntry,
        const MMRInfoObject& aInfoObject )
    {
    FUNC_LOG;
    const MRRecurrenceRule& recurrenceRule =
            aInfoObject.RecurrenceRuleL();

    TCalRRule rRule;
    rRule.SetType( TCalRRule::EYearly );
    TCalTime meetingStartTime = aCalEntry.StartTimeL();
    rRule.SetDtStart( meetingStartTime );

    TInt interval( recurrenceRule.RecurrentInterval() );

    TTime until( recurrenceRule.RecurrentUntil() );
    if ( Time::NullTTime() != until )
        {
        TDateTime untilDt = until.DateTime();

        TDateTime endTime =
            aCalEntry.EndTimeL().TimeLocalL().DateTime();
        untilDt.SetHour( endTime.Hour() );
        untilDt.SetMinute( endTime.Minute() );
        untilDt.SetSecond( endTime.Second() );

        TCalTime rEndTime;
        rEndTime.SetTimeLocalL( TTime(untilDt) );

        rRule.SetUntil( rEndTime );
        }
    else
        {
        TInt count( recurrenceRule.RecurrentCount() );
        rRule.SetCount( count );
        }

    const RArray<MRRecurrenceRule::TMRRecurrenceMonth>&
    recurrenceMonths(
            recurrenceRule.RecurrentMonths() );

    TInt recurrenceMonthCount( recurrenceMonths.Count() );
    if ( recurrenceMonthCount )
        {
        RArray<TMonth> months;
        CleanupClosePushL( months );
        for ( TInt i(0); i < recurrenceMonthCount; ++i )
            {
            months.Append(
                    static_cast<TMonth>(recurrenceMonths[i]) );
            }

        rRule.SetByMonth( months );
        CleanupStack::PopAndDestroy( &months );
        }

    aCalEntry.SetRRuleL( rRule );
    }

// EOF

