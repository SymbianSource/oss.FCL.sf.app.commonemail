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
*  Version     : %version: e002sa33#11 %
*
*/


// INCLUDE FILES

#include "cmrcaleventprocessor.h"
#include "esmrinternaluid.h"
#include "mesmrcalentry.h"
#include "esmrhelper.h"
#include "esmrentryhelper.h"
#include "cmrcalentry.h"
#include "cesmrrecurrenceinfohandler.h"
#include "emailtrace.h"

#include <e32std.h>
#include <calentry.h>
#include <calinstance.h>
#include <calrrule.h>
#include <caluser.h>
#include <caltime.h>
#include <caleninterimutils2.h>
#include <magnentryui.h>
#include <ct/rcpointerarray.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KReplaceLineFeedChars, "\r\n" );
_LIT( KLineFeed, " \x2029");

#ifdef _DEBUG

// Definition for module panic text
_LIT( KMRCalEntryProcessor, "MRCalEntryProcessor" );

/**
 * ESMREntryProcessor panic codes
 */
enum TMRCalEntryProcessorPanic
    {
    // Empty input array
    EMRCalEventProcessorEmptyEntryArray,    
    // Invalid processor mode
    EMRCalEventProcessorInvalidType,
    // Invalid processor mode
    EMRCalEventProcessorInvalidMode,
    // Non-supported method called
    EMRCalEventProcessorNotSupported
    };

/**
 * Raises panic.
 * @param aPanic Panic code
 */
void Panic(TMRCalEntryProcessorPanic aPanic)
    {
    User::Panic( KMRCalEntryProcessor(), aPanic);
    }

#endif // _DEBUG

/**
 * Maps CCalEntry defined entry type to ESMR entry type.
 * 
 * @param aType Calendar entry type
 * @return TESMRCalendarEventType Entry type
 */
TESMRCalendarEventType MapCalEntryTypeToMRType( 
        CCalEntry::TType aType )
    {
    TESMRCalendarEventType returnType(EESMREventTypeAppt);
        
    switch(aType)
        {
        case CCalEntry::EAppt:
            {
            returnType = EESMREventTypeAppt;
            break;
            }
        case CCalEntry::ETodo:
            {
            returnType = EESMREventTypeETodo;
            break;
            }
        case CCalEntry::EEvent:
            {
            returnType = EESMREventTypeEEvent;
            break;
            }
        case CCalEntry::EReminder:
            {
            returnType = EESMREventTypeEReminder;
            break;
            }
        case CCalEntry::EAnniv:
            {
            returnType = EESMREventTypeEAnniv;
            break;
            }
        default:
            // This should not happen
            __ASSERT_DEBUG( 
                    EFalse, 
                    Panic( EMRCalEventProcessorInvalidType ) );
            User::Leave( KErrArgument );
            break;
        }
    
    return returnType;
    }

HBufC* ReplaceCharactersFromBufferLC( const TDesC& aTarget, 
                              const TDesC& aFindString, 
                              const TDesC& aReplacement )
    {
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

}  // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::CMRCalEntryProcessor
// ---------------------------------------------------------------------------
//
CMRCalEntryProcessor::CMRCalEntryProcessor(
        MESMRCalDbMgr& aDbMgr )
:   iDbMgr( aDbMgr )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::~CMRCalEntryProcessor
// ---------------------------------------------------------------------------
//
CMRCalEntryProcessor::~CMRCalEntryProcessor()
    {
    FUNC_LOG;
    
    delete iCalEntry;
    }
    
// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ConstructL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::NewL
// ---------------------------------------------------------------------------
//
CMRCalEntryProcessor* CMRCalEntryProcessor::NewL(
        TAny* aDbMgr )
    {
    FUNC_LOG;
    
    MESMRCalDbMgr* dbMgr = static_cast<MESMRCalDbMgr*>(aDbMgr);
    return CreateL( *dbMgr );
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::CreateL
// ---------------------------------------------------------------------------
//
CMRCalEntryProcessor* CMRCalEntryProcessor::CreateL(
            MESMRCalDbMgr& aDbMgr )
    {
    FUNC_LOG;

    CMRCalEntryProcessor* self = new (ELeave) CMRCalEntryProcessor( aDbMgr );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;    
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ScenarioData
// ---------------------------------------------------------------------------
//
const TESMRScenarioData& CMRCalEntryProcessor::ScenarioData() const
    {
    FUNC_LOG;
    return iScenData;
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ContainsProcessedEntry
// ---------------------------------------------------------------------------
//
TBool CMRCalEntryProcessor::ContainsProcessedEntry() const
    {
    FUNC_LOG;
    return (iCalEntry != NULL);
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ProcessL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::ProcessL(
        const MAgnEntryUi::TAgnEntryUiInParams* aParams,
        RPointerArray<CCalEntry>* aEntries )
    {
    FUNC_LOG;
    
    iParams = aParams;
    ProcessL( aEntries );
    
    if ( MAgnEntryUi::EViewEntry != iParams->iEditorMode )
        {
        iScenData.iViewMode = EESMREditMR;
        }    
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ProcessL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::ProcessL(
            RPointerArray<CCalEntry>* aEntries )
    {
    FUNC_LOG;
    
    __ASSERT_DEBUG( aEntries->Count() > 0,
                    Panic( EMRCalEventProcessorEmptyEntryArray ) );
    
    iCalEntries = aEntries;
    
    CCalEntry& entry = *( (*iCalEntries)[ 0 ] );
    ProcessL( iParams, entry, ETrue );    
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ProcessL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::ProcessL(
            const MAgnEntryUi::TAgnEntryUiInParams* aParams,
            CCalEntry& aEntry, 
            TBool aSetDefaultValuesToEntry,
            const TBool aTypeChanging )
    {
    FUNC_LOG;
    iParams = aParams;
    
    // Plain calendar entries are always used from calendar application        
    iScenData.iCallingApp = EESMRAppCalendar;
    iScenData.iEntryType = MapCalEntryTypeToMRType( aEntry.EntryTypeL() );

    if ( aParams->iEditorMode == MAgnEntryUi::EViewEntry )
        {
        iScenData.iViewMode = EESMRViewMR;
        }
    else
        {
        iScenData.iViewMode = EESMREditMR;
        }
    
    ConvertTextFieldLineFeedsL( aEntry );
    
    CreateEntryL( aEntry, aSetDefaultValuesToEntry );
    
    // When creating a new repeating meeting and adding participants, 
    // meeting request editor is opened and all occurences should be edited
    if ( ( iCalEntry->IsRecurrentEventL() && 
    		MAgnEntryUi::ECreateNewEntry == iParams->iEditorMode ) ||
    	 ( iCalEntry->IsRepeatingMeetingL( aEntry ) && aTypeChanging ) )	
        {
        iCalEntry->SetModifyingRuleL(
                MESMRCalEntry::EESMRAllInSeries, aTypeChanging );
        }
    else //Default case
        {
        iCalEntry->SetModifyingRuleL(
                MESMRCalEntry::EESMRThisOnly, aTypeChanging );
        }

    // If entry is new entry, we want to set default values to it. 
    // On the other hand, when changing entry type from one type to another, 
    // we do not want to set default values
    if ( iParams && MAgnEntryUi::ECreateNewEntry == 
                iParams->iEditorMode && 
                    aSetDefaultValuesToEntry )
        {
        iCalEntry->SetDefaultValuesToEntryL();
        }    
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ProcessL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::ProcessL(
            CCalInstance& /*aInstance*/ )
    {
    FUNC_LOG;
    
    __ASSERT_DEBUG( EFalse, Panic(EMRCalEventProcessorNotSupported) );
    
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::SwitchProcessorToModeL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::SwitchProcessorToModeL(
        TMRProcessorMode aMode )
    {
    FUNC_LOG;
    
    if ( EMRProcessorModeView == aMode )
        {
        iScenData.iViewMode = EESMRViewMR;
        }
    else if ( EMRProcessorModeEdit == aMode )
        {
        iScenData.iViewMode = EESMREditMR;
        }
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ESMREntryL
// ---------------------------------------------------------------------------
//
MESMRCalEntry& CMRCalEntryProcessor::ESMREntryL()
    {
    FUNC_LOG;
    
    if ( !iCalEntry )
        {
        CreateEntryL( *(*iCalEntries)[ 0 ], ETrue );
        }
    
    return *iCalEntry;
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ResetL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::ResetL()
    {
    FUNC_LOG;
    
    iScenData.iViewMode   = EESMRViewUndef;
    iScenData.iCallingApp = EESMRAppCalendar;
    
    delete iCalEntry;
    iCalEntry = NULL;
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ProcessOutputParametersL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::ProcessOutputParametersL(
        MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
        TESMRCommand aCommand )
    {
    FUNC_LOG;
    
    ASSERT( iCalEntry );

     TBool handlingInstanceOnly(
             MESMRCalEntry::EESMRCalEntryMeeting == iCalEntry->Type() &&
             iCalEntry->IsRecurrentEventL() &&
             MESMRCalEntry::EESMRThisOnly == iCalEntry->RecurrenceModRule() );

     switch ( aCommand )
         {
         case EESMRCmdSaveMR:
         case EESMRCmdTodoMarkAsDone:
         case EESMRCmdTodoMarkAsNotDone:
             {
             aOutParams.iAction = MAgnEntryUi::EMeetingSaved;
             if ( handlingInstanceOnly )
                 {
                 aOutParams.iAction = MAgnEntryUi::EInstanceRescheduled;
                 }
             }
             break;

         case EESMRCmdCalEntryUIDelete:
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
         iCalEntry->Entry().StartTimeL();
         }
     else if ( MAgnEntryUi::ENoAction != aOutParams.iAction )
         {
         TCalTime instanceTime = iCalEntry->Entry().StartTimeL();

         if ( MESMRCalEntry::EESMRAllInSeries == iCalEntry->RecurrenceModRule() )
             {
             // For recurrent event there might be exceptions
             // and therefore the first possible start time
             // is set to putput params
             TCalTime end;
             CESMRRecurrenceInfoHandler* recurrenceHandler =
                     CESMRRecurrenceInfoHandler::NewLC( iCalEntry->Entry() );

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
                     iCalEntry->Entry().StartTimeL().TimeLocalL() );
             }
         }
     else
         {
         aOutParams.iNewInstanceDate.SetTimeLocalL( Time::NullTTime() );
         }    
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::SetPhoneOwnerL
// ---------------------------------------------------------------------------
//
TBool CMRCalEntryProcessor::IsDataOk() const
    {
    FUNC_LOG;

    TBool retVal( ETrue );

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
// CMRCalEntryProcessor::CreateEntryL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::CreateEntryL( CCalEntry& aEntry, 
                                            TBool aSetDefaultValuesToEntry  )
    {
    FUNC_LOG;
    if ( !iCalEntry )
        {
        if ( iParams && aSetDefaultValuesToEntry )
            {
            CCalEntry::TType entryType( aEntry.EntryTypeL() );
            if ( CCalEntry::EAppt == entryType ||
                 CCalEntry::EAnniv == entryType )
                {
                // Setting start and end time for child entry
                ESMREntryHelper::CheckRepeatUntilValidityL(
                        aEntry,
                        iParams->iInstanceDate );

                ESMREntryHelper::SetInstanceStartAndEndL(
                        aEntry,
                        aEntry,
                        iParams->iInstanceDate );
                }
            else if ( CCalEntry::ETodo == entryType )
                {
                TTime start = aEntry.StartTimeL().TimeLocalL();
                TTime end   = aEntry.EndTimeL().TimeLocalL();
                
                if ( Time::NullTTime() == start && 
                     Time::NullTTime() == end )
                    {
                    start.HomeTime();
                    end.HomeTime();
                    
                    TCalTime startCalTime;
                    TCalTime endCalTime;
                    
                    startCalTime.SetTimeLocalL( start );
                    endCalTime.SetTimeLocalL( end );    
                    
                    aEntry.SetStartAndEndTimeL( startCalTime, endCalTime );
                    }
                }
            }        
        
        iCalEntry = CMRCalEntry::NewL( aEntry, iDbMgr );
        }
    }

// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ConvertTextFieldLineFeedsL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::ConvertTextFieldLineFeedsL()
    {
    FUNC_LOG;
        
    TInt entryCount( iCalEntries->Count() );
    for (TInt i(0); i < entryCount; ++i )
        {
        ConvertTextFieldLineFeedsL( *(*iCalEntries)[ i ] );
        }
    }


// ---------------------------------------------------------------------------
// CMRCalEntryProcessor::ConvertTextFieldLineFeedsL
// ---------------------------------------------------------------------------
//
void CMRCalEntryProcessor::ConvertTextFieldLineFeedsL( CCalEntry& aEntry )
    {
    TInt ret = aEntry.DescriptionL().Find(KReplaceLineFeedChars);    
    if ( ret != KErrNotFound )
        {
        HBufC* newDescription = 
                ReplaceCharactersFromBufferLC(
                            aEntry.DescriptionL(),       
                            KReplaceLineFeedChars(),
                            KLineFeed() );
        aEntry.SetDescriptionL( *newDescription );
        CleanupStack::PopAndDestroy( newDescription );
        }    
    
    // Location field:
    ret = aEntry.LocationL().Find(KReplaceLineFeedChars);    
    if ( ret != KErrNotFound )
        {
        HBufC* newLocation = 
                ReplaceCharactersFromBufferLC(
                            aEntry.DescriptionL(),       
                            KReplaceLineFeedChars(),
                            KLineFeed() );
        aEntry.SetDescriptionL( *newLocation );
        CleanupStack::PopAndDestroy( newLocation );
        }    
        
    // Subject field:
    ret = aEntry.SummaryL().Find(KReplaceLineFeedChars);    
    if ( ret != KErrNotFound )
        {
        HBufC* newSubject = 
                ReplaceCharactersFromBufferLC(
                        aEntry.DescriptionL(),       
                        KReplaceLineFeedChars(),
                        KLineFeed() );
        aEntry.SetDescriptionL( *newSubject );
        CleanupStack::PopAndDestroy( newSubject );
        }        
    }

// EOF

