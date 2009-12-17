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

// From System
#include <calentry.h>
#include <caltime.h>
#include <calrrule.h>
#include <caluser.h>
#include <msvids.h>
#include <MsgMailUIDs.h>
#include <cmrmailboxutils.h>


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

//  End of File

