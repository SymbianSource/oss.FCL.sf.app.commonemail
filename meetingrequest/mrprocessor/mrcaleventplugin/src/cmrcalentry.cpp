/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar Entry wrapper implementation
*
*/


// INCLUDE FILES

#include "cmrcalentry.h"
#include "esmrhelper.h"

#include "cesmrrecurrenceinfohandler.h"
#include "cesmralarminfohandler.h"
#include "tesmralarminfo.h"
#include "cesmrcaluserutil.h"
#include "cesmrcaldbmgr.h"
#include "esmrentryhelper.h"
#include "cesmrconflictchecker.h"

#include <mrcaleventdata.rsg>
#include <coemain.h>
#include <e32std.h>
#include <AknUtils.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include <calentry.h>
#include <calattachment.h>
#include <calalarm.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <CalenInterimUtils2.h>

#include <CalendarInternalCRKeys.h>
#include <ct/rcpointerarray.h>

#include <calenservices.h>

#include "emailtrace.h"

namespace { // codescanner::namespace

const TInt KHoursInDay( 24 );
const TInt KDefaultTodoAlarmHours( 12 );
const TInt KDefaultAnniversaryAlarmHours( 12 );

// Definition for max hours, minutes, seconds in day for To-Do
const TInt KMaxHoursForTodo( 23 );
const TInt KMaxMinutesForTodo( 59 );
const TInt KMaxSecondsForTodo( 00 );

// Definition for max hours, minutes, seconds in day for Memo
const TInt KMaxHoursForMemo( 23 );
const TInt KMaxMinutesForMemo( 59 );
const TInt KMaxSecondsForMemo( 59 );

// Definition for default alarm time for meeting
const TInt KDefaultMeetingAlarmMinutes( 15 );

// Definitions for alarm information resource file
_LIT( KAlarmInfoResource, "mrcaleventdata.rsc" );

#ifdef _DEBUG

// Definition for panic text
_LIT( KBCCalEntryPanicTxt, "MRCalEntry" );

enum TBCCalEntryPanic
    {
    EBCCalEntryNotExist = 1, // Entry does not exist
    };

// ---------------------------------------------------------------------------
// Raises panic.
// ---------------------------------------------------------------------------
//
void Panic(TBCCalEntryPanic aPanic)
    {
    User::Panic( KBCCalEntryPanicTxt, aPanic);
    }

#endif // _DEBUG

}  // namespace


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRCalEntry::CMRCalEntry
// ---------------------------------------------------------------------------
//
CMRCalEntry::CMRCalEntry(
        MESMRCalDbMgr& aCalDb  )
:   iCalDb( aCalDb )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::~CMRCalEntry
// ---------------------------------------------------------------------------
//
CMRCalEntry::~CMRCalEntry()
    {
    FUNC_LOG;
    delete iEntry;
    delete iOriginalEntry;
    delete iComparativeEntry;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::NewL
// ---------------------------------------------------------------------------
//
CMRCalEntry* CMRCalEntry::NewL(
        CCalEntry& aEntry,
        MESMRCalDbMgr& aCalDb )
    {
    FUNC_LOG;
    
    CMRCalEntry* self = new (ELeave) CMRCalEntry( aCalDb );
    CleanupStack::PushL( self );
    self->ConstructL( aEntry );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CMRCalEntry::ConstructL(
        CCalEntry& aEntry )
    {
    FUNC_LOG;
    iOriginalEntry = CreateCopyL( aEntry );
    
    // Create the entry for editing:
    iEntry = CreateCopyL( *iOriginalEntry );   
    
    // Create copy of initialized entry for 
    // comparing purposes e.g. IsEntryEdited.
    iComparativeEntry = CreateCopyL( *iEntry );
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::CreateCopyL
//
CCalEntry* CMRCalEntry::CreateCopyL( CCalEntry& aSourceEntry )
    {
    FUNC_LOG;
    CCalEntry* entry = 
        ESMRHelper::CopyEntryL(
                aSourceEntry,
                aSourceEntry.MethodL(),
                ESMRHelper::ECopyFull );

    return entry;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::Type
// ---------------------------------------------------------------------------
//
MESMRCalEntry::TESMRCalEntryType CMRCalEntry::Type() const
    {
    FUNC_LOG;
    
    MESMRCalEntry::TESMRCalEntryType type( EESMRCalEntryNotSupported );

    CCalEntry::TType entryType( CCalEntry::EAppt );

    TRAP_IGNORE( {
                 entryType = DoGetEntry()->EntryTypeL();
                 } );

    switch ( entryType )
        {
        case CCalEntry::EAppt:
            {
            type = EESMRCalEntryMeeting;
            break;
            }
        case CCalEntry::ETodo:
            {
            type = EESMRCalEntryTodo;
            break;
            }
        case CCalEntry::EEvent:
            {
            type = EESMRCalEntryMemo;
            break;
            }
        case CCalEntry::EReminder:
            {
            type = EESMRCalEntryReminder;
            break;
            }
        case CCalEntry::EAnniv:
            {
            type = EESMRCalEntryAnniversary;
            break;
            }
        default:
            break;
        }

    return type;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::MESMRCalEntryRef
// ---------------------------------------------------------------------------
//
MESMRCalEntry& CMRCalEntry::MESMRCalEntryRef()
    {
    FUNC_LOG;
    return *this;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::MESMRCalEntryRef
// ---------------------------------------------------------------------------
//
const MESMRCalEntry& CMRCalEntry::MESMRCalEntryRef() const
    {
    FUNC_LOG;
    return *this;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::Entry
// ---------------------------------------------------------------------------
//
const CCalEntry& CMRCalEntry::Entry() const
    {
    FUNC_LOG;
    return *DoGetEntry();
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::Entry
// ---------------------------------------------------------------------------
//
CCalEntry& CMRCalEntry::Entry()
    {
    FUNC_LOG;
    return *DoGetEntry();
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::InstanceL
// ---------------------------------------------------------------------------
//
CCalInstance* CMRCalEntry::InstanceL() const
    {
    FUNC_LOG;

    CCalInstance* instance = NULL;

    instance = iCalDb.FindInstanceL( *DoGetEntry() );
    if ( !instance )
        {
        // Instance not found by using the edited entry
        // Trying with orginal.
        instance = iCalDb.FindInstanceL( *iComparativeEntry );
        }

    if ( !instance )
        {
        // Instance not found by using edited or orginal entry.
        // --> Leave
        User::Leave( KErrNotFound );
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::CanSetRecurrenceL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntry::CanSetRecurrenceL() const
    {
    FUNC_LOG;
    
    TBool canSetRecurrence( ETrue );
    
    if ( DoGetEntry()->EntryTypeL() == CCalEntry::EAppt &&
         IsRepeatingMeetingL(*iEntry) &&
        (!ESMREntryHelper::IsModifyingEntryL(*iEntry) &&
          MESMRCalEntry::EESMRThisOnly == iRecurrenceModRule ))
        {
        canSetRecurrence = EFalse;
        }
    
    return canSetRecurrence;
    }


// ---------------------------------------------------------------------------
// CMRCalEntry::GetRecurrenceL
// ---------------------------------------------------------------------------
//
void CMRCalEntry::GetRecurrenceL(
        TESMRRecurrenceValue& aRecurrence,
        TTime& aUntil ) const
    {
    FUNC_LOG;
    
    CESMRRecurrenceInfoHandler* recurrenceHandler =
    		CESMRRecurrenceInfoHandler::NewLC( *DoGetEntry() );
    
    recurrenceHandler->GetRecurrenceL( aRecurrence, aUntil );
    CleanupStack::PopAndDestroy( recurrenceHandler );
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::IsAllDayEventL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntry::IsAllDayEventL() const
    {
    FUNC_LOG;
    
    TBool allDayEvent(EFalse);

    CCalEntry::TType entryType = DoGetEntry()->EntryTypeL();
    
    if ( (CCalEntry::EAppt == entryType) || 
         (CCalEntry::EEvent == entryType) )        
    	{    
	    TCalTime startTime = iEntry->StartTimeL();
	    TCalTime stopTime  = iEntry->EndTimeL();
	
	    TTimeIntervalHours hoursBetweenStartAndEnd;
	    stopTime.TimeLocalL().HoursFrom(
	            startTime.TimeLocalL(),
	            hoursBetweenStartAndEnd );
	
	    TCalTime::TTimeMode mode = startTime.TimeMode();
	
	    TInt hoursBetweenStartAndEndAsInt( hoursBetweenStartAndEnd.Int() );
	    TInt alldayDivident( hoursBetweenStartAndEndAsInt % KHoursInDay );
	
	    if ( hoursBetweenStartAndEndAsInt && 0 == alldayDivident )
	        {
	        TDateTime startTimeLocal = startTime.TimeLocalL().DateTime();
	        TDateTime stopTimeLocal =  stopTime.TimeLocalL().DateTime();
	
            if ( startTimeLocal.Hour() == 0 &&
	             startTimeLocal.Minute() == 0 && 
	             startTimeLocal.Minute() == stopTimeLocal.Minute() && 
	        	 startTimeLocal.Second() == 0 && 
	        	 startTimeLocal.Second() == stopTimeLocal.Second() )
	            {
	            allDayEvent = ETrue;
	            }
	        }
    	}

    return allDayEvent;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::SetRecurrenceL
// ---------------------------------------------------------------------------
//
void CMRCalEntry::SetRecurrenceL(
        TESMRRecurrenceValue aRecurrence,
        TTime aUntil )
    {
    FUNC_LOG;
    // Check if this entry's recurrence can be edited
    
    if ( !CanSetRecurrenceL() )
        {
        User::Leave( KErrNotSupported );
        }

    CESMRRecurrenceInfoHandler* recurrenceHandler =
            CESMRRecurrenceInfoHandler::NewLC( *DoGetEntry() );

    recurrenceHandler->SetRecurrenceL( aRecurrence, aUntil );
    CleanupStack::PopAndDestroy( recurrenceHandler );

    iRecurrenceModRule = MESMRCalEntry::EESMRAllInSeries;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::SetAllDayEventL
// ---------------------------------------------------------------------------
//
void CMRCalEntry::SetAllDayEventL( TTime aStartDate, TTime aEndDate)
    {
    FUNC_LOG;
    TCalTime startTime;
    TCalTime stopTime;

    TDateTime start;
    TDateTime end;

    // set the start time to 0:00
    start.Set( aStartDate.DateTime().Year(),
               aStartDate.DateTime().Month(),
               aStartDate.DateTime().Day(),
               0,
               0,
               0,
               0);

    // set the end date to next day from given end date since
    // all day event should last 24 hours.
    aEndDate += TTimeIntervalDays( 1 );

    end.Set( aEndDate.DateTime().Year(),
             aEndDate.DateTime().Month(),
             aEndDate.DateTime().Day(),
             0,
             0,
             0,
             0);

    startTime.SetTimeLocalL( start );
    stopTime.SetTimeLocalL( end );

    DoGetEntry()->SetStartAndEndTimeL( startTime, stopTime );
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::IsStoredL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntry::IsStoredL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EBCCalEntryNotExist ) );

    TBool ret(EFalse);
    CCalEntry* dbEntry = NULL;
    
    TRAPD( err, dbEntry = iCalDb.FetchEntryL(
                                iEntry->UidL(),
                                iEntry->RecurrenceIdL() ) );
    
    if ( KErrNotFound == err )
        {
        // Error has occured while retrieving an entry        
        ret = EFalse;
        }    
    else if ( dbEntry)
        {
        // Entry was found from the calendar db --> it is stored for sure.
        ret = ETrue;
        }

    delete dbEntry;

    return ret;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::IsEntryEditedL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntry::IsEntryEditedL() const
    {
    FUNC_LOG;
    
    TBool isSame(ETrue);    
    
    // The compare entry has not been created yet; so 
    // entry has not changed. 
    if ( iComparativeEntry )
        {
        isSame = iComparativeEntry->CompareL( *DoGetEntry() );

        // CCalEntry's CompareL doesn't check the priority value:
        if ( isSame && iEntry->PriorityL() != iComparativeEntry->PriorityL() )
            {
            isSame = EFalse;
            }
        else if ( iEntry->DescriptionL() != iComparativeEntry->DescriptionL() )
            {
            isSame = EFalse;
            }
        else if ( iEntry->LocationL() != iComparativeEntry->LocationL() )
            {
            isSame = EFalse;
            }
        else if ( iEntry->ReplicationStatusL() != iComparativeEntry->ReplicationStatusL() )
            {
            isSame = EFalse;
            }
        
        if ( IsStoredL() )
            if ( iCalDb.EntryViewL( *iEntry ) != iCalDb.EntryView() )
                {
                isSame = EFalse;
                }
        }

    
    
    return !isSame;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::IsRecurrentEventL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntry::IsRecurrentEventL() const
    {
    FUNC_LOG;
    if ( DoGetEntry()->EntryTypeL() != CCalEntry::EAppt )
        {
        return EFalse;
        }

    TBool recurrenceEvent( EFalse );
    if ( IsStoredL() )
        {
        // Entry is stored in calendar db
        // Lets look recurrence using instance

        // Ownership is transferred
        CCalInstance* instance = NULL;
        TRAPD(err, instance = InstanceL() );
        if ( KErrNotFound != err )
            {
            User::LeaveIfError( err );
            }

        if ( instance )
            {
            CleanupStack::PushL( instance );

            CCalEntry& instanceParentEntry = instance->Entry();

            if ( IsRepeatingMeetingL( instanceParentEntry ) )
                {
                recurrenceEvent = ETrue;
                }
            CleanupStack::PopAndDestroy( instance );
            }
        else
            {
            if ( IsRepeatingMeetingL( *iEntry ) )
                {
                recurrenceEvent = ETrue;
                }
            }
        }
    else
        {
        // Entry is not stored in calendar db
        if ( IsRepeatingMeetingL( *iEntry ) )
            {
            // This is repeating meeting
            recurrenceEvent = ETrue;
            }
        }
    
    return recurrenceEvent;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::IsEntryTypeChangedL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntry::IsEntryTypeChangedL() const
    {
    FUNC_LOG;
    
    return iTypeChanged;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::SetModifyingRuleL
// ---------------------------------------------------------------------------
//
void CMRCalEntry::SetModifyingRuleL( TESMRRecurrenceModifyingRule aRule, 
		const TBool aTypeChanging )
    {
    FUNC_LOG;
    
    CESMRRecurrenceInfoHandler* recHandler =
            CESMRRecurrenceInfoHandler::NewL( *iComparativeEntry );
    CleanupStack::PushL( recHandler );

    TESMRRecurrenceValue orginalRecurrence;
    TTime orginalUntil;

    recHandler->GetRecurrenceL(
            orginalRecurrence,
            orginalUntil);
    CleanupStack::PopAndDestroy( recHandler );
    recHandler = NULL;    

    TBool modifyingEntry( ESMREntryHelper::IsModifyingEntryL( *DoGetEntry() ) );
    
    if ( MESMRCalEntry::EESMRAllInSeries == aRule && IsStoredL() && !aTypeChanging)
        {
        // When we want to modify series of recurrence entries -->
        // Parent entry is modified
        if ( ERecurrenceNot == orginalRecurrence && !modifyingEntry )
            {
            // Orginal entry was not recurrent event
            // No need to fect instance at all
            // For modifying entries we need to fetch the original parent entry
            iRecurrenceModRule = aRule;
            return;
            }

        CCalInstance* instance = NULL;
        TRAPD(err, instance = InstanceL() );
        if( KErrNotFound != err )
            {
            User::LeaveIfError( err );
            }

        if ( instance )
            {
            CleanupStack::PushL( instance );

            CCalEntry::TMethod entryMethod( iEntry->MethodL() );
           
            delete iEntry; 
            iEntry = NULL;
            
            delete iComparativeEntry; 
            iComparativeEntry = NULL;

            RCPointerArray<CCalEntry> entries;
            CleanupClosePushL( entries );

            iCalDb.EntryViewL( instance->Entry() )->FetchL(
                    instance->Entry().UidL(), entries );

            TInt parentIndex( KErrNotFound );
            TInt entryCount( entries.Count() );            
            for ( TInt i(0); i < entryCount && KErrNotFound == parentIndex; ++i )
                {
                TBool modifyingEntry( ESMREntryHelper::IsModifyingEntryL( *entries[i]) );
                if ( !modifyingEntry )
                    {
                    parentIndex = i;
                    }
                }
            
            CCalEntry& parent = *entries[parentIndex];

            TPtrC description( parent.DescriptionL() );
            
            iEntry = CreateCopyL( parent );
            
            // Adjust parent entry's start and end time to entry
            TCalTime start;
            TCalTime end;

            CESMRRecurrenceInfoHandler* recurrenceHandler =
                    CESMRRecurrenceInfoHandler::NewLC( parent );

            recurrenceHandler->GetFirstInstanceTimeL( start, end );
            CleanupStack::PopAndDestroy( recurrenceHandler );
            recurrenceHandler = NULL;

            iEntry->SetStartAndEndTimeL( start, end );

            iComparativeEntry = CreateCopyL( *iEntry );

            iEntry->SetDescriptionL( description );
            iComparativeEntry->SetDescriptionL( description );
            
            CleanupStack::PopAndDestroy(); // entries
            CleanupStack::PopAndDestroy( instance );
            }
        }
    else if ( ERecurrenceNot != orginalRecurrence && 
    		aTypeChanging && !modifyingEntry )
    	{
		// if entry( in the memory) is a recurrent event 
		// ,and if entry type is changing, and not in modifying status then EESMRAllInSeries
		iRecurrenceModRule = EESMRAllInSeries ;
		return;
    	}
    

    iRecurrenceModRule = aRule;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::RecurrenceModRule
// ---------------------------------------------------------------------------
//
MESMRCalEntry::TESMRRecurrenceModifyingRule 
        CMRCalEntry::RecurrenceModRule() const
    {
    FUNC_LOG;
    return iRecurrenceModRule;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::GetAlarmL
// ---------------------------------------------------------------------------
//
void CMRCalEntry::GetAlarmL(TESMRAlarmType& aAlarmType, TTime &aAlarmTime )
    {
    FUNC_LOG;
    
    aAlarmType = MESMRCalEntry::EESMRAlarmNotFound;
    aAlarmTime = Time::NullTTime();

    TFileName alarmInfoResource;
    ESMRHelper::LocateResourceFile(
            KAlarmInfoResource,
            KDC_RESOURCE_FILES_DIR,
            alarmInfoResource);

    CESMRAlarmInfoHandler* alarmInfoHandler =
    		CESMRAlarmInfoHandler::NewLC();

    alarmInfoHandler->ReadFromResourceL(
            alarmInfoResource,
            MRCALEVENT_ALARM_INFO_TABLE );

    TRAPD( err,
            alarmInfoHandler->GetAbsoluteAlarmTimeL(*DoGetEntry(), aAlarmTime) );

    if ( KErrNone == err )
        {
        aAlarmType = MESMRCalEntry::EESMRAlarmAbsolute;

        // only meeting that is not allday event can have relative alarm:
        if ( Type() == EESMRCalEntryMeeting && !IsAllDayEventL() )
            {
            TESMRAlarmInfo alarmInfo;
            TRAP( err,
                  alarmInfoHandler->GetAlarmInfoObjectL(*iEntry, alarmInfo) );

            if ( KErrNone == err )
                {
                aAlarmType = MESMRCalEntry::EESMRAlarmRelative;
                if( alarmInfo.iRelativeAlarmInSeconds < 0 )
                    {
                    aAlarmType = MESMRCalEntry::EESMRAlarmNotFound;
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy( alarmInfoHandler );
    alarmInfoHandler = NULL;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::OriginalEntry
// ---------------------------------------------------------------------------
//
const CCalEntry& CMRCalEntry::OriginalEntry()
    {
    FUNC_LOG;
    return *iComparativeEntry;
    }



// ----------------------------------------------------------------------------
// CMRCalEntry::IsRepeatingMeetingL
// ----------------------------------------------------------------------------
//
TBool CMRCalEntry::IsRepeatingMeetingL(const CCalEntry& aEntry) const
    {
    FUNC_LOG;
    TBool retVal( EFalse );

    if ( ESMREntryHelper::IsModifyingEntryL(aEntry) )
        {
        retVal = ETrue;
        }
    else
        {
        TCalRRule dummyRule;
        if ( aEntry.GetRRuleL( dummyRule ) )
            {
            retVal = ETrue;
            }
        else
            {
            RArray<TCalTime> dummyRDateList;
            CleanupClosePushL( dummyRDateList );
            aEntry.GetRDatesL( dummyRDateList );
            if ( dummyRDateList.Count() > 0 )
                {
                retVal = ETrue;
                }
            CleanupStack::PopAndDestroy(); // dummyRDateList
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::SetPriorityL
// ---------------------------------------------------------------------------
//
void CMRCalEntry::SetPriorityL ( TUint aPriority )
    {
    FUNC_LOG;
    DoGetEntry()->SetPriorityL( aPriority );
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::GetPriorityL
// ---------------------------------------------------------------------------
//
TUint CMRCalEntry::GetPriorityL() const
    {
    FUNC_LOG;
    TUint entryPriority = DoGetEntry()->PriorityL();

    if ( iEntry->EntryTypeL() == CCalEntry::ETodo )
        {
        if ( entryPriority != EFSCalenTodoPriorityLow &&
             entryPriority != EFSCalenTodoPriorityNormal &&
             entryPriority != EFSCalenTodoPriorityHigh )
             {
             entryPriority = EFSCalenTodoPriorityNormal;
             }
        }
    else
        { 
        if ( entryPriority != EFSCalenMRPriorityLow &&
             entryPriority != EFSCalenMRPriorityNormal &&
             entryPriority != EFSCalenMRPriorityHigh )
             {
             entryPriority = EFSCalenMRPriorityNormal;
             }
        }
        
    return entryPriority;
    }

// ----------------------------------------------------------------------------
// CMRCalEntry::UpdateEntryAfterStoringL
// ----------------------------------------------------------------------------
//
void CMRCalEntry::UpdateEntryAfterStoringL()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic( EBCCalEntryNotExist ) );

    CCalEntry* storedEntry = NULL;
        
    if ( IsRecurrentEventL() &&
         EESMRThisOnly == iRecurrenceModRule &&
         IsStoredL() && !ESMREntryHelper::IsModifyingEntryL(*iEntry) )
        {
        // We have stored one instance of series. 
        storedEntry = iCalDb.FetchEntryL( 
                    iEntry->UidL(), 
                    iComparativeEntry->StartTimeL() );        
        }
    else
        {
        // We are dealing with single instance or with the series
        storedEntry = iCalDb.FetchEntryL( 
                    iEntry->UidL(), 
                    iEntry->RecurrenceIdL() );
        }
    
    __ASSERT_DEBUG( storedEntry, Panic( EBCCalEntryNotExist ) );
    CleanupStack::PushL( storedEntry );
    
    // Description needs to be fecthed explicitly into memory
    TPtrC description( storedEntry->DescriptionL() );                
    
    delete iEntry;
    iEntry = NULL;
    iEntry = CreateCopyL(*storedEntry);
        
    if ( MESMRCalEntry::EESMRAllInSeries == iRecurrenceModRule && 
         IsRecurrentEventL() )
        {
        // Adjust parent entry's start and end time to entry
        TCalTime start;
        TCalTime end;

        CESMRRecurrenceInfoHandler* recurrenceHandler =
                CESMRRecurrenceInfoHandler::NewLC( *iEntry );

        recurrenceHandler->GetFirstInstanceTimeL( start, end );
        CleanupStack::PopAndDestroy( recurrenceHandler );
        recurrenceHandler = NULL;

        iEntry->SetStartAndEndTimeL( start, end );
        iEntry->SetDescriptionL( description );
        }
    
    CCalEntry* temp  = CreateCopyL( *iEntry );
     
    delete iComparativeEntry;
    iComparativeEntry = temp;
    
    CleanupStack::PopAndDestroy( storedEntry );
    }
    
// ---------------------------------------------------------------------------
// CMRCalEntry::SetDefaultValuesToEntryL
// ---------------------------------------------------------------------------
//
void CMRCalEntry::SetDefaultValuesToEntryL()
    {
    FUNC_LOG;
    
    if ( !IsStoredL() )
        {
        // by default when creating a meeting the priority
        // value is normal
        switch ( DoGetEntry()->EntryTypeL() )
            {
            case CCalEntry::ETodo:
                {
                SetDefaultValuesForTodoL();
                }
                break;
            case CCalEntry::EEvent: // Memo
                {
                SetDefaultValuesForMemoL();
                }
                break;
            case CCalEntry::EAppt:
                {
                SetDefaultValuesForMeetingL();
                }
                break;
            case CCalEntry::EAnniv:
            default:
                {
                SetDefaultValuesForAnniversaryL();
                }
                break;
            }
        iEntry->SetReplicationStatusL( CCalEntry::EOpen );

        // Entry has now been set up with default values; 
        // Let's update comparative entry
        delete iComparativeEntry;
        iComparativeEntry = NULL;
        
        iComparativeEntry = CreateCopyL( *iEntry ); 
        }
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::UpdateTimeStampL
// ---------------------------------------------------------------------------
//
void CMRCalEntry::UpdateTimeStampL()
    {
    FUNC_LOG;
    
    ASSERT( iEntry );
    
    TTime currentUTCTime;
    currentUTCTime.UniversalTime();

    TCalTime currentTime;
    currentTime.SetTimeUtcL( currentUTCTime );

    iEntry->SetDTStampL( currentTime );    
    }

// ----------------------------------------------------------------------------
// CMRCalEntry::CloneEntryLC
// ----------------------------------------------------------------------------
//
CCalEntry* CMRCalEntry::CloneEntryLC( TESMRCalEntryType aType ) const
    {
    CCalEntry* entry = ESMRHelper::CopyEntryLC(
            *DoGetEntry(),
            iEntry->MethodL(),
            ESMRHelper::ECopyFull,
            TESMRCalendarEventType( aType ) );
        
    return entry;
    }

// ----------------------------------------------------------------------------
// CMRCalEntry::UpdateComparativeEntry
// ----------------------------------------------------------------------------
//
void CMRCalEntry::UpdateComparativeEntry( CCalEntry* aNewComparativeEntry )
    {
    FUNC_LOG;
    if( iComparativeEntry )
        {
        delete iComparativeEntry;
        iComparativeEntry = NULL;
        
        iComparativeEntry = aNewComparativeEntry;
        }
    
    }

// ----------------------------------------------------------------------------
// CMRCalEntry::SetDefaultValuesForTodoL
// ----------------------------------------------------------------------------
//
void CMRCalEntry::SetDefaultValuesForTodoL()
    {
    FUNC_LOG;
    
    SetPriorityL( EFSCalenTodoPriorityNormal );
    
    // Update event time
    TDateTime start = iEntry->StartTimeL().TimeLocalL().DateTime();
    start.SetHour( KMaxHoursForTodo );
    start.SetMinute( KMaxMinutesForTodo );
    start.SetSecond( KMaxSecondsForTodo );
    
    TCalTime dueTime;
    dueTime.SetTimeLocalL( start );
    iEntry->SetStartAndEndTimeL( dueTime, dueTime );
    
    // Create default alarm
    TTime eventTime = iEntry->EndTimeL().TimeLocalL();
    
    // Calculate alarm time (due date at 12:00am)
    TTime alarmTime = eventTime;
    TDateTime alarmDateTime = alarmTime.DateTime();
    alarmDateTime.SetHour( KDefaultTodoAlarmHours );
    alarmDateTime.SetMinute( 0 );
    alarmDateTime.SetSecond( 0 );
    alarmDateTime.SetMicroSecond( 0 );
    alarmTime = alarmDateTime;
    
    TTime currentTime;
    currentTime.HomeTime();
    
    if ( alarmTime > currentTime )
        {
        CCalAlarm* alarm = CCalAlarm::NewL();
        CleanupStack::PushL( alarm );
        TTimeIntervalMinutes alarmOffset( 0 );
        eventTime.MinutesFrom( alarmTime, alarmOffset );
        alarm->SetTimeOffset( alarmOffset );
        iEntry->SetAlarmL( alarm );
        CleanupStack::PopAndDestroy( alarm );
        }    
    }

// ----------------------------------------------------------------------------
// CMRCalEntry::SetDefaultValuesForMemoL
// ----------------------------------------------------------------------------
//
void CMRCalEntry::SetDefaultValuesForMemoL()
    {
    FUNC_LOG;

    SetPriorityL( EFSCalenMRPriorityNormal );
    
    // Update event time
    TDateTime start = iEntry->StartTimeL().TimeLocalL().DateTime();
    start.SetHour( 0 );
    start.SetMinute( 0 );
    start.SetSecond( 0 );
        
    TDateTime end = iEntry->EndTimeL().TimeLocalL().DateTime();
    end.SetHour( KMaxHoursForMemo );
    end.SetMinute( KMaxMinutesForMemo );
    end.SetSecond( KMaxSecondsForMemo );
    
    TCalTime startTime;
    startTime.SetTimeLocalL( start );
    TCalTime endTime;
    endTime.SetTimeLocalL( end );

    iEntry->SetStartAndEndTimeL( startTime, endTime );    
    }

// ----------------------------------------------------------------------------
// CMRCalEntry::SetDefaultValuesForMeetingL
// ----------------------------------------------------------------------------
//
void CMRCalEntry::SetDefaultValuesForMeetingL()
    {
    FUNC_LOG;
    
    SetPriorityL( EFSCalenMRPriorityNormal );
    
    // Get default alarm time from central repository
    TInt defaultAlarmTime;
    CRepository* repository = CRepository::NewLC( TUid::Uid(0x101F874B) /*KCRUidCalendar*/ );
    TInt err = repository->Get( KCalendarDefaultAlarmTime, defaultAlarmTime );
    CleanupStack::PopAndDestroy( repository );
    
    if ( err != KErrNone )
        {
        defaultAlarmTime = KDefaultMeetingAlarmMinutes;
        }
    
    // Getting current time
    TTime currentTime;
    currentTime.HomeTime();
    
    // Getting meeting start time
    TTime start = iEntry->StartTimeL().TimeLocalL();
   
    // Create default alarm
    CCalAlarm* alarm = CCalAlarm::NewL();
    CleanupStack::PushL( alarm );
    
    TTimeIntervalMinutes alarmOffset( defaultAlarmTime );
    
    // If alarm time is in past
    if ( ( start - alarmOffset ) < currentTime )
        {
        // Setting alarm off
        iEntry->SetAlarmL( NULL );
        }
    else
        {
        // Set default alarm time
        alarm->SetTimeOffset( alarmOffset );
        iEntry->SetAlarmL( alarm );
        }
    CleanupStack::PopAndDestroy( alarm );   
    
    if ( iEntry->StartTimeL().TimeUtcL() == iEntry->EndTimeL().TimeUtcL() )
        {
        // Should this value be read from cenrep?
        TTimeIntervalHours KDefaultMeetingDuration(1);

        TCalTime newEndTime;
        newEndTime.SetTimeUtcL(
                iEntry->StartTimeL().TimeUtcL() + KDefaultMeetingDuration );
        iEntry->SetStartAndEndTimeL(iEntry->StartTimeL(), newEndTime);        
        }
    }

// ----------------------------------------------------------------------------
// CMRCalEntry::SetDefaultValuesForAnniversaryL
// ----------------------------------------------------------------------------
//
void CMRCalEntry::SetDefaultValuesForAnniversaryL()
    {
    FUNC_LOG;

    SetPriorityL( EFSCalenMRPriorityNormal );

    // Getting event time
    TTime eventTime = iEntry->StartTimeL().TimeLocalL();
    
    // Setting default alarm parameters to previous day at noon
    TTime alarmTime = eventTime - TTimeIntervalDays( 1 );
    TDateTime alarmDateTime = alarmTime.DateTime(); 
    alarmDateTime.SetHour( KDefaultAnniversaryAlarmHours );
    alarmDateTime.SetMinute( 0 );
    alarmDateTime.SetSecond( 0 );
    alarmDateTime.SetMicroSecond( 0 );
    alarmTime = alarmDateTime;
    
    // Calculating alarm offset
    TTimeIntervalMinutes alarmOffset( 0 );
    eventTime.MinutesFrom( alarmTime, alarmOffset );
    
    // Creating alarm
    CCalAlarm* alarm = CCalAlarm::NewL();
    CleanupStack::PushL( alarm );
    alarm->SetTimeOffset( alarmOffset );

    // Setting alarm
    iEntry->SetAlarmL( alarm );
    CleanupStack::PopAndDestroy( alarm );
    alarm = NULL;    
    }

// ----------------------------------------------------------------------------
// CMRCalEntry::DoGetEntry
// ----------------------------------------------------------------------------
//
CCalEntry* CMRCalEntry::DoGetEntry() const
    {
    __ASSERT_DEBUG( iEntry, Panic( EBCCalEntryNotExist ) );
    
    return iEntry;
    }

// ----------------------------------------------------------------------------
// CMRCalEntry::ValidateEntryL
// ----------------------------------------------------------------------------
//
CCalEntry* CMRCalEntry::ValidateEntryL()
    {
    FUNC_LOG;
    
    CCalEntry* entry = NULL;

    MESMRCalEntry::TESMRCalEntryType entryType = Type();
    switch ( entryType )
       {
       case EESMRCalEntryMeeting:
           {
           entry = ValidateMeetingL();
           }
           break;
       
       case EESMRCalEntryAnniversary:
           {
           entry = CreateCopyL( *iEntry );
           CleanupStack::PushL( entry );

           // Special check for anniversary: Occurancy must be set to yearly
           // for new entries
           if ( ( entry->EntryTypeL() == CCalEntry::EAnniv ) && 
                   !IsStoredL() &&
                       !IsRecurrentEventL() )
               {
               TCalRRule rrule( TCalRRule::EYearly );
               rrule.SetDtStart( Entry().StartTimeL() );
               rrule.SetInterval( 1 );
               rrule.SetCount( 0 );
               entry->SetRRuleL( rrule );
               }
          
           CleanupStack::Pop( entry ); //Ownership given to caller
           }
           break;
           
       case EESMRCalEntryTodo: // flow through 
       case EESMRCalEntryMemo: // flow through 
       case EESMRCalEntryReminder:
           {
           entry = CreateCopyL( *iEntry );
           }
           break;
       default:
           // This should not occur
           User::Leave( KErrArgument );
           break;
       }
    
    return entry;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::RemoveInstanceFromSeriesL
// ---------------------------------------------------------------------------
//
CCalEntry* CMRCalEntry::RemoveInstanceFromSeriesL()
    {
    FUNC_LOG;

    CCalEntry* retEntry = NULL;

    if ( IsRecurrentEventL() &&
            EESMRThisOnly == iRecurrenceModRule )
       {
       CCalInstance* instance = NULL;
       TRAPD( err, instance = InstanceL() );
       if( KErrNotFound != err )
           {
           User::LeaveIfError( err );
           }

       if ( instance )
           {
           CleanupStack::PushL( instance );

           CCalEntry& parentEntry = instance->Entry();
           retEntry = ESMRHelper::CopyEntryL(
                    parentEntry,
                    parentEntry.MethodL(),
                    ESMRHelper::ECopyFull );

           CleanupStack::PopAndDestroy( instance );
           instance = NULL;

           CleanupStack::PushL( retEntry );

           CESMRRecurrenceInfoHandler* recurrenceHandler =
                    CESMRRecurrenceInfoHandler::NewLC( *retEntry );

           TCalTime orginalInstanceTime;
           orginalInstanceTime.SetTimeUtcL(
                    iEntry->StartTimeL().TimeUtcL() );

           recurrenceHandler->RemoveInstanceL( orginalInstanceTime );

           CleanupStack::PopAndDestroy( recurrenceHandler );
           CleanupStack::Pop( retEntry );
           }
       }
    else
       {
       User::Leave( KErrNotSupported );
       }

    return retEntry;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::GetDBMgr
// ---------------------------------------------------------------------------
//
MESMRCalDbMgr& CMRCalEntry::GetDBMgr()
    {
    return iCalDb;
    }


// ---------------------------------------------------------------------------
// CMRCalEntry::SupportsCapabilityL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntry::SupportsCapabilityL( 
        MESMRCalEntry::TMREntryCapability aCapability ) const
    {
    FUNC_LOG;
    
    TBool retValue( EFalse );
    
    if ( aCapability == MESMRCalEntry::EMRCapabilityAttachments )
        {
        retValue = ETrue;
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::ContainsRemoteAttachmentsL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntry::ContainsRemoteAttachmentsL()
    {
    FUNC_LOG;
    
    TBool retValue( EFalse );
    
    TInt attachmentCount( iEntry->AttachmentCountL() );
    
    for ( TInt i(0); i < attachmentCount && !retValue; ++i )
        {
        CCalAttachment* attachment = iEntry->AttachmentL(i);        
        CCalAttachment::TType type( attachment->Type() );
        
        if ( CCalAttachment::EFile != type )
            {
            retValue = ETrue;
            }        
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::ValidateMeetingL
// ---------------------------------------------------------------------------
//
CCalEntry* CMRCalEntry::ValidateMeetingL()
    {
    FUNC_LOG;
    
    CCalEntry* entry = NULL;
    
    if ( IsRecurrentEventL() &&
         EESMRThisOnly == iRecurrenceModRule &&
         IsStoredL() && 
         !ESMREntryHelper::IsModifyingEntryL( *iEntry ) )
        {
        CCalInstance* instance = NULL;
        TRAPD( err, instance = InstanceL() );
        if( KErrNotFound != err )
            {
            User::LeaveIfError( err );
            }

        if ( instance )
            {

            CleanupStack::PushL( instance );
            CCalEntry& parent = instance->Entry();

            entry = iCalDb.FetchEntryL(
                            parent.UidL(),
                            iComparativeEntry->StartTimeL() );

            if ( !entry )
                {

                // copy global UID from the original entry
                HBufC8* guid = parent.UidL().AllocLC();

                // create new (child) entry
                entry = CCalEntry::NewL(
                                parent.EntryTypeL(),
                                guid,
                                parent.MethodL(),
                                0,
                                iComparativeEntry->StartTimeL(),
                                CalCommon::EThisOnly );

                CleanupStack::Pop( guid ); 
                guid = NULL; // ownership transferred
                }

            CleanupStack::PopAndDestroy( instance );  
            instance = NULL; // instance
            CleanupStack::PushL( entry );


            CCalEntry::TMethod method( iEntry->MethodL() );

            entry->CopyFromL( *iEntry, CCalEntry::EDontCopyId );
            entry->SetSequenceNumberL( 0 );
            entry->SetMethodL( method );
            entry->SetSummaryL( iEntry->SummaryL() );
            entry->SetLocalUidL( TCalLocalUid( 0 ) );
            entry->ClearRepeatingPropertiesL();

            CleanupStack::Pop( entry );
            }
        }

    if ( !entry )
        {
        entry = CreateCopyL( *iEntry );
        }
    
    return entry;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::SetTypeChanged
// ---------------------------------------------------------------------------
//
void CMRCalEntry::SetTypeChanged( TBool aTypeChanged )
    {
    FUNC_LOG;
    
    iTypeChanged = aTypeChanged;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::AnyInstancesBetweenTimePeriodL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntry::AnyInstancesBetweenTimePeriodL(
            TTime& aStart,
            TTime& aEnd )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic( EBCCalEntryNotExist ) );

    TBool retValue( EFalse );

    RCPointerArray<CCalEntry> entries;
    CleanupClosePushL( entries );

    CESMRConflictChecker* conflictCheckker =
        CESMRConflictChecker::NewL( iCalDb );
    CleanupStack::PushL( conflictCheckker );

    CCalInstance* instance = InstanceL();
    CleanupStack::PushL( instance );
    
    TCalCollectionId colId = instance->InstanceIdL().iCollectionId;
    CleanupStack::PopAndDestroy( instance );
    
    conflictCheckker->FindInstancesForEntryL( aStart,
                                              aEnd,
                                              *iEntry,
                                              colId, 
                                              entries );

    if ( entries.Count() )
        {
        retValue = ETrue;
        }

    CleanupStack::PopAndDestroy( conflictCheckker );
    CleanupStack::PopAndDestroy(); // entries

    return retValue;
    }

// ---------------------------------------------------------------------------
// CMRCalEntry::GetFirstInstanceStartAndEndTimeL
// ---------------------------------------------------------------------------
//
void CMRCalEntry::GetFirstInstanceStartAndEndTimeL(
            TTime& aStart,
            TTime& aEnd )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic( EBCCalEntryNotExist ) );

    // This fetches the parent entry
    TCalTime recurrenceId;
    recurrenceId.SetTimeLocalL( Time::NullTTime() );
    CCalEntry* parent = iCalDb.FetchEntryL(
                iEntry->UidL(),
                recurrenceId );

    CleanupStack::PushL( parent );

    aStart = parent->StartTimeL().TimeLocalL();
    aEnd = parent->EndTimeL().TimeLocalL();

    CleanupStack::PopAndDestroy( parent );
    }

// EOF
