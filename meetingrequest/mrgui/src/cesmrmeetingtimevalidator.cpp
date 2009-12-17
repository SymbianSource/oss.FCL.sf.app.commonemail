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
* Description:  ESMR time and date sanity checks and saving
 *
*/


#include "emailtrace.h"
#include <eikmfne.h>
#include <calentry.h>
#include <calinstance.h>
#include <calalarm.h>
#include <calrrule.h>
#include "cesmrmeetingtimevalidator.h"
#include "cesmrglobalnote.h"
#include "mesmrfieldstorage.h"
#include "mesmrcalentry.h"
#include "mesmrmeetingrequestentry.h"
#include "mesmrfieldeventqueue.h"
#include "cesmrgenericfieldevent.h"

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

// Definition fom max possible time
#define KMaxTTime (TTime(TDateTime(2100, EDecember, 31-2, 23, 59, 59, 999999)))

// Definition fom min possible time
#define KMinTTime (TTime(TDateTime(1900, EJanuary, 2-1, 0, 0, 0, 0)))

// Definition for max alarm duration
#define KMaxAlarmTime TTimeIntervalDays(31)

// Definition for alarm time offset, when alarm is off
const TInt KAlarmNotSet(-1);

#ifdef _DEBUG

// Literal for panics
_LIT( KESMRMeetingTimeValidator, "ESMRMeetingTimeValidator" );

/** Panic code definitions */
enum TKESMRMeetingTimeValidator
    {
    /** Start time field not set */
    EESMRMeetingTimeValidatorNullStartTime,
    /** Stop time field not set */
    EESMRMeetingTimeValidatorNullStopTime,
    /** Start date field not set */
    EESMRMeetingTimeValidatorNullStartDate,
    /** Stop date field not set */
    EESMRMeetingTimeValidatorNullStopDate,
    /** Stop time field not set */
    EESMRMeetingTimeValidatorStartLaterThanEnd,
    /** Invalid date field id */
    EESMRMeetingTimeValidatorInvalidDateField
    };

/**
 * Raises system panic.
 * @param aPanic Panic code
 * @see TESMRRecurrentEditValidatorPanic
 */
void Panic( TKESMRMeetingTimeValidator aPanic )
    {
    User::Panic( KESMRMeetingTimeValidator, aPanic );
    }

#endif // _DEBUG

const TInt KZero( 0 );
const TInt KOne( 1 );
const TInt KDaysInWeek( 7 );
const TInt KDaysInFortnight( 14 );
const TInt KNoon(12);

/**
 * Calculates time for next instance for recurrent event.
 * @param aRecurrenceValue Defines the used recurrence.
 * @param aPrevInstanceStartTime Start time of the previous instance.
 */
TTime TimeForNextInstaceStartTime(
        TESMRRecurrenceValue aRecurrenceValue,
        TTime& aPrevInstanceStartTime )
    {
    TTime nextStartTime = aPrevInstanceStartTime;

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
            nextStartTime += TTimeIntervalDays( KDaysInFortnight );
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
            nextStartTime = aPrevInstanceStartTime;
            break;
        }

    return nextStartTime;
    }

/**
 * Returns the default alarm time for all day event
 * @param allday event start time
 * @return alarm time
 */
TTime DefaultAlldayEventAlarmTime( TTime& aMeetingStartTime )
    {
    TTime defaultAlarmTime = aMeetingStartTime - TTimeIntervalHours(KNoon);

    if ( defaultAlarmTime < KMinTTime )
        {
        defaultAlarmTime = KMinTTime;
        }
    return defaultAlarmTime;
    }


/**
 * Sets time to editor. Time is checked and adjusted
 * between min and max before setting
 * @param aEditor Reference to time editor
 * @param aTime On return contains the set time.
 */
void SetTimeToEditor(
        CEikTimeEditor& aEditor,
        TTime& aTime )
    {
    if ( aTime < KMinTTime )
        {
        aTime = KMinTTime;
        }
    else if ( aTime > KMaxTTime )
        {
        aTime = KMaxTTime;
        }

    if ( aEditor.IsVisible() )
        {
        aEditor.SetTime( aTime );
        }
    }

/**
 * Sets date to editor. Date is checked and adjusted
 * between min and max before setting
 * @param aEditor Reference to time editor
 * @param aDate On return contains the set date.
 */
void SetDateToEditor(
        CEikDateEditor& aEditor,
        TTime& aDate )
    {
    if ( aDate < KMinTTime )
        {
        aDate = KMinTTime;
        }
    else if ( aDate > KMaxTTime )
        {
        aDate = KMaxTTime;
        }

    if ( aEditor.IsVisible() )
        {
        aEditor.SetDate( aDate );
        }
    }

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::CESMRMeetingTimeValidator
// ---------------------------------------------------------------------------
//
inline CESMRMeetingTimeValidator::CESMRMeetingTimeValidator()
:   iRecurrenceValue( ERecurrenceNot ),
    iCurrentStartTime( Time::NullTTime() ),
    iCurrentEndTime( Time::NullTTime() ),
    iCurrentAlarmTime( Time::NullTTime() ),
    iCurrentRecurrenceUntil( Time::NullTTime() ),
    iStartTimeBeforeAlldayEvent( Time::NullTTime() ),
    iEndTimeBeforeAlldayEvent( Time::NullTTime() ),
    iAlldayEvent( EFalse ),
    iAlarmOnOff( EFalse ),
    iComparativeStartTime( Time::NullTTime().DateTime() )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::~CESMRMeetingTimeValidator
// ---------------------------------------------------------------------------
//
CESMRMeetingTimeValidator::~CESMRMeetingTimeValidator( )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::NewL
// ---------------------------------------------------------------------------
//
CESMRMeetingTimeValidator* CESMRMeetingTimeValidator::NewL( )
    {
    FUNC_LOG;
    CESMRMeetingTimeValidator* self = new (ELeave) CESMRMeetingTimeValidator();
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::ValidateL
// ---------------------------------------------------------------------------
//
MESMRFieldValidator::TESMRFieldValidatorError
        CESMRMeetingTimeValidator::ValidateL(
                    TBool aCorrectAutomatically )
    {
    FUNC_LOG;
    MESMRFieldValidator::TESMRFieldValidatorError KErrorNone(
            MESMRFieldValidator::EErrorNone );

    MESMRFieldValidator::TESMRFieldValidatorError error(
            MESMRFieldValidator::EErrorNone );

    if( aCorrectAutomatically )
        {
        ForceValuesL();
        }
    else
        {
        TTime startTime = StartDateTime();
        TTime endTime = EndDateTime();

        // In recurrent events if user has changed the start date and
        // presses Save or Done, we have to force the end date to same
        // value as start date and modify the recurrence until according
        // to the new start time value
        if ( ERecurrenceNot != iRecurrenceValue &&
                ( startTime.DateTime().Day() != endTime.DateTime().Day() ||
                startTime.DateTime().Month() != endTime.DateTime().Month() ||
                startTime.DateTime().Year() != endTime.DateTime().Year() ) )
            {
            endTime = ForceEndDateTime();

            SetTimeToEditor( *iEndTime, endTime );
            SetDateToEditor( *iEndDate, endTime );

            // We have to also adjust the recurrence until time
            // according to the new start time
            TTime newUntil = ForceRecurrenceUntilTime();

            SetDateToEditor( *iRecurrenceUntilDate, newUntil );
            }

        TTime currentAlarmTime = startTime - iCurrentAlarmTimeOffset;
        TTime currentTime;
        currentTime.HomeTime();

        if ( endTime < startTime )
            {
            error = MESMRFieldValidator::EErrorEndEarlierThanStart;
            }

        if ( KErrorNone == error && ERecurrenceNot != iRecurrenceValue )
            {
            TDateTime startDt = startTime.DateTime();
            TDateTime endDt = endTime.DateTime();

            if ( startDt.Day() != endDt.Day() ||
                    startDt.Month() != endDt.Month() ||
                    startDt.Year() != endDt.Year() )
                {
                error = MESMRFieldValidator::EErrorRecDifferetStartAndEnd;
                }

            if ( KErrorNone == error &&
                 ERecurrenceNot != iRecurrenceValue &&
                 iRecurrenceUntilDate && iRecurrenceUntilDate->IsVisible() )
                {
                TTime recurUntil = RecurrenceUntilTime();
                if ( ERecurrenceUnknown != iRecurrenceValue &&
                     recurUntil < startTime )
                    {
                    error = MESMRFieldValidator::EErrorRecUntilEarlierThanStart;
                    }
                }
            }

        if (  KErrorNone == error &&  
              MESMRCalEntry::EESMRCalEntryMeetingRequest == iEntry->Type() )
            {
            // Check that is modified entry 
            MESMRMeetingRequestEntry* mrEntry = 
                    static_cast<MESMRMeetingRequestEntry*>( iEntry );
            if ( mrEntry->IsRecurrentEventL() &&
                 MESMRCalEntry::EESMRThisOnly == mrEntry->RecurrenceModRule() )
                {
                TBool instanceAlreadyOnThisDay( 
                           mrEntry->AnyInstanceOnDayL(startTime, endTime) );
                
                if ( instanceAlreadyOnThisDay )
                    {
                    error = MESMRFieldValidator::EErrorRescheduleInstance;
                    }
                }
            }         
        
        if ( KErrorNone == error && iAlldayEvent && iAlarmOnOff )
            {
            TTime alarmTime = AlarmDateTime();
            if ( alarmTime > startTime )
                {
                SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
                SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );

                error = MESMRFieldValidator::EErrorAlarmLaterThanStart;
                }

            TTime currentTime;
            currentTime.HomeTime();
            if ( KErrorNone == error && alarmTime < currentTime )
                {
                error = MESMRFieldValidator::EErrorAlarmInPast;
                }
            if ( KErrorNone == error && alarmTime < startTime )
                {
                TTimeIntervalDays alarmBefore = startTime.DaysFrom(alarmTime);
                if ( alarmBefore > KMaxAlarmTime )
                    {
                    error =  MESMRFieldValidator::EErrorAlarmTooMuchInPast;
                    }
                }
            }

        MESMRCalEntry::TESMRRecurrenceModifyingRule rule =
            MESMRCalEntry::EESMRThisOnly;

        // Checking if entry is recurrent
        if ( iEntry->IsRecurrentEventL())
            {
            // Getting the recurrence information
            rule = iEntry->RecurrenceModRule();
            }

        // Showing error note if alarm in past, when
        // entry has no recurrence
        if ( KErrorNone == error &&
             currentAlarmTime < currentTime &&
             !iAlldayEvent &&
             iCurrentAlarmTimeOffset.Int() != KAlarmNotSet &&
             iRecurrenceValue == ERecurrenceNot )
            {
            error = MESMRFieldValidator::EErrorRelativeAlarmInPast;
            }

        TTime until;
        if ( iRecurrenceValue != ERecurrenceNot &&
             iRecurrenceValue != ERecurrenceUnknown )
            {
            until = RecurrenceUntilTime();

            // Showing error note if is alarm in past, 
            // when entry has recurrence
            if ( KErrorNone == error &&
                 iCurrentAlarmTimeOffset.Int() != KAlarmNotSet &&
                 until < currentTime &&
                 rule != MESMRCalEntry::EESMRAllInSeries )
                {
                error = MESMRFieldValidator::EErrorRelativeAlarmInPast;
                }
            }
        }
    return error;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::ReadValuesFromEntryL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::ReadValuesFromEntryL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    MESMRCalEntry::TESMRCalEntryType entryType( aEntry.Type() );

    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == entryType ||
         MESMRCalEntry::EESMRCalEntryMeeting == entryType   )
        {
        // Meeting or meeting request
        __ASSERT_DEBUG(
                iStartTime,
                Panic(EESMRMeetingTimeValidatorNullStartTime) );

        __ASSERT_DEBUG(
                iEndTime,
                Panic(EESMRMeetingTimeValidatorNullStopTime) );

        __ASSERT_DEBUG(
                iStartDate,
                Panic(EESMRMeetingTimeValidatorNullStartTime) );

        __ASSERT_DEBUG(
                iEndDate,
                Panic(EESMRMeetingTimeValidatorNullStopTime) );

        iEntry = &aEntry;
        CCalEntry& entry( aEntry.Entry() );

        iCurrentStartTime = entry.StartTimeL().TimeLocalL();
        iCurrentEndTime   = entry.EndTimeL().TimeLocalL();

        //Comparative start time required for comparisons in editor
        //recurrence handling
        iComparativeStartTime = iCurrentStartTime.DateTime();

        if ( aEntry.IsAllDayEventL() )
            {
            iCurrentEndTime -= TTimeIntervalDays( KOne );
            }

        TTime start = iCurrentStartTime;
        TTime end = iCurrentEndTime;

        SetTimeToEditor( *iStartTime, start );
        SetDateToEditor( *iStartDate, start );

        SetTimeToEditor( *iEndTime, end );
        SetDateToEditor( *iEndDate, end );

        ReadAlarmFromEntryL( entry );
        ReadRecurrenceFromEntryL( aEntry );

        DrawEditorsDeferred();
        }
    }
// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::StoreValuesToEntryL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::StoreValuesToEntryL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalEntry& entry( aEntry.Entry() );

    // Before writing start and end time to enty, let's
    // check is currently edited entry exception for series
    // If entry is an exception and start time is set after
    // repeat until date; the calinterim API will delete
    // the recurrence rules from entry, so the
    // solution to avoid this is to move the repeat until date
    // to match the new start and end time for this exception entry.
    if ( ERecurrenceNot == iRecurrenceValue &&
         aEntry.IsRecurrentEventL() &&
         MESMRCalEntry::EESMRThisOnly == aEntry.RecurrenceModRule() )
        {
        TCalRRule originalRRule;
        if ( iEntry->Entry().GetRRuleL( originalRRule ) )
            {
            TTime localUntilTime = originalRRule.Until().TimeLocalL();
            TTime endTime = EndDateTime();
            TDateTime datetimeendtime = endTime.DateTime();
            TDateTime datetimelocalUntil = localUntilTime.DateTime();

            if ( endTime > localUntilTime )
                {
                TCalRRule rRule;
                rRule.SetDtStart( originalRRule.DtStart() );
                rRule.SetType( originalRRule.Type() );

                TCalTime newUntil;
                newUntil.SetTimeLocalL( endTime );
                rRule.SetUntil( newUntil );
                rRule.SetInterval( originalRRule.Interval() );
                iEntry->Entry().SetRRuleL( rRule );
                }
            }
        }
    else if ( aEntry.IsRecurrentEventL() &&
              MESMRCalEntry::EESMRAllInSeries == aEntry.RecurrenceModRule() )
        {
        TTime orgStartTime = iEntry->Entry().StartTimeL().TimeLocalL();
        TTime orgEndTime = iEntry->Entry().EndTimeL().TimeLocalL();
        
        if ( orgStartTime != StartDateTime() || 
             orgEndTime != EndDateTime() )
            {
            // We are editing the series --> We need to clear the recurrence from the entry
            // When changing start or end time --> Exceptions needs to be cleared from the series
            // It is set later in this method.
            aEntry.SetRecurrenceL( ERecurrenceNot, Time::NullTTime() );
            }
        }

    WriteStartAndEndTimeToEntryL( aEntry );

    if ( iAlldayEvent )
        {
        TTime startTime = StartDateTime();
        TTime endTime = EndDateTime();

        // For all day event end time is adjusted 24 hours 
        // forward so that it ends correctly
        aEntry.SetAllDayEventL( startTime, endTime );

        if ( iAlarmOnOff )
            {
            TTime alarmTime = AlarmDateTime();
            TTimeIntervalMinutes alarmDiff;
            startTime.MinutesFrom( alarmTime, alarmDiff );

            CCalAlarm* alarm = CCalAlarm::NewL();
            CleanupStack::PushL( alarm );
            alarm->SetTimeOffset( alarmDiff );

            entry.SetAlarmL( alarm );
            CleanupStack::PopAndDestroy( alarm );
            alarm = NULL;
            }
        else
            {
            entry.SetAlarmL( NULL );
            }
        }

    if ( ERecurrenceNot != iRecurrenceValue &&
         ERecurrenceUnknown != iRecurrenceValue )
        {
        aEntry.SetRecurrenceL(
                iRecurrenceValue,
                RecurrenceUntilTime() );
        }
    else if ( ERecurrenceNot == iRecurrenceValue &&
              aEntry.IsRecurrentEventL() &&
              MESMRCalEntry::EESMRAllInSeries == aEntry.RecurrenceModRule() )
        {
        aEntry.SetRecurrenceL(
                iRecurrenceValue,
                Time::NullTTime() );
        }
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::SetStartTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SetStartTimeFieldL(
        CEikTimeEditor& aStartTime )
    {
    FUNC_LOG;
    iStartTime = &aStartTime;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::SetEndTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SetEndTimeFieldL(
        CEikTimeEditor& aEndTime )
    {
    FUNC_LOG;
    iEndTime = &aEndTime;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::SetStartDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SetStartDateFieldL(
        CEikDateEditor& aStartDate )
    {
    FUNC_LOG;
    iStartDate = &aStartDate;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::SetEndDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SetEndDateFieldL(
        CEikDateEditor& aEndTime )
    {
    FUNC_LOG;
    iEndDate = &aEndTime;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::SetAlarmTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SetAlarmTimeFieldL(
        CEikTimeEditor& aAlarmTime )
    {
    FUNC_LOG;
    iAlarmTime = &aAlarmTime;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::SetAlarmDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SetAlarmDateFieldL(
        CEikDateEditor& aAlarmDate )
    {
    FUNC_LOG;
    iAlarmDate = &aAlarmDate;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::StartTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SetRecurrenceUntilDateFieldL(
        CEikDateEditor& aRecurrenceUntil )
    {
    FUNC_LOG;
    iRecurrenceUntilDate = &aRecurrenceUntil;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::StartTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::StartTimeChangedL()
    {
    FUNC_LOG;
    StartDateChandedL();
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::EndTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::EndTimeChangedL()
    {
    FUNC_LOG;
    PreValidateEditorContent();
    HandleEndTimeChangeL();
    DrawEditorsDeferred();
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::StartDateChandedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::StartDateChandedL()
    {
    FUNC_LOG;
    PreValidateEditorContent();    
    HandleStartTimeChangeL();
    HandleAlarmTimeChangedL();
    DrawEditorsDeferred();
    SendFieldChangeEventL( EESMRFieldStartDate );
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::EndDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::EndDateChangedL()
    {
    FUNC_LOG;
    TInt err( KErrNone );

    PreValidateEditorContent();

    TTime start = StartDateTime();
    TTime end = EndDateTime();

    if ( end < start )
        {
        err = KErrArgument;
        }

    if ( err == KErrNone )
        {
        HandleEndTimeChangeL();
        HandleAlarmTimeChangedL();
        }
    else
        {
        SetTimeToEditor( *iEndTime, iCurrentEndTime );
        SetDateToEditor( *iEndDate, iCurrentEndTime );
        }

    DrawEditorsDeferred();

    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::AlarmTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::AlarmTimeChangedL()
    {
    FUNC_LOG;
    AlarmDateChangedL();
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::AlarmDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::AlarmDateChangedL()
    {
    FUNC_LOG;
    if ( iAlarmOnOff && iAlarmTime && iAlarmDate && iAlldayEvent )
        {
        PreValidateEditorContent();
        HandleAlarmTimeChangedL();
        DrawEditorsDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::RelativeAlarmChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::RelativeAlarmChangedL(
                                TTimeIntervalMinutes aCurrentAlarmTimeOffset,
                                TBool aHandleAlarmChange,
                                TBool& aRelativeAlarmValid )
    {
    FUNC_LOG;
    iRelativeAlarmValid = &aRelativeAlarmValid;
    iCurrentAlarmTimeOffset = aCurrentAlarmTimeOffset;

    if ( aHandleAlarmChange )
        {
        HandleRelativeAlarmTimeChangedL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::SetAllDayEventL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SetAllDayEventL(
        TBool aAlldayEvent )
    {
    FUNC_LOG;
    if ( iAlldayEvent != aAlldayEvent )
        {
        iAlldayEvent = aAlldayEvent;

        if ( iAlldayEvent )
            {
            iStartTimeBeforeAlldayEvent = iCurrentStartTime;
            iEndTimeBeforeAlldayEvent = iCurrentEndTime;

            TTime start = StartDateTime();
            iCurrentStartTime = StartDateTime();
            iCurrentEndTime = EndDateTime();

            if ( iAlarmOnOff )
                {
                TTime alarm = DefaultAlldayEventAlarmTime( start );
                SetTimeToEditor( *iAlarmTime, alarm );
                SetDateToEditor( *iAlarmDate, alarm );
                iCurrentAlarmTime = alarm;
                }

            DrawEditorsDeferred();
            }
        else
            {
            SetTimeToEditor( *iStartTime, iStartTimeBeforeAlldayEvent );
            SetTimeToEditor( *iEndTime, iEndTimeBeforeAlldayEvent );

            TTime start = StartDateTime();
            TTime end = EndDateTime();

            if ( start > end )
                {
                // User has modified dates so that end time
                // is earlier than start time
                end = start;
                }

            SetTimeToEditor( *iStartTime, start );
            SetDateToEditor( *iStartDate, start );

            SetTimeToEditor( *iEndTime, end );
            SetDateToEditor( *iEndDate, end );

            iCurrentStartTime = start;
            iCurrentEndTime = end;

            DrawEditorsDeferred();
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::SetAlarmOnOffL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SetAlarmOnOffL(
        TBool aAlarmOn )
    {
    FUNC_LOG;
    iAlarmOnOff = aAlarmOn;

    if ( iAlarmOnOff && iAlldayEvent )
        {
        TTime nullTime = Time::NullTTime();

        if ( iCurrentAlarmTime > iCurrentStartTime ||
             iCurrentAlarmTime == nullTime )
            {
            iCurrentAlarmTime = 
				DefaultAlldayEventAlarmTime( iCurrentStartTime );
            SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
            SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::RecurrenceChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::RecurrenceChangedL(
        TESMRRecurrenceValue aRecurrence )
    {
    FUNC_LOG;
    if ( aRecurrence != iRecurrenceValue )
        {

        TDateTime start = StartDateTime();
        TDateTime end = EndDateTime();

        if ( start.Day() != end.Day() ||
             start.Month() != end.Month() ||
             start.Year() != end.Year())
            {
            User::Leave( KErrArgument );
            }

        iRecurrenceValue = aRecurrence;
        PreValidateEditorContent();
        HandleRecurrenceTypeChanged();
        DrawEditorsDeferred();
        }

    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::RecurrenceEndDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::RecurrenceEndDateChangedL()
    {
    FUNC_LOG;
    PreValidateEditorContent();
    HandleRecurrenceEndDateChangedL();
    DrawEditorsDeferred();
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::IsRelativeAlarmValid
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingTimeValidator::IsRelativeAlarmValid(
		TTimeIntervalMinutes aAlarmTimeOffset )
    {
    FUNC_LOG;
    TBool validity( ETrue );
    TTime startTime = StartDateTime();
    TTime currentTime;
    currentTime.HomeTime();

    if ( ( startTime - aAlarmTimeOffset ) < currentTime &&
            aAlarmTimeOffset.Int() != KAlarmNotSet )
        {
        validity = EFalse;
        }

    return validity;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::SetFieldEventQueue
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SetFieldEventQueue(
        MESMRFieldEventQueue* aEventQueue )
    {
    FUNC_LOG;
    iEventQueue = aEventQueue;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::PreValidateEditorContent
// ---------------------------------------------------------------------------
//
TInt CESMRMeetingTimeValidator::PreValidateEditorContent()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TRAP( err, PreValidateEditorContentL() );
    return err;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::PreValidateEditorContentL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::PreValidateEditorContentL()
    {
    FUNC_LOG;
    if (iStartTime && iStartTime->IsVisible() )
        {
        iStartTime->PrepareForFocusLossL();
        }
    if (iEndTime && iEndTime->IsVisible() )
        {
        iEndTime->PrepareForFocusLossL();
        }
    if (iStartDate && iStartDate->IsVisible() )
        {
        iStartDate->PrepareForFocusLossL();
        }
    if (iEndDate && iEndDate->IsVisible() )
        {
        iEndDate->PrepareForFocusLossL();
        }
    if ( iAlarmOnOff && iAlarmTime && iAlldayEvent && iAlarmTime->IsVisible() )
        {
        TDateTime dt = iAlarmTime->Time().DateTime();
        iAlarmTime->PrepareForFocusLossL();
        }
    if ( iAlarmOnOff && iAlarmDate && iAlldayEvent && iAlarmDate->IsVisible() )
        {
        iAlarmDate->PrepareForFocusLossL();
        }
    if ( iRecurrenceUntilDate && iRecurrenceUntilDate->IsVisible() )
        {
        iRecurrenceUntilDate->PrepareForFocusLossL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::HandleStartTimeChangeL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::HandleStartTimeChangeL()
    {
    FUNC_LOG;
    TTime startTime = StartDateTime();
    TTime endTime   = EndDateTime();
    
    SetTimeToEditor( *iStartTime, startTime );
    SetDateToEditor( *iStartDate, startTime );

    TTimeIntervalMinutes startTimeChange;
    User::LeaveIfError(
            startTime.MinutesFrom(
                    iCurrentStartTime,
                    startTimeChange ) );

    endTime += startTimeChange;
    
    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == iEntry->Type() )
        {
        MESMRMeetingRequestEntry* mrEntry = 
                static_cast<MESMRMeetingRequestEntry*>( iEntry );
        if ( mrEntry->IsRecurrentEventL() &&
             MESMRCalEntry::EESMRThisOnly == mrEntry->RecurrenceModRule() )
            {
            TBool instanceAlreadyOnThisDay( 
                       mrEntry->AnyInstanceOnDayL(startTime, endTime) );
            
            if ( instanceAlreadyOnThisDay )
                {
                // Restore previous time
                SetTimeToEditor( *iStartTime, iCurrentStartTime );
                SetDateToEditor( *iStartDate, iCurrentStartTime );
                
                User::Leave( KErrOverflow );
                }
            }
        }
    
    SetTimeToEditor( *iEndTime, endTime );
    SetDateToEditor( *iEndDate, endTime );

    iCurrentStartTime = startTime;
    iCurrentEndTime = endTime;

    if ( iAlldayEvent && iAlarmOnOff &&
         iAlarmTime->IsVisible() && iAlarmDate->IsVisible() )
        {
        TTime alarmTime = AlarmDateTime();
        alarmTime += startTimeChange;

        SetTimeToEditor( *iAlarmTime, alarmTime );
        SetDateToEditor( *iAlarmDate, alarmTime );

        iCurrentAlarmTime = alarmTime;
        }

    if ( ERecurrenceNot != iRecurrenceValue &&
            iRecurrenceUntilDate && iRecurrenceUntilDate->IsVisible() )
        {
        TTime recUntil = RecurrenceUntilTime();

        if ( startTime.DateTime().Day() != iComparativeStartTime.Day() ||
             startTime.DateTime().Month() != iComparativeStartTime.Month() ||
             startTime.DateTime().Year() != iComparativeStartTime.Year() )
            {
            // We want to update the recurrence until only when
            // a) Day, b) Month, c) or Year of the start time has changed
            recUntil += startTimeChange;
            iComparativeStartTime = startTime.DateTime();
            }

        SetDateToEditor( *iRecurrenceUntilDate, recUntil );
        iCurrentRecurrenceUntil = recUntil;
        }
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::HandleEndTimeChangeL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::HandleEndTimeChangeL()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TTime startTime = StartDateTime();
    TTime endTime   = EndDateTime();

    if ( !iAlldayEvent && endTime < startTime )
        {
        if( ERecurrenceNot != iRecurrenceValue )
            {
            err = KErrArgument;
            }
        else
            {
            // End time is earlier than start time
            // and this is not allday event.
            endTime += TTimeIntervalDays( KOne );
            }
        }

    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == iEntry->Type() )
        {
        // Check that is modified entry 
        MESMRMeetingRequestEntry* mrEntry = 
                static_cast<MESMRMeetingRequestEntry*>( iEntry );
        if ( mrEntry->IsRecurrentEventL() &&
             MESMRCalEntry::EESMRThisOnly == mrEntry->RecurrenceModRule() )
            {
            TBool instanceAlreadyOnThisDay( 
                       mrEntry->AnyInstanceOnDayL(startTime, endTime) );
            
            if ( instanceAlreadyOnThisDay )
                {
                // Restore previous time
                SetTimeToEditor( *iStartTime, iCurrentStartTime );
                SetDateToEditor( *iStartDate, iCurrentStartTime );
                
                err = KErrOverflow;
                }
            }
        }    
    
    if ( KErrNone == err )
        {
        iCurrentStartTime = startTime;
        iCurrentEndTime = endTime;
        }

    if ( !iAlldayEvent )
        {
        SetTimeToEditor( *iStartTime, iCurrentStartTime );
        SetTimeToEditor( *iEndTime, iCurrentEndTime );
        }

    SetDateToEditor( *iStartDate, iCurrentStartTime );
    SetDateToEditor( *iEndDate, iCurrentEndTime );

    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::HandleAlarmTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::HandleAlarmTimeChangedL()
    {
    FUNC_LOG;
    TInt err(KErrNone);

    if ( iAlarmOnOff && iAlarmTime && iAlarmDate && iAlldayEvent )
        {
        TTime alarm = AlarmDateTime();
        TTime start = StartDateTime();

        if ( start < alarm )
            {
            alarm = iCurrentAlarmTime;
            err = KErrArgument;
            }
        else
            {
            iCurrentAlarmTime = alarm;
            }

        SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
        SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );
        }

    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::HandleRelativeAlarmTimeChangedL()
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::HandleRelativeAlarmTimeChangedL()
    {
    FUNC_LOG;
    TInt err( KErrNone );

    // Getting current time
    TTime currentTime;
    currentTime.HomeTime();

    // Getting meeting start time
    TTime meetingStartTime = StartDateTime();

    // Getting current alarm time
    TTime currentAlarmTime = meetingStartTime - iCurrentAlarmTimeOffset;

    // If alarm is set, entry is not recurrent, but alarm occures in past
    if ( currentAlarmTime < currentTime &&
            iCurrentAlarmTimeOffset.Int() != KAlarmNotSet &&
            iRecurrenceValue == ERecurrenceNot )
        {
        err = KErrArgument;
        }

    if ( iRecurrenceValue != ERecurrenceNot &&
            iRecurrenceValue != ERecurrenceUnknown )
        {
        TTime until = RecurrenceUntilTime();

        // if alarm is set, entry is recurrent, but until date is in past
        if ( iCurrentAlarmTimeOffset.Int() != KAlarmNotSet &&
                until < currentTime )
            {
            err = KErrArgument;
            }
        }
    
    User::LeaveIfError( err );
    }
// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::HandleRecurrenceTypeChanged
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::HandleRecurrenceTypeChanged()
    {
    FUNC_LOG;
    if ( ERecurrenceNot != iRecurrenceValue )
        {
        TTime start = StartDateTime();
        TTime minTimeForRecurrenceEnd =
                TimeForNextInstaceStartTime( iRecurrenceValue, start );

        TDateTime a = iCurrentRecurrenceUntil.DateTime();
        TDateTime b = minTimeForRecurrenceEnd.DateTime();

        if ( iCurrentRecurrenceUntil < minTimeForRecurrenceEnd )
            {
            iCurrentRecurrenceUntil = minTimeForRecurrenceEnd;
            }
        }
    else
        {
        iCurrentRecurrenceUntil = StartDateTime();
        }

    if ( iRecurrenceUntilDate )
        {
        SetDateToEditor( *iRecurrenceUntilDate, iCurrentRecurrenceUntil );
        }
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::HandleRecurrenceEndDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::HandleRecurrenceEndDateChangedL()
    {
    FUNC_LOG;
    TInt err( KErrNone );

    TTime start = StartDateTime();
    TTime recurrenceUntilTime = RecurrenceUntilTime();

    if ( recurrenceUntilTime <= start )
        {
        err = KErrArgument;
        }
    else
        {
        iCurrentRecurrenceUntil = recurrenceUntilTime;
        }

    if ( iRecurrenceUntilDate )
        {
        SetDateToEditor( *iRecurrenceUntilDate, iCurrentRecurrenceUntil );
        }

    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::DrawEditorsDeferred
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::DrawEditorsDeferred()
    {
    FUNC_LOG;
    if (iStartTime && iStartTime->IsVisible() )
        {
        iStartTime->DrawDeferred();
        }
    if (iEndTime && iEndTime->IsVisible() )
        {
        iEndTime->DrawDeferred();
        }
    if (iStartDate && iStartDate->IsVisible() )
        {
        iStartDate->DrawDeferred();
        }
    if (iEndDate && iEndDate->IsVisible() )
        {
        iEndDate->DrawDeferred();
        }
    if ( iAlarmOnOff && iAlarmTime && iAlldayEvent && iAlarmTime->IsVisible()  )
        {
        iAlarmTime->DrawDeferred();
        }
    if ( iAlarmOnOff && iAlarmDate && iAlldayEvent  && iAlarmDate->IsVisible() )
        {
        iAlarmDate->DrawDeferred();
        }
    if ( iRecurrenceUntilDate  && iRecurrenceUntilDate->IsVisible() )
        {
        iRecurrenceUntilDate->DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::StartDateTime
// ---------------------------------------------------------------------------
//
TDateTime CESMRMeetingTimeValidator::StartDateTime()
    {
    FUNC_LOG;
    TDateTime startDateTime = iStartDate->Date().DateTime();
    TDateTime startTime = iStartTime->Time().DateTime();

    if ( iAlldayEvent )
        {
        startDateTime.SetHour( KZero );
        startDateTime.SetMinute( KZero );
        startDateTime.SetSecond( KZero );
        }
    else
        {
        startDateTime.SetHour( startTime.Hour() );
        startDateTime.SetMinute( startTime.Minute() );
        startDateTime.SetSecond( startTime.Second() );
        }

    return startDateTime;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::EndDateTime
// ---------------------------------------------------------------------------
//
TDateTime CESMRMeetingTimeValidator::EndDateTime()
    {
    FUNC_LOG;
    TDateTime endDateTime = iEndDate->Date().DateTime();
    TDateTime endTime = iEndTime->Time().DateTime();

    if ( iAlldayEvent )
        {
        endDateTime.SetHour( KZero );
        endDateTime.SetMinute( KZero );
        endDateTime.SetSecond( KZero );
        }
    else
        {
        endDateTime.SetHour( endTime.Hour() );
        endDateTime.SetMinute( endTime.Minute() );
        endDateTime.SetSecond( endTime.Second() );
        }

    return endDateTime;
    }


// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::AlarmDateTime
// ---------------------------------------------------------------------------
//
TDateTime CESMRMeetingTimeValidator::AlarmDateTime()
    {
    FUNC_LOG;
    TDateTime alarmDateTime = iAlarmDate->Date().DateTime();
    TDateTime alarmTime = iAlarmTime->Time().DateTime();

    alarmDateTime.SetHour( alarmTime.Hour() );
    alarmDateTime.SetMinute( alarmTime.Minute() );
    alarmDateTime.SetSecond( alarmTime.Second() );

    return alarmDateTime;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::RecurrenceUntilTime
// ---------------------------------------------------------------------------
//
TDateTime CESMRMeetingTimeValidator::RecurrenceUntilTime()
    {
    FUNC_LOG;
    TDateTime recurrenceUntil =
            iRecurrenceUntilDate->Date().DateTime();
    TDateTime startTime = iStartTime->Time().DateTime();

    recurrenceUntil.SetHour( startTime.Hour() );
    recurrenceUntil.SetMinute( startTime.Minute() );
    recurrenceUntil.SetSecond( startTime.Second() );

    return recurrenceUntil;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::ReadAlarmFromEntryL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::ReadAlarmFromEntryL(
        CCalEntry& aEntry )
    {
    FUNC_LOG;
    // Alarm is set for validating
    CCalAlarm *alarm = aEntry.AlarmL();
    CleanupStack::PushL( alarm );

    if ( alarm )
        {
        // Alarm is set
        iCurrentAlarmTime = iCurrentStartTime - alarm->TimeOffset();
        }
    else
        {
        iCurrentAlarmTime = Time::NullTTime();
        iAlarmOnOff = EFalse;
        }

    CleanupStack::PopAndDestroy( alarm );
    alarm = NULL;

    if ( iAlarmDate && iAlarmTime && iAlarmOnOff )
        {
        SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
        SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );
        }
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::ReadRecurrenceFromEntryL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::ReadRecurrenceFromEntryL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( aEntry.IsRecurrentEventL() &&
            MESMRCalEntry::EESMRAllInSeries == aEntry.RecurrenceModRule() )
        {
        aEntry.GetRecurrenceL(
                iRecurrenceValue,
                iCurrentRecurrenceUntil );

        if ( iRecurrenceUntilDate && iRecurrenceUntilDate->IsVisible() )
            {
            // Recurrence time has to be shown in the editor as local time
            TCalTime recurrenceTime;
            recurrenceTime.SetTimeUtcL( iCurrentRecurrenceUntil );
            TTime localRecurrenceTime = recurrenceTime.TimeLocalL();
            
            SetDateToEditor( *iRecurrenceUntilDate, localRecurrenceTime );
            }
        }
    else
        {
        iRecurrenceValue = ERecurrenceNot;        
        }

    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::ForceValuesL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::ForceValuesL()
    {
    FUNC_LOG;
    TTime startTime = StartDateTime();
    TTime endTime = EndDateTime();

    if ( endTime < startTime )
        {
        // For end and start date following check and correction is made:
        // - Entry ends earlier than starts
        //    --> For stored entries, orginal values are restored.
        //    --> For unsaved entries endTime is set to start time.
        //
        if ( iEntry->IsStoredL() )
            {
            // If entry is stored, orginal values are restored
            const CCalEntry& orginalEntry( iEntry->OriginalEntry() );
            startTime = orginalEntry.StartTimeL().TimeLocalL();
            endTime = orginalEntry.EndTimeL().TimeLocalL();
            }
        else
            {
            endTime = startTime;
            }
        }

    // if entry has not directly supported recurrence value
    // e.g. every second day; the repeat until field is
    // in read only mode and iRecurrenceUntilDate is
    // not set to validator
    if ( iRecurrenceUntilDate &&
         ERecurrenceNot != iRecurrenceValue )
        {
        // For recurrent event following checks and corrections are made:
        //   - Start and end date are the same. If not --> end = start
        //   - recurrence until is not earlier than start
        //     --> recurrenceUntil = start + one year
        //
        TDateTime startDt = startTime.DateTime();
        TDateTime endDt = endTime.DateTime();

        if ( startDt.Day() != endDt.Day() ||
             startDt.Month() != endDt.Month() ||
             startDt.Year() != endDt.Year() )
            {
            // Setting start and end date to be the same day
            endDt.SetDay( startDt.Day() );
            endDt.SetMonth( startDt.Month() );
            endDt.SetYear( startDt.Year() );
            endTime = endDt;
            }

        TTime recurUntil = RecurrenceUntilTime();
        if ( ERecurrenceUnknown != iRecurrenceValue &&
             recurUntil < startTime )
            {
            recurUntil = startTime + TTimeIntervalYears( KOne );
            iCurrentRecurrenceUntil = recurUntil;
            }
        }

    if ( iAlldayEvent && iAlarmOnOff )
        {
        // For alarm following cehcks and corrections are made:
        // - Alarm is more that 31 days before start
        //    --> Alarm is adjusted to be 30,5 half days before
        // - Alarm is later than start time
        //     --> Default alarm time is set for alarm
        // - Alarm occurs in past
        //    --> For unsaved entries alarm is set off
        //
        TTime alarmTime = AlarmDateTime();
        TTimeIntervalDays alarmBefore = startTime.DaysFrom(alarmTime);
        if ( alarmBefore > KMaxAlarmTime )
            {
            // Adjust alarm time to 30,5 half days earlier
            alarmTime = startTime - KMaxAlarmTime + TTimeIntervalHours(KNoon);
            }

        if ( alarmTime > startTime )
            {
            // Setting alarm to default value
            alarmTime = DefaultAlldayEventAlarmTime( startTime );
            }

        TTime currentTime; currentTime.HomeTime();
        if ( alarmTime < currentTime )
            {
            if ( !iEntry->IsStoredL() )
                {
                // Setting alarm to default value if entry is not stored
                iAlarmOnOff = EFalse;
                }
            }

        iCurrentAlarmTime = alarmTime;
        }

    TTime currentTime;
    currentTime.HomeTime();

    // If relative alarm time is in past
    if ( !iAlldayEvent &&
            ( startTime - iCurrentAlarmTimeOffset ) < currentTime )
        {
        // Setting relative alarm to off
        iCurrentAlarmTimeOffset = KAlarmNotSet;
        *iRelativeAlarmValid = EFalse;
        }

    iCurrentStartTime = startTime;
    iCurrentEndTime = endTime;

    // Start
    SetTimeToEditor( *iStartTime, startTime );
    SetDateToEditor( *iStartDate, startTime );

    // End
    SetTimeToEditor( *iEndTime, endTime );
    SetDateToEditor( *iEndDate, endTime );

    // Alarm
    if(iAlarmTime)
        {
        SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
        }

    if(iAlarmDate)
        {
        SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );
        }


    // if editing certain occurance of recurrent event series
    // the repeat until date is not available.
    if ( iRecurrenceUntilDate )
        {
        // Recurrence until
        SetDateToEditor( *iRecurrenceUntilDate, iCurrentRecurrenceUntil );
        }

    }


// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::ForceEndDateTime
// ---------------------------------------------------------------------------
//
TDateTime CESMRMeetingTimeValidator::ForceEndDateTime()
    {
    FUNC_LOG;
    // In recurrent event cases, when user has modified the end date
    // and presses Done or Save, the end date has to be adjusted to
    // the current start date. The Hours, minutes and seconds should
    // still remain in original value though.

    TDateTime startDateTime = iStartDate->Date().DateTime();

    TDateTime modifiedEndDateTime = iEndDate->Date().DateTime();
    TDateTime endTime = iEndTime->Time().DateTime();

    modifiedEndDateTime.SetDay( startDateTime.Day() );
    modifiedEndDateTime.SetMonth( startDateTime.Month() );
    modifiedEndDateTime.SetYear( startDateTime.Year() );

    modifiedEndDateTime.SetHour( endTime.Hour() );
    modifiedEndDateTime.SetMinute( endTime.Minute() );
    modifiedEndDateTime.SetSecond( endTime.Second() );

    return modifiedEndDateTime;
    }


// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::ForceRepeatUntilTimeL
// ---------------------------------------------------------------------------
//
TTime CESMRMeetingTimeValidator::ForceRecurrenceUntilTime()
    {
    FUNC_LOG;
    // In recurrent event cases, when user has modified the end date
    // and presses Done or Save, the recurrence until time has to be
    // adjusted according to the new start date.

    TTime newUntil(0);

    // Let's find out what the original period of recurrence was
    TTimeIntervalMinutes recurrencePeriod(0);
    TDateTime originalStartTime = iComparativeStartTime;

    // We do not need Hours, Minutes, seconds when setting
    // recurrence until date
    originalStartTime.SetHour(0);
    originalStartTime.SetMinute(0);
    originalStartTime.SetSecond(0);

    TTime originalRecurrenceUntilTime = iRecurrenceUntilDate->Date();

    originalRecurrenceUntilTime.MinutesFrom(
            originalStartTime, recurrencePeriod );

    // New recurrence until time is the new meeting start date plus
    // the recurrence period
    newUntil = iStartDate->Date() + recurrencePeriod;

    return newUntil;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::WriteStartAndEndTimeToEntryL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::WriteStartAndEndTimeToEntryL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalEntry& entry( aEntry.Entry() );
    const CCalEntry& originalEntry( aEntry.OriginalEntry() );

    TTime startTime = StartDateTime();
    TTime endTime = EndDateTime();

    TTime orgStartTime = originalEntry.StartTimeL().TimeLocalL();
    TTime orgEndTime   = originalEntry.EndTimeL().TimeLocalL();

    if ( !aEntry.IsStoredL() || orgStartTime != startTime ||
         orgEndTime != endTime )
        {
        // End time has changed --> Store it to entry
        TCalTime start;
        start.SetTimeLocalL( startTime );

        TCalTime end;
        end.SetTimeLocalL( endTime );

        TDateTime st = start.TimeLocalL().DateTime();
        TDateTime et = end.TimeLocalL().DateTime();

        entry.SetStartAndEndTimeL( start, end );
        }
    else if ( aEntry.IsRecurrentEventL() &&
              MESMRCalEntry::EESMRAllInSeries == aEntry.RecurrenceModRule() )
        {
        // For recurrent events, parent entry's start and end time is stored
        // to entry.

        CCalInstance* instance = aEntry.InstanceL();
        if ( instance )
            {
            CleanupStack::PushL( instance );

            CCalEntry& parent = instance->Entry();
            entry.SetStartAndEndTimeL(
                    parent.StartTimeL(),
                    parent.EndTimeL() );

            CleanupStack::PopAndDestroy( instance );
            instance = NULL;
            }
        }

    }

// ---------------------------------------------------------------------------
// CESMRMeetingTimeValidator::SendFieldChangeEventL
// ---------------------------------------------------------------------------
//
void CESMRMeetingTimeValidator::SendFieldChangeEventL(
        TESMREntryFieldId aFieldId )
    {
    FUNC_LOG;
    if ( iEventQueue )
        {
        CESMRGenericFieldEvent* event =
            CESMRGenericFieldEvent::NewLC( NULL,
                                           MESMRFieldEvent::EESMRFieldChangeEvent );
        TInt fieldId = aFieldId;
        CESMRFieldEventValue* field = CESMRFieldEventValue::NewLC(
                CESMRFieldEventValue::EESMRInteger, &fieldId );
        event->AddParamL( field );
        CleanupStack::Pop( field );
        CESMRFieldEventValue* date = CESMRFieldEventValue::NewLC(
                CESMRFieldEventValue::EESMRTTime, &iCurrentStartTime );
        event->AddParamL( date, ETrue );
        CleanupStack::Pop( date );
        iEventQueue->NotifyEventL( *event );
        CleanupStack::PopAndDestroy( event );
        }
    }

// EOF

