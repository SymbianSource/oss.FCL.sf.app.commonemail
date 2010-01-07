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
*  Description : ESMR entry processor implementation
*  Version     : %version: e003sa40#13 %
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmrentryprocessor.h"
#include "cesmrcaldbmgr.h"
#include "cesmrmeetingrequestentry.h"
#include "cesmrconflictchecker.h"
#include "esmrhelper.h"
#include "esmrinternaluid.h"
#include "cesmrfsmailboxutils.h"

//<cmail>
#include "esmrdef.h"
//</cmail>
#include <e32std.h>
#include <calentry.h>
#include <calinstance.h>
#include <calrrule.h>
#include <caluser.h>
#include <caltime.h>
#include <CalenInterimUtils2.h>
#include <cmrmailboxutils.h>
#include <MAgnEntryUi.h>
//<cmail>
#include "FreestyleEmailUiConstants.h"
#include "cesmrrecurrenceinfohandler.h"
//</cmail>
#include "esmrentryhelper.h"
#include "tesmrinputparams.h"

/// Unnamed namespace for local definitions
namespace {

// Definition for first item index
const TInt KFirstEntryIndex = 0;

// Definition for zero interval
const TInt KZeroInterval = 0;

#ifdef _DEBUG

// Definition for module panic text
_LIT( KESMRProcessorPanicTxt, "ESMREntryProcessor" );

/**
 * ESMREntryProcessor panic codes
 */
enum TESMREntryProcessorPanic
    {
    // Input entry array is empty
    EESMRProcessorEmptyEntryArray = 1,
    // Too many input entries
    EESMRProcessorTooManyEntries = 2,
    // entries are not processed
    EESMRProcessorEntriesNotProcessed,
    // View mode for policy resolving
    // cannot be check.
    EESMRProcessorCannotCheckViewMode,
    };

/**
 * Raises panic.
 * @param aPanic Panic code
 */
void Panic(TESMREntryProcessorPanic aPanic)
    {

    User::Panic( KESMRProcessorPanicTxt(),
                 aPanic);
    }

/**
 * Logs scenatio data
 * @param aData Reference to scenario data.
 */
void LogScenatioData(
        const TESMRScenarioData& /*aScenarioData*/ )
    {




    }

#endif // _DEBUG

/**
 * Cleanup operation for RPointerArray.
 *
 * @param aArray Pointer to RPointerArray.
 */
void CalEntryPointerArrayCleanup( TAny* aArray )
    {
    RPointerArray<CCalEntry>* entryArray =
        static_cast<RPointerArray<CCalEntry>*>( aArray );

    entryArray->ResetAndDestroy();
    entryArray->Close();
    }

/**
 * Sets child entry's start and end time.
 *
 * @param aChild Reference to child entry
 * @param aParent Reference to parent entry
 * @param aChildEnd Child entry's start time
 */
void SetInstanceStartAndEndL(
        CCalEntry& aChild,
        const CCalEntry& aParent,
        const TCalTime& aChildStart )
    {
    TTime parentStart( aParent.StartTimeL().TimeUtcL() );
    TTime parentEnd( aParent.EndTimeL().TimeUtcL() );
    TTime childStart( aChildStart.TimeUtcL() );
    const TTime KNullTime( Time::NullTTime() );

    if ( KNullTime == parentStart ||
         KNullTime == parentEnd ||
         KNullTime == childStart )
        {
        // Invalid time --> Leaving
        User::Leave( KErrArgument );
        }

    TTimeIntervalMicroSeconds duration( KZeroInterval );
    duration = parentEnd.MicroSecondsFrom( parentStart );
    TCalTime childEnd;
    childEnd.SetTimeUtcL( childStart + duration );

    aChild.SetStartAndEndTimeL( aChildStart, childEnd );
    }

void CheckRepeatUntilValidityL(
        CCalEntry& aEntry,
        const TCalTime& aInstanceTime )
    {

    if ( ESMREntryHelper::IsRepeatingMeetingL(aEntry) &&
         !ESMREntryHelper::IsModifyingEntryL(aEntry) )
        {
        TCalRRule originalRRule;
        if ( aEntry.GetRRuleL( originalRRule ) )
            {
            TCalTime endTime = aEntry.EndTimeL();
            TCalTime startTime = aEntry.StartTimeL();

            TTimeIntervalMicroSeconds duration =
                endTime.TimeLocalL().MicroSecondsFrom(
                        startTime.TimeLocalL() );

            TTime instanceEndTime( aInstanceTime.TimeLocalL() );
            instanceEndTime += duration;
            TCalTime repeatUntilTime = originalRRule.Until();

            TDateTime repeat = repeatUntilTime.TimeLocalL().DateTime();
            TDateTime instanceEndDatetime = instanceEndTime.DateTime();

            if ( repeatUntilTime.TimeLocalL() < instanceEndTime )
                {
                // reset the recurrence so, that the repeat until
                // time is included in repeat range. This is done,
                // because when setting the instance start and end
                // time, the CCalEntry implementation clears the
                // recurrence information if the end time is
                // after repeat until end time.
                // CCalEntry forces the repeat until time to be
                // same than last entry's start time when storing the
                // entry to db. Reason uknown.
                TCalRRule rRule;
                rRule.SetDtStart( originalRRule.DtStart() );
                rRule.SetType( originalRRule.Type() );

                TCalTime newUntil;
                newUntil.SetTimeLocalL( instanceEndTime );
                rRule.SetUntil( newUntil );
                rRule.SetInterval( originalRRule.Interval() );
                aEntry.SetRRuleL( rRule );
                }
            }
        }
    }

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMREntryProcessor::CESMREntryProcessor
// ---------------------------------------------------------------------------
//
inline CESMREntryProcessor::CESMREntryProcessor(
    CMRMailboxUtils& aMRMailboxUtils,
    MESMRCalDbMgr& aDbMgr )
:   iMRMailboxUtils( aMRMailboxUtils ),
    iDbMgr( aDbMgr )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::~CESMREntryProcessor
// ---------------------------------------------------------------------------
//
EXPORT_C CESMREntryProcessor::~CESMREntryProcessor()
    {
    FUNC_LOG;
    delete iESMREntry;
    delete iMbUtils;
    }
    
// ---------------------------------------------------------------------------
// CESMREntryProcessor::ConstructL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::ConstructL()
    {
    FUNC_LOG;
    iMbUtils =
            CESMRFsMailboxUtils::NewL( iMRMailboxUtils );
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMREntryProcessor* CESMREntryProcessor::NewL(
        CMRMailboxUtils& aMRMailboxUtils,
        MESMRCalDbMgr& aDbMgr )
    {
    FUNC_LOG;

    CESMREntryProcessor* self =
        new (ELeave) CESMREntryProcessor(
            aMRMailboxUtils, aDbMgr );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    

    return self;
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ExtensionUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CESMREntryProcessor::ExtensionUid()
    {
    FUNC_LOG;
    return TUid::Uid( KESMRMREntryProcessorUid );
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ProcessL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREntryProcessor::ProcessL(
        const MAgnEntryUi::TAgnEntryUiInParams* aParams,
        RPointerArray<CCalEntry>* aEntries )
    {
    FUNC_LOG;

    iParams = aParams;
    ProcessL( aEntries );
    CheckViewModeL();

    if ( !IsDataOk() )
        {
        // Data is not ok --> Leave
        User::Leave( KErrArgument );
        }

    // Check mail parameters
    if ( iParams )
        {
        CCalEntry* entry = &iESMREntry->Entry();
        if(entry)
            {
            TBool recurrent(EFalse);
            recurrent = iESMREntry->IsRecurrentEventL();
        
            if(recurrent)
                {                
                if ( !ESMREntryHelper::IsModifyingEntryL(*entry) )
                    {
                    iESMREntry->SetModifyingRuleL( MESMRMeetingRequestEntry::EESMRAllInSeries );
                    
                    SetPhoneOwnerToOwnEntryL();
                    }
                }
            }
        }

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ProcessL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREntryProcessor::ProcessL(
            RPointerArray<CCalEntry>* aEntries )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( aEntries->Count() > 0,
                    Panic( EESMRProcessorEmptyEntryArray ) );
    
    iMREntries = aEntries;

    // Set initial values to scenario data
    iScenData.iRole       = EESMRRoleUndef;
    iScenData.iViewMode   = EESMRViewUndef;
    iScenData.iCallingApp = EESMRAppUndef;

    SetPhoneOwnerL();
    CreateESMREntryL();
    CheckOrganizerL();

    iESMREntry->SetModifyingRuleL(
            MESMRMeetingRequestEntry::EESMRThisOnly );

    if ( iParams && MAgnEntryUi::ECreateNewEntry == iParams->iEditorMode )
        {
        iESMREntry->SetDefaultValuesToEntryL();
        }

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ProcessL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREntryProcessor::ProcessL(
            CCalInstance& aInstance )
    {
    FUNC_LOG;

    RPointerArray<CCalEntry> entries;
    CleanupStack::PushL(
                TCleanupItem(
                    CalEntryPointerArrayCleanup,
                    &entries    ) );

    // Fetches parent entry. We need to construct entry with
    // correct time.
    CCalEntry &entry = aInstance.Entry();
    CCalEntry* instanceEntry = ESMRHelper::CopyEntryL(
            entry,
            entry.MethodL(),
            ESMRHelper::ECopyFull );
    
    entries.Append(instanceEntry);

    // Setting start and end time for child entry
    CheckRepeatUntilValidityL(
            *instanceEntry,
            aInstance.StartTimeL() );

    SetInstanceStartAndEndL(
            *instanceEntry,
            entry,
            aInstance.StartTimeL() );

    // Filling child with correct data
    // This can be replaced withCCalenInterimUtils2::PopulateChildFromParentL
    // after its memory leak has been fixed.
    ESMRHelper::PopulateChildFromParentL(
            *instanceEntry,
            entry );

    ProcessL( &entries );

    if ( !iESMREntry )
        {
        User::Leave( KErrArgument );
        }

    // Mark only this entry to be affected.
    iESMREntry->SetModifyingRuleL(
            MESMRMeetingRequestEntry::EESMRThisOnly );

    CleanupStack::PopAndDestroy( &entries );

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::SwitchToForwardL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREntryProcessor::SwitchToForwardL()
    {
    FUNC_LOG;

    if ( !iESMREntry )
        {
        // No entries has been processed yet.
        User::Leave( KErrNotReady );
        }

    iESMREntry->SwitchToForwardL();

    // Rest of the scenario data remains as it is
    iScenData.iViewMode   = EESMRForwardMR;

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::SwitchToViewL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREntryProcessor::SwitchToEditL()
    {
    FUNC_LOG;

    if ( !iESMREntry )
        {
        // No entries has been processed yet.
        User::Leave( KErrNotReady );
        }

    iESMREntry->SwitchToOrginalL();

    // Rest of the scenario data remains as it is
    iScenData.iViewMode   = EESMREditMR;

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::SwitchToViewL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREntryProcessor::SwitchToViewL()
    {
    FUNC_LOG;

    if ( !iESMREntry )
        {
        // No entries has been processed yet.
        User::Leave( KErrNotReady );
        }

    iESMREntry->SwitchToOrginalL();

    // Rest of the scenario data remains as it is
    iScenData.iViewMode   = EESMRViewMR;

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::SwitchToTrackL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREntryProcessor::SwitchToTrackL()
    {
    FUNC_LOG;

    if ( !iESMREntry )
        {
        // No entries has been processed yet.
        User::Leave( KErrNotReady );
        }

    iESMREntry->SwitchToOrginalL();

    // Rest of the scenario data remains as it is
    iScenData.iViewMode   = EESMRTrackingViewMR;
    iScenData.iRole       = EESMRRoleOrganizer;
    iScenData.iEntryType  = EESMREventTypeAppt;


    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ESMREntryL
// ---------------------------------------------------------------------------
//
EXPORT_C MESMRMeetingRequestEntry& CESMREntryProcessor::ESMREntryL()
    {
    FUNC_LOG;
    CreateESMREntryL();
    return *iESMREntry;
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ResetL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREntryProcessor::ResetL()
    {
    FUNC_LOG;

    iScenData.iRole       = EESMRRoleUndef;
    iScenData.iViewMode   = EESMRViewUndef;
    iScenData.iCallingApp = EESMRAppUndef;

    delete iESMREntry;
    iESMREntry = NULL;

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ProcessOutputParametersL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMREntryProcessor::ProcessOutputParametersL(
        MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
        TESMRCommand aCommand )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iESMREntry, Panic(EESMRProcessorEntriesNotProcessed) );

    if ( !iParams  ||
         EESMRForwardMR == iScenData.iViewMode  ||
         EESMRViewUndef == iScenData.iViewMode )
        {
        // No input parameters supplied --> Output parameters cannot be set
        return;
        }

    TBool handlingInstanceOnly(
            iESMREntry->IsRecurrentEventL() &&
            MESMRCalEntry::EESMRThisOnly == iESMREntry->RecurrenceModRule() );

    switch ( aCommand )
        {
        case EESMRCmdSendMR:
        case EESMRCmdSendMRUpdate:
        case EESMRCmdSaveMR:
        case EESMRCmdAcceptMR:
        case EESMRCmdTentativeMR:
            {
            aOutParams.iAction = MAgnEntryUi::EMeetingSaved;
            if ( handlingInstanceOnly )
                {
                aOutParams.iAction = MAgnEntryUi::EInstanceRescheduled;
                }
            }
            break;

        case EESMRCmdDeclineMR:
        case EESMRCmdDeleteMR:
            {
            aOutParams.iAction = MAgnEntryUi::EMeetingDeleted;
            if ( handlingInstanceOnly )
                {
                aOutParams.iAction = MAgnEntryUi::EInstanceDeleted;
                }
            }
            break;

        default:
            {
            aOutParams.iAction = MAgnEntryUi::ENoAction;
            }
            break;
        }

    if ( handlingInstanceOnly )
        {
        aOutParams.iNewInstanceDate =
                iESMREntry->Entry().StartTimeL();
        }
    else if ( MAgnEntryUi::ENoAction != aOutParams.iAction )
        {
        // Copy entry details to input entry
        CCalEntry& baseEntry = *((*iMREntries)[KFirstEntryIndex]);
        baseEntry.CopyFromL( iESMREntry->Entry() );

        TCalTime instanceTime = iESMREntry->Entry().StartTimeL();

        if ( MESMRCalEntry::EESMRAllInSeries == iESMREntry->RecurrenceModRule() )
            {
            // For recurrent event there might be exceptions
            // and therefore the first possible start time
            // is set to putput params
            TCalTime end;
            CESMRRecurrenceInfoHandler* recurrenceHandler =
                    CESMRRecurrenceInfoHandler::NewLC( iESMREntry->Entry() );

            recurrenceHandler->GetFirstInstanceTimeL(
                    instanceTime,
                    end );

            CleanupStack::PopAndDestroy( recurrenceHandler );
            recurrenceHandler = NULL;
            aOutParams.iNewInstanceDate = instanceTime;
            }
        else
            {
            aOutParams.iNewInstanceDate.SetTimeLocalL(
                    iESMREntry->Entry().StartTimeL().TimeLocalL() );
            }
        }
    else
        {
        aOutParams.iNewInstanceDate.SetTimeLocalL( Time::NullTTime() );
        }

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::SetPhoneOwnerL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::SetPhoneOwnerL()
    {
    FUNC_LOG;

    TInt count( iMREntries->Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        // Set phone owner for all entries
        CCalEntry& entry = *( (*iMREntries)[i] );

        TESMRInputParams* inputParams = NULL;
        if ( iParams )
            {
            inputParams = (TESMRInputParams*)(iParams->iSpare);
            }        
        
        TInt err(KErrNone);
        if ( inputParams && 
             inputParams->iMailClient &&
             inputParams->iMailMessage )
            {
            err = iMbUtils->SetPhoneOwnerL(
                        entry,
                        *(inputParams->iMailClient),
                        *(inputParams->iMailMessage) );            
            }
        else if ( iParams )
            {
            err = iMbUtils->SetPhoneOwnerL(
                        entry,
                        iParams->iMailBoxId );
            }
        else
            {
            err = iMbUtils->SetPhoneOwnerL( entry );
            }

        if ( KErrNone != err )
            {
            User::Leave( KErrArgument );
            }
        }

    // set iOwnerRole, use entry at index zero:
    CCalEntry& baseEntry = *( (*iMREntries)[KFirstEntryIndex] );
    if ( iMRMailboxUtils.IsOrganizerL( baseEntry ) )
        {
        iScenData.iRole = EESMRRoleOrganizer;
        }
    else
        {
        // We are not organizer. Let's see if we are in attendee list
        // or not.
        CCalAttendee* thisAttendee =
            iMRMailboxUtils.ThisAttendeeL( baseEntry );
        if ( thisAttendee )
            {
            switch ( thisAttendee->RoleL() )
                {
                case CCalAttendee::EOptParticipant:
                    {
                    iScenData.iRole = EESMRRoleOptionalAttendee;
                    break;
                    }
                case CCalAttendee::ENonParticipant:
                    {
                    iScenData.iRole = EESMRRoleNonParticipant;
                    break;
                    }
                case CCalAttendee::EReqParticipant: // fall through
                case CCalAttendee::EChair: // fall through
                default:
                    { // Note: chair MUST NOT be interpreted as organizer
                      // Req.participant is not correct either but has less
                      // side effects.
                    iScenData.iRole = EESMRRoleRequiredAttendee;
                    break;
                    }
                }
            }
        else
            {
            // Phone owner cannot be set --> Leave
            User::Leave( KErrCorrupt );
            }
        }

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::CheckViewModeL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::CheckViewModeL()
    {
    FUNC_LOG;

    // View mode cannot be checked
    __ASSERT_DEBUG(
            iESMREntry,
            Panic(EESMRProcessorCannotCheckViewMode) );

    TInt32 callingAppUid = iParams->iCallingApp.iUid;
    switch ( callingAppUid )
        {
        case KUidCalendarApplication:
            {
            iScenData.iCallingApp = EESMRAppCalendar;
            iScenData.iViewMode = EESMRViewMR;
            if ( MAgnEntryUi::EViewEntry == iParams->iEditorMode )
                {
                if ( iESMREntry->IsAllDayEventL() )
                    {
                    iScenData.iViewMode = EESMRViewAllDayMR;
                    }
                }
            else
                {
                iScenData.iViewMode = EESMREditMR;
                }
            break;
            }
        case KUidMceMailApplication:
        case KUidEsMailApplication:
        case KUidBVAApplication:
            {
            iScenData.iCallingApp = EESMRAppESEmail;
            iScenData.iViewMode = EESMRViewMR;

            if ( iESMREntry->IsAllDayEventL() )
                {
                iScenData.iViewMode = EESMRViewAllDayMR;
                }
            break;
            }
        default:
            {
            //error situation, should never come here
            // Setting data to undef --> will leave later on.
            iScenData.iViewMode =  EESMRViewUndef;
            break;
            }
        }

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::SetPhoneOwnerL
// ---------------------------------------------------------------------------
//
TBool CESMREntryProcessor::IsDataOk() const
    {
    FUNC_LOG;

#ifdef _DEBUG
    LogScenatioData( iScenData );
#endif

    TBool retVal(ETrue);

    // Check that all values has been set and there is no
    // undefined values. Policies cannot be resolved, if
    // scenario data is undefined.
    if ( EESMRRoleUndef == iScenData.iRole ||
         EESMRViewUndef == iScenData.iViewMode ||
         EESMRAppUndef == iScenData.iCallingApp )
        {
        retVal = EFalse;
        }



    return retVal;
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::CreateESMREntryL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::CreateESMREntryL()
    {
    FUNC_LOG;

   __ASSERT_DEBUG( iMREntries, Panic(EESMRProcessorEntriesNotProcessed) );

    if ( !iESMREntry )
        {
        // ES MR Entry is created
        CCalEntry& base = *((*iMREntries)[KFirstEntryIndex]);
        HBufC* description = NULL;
        TRAPD( err, description = base.DescriptionL().AllocL() );
        CleanupStack::PushL( description );
        if ( KErrNone != err )
            {
            User::Leave( KErrCorrupt );
            }
        CleanupStack::PopAndDestroy( description );
        description = NULL;

        CCalEntry* entry =
                ESMRHelper::CopyEntryLC(
                        base,
                        base.MethodL(),
                        ESMRHelper::ECopyFull );

        CESMRConflictChecker* conflictChecker =
                CESMRConflictChecker::NewL(iDbMgr);
        CleanupStack::PushL( conflictChecker );

        RPointerArray<CCalEntry> conflicts;
            CleanupStack::PushL(
                TCleanupItem(
                    CalEntryPointerArrayCleanup,
                    &conflicts    ) );

        conflictChecker->FindConflictsL( *entry, conflicts );
        TBool conflictsExists( conflicts.Count() );

        if ( iParams &&
             KUidCalendarApplication == iParams->iCallingApp.iUid )
            {
            // Setting start and end time for child entry
            CheckRepeatUntilValidityL(
                    *entry,
                    iParams->iInstanceDate );

            SetInstanceStartAndEndL(
                *entry,
                base,
                iParams->iInstanceDate );
            }

        TESMRInputParams* inputParams = NULL;
        if ( iParams )
            {
            inputParams = (TESMRInputParams*)(iParams->iSpare);
            }

        iESMREntry =
            CESMRMeetingRequestEntry::NewL(
                *entry,
                iMRMailboxUtils,
                iDbMgr,
                conflictsExists,
                inputParams );

        CleanupStack::PopAndDestroy(); // conflicts
        CleanupStack::PopAndDestroy( conflictChecker );
        CleanupStack::PopAndDestroy( entry );

        SetPhoneOwnerToOwnEntryL();
        }

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::CheckOrganizerL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::CheckOrganizerL()
    {
    FUNC_LOG;

    __ASSERT_DEBUG( iMREntries, Panic(EESMRProcessorEntriesNotProcessed) );

    if ( EESMRRoleOrganizer == iScenData.iRole )
        {
        CCalEntry& entry = *((*iMREntries)[KFirstEntryIndex]);

        CCalUser *phoneOwner = entry.PhoneOwnerL();

        if ( !phoneOwner )
            {
            User::Leave( KErrCorrupt );
            }

        CCalUser* organizer = ESMRHelper::CopyUserLC( *phoneOwner );

        // Ownership is transferred
        entry.SetOrganizerL( organizer );
        CleanupStack::Pop( organizer );
        }

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::SetPhoneOwnerToOwnEntryL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::SetPhoneOwnerToOwnEntryL()
    {
    FUNC_LOG;

    // Phone owner cannot be checked
    __ASSERT_DEBUG(
            iESMREntry,
            Panic(EESMRProcessorEntriesNotProcessed) );

    TESMRInputParams* inputParams = NULL;
    if ( iParams )
        {
        inputParams = (TESMRInputParams*)(iParams->iSpare);
        } 
    
    TInt err(KErrNone);
    if ( inputParams && 
         inputParams->iMailClient &&
         inputParams->iMailMessage )
        {
        err = iMbUtils->SetPhoneOwnerL(
                    iESMREntry->Entry(),
                    *(inputParams->iMailClient),
                    *(inputParams->iMailMessage) );            
        }
    else if ( iParams )
        {
        err = iMbUtils->SetPhoneOwnerL(
                iESMREntry->Entry(),
                    iParams->iMailBoxId );
        }
    else
        {
        err = iMbUtils->SetPhoneOwnerL( iESMREntry->Entry() );
        }

    User::LeaveIfError( err );

    }

// EOF

