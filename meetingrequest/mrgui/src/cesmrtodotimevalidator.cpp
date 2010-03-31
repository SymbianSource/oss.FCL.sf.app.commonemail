/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <calalarm.h>
#include <e32std.h>

#include "cesmrtodotimevalidator.h"
#include "cesmrglobalnote.h"
#include "mesmrfieldstorage.h"
#include "mesmrcalentry.h"

// Unnamed namespace for local definitions
namespace {

// Definition fom max possible time
#define KMaxTTime (TTime(TDateTime(2100, EDecember, 31-2, 23, 59, 59, 999999)))

// Definition fom min possible time
#define KMinTTime (TTime(TDateTime(1900, EJanuary, 2-1, 0, 0, 0, 0)))

// Definition for max alarm duration
#define KMaxAlarmTime TTimeIntervalDays(31)

// Definition for zero
const TInt KZero( 0 );

// Definition for half day
const TInt KNoon( 12 );

// Definition for max hours in day
const TInt KMaxHours(23);

// Definition for max minutes in day
const TInt KMaxMinutes(59);

// Definition for max seconsds in day
const TInt KMaxSeconds(00);

#ifdef _DEBUG

// Literal for panics
_LIT( KESMRAnnivTimeValidator, "ESMRAnnivTimeValidator" );

/** Panic code definitions */
enum TESMRAnnivTimeValidator
    {
    /** Start date field not set */
    EESMRAnnivValidatorNullStartDate,
    /** Start date field not set */
    EESMRAnnivValidatorNullAlarmTime,
    /** Start date field not set */
    EESMRAnnivValidatorNullAlarmDate,
    /** Relative alarm called for non-relative alarm event */
    EESMRAnnivValidatorRelativeAlarm

    };

/**
 * Raises system panic.
 * @param aPanic Panic code
 * @see TESMRRecurrentEditValidatorPanic
 */
void Panic( TESMRAnnivTimeValidator aPanic )
    {
    User::Panic( KESMRAnnivTimeValidator, aPanic );
    }

#endif // _DEBUG

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

    aEditor.SetTime( aTime );
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
    aEditor.SetDate( aDate );
    }

/**
 * Returns the default alarm time for to-do
 * @param allday event start time
 * @return alarm time
 */
TTime DefaultToDoAlarmL( TTime& aTodoDueDate )
    {
    TDateTime alarmTime = aTodoDueDate.DateTime();
    alarmTime.SetHour( KNoon );
    alarmTime.SetMinute( KZero );
    alarmTime.SetSecond( KZero );

    TTime defaultAlarmTime = alarmTime;

    if ( defaultAlarmTime < KMinTTime )
        {
        defaultAlarmTime = KMinTTime;
        }

    return defaultAlarmTime;
    }

}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::CESMRAnnivTimeValidator()
// ---------------------------------------------------------------------------
//
inline CESMRTodoTimeValidator::CESMRTodoTimeValidator()
:   iCurrentDueDate( Time::NullTTime() ),
    iCurrentAlarmTime( Time::NullTTime() ),
    iAlarmOnOff( EFalse ),
    iInitialAlarmTime( Time::NullTTime() ),
    iInitialAlarmOnOff( EFalse ),
    iInitialDueDate( Time::NullTTime() )
    
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::~CESMRTodoTimeValidator
// ---------------------------------------------------------------------------
//
CESMRTodoTimeValidator::~CESMRTodoTimeValidator( )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::NewL
// ---------------------------------------------------------------------------
//
CESMRTodoTimeValidator* CESMRTodoTimeValidator::NewL()
    {
    FUNC_LOG;
    CESMRTodoTimeValidator* self = new (ELeave) CESMRTodoTimeValidator();
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::ValidateL
// ---------------------------------------------------------------------------
//
MESMRFieldValidator::TESMRFieldValidatorError
        CESMRTodoTimeValidator::ValidateL( TBool aCorrectAutomatically )
    {
    FUNC_LOG;
    const MESMRFieldValidator::TESMRFieldValidatorError KErrorNone(
            MESMRFieldValidator::EErrorNone );

    MESMRFieldValidator::TESMRFieldValidatorError error( KErrorNone );

    if ( aCorrectAutomatically )
        {
        ForceValuesL();
        }
    else
        {
        if ( iAlarmOnOff )
            {
            TTime currentTime; currentTime.HomeTime();

            TTime start = DueDateTimeL();
            TTime alarm = AlarmDateTimeL();

            if ( currentTime > alarm )
                {
                error = MESMRFieldValidator::EErrorAlarmInPast;
                }

            if ( start < alarm )
                {
                SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
                SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );

                error = MESMRFieldValidator::EErrorAlarmLaterThanStart;
                }

			if ( alarm < start )
				{
				TTimeIntervalDays alarmBefore = start.DaysFrom(alarm);
				if ( alarmBefore > KMaxAlarmTime )
					{
					error =  MESMRFieldValidator::EErrorAlarmTooMuchInPast;
					}
				}
                
            }
        }
    return error;
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::ReadValuesFromEntryL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::ReadValuesFromEntryL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( MESMRCalEntry::EESMRCalEntryTodo == aEntry.Type() )
        {
        __ASSERT_DEBUG( iDueDate, Panic(EESMRAnnivValidatorNullStartDate) );
        __ASSERT_DEBUG( iAlarmTime, Panic(EESMRAnnivValidatorNullAlarmTime) );
        __ASSERT_DEBUG( iAlarmDate, Panic(EESMRAnnivValidatorNullAlarmDate) );

        iEntry = &aEntry;
        CCalEntry& entry( aEntry.Entry() );

        TCalTime dueDate = entry.EndTimeL();
        iCurrentDueDate = dueDate.TimeLocalL();
        SetDateToEditor( *iDueDate, iCurrentDueDate );
        
        iInitialDueDate = iDueDate->Date();

        TDateTime dueDt = iCurrentDueDate.DateTime();

        CCalAlarm* alarm = entry.AlarmL();
        CleanupStack::PushL( alarm );

        if  ( alarm )
            {
            iCurrentAlarmTime = iCurrentDueDate - alarm->TimeOffset();
            iAlarmOnOff = ETrue;
            iInitialAlarmOnOff = iAlarmOnOff;

            SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
            SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );
            
            iInitialAlarmTime = iAlarmTime->Time();
            iInitialAlarmDate = iAlarmDate->Date();
            }

        CleanupStack::PopAndDestroy( alarm );
        alarm = NULL;

        DrawEditorsDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::StoreValuesToEntryL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::StoreValuesToEntryL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( MESMRCalEntry::EESMRCalEntryTodo == aEntry.Type() )
        {
        PreValidateEditorContent();
        CCalEntry& entry( aEntry.Entry() );

        // If entry has been modified, we will write new values to entry.
        // Otherwise entry will not be modified and thus saved 
        // (ex. new entry and press only Done )
        if( IsModifiedL( aEntry ) )
            {
            TTime toEditorDueDate = DueDateTimeL();
            TCalTime todoDueDate;
    
            // The default mode for To-do is EFloating,
            // But some 3rd party application might have saved a different type
            // for one reason or another. In that case we are using
            // the existing value.
            if ( aEntry.IsStoredL() )
                {
                TCalTime::TTimeMode timeMode =
                                aEntry.Entry().StartTimeL().TimeMode();
    
                switch ( timeMode )
                    {
                    case TCalTime::EFixedUtc:
                        {
                        todoDueDate.SetTimeUtcL( toEditorDueDate );
                        break;
                        }
                    case TCalTime::EFixedTimeZone:
                        {
                        todoDueDate.SetTimeLocalL( toEditorDueDate );
                        break;
                        }
                    case TCalTime::EFloating: // Fall through
                    default:
                        {
                        todoDueDate.SetTimeLocalFloatingL( toEditorDueDate );
                        break;
                        }
                    }
                }
            else
                {
                todoDueDate.SetTimeLocalFloatingL( toEditorDueDate );
                }
    
            entry.SetStartAndEndTimeL( todoDueDate, todoDueDate );
    
            if ( iAlarmOnOff )
                {
                TTimeIntervalMinutes diff;
    
                TTime alarm = AlarmDateTimeL();
                toEditorDueDate.MinutesFrom( alarm, diff );
    
                CCalAlarm* alarmObject = entry.AlarmL();
                if ( !alarmObject )
                    {
                    alarmObject = CCalAlarm::NewL();
                    }
                CleanupStack::PushL( alarmObject );
                alarmObject->SetTimeOffset( diff );
                entry.SetAlarmL( alarmObject );
                CleanupStack::PopAndDestroy( alarmObject );
                alarmObject = NULL;
                }
            else
                {
                entry.SetAlarmL( NULL );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::SetStartTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::SetStartTimeFieldL(
        CEikTimeEditor& /*aStartTime*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::SetEndTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::SetEndTimeFieldL(
        CEikTimeEditor& /*aEndTime*/ )
    {
    FUNC_LOG;
    // No implementation for to-do
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::SetStartDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::SetStartDateFieldL(
        CEikDateEditor& /*aStartDate*/ )
    {
    FUNC_LOG;
    // No implementation for to-do
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::SetEndDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::SetEndDateFieldL(
        CEikDateEditor& aEndDate )
    {
    FUNC_LOG;
    iDueDate = &aEndDate;
    
    if ( Time::NullTTime() != iCurrentDueDate )
        {
        SetDateToEditor( *iDueDate, iCurrentDueDate );
        }  
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::SetAlarmTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::SetAlarmTimeFieldL(
        CEikTimeEditor& aAlarmTime )
    {
    FUNC_LOG;
    iAlarmTime = &aAlarmTime;
    
    if ( iAlarmOnOff )
        {
        SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
        }
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::SetAlarmDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::SetAlarmDateFieldL(
        CEikDateEditor& aAlarmDate )
    {
    FUNC_LOG;
    iAlarmDate = &aAlarmDate;
    
    if ( iAlarmOnOff )
        {
        SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );
        }
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::SetRecurrenceUntilDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::SetRecurrenceUntilDateFieldL(
            CEikDateEditor& /*aRecurrenceUntil*/ )
    {
    FUNC_LOG;
    // No implementation for to-do
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::SetAbsoluteAlarmOnOffFieldL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::SetAbsoluteAlarmOnOffFieldL( 
        MMRAbsoluteAlarmController& /*aAbsoluteAlarmController*/ )
    {
    FUNC_LOG;
    // No implementation for to-do
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::StartTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::StartTimeChangedL()
    {
    FUNC_LOG;
    StartDateChandedL();
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::EndTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::EndTimeChangedL()
    {
    FUNC_LOG;
    // No implementation for to-do
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::StartDateChandedL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::StartDateChandedL()
    {
    FUNC_LOG;
    // No implementation for to-do
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::EndDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::EndDateChangedL()
    {
    FUNC_LOG;
    PreValidateEditorContent();

    TTime due = DueDateTimeL();
    TTimeIntervalDays diff =  due.DaysFrom( iCurrentDueDate );
    iCurrentDueDate = due;
    SetDateToEditor( *iDueDate, iCurrentDueDate );

    if ( iAlarmOnOff )
        {
        TTime alarm = AlarmDateTimeL();
        alarm += diff;

        iCurrentAlarmTime = alarm;

        SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
        SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );
        }

    DrawEditorsDeferred();
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::AlarmTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::AlarmTimeChangedL()
    {
    FUNC_LOG;
    AlarmDateChangedL();
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::AlarmDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::AlarmDateChangedL()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    PreValidateEditorContent();

    TTime due = DueDateTimeL();
    TTime alarm = AlarmDateTimeL();

    if ( alarm > due )
        {
        err = KErrArgument;
        }
    else
        {
        iCurrentAlarmTime = alarm;
        }

    SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
    SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );

    DrawEditorsDeferred();

    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::RelativeAlarmChangedL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::RelativeAlarmChangedL(
        TTimeIntervalMinutes /*aCurrentAlarmTimeOffset*/,
        TBool /*aHandleAlarmChange*/,
        TBool& /*aRelativeAlarmValid*/ )
    {
    FUNC_LOG;
    // No implementation for To-do
    }


// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::SetAllDayEventL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::SetAllDayEventL(
        TBool /*aAlldayEvent*/ )
    {
    FUNC_LOG;
    // No implementation for to-do
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::SetAlarmOnOffL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::SetAlarmOnOffL(
        TBool aAlarmOn )
    {
    FUNC_LOG;
    iAlarmOnOff = aAlarmOn;
    
    if ( iAlarmOnOff )
        {
        if ( iCurrentAlarmTime == Time::NullTTime() ||
             iCurrentAlarmTime > iCurrentDueDate )
            {
            iCurrentAlarmTime = DefaultToDoAlarmL( iCurrentDueDate );
            }

        SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
        SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );

        DrawEditorsDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::RecurrenceChangedL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::RecurrenceChangedL(
        TESMRRecurrenceValue /*aRecurrence*/ )
    {
    FUNC_LOG;
    // No implementation for to-do
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::RecurrenceEndDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::RecurrenceEndDateChangedL()
    {
    FUNC_LOG;
    // No implementation for to-do
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::IsRelativeAlarmValid
// ---------------------------------------------------------------------------
//
TBool CESMRTodoTimeValidator::IsRelativeAlarmValid(
		TTimeIntervalMinutes /*aAlarmTimeOffset*/ )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( ETrue, Panic( EESMRAnnivValidatorRelativeAlarm ) );
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::PreValidateEditorContent
// ---------------------------------------------------------------------------
//
TInt CESMRTodoTimeValidator::PreValidateEditorContent()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TRAP( err, PreValidateEditorContentL() );
    return err;
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::PreValidateEditorContentL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::PreValidateEditorContentL()
    {
    FUNC_LOG;
    if ( iDueDate && iDueDate->IsVisible() )
        {
        iDueDate->PrepareForFocusLossL();
        }

    if ( iAlarmOnOff && iAlarmTime && iAlarmTime->IsVisible() )
        {
        iAlarmTime->PrepareForFocusLossL();
        }

    if ( iAlarmOnOff && iAlarmDate && iAlarmDate->IsVisible() )
        {
        iAlarmDate->PrepareForFocusLossL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::DrawEditorsDeferred
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::DrawEditorsDeferred()
    {
    FUNC_LOG;
    if ( iDueDate && iDueDate->IsVisible() )
        {
        iDueDate->DrawDeferred();
        }

    if ( iAlarmTime && iAlarmTime->IsVisible() )
        {
        iAlarmTime->DrawDeferred();
        }

    if ( iAlarmDate && iAlarmDate->IsVisible() )
        {
        iAlarmDate->DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::DueStartDateL
// ---------------------------------------------------------------------------
//
TDateTime CESMRTodoTimeValidator::DueStartDateL()
    {
    FUNC_LOG;
    return iDueDate->Date().DateTime();
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::DueDateTimeL
// ---------------------------------------------------------------------------
//
TDateTime CESMRTodoTimeValidator::DueDateTimeL()
    {
    FUNC_LOG;
    TDateTime todoDueDate = iDueDate->Date().DateTime();

    // We want to set 23 h 59 min 00 sec for new entries
    if( !iEntry->IsStoredL() )
        {
        todoDueDate.SetHour( KMaxHours );
        todoDueDate.SetMinute( KMaxMinutes );
        todoDueDate.SetSecond( KMaxSeconds );
        }
    // But for existing entries, we want to use existing values to avoid 
    // problems when saving the values to entry
    else
   	    {
        todoDueDate.SetHour( 
            iEntry->Entry().EndTimeL().TimeLocalL().DateTime().Hour() );
        todoDueDate.SetMinute( 
            iEntry->Entry().EndTimeL().TimeLocalL().DateTime().Minute() );
        todoDueDate.SetSecond( 
            iEntry->Entry().EndTimeL().TimeLocalL().DateTime().Second() );
        }

    return todoDueDate;
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::AlarmDateTimeL
// ---------------------------------------------------------------------------
//
TDateTime CESMRTodoTimeValidator::AlarmDateTimeL()
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
// CESMRTodoTimeValidator::ForceValuesL
// ---------------------------------------------------------------------------
//
void CESMRTodoTimeValidator::ForceValuesL()
    {
    FUNC_LOG;
    if ( iAlarmOnOff )
        {
        TTime dueDate = DueDateTimeL();
        TTime alarm = AlarmDateTimeL();

        // For alarm following cehcks and corrections are made:
        // - Alarm is more that 31 days before start
        //    --> Alarm is adjusted to be 30,5 half days before
        // - Alarm is later than start time
        //     --> Default alarm time is set for alarm
        // - Alarm occurs in past
        //    --> For unsaved entries alarm is set off
        //
        TTimeIntervalDays alarmBefore = dueDate.DaysFrom(alarm);
        if ( alarmBefore > KMaxAlarmTime )
            {
            // Adjust alarm time to 30,5 half days earlier
            alarm = dueDate - KMaxAlarmTime + TTimeIntervalHours(KNoon);
            TDateTime alarmTime = alarm.DateTime();
            alarmTime.SetHour( KNoon );
            alarmTime.SetMinute( KZero );
            alarmTime.SetSecond( KZero );
            alarm = alarmTime;
            }

        if ( alarm > dueDate )
            {
            // Setting alarm to default value
            alarm = DefaultToDoAlarmL( dueDate );
            }

        TTime currentTime; 
        currentTime.HomeTime();
        if ( alarm < currentTime )
            {
            if ( !iEntry->IsStoredL() )
                {
                // Setting alarm to default value if entry is not stored
                iAlarmOnOff = EFalse;
                }
            // For stored entries the alarm can remain as it is if it 
            // not adjusted by two earlier checks.
            }

        SetTimeToEditor( *iAlarmTime, alarm );
        SetDateToEditor( *iAlarmDate, alarm );

        iCurrentAlarmTime = alarm;
        }
    }

// ---------------------------------------------------------------------------
// CESMRTodoTimeValidator::IsModifiedL
// ---------------------------------------------------------------------------
//
TBool CESMRTodoTimeValidator::IsModifiedL( MESMRCalEntry& aEntry )
    {
    // Checks if any of the editor fields have changed from the original /
    // initial values
    TBool isEdited( EFalse );
    
    TBool isSame = aEntry.Entry().CompareL( aEntry.OriginalEntry() );
        
    if( !isSame ||
            iInitialAlarmOnOff != iAlarmOnOff ||
                iDueDate->Date() != iInitialDueDate )
        {
        isEdited = ETrue;
        }

    // If alarm is on, we will check alarm time and date also for changes
    if( iAlarmOnOff )
        {
        if( iAlarmTime->Time() != iInitialAlarmTime ||
                iAlarmDate->Date() != iInitialAlarmDate )
            {
            isEdited = ETrue;
            }
        }

    return isEdited;
    }

// EOF
