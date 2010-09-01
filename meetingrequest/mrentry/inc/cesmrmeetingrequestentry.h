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
* Description:  ESMR mr entry definition
*
*/


#ifndef CESMRMEETINGREQUESTENTRY_H
#define CESMRMEETINGREQUESTENTRY_H


//  INCLUDES
#include <e32base.h>
#include "mesmrmeetingrequestentry.h"
#include "tesmrinputparams.h"

class CMRMailboxUtils;
class MESMRCalDbMgr;
class TESMRInputParams;

/**
 * CESMRCalEntry is wrapper for CCalEntry.
 * It provides also some utility functions for
 * handling MR Entry
 */
NONSHARABLE_CLASS(CESMRMeetingRequestEntry) :
        public CBase,
        public MESMRMeetingRequestEntry

    {
public: // Construction and destruction
    /**
     * Creates and initialzes CESMRCalEntry object.
     * @param aEntry Reference to calendar entry
     * @param aMRMailboxUtils Reference to mailbox utilities
     * @param aCalDb Reference to cal db manager
     * @param aConflictsExists ETrue, if conflicts exists
     * @param aESMRInputParams ESMR input parameters structure
     */
    IMPORT_C static CESMRMeetingRequestEntry* NewL(
            const CCalEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            MESMRCalDbMgr& aCalDb,
            TBool aConflictsExists,
            TESMRInputParams* aESMRInputParams = NULL );

    /**
     * C++ destructor
     */
    IMPORT_C ~CESMRMeetingRequestEntry();

public: // From MESMRMeetingRequestEntry
    TESMRCalEntryType Type() const;
    MESMRCalEntry& MESMRCalEntryRef();
    const MESMRCalEntry& MESMRCalEntryRef() const;
    const CCalEntry& Entry() const;
    CCalEntry& Entry();
    CCalInstance* InstanceL() const;
    TBool CanSetRecurrenceL() const;
    TBool IsRecurrentEventL() const;
    void SetRecurrenceL(
            TESMRRecurrenceValue aRecurrence,
            TTime aUntil = Time::NullTTime() );
    void GetRecurrenceL(
            TESMRRecurrenceValue& aRecurrence,
            TTime& aUntil) const;
    TESMRRecurrenceModifyingRule RecurrenceModRule() const;
    void SetModifyingRuleL(TESMRRecurrenceModifyingRule aRule,
    		const TBool aTypeChanging = EFalse );
    void SetAllDayEventL(
            TTime aStartDate,
            TTime aEndDate );
    TBool IsAllDayEventL() const;
    TBool IsStoredL() const;
    TBool IsSentL() const;
    TBool IsEntryEditedL() const;
    TBool IsEntryTypeChangedL() const;
    void GetAlarmL(
            MESMRCalEntry::TESMRAlarmType& aAlarmType,
            TTime &aAlarmTime );
    const CCalEntry& OriginalEntry();
    void UpdateEntryAfterStoringL();
    void SetDefaultValuesToEntryL();
    CCalEntry* CloneEntryLC( TESMRCalEntryType aType ) const;
    TESMRRole RoleL() const;
    TBool Conflicts() const;
    void MarkMeetingCancelledL();
    void ConstructReplyL(
            TESMRAttendeeStatus aStatus );
    TBool IsEntryOutOfDateL() const;
    TBool IsMeetingCancelledL() const;
    TESMRAttendeeStatus AttendeeStatusL() const;
    TBool IsForwardedL() const;
    void SwitchToForwardL();
    void SwitchToOrginalL();
    void ConfirmEntryL();
    TBool OccursInPastL() const;
    TESMREntryInfo EntryAttendeeInfoL() const;
    void SetPriorityL(
            TUint aPriority );
    TUint GetPriorityL() const;
    void GetAttendeesL(
            RArray<CCalAttendee*>& aAttendeeArray,
            TUint aFilterFlags ) const;
    CCalEntry* ValidateEntryL();
    TInt FetchConflictingEntriesL(RPointerArray<CCalEntry>& aEntryArray);
    TBool IsSyncObjectPresent() const;
    MMRInfoObject& SyncObjectL();
    MMRInfoObject& ValidateSyncObjectL();
    TBool StartupParameters(
            TESMRInputParams& aStarupParams) const;
    TInt AttendeeCountL(
            TUint aFilterFlags ) const;
    CCalEntry* RemoveInstanceFromSeriesL();
    TBool IsOpenedFromMail() const;
    void GetAddedAttendeesL(
            RArray<CCalAttendee*>& aAttendeeArray,
            TUint aFilterFlags ) const;
    void GetRemovedAttendeesL(
            RArray<CCalAttendee*>& aAttendeeArray,
            TUint aFilterFlags ) const;
    void UpdateChildEntriesSeqNumbersL();
    TESMRMailPlugin CurrentPluginL();
    void UpdateTimeStampL();
    TBool AnyInstancesBetweenTimePeriodL(
                TTime& aStart,
                TTime& aEnd );
    void GetFirstInstanceStartAndEndTimeL(
                TTime& aStart,
                TTime& aEnd );
    const TDesC& CalendarOwnerAddressL() const;
    CMRMailboxUtils& MailboxUtils() const;
    void UpdateComparativeEntry(
            CCalEntry* aNewComparativeEntry );
    MESMRCalDbMgr& GetDBMgr();
    TBool SupportsCapabilityL(
            MESMRCalEntry::TMREntryCapability aCapability ) const;
    TBool ContainsRemoteAttachmentsL();
    TBool SendCanellationAvailable();
    void SetSendCanellationAvailable (TBool aSendCanellation);
    void SetTypeChanged( TBool aTypeChanged );

private: // Implementaton
    CESMRMeetingRequestEntry(
            CMRMailboxUtils& aMRMailboxUtils,
            MESMRCalDbMgr& aCalDb,
            TBool aConflictsExists,
            TESMRInputParams* aESMRInputParams );
    void ConstructL(
            const CCalEntry& aEntry );
    HBufC* ReplaceCharactersFromBufferLC(
            const TDesC& aTarget,
            const TDesC& aFindString,
            const TDesC& aReplacement );
    void SetDefaultDatabaseL();

private: // Data
    /// Own: Calendar entry
    CCalEntry* iEntry;
    /// Own: Forward calendar entry
    CCalEntry* iForwardEntry;
    /// Ref: Reference to mailbox utilities
    CMRMailboxUtils& iMRMailboxUtils;
    /// Own: Flag indicating, whether conflicts exists
    TBool iConflictsExists;
    /// Ref: Reference to cal db manager
    MESMRCalDbMgr& iCalDb;
    /// Own: Recurrence modification rule
    TESMRRecurrenceModifyingRule iRecurrenceModRule;
    /// Ref: ESMR input parameters
    TESMRInputParams* iESMRInputParams;
    /// Own: Orginal entry
    CCalEntry* iOrginalEntry;
    /// Own: Entry received as parameter
    CCalEntry* iParameterEntry;
    /// Own: Current FS Email plug-in
    TESMRMailPlugin iCurrentFSEmailPlugin;
    /// Flag for removing attachments
    TBool iRemoveAttachments;
    /// Flag for send canellation
    TBool iSendCanellation;
    // Own: Indicates if the entry is type changed or not
    TBool iTypeChanged;
    };

#endif // CESMRMEETINGREQUESTENTRY_H

// EOF

