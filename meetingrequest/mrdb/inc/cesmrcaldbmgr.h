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
* Description:  Interface for calendar DB operations
*
*/


#ifndef CESMRCALDBMGR_H
#define CESMRCALDBMGR_H

#include <e32base.h>
#include <gdi.h>
//<cmail>
#include "mesmrutilstombsext.h"
//</cmail>
#include <calprogresscallback.h>
#include "cesmrcaldbbase.h"


class MMRUtilsObserver;
class CCalSession;
class CCalEntry;
class CESMREntryCmdIteratorAO;
class CESMRGraveyardCleaner;
class MESMRCalEntry;
class CCalEntryView;
class CCalInstanceView;
class CCalCalendarInfo;


/**
* MESMRServicesCalDbMgr defines interface for calendar DB
* operations.
*
* @lib esmrservices.lib
*/
class MESMRCalDbMgr : public MCalProgressCallBack
    {
public: // Constructors and destructors

    /**
    * Destructor.
    */
    virtual ~MESMRCalDbMgr() {};

    /**
     * Accessor for Agenda Model CCalEntryView owned by utility.
     * Returns a valid pointer if status is EAvailable or
     * EAvailableWithoutTombs, otherwise NULL.
     * @return entry view pointer, ownership not transferred
     */
    virtual CCalEntryView* NormalDbEntryView() = 0;

    /**
     * Accessor for Agenda Model CCalInstanceView owned by this utility.
     * Returns a valid pointer if status is EAvailable or
     * EAvailableWithoutTombs, otherwise NULL.
     * @return instance view pointer, ownership not transferred
     */
    virtual CCalInstanceView* NormalDbInstanceView() = 0;

    /**
     * Accessor for Agenda Model CCalInstanceView owned by this utility.
     * Returns a valid array of pointer if status is EAvailable or
     * EAvailableWithoutTombs, otherwise NULL.
     * @return instance view pointer, ownership not transferred
     */
    virtual RPointerArray<CCalInstanceView> NormalDbAllCalenInstanceView() = 0;

    /**
     * Fetches entries from the Agenda Model based on time range.
     * In case of repeating entries the entire sequence must
     * be within range (but modifying entries are considered separately).
     * Ownership of entries is transferred to caller.
     * See CCalEntryView documentation for other fetch variants.
     * @param aCalEntryArray will contain fetched entries
     * @param aTimeRange criteria
     */
    virtual void FetchEntriesL(
            RPointerArray<CCalEntry>& aCalEntryArray,
            const CalCommon::TCalTimeRange& aTimeRange ) = 0;

    /**
     * Fetches entry from the Agenda Model based on Uid and recurrence id.
     * See CCalEntryView documentation for other fetch variants. For fetching
     * originating entry use TCalTime set to Time::NullTTime().
     * Returns NULL if no matching entry was found.
     * @param aUid criteria
     * @param aRecurrenceId criteria
     * @return entry, ownership is transferred to caller
     */
    virtual CCalEntry* FetchEntryL(
            const TDesC8& aUid,
            const TCalTime& aRecurrenceId ) = 0;

    /**
     * Stores the given entry in the Agenda Model. Entry is either
     * stored to normal calendar DB or to tombstone calendar DB depending
     * from input parameters.
     * @param aCalEntry entry to be stored.
     * @parma aToNormalDb ETrue  --> stored to normal DB.
     *                    EFalse --> stored to tombstone calendar DB.
     * @return KErrNone or a system wide error code.
     */
    virtual TInt StoreEntryL(
            CCalEntry& aCalEntry,
            TBool aToNormalDb = ETrue ) = 0;

    /**
     * Updates the given entry in the Agenda Model.
     * @param aCalEntry entry to update
     * @return KErrNone or a system wide error code
     */
    virtual TInt UpdateEntryL(
            const CCalEntry& aCalEntry ) = 0;

    /**
     * Deletes the given entry from the Agenda Model.
     * @param aUid Entry uid to be deleted
     * @return KErrNone or a system wide error code
     */
    virtual TInt DeleteEntryL(
            const TDesC8& aUid, TInt aCalenIndex ) = 0;

    /**
     * Deletes the given entry from the Agenda Model.
     * @param aLocalUid Entry uid to be deleted
     * @return KErrNone or a system wide error code
     */
    virtual TInt DeleteEntryL(
            const TCalLocalUid& aLocalUid, TInt aCalenIndex ) = 0;

    /**
     * Check entry: GUID, sequence number, existence in Agenda Model and
     * tombstone database. Same as CMRUtilsTombsExt::StoreL() but doesn't
     * actually store.
     * @param aCalEntry entry to be stored
     * @param aResurrectif ETrue then entry stored even if it has been
     * previously deleted
     * @param aCheckOnly
     * @return result code
     */
    virtual MESMRUtilsTombsExt::TESMRUtilsDbResult StoreEntryCondL(
            CCalEntry& aCalEntry,
            TBool aResurrect,
            TBool aCheckOnly = EFalse ) = 0;

    /**
     * Delete entry from Agenda Model if found
     * This method causes deletion of all entries with given GUID.
     * @param aUid determines entry to be deleted
     */
    virtual void DeleteEntryCondL(
            const TDesC8& aUid ) = 0;

    /**
     * Delete entry from Agenda Model if found.
     * This method causes deletion of a modifying entry, or entire
     * recurring event if aLocalUid corresponds to an originating entry.
     * @param aLocalUid determines entry to be deleted
     */
    virtual void DeleteEntryCondL(
            const TCalLocalUid& aLocalUid ) = 0;

    /**
     * Delete entry from Agenda Model if found.
     * This method causes deletion of a modifying entry, or entire
     * recurring event if aCalEntry is an originating entry.
     * @param aCalEntry entry to be deleted
     */
    virtual void DeleteEntryCondL(
            const CCalEntry& aCalEntry ) = 0;

    /**
     * Delete entries from Agenda Model if found.
     * This is asynchronous, user gets notified with
     * MMRUtilsObserver::HandleOperation( EDeleteEntries,... )
     * @param aCalTimeRange determines entries to be deleted
     */
    virtual void DeleteEntryCondL(
            const CalCommon::TCalTimeRange& aCalTimeRange ) = 0;

    /**
     * Fetches the calendar session associated with this
     * cal db manager.
     * @return Reference to calendar server session.
     */
    virtual CCalSession& CalSession() = 0;

    /**
     * Finds matching calendar instance from calendar db. Ownership is
     * transferred to caller.
     * If instance cannot be found, NULL is returned.
     *
     * @param aCalDb Reference to cal db manager.
     * @param aEntry Reference to calendar entry
     * @return Pointer to calendar entry instance.
     */
    virtual CCalInstance* FindInstanceL(
            const CCalEntry& aEntry ) = 0;

    /**
     * Get calendar's color by entry
     * @param aEntry Reference to calendar entry
     * @return calendar's color rgb value
     */
    virtual TRgb GetCalendarColorByEntryL( MESMRCalEntry& aEntry ) = 0;

    /**
     * Get multi-calendar name list
     * @param aCalendarNameList Reference to name string array
     */
    virtual void GetMultiCalendarNameListL( RArray<TPtrC>& aCalendarNameList ) = 0;

    /**
     * Get calendar's name by entry
     * @param aEntry Reference to calendar entry
     * @return calendar's name
     */
    virtual TPtrC GetCalendarNameByEntryL( MESMRCalEntry& aEntry ) = 0;

    /**
     * Get calendar's color by calendar name
     * @param aCalendarName Reference to calendar name
     * @return calendar's color rgb value
     */
    virtual TRgb GetCalendarColorByNameL( const TDesC& aCalendarName ) = 0;

    /**
     * Set current calendar by pass calendar index to it
     * @param aIndex current calendar index
     */
    virtual void SetCurCalendarByIndex( TInt aIndex ) = 0;

    /**
     * Set current calendar by pass calendar name to it
     * @param aCalendarName current calendar name
     */
    virtual void SetCurCalendarByNameL( const TDesC& aCalendarName ) = 0;

    /**
     * Set current calendar by pass entry
     * @param aColId current calendar collectionID
     */
    virtual void SetCurCalendarByColIdL( TInt aColId ) = 0;

    /**
     * Set current calendar by pass entry
     * @param aEntry entry which belongs current calendar
     */
    virtual void SetCurCalendarByEntryL( MESMRCalEntry& aEntry ) = 0;

    /**
     * Sets current calendar by calendar database file id.
     * @param aDbId calendar database file id to use
     */
    virtual void SetCurCalendarByDbIdL( TCalFileId aDbId ) = 0;

    /*
     * Get color of current calendar
     * @return current calendar's color(rgb value)
     */
    virtual TRgb GetCurCalendarColor() = 0;

    /*
     * Get colId of current calendar
     * @return current calendar's colId
     */
    virtual TInt GetCurCalendarColIdL() = 0;

    /*
     * Get index of current calendar
     * @return current calendar's index
     */
    virtual TInt GetCurCalendarIndex() = 0;

    /*
     * Get entryview by CCalEntry
     * @param aCalEntry use to get calendar which aCalEntry belongs
     * @return CCalEntryView aCalEntry belongs
     */
    virtual CCalEntryView* EntryViewL(const CCalEntry& aCalEntry ) = 0;

    /*
     * Get instanceview by CCalEntry
     * @param aCalEntry use to get calendar which aCalEntry belongs
     * @return a CCalInstanceView that aCalEntry belongs
     */
    virtual CCalInstanceView* InstanceViewL(const CCalEntry& aCalEntry ) = 0;

    /*
     * Get current entryview
     * @return current entryView
     */
    virtual CCalEntryView* EntryView() = 0;
    };

/**
 * Manages two databases: normal Agenda db and tombstone db, and
 * handles command logic related to these databases. This class is
 * also responsible for collecting both database status information
 * and async command progress, and to notify aObserver.
 *
 * @lib esmrdb.lib
 */
NONSHARABLE_CLASS(CESMRCalDbMgr) :
        public CBase,
        public MESMRCalDbMgr,
        public MESMRCalDbObserver
    {
public: // Constructors and destructors
    /**
     * Symbian two-phased constructor.
     * @aCalSession calendar session reference
     * @aObserver utility observer reference
     */
    IMPORT_C static CESMRCalDbMgr* NewL(
            CCalSession& aCalSession,
            MMRUtilsObserver& aObserver );

    /**
     * Destructor.
     */
    ~CESMRCalDbMgr();

protected: // From MESMRUtilsCalDbObserver

    void HandleCalDbStatus(
            const CESMRCalDbBase* aNotifier,
            MESMRCalDbObserver::TDbStatus aStatus );

protected: // From MCalProgressCallBack

    void Progress(
            TInt aPercentageCompleted );
    TBool NotifyProgress();
    void Completed(
            TInt aError );

protected: // From MESMRUtilsCalDbMgr

    CCalEntryView* NormalDbEntryView();
    CCalInstanceView* NormalDbInstanceView();
    RPointerArray<CCalInstanceView> NormalDbAllCalenInstanceView();
    void FetchEntriesL(
            RPointerArray<CCalEntry>& aCalEntryArray,
            const CalCommon::TCalTimeRange& aTimeRange );
    CCalEntry* FetchEntryL(
            const TDesC8& aUid,
            const TCalTime& aRecurrenceId );
    TInt StoreEntryL(
            CCalEntry& aCalEntry,
            TBool aToNormalDb = ETrue );
    TInt UpdateEntryL(
            const CCalEntry& aCalEntry );
    TInt DeleteEntryL(
            const TDesC8& aUid, TInt aCalenIndex );
    TInt DeleteEntryL(
            const TCalLocalUid& aLocalUid, TInt aCalenIndex );
    MESMRUtilsTombsExt::TESMRUtilsDbResult StoreEntryCondL(
            CCalEntry& aCalEntry,
            TBool aResurrect,
            TBool aCheckOnly = EFalse );
    MESMRUtilsTombsExt::TESMRUtilsDbResult CheckEntryCondL(
            const CCalEntry& aCalEntry );
    void DeleteEntryCondL(
            const TDesC8& aUid );
    void DeleteEntryCondL(
            const TCalLocalUid& aLocalUid );
    void DeleteEntryCondL(
            const CCalEntry& aCalEntry );
    void DeleteEntryCondL(
            const CalCommon::TCalTimeRange& aCalTimeRange );
    CCalSession& CalSession();
    CCalInstance* FindInstanceL(
            const CCalEntry& aEntry );

    TRgb GetCalendarColorByEntryL(
            MESMRCalEntry& aEntry);
    void GetMultiCalendarNameListL(
            RArray<TPtrC>& aCalendarNameList);
    TPtrC GetCalendarNameByEntryL(
            MESMRCalEntry& aEntry);
    TRgb GetCalendarColorByNameL(
            const TDesC& aCalendarName);
    void SetCurCalendarByNameL(
            const TDesC& aCalendarName );
    void SetCurCalendarByIndex(
            TInt aIndex );
    void SetCurCalendarByColIdL(
            TInt aColId );
    void SetCurCalendarByEntryL(
            MESMRCalEntry& aEntry );
    void SetCurCalendarByDbIdL( TCalFileId aDbId );
    TRgb GetCurCalendarColor();
    TInt GetCurCalendarColIdL();
    TInt GetCurCalendarIndex();
    CCalEntryView* EntryViewL(const CCalEntry& aCalEntry );
    CCalInstanceView* InstanceViewL(const CCalEntry& aCalEntry );
    CCalEntryView* EntryView();

protected: // New functions
    /**
     * Tests whether entry with same GUID and RECURRENCE-ID exists
     * in aDb. If yes, then aIndex will tell it's position in aCalEntryArray
     * which contains all entries with the same GUID.
     * @param aUid first identity criterion
     * @param aRecurrenceId second identity criterion
     * @param aDb database where to look
     * @param aCalEntryArray contains all entries with same GUID
     * @param aIndex tells position of entry, if method returned ETrue
     * @return ETrue if entry exists in aDb
     */
    TBool EntryExistsInDbL(
            const TDesC8& aUid,
            const TCalTime& aRecurrenceId,
            const CESMRCalDbBase& aDb,
            RPointerArray<CCalEntry>& aCalEntryArray,
            TInt& aIndex ) const;
    /**
     * Tests whether entry with same GUID and RECURRENCE-ID exists
     * in aDb. If yes, then aIndex will tell it's position in aCalEntryArray
     * which contains all entries with the same GUID.
     * @param aEntry, whose GUID and RECURRENCE-ID are compared
     * @param aDb database where to look
     * @param aCalEntryArray contains all entries with same GUID
     * @param aIndex tells position of entry, if method returned ETrue
     * @return ETrue if entry exists in aDb
     */
    TBool EntryExistsInDbL(
            const CCalEntry& aEntry,
            const CESMRCalDbBase& aDb,
            RPointerArray<CCalEntry>& aCalEntryArray,
            TInt& aIndex ) const;

    /**
     * Helper method for evaluating validity and status of entry
     * in relation to an another entry which exists in a database.
     * @param aEntry entry to be evaluated
     * @param aDbEntry corresponding entry existing in a database
     * @return entry evaluation result
     */
    MESMRUtilsTombsExt::TESMRUtilsDbResult EvaluateExistingEntryL(
            const CCalEntry& aEntry,
            const CCalEntry& aDbEntry ) const;

    /**
     * Helper method for evaluating validity and status of a new entry.
     * @aEntry entry to be evaluated
     * @return entry evaluation result
     */
    MESMRUtilsTombsExt::TESMRUtilsDbResult EvaluateNewEntryL(
            const CCalEntry& aEntry ) const;

    /**
     * Helper method which checks if given child entry would be a
     * valid new modifying entry, i.e. if it has a recurrence id
     * which matches to an existing instance.
     * @param aEntry entry to be evaluated
     * @return ETrue if valid
     */
    TBool IsValidNewModL(
            const CCalEntry& aEntry ) const;

    /**
     * Evaluates if corresponding originating entry exists in database.
     * @param aEntry
     * @return ETrue if originating entry exists in database
     */
    TBool OriginatingExistInDbL(
            const CCalEntry& aModEntry );

    /**
     * Helper method for deleting a tombstone entry.
     * @param aEntry entry to be deleted
     */
    void DeleteTombstoneL(
            const CCalEntry& aEntry );

protected: // Constructors and destructors

    /**
     * C++ default constructor.
     * @param aCalSession calendar session reference
     * @param aObserver utility observer reference
     */
    CESMRCalDbMgr(
            CCalSession& aCalSession,
            MMRUtilsObserver& aObserver );

    /**
     *  Constructor, second phase.
     */
    void ConstructL();

private:
    TBool CheckSpaceBelowCriticalLevelL();
    /**
     * Load multi calendar info to dbmgr
     */
    void LoadMultiCalenInfoL();

protected: // data

    /**
    * Currently ongoing asynchronous utils operation
    */
    TInt iCurrentAsyncOp;

    /**
    * Calendar session reference
    * Not own.
    */
    CCalSession& iCalSession;

    /**
     * Calendar session array, support multi calendar
     * Own.
     */
    RPointerArray<CCalSession> iCalSessionArray;

    /**
     * CCalEntryView array, support multi calendar
     * Own.
     */
    RPointerArray<CCalEntryView> iCalEntryViewArray;

    /**
     * CCalInstanceView array, support multi calendar
     * Own.
     */
    RPointerArray<CCalInstanceView> iCalInstanceViewArray;

    /**
     * CCalCalendarInfo array, support multi calendar
     * Own.
     */
    RPointerArray<CCalCalendarInfo> iCalendarInfoArray;

    /**
    * Reference to cal db observer
    *
    */
    MMRUtilsObserver& iObserver;

    /**
    * Normal agenda database
    * Own.
    */
    CESMRCalDbBase* iNormalDb;

    /**
    * Handles asynchronous operation iteration
    * Own.
    */
    CESMREntryCmdIteratorAO* iCmdIterator;

private:
    TInt iCurCalenIndex;
    };

#endif // CESMRCALDBMGR_H
