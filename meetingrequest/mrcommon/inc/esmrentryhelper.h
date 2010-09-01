/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition for static entry helper methods
*
*/


#ifndef MESMRENTRYHELPER_H
#define MESMRENTRYHELPER_H

#include "esmrdef.h"
#include <e32base.h>
#include <MAgnEntryUi.h>

class CCalEntry;
class CCalAttendee;
class CMRMailboxUtils;

/**
 *  Static helpers methods for MR Viewers for analyzing entry
 *  content and properties.
 */
NONSHARABLE_CLASS(ESMREntryHelper)
    {
public: // New functions

    /**
    * Tells the mailbox id to use for sending a request or a response.
    * From mail application we may only respond, and in that case we
    * use the incoming mailbox. From other places we must use the
    * default MR mailbox setting.
    * @param aInParams input parameters for AgnEntryUi
    * @param aUtils reference to MR Utils
    * @return mailbox id, KMsvNullIndexEntryId if not available
    */
    IMPORT_C static TMsvId SendingMailBoxL(
            const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
            CMRMailboxUtils& aUtils );

    /**
    * Tells whether the entry belongs to a repeating meeting.
    * The definition of a repeating meeting is not that there must be
    * multiple instances, but that repeating properties exist (e.g. RRule).
    */
    IMPORT_C static TBool IsRepeatingMeetingL(
            const CCalEntry& aEntry );

    /**
    * Tests whether given entry is an originating entry or a modifying
    * entry.
    * @param aEntry to evaluate
    * @return ETrue if modifying entry, EFalse otherwise
    */
    IMPORT_C static TBool IsModifyingEntryL(
            const CCalEntry& aEntry );

    /**
    * Has this particular entry with it's latest saved content been sent?
    * This is determined from the DTSTAMP value which is null until
    * up to date entry is sent.
    * @param aEntry
    * @return ETrue if entry has been sent
    */
    IMPORT_C static TBool IsLatestSavedSentL(
            const CCalEntry& aEntry );

    /**
    * Checks if either entire meeting is cancelled, or if entry is
    * cancellation for this particular attendee.
    * @param aEntry
    * @return ETrue if meeting cancelled from user's point of view
    */
    IMPORT_C static TBool IsCancelledL(
            const CCalEntry& aEntry,
            CMRMailboxUtils& aUtils );
    
    /**
    * Checks if entry is all-day event.
    * @param aEntry
    * @return ETrue if meeting is all-day event
    */
    IMPORT_C static TBool IsAllDayEventL(
            const CCalEntry& aEntry );

    /**
    * Checks whether entry is out of date or not.
    * @param aEntry entry to check
    * @return ETrue if entry is outdated
    */
    IMPORT_C static TBool OccursInPastL(
            const CCalEntry& aEntry );

    /**
    * Returns phone owner's address, KNullDesC if not found.
    * @param aEntry entry to evaluate
    * @param aUtils utility reference
    * @return address
    */
    IMPORT_C static const TDesC& PhoneOwnerAddrL(
            const CCalEntry& aEntry,
            CMRMailboxUtils& aUtils );

    /**
    * Searches for the given attendee in aEntry. This is usable e.g.
    * in case of response, when compare and update attendee in
    * response and request. Matching is done based on e-mail address.
    * @param aAttendee reference attendee to be found
    * @aEntry entry containing possibly multiple attendees
    * @return matching attendee or NULL, ownership is not transferred
    */
    IMPORT_C static CCalAttendee* EqualAttendeeL(
            const CCalAttendee& aAttendee,
            const CCalEntry& aEntry );

    /**
    * Method which checks if start and end time span multiple days.
    * @param aEntry
    * @return ETrue if spans multiple days
    */
    IMPORT_C static TBool SpansManyDaysL(
            const TCalTime& aStartTime,
            const TCalTime& aEndTime );
    
    /**
     * Resolves the calendar entry type.
     * @param aCalEntry reference to calendar entry
     */
    IMPORT_C static TESMRCalendarEventType EventTypeL(
            const CCalEntry& aCalEntry );
    
    /**
     * Sets child entry's start and end time.
     *
     * @param aChild Reference to child entry
     * @param aParent Reference to parent entry
     * @param aChildEnd Child entry's start time
     */
    IMPORT_C static void SetInstanceStartAndEndL(
            CCalEntry& aChild,
            const CCalEntry& aParent,
            const TCalTime& aChildStart );

    /**
     * Checks and adjust repeat until validity. Method adjusts entry's
     * recurrence until time if needed.
     * @param aEntry Reference to parent entry
     * @param aInstanceTime Reference to instances time
     */
    IMPORT_C static void CheckRepeatUntilValidityL(
            CCalEntry& aEntry,
            const TCalTime& aInstanceTime );    
    };

#endif      // MESMRENTRYHELPER_H
