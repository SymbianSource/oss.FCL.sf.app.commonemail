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
* Description:  ESMR MR Entry implementation
*
*/


// INCLUDE FILES
#include "cesmrmeetingrequestentry.h"
#include "cesmrfsmailboxutils.h"
#include "cesmrcaldbmgr.h"
#include "esmrhelper.h"
#include "esmrentryhelper.h"
#include "cesmrconflictchecker.h"
#include "cesmrcaluserutil.h"
#include "cesmralarminfohandler.h"
#include "cesmrrecurrenceinfohandler.h"
#include "mmrinfoobject.h"
#include "mmrattendee.h"

#include "CFSMailMessage.h"

#include "esmrconfig.hrh"
#include "cesmrfeaturesettings.h"

#include "emailtrace.h"

#include <esmralarminfo.rsg>
#include <calentry.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caluser.h>
#include <calattachment.h>
#include <caleninterimutils2.h>
#include <cmrmailboxutils.h>
#include <calrrule.h>
#include <centralrepository.h>
#include <calalarm.h>
#include <calendarinternalcrkeys.h>
#include <data_caging_path_literals.hrh>
#include <coemain.h>
#include <calentryview.h>
#include <ct/rcpointerarray.h>
#include <apmstd.h>
#include <apgcli.h>
#include <apmrec.h>

/// Unnamed namespace for local definitions
namespace {

// Alarm resource file location
_LIT( KAlarmInfoResource, "esmralarminfo.rsc" );

// Definition for default alarm time for meeting
const TInt KDefaultMeetingAlarmMinutes( 15 );

_LIT( KReplaceLineFeedChar, "\n" );
_LIT( KLineFeed, "\x2029");

/**
 * Sets phone owner to entry.
 * @param aMbUtils Reference to mailbox utils.
 * @param aEntry Reference to calendar entry.
 */
void SetPhoneOwnerL(
        CMRMailboxUtils& aMbUtils,
        CCalEntry& aEntry )
    {
    CESMRFsMailboxUtils* fsMbUtils =
            CESMRFsMailboxUtils::NewL( aMbUtils );
    CleanupStack::PushL( fsMbUtils );

    fsMbUtils->SetPhoneOwnerL( aEntry );
    CleanupStack::PopAndDestroy( fsMbUtils );
    fsMbUtils = NULL;
    }

/**
 * Makes copy of the entry and sets phone owner accordingly
 * @param aMbUtils Reference to mailbox utils.
 * @param aEntry Reference to calendar entry.
 */
CCalEntry* CopyEntryL(
        CMRMailboxUtils& aMbUtils,
        const CCalEntry& aEntry )
    {
    CCalEntry* entry = ESMRHelper::CopyEntryL(
                            aEntry,
                            aEntry.MethodL(),
                            ESMRHelper::ECopyFull,
                            EESMREventTypeMeetingRequest );

    SetPhoneOwnerL( aMbUtils, *entry );

    return entry;
    }

#ifdef _DEBUG

// Definition for panic text
_LIT( KESMREntryPanicTxt, "ESMRMeetingRequestEntry" );

/**
 * ES MR Entry panic codes
 */
enum TESMRMeetingRequestEntry
    {
    EESMREntryNotExist = 1, // Entry does not exist
    EESMRInvalidRole,       // Phone owner has invalid role
    EESMRNoInfoObjectAttendeeFound,
    EESMRPhoneOwnerNotSet,
    EESMRRecurrenceError,
    EESMRInvalidReplyType,
    EESMRInvalidInvalidAttendeeStatus,
    EESMRParentNotFound
    };

/**
 * Raises panic.
 * @param aPanic Panic code
 */
void Panic(TESMRMeetingRequestEntry aPanic)
    {
    User::Panic( KESMREntryPanicTxt, aPanic);
    }

#endif // _DEBUG

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::CESMRMeetingRequestEntry
// ---------------------------------------------------------------------------
//
inline CESMRMeetingRequestEntry::CESMRMeetingRequestEntry(
        CMRMailboxUtils& aMRMailboxUtils,
        MESMRCalDbMgr& aCalDb,
        TBool aConflictsExists,
        TESMRInputParams* aESMRInputParams )
:   iMRMailboxUtils( aMRMailboxUtils ),
    iConflictsExists( aConflictsExists),
    iCalDb( aCalDb ),
    iESMRInputParams( aESMRInputParams ),
    iSendCanellation( ETrue )
    {
    FUNC_LOG;
    // Not yet implementation
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::~CESMRMeetingRequestEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRMeetingRequestEntry::~CESMRMeetingRequestEntry()
    {
    FUNC_LOG;
    delete iEntry;
    delete iForwardEntry;
    delete iOrginalEntry;
    delete iParameterEntry;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRMeetingRequestEntry* CESMRMeetingRequestEntry::NewL(
        const CCalEntry& aEntry,
        CMRMailboxUtils& aMRMailboxUtils,
        MESMRCalDbMgr& aCalDb,
        TBool aConflictsExists,
        TESMRInputParams* aESMRInputParams )
    {
    FUNC_LOG;

    CESMRMeetingRequestEntry* self =
            new (ELeave) CESMRMeetingRequestEntry(
                aMRMailboxUtils,
                aCalDb,
                aConflictsExists,
                aESMRInputParams );

    CleanupStack::PushL( self );
    self->ConstructL( aEntry );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::ConstructL(
        const CCalEntry& aEntry )
    {
    FUNC_LOG;

    iParameterEntry = CopyEntryL( iMRMailboxUtils, aEntry );
    iEntry = CopyEntryL( iMRMailboxUtils, aEntry );

    iOrginalEntry  = CopyEntryL( iMRMailboxUtils, aEntry );

    if ( EESMRRoleOrganizer == RoleL() && IsStoredL() )
        {
        // Increase sequence number
        TInt seqNo( iEntry->SequenceNumberL() + 1);
        iEntry->SetSequenceNumberL( seqNo );
        iOrginalEntry->SetSequenceNumberL( seqNo );
        }


    CCalEntry::TMethod method( iEntry->MethodL() );

    if ( CCalEntry::EMethodNone == method )
        {
        iEntry->SetMethodL( CCalEntry::EMethodRequest );
        iOrginalEntry->SetMethodL( CCalEntry::EMethodRequest );
        }

    HBufC* newDescription = ReplaceCharactersFromBufferLC(
                                    iEntry->DescriptionL(),
                                    KReplaceLineFeedChar(),
                                    KLineFeed() );
    iEntry->SetDescriptionL( *newDescription );
    iOrginalEntry->SetDescriptionL( *newDescription );

    CleanupStack::PopAndDestroy( newDescription );

    __ASSERT_DEBUG( iEntry->PhoneOwnerL(), Panic( EESMRPhoneOwnerNotSet) );
    
    CESMRFeatureSettings* settings = CESMRFeatureSettings::NewL();
    if ( settings->FeatureSupported(
            CESMRFeatureSettings::EMRUIMeetingRequestViewerCmailOnly ) )
        {
        // Meeting request viewer available only from email.
        // Remove attachments from entry.  
        iRemoveAttachments = ETrue;
        }
    delete settings;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::ReplaceCharactersFromBufferL
// ---------------------------------------------------------------------------
//
HBufC* CESMRMeetingRequestEntry::ReplaceCharactersFromBufferLC( const TDesC& aTarget,
                              const TDesC& aFindString,
                              const TDesC& aReplacement )
    {
    FUNC_LOG;
    HBufC* newBuffer = aTarget.AllocLC();
    TPtr16 ptr = newBuffer->Des();

    // find next occurance:
    TInt offset = ptr.Find(aFindString);
    while ( offset != KErrNotFound )
        {
        // replace the data:
        ptr.Replace( offset, aFindString.Length(), aReplacement);

        // find next occurance:
        offset = ptr.Find(aFindString);
        }

    return newBuffer;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::Type
// ---------------------------------------------------------------------------
//
MESMRCalEntry::TESMRCalEntryType CESMRMeetingRequestEntry::Type() const
    {
    FUNC_LOG;
    // This is meeting request
    return MESMRCalEntry::EESMRCalEntryMeetingRequest;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::MESMRCalEntryRef
// ---------------------------------------------------------------------------
//
MESMRCalEntry& CESMRMeetingRequestEntry::MESMRCalEntryRef()
    {
    FUNC_LOG;
    return *this;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::MESMRCalEntryRef
// ---------------------------------------------------------------------------
//
const MESMRCalEntry& CESMRMeetingRequestEntry::MESMRCalEntryRef() const
    {
    FUNC_LOG;
    return *this;
    }
// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::Entry
// ---------------------------------------------------------------------------
//
CCalEntry& CESMRMeetingRequestEntry::Entry()
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    if ( iForwardEntry )
        {
        return *iForwardEntry;
        }

    return *iEntry;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::Entry
// ---------------------------------------------------------------------------
//
const CCalEntry& CESMRMeetingRequestEntry::Entry() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );
    if ( iForwardEntry )
        {
        return *iForwardEntry;
        }
    return *iEntry;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::InstanceL
// ---------------------------------------------------------------------------
//
CCalInstance* CESMRMeetingRequestEntry::InstanceL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );
    __ASSERT_DEBUG( iOrginalEntry, Panic(EESMREntryNotExist ) );

    CCalInstance* instance = NULL;

    instance = iCalDb.FindInstanceL( *iEntry );
    if ( !instance )
        {
        // Instance not found by using the edited entry
        // Trying with orginal.
        instance = iCalDb.FindInstanceL( *iOrginalEntry );
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
// CESMRMeetingRequestEntry::CanSetRecurrenceL
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::CanSetRecurrenceL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    TBool canSetRecurrence( ETrue );
    
    if ( iEntry->EntryTypeL() == CCalEntry::EAppt &&
            ESMREntryHelper::IsRepeatingMeetingL(*iEntry) &&
        (!ESMREntryHelper::IsModifyingEntryL(*iEntry) &&
          MESMRCalEntry::EESMRThisOnly == iRecurrenceModRule ))
        {
        canSetRecurrence = EFalse;
        }


    return canSetRecurrence;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::IsRecurrentEventL
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsRecurrentEventL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

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

            if ( ESMREntryHelper::IsRepeatingMeetingL( instanceParentEntry ) )
                {
                recurrenceEvent = ETrue;
                }
            CleanupStack::PopAndDestroy( instance );
            }
        else if ( ESMREntryHelper::IsRepeatingMeetingL( *iEntry ) )
            {
            recurrenceEvent = ETrue;
            }
        }
    else
        {
        // Entry is not stored in calendar db
        if ( ESMREntryHelper::IsRepeatingMeetingL( *iEntry ) )
            {
            // This is repeating meeting
            recurrenceEvent = ETrue;
            }
        }


    return recurrenceEvent;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SetRecurrenceL
//
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::SetRecurrenceL(
        TESMRRecurrenceValue aRecurrence,
        TTime aUntil )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    // Check if this entry's recurrence can be edited
    if ( !CanSetRecurrenceL() )
        {
        User::Leave( KErrNotSupported );
        }

    if ( aRecurrence != ERecurrenceNot &&
            MESMRCalEntry::EESMRAllInSeries != iRecurrenceModRule )
        {
        // Make sure that mod rule is correct
        SetModifyingRuleL(
                MESMRCalEntry::EESMRAllInSeries );
        }

    CESMRRecurrenceInfoHandler* recurrenceHandler =
            CESMRRecurrenceInfoHandler::NewLC( *iEntry );

    recurrenceHandler->SetRecurrenceL( aRecurrence, aUntil );

    CleanupStack::PopAndDestroy( recurrenceHandler );

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::GetRecurrenceL
//
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::GetRecurrenceL(
        TESMRRecurrenceValue& aRecurrence,
        TTime& aUntil) const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    CESMRRecurrenceInfoHandler* recurrenceHandler =
            CESMRRecurrenceInfoHandler::NewLC( *iEntry, &iCalDb );

    recurrenceHandler->GetRecurrenceL( aRecurrence, aUntil );
    CleanupStack::PopAndDestroy( recurrenceHandler );
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::RecurrenceModRule
// ---------------------------------------------------------------------------
//
MESMRCalEntry::TESMRRecurrenceModifyingRule
    CESMRMeetingRequestEntry::RecurrenceModRule() const
    {
    return iRecurrenceModRule;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SetModifyingRuleL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::SetModifyingRuleL(
        TESMRRecurrenceModifyingRule aRule, const TBool aTypeChanging )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    CESMRRecurrenceInfoHandler* recHandler =
            CESMRRecurrenceInfoHandler::NewL( *iOrginalEntry );
    CleanupStack::PushL( recHandler );

    TESMRRecurrenceValue orginalRecurrence;
    TTime orginalUntil;

    recHandler->GetRecurrenceL(
            orginalRecurrence,
            orginalUntil);
    CleanupStack::PopAndDestroy( recHandler );
    recHandler = NULL;

    TBool modifyingEntry( ESMREntryHelper::IsModifyingEntryL( *iEntry ) );

    if ( MESMRCalEntry::EESMRAllInSeries == aRule &&
         IsStoredL() && !IsForwardedL() && !IsOpenedFromMail() && !aTypeChanging )
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

            delete iForwardEntry;
            iForwardEntry = NULL;

            delete iOrginalEntry;
            iOrginalEntry = NULL;

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

            __ASSERT_DEBUG( KErrNotFound != parentIndex, Panic(EESMRParentNotFound) );

            CCalEntry& parent = *entries[parentIndex];

            TPtrC description( parent.DescriptionL() );

            iEntry = CopyEntryL( iMRMailboxUtils, parent );

            // Adjust parent entry's start and end time to entry
            TCalTime start;
            TCalTime end;

            CESMRRecurrenceInfoHandler* recurrenceHandler =
                    CESMRRecurrenceInfoHandler::NewLC( parent );

            recurrenceHandler->GetFirstInstanceTimeL( start, end );
            CleanupStack::PopAndDestroy( recurrenceHandler );
            recurrenceHandler = NULL;

            iEntry->SetStartAndEndTimeL( start, end );

            iOrginalEntry = CopyEntryL( iMRMailboxUtils, *iEntry );

            if ( iEntry->MethodL() != entryMethod )
                {
                iEntry->SetMethodL( entryMethod );
                iOrginalEntry->SetMethodL( entryMethod );
                }

            iEntry->SetDescriptionL( description );
            iOrginalEntry->SetDescriptionL( description );

            CleanupStack::PopAndDestroy(); // entries
            CleanupStack::PopAndDestroy( instance );

            if ( EESMRRoleOrganizer == RoleL() && IsStoredL() )
                {
                // Increase sequence number
                TInt seqNo( iEntry->SequenceNumberL() + 1);
                iEntry->SetSequenceNumberL( seqNo );
                iOrginalEntry->SetSequenceNumberL( seqNo );
                }
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
// CESMRMeetingRequestEntry::SetAllDayEventL
//
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::SetAllDayEventL(
        TTime aStartDate,
        TTime aEndDate )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

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
    TTime endDate = aEndDate + TTimeIntervalDays( 1 );
    end.Set( endDate.DateTime().Year(),
             endDate.DateTime().Month(),
             endDate.DateTime().Day(),
             0,
             0,
             0,
             0 );

    startTime.SetTimeLocalL( start );
    stopTime.SetTimeLocalL( end );

    iEntry->SetStartAndEndTimeL( startTime, stopTime );

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::IsAllDayEventL
//
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsAllDayEventL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    CESMRCalUserUtil* entryUtil = CESMRCalUserUtil::NewLC( *iEntry );

    TBool allDayEvent( entryUtil->IsAlldayEventL() );

    CleanupStack::PopAndDestroy( entryUtil );

    return allDayEvent;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::IsStoredL
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsStoredL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

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
// CESMRMeetingRequestEntry::IsSentL
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsSentL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iOrginalEntry, Panic(EESMREntryNotExist ) );

    TBool retVal( EFalse );

    if ( EESMRRoleOrganizer == RoleL()  )
        {
        CCalEntry::TStatus status( iOrginalEntry->StatusL() );
        if ( CCalEntry::ENullStatus != status )
            { // When we send a request for the first time we set
              // it's status to some other value than ENullStatus
            retVal = ETrue;
            }
        }
    else
        {
        // In attendee mode, we have sent the entry, if it is stored to
        // calendar db and status is known and not declined.
        // Declined entries are not stored to calendar db.
        TESMRAttendeeStatus currentStatus( AttendeeStatusL() );
        if ( IsStoredL()
             && EESMRAttendeeStatusUnknown != currentStatus
             && EESMRAttendeeStatusDecline != currentStatus )
            {
            retVal = ETrue;
            }
        }


    return retVal;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::IsEntryEditedL
//
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsEntryEditedL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );
    __ASSERT_DEBUG( iOrginalEntry, Panic(EESMREntryNotExist ) );

    TBool edited( EFalse );

    if ( iOrginalEntry )
        { // edited if differs from the db entry
        edited = !( iEntry->CompareL( *iOrginalEntry ) );

        TESMRRole role( RoleL() );
        if ( !edited && EESMRRoleOrganizer != role )
            {
            // CCalEntry::CompareL does not compare attedee statuses
            CCalAttendee* dbAttendee =
                    iMRMailboxUtils.ThisAttendeeL( *iOrginalEntry );

            CCalAttendee* me =
                    iMRMailboxUtils.ThisAttendeeL( *iEntry );

            if ( dbAttendee &&  me )
                {
                edited = dbAttendee->StatusL() != me->StatusL();
                }
            }
        if ( !edited && EESMRRoleOrganizer == role &&
              iEntry->MethodL() != iOrginalEntry->MethodL() )
            {
            // For organizer CCalEntry::Compare does not compare entry's
            // method at all --> Need to compare ourselves.
            edited = ETrue;
            }

        // CCalEntry's CompareL doesn't check the priority value:
        if ( iOrginalEntry->PriorityL() != iEntry->PriorityL() )
            {
            edited = ETrue;
            }

        TPtrC description( iEntry->DescriptionL() );
        if ( description.CompareF( iOrginalEntry->DescriptionL() ) )
            {
            edited = ETrue;
            }

        TPtrC location( iEntry->LocationL() );
        if ( location.CompareF( iOrginalEntry->LocationL() ) )
            {
            edited = ETrue;
            }

        if( IsStoredL() )
            {
            if ( iCalDb.EntryViewL( *iEntry ) != iCalDb.EntryView() )
                {
                edited = ETrue;
                }
            }
        }



    return edited;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::IsEntryTypeChangedL
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsEntryTypeChangedL() const
    {
    FUNC_LOG;
    
    return iTypeChanged;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::GetAlarmL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::GetAlarmL(
        MESMRCalEntry::TESMRAlarmType& aAlarmType,
        TTime &aAlarmTime )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

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
            ESRM_ALARM_INFO_TABLE );

    TRAPD( err,
            alarmInfoHandler->GetAbsoluteAlarmTimeL(*iEntry, aAlarmTime) );

    if ( KErrNone == err )
        {
        aAlarmType = MESMRCalEntry::EESMRAlarmAbsolute;

        // only meeting request that is not allday event can have
        // relative alarm:
        if ( !IsAllDayEventL() )
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

    }


// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::OriginalEntry
// ---------------------------------------------------------------------------
//
const CCalEntry& CESMRMeetingRequestEntry::OriginalEntry()
    {
    FUNC_LOG;
    return *iOrginalEntry;
    }


// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::UpdateEntryAfterStoringL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::UpdateEntryAfterStoringL()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    CCalEntry* storedEntry = NULL;


    if ( IsRecurrentEventL() &&
         EESMRThisOnly == iRecurrenceModRule &&
         !IsForwardedL() && IsStoredL() &&
         !ESMREntryHelper::IsModifyingEntryL(*iEntry) )
        {
        // We have stored one instance of series.
        storedEntry = iCalDb.FetchEntryL(
                    iEntry->UidL(),
                    iOrginalEntry->StartTimeL() );
        }
    else
        {
        // We are dealing with single instance or with the series
        storedEntry = iCalDb.FetchEntryL(
                    iEntry->UidL(),
                    iEntry->RecurrenceIdL() );
        }


    __ASSERT_DEBUG( storedEntry, Panic( EESMREntryNotExist ) );
    CleanupStack::PushL( storedEntry );

    // Description needs to be fecthed explicitly into memory
    TPtrC description( storedEntry->DescriptionL() );


    delete iEntry;
    iEntry = NULL;
    iEntry = CopyEntryL( iMRMailboxUtils, *storedEntry );

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

    CCalEntry* temp  = CopyEntryL( iMRMailboxUtils, *iEntry );

    delete iOrginalEntry;
    iOrginalEntry = temp;

    CleanupStack::PopAndDestroy( storedEntry );
    }

// ----------------------------------------------------------------------------
// CMRCalEntry::UpdateComparativeEntry
// ----------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::UpdateComparativeEntry(
        CCalEntry* aNewComparativeEntry )
    {
    FUNC_LOG;

    if( iOrginalEntry ) // Update comparative entry
        {
        delete iOrginalEntry;
        iOrginalEntry = NULL;

        iOrginalEntry = aNewComparativeEntry;
        }

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SetDefaultValuesToEntryL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::SetDefaultValuesToEntryL()
    {
    FUNC_LOG;

    if ( !IsStoredL() )
        {
        SetPriorityL( EFSCalenMRPriorityNormal );

        // Get default alarm time from central repository
        TInt defaultAlarmTime=0;
        CRepository* repository = CRepository::NewLC( KCRUidCalendar );

        TInt err = KErrNotFound;

        if ( repository )
            {
            err = repository->Get(
                    KCalendarDefaultAlarmTime,
                    defaultAlarmTime );
            CleanupStack::PopAndDestroy( repository );
            }

        if ( err != KErrNone )
            {
            // By default 15 minutes if not found from central repository
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
        }

    // Set the default end time if not set by client
    if ( iEntry->StartTimeL().TimeUtcL() == iEntry->EndTimeL().TimeUtcL() )
        {
        // This value might also be read from cenrep
        TTimeIntervalHours KDefaultMeetingDuration(1);

        TCalTime newEndTime;
        newEndTime.SetTimeUtcL(
                iEntry->StartTimeL().TimeUtcL() + KDefaultMeetingDuration );
        iEntry->SetStartAndEndTimeL(iEntry->StartTimeL(), newEndTime);
        }

    iEntry->SetReplicationStatusL( CCalEntry::EOpen );

    //Original entry must be stored after the default values are set.
    //Otherwise it looks like settings are changed even though they haven't
    CCalEntry* temp = CopyEntryL( iMRMailboxUtils, *iEntry );

    delete iOrginalEntry;
    iOrginalEntry = temp;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::CloneEntryLC
// ---------------------------------------------------------------------------
//
CCalEntry* CESMRMeetingRequestEntry::CloneEntryLC(
        TESMRCalEntryType aType ) const
    {
    CCalEntry* entry =
        ESMRHelper::CopyEntryLC( *iEntry,
                                 iEntry->MethodL(),
                                 ESMRHelper::ECopyFull,
                                 TESMRCalendarEventType( aType ) );

    return entry;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::RoleL
// ---------------------------------------------------------------------------
//
TESMRRole CESMRMeetingRequestEntry::RoleL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    CESMRCalUserUtil* caluserUtil = CESMRCalUserUtil::NewLC( *iEntry );
    TESMRRole role = caluserUtil->PhoneOwnerRoleL();
    CleanupStack::PopAndDestroy( caluserUtil );


    return role;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::Conflicts
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::Conflicts() const
    {
    FUNC_LOG;
    return iConflictsExists;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::MarkMeetingCancelledL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::MarkMeetingCancelledL()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    TESMRRole role = RoleL();

    if ( EESMRRoleOrganizer == role )
        {
        iEntry->SetMethodL( CCalEntry::EMethodCancel );
        iEntry->SetStatusL( CCalEntry::ECancelled );
        }
    else if ( EESMRRoleRequiredAttendee == role ||
              EESMRRoleOptionalAttendee == role ||
              EESMRRoleNonParticipant == role )
        {
        ConstructReplyL( EESMRAttendeeStatusDecline );
        }

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::ConstructReplyL
//
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::ConstructReplyL(
            TESMRAttendeeStatus aStatus )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    CCalAttendee* attendee =
            iMRMailboxUtils.ThisAttendeeL( *iEntry );

    if ( !attendee )
        {

        User::Leave( KErrNotFound );
        }

    CCalAttendee::TCalStatus status(
            CCalAttendee::EDeclined );

    CCalEntry::TStatus entryStatus = CCalEntry::ENullStatus;
    switch ( aStatus )
        {
        case EESMRAttendeeStatusAccept:
            status = CCalAttendee::EAccepted;
            entryStatus = CCalEntry::EConfirmed;
            break;

        case EESMRAttendeeStatusTentative:
            status = CCalAttendee::ETentative;
            entryStatus = CCalEntry::ETentative;
            break;
        case EESMRAttendeeStatusDecline:
            status = CCalAttendee::EDeclined;
            entryStatus = CCalEntry::ECancelled;
            break;
        default:
            // This should not occur
            __ASSERT_DEBUG(
                    EFalse,
                    Panic( EESMRInvalidReplyType ) );

            User::Leave( KErrArgument );
            break;
        }

    attendee->SetStatusL(status);
    iEntry->SetStatusL(entryStatus);

    iEntry->SetMethodL( CCalEntry::EMethodReply );

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::IsEntryOutOfDateL
//
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsEntryOutOfDateL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    TBool outOfDate( EFalse );

    CCalEntry::TMethod method( iEntry->MethodL() );
    if ( IsOpenedFromMail() &&
         CCalEntry::EMethodCancel != method )
        {
        CCalEntry* dbEntry = NULL;

        TRAP_IGNORE( dbEntry = iCalDb.FetchEntryL( iEntry->UidL(),
                                                 iEntry->RecurrenceIdL() ) );

        CleanupStack::PushL( dbEntry );
        if ( dbEntry )
            {
            TInt currentSeqNo( iEntry->SequenceNumberL() );
            TInt dbEntrySeqNo( dbEntry->SequenceNumberL() );

            if ( currentSeqNo < dbEntrySeqNo )
                {
                outOfDate = ETrue;
                }
            }
        CleanupStack::PopAndDestroy( dbEntry );
        }


    return outOfDate;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::IsMeetingCancelledL
//
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsMeetingCancelledL() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    TBool retValue( EFalse );

    CCalEntry::TMethod method( iEntry->MethodL() );

    if ( CCalEntry::ECancelled == iEntry->StatusL() ||
         CCalEntry::EMethodCancel == method )
        {
        retValue = ETrue;
        }

    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::AttendeeStatusL
//
// ---------------------------------------------------------------------------
//
TESMRAttendeeStatus CESMRMeetingRequestEntry::AttendeeStatusL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    TESMRAttendeeStatus status(
            EESMRAttendeeStatusDecline );

    if ( iMRMailboxUtils.IsOrganizerL( *iEntry ) )
        {

        User::Leave( KErrNotFound );
        }

    CCalAttendee* attendee = iMRMailboxUtils.ThisAttendeeL( *iEntry );
    if (!attendee )
        {
        User::Leave( KErrNotFound );
        }

    CCalAttendee::TCalStatus attendeeStatus( attendee->StatusL() );
    switch ( attendeeStatus )
        {
        case CCalAttendee::ENeedsAction:
            status = EESMRAttendeeStatusUnknown;
            break;

        case CCalAttendee::EAccepted:
            status = EESMRAttendeeStatusAccept;
            break;

        case CCalAttendee::ETentative:
            status = EESMRAttendeeStatusTentative;
            break;

        case CCalAttendee::EDeclined:
            status = EESMRAttendeeStatusDecline;
            break;

        default:
            status = EESMRAttendeeStatusDecline;
            break;
        }


    return status;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::IsForwardedL
//
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsForwardedL() const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    TBool retValue( EFalse );
    if ( iForwardEntry )
        {
        retValue = ETrue;
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SwitchToForwardL
//
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::SwitchToForwardL()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    CCalEntry* temp = CopyEntryL( iMRMailboxUtils, *iEntry );

    delete iForwardEntry;
    iForwardEntry = temp;

    RPointerArray<CCalAttendee>& attendeeList =
                                    iForwardEntry->AttendeesL();

    while ( attendeeList.Count() )
        {
        //remove attendees from entry that is to be forwarded
        iForwardEntry->DeleteAttendeeL( 0 );
        }

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SwitchToOrginalL
//
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::SwitchToOrginalL()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    // We just delete the forwarded entry
    delete iForwardEntry;
    iForwardEntry = NULL;

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SwitchToOrginalL
//
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::ConfirmEntryL()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    if ( iForwardEntry &&
         iForwardEntry->StatusL() == CCalEntry::ENullStatus )
        {
        iForwardEntry->SetStatusL( CCalEntry::EConfirmed );
        }
    else if ( RoleL() == EESMRRoleOrganizer ||
            iEntry->StatusL() == CCalEntry::ENullStatus )
        {
        iEntry->SetStatusL( CCalEntry::EConfirmed );
        }

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::OccursInPastL
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::OccursInPastL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    TTime currentTimeUtc;
    currentTimeUtc.UniversalTime();

    TTime startTimeUtc = iEntry->StartTimeL().TimeUtcL();


    return (startTimeUtc < currentTimeUtc);
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::EntryAttendeeInfoL
// ---------------------------------------------------------------------------
//
MESMRMeetingRequestEntry::TESMREntryInfo
        CESMRMeetingRequestEntry::EntryAttendeeInfoL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

#ifdef _DEBUG

    __ASSERT_DEBUG( EESMRRoleOrganizer != RoleL(), Panic(EESMRInvalidRole) );

#else

    if ( EESMRRoleOrganizer == RoleL() )
        {
        // Info cannot be resolved in organzier role
        User::Leave( KErrNotSupported );
        }
#endif

    TESMRAttendeeStatus attendeeStatus(
            AttendeeStatusL() );

    MESMRMeetingRequestEntry::TESMREntryInfo info =
            EESMREntryInfoNormal;

    if ( IsEntryOutOfDateL() )
        {
        info = EESMREntryInfoOutOfDate;
        }
    else if( IsMeetingCancelledL() )
        {
        info = EESMREntryInfoCancelled;
        }
    else if ( IsSentL() )
        {
        info = EESMREntryInfoAccepted;
        if ( EESMRAttendeeStatusTentative == attendeeStatus )
            {
            info = EESMREntryInfoTentativelyAccepted;
            }
        }
    else if ( OccursInPastL() )
        {
        info = EESMREntryInfoOccursInPast;
        }


    return info;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SetPriorityL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::SetPriorityL(
        TUint aPriority )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    iEntry->SetPriorityL( aPriority );

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::GetPriorityL
// ---------------------------------------------------------------------------
//
TUint CESMRMeetingRequestEntry::GetPriorityL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    TUint entryPriority = iEntry->PriorityL();

    if ( entryPriority != EFSCalenMRPriorityLow &&
         entryPriority != EFSCalenMRPriorityNormal &&
         entryPriority != EFSCalenMRPriorityHigh )
        {
        entryPriority = EFSCalenMRPriorityNormal;
        }


    return entryPriority;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::GetAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::GetAttendeesL(
        RArray<CCalAttendee*>& aAttendeeArray,
        TUint aFilterFlags ) const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    CESMRCalUserUtil* caluserUtil = CESMRCalUserUtil::NewLC( *iEntry );
    caluserUtil->GetAttendeesL( aAttendeeArray, aFilterFlags );
    CleanupStack::PopAndDestroy( caluserUtil );

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::ValidateEntryL
// ---------------------------------------------------------------------------
//
CCalEntry* CESMRMeetingRequestEntry::ValidateEntryL()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );
    __ASSERT_DEBUG( iOrginalEntry, Panic(EESMREntryNotExist ) );

    CCalEntry* entry = NULL;

    if ( iForwardEntry )
        {
        entry = CopyEntryL( iMRMailboxUtils, *iForwardEntry );
        }

    if ( !entry && IsRecurrentEventL() &&
         EESMRThisOnly == iRecurrenceModRule &&
         !IsForwardedL() && IsStoredL() &&
         !ESMREntryHelper::IsModifyingEntryL(*iEntry) )
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
                            iOrginalEntry->StartTimeL() );

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
                                iOrginalEntry->StartTimeL(),
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
            iMRMailboxUtils.SetPhoneOwnerL( *entry );

            CleanupStack::Pop( entry );
            }
        }

    if ( !entry )
        {
        entry = CopyEntryL( iMRMailboxUtils, *iEntry );
        }

    if ( iRemoveAttachments )
        {
        // Remove attachments from the entry
        TInt count( entry->AttachmentCountL() );
        
        for ( TInt i = 0; i < count; ++i )
            {
            CCalAttachment* attachment = entry->AttachmentL( i );
            entry->DeleteAttachmentL( *attachment );
            }
        }

    return entry;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::FetchConflictingEntriesL
// ---------------------------------------------------------------------------
//
TInt CESMRMeetingRequestEntry::FetchConflictingEntriesL(
        RPointerArray<CCalEntry>& aEntryArray)
    {
    FUNC_LOG;

    TInt ret( KErrNone );

    CESMRConflictChecker* conflictChecker =
                CESMRConflictChecker::NewL(iCalDb);
    CleanupStack::PushL( conflictChecker );


    conflictChecker->FindConflictsL( *iEntry, aEntryArray );

    CleanupStack::PopAndDestroy( conflictChecker );

    if ( aEntryArray.Count() == 0 )
        {
        ret = KErrNotFound;
        }


    return ret;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::IsSyncObjectPresent
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsSyncObjectPresent() const
    {
    FUNC_LOG;
    TBool retValue(EFalse);

    if ( iESMRInputParams && iESMRInputParams->iMRInfoObject)
        {
        retValue = ETrue;
        }

    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SyncObjectL
// ---------------------------------------------------------------------------
//
MMRInfoObject& CESMRMeetingRequestEntry::SyncObjectL()
    {
    FUNC_LOG;
    if ( !IsSyncObjectPresent() )
        {
        User::Leave(KErrNotSupported);
        }

    return *iESMRInputParams->iMRInfoObject;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::ValidateSyncObjectL
// ---------------------------------------------------------------------------
//
MMRInfoObject& CESMRMeetingRequestEntry::ValidateSyncObjectL()
    {
    FUNC_LOG;
    if ( EESMRRoleOrganizer == RoleL() )
        {
        User::Leave( KErrNotSupported );
        }

    MMRInfoObject& syncObject = SyncObjectL();

    CCalAttendee* thisAttendee =
            iMRMailboxUtils.ThisAttendeeL( *iEntry );

    RPointerArray<MMRAttendee> attendeeArray =
            syncObject.AttendeesL();

    TBool found( EFalse );
    TInt attendeeCount( attendeeArray.Count() );
    for (TInt i (0); i < attendeeCount && !found; ++i )
        {
        MMRAttendee* attendee = attendeeArray[i];

        TPtrC calEntryAddress( thisAttendee->Address() );
        TPtrC infoAddress( attendee->Address() );
        if ( 0 == infoAddress.Compare(calEntryAddress ) )
            {
            found = ETrue;

            MMRInfoObject::TResponse entryResp(
                    MMRInfoObject::EMrCmdResponseAccept );

            CCalAttendee::TCalStatus calEntryAttStatus(
                    thisAttendee->StatusL() );

            MMRAttendee::TAttendeeStatus infoAttStatus(
                    MMRAttendee::EMRAttendeeActionAccepted );

            if ( CCalAttendee::ETentative == calEntryAttStatus)
                {
                infoAttStatus = MMRAttendee::EMRAttendeeActionTentative;
                entryResp = MMRInfoObject::EMrCmdResponseTentative;
                }
            else if ( CCalAttendee::EDeclined == calEntryAttStatus ||
                      CCalEntry::EMethodCancel == iEntry->MethodL() )
                {
                infoAttStatus = MMRAttendee::EMRAttendeeActionDeclined;
                entryResp = MMRInfoObject::EMrCmdResponseDecline;
                }

            attendee->SetAttendeeStatusL(infoAttStatus);
            syncObject.SetMRResponseL( entryResp );
            }
        }

    __ASSERT_DEBUG( found, Panic(EESMRNoInfoObjectAttendeeFound) );

    return syncObject;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::StartupParameters
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::StartupParameters(
        TESMRInputParams& aStartupParams) const
    {
    FUNC_LOG;
    TBool retValue( EFalse );

    if ( iESMRInputParams )
        {
        retValue = ETrue;

        aStartupParams.iCalEntry       = iESMRInputParams->iCalEntry;
        aStartupParams.iMRInfoObject   = iESMRInputParams->iMRInfoObject;
        aStartupParams.iMailMessage    = iESMRInputParams->iMailMessage;
        aStartupParams.iMailClient     = iESMRInputParams->iMailClient;        
        aStartupParams.iSpare          = iESMRInputParams->iSpare;
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::AttendeeCountL
// ---------------------------------------------------------------------------
//
TInt CESMRMeetingRequestEntry::AttendeeCountL(
        TUint aFilterFlags ) const
    {
    FUNC_LOG;

    TInt attendeeCount(0);

    RArray<CCalAttendee*> attendeeArray;
    CleanupClosePushL( attendeeArray );

    GetAttendeesL(
            attendeeArray,
            aFilterFlags );

    attendeeCount = attendeeArray.Count();

    CleanupStack::PopAndDestroy( &attendeeArray );


    return attendeeCount;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::RemoveInstanceFromSeriesL
// ---------------------------------------------------------------------------
//
CCalEntry* CESMRMeetingRequestEntry::RemoveInstanceFromSeriesL()
    {
    FUNC_LOG;

    CCalEntry* retEntry = NULL;

    if ( IsRecurrentEventL() &&
            EESMRThisOnly == iRecurrenceModRule &&
            !IsForwardedL() )
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
                        iOrginalEntry->StartTimeL().TimeUtcL() );

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
// CESMRMeetingRequestEntry::IsOpenedFromMail
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::IsOpenedFromMail() const
    {
    FUNC_LOG;
    TBool openedFromMail( EFalse );

    if ( iESMRInputParams )
        {
        openedFromMail = ETrue;
        }

    return openedFromMail;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::GetAddedAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::GetAddedAttendeesL(
        RArray<CCalAttendee*>& aAttendeeArray,
        TUint aFilterFlags ) const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    CESMRCalUserUtil* caluserUtil = CESMRCalUserUtil::NewLC( *iEntry );
    caluserUtil->GetAttendeesL( aAttendeeArray, aFilterFlags );
    CleanupStack::PopAndDestroy( caluserUtil );
    caluserUtil = NULL;

    RArray<CCalAttendee*> orgAttendees;
    CleanupClosePushL( orgAttendees );

    caluserUtil = CESMRCalUserUtil::NewLC( *iParameterEntry );
    caluserUtil->GetAttendeesL( orgAttendees, aFilterFlags );

    CleanupStack::PopAndDestroy( caluserUtil );
    caluserUtil = NULL;

    TInt index( 0 );
    while( index < aAttendeeArray.Count() )
        {
        TPtrC attendeeEmail( aAttendeeArray[index]->Address() );

        TBool found( EFalse );
        TInt orgAttendeeCount( orgAttendees.Count() );
        for( TInt i(0); (i < orgAttendeeCount) && !found; ++i )
            {
            TPtrC orgAttendeeEmail( orgAttendees[i]->Address() );
            if ( 0 == attendeeEmail.CompareF(orgAttendeeEmail) )
                {
                found = ETrue;
                }
            }

        if ( found )
            {
            // Attendee was not found from orginal list
            // --> it is added
            aAttendeeArray.Remove( index );
            }
        else
            {
            ++index;
            }
        }

    CleanupStack::PopAndDestroy( &orgAttendees );

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::GetRemovedAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::GetRemovedAttendeesL(
        RArray<CCalAttendee*>& aAttendeeArray,
        TUint aFilterFlags ) const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    CESMRCalUserUtil* caluserUtil = CESMRCalUserUtil::NewLC( *iParameterEntry );
    caluserUtil->GetAttendeesL( aAttendeeArray, aFilterFlags );
    CleanupStack::PopAndDestroy( caluserUtil );
    caluserUtil = NULL;

    RArray<CCalAttendee*> currentAttendees;
    CleanupClosePushL( currentAttendees );

    caluserUtil = CESMRCalUserUtil::NewLC( *iEntry );
    caluserUtil->GetAttendeesL( currentAttendees, aFilterFlags );

    CleanupStack::PopAndDestroy( caluserUtil );
    caluserUtil = NULL;

    TInt index( 0 );
    while( index < aAttendeeArray.Count() )
        {
        TPtrC attendeeEmail( aAttendeeArray[index]->Address() );

        TBool found( EFalse );
        TInt orgAttendeeCount( currentAttendees.Count() );
        for( TInt i(0); (i < orgAttendeeCount) && !found; ++i )
            {
            TPtrC curAttendeeEmail( currentAttendees[i]->Address() );
            if ( 0 == attendeeEmail.CompareF(curAttendeeEmail) )
                {
                found = ETrue;
                }
            }

        if ( found )
            {
            // Attendee was found from orginal list
            // --> It has not been removed from MR
            aAttendeeArray.Remove( index );
            }
        else
            {
            ++index;
            }
        }

    CleanupStack::PopAndDestroy( &currentAttendees );

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::UpdateChildEntriesSeqNumbersL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::UpdateChildEntriesSeqNumbersL()
    {
    FUNC_LOG;

    if ( MESMRCalEntry::EESMRAllInSeries == iRecurrenceModRule &&
         IsStoredL() )
        {
        RCPointerArray<CCalEntry> childEntries;
        CleanupClosePushL( childEntries );

        // Fetch all entries (this and child entries
        iCalDb.EntryView()->FetchL(
                iEntry->UidL(),
                childEntries );

        // Next: Remove parent entry from the array
        TBool removed( EFalse );
        TInt entryCount( childEntries.Count() );
        for ( TInt i(0); (i < entryCount) && !removed; ++i  )
            {
            CCalEntry* entry = childEntries[i];

            if ( !ESMREntryHelper::IsModifyingEntryL( *entry) )
                {
                removed = ETrue;
                childEntries.Remove( i );
                delete entry;
                }
            entry = NULL;
            }

        TInt childCount( childEntries.Count() );
        if ( childCount )
            {
            for (TInt i(0); i < childCount; ++i )
                {
                CCalEntry* child = childEntries[i];
                TInt childSeqNo( child->SequenceNumberL() );
                child->SetSequenceNumberL( childSeqNo + 1);
                }

            TInt updatedChilds;
            iCalDb.EntryView()->StoreL(childEntries, updatedChilds);
            }

        CleanupStack::PopAndDestroy(); // childEntries
        }

    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::CurrentPluginL
// ---------------------------------------------------------------------------
//
TESMRMailPlugin CESMRMeetingRequestEntry::CurrentPluginL()
    {
    FUNC_LOG;

    if ( EESMRUnknownPlugin == iCurrentFSEmailPlugin)
        {
        CESMRFsMailboxUtils* fsMbUtils =
                CESMRFsMailboxUtils::NewL( iMRMailboxUtils );
        CleanupStack::PushL( fsMbUtils );

        iCurrentFSEmailPlugin = fsMbUtils->FSEmailPluginForEntryL( *iEntry );
        CleanupStack::PopAndDestroy( fsMbUtils );
        fsMbUtils = NULL;
        }

    return iCurrentFSEmailPlugin;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::UpdateTimeStampL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::UpdateTimeStampL()
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

    TTime currentUTCTime;
    currentUTCTime.UniversalTime();

    TCalTime currentTime;
    currentTime.SetTimeUtcL( currentUTCTime );

    iEntry->SetDTStampL( currentTime );
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::AnyInstancesBetweenTimePeriodL
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::AnyInstancesBetweenTimePeriodL(
            TTime& aStart,
            TTime& aEnd )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );
    __ASSERT_DEBUG( iOrginalEntry, Panic(EESMREntryNotExist ) );

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
// CESMRMeetingRequestEntry::GetFirstInstanceStartAndEndTimeL
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::GetFirstInstanceStartAndEndTimeL(
            TTime& aStart,
            TTime& aEnd )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic(EESMREntryNotExist ) );

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

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::CalendarOwnerAddressL
// ---------------------------------------------------------------------------
//
const TDesC& CESMRMeetingRequestEntry::CalendarOwnerAddressL() const
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iEntry, Panic( EESMREntryNotExist ) );

    CCalUser* po = iEntry->PhoneOwnerL();

    __ASSERT_DEBUG( po, Panic( EESMRPhoneOwnerNotSet ) );

    return po->Address();
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::MailboxUtils
// ---------------------------------------------------------------------------
//
CMRMailboxUtils& CESMRMeetingRequestEntry::MailboxUtils() const
    {
    return iMRMailboxUtils;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::GetDBMgr
// ---------------------------------------------------------------------------
//
MESMRCalDbMgr& CESMRMeetingRequestEntry::GetDBMgr()
    {
    return iCalDb;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SupportsCapabilityL
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::SupportsCapabilityL( 
        MESMRCalEntry::TMREntryCapability aCapability ) const
    {
    TBool retValue( EFalse );
    
    switch( aCapability )
        {
        case MESMRCalEntry::EMRCapabilityAttachments:
            {
            CESMRFsMailboxUtils* fsMbUtils =
                    CESMRFsMailboxUtils::NewL( iMRMailboxUtils );
            CleanupStack::PushL( fsMbUtils );

            retValue = fsMbUtils->DefaultMailboxSupportCapabilityL(
                    CESMRFsMailboxUtils::EMRCapabilityAttachment );
            CleanupStack::PopAndDestroy( fsMbUtils );        
            }
            break;
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::ContainsRemoteAttachmentsL
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::ContainsRemoteAttachmentsL()
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
// CESMRMeetingRequestEntry::SendCanellationAvailable
// ---------------------------------------------------------------------------
//
TBool CESMRMeetingRequestEntry::SendCanellationAvailable ()
	{
	return iSendCanellation;
	}

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SetSendCanellationAvailable
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::SetSendCanellationAvailable (
        TBool aSendCanellation )
	{
	iSendCanellation = aSendCanellation;
	}

// ---------------------------------------------------------------------------
// CESMRMeetingRequestEntry::SetTypeChanged
// ---------------------------------------------------------------------------
//
void CESMRMeetingRequestEntry::SetTypeChanged( TBool aTypeChanged )
    {
    FUNC_LOG;
    
    iTypeChanged = aTypeChanged;
    }

// EOF
