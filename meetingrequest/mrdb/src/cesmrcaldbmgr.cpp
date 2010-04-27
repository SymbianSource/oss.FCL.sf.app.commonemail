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
#include "mesmrcalentry.h"
#include "mesmrutilstombsext.h"
#include "mruidomaincrkeys.h"

// From System
#include <ct/rcpointerarray.h>
#include <calsession.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calentry.h>
#include <calcommon.h>
#include <caluser.h>
#include <caleninterimutils2.h>
#include <sysutil.h>
#include <errorui.h>
#include <coemain.h>
#include <calcalendarinfo.h>
#include <centralrepository.h>

// CONSTANTS

// Unnamed namespace for local definitions
namespace {

const TInt KDbInitReady( 100 );

/**
 * Reads last used database index from central repository
 *
 * @param aIndex on return contains the last used database index
 */
void ReadDatabaseIndexL( TInt& aIndex )
    {
    FUNC_LOG;

    CRepository* repository = CRepository::NewLC( KCRUidESMRUIFeatures );
    User::LeaveIfError( repository->Get( KMRUIDefaultCalDbIndex, aIndex ) );
    CleanupStack::PopAndDestroy( repository );
    }

/**
 * Writes used database index into central repository
 *
 * @param database index
 */
void WriteDatabaseIndexL( TInt aIndex )
    {
    FUNC_LOG;

    CRepository* repository = CRepository::NewLC( KCRUidESMRUIFeatures );
    User::LeaveIfError( repository->Set( KMRUIDefaultCalDbIndex, aIndex ) );
    CleanupStack::PopAndDestroy( repository );
    }


// Panic code definitions
enum TPanicCode
    {
    EPanicIllegalFetchParams = 1,
    EPanicAccessedWhenUnavailable,
    EPanicAsyncOpAlreadyExists,
    EPanicIllegalEntryStatus,
    EPanicIllegalResurrect,
    EPanicUnexpectedUidValue,
    EPanicLoadMultiDbInfoFail,
    EPanicInvalidDbIndex
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

    iCalInstanceViewArray.ResetAndDestroy();
    iCalEntryViewArray.ResetAndDestroy();
    iCalendarInfoArray.ResetAndDestroy();
    iCalSessionArray.ResetAndDestroy();

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

    TRAPD( err, LoadMultiCalenInfoL() );
    __ASSERT_ALWAYS( err == KErrNone, Panic( EPanicLoadMultiDbInfoFail ) );
    User::LeaveIfError( err );
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
// CESMRCalDbMgr::NormalDbAllCalenInstanceView
// ----------------------------------------------------------------------------
//
RPointerArray<CCalInstanceView> CESMRCalDbMgr::NormalDbAllCalenInstanceView()
    {
    FUNC_LOG;
    return iCalInstanceViewArray;
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

    if ( CheckSpaceBelowCriticalLevelL() )
        {
        retVal = KErrNoMemory;
        }

    else
        {
        if( aToNormalDb )
            {
            CCalenInterimUtils2::StoreL( *(iCalEntryViewArray[iCurCalenIndex]),
                                     aCalEntry,
                                     ETrue );
            WriteDatabaseIndexL( iCurCalenIndex );
            }
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
    EntryViewL( aCalEntry )->UpdateL( calEntryTmpArray, numSuccessfulEntry );
    TInt retVal( numSuccessfulEntry == 1 ? KErrNone : KErrGeneral );
    CleanupStack::Pop(); // calEntryTmpArray, only close array

    return retVal;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::DeleteEntryL
// ----------------------------------------------------------------------------
//
TInt CESMRCalDbMgr::DeleteEntryL( const TDesC8& aUid, TInt aCalenIndex )
    {
    FUNC_LOG;

    CDesC8ArrayFlat* uidArray = new( ELeave ) CDesC8ArrayFlat( 1 );
    CleanupStack::PushL( uidArray );
    uidArray->AppendL( aUid );
    // entry view surely exists when status is 'available':
    iCalEntryViewArray[aCalenIndex]->DeleteL( *uidArray );
    CleanupStack::PopAndDestroy( uidArray );

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::DeleteEntryL
// ----------------------------------------------------------------------------
//
TInt CESMRCalDbMgr::DeleteEntryL( const TCalLocalUid& aLocalUid, TInt aCalenIndex )
    {
    FUNC_LOG;

    RArray<TCalLocalUid> localUidArray( 1 );
    CleanupClosePushL( localUidArray );
    localUidArray.AppendL( aLocalUid );
    TInt numSuccessfulEntry( 0 );
    iCalEntryViewArray[aCalenIndex]->DeleteL( localUidArray, numSuccessfulEntry );
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
    MESMRUtilsTombsExt::TESMRUtilsDbResult entryStatus(
        MESMRUtilsTombsExt::EUndefined );

    if ( CheckSpaceBelowCriticalLevelL() )
        {
        entryStatus = MESMRUtilsTombsExt::EErrorCancelled;
        }
    else
        {
        RCPointerArray<CCalEntry> tmpFetchArray;
        CleanupClosePushL( tmpFetchArray );

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
        }

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
    TInt count = iCalEntryViewArray.Count();
    for( TInt i(0); i < count; i++ )
        {
        iCalEntryViewArray[i]->FetchL( aUid, tmpFetchArray );
        if( tmpFetchArray.Count() > 0 )
            {
            DeleteEntryL( aUid, i );
            tmpFetchArray.ResetAndDestroy();
            }
        }

    CleanupStack::PopAndDestroy(); // tmpFetchArray
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::DeleteEntryCondL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::DeleteEntryCondL( const TCalLocalUid& aLocalUid )
    {
    FUNC_LOG;

    TInt count = iCalEntryViewArray.Count();
    for( TInt i(0); i < count; i++ )
        {
        CCalEntry* entry = iCalEntryViewArray[i]->FetchL( aLocalUid );
        if ( entry )
            {
            CleanupStack::PushL( entry );
            DeleteEntryCondL( *entry );
            CleanupStack::PopAndDestroy( entry );
            }
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

        CCalInstance* instance = FindInstanceL( (CCalEntry&)aCalEntry );
        CleanupStack::PushL( instance );
        TInt colId = instance->InstanceIdL().iCollectionId;

        TInt index = 0;
        TInt count = iCalSessionArray.Count();
        for( TInt i = 0; i < count; i++ )
            {
            if( colId == iCalSessionArray[i]->CollectionIdL() )
                {
                index = i;
                break;
                }
            }
        iCalEntryViewArray[index]->DeleteL( aCalEntry );
        CleanupStack::PopAndDestroy( instance );
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
// CESMRCalDbMgr::FindInstanceL
// ----------------------------------------------------------------------------
//
CCalInstance* CESMRCalDbMgr::FindInstanceL(
            const CCalEntry& aEntry )
    {
    FUNC_LOG;

    CCalInstance* instance = NULL;
    RCPointerArray<CCalInstance> calInstances;
    CleanupClosePushL( calInstances );

    CalCommon::TCalViewFilter instanceFilter =
                                CalCommon::EIncludeAppts |
                                CalCommon::EIncludeEvents |
                                CalCommon::EIncludeReminder |
                                CalCommon::EIncludeAnnivs |
                                CalCommon::EIncludeCompletedTodos |
                                CalCommon::EIncludeIncompletedTodos;

    // Removing one seconds from start time and adding one second to stop
    // time. Otherwise wanted entry is not included into results.
    TCalTime startTime;
    startTime.SetTimeLocalL(
        aEntry.StartTimeL().TimeLocalL() - TTimeIntervalSeconds( 1 ) );
    TCalTime endTime;
    endTime.SetTimeLocalL(
        aEntry.EndTimeL().TimeLocalL() + TTimeIntervalSeconds( 1 ) );

    TDateTime start = startTime.TimeLocalL().DateTime();
    TDateTime end   = endTime.TimeLocalL().DateTime();

    CalCommon::TCalTimeRange timeRange(
            startTime,
            endTime );


    TInt count = iCalInstanceViewArray.Count();
    for( TInt i = 0; i < count && !instance; i++ )
        {
        iCalInstanceViewArray[i]->FindInstanceL(
                    calInstances,
                    instanceFilter,
                    timeRange);
        if( calInstances.Count() > 0 )
            {
            TInt instanceCount( calInstances.Count() );
            for (TInt i = 0; (i < instanceCount && !instance); ++i)
                {
                CCalEntry& entry = calInstances[i]->Entry();

                // Finding the entry we are intrested for
                if ( !entry.UidL().Compare( aEntry.UidL() ) )
                    {
                    instance = calInstances[i];
                    calInstances.Remove( i );
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(); // arrayCleanup
    return instance;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::GetCalendarColorByEntryL
// ----------------------------------------------------------------------------
//
TRgb CESMRCalDbMgr::GetCalendarColorByEntryL(MESMRCalEntry& aEntry)
    {
    FUNC_LOG;
    TInt count = iCalSessionArray.Count();
    TRgb color(0);
    if( count < 1 )
        return color;

    if( !aEntry.IsStoredL() )
        {
        color = iCalendarInfoArray[0]->Color();
        }
    else
        {
        CCalInstance* instance = aEntry.InstanceL();
        CleanupStack::PushL( instance );
        TInt collectionId = instance->InstanceIdL().iCollectionId;

        for( TInt i = 0; i < count; i++ )
            {
            if( collectionId == iCalSessionArray[i]->CollectionIdL() )
                {
                color = iCalendarInfoArray[i]->Color();
                break;
                }
            }

        CleanupStack::PopAndDestroy( instance );
        }

    return color;
    }
// ----------------------------------------------------------------------------
// CESMRCalDbMgr::GetMultiCalendarNameListL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::GetMultiCalendarNameListL(RArray<TPtrC>& aCalendarNameList)
    {
    FUNC_LOG;
    TInt count = iCalendarInfoArray.Count();
    aCalendarNameList.ReserveL( count );

    for( TInt i = 0; i < count; i++ )
        {
        const TDesC& calenName = iCalendarInfoArray[i]->NameL();
        aCalendarNameList.AppendL( TPtrC( calenName ) );
        }
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::GetCalendarNameByEntryL
// ----------------------------------------------------------------------------
//
TPtrC CESMRCalDbMgr::GetCalendarNameByEntryL(MESMRCalEntry& aEntry)
    {
    FUNC_LOG;

    TInt count = iCalSessionArray.Count();

    TRgb color(0);
    TPtrC calenName;

    if( !aEntry.IsStoredL() )
        {
        //while create new entry, use current calendar db
        if( count > 0 )
            {
            calenName.Set(iCalendarInfoArray[iCurCalenIndex]->NameL());
            }
        }
    else
        {
        CCalInstance* instance = NULL;
        TRAPD( err, instance = aEntry.InstanceL() );
        CleanupStack::PushL( instance );
        if( err == KErrNotFound || instance == NULL )
            calenName.Set(iCalendarInfoArray[iCurCalenIndex]->NameL());
        else
            {
            TInt collectionId = instance->InstanceIdL().iCollectionId;
            TBuf<16> val;

            for( TInt i = 0; i < count; i++ )
                {
                if( collectionId == iCalSessionArray[i]->CollectionIdL() )
                    {
                    calenName.Set(iCalendarInfoArray[i]->NameL());
                    break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( instance );
        }

    return calenName;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::GetCalendarColorByNameL
// ----------------------------------------------------------------------------
//
TRgb CESMRCalDbMgr::GetCalendarColorByNameL( const TDesC& aCalendarName )
    {
    FUNC_LOG;
    TInt count = iCalendarInfoArray.Count();
    TRgb color(0);

    for ( TInt i = 0; i < count; i++)
        {
        if( aCalendarName.Compare( iCalendarInfoArray[i]->NameL() ) == 0 )
            {
            color = iCalendarInfoArray[i]->Color();
            }
        }

    return color;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::SetCurCalendarByNameL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::SetCurCalendarByNameL(  const TDesC& aCalendarName )
    {
    FUNC_LOG;
    TInt count = iCalendarInfoArray.Count();

    for( TInt i(0); i < count; i++ )
        {
        if( aCalendarName.Compare( iCalendarInfoArray[i]->NameL() ) == 0 )
            {
            SetCurCalendarByIndex( i );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::SetCurCalendarByColIdL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::SetCurCalendarByColIdL( TInt aColId )
    {
    FUNC_LOG;
    TInt count = iCalSessionArray.Count();

    for( TInt i = 0; i < count; i++ )
        {
        if( aColId == iCalSessionArray[i]->CollectionIdL() )
            {
            SetCurCalendarByIndex( i );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::SetCurCalendarByEntry
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::SetCurCalendarByEntryL( MESMRCalEntry& aEntry )
    {
    TPtrC calenName = GetCalendarNameByEntryL( aEntry );
    SetCurCalendarByNameL( calenName );
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::SetCurCalendarByIndex
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::SetCurCalendarByIndex( TInt aIndex )
    {
    FUNC_LOG;
    iCurCalenIndex = aIndex;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::SetCurCalendarByDbIdL
// ----------------------------------------------------------------------------
//
void CESMRCalDbMgr::SetCurCalendarByDbIdL( TCalFileId aDbId )
    {
    FUNC_LOG;

    for ( TInt i = 0; i < iCalSessionArray.Count(); ++i )
        {
        TCalFileId fileId( KNullFileId );
        iCalSessionArray[ i ]->FileIdL( fileId );

        if ( fileId == aDbId )
            {
            iCurCalenIndex = i;
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::GetCurCalendarColor
// ----------------------------------------------------------------------------
//
TRgb CESMRCalDbMgr::GetCurCalendarColor()
    {
    FUNC_LOG;
    __ASSERT_ALWAYS(
            iCurCalenIndex < iCalendarInfoArray.Count(),
            Panic( EPanicInvalidDbIndex ) );
    return iCalendarInfoArray[iCurCalenIndex]->Color();
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::GetCurCalendarColIdL
// ----------------------------------------------------------------------------
//
TInt CESMRCalDbMgr::GetCurCalendarColIdL()
    {
    FUNC_LOG;
    __ASSERT_ALWAYS(
                iCurCalenIndex < iCalSessionArray.Count(),
                Panic( EPanicInvalidDbIndex ) );

    return iCalSessionArray[iCurCalenIndex]->CollectionIdL();
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::GetCurCalendarIndex
// ----------------------------------------------------------------------------
//
TInt CESMRCalDbMgr::GetCurCalendarIndex()
    {
    return iCurCalenIndex;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::EntryViewL
// ----------------------------------------------------------------------------
//
CCalEntryView* CESMRCalDbMgr::EntryViewL(const CCalEntry& aCalEntry )
    {
    FUNC_LOG;
    CCalInstance* instance = FindInstanceL( aCalEntry );
    if( !instance )
        {
        return NULL;
        }

    CleanupStack::PushL( instance );
    CCalEntryView* entryView = NULL;

    TInt colId = instance->InstanceIdL().iCollectionId;
    TInt count = iCalSessionArray.Count();

    for( TInt i = 0; i < count; i++ )
        {
        if( colId == iCalSessionArray[i]->CollectionIdL() )
            {
            entryView = iCalEntryViewArray[i];
            break;
            }
        }

    if ( !entryView )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( instance );
    return entryView;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::InstanceViewL
// ----------------------------------------------------------------------------
//
CCalInstanceView* CESMRCalDbMgr::InstanceViewL(const CCalEntry& aCalEntry )
    {
    FUNC_LOG;
    CCalInstance* instance = FindInstanceL( aCalEntry );
    if( !instance )
        {
        return NULL;
        }

    CleanupStack::PushL( instance );
    CCalInstanceView* instanceView = NULL;

    TInt colId = instance->InstanceIdL().iCollectionId;
    TInt count = iCalSessionArray.Count();

    for( TInt i = 0; i < count; i++ )
        {
        if( colId == iCalSessionArray[i]->CollectionIdL() )
            {
            instanceView = iCalInstanceViewArray[i];
            break;
            }
        }

    if ( !instanceView )
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy( instance );
    return instanceView;
    }

// ----------------------------------------------------------------------------
// CESMRCalDbMgr::EntryView
// ----------------------------------------------------------------------------
//
CCalEntryView* CESMRCalDbMgr::EntryView()
    {
    FUNC_LOG;
    __ASSERT_ALWAYS(
                    iCurCalenIndex < iCalEntryViewArray.Count(),
                    Panic( EPanicInvalidDbIndex ) );
    return iCalEntryViewArray[iCurCalenIndex];
    }



// ----------------------------------------------------------------------------
// CESMRCalDbMgr::EntryExistsInDbL
// ----------------------------------------------------------------------------
//
TBool CESMRCalDbMgr::EntryExistsInDbL(
    const TDesC8& aUid,
    const TCalTime& aRecurrenceId,
    const CESMRCalDbBase& /*aDb*/,
    RPointerArray<CCalEntry>& aCalEntryArray,
    TInt& aIndex ) const
    {
    FUNC_LOG;

    TBool retVal( EFalse );
    aIndex = KErrNotFound;

    TInt sessionCount = iCalSessionArray.Count();
    for( TInt i = 0; i < sessionCount; i++ )
        {
        iCalEntryViewArray[i]->FetchL( aUid, aCalEntryArray );
        if( aCalEntryArray.Count() > 0)
            {
            break;
            }
        }

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

    __ASSERT_DEBUG( aEntry.UidL() == aDbEntry.UidL(),
                    Panic( EPanicUnexpectedUidValue ) );

    MESMRUtilsTombsExt::TESMRUtilsDbResult retVal(
            MESMRUtilsTombsExt::EUndefined );

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

    // The entry can also be in other calendar than the default one,
    // so the instance view array should be checked in this case.
    if( count == 0 )
    	{
		TInt instanceViewCount = iCalInstanceViewArray.Count();
		for( TInt i(0); i < instanceViewCount; i++ )
			{
			iCalInstanceViewArray[i]->FindInstanceL( instances,
					                                 CalCommon::EIncludeAppts,
                                                     range );
			}
		count = instances.Count();
    	}

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

// ---------------------------------------------------------------------------
// CESMRCalDbMgr::CheckSpaceBelowCriticalLevelL
// ---------------------------------------------------------------------------
//
TBool CESMRCalDbMgr::CheckSpaceBelowCriticalLevelL()
    {
    FUNC_LOG;

    TBool retcode(EFalse);

    CCoeEnv* coeEnv = CCoeEnv::Static();

    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &( coeEnv->FsSession() ) ) )
        {
        CErrorUI* errorUi = CErrorUI::NewLC();
        errorUi->ShowGlobalErrorNoteL( KErrDiskFull );
        CleanupStack::PopAndDestroy(); // errorUi
        retcode = ETrue;
        }

    return retcode;
    }

// ---------------------------------------------------------------------------
// CESMRCalDbMgr::LoadMultiCalenInfo
// ---------------------------------------------------------------------------
//
void CESMRCalDbMgr::LoadMultiCalenInfoL()
    {
    FUNC_LOG;

    CDesCArray* fileArray = iCalSession.ListCalFilesL();
    CleanupStack::PushL( fileArray );

    TInt count = fileArray->Count();

    iCalSessionArray.ReserveL( count );
    iCalEntryViewArray.ReserveL( count );
    iCalInstanceViewArray.ReserveL( count );
    iCalendarInfoArray.ReserveL( count );

    for( TInt i = 0; i < count; i++ )
        {
        CCalSession* file = CCalSession::NewL();
        CleanupStack::PushL( file );

        const TDesC& fileName = (*fileArray)[i];
        file->OpenL( fileName );
        CCalCalendarInfo* calendarInfo = file->CalendarInfoL();

        if( calendarInfo->IsValid()
            && calendarInfo->Enabled() )
            {
            // Push info into CleanupStack because NewL may leave
            CleanupStack::PushL( calendarInfo );

            CCalEntryView* entryView = CCalEntryView::NewL( *file );
            // Push entryView into CleanupStack because NewL may leave
            CleanupStack::PushL( entryView );

            CCalInstanceView* instanceView = CCalInstanceView::NewL( *file );

            // Space has been reserved so AppendL won't leave
            iCalInstanceViewArray.AppendL( instanceView );

            iCalEntryViewArray.AppendL( entryView );
            CleanupStack::Pop( entryView );

            iCalendarInfoArray.AppendL( calendarInfo );
            CleanupStack::Pop( calendarInfo );

            iCalSessionArray.AppendL( file );
            CleanupStack::Pop( file );
            }
        else
            {
            delete calendarInfo;
            CleanupStack::PopAndDestroy( file );
            }
        }

    CleanupStack::PopAndDestroy( fileArray );

    // Restore default database index from repository
    ReadDatabaseIndexL( iCurCalenIndex );
    }


// End of file


