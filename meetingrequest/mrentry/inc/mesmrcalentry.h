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
class MESMRCalDbMgr;
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
        EESMRCalEntryNotSupported = EESMREventTypeNone,
        EESMRCalEntryMeetingRequest = EESMREventTypeMeetingRequest,
        EESMRCalEntryMeeting = EESMREventTypeAppt,
        EESMRCalEntryTodo = EESMREventTypeETodo,
        EESMRCalEntryMemo = EESMREventTypeEEvent,
        EESMRCalEntryReminder = EESMREventTypeEReminder,
        EESMRCalEntryAnniversary = EESMREventTypeEAnniv
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

    /**
     * Enumeration for entry 
     */
    enum TMREntryCapability
        {
        // Entry supports attachments
        EMRCapabilityAttachments
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
    * @param aRule ETrue event type is changing.
    */
   virtual void SetModifyingRuleL(TESMRRecurrenceModifyingRule aRule, 
		   const TBool aTypeChanging = EFalse ) = 0;
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
     * Tests, whether entry is edited.
     * @return ETrue, if entry is edited.
     */
    virtual TBool IsEntryEditedL() const = 0;
    
    /**
     * Tests, if entry type has changed from the original
     * @return ETrue, if entry type has changed
     */
    virtual TBool IsEntryTypeChangedL() const = 0;

    /**
     * Sets the entry type changed
     * @param aTypeChanged, ETrue if type changed, otherwise EFalse
     */
    virtual void SetTypeChanged( TBool aTypeChanged ) = 0;
    
    /**
     * Checks if any instance (having same UID)
     * occurs between specified time. Time information
     * is considered to be device's local time.
     *
     * @param aStart Start time
     * @param aEnd End time
     */
    virtual TBool AnyInstancesBetweenTimePeriodL(
                TTime& aStart,
                TTime& aEnd ) = 0;
    
    /**
     * Fetches first instances start and end time. For non-recurrent
     * entries this returns the entry's start and end time.
     *
     * @param aStart On returns contains the first instance's start time.
     * @param aEnd On returns contains the first instance's end time.
     */
    virtual void GetFirstInstanceStartAndEndTimeL(
                TTime& aStart,
                TTime& aEnd ) = 0;

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
    
    /**
     * Updates the entry after storing
     */
    virtual void UpdateEntryAfterStoringL() = 0;    
    
    /**
     * Updates the comparative entry (needed in entry type changing)
     */
    virtual void UpdateComparativeEntry( CCalEntry* aNewComparativeEntry ) = 0;    
    
    /**
     * Sets default values to entry.
     */
    virtual void SetDefaultValuesToEntryL() = 0;    
    
    /**
     * Updates entry's timestamp (DTSTAMP) information.
     */
    virtual void UpdateTimeStampL() = 0;
    
    /**
     * Clones entry and leaves the clone into CleanupStack.
     * 
     * @param aType the entry type of the clone. If EESMRCalEntryNotSupported
     * the original type is preserved. 
     */
    virtual CCalEntry* CloneEntryLC(
            TESMRCalEntryType aType = EESMRCalEntryNotSupported ) const = 0;
    
    /**
     * Fetches validated entry. Ownership is transferred to caller.
     * @return Pointer to validated entry.
     */
    virtual CCalEntry* ValidateEntryL() = 0;
    
    /**
     * For recurrent event method removes this from the series.
     * Entry needs to be recurrent event and modification rule
     * needs to be MESMRCalEntry::EESMRThisOnly.
     * Ownership of the returned calendar entry is transferred to caller.
     *
     * @return Parent entry
     */
    virtual CCalEntry* RemoveInstanceFromSeriesL() = 0;
    
    /**
     * Get calendar dbmgr
     * 
     * @return calendar dbmgr
     */
    virtual MESMRCalDbMgr& GetDBMgr() = 0; 
    
    /**
     * Tests whether entry supports capability or not.
     * @param aCapability Capability to be checked
     * @return ETrue if capability is supported.
     */
    virtual TBool SupportsCapabilityL( 
            TMREntryCapability aCapability ) const = 0;
    
    /**
     * Tests whether entry contains remote attachments or not.
     * @return ETrue if entry contains remote attachments
     */
    virtual TBool ContainsRemoteAttachmentsL() = 0;
    };

#endif // MESMRCALENTRY_H

// EOF
