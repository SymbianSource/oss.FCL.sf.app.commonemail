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
#include "cesmrannivtimevalidator.h"
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

// Definition for halfday
const TInt KNoon( 12 );

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

/**
 * Returns the default alarm time for all day event
 * @param allday event start time
 * @return alarm time
 */
TTime DefaultAnniversaryAlarm( TTime& aAnnivStartTime )
    {
    TTime defaultAlarmTime = aAnnivStartTime - TTimeIntervalHours(KNoon);

    if ( defaultAlarmTime < KMinTTime )
        {
        defaultAlarmTime = KMinTTime;
        }
    return defaultAlarmTime;
    }

}


// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::CESMRAnnivTimeValidator()
// ---------------------------------------------------------------------------
//
inline CESMRAnnivTimeValidator::CESMRAnnivTimeValidator()
:   iCurrentAnnivDate( Time::NullTTime() ),
    iCurrentAlarmTime( Time::NullTTime() ),
    iAlarmOnOff( EFalse ),
    iInitialAlarmTime( Time::NullTTime() ),
    iInitialAlarmOnOff( EFalse ),
    iInitialStartDate( Time::NullTTime() )

    {
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::~CESMRAnnivTimeValidator
// ---------------------------------------------------------------------------
//
CESMRAnnivTimeValidator::~CESMRAnnivTimeValidator( )
    {
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::NewL
// ---------------------------------------------------------------------------
//
CESMRAnnivTimeValidator* CESMRAnnivTimeValidator::NewL()
    {
    FUNC_LOG;
    CESMRAnnivTimeValidator* self = new (ELeave) CESMRAnnivTimeValidator();
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::ValidateL
// ---------------------------------------------------------------------------
//
MESMRFieldValidator::TESMRFieldValidatorError
        CESMRAnnivTimeValidator::ValidateL( TBool aCorrectAutomatically )
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
            TTime start = StartTimeL();
            TTime alarm = AlarmTimeL();

            if ( start < alarm )
                {
                TDateTime startDt = start.DateTime();
                TDateTime alarmDt = alarm.DateTime();

                // Alarm needs to be on the same day if it in future
                if ( startDt.Year() != alarmDt.Year() ||
                     startDt.Month() != alarmDt.Month() ||
                     startDt.Day() != alarmDt.Day() )
                     {
                     iCurrentAlarmTime = DefaultAnniversaryAlarm( start );

                     SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
                     SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );

                     error = MESMRFieldValidator::EErrorAlarmLaterThanStart;
                     }
                }
            }
        }
    return error;
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::ReadValuesFromEntryL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::ReadValuesFromEntryL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( MESMRCalEntry::EESMRCalEntryAnniversary == aEntry.Type() )
        {
        __ASSERT_DEBUG( iStartDate, Panic(EESMRAnnivValidatorNullStartDate) );
        __ASSERT_DEBUG( iAlarmTime, Panic(EESMRAnnivValidatorNullAlarmTime) );
        __ASSERT_DEBUG( iAlarmDate, Panic(EESMRAnnivValidatorNullAlarmDate) );

        iEntry = &aEntry;
        CCalEntry& entry( aEntry.Entry() );

        TCalTime startTime = entry.StartTimeL();
        iCurrentAnnivDate = startTime.TimeLocalL();
        SetDateToEditor( *iStartDate, iCurrentAnnivDate );
        iInitialStartDate = iStartDate->Date();

        CCalAlarm* alarm = entry.AlarmL();
        CleanupStack::PushL( alarm );

        if  ( alarm )
            {
            iCurrentAlarmTime = iCurrentAnnivDate - alarm->TimeOffset();
            iAlarmOnOff = ETrue;
            iInitialAlarmOnOff = iAlarmOnOff;

            SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
            SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );
            
            iInitialAlarmTime = iAlarmTime->Time();
            iInitialAlarmDate = iAlarmDate->Date();
            }

        CleanupStack::PopAndDestroy( alarm );
        DrawEditorsDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::StoreValuesToEntryL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::StoreValuesToEntryL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( MESMRCalEntry::EESMRCalEntryAnniversary == aEntry.Type() )
        {
        PreValidateEditorContent();
        CCalEntry& entry( aEntry.Entry() );
        TDateTime annivEditorTime = StartTimeL();
        
        // If entry has been modified, we will write new values to entry.
        // Otherwise entry will not be modified and thus saved 
        // (ex. new entry and press only Done )
        if( IsModifiedL( aEntry ) )
            {
            TCalTime annivTime;
    
            // The default mode for anniversary is EFloating,
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
                        annivTime.SetTimeUtcL( annivEditorTime );
                        break;
                        }
                    case TCalTime::EFixedTimeZone:
                        {
                        annivTime.SetTimeLocalL( annivEditorTime );
                        break;
                        }
                    case TCalTime::EFloating:
                    default:
                        {
                        annivTime.SetTimeLocalFloatingL( annivEditorTime );
                        }
                    }
                }
            else
                {
                annivTime.SetTimeLocalFloatingL( annivEditorTime );
                }
    
            entry.SetStartAndEndTimeL( annivTime, annivTime );


            // Setting alarm only if entry needs to be saved
            if ( iAlarmOnOff )
                {
                TTimeIntervalMinutes diff;
                
                TTime alarm = AlarmTimeL();
                TTime editorTime = annivEditorTime;
                editorTime.MinutesFrom( alarm, diff );
    
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
// CESMRAnnivTimeValidator::SetStartTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::SetStartTimeFieldL(
        CEikTimeEditor& /*aStartTime*/ )
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::SetEndTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::SetEndTimeFieldL(
        CEikTimeEditor& /*aEndTime*/ )
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::SetStartDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::SetStartDateFieldL(
        CEikDateEditor& aStartDate )
    {
    FUNC_LOG;
    iStartDate = &aStartDate;
    
    if ( Time::NullTTime() != iCurrentAnnivDate )
        {
        SetDateToEditor( *iStartDate, iCurrentAnnivDate );
        }    
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::SetEndDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::SetEndDateFieldL(
        CEikDateEditor& /*aEndDate*/ )
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::SetAlarmTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::SetAlarmTimeFieldL(
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
// CESMRAnnivTimeValidator::SetAlarmDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::SetAlarmDateFieldL(
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
// CESMRAnnivTimeValidator::SetRecurrenceUntilDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::SetRecurrenceUntilDateFieldL(
            CEikDateEditor& /*aRecurrenceUntil*/ )
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::SetAbsoluteAlarmOnOffFieldL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::SetAbsoluteAlarmOnOffFieldL( 
        MMRAbsoluteAlarmController& /*aAbsoluteAlarmController*/ )
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::StartTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::StartTimeChangedL()
    {
    FUNC_LOG;
    StartDateChandedL();
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::EndTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::EndTimeChangedL()
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::StartDateChandedL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::StartDateChandedL()
    {
    FUNC_LOG;
    PreValidateEditorContent();

    TTime start = StartTimeL();
    TTimeIntervalDays diff =  start.DaysFrom( iCurrentAnnivDate );
    iCurrentAnnivDate = start;
    SetDateToEditor( *iStartDate, iCurrentAnnivDate );

    if ( iAlarmOnOff )
        {
        TTime alarm = AlarmTimeL();
        alarm += diff;

        iCurrentAlarmTime = alarm;

        SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
        SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );
        }

    DrawEditorsDeferred();    
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::EndDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::EndDateChangedL()
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::AlarmTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::AlarmTimeChangedL()
    {
    FUNC_LOG;
    AlarmDateChangedL();
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::AlarmDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::AlarmDateChangedL()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    PreValidateEditorContent();

    TTime start = StartTimeL();
    TTime alarm = AlarmTimeL();

    if ( alarm > start )
        {
        TDateTime startDt = start.DateTime();
        TDateTime alarmDt = alarm.DateTime();

        // Alarm needs to be on the same day if it in future
        if ( startDt.Year() != alarmDt.Year() ||
             startDt.Month() != alarmDt.Month() ||
             startDt.Day() != alarmDt.Day() )
             {
             err = KErrArgument;
             }
        }

    if ( KErrNone == err )
        {
        iCurrentAlarmTime = alarm;
        }

    SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
    SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );

    DrawEditorsDeferred();
    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::RelativeAlarmChangedL()
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::RelativeAlarmChangedL(
        TTimeIntervalMinutes /*aCurrentAlarmTimeOffset*/,
        TBool /*aHandleAlarmChange*/,
        TBool& /*aRelativeAlarmValid*/ )
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::SetAllDayEventL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::SetAllDayEventL(
        TBool /*aAlldayEvent*/ )
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::SetAlarmOnOffL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::SetAlarmOnOffL(
        TBool aAlarmOn )
    {
    FUNC_LOG;
    iAlarmOnOff = aAlarmOn;

    if ( iAlarmOnOff )
        {
        if ( iCurrentAlarmTime == Time::NullTTime() ||
             iCurrentAlarmTime > iCurrentAnnivDate  )
            {
            iCurrentAlarmTime = DefaultAnniversaryAlarm( iCurrentAnnivDate );
            }

        SetTimeToEditor( *iAlarmTime, iCurrentAlarmTime );
        SetDateToEditor( *iAlarmDate, iCurrentAlarmTime );

        DrawEditorsDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::RecurrenceChangedL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::RecurrenceChangedL(
        TESMRRecurrenceValue /*aRecurrence*/ )
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::RecurrenceEndDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::RecurrenceEndDateChangedL()
    {
    FUNC_LOG;
    // No implementation for anniversary
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::IsRelativeAlarmValid
// ---------------------------------------------------------------------------
//
TBool CESMRAnnivTimeValidator::IsRelativeAlarmValid(
		TTimeIntervalMinutes /*aAlarmTimeOffset*/ )
    {
    FUNC_LOG;
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::PreValidateEditorContent
// ---------------------------------------------------------------------------
//
TInt CESMRAnnivTimeValidator::PreValidateEditorContent()
    {
    FUNC_LOG;
    TRAPD( err, PreValidateEditorContentL() );
    return err;
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::PreValidateEditorContentL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::PreValidateEditorContentL()
    {
    FUNC_LOG;
    if ( iStartDate && iStartDate->IsVisible() )
        {
        iStartDate->PrepareForFocusLossL();
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
// CESMRAnnivTimeValidator::DrawEditorsDeferred
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::DrawEditorsDeferred()
    {
    FUNC_LOG;
    if ( iStartDate && iStartDate->IsVisible() )
        {
        iStartDate->DrawDeferred();
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
// CESMRAnnivTimeValidator::StartTimeL
// ---------------------------------------------------------------------------
//
TDateTime CESMRAnnivTimeValidator::StartTimeL()
    {
    FUNC_LOG;
    return iStartDate->Date().DateTime();
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::AlarmTimeL
// ---------------------------------------------------------------------------
//
TDateTime CESMRAnnivTimeValidator::AlarmTimeL()
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
// CESMRAnnivTimeValidator::ForceValuesL
// ---------------------------------------------------------------------------
//
void CESMRAnnivTimeValidator::ForceValuesL()
    {
    FUNC_LOG;
    if ( iAlarmOnOff )
        {
        TTime start = StartTimeL();
        TTime alarm = AlarmTimeL();

        // For alarm following cehcks and corrections are made:
        // - Alarm is more that 31 days before start
        //    --> Alarm is adjusted to be 30,5 half days before
        // - Alarm is later than start time
        //     --> Default alarm time is set for alarm
        // - Alarm occurs in past
        //    --> For unsaved entries alarm is set off
        //
        TTimeIntervalDays alarmBefore = start.DaysFrom(alarm);
        if ( alarmBefore > KMaxAlarmTime )
            {
            // Adjust alarm time to 30,5 half days earlier
            alarm = start - KMaxAlarmTime + TTimeIntervalHours(KNoon);
            }

        if ( alarm > start )
            {
            // Setting alarm to default value
            alarm = DefaultAnniversaryAlarm( start );
            }

        TTime currentTime; currentTime.HomeTime();
        if ( alarm < currentTime )
            {
            if ( !iEntry->IsStoredL() )
                {
                // Setting alarm to default value if entry is not stored
                iAlarmOnOff = EFalse;
                }
            }

        SetTimeToEditor( *iAlarmTime, alarm );
        SetDateToEditor( *iAlarmDate, alarm );

        iCurrentAlarmTime = alarm;
        }
    }

// ---------------------------------------------------------------------------
// CESMRAnnivTimeValidator::IsModifiedL
// ---------------------------------------------------------------------------
//
TBool CESMRAnnivTimeValidator::IsModifiedL( MESMRCalEntry& aEntry )
    {
    // Checks if any of the editor fields have changed from the original /
    // initial values
    TBool isEdited( EFalse );
    
    TBool isSame = aEntry.Entry().CompareL( aEntry.OriginalEntry() );
        
    if( !isSame ||
            iInitialAlarmOnOff != iAlarmOnOff ||
                iStartDate->Date() != iInitialStartDate )
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
