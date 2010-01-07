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
    static CESMRMeetingRequestEntry* NewL(
            const CCalEntry& aEntry,
            CMRMailboxUtils& aMRMailboxUtils,
            MESMRCalDbMgr& aCalDb,
            TBool aConflictsExists,
            TESMRInputParams* aESMRInputParams = NULL );

    /**
     * C++ destructor
     */
    ~CESMRMeetingRequestEntry();

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
    void SetModifyingRuleL(
            TESMRRecurrenceModifyingRule aRule );
    void SetAllDayEventL(
            TTime aStartDate,
            TTime aEndDate );
    TBool IsAllDayEventL() const;
    TBool IsStoredL() const;
    TBool IsSentL() const;
    TBool IsEntryEditedL() const;
    void GetAlarmL(
            MESMRCalEntry::TESMRAlarmType& aAlarmType,
            TTime &aAlarmTime );
    const CCalEntry& OriginalEntry();
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
    void SetDefaultValuesToEntryL();
    TBool IsOpenedFromMail() const;
    void GetAddedAttendeesL(
            RArray<CCalAttendee*>& aAttendeeArray,
            TUint aFilterFlags ) const;
    void GetRemovedAttendeesL(
            RArray<CCalAttendee*>& aAttendeeArray,
            TUint aFilterFlags ) const;
    void UpdateEntryAfterStoringL();
    void UpdateChildEntriesSeqNumbersL();
    TESMRMailPlugin CurrentPluginL();
    TFSMailMsgId CurrentMailBoxIdL();
    void UpdateTimeStampL();
    TBool AnyInstanceOnDayL(
                TTime& aStart,
                TTime& aEnd );    
    
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

private: // Data
    /// Own: Calendar entry
    CCalEntry* iEntry;
    CCalEntry* iBackupEntry;
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
    /// Ref: Entry received as parameter
    CCalEntry* iParameterEntry;
    /// Own: Current FS Email plug-in
    TESMRMailPlugin iCurrentFSEmailPlugin;
    };

#endif // CESMRMEETINGREQUESTENTRY_H

// EOF

