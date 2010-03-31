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
*  Version     : %version: e002sa33#14 %
*
*/


// INCLUDE FILES

#include "cesmrentryprocessor.h"
#include "esmrinternaluid.h"
#include "mesmrcalentry.h"
#include "esmrhelper.h"
#include "esmrentryhelper.h"
#include "cesmrfsmailboxutils.h"
#include "cesmrmeetingrequestentry.h"
#include "cesmrconflictchecker.h"
#include "FreestyleEmailUiConstants.h"
#include "cesmrrecurrenceinfohandler.h"
#include "emailtrace.h"

#include <e32std.h>
#include <calentry.h>
#include <calinstance.h>
#include <calrrule.h>
#include <caluser.h>
#include <caltime.h>
#include <caleninterimutils2.h>
#include <cmrmailboxutils.h>
#include <magnentryui.h>
#include <ct/rcpointerarray.h>

/// Unnamed namespace for local definitions
namespace {

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
    // Invalid processor mode
    EESMRProcessorInvalidMode
    };

/**
 * Raises panic.
 * @param aPanic Panic code
 */
void Panic(TESMREntryProcessorPanic aPanic)
    {
    User::Panic( KESMRProcessorPanicTxt(), aPanic);
    }

#endif // _DEBUG

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMREntryProcessor::CESMREntryProcessor
// ---------------------------------------------------------------------------
//
CESMREntryProcessor::CESMREntryProcessor(
        MESMRCalDbMgr& aDbMgr )
:   iDbMgr( aDbMgr )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::~CESMREntryProcessor
// ---------------------------------------------------------------------------
//
CESMREntryProcessor::~CESMREntryProcessor()
    {
    FUNC_LOG;
    delete iMRMailboxUtils;
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
    iMRMailboxUtils = CMRMailboxUtils::NewL();
    iMbUtils = CESMRFsMailboxUtils::NewL( *iMRMailboxUtils );
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::NewL
// ---------------------------------------------------------------------------
//
CESMREntryProcessor* CESMREntryProcessor::NewL(
        TAny* aDbMgr )
    {
    FUNC_LOG;

    MESMRCalDbMgr* dbMgr = static_cast<MESMRCalDbMgr*>(aDbMgr);
    return CreateL( *dbMgr );
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::CreateL
// ---------------------------------------------------------------------------
//
CESMREntryProcessor* CESMREntryProcessor::CreateL(
            MESMRCalDbMgr& aDbMgr )
    {
    FUNC_LOG;

    CESMREntryProcessor* self = new (ELeave) CESMREntryProcessor( aDbMgr );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ScenarioData
// ---------------------------------------------------------------------------
//
const TESMRScenarioData& CESMREntryProcessor::ScenarioData() const
    {
    FUNC_LOG;
    return iScenData;
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ContainsProcessedEntry
// ---------------------------------------------------------------------------
//
TBool CESMREntryProcessor::ContainsProcessedEntry() const
    {
    FUNC_LOG;
    return (iESMREntry != NULL);
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ProcessL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::ProcessL(
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
    if ( iParams && iParams->iSpare )
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
void CESMREntryProcessor::ProcessL(
            const MAgnEntryUi::TAgnEntryUiInParams* aParams,
            CCalEntry& aEntry,
            TBool aSetDefaultValuesToEntry,
            const TBool aTypeChanging)
    {
    FUNC_LOG

    iParams = aParams;
    iMREntries = NULL;
    iCalEntry = &aEntry;

    // Set initial values to scenario data
    iScenData.iRole       = EESMRRoleUndef;
    iScenData.iViewMode   = EESMRViewUndef;
    iScenData.iCallingApp = EESMRAppUndef;

    SetPhoneOwnerL( aEntry, aEntry );

    CreateESMREntryL( aEntry, aSetDefaultValuesToEntry );
    CheckOrganizerL( aEntry );

    // When creating a new repeating meeting and adding participants,
    // meeting request editor is opened and all occurences should be edited
    if ( ( iESMREntry->IsRecurrentEventL() &&
            MAgnEntryUi::ECreateNewEntry == iParams->iEditorMode ) ||
         ( ESMREntryHelper::IsRepeatingMeetingL( aEntry ) && aTypeChanging ) )  
        {
        iESMREntry->SetModifyingRuleL(
                    MESMRMeetingRequestEntry::EESMRAllInSeries, aTypeChanging );
        }
    else //Default case
        {
        iESMREntry->SetModifyingRuleL(
                    MESMRMeetingRequestEntry::EESMRThisOnly, aTypeChanging );
        }

    if ( iParams && MAgnEntryUi::ECreateNewEntry ==
            iParams->iEditorMode &&
                aSetDefaultValuesToEntry )
        {
        iESMREntry->SetDefaultValuesToEntryL();
        }


    CheckViewModeL();

    if ( !IsDataOk() )
        {
        // Data is not ok --> Leave
        User::Leave( KErrArgument );
        }

    // Check mail parameters
    if ( iParams && iParams->iSpare )
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
void CESMREntryProcessor::ProcessL(
            RPointerArray<CCalEntry>* aEntries )
    {
    FUNC_LOG;

    __ASSERT_DEBUG( aEntries->Count() > 0,
                    Panic( EESMRProcessorEmptyEntryArray ) );

    iMREntries = aEntries;
    iCalEntry = (*iMREntries)[ 0 ];

    // Set initial values to scenario data
    iScenData.iRole       = EESMRRoleUndef;
    iScenData.iViewMode   = EESMRViewUndef;
    iScenData.iCallingApp = EESMRAppUndef;

    for ( TInt i = 0; i < iMREntries->Count(); ++i )
        {
        SetPhoneOwnerL( *(*iMREntries)[ i ], *iCalEntry );
        }

    CreateESMREntryL( *iCalEntry, ETrue );
    CheckOrganizerL( *iCalEntry );

    // When creating a new repeating meeting and adding participants,
    // meeting request editor is opened and all occurences should be edited
    if ( iESMREntry->IsRecurrentEventL()
         && iParams
         && MAgnEntryUi::ECreateNewEntry == iParams->iEditorMode )
    	{
    	iESMREntry->SetModifyingRuleL(
    			    MESMRMeetingRequestEntry::EESMRAllInSeries );
    	}
    else //Default case
    	{
    	iESMREntry->SetModifyingRuleL(
    	            MESMRMeetingRequestEntry::EESMRThisOnly );
    	}

    if ( iParams && MAgnEntryUi::ECreateNewEntry == iParams->iEditorMode )
        {
        iESMREntry->SetDefaultValuesToEntryL();
        }
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ProcessL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::ProcessL(
            CCalInstance& aInstance )
    {
    FUNC_LOG;

    RCPointerArray<CCalEntry> entries;
    CleanupClosePushL( entries );

    // Fetches parent entry. We need to construct entry with
    // correct time.
    CCalEntry &entry = aInstance.Entry();
    CCalEntry* instanceEntry = ESMRHelper::CopyEntryL(
            entry,
            entry.MethodL(),
            ESMRHelper::ECopyFull );

    entries.Append(instanceEntry);

    // Setting start and end time for child entry
    ESMREntryHelper::CheckRepeatUntilValidityL(
            *instanceEntry,
            aInstance.StartTimeL() );

    ESMREntryHelper::SetInstanceStartAndEndL(
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

    // entries
    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::SwitchProcessorToModeL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::SwitchProcessorToModeL(
        TMRProcessorMode aMode )
    {
    FUNC_LOG;

    switch ( aMode )
        {
        case EMRProcessorModeView:
            SwitchToViewL();
            break;
        case EMRProcessorModeEdit:
            SwitchToEditL();
            break;
        case EMRProcessorModeForward:
            SwitchToForwardL();
            break;
        case EMRProcessorModeTrack:
            SwitchToTrackL();
            break;
        default:
            __ASSERT_DEBUG( EFalse, Panic( EESMRProcessorInvalidMode) );
        }
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ESMREntryL
// ---------------------------------------------------------------------------
//
MESMRCalEntry& CESMREntryProcessor::ESMREntryL()
    {
    FUNC_LOG;

    CreateESMREntryL( *iCalEntry, ETrue );
    return *iESMREntry;
    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::ResetL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::ResetL()
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
void CESMREntryProcessor::ProcessOutputParametersL(
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
        //Copy entry LocalUid to input entry
        iCalEntry->SetLocalUidL( iESMREntry->Entry().LocalUidL() );

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
void CESMREntryProcessor::SetPhoneOwnerL( CCalEntry& aEntry,
                                          CCalEntry& aBaseEntry )
    {
    FUNC_LOG;

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
                    aEntry,
                    *(inputParams->iMailClient),
                    *(inputParams->iMailMessage) );
        }
    else if ( iParams )
        {
        err = iMbUtils->SetPhoneOwnerL(
                    aEntry,
                    iParams->iMailBoxId );
        }
    else
        {
        err = iMbUtils->SetPhoneOwnerL( aEntry );
        }

    if ( KErrNone != err )
        {
        User::Leave( KErrArgument );
        }

    // set iOwnerRole, use entry at index zero:
    if ( iMRMailboxUtils->IsOrganizerL( aBaseEntry ) )
        {
        iScenData.iRole = EESMRRoleOrganizer;
        }
    else
        {
        // We are not organizer. Let's see if we are in attendee list
        // or not.
        CCalAttendee* thisAttendee =
            iMRMailboxUtils->ThisAttendeeL( aBaseEntry );
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

    TInt32 callingAppUid = 0;
    if( iParams )
    	{
		callingAppUid = iParams->iCallingApp.iUid;
    	}
    
    switch ( callingAppUid )
        {
        case KUidCalendarApplication:
            {
            iScenData.iCallingApp = EESMRAppCalendar;
            iScenData.iViewMode = EESMRViewMR;
            if ( iParams && ( MAgnEntryUi::EViewEntry == iParams->iEditorMode ))
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
void CESMREntryProcessor::CreateESMREntryL( CCalEntry& aEntry,
                                            TBool aSetDefaultValuesToEntry )
    {
    FUNC_LOG;

    if ( !iESMREntry )
        {
        // ES MR Entry is created
        //CCalEntry& base = *((*iMREntries)[KFirstEntryIndex]);
        HBufC* description = NULL;
        TRAPD( err, description = aEntry.DescriptionL().AllocL() );
        CleanupStack::PushL( description );
        if ( KErrNone != err )
            {
            User::Leave( KErrCorrupt );
            }
        CleanupStack::PopAndDestroy( description );
        description = NULL;

        CCalEntry* entry =
                ESMRHelper::CopyEntryLC(
                        aEntry,
                        aEntry.MethodL(),
                        ESMRHelper::ECopyFull );

        CESMRConflictChecker* conflictChecker =
                CESMRConflictChecker::NewL(iDbMgr);
        CleanupStack::PushL( conflictChecker );

        RCPointerArray<CCalEntry> conflicts;
        CleanupClosePushL( conflicts );

        conflictChecker->FindConflictsL( *entry, conflicts );
        TBool conflictsExists( conflicts.Count() );

        if ( iParams &&
             KUidCalendarApplication == iParams->iCallingApp.iUid
             && aSetDefaultValuesToEntry )
            {
            // Setting start and end time for child entry
            ESMREntryHelper::CheckRepeatUntilValidityL(
                    *entry,
                    iParams->iInstanceDate );

            ESMREntryHelper::SetInstanceStartAndEndL(
                *entry,
                aEntry,
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
                *iMRMailboxUtils,
                iDbMgr,
                conflictsExists,
                inputParams );

        // conflicts
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy( conflictChecker );
        CleanupStack::PopAndDestroy( entry );

        SetPhoneOwnerToOwnEntryL();
        }

    }

// ---------------------------------------------------------------------------
// CESMREntryProcessor::CheckOrganizerL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::CheckOrganizerL( CCalEntry& aEntry )
    {
    FUNC_LOG;

    if ( EESMRRoleOrganizer == iScenData.iRole )
        {
        CCalUser *phoneOwner = aEntry.PhoneOwnerL();

        if ( !phoneOwner )
            {
            User::Leave( KErrCorrupt );
            }

        CCalUser* organizer = ESMRHelper::CopyUserLC( *phoneOwner );

        // Ownership is transferred
        aEntry.SetOrganizerL( organizer );
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

// ---------------------------------------------------------------------------
// CESMREntryProcessor::SwitchToForwardL
// ---------------------------------------------------------------------------
//
void CESMREntryProcessor::SwitchToForwardL()
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
void CESMREntryProcessor::SwitchToEditL()
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
void CESMREntryProcessor::SwitchToViewL()
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
void CESMREntryProcessor::SwitchToTrackL()
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

// EOF

