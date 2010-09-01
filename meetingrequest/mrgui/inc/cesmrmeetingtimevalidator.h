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
* Description:  ESMR wrapper class for date/time sanity checks and updates
*
*/


#ifndef CESMRMEETINGTIMEVALIDATOR_H
#define CESMRMEETINGTIMEVALIDATOR_H

#include <e32base.h>
#include <e32std.h>
//<cmail>
#include "esmrdef.h"
//</cmail>
#include "mesmrfieldvalidator.h"

class CCalEntry;
class CEikTimeEditor;
class CEikDateEditor;
class MESMRCalEntry;
class MMRAbsoluteAlarmController;

/**
 * Validator for meetings.
 *
 * @lib esmrgui.lib
 * @see MESMRFieldValidator
 */
NONSHARABLE_CLASS( CESMRMeetingTimeValidator ) :
        public CBase,
        public MESMRFieldValidator
    {
public:
    /**
     * Symbian constructor. Creates and initializes new
     * CESMRMeetingTimeValidator object.
     * @return Pointer to CESMRMeetingTimeValidator object
     */
    static CESMRMeetingTimeValidator* NewL();

    /**
     * C++ Destructor.
     */
    ~CESMRMeetingTimeValidator();

public: // from MESMRFieldValidator
    MESMRFieldValidator::TESMRFieldValidatorError ValidateL(
            TBool aCorrectAutomatically );
    void ReadValuesFromEntryL(
            MESMRCalEntry& aEntry );
    void StoreValuesToEntryL(
            MESMRCalEntry& aEntry );
    void SetStartTimeFieldL(
            CEikTimeEditor& aStartTime );
    void SetEndTimeFieldL(
            CEikTimeEditor& aEndTime );
    void SetStartDateFieldL(
            CEikDateEditor& aStartDate );
    void SetEndDateFieldL(
            CEikDateEditor& aEndDate );
    void SetAlarmTimeFieldL(
            CEikTimeEditor& aAlarmTime );
    void SetAlarmDateFieldL(
            CEikDateEditor& aAlarmDate );
    void SetRecurrenceUntilDateFieldL(
            CEikDateEditor& aRecurrenceUntil );
    void SetAbsoluteAlarmOnOffFieldL( 
            MMRAbsoluteAlarmController& aAbsoluteAlarmController ); 
    void StartTimeChangedL();
    void EndTimeChangedL();
    void StartDateChandedL();
    void EndDateChangedL();
    void AlarmTimeChangedL();
    void AlarmDateChangedL();
    void RelativeAlarmChangedL(
                TTimeIntervalMinutes aCurrentAlarmTimeOffset,
                TBool aHandleAlarmChange,
                TBool& aRelativeAlarmValid );
    void SetAllDayEventL(
            TBool aAlldayEvent );
    void SetAlarmOnOffL(
            TBool aAlarmOn );
    void RecurrenceChangedL(
            TESMRRecurrenceValue aRecurrence );
    void RecurrenceEndDateChangedL();
    TBool IsRelativeAlarmValid(
            TTimeIntervalMinutes aAlarmTimeOffset );
    void SetFieldEventQueue( MESMRFieldEventQueue* aEventQueue );
    MESMRFieldValidator::TESMRFieldValidatorError ValidateEditedInstanceTimeL();
private: // Implementation
    CESMRMeetingTimeValidator();
    TInt PreValidateEditorContent();
    void PreValidateEditorContentL();
    void HandleStartTimeChangeL();
    void HandleEndTimeChangeL();
    void HandleAlarmTimeChangedL();
    void HandleRelativeAlarmTimeChangedL();
    void HandleRecurrenceTypeChanged();
    void HandleRecurrenceEndDateChangedL();
    void DrawEditorsDeferred();
    TDateTime StartDateTime();
    TDateTime EndDateTime();
    TDateTime AlarmDateTime();
    TDateTime RecurrenceUntilTime();
    void ReadAlarmFromEntryL(
            CCalEntry& aEntry );
    void ReadRecurrenceFromEntryL(
            MESMRCalEntry& aEntry );
    void ForceValuesL();
    TDateTime ForceEndDateTime();
    TTime ForceRecurrenceUntilTime();
    void WriteStartAndEndTimeToEntryL(
            MESMRCalEntry& aEntry );
    void SendFieldChangeEventL( TESMREntryFieldId aFieldId );

private:
    // Ref: Reference to start time editor
    CEikTimeEditor* iStartTime;
    // Ref: Reference to end time editor
    CEikTimeEditor* iEndTime;
    // Ref: Reference to alarm time editor
    CEikTimeEditor* iAlarmTime;
    // Ref: Reference to start date editor
    CEikDateEditor* iStartDate;
    // Ref: Reference to end date editor
    CEikDateEditor* iEndDate;
    // Ref: Reference to alarm date editor
    CEikDateEditor* iAlarmDate;
    /// Ref: Reference to recurrence until date field
    CEikDateEditor* iRecurrenceUntilDate;
    // Ref: Reference to absolute alarm on of interface
    MMRAbsoluteAlarmController* iAbsoluteAlarmController;
    /// Own: Current recurrence value
    TESMRRecurrenceValue iRecurrenceValue;
    /// Ref: Entry being handled
    MESMRCalEntry* iEntry;
    /// Own: Current startTime
    TTime iCurrentStartTime;
    /// Own: Current end time
    TTime iCurrentEndTime;
    /// Own: Current alarm time
    TTime iCurrentAlarmTime;
    /// Own: Current recurrence until date
    TTime iCurrentRecurrenceUntil;
    /// Own: Start time before allday event
    TTime iStartTimeBeforeAlldayEvent;
    /// Own: End time before allday event
    TTime iEndTimeBeforeAlldayEvent;
    /// Own: Current alarm time offset
    TTimeIntervalMinutes iCurrentAlarmTimeOffset;
    /// Own: Flag if this is allday event
    TBool iAlldayEvent;
    /// Own: Flag for absolute alarm
    TBool iAlarmOnOff;
    /// Ref: Reference to relative alarm validity value
    TBool* iRelativeAlarmValid;
    /// Own: Editors previous start time value for recurrence handling
    TDateTime iComparativeStartTime;
    /// Ref: Field event queue
    MESMRFieldEventQueue* iEventQueue;
    };

#endif  // CESMRMEETINGTIMEVALIDATOR_H
