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
* Description:  ESMR To-do time wrapper class for date/time sanity checks
*                and updates
*
*/


#ifndef CESMRMEMOTIMEVALIDATOR_H
#define CESMRMEMOTIMEVALIDATOR_H

//<cmail>
#include "esmrdef.h"
//</cmail>
#include <e32base.h>
#include <e32std.h>
#include "mesmrfieldvalidator.h"

class MESMRCalEntry;
class CEikTimeEditor;
class CEikDateEditor;

/**
 * Wrapper class handles sanity checks for date/time fields on Memo/Day note
 * Entry. For example start date can not be after end date.
 * this calss deals with the cheks for To-do option
 * InternalizeL() / ExternalizeL() is delegated to this class since those
 * need sanity checks. Fields (who use this wrapper class) shouldn't react
 * on ExternalizeL() functions since this wrapper class handles them. On
 * InternalizeL() they should attach their date/time editors in here.
 *
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRMemoTimeValidator ) :
        public CBase,
        public MESMRFieldValidator
    {

public:
    /**
     * Two phase constructor. Creates and initializes CESMRMemoTimeValidator
     * object.
     * @return Pointer to CESMRMemoTimeValidator object.
     */
    static CESMRMemoTimeValidator* NewL();

    /**
     * C++ destructor.
     */
    ~CESMRMemoTimeValidator();

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
    CESMRMemoTimeValidator();
    TInt PreValidateEditorContent();
    void PreValidateEditorContentL();
    void DrawEditorsDeferred();
    TDateTime StartDateTimeL();
    TDateTime EndDateTimeL();

private: // Data
    /// Ref: Start date editor
    CEikDateEditor* iStartDate;
    /// Ref: End date editor
    CEikDateEditor* iEndDate;
    /// Own: Current start time.
    TTime iCurrentStartTime;
    /// Own: Current end time
    TTime iCurrentEndTime;
    /// Ref: Orginal entry
    MESMRCalEntry* iEntry;
    };

#endif  // CESMRMEMOTIMEVALIDATOR_H

