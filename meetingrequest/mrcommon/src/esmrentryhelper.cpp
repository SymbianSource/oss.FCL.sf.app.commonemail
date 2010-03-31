/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Static helper methods for analyzing entries
*
*/



// INCLUDE FILES
#include "emailtrace.h"
#include "esmrentryhelper.h"
#include "esmrhelper.h"
#include "esmrdef.h"

// From System
#include <calentry.h>
#include <caltime.h>
#include <calrrule.h>
#include <caluser.h>
#include <msvids.h>
#include <msgmailuids.h>
#include <cmrmailboxutils.h>
#include <caleninterimutils2.h>

// unnamed naespace for local definitions
namespace { // codescanner::namespace

// Definition for zero interval
const TInt KZeroInterval( 0 );

}

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// ESMREntryHelper::SendingMailBoxL
// ----------------------------------------------------------------------------
//
EXPORT_C TMsvId ESMREntryHelper::SendingMailBoxL(
    const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
    CMRMailboxUtils& aUtils )
    {
    FUNC_LOG;
    TMsvId result(0); 
    if ( aInParams.iCallingApp.iUid == KUidMsgMailViewer )
        {
        result = aInParams.iMailBoxId;
        }
    else
        {
        CMRMailboxUtils::TMailboxInfo info;
        // returns KMsvNullIndexEntryId if default box cannot be resolved:
        aUtils.GetDefaultMRMailBoxL( info );
        result = info.iEntryId;
        }
    return result;
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::IsRepeatingMeetingL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool ESMREntryHelper::IsRepeatingMeetingL(
        const CCalEntry& aEntry )
    {
    FUNC_LOG;

    TBool retVal( EFalse );

    if ( IsModifyingEntryL( aEntry ) )
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
            CleanupStack::PopAndDestroy( &dummyRDateList );
            }
        }


    return retVal;
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::IsModifyingEntryL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool ESMREntryHelper::IsModifyingEntryL( const CCalEntry& aEntry )
    {
    FUNC_LOG;
    return ( aEntry.RecurrenceIdL().TimeUtcL() !=
             Time::NullTTime() ) ? ETrue : EFalse;
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::IsLatestSavedSentL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool ESMREntryHelper::IsLatestSavedSentL(
    const CCalEntry& aEntry )
    {
    FUNC_LOG;
    TBool retVal( EFalse );
    if ( aEntry.DTStampL().TimeUtcL() == Time::NullTTime() )
        {
        // If entries are synchronized to phone we must always assume that
        // they have been sent already. In case of our own requests we keep the
        // DTSTAMP as null until sending.

        retVal = ETrue;
        }
    return retVal;
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::IsCancelledL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool ESMREntryHelper::IsCancelledL(
    const CCalEntry& aEntry,
    CMRMailboxUtils& aUtils )
    {
    FUNC_LOG;
    TBool retVal( EFalse );
    // Check if either entire meeting is cancelled, or this attendee
    // has been removed from the participant list. According to RFC2446
    // in that case entry doesn't have status field set.
    if ( aEntry.StatusL() == CCalEntry::ECancelled ||
         ( aEntry.MethodL() == CCalEntry::EMethodCancel &&
         !aUtils.ThisAttendeeL( aEntry ) ) )
        {
        retVal = ETrue;
        }
    return retVal;
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::IsAllDayEventL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool ESMREntryHelper::IsAllDayEventL( const CCalEntry& aEntry )
    {
    FUNC_LOG;
    TBool retVal( EFalse );
   
    TDateTime start = aEntry.StartTimeL().TimeLocalL().DateTime();
    TDateTime end = aEntry.EndTimeL().TimeLocalL().DateTime();

    // All-day events: 
    // From one days 00:00:00 to at least next days 00:00:00.
    // Start times hour, minute and second need to be the same.
    // Start and end time days need to be different
    if( start.Hour() == end.Hour() && 
            start.Minute() == end.Minute() &&
                start.Second() == end.Second() && 
                    start.Day() != end.Day() )
        {
        if( start.Hour() == 0 && 
                start.Minute() == 0 &&
                    start.Second() == 0 )
            {
            retVal = ETrue;
            }
        }

    return retVal;
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::OccursInPastL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool ESMREntryHelper::OccursInPastL( const CCalEntry& aEntry )
    {
    FUNC_LOG;
    TBool retVal( EFalse );
    TTime universalTime;
    universalTime.UniversalTime();
    TTime meetingUniversalTime = aEntry.StartTimeL().TimeUtcL();
    if ( universalTime  > meetingUniversalTime )
        {
        retVal = ETrue;
        }
    return retVal;
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::PhoneOwnerAddrL
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC& ESMREntryHelper::PhoneOwnerAddrL(
    const CCalEntry& aEntry,
    CMRMailboxUtils& aUtils )
    {
    FUNC_LOG;
    if ( aUtils.IsOrganizerL( aEntry ) )
        {
        return aEntry.OrganizerL()->Address();
        }
    else
        {
        CCalAttendee* thisAttendee = aUtils.ThisAttendeeL( aEntry  );
        if ( thisAttendee )
            {
            return thisAttendee->Address();
            }
        else
            {
            return KNullDesC;
            }
        }
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::EqualAttendeeL
// ----------------------------------------------------------------------------
//
EXPORT_C CCalAttendee* ESMREntryHelper::EqualAttendeeL(
    const CCalAttendee& aAttendee,
    const CCalEntry& aEntry )
    {
    FUNC_LOG;
    TPtrC addr = ESMRHelper::AddressWithoutMailtoPrefix(
                    aAttendee.Address() );

    RPointerArray<CCalAttendee>& attendees = aEntry.AttendeesL();
    TInt count( attendees.Count() );
    for ( TInt i ( 0 ); i < count; ++i )
        {
        TPtrC testAddr =
            ESMRHelper::AddressWithoutMailtoPrefix(
                    attendees[i]->Address() );

        if ( addr.CompareF( testAddr ) == 0 )
            {
            return attendees[i];
            }
        }
    return NULL;
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::SpansManyDaysL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool ESMREntryHelper::SpansManyDaysL(
    const TCalTime& aStartTime,
    const TCalTime& aEndTime )
    {
    FUNC_LOG;
    TBool retVal( EFalse );

    // mustn't use UTC time here, local time specifies if the midnight
    // is crossed
    TTime localStartTime = aStartTime.TimeLocalL();
    TTime localEndTime = aEndTime.TimeLocalL();

    TTimeIntervalDays days = localEndTime.DaysFrom( localStartTime );

    if ( days.Int() > 0 )
        {
        retVal = ETrue;
        }
    return retVal;
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::EventTypeL
// ----------------------------------------------------------------------------
//
EXPORT_C TESMRCalendarEventType ESMREntryHelper::EventTypeL(
            const CCalEntry& aCalEntry )
    {
    FUNC_LOG;
    
    TESMRCalendarEventType type( EESMREventTypeNone );
    
    switch ( aCalEntry.EntryTypeL() )
        {
        case CCalEntry::EAppt:
            if ( CCalenInterimUtils2::IsMeetingRequestL( 
                    const_cast< CCalEntry& >( aCalEntry ) ) )
                {
                type = EESMREventTypeMeetingRequest;
                }
            else
                {
                type = EESMREventTypeAppt;
                }
            break;
            
        case CCalEntry::ETodo:
            type = EESMREventTypeETodo;
            break;
            
        case CCalEntry::EEvent:
            type = EESMREventTypeEEvent;
            break;
            
        case CCalEntry::EReminder:
            type = EESMREventTypeEReminder;
            break;
            
        case CCalEntry::EAnniv:
            type = EESMREventTypeEAnniv;
            break;
            
        }

    return type;
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::SetInstanceStartAndEndL
// ----------------------------------------------------------------------------
//
EXPORT_C void ESMREntryHelper::SetInstanceStartAndEndL(
        CCalEntry& aChild,
        const CCalEntry& aParent,
        const TCalTime& aChildStart )
    {
    TTime parentStart( aParent.StartTimeL().TimeUtcL() );
    TTime parentEnd( aParent.EndTimeL().TimeUtcL() );
    const TTime KNullTime( Time::NullTTime() );

    if ( KNullTime == parentStart ||
         KNullTime == parentEnd ||
         KNullTime == aChildStart.TimeUtcL() )
        {
        // Invalid time --> Leaving
        User::Leave( KErrArgument );
        }

    TTimeIntervalMicroSeconds duration( KZeroInterval );
    duration = parentEnd.MicroSecondsFrom( parentStart );
    TCalTime childStart;
    TCalTime childEnd;
    
    // Meeting time mode is Fixed Utc
    if( aChild.EntryTypeL() == CCalEntry::EAppt )
        {
        const TTime startTime = aChildStart.TimeUtcL();
        childStart.SetTimeUtcL( startTime );
        childEnd.SetTimeUtcL( startTime + duration );   
        }
    
    // Anniversary time mode is local floating
    if( aChild.EntryTypeL() == CCalEntry::EAnniv )
        {
        const TTime startTime = aChildStart.TimeLocalL();
        childStart.SetTimeLocalFloatingL( startTime );
        childEnd.SetTimeLocalFloatingL( startTime + duration );  
        }

    aChild.SetStartAndEndTimeL( childStart, childEnd );
    }

// ----------------------------------------------------------------------------
// ESMREntryHelper::CheckRepeatUntilValidityL
// ----------------------------------------------------------------------------
//
EXPORT_C void ESMREntryHelper::CheckRepeatUntilValidityL(
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
                TCalRRule rRule = originalRRule;

                TCalTime newUntil;
                newUntil.SetTimeLocalL( instanceEndTime );
                rRule.SetUntil( newUntil );
                
                aEntry.SetRRuleL( rRule );
                }
            }
        }
    }

//  End of File

