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
* Description:  Interface definition for ESMR meeting request entry
*
*/


#ifndef MESMRMEETINGREQUESTENTRY_H
#define MESMRMEETINGREQUESTENTRY_H

#include "CFSMailCommon.h"
#include "mesmrcalentry.h"
#include "esmrdef.h"

class CCalAttendee;
class MMRInfoObject;
class TESMRInputParams;

/**
 * MESMRCalEntry provides utility functions for handling MR Entry.
 *
 * @lib esmrservices.lib
 */
class MESMRMeetingRequestEntry : public MESMRCalEntry
    {
public:
    /** Enumeration for meeting request entry information */
    enum TESMREntryInfo
        {
        EESMREntryInfoOutOfDate,            // Entry is ouyt of date
        EESMREntryInfoCancelled,            // Entry is cancelled
        EESMREntryInfoAccepted,             // Entry is accepted
        EESMREntryInfoTentativelyAccepted,  // Entry is tentatively accepted
        EESMREntryInfoDeclined,             // Entry is declined
        EESMREntryInfoOccursInPast,         // Entry occurs in past
        EESMREntryInfoConflicts,            // Entry conflicts with another entry
        EESMREntryInfoNormal                // Normal case
        };

public:
    /**
     * Virtual destructor.
     */
    virtual ~MESMRMeetingRequestEntry() { }

public:

    /**
     * Fetches the phone owner's role in this calendar entry.
     * @return User role.
     * @see TESMRRole.
     */
    virtual TESMRRole RoleL() const = 0;

    /**
     * Checks, if this entry conflicts with other entries.
     * @return ETrue, if conflicts exists.
     */
    virtual TBool Conflicts() const = 0;

    /**
     * For organizer, this marks meeting cancelled.
     * For attendee, this constructs response entry
     * and marks attendee status 'declined'.
     */
    virtual void MarkMeetingCancelledL() = 0;

    /**
     * Constructs reply entry and sets attendee status.
     * Phone owner needs to be found from entrys attendee list
     * and role needs to be participant or optional participant.
     *
     * @param aStatus Attendee status
     */
    virtual void ConstructReplyL(
            TESMRAttendeeStatus aStatus ) = 0;

    /**
     * Tests, whether entry is out of date. Entry is out of date, if
     * newer entry exists in calendar db.
     * @return ETrue, if entry is out of date.
     */
    virtual TBool IsEntryOutOfDateL() const = 0;

    /**
     * Tests, if meeting is cancelled.
     * @return ETrue, if meeting is cancelled.
     */
    virtual TBool IsMeetingCancelledL() const = 0;

    /**
     * Fetches attendee status. Phone owner needs to be in the attendee
     * list.
     * @return Attendee status.
     * @error KErrNotFound if phone owner not found from attendee list.
     */
    virtual TESMRAttendeeStatus AttendeeStatusL() const = 0;

    /**
     * Tests if this is forwarded entry.
     * @return ETrue, if entry is forwarded.
     */
    virtual TBool IsForwardedL() const = 0;

    /**
     * Switched entry into forward mode. Current entry's all fields remain
     * as they are.Entry's method is switched into request and attendee
     * list is emptied.
     * @error KErrNotSupported Current entry cannot be forwarded.
     */
    virtual void SwitchToForwardL() = 0;

    /**
     * Switched entry into orginal mode. Entry is restored to same state
     * before switching into forwarding mode.
     * @error KErrNotSupported Current entry cannot be forwarded.
     */
    virtual void SwitchToOrginalL() = 0;

    /**
     * Sets entry's status to confirmed.
     */
    virtual void ConfirmEntryL() = 0;

    /**
     * Checks, whether entry occurs in the past
     * @return ETrue, if entry occurs in the past.
     */
    virtual TBool OccursInPastL() const = 0;

    /**
     * Fetches entry information in attendee mode.
     * Method will leave if phone owner is organizer.
     * @return Entry attendee information
     * @error KErrNotSupported Phone owner is organizer.
     */
    virtual TESMREntryInfo EntryAttendeeInfoL() const = 0;

    /**
     * Fetches attendees from entry. Attendees are added to
     * aAttendeeArray. Attendees are filtered according to aFilterFlags
     * parameter.
     * Filtering flags is combimnation of enumeration TESMRRole values.
     * For example, aFilterFlags =  EESMRRoleRequiredAttendee --> Required
     * attendees are included to array. Ownership of the attendee objects is
     * not transferred.
     * @param aAttendeeArray Reference to attendee array
     * @param aFilterFlags Attendee filtering flags.
     */
    virtual void GetAttendeesL(
            RArray<CCalAttendee*>& aAttendeeArray,
            TUint aFilterFlags ) const = 0;

    /**
     * Fetches validated entry. Ownership is transferred to caller.
     * @return Pointer to validated entry.
     */
    virtual CCalEntry* ValidateEntryL() = 0;

    /**
    * Fetches the conflicting entries. Conflicting entries are added
    * to aEntryArray.
    * @param aEntryArray Array containing conflicting entries
    * @return KErrNotFound if no conflicting entries, KErrNone otherwise.
    */
    virtual TInt FetchConflictingEntriesL(
            RPointerArray<CCalEntry>& aEntryArray ) = 0;

    /**
     * Tests if meeting request entry contains synchronization object.
     * @return ETrue, if sync object is present
     */
    virtual TBool IsSyncObjectPresent() const = 0;

    /**
     * Fetches the reference to sync object If sync object is not
     * present, method leaves with KErrNotSupported.
     */
    virtual MMRInfoObject& SyncObjectL() = 0;

    /**
     * Validates the sync object and return pointer to it. If sync object is not
     * present, method leaves with KErrNotSupported.
     */
    virtual MMRInfoObject& ValidateSyncObjectL() = 0;

    /**
     * Fetches startup parameters. Return ETrue if startup parameters
     * were filled to aStarupParams structure.
     * @param aStarupParams Reference to startup params structure
     * @return ETrue, if startup params were present
     */
    virtual TBool StartupParameters(
            TESMRInputParams& aStarupParams) const = 0;

    /**
     * Fetches attendee count.
     *
     * Filtering flags is combimnation of enumeration TESMRRole values.
     * For example, aFilterFlags =  EESMRRoleRequiredAttendee --> Required
     * attendee count is returns.
     *
     * @return Attendee count
     */
    virtual TInt AttendeeCountL(
            TUint aFilterFlags ) const = 0;

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
     * Sets default values to entry.
     */
    virtual void SetDefaultValuesToEntryL() = 0;

    /**
     * Tests, if entry is opened from mail.
     * @return ETrue if entry is opened from mail.
     */
    virtual TBool IsOpenedFromMail() const = 0;

    /**
     * Fetches attendees from entry. Attendees are added to
     * aAttendeeArray. Attendees are filtered according to aFilterFlags
     * parameter.
     * Filtering flags is combimnation of enumeration TESMRRole values.
     * For example, aFilterFlags =  EESMRRoleRequiredAttendee --> Required
     * attendees are included to array. Ownership of the attendee objects is
     * not transferred.
     * @param aAttendeeArray Reference to attendee array
     * @param aFilterFlags Attendee filtering flags.
     */
    virtual void GetAddedAttendeesL(
            RArray<CCalAttendee*>& aAttendeeArray,
            TUint aFilterFlags ) const = 0;

    /**
     * Fetches attendees from entry. Attendees are added to
     * aAttendeeArray. Attendees are filtered according to aFilterFlags
     * parameter.
     * Filtering flags is combimnation of enumeration TESMRRole values.
     * For example, aFilterFlags =  EESMRRoleRequiredAttendee --> Required
     * attendees are included to array. Ownership of the attendee objects is
     * not transferred.
     * @param aAttendeeArray Reference to attendee array
     * @param aFilterFlags Attendee filtering flags.
     */
    virtual void GetRemovedAttendeesL(
            RArray<CCalAttendee*>& aAttendeeArray,
            TUint aFilterFlags ) const = 0;

    /**
     * Updates the entry after storing
     */
    virtual void UpdateEntryAfterStoringL() = 0;
    
    /**
     * Updates child entries sequence numbers and stores them to
     * calendar DB.
     */
    virtual void UpdateChildEntriesSeqNumbersL() = 0;
    
    /**
     * Resolves current plugin in use.
     * @return current plug-in in use.
     */
    virtual TESMRMailPlugin CurrentPluginL() = 0;

    /**
     * Resolves current message box Id.
     * @return current message box Id.
     */
    virtual TFSMailMsgId CurrentMailBoxIdL() = 0;
    
    /**
     * Updates entry's timestamp (DTSTAMP) information.
     */
    virtual void UpdateTimeStampL() = 0;
    
    /**
     * Checks if any instance (having same UID)
     * occurs between specified time. Time information
     * is considered to be device's local time.
     * 
     * @param aStart Start time
     * @param aEnd End time
     */
    virtual TBool AnyInstanceOnDayL(
            TTime& aStart,
            TTime& aEnd ) = 0;
    };

#endif // MESMREMEETINGREQUESTSENDER_H
