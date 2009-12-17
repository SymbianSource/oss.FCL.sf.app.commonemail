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
* Description:  Interface definition for ESMR calendar entry
*
*/


#ifndef MESMRCALENTRY_H
#define MESMRCALENTRY_H

//<cmail>
#include "esmrdef.h"
//</cmail>

class CCalEntry;
class CCalInstance;

/**
 * MESMRMeetingRequestEntry provides utility functions for handling MR Entry.
 *
 * @lib esmrservices.lib
 */
class MESMRCalEntry
    {
public: // Definitions
    /** 
     * Enumeration for cal entry type
     */
    enum TESMRCalEntryType
        {
        EESMRCalEntryNotSupported = -1, // Entry type not supported
        EESMRCalEntryMeetingRequest,    // Meeting request entry
        EESMRCalEntryMeeting,           // Meeting entry
        EESMRCalEntryTodo,              // TO-DO entry
        EESMRCalEntryMemo,              // Memo entry
        EESMRCalEntryReminder,          // Reminder entry
        EESMRCalEntryAnniversary        // Anniversary entry
        };

    /** 
     * Enumeration for recurrence modifying rule
     */
    enum TESMRRecurrenceModifyingRule
        {
        // Modify only this entry
        EESMRThisOnly,
        // Modification applies to all entries in the series
        EESMRAllInSeries
        };

    /** 
     * Enumeration for alarm type
     */
    enum TESMRAlarmType
        {
        /** 
         * Entry does not contain alarm 
         */
        EESMRAlarmNotFound,
        /** 
         * Alarm is absolute
         */
        EESMRAlarmAbsolute,
        /** 
         * Alarm is relative to meeting start time
         */
        EESMRAlarmRelative
        };

public: // Destruction
    /**
     * Virtual destructor.
     */
    virtual ~MESMRCalEntry() { }

public: // Interface
    /**
     * Fetches calendar entry type.
     * @return calendar entry type.
     */
    virtual TESMRCalEntryType Type() const = 0;

    /**
     * Fetches reference to this calendar entry.
     * @return Reference to this entry.
     */
    virtual MESMRCalEntry& MESMRCalEntryRef() = 0;

    /**
     * Fetches const reference to this calendar entry.
     * @return const reference to this calendar entry.
     */
    virtual const MESMRCalEntry& MESMRCalEntryRef() const = 0;

    /**
     * Fetches constant reference to CCalEntry.
     * @return const reference to CCalEntry.
     */
    virtual const CCalEntry& Entry() const = 0;

    /**
     * Fetches reference to CCalEntry.
     * @return reference to CCalEntry.
     */
    virtual CCalEntry& Entry() = 0;

    /**
     * Fetches calendar entry's instance. Ownership is transferred to caller.
     * @return Reference to calendar entry's instance.
     */
    virtual CCalInstance* InstanceL() const = 0;

    /**
     * Tests, if recurrence can be set.
     * @return ETrue, if recurrence can be set. EFalse otherwise.
     */
    virtual TBool CanSetRecurrenceL() const = 0;

    /**
     * Tests, if this is recurrent meeting request.
     * @return ETrue, if this is recurrent meeting request.
     */
   virtual TBool IsRecurrentEventL() const = 0;

    /**
     * Fetches entry's recurrence value.
     * @param aRecurrence On return contains entry's recurrence information
     * @param aUntil On return contains time in UTC until recurrence is valid.
     */
    virtual void GetRecurrenceL(
            TESMRRecurrenceValue& aRecurrence,
            TTime& aUntil ) const = 0;

    /**
     * Sets recurrence to ES MR entry. If until time is not set
     * (i.e, it is null time), then until time is implementation
     * dependent.
     * When recurrence is set to ERecurrenceNot, then aUntil parameter
     * is ignored.
     * @param aRecurrece Recurrece value.
     * @param aUntil Recurrence valid until in UTC format.
     * @error KErrNotSupported Entry cannot be recurrenced.
     */
    virtual void SetRecurrenceL(
            TESMRRecurrenceValue aRecurrence,
            TTime aUntil = Time::NullTTime() ) = 0;

    /**
     * Fetches recurrence modification rule.
     * @return Modification rule.
     */
    virtual TESMRRecurrenceModifyingRule RecurrenceModRule() const = 0;

    /**
    * Sets modifying rule role to meeting request.
    * @param aRule Recurrence modification rule.
    */
   virtual void SetModifyingRuleL(
           TESMRRecurrenceModifyingRule aRule ) = 0;

        /**
     * Sets the entry priority.
     * @param aPriority entry priority.
     */
    virtual void SetPriorityL(
            TUint aPriority ) = 0;

    /**
     * Fetches the entry priority.
     * @return entry priority.
     */
    virtual TUint GetPriorityL() const = 0;

    /**
     * Sets meeting to be all day event.
     * @param aStartDate Start date information
     * @param aEndDate End date information
     */
    virtual void SetAllDayEventL(
            TTime aStartDate,
            TTime aEndDate ) = 0;

    /**
     * Tests, if entry is all day event.
     * @return ETrue, if entry is all day event
     */
    virtual TBool IsAllDayEventL() const = 0;

    /**
     * Tests whether this entry has been stored to db or not.
     * @return ETrue, if entry is stored to calendar DB.
     */
    virtual TBool IsStoredL() const = 0;

    /**
     * Tests if this entry has been sent to attendees.
     * @return ETrue, if entry is sent to attendees
     */
    virtual TBool IsSentL() const = 0;

    /**
     * Tests, whether entry is edited.
     * @return ETrue, if entry is edited.
     */
    virtual TBool IsEntryEditedL() const = 0;

    /**
     * Fetches alarm information from entry.
     * @param aAlarmType On return contains alarm type.
     * @param aAlarmTime On return contains time when alarm occurs.
     */
    virtual void GetAlarmL(
            TESMRAlarmType& aAlarmType,
            TTime &aAlarmTime ) = 0;

    /**
     * Fetches unmodified entry instance.
     * @return CCalEntry original entry
     */
    virtual const CCalEntry& OriginalEntry() = 0;
    };


#endif // MESMRCALENTRY_H

// EOF
