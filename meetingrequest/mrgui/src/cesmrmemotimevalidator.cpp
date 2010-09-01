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
#include <e32std.h>
#include "cesmrmemotimevalidator.h"
#include "cesmrglobalnote.h"
#include "mesmrfieldstorage.h"
#include "mesmrcalentry.h"

// Unnamed namespace for local definitions
namespace {

const TInt KMaxHours(23);
const TInt KMaxMinutes(59);
const TInt KMaxSeconds(59);

// Definition fom max possible time
#define KMaxTTime (TTime(TDateTime(2100, EDecember, 31-2, 23, 59, 59, 999999)))

// Definition fom min possible time
#define KMinTTime (TTime(TDateTime(1900, EJanuary, 2-1, 0, 0, 0, 0)))

#ifdef _DEBUG

// Literal for panics
_LIT( KESMRMemoTimeValidator, "ESMRMemoTimeValidator" );

/** Panic code definitions */
enum TESMRMemoTimeValidator
    {
    /** Start date field not set */
    EESMRMemoValidatorNullStartDate,
    /** Stop date field not set */
    EESMRMemoValidatorNullStopDate,
    /** Relative alarm called for non-relative alarm event */
    EESMRMemoValidatorRelativeAlarm
    };

/**
 * Raises system panic.
 * @param aPanic Panic code
 * @see TESMRRecurrentEditValidatorPanic
 */
void Panic( TESMRMemoTimeValidator aPanic )
    {
    User::Panic( KESMRMemoTimeValidator, aPanic );
    }

#endif // _DEBUG

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

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::~CESMRMemoTimeValidator
// ---------------------------------------------------------------------------
//
inline CESMRMemoTimeValidator::CESMRMemoTimeValidator( )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::~CESMRMemoTimeValidator
// ---------------------------------------------------------------------------
//
CESMRMemoTimeValidator::~CESMRMemoTimeValidator( )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::NewL
// ---------------------------------------------------------------------------
//
CESMRMemoTimeValidator* CESMRMemoTimeValidator::NewL( )
    {
    FUNC_LOG;
    CESMRMemoTimeValidator* self = new (ELeave) CESMRMemoTimeValidator();
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::ValidateL
// ---------------------------------------------------------------------------
//
MESMRFieldValidator::TESMRFieldValidatorError
        CESMRMemoTimeValidator::ValidateL( TBool aCorrectAutomatically )
    {
    FUNC_LOG;
    TTime start = StartDateTimeL();
    TTime end = EndDateTimeL();

    PreValidateEditorContent();

    if ( aCorrectAutomatically )
        {
        const CCalEntry& orginalEntry( iEntry->OriginalEntry() );
        if ( end < start )
            {
            if ( iEntry->IsStoredL() )
                {
                start = orginalEntry.StartTimeL().TimeLocalL();
                end = orginalEntry.EndTimeL().TimeLocalL();
                }
            else
                {
                end = start;
                }
            }

        iCurrentStartTime = start;
        iCurrentEndTime = end;

        SetDateToEditor( *iStartDate, iCurrentStartTime );
        SetDateToEditor( *iEndDate, iCurrentEndTime );
        }
    else
        {
        if ( end < start )
            {
            return MESMRFieldValidator::EErrorEndEarlierThanStart;
            }
        }

    return MESMRFieldValidator::EErrorNone;
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::ReadValuesFromEntryL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::ReadValuesFromEntryL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( MESMRCalEntry::EESMRCalEntryMemo == aEntry.Type() )
        {
        __ASSERT_DEBUG( iStartDate, Panic( EESMRMemoValidatorNullStartDate) );
        __ASSERT_DEBUG( iEndDate, Panic( EESMRMemoValidatorNullStopDate) );

        iEntry = &aEntry;
        CCalEntry& entry( iEntry->Entry() );

        iCurrentStartTime = entry.StartTimeL().TimeLocalL();
        
        if ( aEntry.IsAllDayEventL() )
            {
            // if the event is allday event, previous day for end date will be
            // shown in UI. See Outlook for reference...
            iCurrentEndTime = iCurrentStartTime;
            }
        else
            {
            iCurrentEndTime = entry.EndTimeL().TimeLocalL();            
            }

        SetDateToEditor( *iStartDate, iCurrentStartTime );
        SetDateToEditor( *iEndDate, iCurrentEndTime );

        DrawEditorsDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::StoreValuesToEntryL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::StoreValuesToEntryL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( MESMRCalEntry::EESMRCalEntryMemo == aEntry.Type() )
        {
        PreValidateEditorContent();

        CCalEntry& entry( aEntry.Entry() );
        const CCalEntry& originalEntry( aEntry.OriginalEntry() );
        
        TTime start = StartDateTimeL();
        TTime end = EndDateTimeL();

        if ( end < start )
            {
            User::Leave( KErrArgument );
            }

        TTime orgStartTime = originalEntry.StartTimeL().TimeLocalL();
        TTime orgEndTime   = originalEntry.EndTimeL().TimeLocalL();

        if( orgStartTime != start || orgEndTime != end )
            {
            TCalTime startTime;
            TCalTime endTime;
    
            // The default mode for memo is EFloating,
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
                        startTime.SetTimeUtcL( start );
                        endTime.SetTimeUtcL( end );
                        break;
                        }
                    case TCalTime::EFixedTimeZone:
                        {
                        startTime.SetTimeLocalL( start );
                        endTime.SetTimeLocalL( end );
                        break;
                        }
                    case TCalTime::EFloating: // Fall through
                    default:
                        {
                        startTime.SetTimeLocalFloatingL( start );
                        endTime.SetTimeLocalFloatingL( end );
                        break;
                        }
                    }
                }
            else
                {
                startTime.SetTimeLocalFloatingL( start );
                endTime.SetTimeLocalFloatingL( end );
                }
    
            entry.SetStartAndEndTimeL( startTime, endTime );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::SetStartTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::SetStartTimeFieldL(
        CEikTimeEditor& /*aStartTime*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::SetEndTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::SetEndTimeFieldL(
        CEikTimeEditor& /*aEndTime*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::SetStartDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::SetStartDateFieldL(
        CEikDateEditor& aStartDate )
    {
    FUNC_LOG;
    iStartDate = &aStartDate;
    
    if ( Time::NullTTime() != iCurrentStartTime )
        {
        SetDateToEditor( *iStartDate, iCurrentStartTime );
        }    
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::SetEndDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::SetEndDateFieldL(
        CEikDateEditor& aEndDate )
    {
    FUNC_LOG;
    iEndDate = &aEndDate;
    
    if ( Time::NullTTime() != iCurrentEndTime )
        {
        SetDateToEditor( *iEndDate, iCurrentEndTime );
        } 
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::SetAlarmTimeFieldL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::SetAlarmTimeFieldL(
        CEikTimeEditor& /*aAlarmTime*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::SetAlarmDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::SetAlarmDateFieldL(
        CEikDateEditor& /*aAlarmDate*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::SetRecurrenceUntilDateFieldL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::SetRecurrenceUntilDateFieldL(
            CEikDateEditor& /*aRecurrenceUntil*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::SetAbsoluteAlarmOnOffFieldL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::SetAbsoluteAlarmOnOffFieldL( 
        MMRAbsoluteAlarmController& /*aAbsoluteAlarmController*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::StartTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::StartTimeChangedL()
    {
    FUNC_LOG;
    StartDateChandedL();
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::EndTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::EndTimeChangedL()
    {
    FUNC_LOG;
    EndDateChangedL();
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::StartDateChandedL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::StartDateChandedL()
    {
    FUNC_LOG;
    PreValidateEditorContent();

    TTime start = StartDateTimeL();
    TTime end = EndDateTimeL();

    TTimeIntervalMinutes diff;
    start.MinutesFrom( iCurrentStartTime, diff );

    end += diff;

    iCurrentStartTime = start;
    iCurrentEndTime = end;

    SetDateToEditor( *iStartDate, iCurrentStartTime );
    SetDateToEditor( *iEndDate, iCurrentEndTime );

    DrawEditorsDeferred();
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::EndDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::EndDateChangedL()
    {
    FUNC_LOG;
    TInt err( KErrNone );

    PreValidateEditorContent();

    TTime start = StartDateTimeL();
    TTime end = EndDateTimeL();

    if ( end < start )
        {
        err = KErrArgument;
        }
    else
        {
        iCurrentEndTime = end;
        }

    SetDateToEditor( *iStartDate, iCurrentStartTime );
    SetDateToEditor( *iEndDate, iCurrentEndTime );

    DrawEditorsDeferred();

    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::AlarmTimeChangedL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::AlarmTimeChangedL()
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::AlarmDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::AlarmDateChangedL()
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::RelativeAlarmChangedL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::RelativeAlarmChangedL(
        TTimeIntervalMinutes /*aCurrentAlarmTimeOffset*/,
        TBool /*aHandleAlarmChange*/,
        TBool& /*aRelativeAlarmValid*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::SetAllDayEventL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::SetAllDayEventL(
        TBool /*aAlldayEvent*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::SetAlarmOnOffL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::SetAlarmOnOffL(
        TBool /*aAlarmOn*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::RecurrenceChangedL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::RecurrenceChangedL(
        TESMRRecurrenceValue /*aRecurrence*/ )
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::RecurrenceEndDateChangedL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::RecurrenceEndDateChangedL()
    {
    FUNC_LOG;
    // No implementation for memo
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::IsRelativeAlarmValid
// ---------------------------------------------------------------------------
//
TBool CESMRMemoTimeValidator::IsRelativeAlarmValid(
		TTimeIntervalMinutes /*aAlarmTimeOffset*/ )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( ETrue, Panic( EESMRMemoValidatorRelativeAlarm ) );
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::PreValidateEditorContent
// ---------------------------------------------------------------------------
//
TInt CESMRMemoTimeValidator::PreValidateEditorContent()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TRAP( err, PreValidateEditorContentL() );
    return err;
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::PreValidateEditorContentL
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::PreValidateEditorContentL()
    {
    FUNC_LOG;
    if ( iStartDate && iStartDate->IsVisible() )
        {
        iStartDate->PrepareForFocusLossL();
        }

    if ( iEndDate && iEndDate->IsVisible() )
        {
        iEndDate->PrepareForFocusLossL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::DrawEditorsDeferred
// ---------------------------------------------------------------------------
//
void CESMRMemoTimeValidator::DrawEditorsDeferred()
    {
    FUNC_LOG;
    if ( iStartDate && iStartDate->IsVisible() )
        {
        iStartDate->DrawDeferred();
        }

    if ( iEndDate && iEndDate->IsVisible() )
        {
        iEndDate->DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::StartDateTimeL
// ---------------------------------------------------------------------------
//
TDateTime CESMRMemoTimeValidator::StartDateTimeL()
    {
    FUNC_LOG;
    return iStartDate->Date().DateTime();
    }

// ---------------------------------------------------------------------------
// CESMRMemoTimeValidator::EndDateTimeL
// ---------------------------------------------------------------------------
//
TDateTime CESMRMemoTimeValidator::EndDateTimeL()
    {
    FUNC_LOG;
    TDateTime endTime = iEndDate->Date().DateTime();
    // these need to be set; calendar views does not show
    // the memo for last date if end date is set different
    // from start date.

    endTime.SetHour(KMaxHours);
    endTime.SetMinute(KMaxMinutes);
    endTime.SetSecond(KMaxSeconds);

    return endTime;
    }

// EOF

