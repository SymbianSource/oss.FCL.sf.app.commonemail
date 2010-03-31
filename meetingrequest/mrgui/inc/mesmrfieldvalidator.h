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
* Description:  ESMR interface for time/date validation implemenatation
 *
*/


#ifndef MESMRFIELDVALIDATOR_H
#define MESMRFIELDVALIDATOR_H

//<cmail>
#include "esmrdef.h"
//</cmail>

class MESMRCalEntry;
class CEikTimeEditor;
class CEikDateEditor;
class MESMRFieldEventQueue;
class MMRAbsoluteAlarmController;

/**
 * MESMRFieldValidator defines interface for field validation.
 * Validators check input values and adjust field values to
 * correctly.
 *
 * MESMRFieldValidator is base class for all concrete validators.
 */
class MESMRFieldValidator
    {
public:

    /**
     * Enumeration for validator error types
     */
    enum TESMRFieldValidatorError
        {
        /** No validation errors */
        EErrorNone = 0,
        /** Entry ends earlier than starts */
        EErrorEndEarlierThanStart,
        /** Entry is recurrent and has differend start and end dates */
        EErrorRecDifferetStartAndEnd,
        /** Recurrence until is earlier than first instance start time */
        EErrorRecUntilEarlierThanStart,
        /** Alarm is later that entry starts */
        EErrorAlarmLaterThanStart,
        /** Alarm occurs in past */
        EErrorAlarmInPast,
        /** Alarm occurs too much in past*/
        EErrorAlarmTooMuchInPast,
        /** Relative alarm occurs in past */
        EErrorRelativeAlarmInPast,
        /** Reschedule instance to same day with another instance */
        EErrorRescheduleInstance,
        /** The errors be used to validate the time of one edited intance of seires*/
        EErrorInstanceOverlapsExistingOne,
        EErrorInstanceAlreadyExistsOnThisDay,
        EErrorInstanceOutOfSequence
        };

public:
    /**
     * Virtual destructor.
     */
    virtual ~MESMRFieldValidator( ) { }

    /**
     * Validate entered values. Returns validation eror code.
     * Parameter aCorrectAutomatically defines, if validator
     * adjusts field so that no validation errors occur.
     *
     * @param aCorrectAutomatically If ETrue, fields are adjusted
     *                              automatically.
     * @return Validation error code.
     */
    virtual TESMRFieldValidatorError ValidateL(
            TBool aCorrectAutomatically ) = 0;

    /**
     * Read values to entry and stores them to fields.
     * @param aEntry Reference to entry.
     */
    virtual void ReadValuesFromEntryL(
            MESMRCalEntry& aEntry ) = 0;

    /**
     * Store values to entry.
     * @param aEntry Reference to entry.
     */
    virtual void StoreValuesToEntryL(
            MESMRCalEntry& aEntry ) = 0;

    /**
     * Sets start time field.
     * @param aStartTime Reference to start time field.
     */
    virtual void SetStartTimeFieldL(
            CEikTimeEditor& aStartTime ) = 0;

    /**
     * Sets end time field.
     * @param aEndTime Reference to end time field.
     */
    virtual void SetEndTimeFieldL(
            CEikTimeEditor& aEndTime ) = 0;

    /**
     * Sets start time field.
     * @param aStartDate Reference to start date field.
     */
    virtual void SetStartDateFieldL(
            CEikDateEditor& aStartDate ) = 0;

    /**
     * Sets end time field.
     * @param aEndDate Reference to end date field.
     */
    virtual void SetEndDateFieldL(
            CEikDateEditor& aEndDate ) = 0;

    /**
     * Sets alarm time field.
     * @param aAlarmTime Reference to alarm time field.
     */
    virtual void SetAlarmTimeFieldL(
            CEikTimeEditor& aAlarmTime ) = 0;

    /**
     * Sets alarm date field.
     * @param aAlarmDate Reference to alarm date field.
     */
    virtual void SetAlarmDateFieldL(
            CEikDateEditor& aAlarmDate ) = 0;

    /**
     * Sets absolute alarm on off field.
     * @param aAbsoluteAlarmOnOff Reference to interface to handle absolute 
     * alarm on-off setting.
     */
    virtual void SetAbsoluteAlarmOnOffFieldL( 
            MMRAbsoluteAlarmController& aAbsoluteAlarmController ) = 0;
    
    /**
     * Sets alarm date field.
     * @param aAlarmDate Reference to alarm date field.
     */
    virtual void SetRecurrenceUntilDateFieldL(
            CEikDateEditor& aRecurrenceUntil ) = 0;

    /**
     * Triggers start time change.
     */
    virtual void StartTimeChangedL() = 0;

    /**
     * Triggers end time change.
     */
    virtual void EndTimeChangedL() = 0;

    /**
     * Triggers start date change.
     */
    virtual void StartDateChandedL() = 0;

    /**
     * Triggers end date change.
     */
    virtual void EndDateChangedL() = 0;

    /**
     * Triggers alarm time change.
     */
    virtual void AlarmTimeChangedL() = 0;

    /**
     * Triggers alarm date change.
     */
    virtual void AlarmDateChangedL() = 0;

    /**
     * Triggers relative alarm change.
     * When values are forced in meeting validator, alarmfields
     * Externalize method needs to know if relative alarm is valid.
     * If aRelativeAlarmValid != ETrue, then alarm is considered
     * to be unvalid, and is set off (in CESMRAlarmField::ExternalizeL)
     *
     * @param aCurrentAlarmTimeOffset current alarm time offset
     * @param aRelativeAlarmValid for alarm validity check
     */
    virtual void RelativeAlarmChangedL(
            TTimeIntervalMinutes aCurrentAlarmTimeOffset,
            TBool aHandleAlarmChange,
            TBool& aRelativeAlarmValid ) = 0;

    /**
     * Set 'all day' flag for validator.
     */
    virtual void SetAllDayEventL(
            TBool aAlldayEvent ) = 0;

    /**
     * Set 'alarm' flag for validator.
     * @param aAlarmOn Alarm on/off
     */
    virtual void SetAlarmOnOffL(
            TBool aAlarmOn ) = 0;

    /**
     * Triggers recurrence change.
     * @param aRecurrence new recurrence value
     */
    virtual void RecurrenceChangedL(
            TESMRRecurrenceValue aRecurrence ) = 0;

    /**
     * Triggers end date change.
     */
    virtual void RecurrenceEndDateChangedL() = 0;

    /**
     * Checks if relative alarm is valid
     * @param aAlarmTimeOffset Alarm time offset
     */
    virtual TBool IsRelativeAlarmValid(
            TTimeIntervalMinutes aAlarmTimeOffset ) = 0;

    /**
     * Sets field event queue for validator.
     * Validator can send field events to event observers using the
     * event queue.
     * 
     * @param aEventQueue the event queue.
     */
    virtual void SetFieldEventQueue( MESMRFieldEventQueue* /*aEventQueue*/ ) {}
    };

#endif  // MESMRFIELDVALIDATOR_H

// EOF


