/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CESMRCalDbMgr.
*
*/


//INCLUDE FILES
#include "emailtrace.h"
#include "cesmrcaldbmgr.h"
#include "cesmrcaldbnormal.h"
#include "cesmrentrycmditeratorao.h"
#include "esmrhelper.h"
#include "esmrentryhelper.h"
//<cmail>
#include "mesmrutilstombsext.h"
//</cmail>

// From System
#include <ct/rcpointerarray.h>
#include <calsession.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calentry.h>
#include <calcommon.h>
#include <caluser.h>
#include <CalenInterimUtils2.h>

// CONSTANTS

// Unnamed namespace for local definitions
namespace {

const TInt KDbInitReady = 100;

#ifdef _DEBUG

// Panic code definitions
enum TPanicCode
    {
    EPanicIllegalFetchParams = 1,
    EPanicAccessedWhenUnavailable,
    EPanicAsyncOpAlreadyExists,
    EPanicIllegalEntryStatus,
    EPanicIllegalResurrect,
    EPanicUnexpectedUidValue
    };

// Panic string definition
_LIT( KPanicMsg, "CESMRCalDbMgr" );

/**
 * Raises system panic
 * @param aReason Panic reason.
 */
void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

#endif // _DEBUG

}  // namespace


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CESMRCalDbMgr* CESMRCalDbMgr::NewL(
    CCalSession& aCalSession,
    MMRUtilsObserver& aObserver )
    {
    FUNC_LOG;
    CESMRCalDbMgr* self =
            new( ELeave ) CESMRCalDbMgr(
                aCalSession,
                aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();


    return self;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::CESMRCalDbMgr
//
// Constructor.
// ----------------------------------------------------------------------------
//
CESMRCalDbMgr::CESMRCalDbMgr(
    CCalSession& aCalSession,
    MMRUtilsObserver& aObserver )
    : iCurrentAsyncOp( CMRUtils::ENoOperation ),
      iCalSession( aCalSession ),
      iObserver( aObserver )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::~CESMRCalDbMgr
//
// Destructor.
// ----------------------------------------------------------------------------
//
CESMRCalDbMgr::~CESMRCalDbMgr()
    {
    FUNC_LOG;
    delete iCmdIterator;
    delete iNormalDb;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::ConstructL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::ConstructL()
    {
    FUNC_LOG;

    iNormalDb    = CESMRCalDbNormal::NewL( iCalSession, *this, *this );
    iCmdIterator = CESMREntryCmdIteratorAO::NewL( *this );

    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::HandleCalDbStatus
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::HandleCalDbStatus(
    const CESMRCalDbBase* /*aNotifier*/,
    MESMRCalDbObserver::TDbStatus aStatus )
    {
    FUNC_LOG;

    if ( aStatus == EReseting )
        { // reset db is ongoing, engine becomes unavailable
        iObserver.HandleCalEngStatus( MMRUtilsObserver::ENotReady );
        }
    else if ( iNormalDb->DbStatus() == MESMRCalDbObserver::EFinishedOk )
        {
        iObserver.HandleCalEngStatus( MMRUtilsObserver::EAvailable );
        }
    else if ( iNormalDb->DbStatus() == MESMRCalDbObserver::EFinishedOk )
        {
        iObserver.HandleCalEngStatus(
            MMRUtilsObserver::EAvailableWithoutTombs );
        }
    else if ( iNormalDb->DbStatus() == MESMRCalDbObserver::EFinishedError )
        {
        iObserver.HandleCalEngStatus( MMRUtilsObserver::ENotAvailable );
        }


    // In other cases either db is not yet finished
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::Progress
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::Progress( TInt /*aPercentageCompleted*/ )
    {
    FUNC_LOG;
    // Not interested in progress currently
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::NotifyProgress
// ----------------------------------------------------------------------------
//
TBool CESMRCalDbMgr::NotifyProgress()
    {
    FUNC_LOG;
    // Not interested in progress currently
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::Completed
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::Completed( TInt aError )
    {
    FUNC_LOG;

    iObserver.HandleOperation( iCurrentAsyncOp, KDbInitReady, aError );
    iCurrentAsyncOp = CMRUtils::ENoOperation;

    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::NormalDbEntryView
// ----------------------------------------------------------------------------
//
CCalEntryView* CESMRCalDbMgr::NormalDbEntryView()
    {
    FUNC_LOG;
    return iNormalDb->EntryView();
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::NormalDbInstanceView
// ----------------------------------------------------------------------------
//
CCalInstanceView* CESMRCalDbMgr::NormalDbInstanceView()
    {
    FUNC_LOG;
    return iNormalDb->InstanceView();
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::FetchEntriesL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::FetchEntriesL(
    RPointerArray<CCalEntry>& aCalEntryArray,
    const CalCommon::TCalTimeRange& aTimeRange )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( aCalEntryArray.Count() == 0,
                    Panic( EPanicIllegalFetchParams ) );
    aCalEntryArray.ResetAndDestroy();
    // caller definitely wants to get full copies -> ETrue as 3rd argument:
    iNormalDb->FetchWithRangeL( aCalEntryArray,
                                aTimeRange,
                                ETrue );

    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::FetchEntryL
// ----------------------------------------------------------------------------
//
CCalEntry* CESMRCalDbMgr::FetchEntryL(
    const TDesC8& aUid,
    const TCalTime& aRecurrenceId )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( aUid.Length() > 0,
                    Panic( EPanicIllegalFetchParams ) );

    RCPointerArray<CCalEntry> tmpFetchArray;
    CleanupClosePushL( tmpFetchArray );

    CCalEntry* retVal = NULL;

    TInt index( -1 );
    if ( EntryExistsInDbL( aUid,
                           aRecurrenceId,
                           *iNormalDb,
                           tmpFetchArray,
                           index ) )
        {
        retVal = tmpFetchArray[index];
        tmpFetchArray.Remove( index ); // ownership transferred to retVal
        }

    CleanupStack::PopAndDestroy(); // tmpFetchArray


    return retVal;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::StoreEntryL
// ----------------------------------------------------------------------------
//
TInt CESMRCalDbMgr::StoreEntryL(
    CCalEntry& aCalEntry,
    TBool aToNormalDb )
    {
    FUNC_LOG;

    TInt retVal( KErrNone );

    if(aToNormalDb)
        {
        CCalenInterimUtils2::StoreL( *( iNormalDb->EntryView() ),
                                 aCalEntry,
                                 ETrue );

        }



    return retVal;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::UpdateEntryL
// ----------------------------------------------------------------------------
//
TInt CESMRCalDbMgr::UpdateEntryL( const CCalEntry& aCalEntry )
    {
    FUNC_LOG;

    RPointerArray<CCalEntry> calEntryTmpArray( 1 ); // entry not own
    CleanupClosePushL( calEntryTmpArray );
    calEntryTmpArray.AppendL( &aCalEntry );
    TInt numSuccessfulEntry( 0 );
    // entry view surely exists when status is 'available':
    iNormalDb->EntryView()->UpdateL( calEntryTmpArray, numSuccessfulEntry );
    TInt retVal( numSuccessfulEntry == 1 ? KErrNone : KErrGeneral );
    CleanupStack::PopAndDestroy(); // calEntryTmpArray, only close array


    return retVal;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::DeleteEntryL
// ----------------------------------------------------------------------------
//
TInt CESMRCalDbMgr::DeleteEntryL( const TDesC8& aUid )
    {
    FUNC_LOG;

    CDesC8ArrayFlat* uidArray = new( ELeave ) CDesC8ArrayFlat( 1 );
    CleanupStack::PushL( uidArray );
    uidArray->AppendL( aUid );
    // entry view surely exists when status is 'available':
    iNormalDb->EntryView()->DeleteL( *uidArray );
    CleanupStack::PopAndDestroy( uidArray );


    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::DeleteEntryL
// ----------------------------------------------------------------------------
//
TInt CESMRCalDbMgr::DeleteEntryL( const TCalLocalUid& aLocalUid )
    {
    FUNC_LOG;

    RArray<TCalLocalUid> localUidArray( 1 );
    CleanupClosePushL( localUidArray );
    localUidArray.AppendL( aLocalUid );
    TInt numSuccessfulEntry( 0 );
    iNormalDb->EntryView()->DeleteL( localUidArray, numSuccessfulEntry );
    TInt retVal( numSuccessfulEntry == 1 ? KErrNone : KErrGeneral );
    CleanupStack::PopAndDestroy(); // localUidArray


    return retVal;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::StoreEntryCondL
// aResurrect can only be ETrue if aCheckOnly is EFalse, i.e. when user is
// really trying to store something.
// ----------------------------------------------------------------------------
//
MESMRUtilsTombsExt::TESMRUtilsDbResult CESMRCalDbMgr::StoreEntryCondL(
    CCalEntry& aCalEntry,
    TBool aResurrect,
    TBool aCheckOnly )
    {
    FUNC_LOG;

    RCPointerArray<CCalEntry> tmpFetchArray;
    CleanupClosePushL( tmpFetchArray );

    MESMRUtilsTombsExt::TESMRUtilsDbResult entryStatus(
        MESMRUtilsTombsExt::EUndefined );
    TInt index( -1 );

    // 1. Normal db part, if entry is found in there then tombstones aren't
    //    checked at all:

    if ( EntryExistsInDbL( aCalEntry, *iNormalDb, tmpFetchArray, index ) )
        { // Entry exists in normal db
        entryStatus = EvaluateExistingEntryL( aCalEntry,
                                              *( tmpFetchArray[index] ) );
        if ( entryStatus == MESMRUtilsTombsExt::ECheckedValidUpdate &&
             !aCheckOnly )
            {
            StoreEntryL( aCalEntry );
            entryStatus = MESMRUtilsTombsExt::EStoredUpdate;
            }
        else if ( entryStatus == MESMRUtilsTombsExt::EErrorCancelled &&
                  aResurrect )
            {
            __ASSERT_DEBUG( !aCheckOnly, Panic( EPanicIllegalResurrect ) );
            StoreEntryL( aCalEntry );
            entryStatus = MESMRUtilsTombsExt::EResurrectedCancelled;
            }
        // entry can't be new if it exists in the db already:
        __ASSERT_DEBUG( entryStatus != MESMRUtilsTombsExt::ECheckedValidNew,
                        Panic( EPanicIllegalEntryStatus) );
        }

    // 2. New entry in this phone:

    else
        { // Completely new entry (or tombstone has disappeared)
        entryStatus = EvaluateNewEntryL( aCalEntry );
        if ( entryStatus == MESMRUtilsTombsExt::ECheckedValidNew && !aCheckOnly )
            {
            StoreEntryL( aCalEntry );
            entryStatus = MESMRUtilsTombsExt::EStoredNew;
            }

        // entry can't be update if doesn't exist in the db already:
        __ASSERT_DEBUG( entryStatus != MESMRUtilsTombsExt::ECheckedValidUpdate,
                        Panic( EPanicIllegalEntryStatus) );
        }

    CleanupStack::PopAndDestroy(); // tmpFetchArray


    return entryStatus;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::DeleteEntryCondL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::DeleteEntryCondL( const TDesC8& aUid )
    {
    FUNC_LOG;

    RCPointerArray<CCalEntry> tmpFetchArray;
    CleanupClosePushL( tmpFetchArray );

    // 1. Delete all found entries from normal db:

    iNormalDb->EntryView()->FetchL( aUid, tmpFetchArray );
    DeleteEntryL( aUid );

    CleanupStack::PopAndDestroy(); // tmpFetchArray

    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::DeleteEntryCondL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::DeleteEntryCondL( const TCalLocalUid& aLocalUid )
    {
    FUNC_LOG;

    CCalEntry* entry = iNormalDb->EntryView()->FetchL( aLocalUid );
    if ( entry )
        {
        CleanupStack::PushL( entry );
        DeleteEntryCondL( *entry );
        CleanupStack::PopAndDestroy( entry );
        }

    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::DeleteEntryCondL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::DeleteEntryCondL( const CCalEntry& aCalEntry )
    {
    FUNC_LOG;

    if ( ESMREntryHelper::IsModifyingEntryL( aCalEntry ) )
        {
        // Modifying entry
        // 1. Delete entry from normal db:
        iNormalDb->EntryView()->DeleteL( aCalEntry );
        }

    else
        { // Originating entry, this is the same case as deleting with GUID:
        DeleteEntryCondL( aCalEntry.UidL() );
        }

    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::DeleteEntryCondL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::DeleteEntryCondL(
    const CalCommon::TCalTimeRange& aCalTimeRange )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iCurrentAsyncOp == CMRUtils::ENoOperation,
                    Panic( EPanicAsyncOpAlreadyExists ) );

    RCPointerArray<CCalEntry> tmpFetchArray;
    CleanupClosePushL( tmpFetchArray );
    // skeleton copies are enough -> set 3rd argument to EFalse:
    iNormalDb->FetchWithRangeL( tmpFetchArray, aCalTimeRange, EFalse );
    iCmdIterator->StartCmdIterationL( tmpFetchArray, CMRUtils::EDeleteEntries );
    iCurrentAsyncOp = CMRUtils::EDeleteEntries;
    tmpFetchArray.Reset(); // ownership of entries was transferred
    CleanupStack::PopAndDestroy(); // tmpFetchArray

    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::DeleteEntryCondL
// ----------------------------------------------------------------------------
//
CCalSession& CESMRCalDbMgr::CalSession()
    {
    FUNC_LOG;
    return iCalSession;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::EntryExistsInDbL
// ----------------------------------------------------------------------------
//
TBool CESMRCalDbMgr::EntryExistsInDbL(
    const TDesC8& aUid,
    const TCalTime& aRecurrenceId,
    const CESMRCalDbBase& aDb,
    RPointerArray<CCalEntry>& aCalEntryArray,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    TBool retVal( EFalse );
    aIndex = KErrNotFound;

    aDb.EntryView()->FetchL( aUid, aCalEntryArray );
    TInt count( aCalEntryArray.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        const CCalEntry& dbEntry( *( aCalEntryArray[i] ) );
        if ( aRecurrenceId.TimeUtcL() == dbEntry.RecurrenceIdL().TimeUtcL() )
            { // Entry was found in the db,
              // it may be either originating or modifying entry
            retVal = ETrue;
            aIndex = i;
            }
        }


    return retVal;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::EntryExistsInDbL
// ----------------------------------------------------------------------------
//
inline TBool CESMRCalDbMgr::EntryExistsInDbL(
    const CCalEntry& aEntry,
    const CESMRCalDbBase& aDb,
    RPointerArray<CCalEntry>& aCalEntryArray,
    TInt& aIndex ) const
    {
    FUNC_LOG;
    return EntryExistsInDbL( aEntry.UidL(),
                             aEntry.RecurrenceIdL(),
                             aDb,
                             aCalEntryArray,
                             aIndex );
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::EvaluateExistingEntryL
// ----------------------------------------------------------------------------
//
MESMRUtilsTombsExt::TESMRUtilsDbResult CESMRCalDbMgr::EvaluateExistingEntryL(
    const CCalEntry& aEntry,
    const CCalEntry& aDbEntry ) const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( aEntry.UidL() == aEntry.UidL(),
                    Panic( EPanicUnexpectedUidValue ) );
    MESMRUtilsTombsExt::TESMRUtilsDbResult retVal( MESMRUtilsTombsExt::EUndefined );

    TInt seq( aEntry.SequenceNumberL() );
    TInt dbSeq( aDbEntry.SequenceNumberL() );
    TTime stamp = aEntry.DTStampL().TimeUtcL();
    TTime dbStamp = aDbEntry.DTStampL().TimeUtcL();

    if ( ( seq >= dbSeq ) && ( stamp >= dbStamp ) )
        {
        if ( ( seq == dbSeq ) && ( stamp == dbStamp ) )
            {
            retVal = MESMRUtilsTombsExt::EErrorIdenticalExists;
            }

        else if ( aDbEntry.StatusL() == CCalEntry::ECancelled )
            {
            retVal = MESMRUtilsTombsExt::EErrorCancelled;
            }
        else
            {
            retVal = MESMRUtilsTombsExt::ECheckedValidUpdate;
            }
        }
    else
        {
        retVal = MESMRUtilsTombsExt::EErrorObsolete;
        }


    return retVal;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::EvaluateNewEntryL
// This method is only used with iNormalDb
// ----------------------------------------------------------------------------
//
MESMRUtilsTombsExt::TESMRUtilsDbResult CESMRCalDbMgr::EvaluateNewEntryL(
    const CCalEntry& aEntry ) const
    {
    FUNC_LOG;

    MESMRUtilsTombsExt::TESMRUtilsDbResult retVal( MESMRUtilsTombsExt::EUndefined );

    if ( ESMREntryHelper::IsModifyingEntryL( aEntry ) &&
         !IsValidNewModL( aEntry ) )
        { // recurrence id does not match to any instance of an entry
        retVal = MESMRUtilsTombsExt::EErrorRecurrence;
        }
    else
        {
        retVal = MESMRUtilsTombsExt::ECheckedValidNew;
        }


    return retVal;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::EvaluateNewEntryL
// This method is only used with iNormalDb
// ----------------------------------------------------------------------------
//
TBool CESMRCalDbMgr::IsValidNewModL( const CCalEntry& aModEntry ) const
    {
    FUNC_LOG;

    TBool retVal( EFalse );
    RCPointerArray<CCalInstance> instances; // takes ownership
    CleanupClosePushL( instances );
    TCalTime recurrenceId( aModEntry.RecurrenceIdL() );
    CalCommon::TCalTimeRange range( recurrenceId, recurrenceId );

    iNormalDb->InstanceView()->FindInstanceL( instances,
                                              CalCommon::EIncludeAppts,
                                              range );
    TInt count( instances.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        // When creating a new modifying entry, recurrence id must match
        // to an existing instance start time (later on start time may get
        // modified whereas recurrence id remains unchanged).
        if ( instances[i]->Entry().UidL() == aModEntry.UidL() &&
             instances[i]->StartTimeL().TimeUtcL() == recurrenceId.TimeUtcL() )
            {
            retVal = ETrue;
            break; // match found, ready to return
            }
        }
    CleanupStack::PopAndDestroy(); // instances, delete array items


    return retVal;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::OriginatingExistInDbL
// This method is only used with iNormalDb
// ----------------------------------------------------------------------------
//
TBool CESMRCalDbMgr::OriginatingExistInDbL(
        const CCalEntry& aModEntry )
    {
    FUNC_LOG;

    TInt retVal( EFalse );
    CCalEntry* dbEntry = FetchEntryL( aModEntry.UidL(), TCalTime() );
    if ( dbEntry )
        {
        delete dbEntry;
        retVal = ETrue;
        }


    return retVal;
    }

// End of file

