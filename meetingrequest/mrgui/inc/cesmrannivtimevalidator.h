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


#ifndef CESMRANNIVTIMEVALIDATOR_H
#define CESMRANNIVTIMEVALIDATOR_H

//<cmail>
#include "esmrdef.h"
//</cmail>
#include <e32base.h>
#include <e32std.h>
#include "mesmrfieldvalidator.h"

class MESMRCalEntry;
class CEikTimeEditor;
class CEikDateEditor;
class MMRAbsoluteAlarmController;

/**
 * Wrapper class handles sanity checks for date/time fields on Anniversary
 * Entry. For example start date can not be after end date.
 * this calss deals with the cheks for To-do option
 * InternalizeL() / ExternalizeL() is delegated to this class since those
 * need sanity checks. Fields (who use this wrapper class) shouldn't react
 * on ExternalizeL() functions since this wrapper class handles them. On
 * InternalizeL() they should attach their date/time editors in here.
 *
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRAnnivTimeValidator ):
        public CBase,
        public MESMRFieldValidator
    {
public:
    /**
     * Two phase constructor.
     */
    static CESMRAnnivTimeValidator* NewL();

    /**
     * Destructor.
     */
    ~CESMRAnnivTimeValidator();

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

private: // Implementation
    CESMRAnnivTimeValidator();
    TInt PreValidateEditorContent();
    void PreValidateEditorContentL();
    void DrawEditorsDeferred();
    TDateTime StartTimeL();
    TDateTime AlarmTimeL();
    void ForceValuesL();
    TBool IsModifiedL( MESMRCalEntry& aEntry );

private:
    /// Ref: Start date editor.
    CEikDateEditor* iStartDate;
    /// Ref: Alarm time editor
    CEikTimeEditor* iAlarmTime;
    /// Ref: Alarm date editor.
    CEikDateEditor* iAlarmDate;
    /// Own: Current anniversary date
    TTime iCurrentAnnivDate;
    /// Own: Current alarm time
    TTime iCurrentAlarmTime;
    /// Own: Flag for amarm on/off.
    TBool iAlarmOnOff;
    /// Ref: Entry being handled
    MESMRCalEntry* iEntry;
    /// Own: Saves initial alarm time, for later comparisons
    TTime iInitialAlarmTime;
    /// Own: Saves initial alarm date, for later comparisons
    TTime iInitialAlarmDate;
    /// Own: Saves initial alarm on off, for later comparisons
    TBool iInitialAlarmOnOff;
    /// Own: Saves initial due date, for later comparisons
    TTime iInitialStartDate;
    };

#endif  // CESMRANNIVTIMEVALIDATOR_H
