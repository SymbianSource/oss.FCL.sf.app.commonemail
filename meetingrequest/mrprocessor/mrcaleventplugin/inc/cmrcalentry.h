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
* Description:  CCalEntry wrapper class
*
*/


#ifndef C_MRCALENTRY_H
#define C_MRCALENTRY_H

#include <e32base.h>
#include <e32std.h>
#include <caltime.h>
#include <magnentryui.h>
#include "mesmrcalentry.h"

class CCalEntry;
class MESMRCalDbMgr;

 /**
  *  Provides utility functions for handling calendar entry.
  */
NONSHARABLE_CLASS( CMRCalEntry ) :
        public CBase,
        public MESMRCalEntry
    {
public:
    /**
     * Two-phased constructor.
     * @param aEntry Calendar Entry .
     * @param aGlobalData Calendar Global data instace.
     * @param aInstanceTime Entry instance time.
     * @param aEditorMode Defines: creating new / editing existing / viewing entry
     */
     /*static CMRCalEntry* NewL( 
             CCalEntry& aEntry,
             MCalenServices& aServices,
    		 const TCalTime& aInstanceTime,
    		 MAgnEntryUi::TAgnEntryUiEditorMode aEditorMode );*/
    
    static CMRCalEntry* NewL(
            CCalEntry& aEntry,
            MESMRCalDbMgr& aCalDb );

    /**
     * C++ destructor.
     */
    ~CMRCalEntry();

    /**
    * Updates the original entry. This is used when user store the entry
    * without exiting the editor.
    */
    void UpdateComparativeEntryL();
    
// From MESMRCalEntry

    TESMRCalEntryType Type() const;
    MESMRCalEntry& MESMRCalEntryRef();
    const MESMRCalEntry& MESMRCalEntryRef() const;
    const CCalEntry& Entry() const;
    CCalEntry& Entry();
    CCalInstance* InstanceL() const;
    TBool CanSetRecurrenceL() const;
    void GetRecurrenceL( TESMRRecurrenceValue& aRecurrence,
    					 TTime& aUntil ) const;
    TBool IsAllDayEventL() const;
    void SetRecurrenceL( TESMRRecurrenceValue aRecurrence,
    					 TTime aUntil = Time::NullTTime() );
    void SetAllDayEventL( TTime aStartDate,
    					  TTime aEndDate );
    TBool IsStoredL() const;
    TBool IsEntryEditedL() const;
    TBool IsRecurrentEventL() const;
    TBool IsEntryTypeChangedL() const;
    TESMRRecurrenceModifyingRule RecurrenceModRule() const;
    void SetModifyingRuleL( TESMRRecurrenceModifyingRule aRule,
    		const TBool aTypeChanging = EFalse );
    void SetPriorityL(
            TUint aPriority );
    TUint GetPriorityL() const;
    void GetAlarmL( TESMRAlarmType& aAlarmType,
    			    TTime &aAlarmTime );
    const CCalEntry& OriginalEntry();
    void UpdateEntryAfterStoringL();
    void UpdateComparativeEntry( CCalEntry* aNewComparativeEntry );
    void SetDefaultValuesToEntryL();
    void UpdateTimeStampL();
    CCalEntry* CloneEntryLC( TESMRCalEntryType aType ) const;
    CCalEntry* ValidateEntryL();
    CCalEntry* RemoveInstanceFromSeriesL();
    MESMRCalDbMgr& GetDBMgr();
    TBool SupportsCapabilityL( 
            MESMRCalEntry::TMREntryCapability aCapability ) const;
    TBool ContainsRemoteAttachmentsL();    
    void SetTypeChanged( TBool aTypeChanged );
    TBool AnyInstancesBetweenTimePeriodL(
                TTime& aStart,
                TTime& aEnd );
    void GetFirstInstanceStartAndEndTimeL(
                TTime& aStart,
                TTime& aEnd );
    TBool IsRepeatingMeetingL(
            const CCalEntry& aEntry) const;
    
private: // Implementation
    CMRCalEntry( MESMRCalDbMgr& aCalDb  );
    void ConstructL(
            CCalEntry& aEntry );

    CCalEntry* CreateCopyL( CCalEntry& aSourceEntry );

    
    void CreateInternalEntryL();
    void SetDefaultValuesForTodoL();
    void SetDefaultValuesForMemoL();
    void SetDefaultValuesForMeetingL();
    void SetDefaultValuesForAnniversaryL();    
    inline CCalEntry* DoGetEntry() const;
    CCalEntry* ValidateMeetingL();
    
private: // Data
    /// Ref: Caledar global data utility class
    // MCalenServices& iServices; // Not owned, don't release here.
    /// Own: Calendar entry
    CCalEntry* iEntry;
    /// Own: Copy of calendar entry instance
    CCalEntry* iComparativeEntry;
    /// Own: Copy from client. 
    CCalEntry* iOriginalEntry;    
    /// Own: Entry modifying rule
    TESMRRecurrenceModifyingRule iRecurrenceModRule;
    /// Own: Instance time
    TCalTime iInstanceTime;
    /// Cal DB Manager mode:
    MESMRCalDbMgr& iCalDb;
    // Own: Indicates if the entry is type changed or not
    TBool iTypeChanged;
    };

#endif // CBCCALENTRY_H
