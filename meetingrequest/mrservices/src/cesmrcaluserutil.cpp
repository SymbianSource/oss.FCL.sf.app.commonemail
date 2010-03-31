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
* Description:  MR user utility class implementation
*
*/


#include "emailtrace.h"
#include "cesmrcaluserutil.h"
//<cmail>
#include "esmrdef.h"
//</cmail>
#include <calentry.h>
#include <caluser.h>

namespace { // codescanner::namespace

// Definition for 0
const TInt KZero = 0;

// Definition for number of hours within day
const TInt KHoursInDay = 24;

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCalUserUtil::CESMRCalUserUtil
// ---------------------------------------------------------------------------
//
inline CESMRCalUserUtil::CESMRCalUserUtil(
        CCalEntry& aEntry )
:   iEntry( aEntry )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCalUserUtil::~CESMRCalUserUtil
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRCalUserUtil::~CESMRCalUserUtil()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCalUserUtil::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRCalUserUtil* CESMRCalUserUtil::NewL(
             CCalEntry& aEntry )
     {
    FUNC_LOG;
     CESMRCalUserUtil* self = NewLC( aEntry );
     CleanupStack::Pop( self );
     return self;
     }

// ---------------------------------------------------------------------------
// CESMRCalUserUtil::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRCalUserUtil* CESMRCalUserUtil::NewLC(
             CCalEntry& aEntry )
     {
    FUNC_LOG;
     CESMRCalUserUtil* self = new (ELeave) CESMRCalUserUtil( aEntry );
     CleanupStack::PushL( self );
     self->ConstructL();
     return self;
     }

// ---------------------------------------------------------------------------
// CESMRCalUserUtil::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCalUserUtil::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRCalUserUtil::GetAttendeesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRCalUserUtil::GetAttendeesL(
        RArray<CCalAttendee*>& aAttendeeArray,
        TUint aFilterFlags ) const
    {
    FUNC_LOG;

    aAttendeeArray.Reset();
    RPointerArray<CCalAttendee>& attendees = iEntry.AttendeesL();

    TBool includeRequired( EESMRRoleRequiredAttendee & aFilterFlags);
    TBool includeOptional( EESMRRoleOptionalAttendee & aFilterFlags);

    TInt attendeeCount( attendees.Count() );
    for (TInt i(0); i < attendeeCount; ++i )
        {
        CCalAttendee* attendee = attendees[i];
        CCalAttendee::TCalRole role(
                 attendee->RoleL() );

        TPtrC cn( attendee->CommonName() );
        TPtrC ad( attendee->Address() );

        if ( includeRequired &&
             CCalAttendee::EReqParticipant == role )
            {
            aAttendeeArray.Append(attendee);
            }
        else if ( includeRequired &&
                  CCalAttendee::EChair == role )
            {
            aAttendeeArray.Append(attendee);
            }
        else if ( includeOptional &&
                  CCalAttendee::EOptParticipant == role )
            {
            aAttendeeArray.Append(attendee);
            }
        }

    }

// ---------------------------------------------------------------------------
// CESMRCalUserUtil::PhoneOwnerAttendeeRoleL
// ---------------------------------------------------------------------------
//
EXPORT_C TESMRRole CESMRCalUserUtil::PhoneOwnerRoleL() const
    {
    FUNC_LOG;
    
    TESMRRole role = EESMRRoleUndef;
    CCalUser* phoneOwner = iEntry.PhoneOwnerL();
    RPointerArray<CCalAttendee>& attendees = iEntry.AttendeesL();

    if ( phoneOwner )
        {
        CCalUser* organizer = iEntry.OrganizerL();

        if( phoneOwner && phoneOwner == organizer )
            {
            role = EESMRRoleOrganizer;
            }
        else
            {
            CCalAttendee* thisAttendee = NULL;
            TInt attendeeCount( attendees.Count() );
            for ( TInt i(0); (i < attendeeCount) && !thisAttendee; ++i )
                {
                if ( phoneOwner && phoneOwner == attendees[i] )
                    {
                    thisAttendee = attendees[i];
                    }
                }

            if ( thisAttendee )
                {
                switch( thisAttendee->RoleL() )
                    {
                    case CCalAttendee::EChair://fallthrough
                    case CCalAttendee::EReqParticipant:
                        {
                        role = EESMRRoleRequiredAttendee;
                        }
                        break;
                    case CCalAttendee::EOptParticipant:
                        {
                        role = EESMRRoleOptionalAttendee;
                        }
                        break;
                    case CCalAttendee::ENonParticipant:
                        {
                        role = EESMRRoleNonParticipant;
                        }
                        break;
                    default:
                        {
                        role = EESMRRoleUndef;
                        }
                        break;
                    }
                }
            }
        }
    
    return role;
    }

// ---------------------------------------------------------------------------
// CESMRCalUserUtil::IsAlldayEventL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRCalUserUtil::IsAlldayEventL() const
    {
    FUNC_LOG;
    
    TBool allDayEvent(EFalse);

    TCalTime startTime = iEntry.StartTimeL();
    TCalTime stopTime  = iEntry.EndTimeL();

    TTimeIntervalHours hoursBetweenStartAndEnd;
    stopTime.TimeLocalL().HoursFrom(
            startTime.TimeLocalL(),
            hoursBetweenStartAndEnd );

    TCalTime::TTimeMode mode = startTime.TimeMode();

    TInt hoursBetweenStartAndEndAsInt(  hoursBetweenStartAndEnd.Int() );
    TInt alldayDivident(  hoursBetweenStartAndEndAsInt % KHoursInDay );

    TDateTime startTimeLocal = startTime.TimeLocalL().DateTime();
    TDateTime stopTimeLocal =  stopTime.TimeLocalL().DateTime();
    
    if ( hoursBetweenStartAndEndAsInt && KZero == alldayDivident )
        {
        if ( startTimeLocal.Hour() == stopTimeLocal.Hour() &&
             startTimeLocal.Minute() == stopTimeLocal.Minute() &&
             startTimeLocal.Second() == stopTimeLocal.Second() )
            {
            allDayEvent = ETrue;
            }
        }
    
    return allDayEvent;
    }

// EOF

