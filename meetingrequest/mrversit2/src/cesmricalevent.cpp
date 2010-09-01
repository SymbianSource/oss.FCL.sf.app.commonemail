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
* Description: This file implements class CESMRICalEvent.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricalevent.h" // CESMRICalEvent

//debug

// User includes.
#include "esmricalkeywords.h"           // Literals

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRICalEvent::NewLC
// ---------------------------------------------------------------------------
//
CESMRICalEvent* CESMRICalEvent::NewLC(TICalMethod aMethod)
    {
    FUNC_LOG;

    CESMRICalEvent* self = new (ELeave) CESMRICalEvent;
    CleanupStack::PushL(self);
    self->ConstructL(aMethod);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRICalEvent::~CESMRICalEvent
// ---------------------------------------------------------------------------
//
CESMRICalEvent::~CESMRICalEvent()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRICalEvent::CESMRICalEvent
// ---------------------------------------------------------------------------
//
CESMRICalEvent::CESMRICalEvent()
    {
    FUNC_LOG;

    iComponentType = EICalEvent;

    }

// ---------------------------------------------------------------------------
// CESMRICalEvent::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRICalEvent::ConstructL(TICalMethod aMethod)
    {
    FUNC_LOG;

    iMethod = aMethod;
    switch(iMethod)
        {
        case EMethodNone:
            iComponentMethodBitMask = EMaskEventNone;
            break;
        case EMethodPublish:
            iComponentMethodBitMask = EMaskEventPublish;
            break;
        case EMethodRequest:
            iComponentMethodBitMask = EMaskEventRequest;
            break;
        case EMethodReply:
            iComponentMethodBitMask = EMaskEventReply;
            break;
        case EMethodAdd:
            iComponentMethodBitMask = EMaskEventAdd;
            break;
        case EMethodCancel:
            iComponentMethodBitMask = EMaskEventCancel;
            break;
        case EMethodRefresh:
            iComponentMethodBitMask = EMaskEventRefresh;
            break;
        case EMethodCounter:
            iComponentMethodBitMask = EMaskEventCounter;
            break;
        case EMethodDeclineCounter:
            iComponentMethodBitMask = EMaskEventDeclineCounter;
            break;
        }
    }

// ---------------------------------------------------------------------------
// CESMRICalEvent::ValidatePropertyImpl
// ---------------------------------------------------------------------------
//
/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName
@param aName the name of the property to validate
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CESMRICalEvent::ValidatePropertyImpl(const TDesC& aName) const
    {
    FUNC_LOG;

    if ((aName.CompareF(KICalClass) == 0) || (aName.CompareF(KICalCreated) == 0) ||
        (aName.CompareF(KICalDescription) == 0) || (aName.CompareF(KICalDtstart) == 0) ||
        (aName.CompareF(KICalGeo) == 0) || (aName.CompareF(KICalLastmodified) == 0) ||
        (aName.CompareF(KICalLocation) == 0) || (aName.CompareF(KICalOrganizer) == 0) ||
        (aName.CompareF(KICalPriority) == 0) || (aName.CompareF(KICalDtstamp) == 0) ||
        (aName.CompareF(KICalSequence) == 0) || (aName.CompareF(KICalStatus) == 0) ||
        (aName.CompareF(KICalSummary) == 0) || (aName.CompareF(KICalTransp) == 0) ||
        (aName.CompareF(KICalUid) == 0) || (aName.CompareF(KICalUrl) == 0) ||
        (aName.CompareF(KICalRecurrenceId) == 0))
        {
        //  0..1    CLASS
        //  0..1    CREATED
        //  0..1    DESCRIPTION
        //  0..1    DTSTART
        //  0..1    GEO
        //  0..1    LAST-MOD
        //  0..1    LOCATION
        //  0..1    ORGANIZER
        //  0..1    PRIORITY
        //  0..1    DTSTAMP
        //  0..1    SEQUENCE
        //  0..1    STATUS
        //  0..1    SUMMARY
        //  0..1    TRANSP
        //  0..1    UID
        //  0..1    URL
        //  0..1    RECURRENCE-ID
        return (FindProperty(aName) == NULL);
        }
    // Either DTEND or DURATION may exist but *not* both
    else if (aName.CompareF(KICalDtend) == 0)
        {
        //  0..1    DTEND
        if (!FindProperty(aName))
            {
            return (FindProperty(KICalDuration) == NULL);
            }
        return EFalse;
        }
    else if (aName.CompareF(KICalDuration) == 0)
        {
        //  0..1    DURATION
        if (!FindProperty(aName))
            {
            return (FindProperty(KICalDtend) == NULL);
            }

        return EFalse;
        }
    else if ((aName.CompareF(KICalAttach) == 0) || (aName.CompareF(KICalAttendee) == 0) ||
            (aName.CompareF(KICalCategories) == 0) || (aName.CompareF(KICalComment) == 0) ||
            (aName.CompareF(KICalContact) == 0) || (aName.CompareF(KICalExdate) == 0) ||
            (aName.CompareF(KICalExrule) == 0) || (aName.CompareF(KICalRequeststatus) == 0) ||
            (aName.CompareF(KICalRelatedto) == 0) || (aName.CompareF(KICalResources) == 0) ||
            (aName.CompareF(KICalRdate) == 0) || (aName.CompareF(KICalRRule) == 0))
        {
        //  0..*    ATTACH
        //  0..*    ATTENDEE
        //  0..*    CATEGORIES
        //  0..*    COMMENT
        //  0..*    CONTACT
        //  0..*    EXDATE
        //  0..*    EXRULE
        //  0..*    REQUEST-STATUS
        //  0..*    RELATED-TO
        //  0..*    RESOURCES
        //  0..*    RDATE
        //  0..*    RRULE
        return ETrue;
        }
    else if ((aName.Length() >= 2) && (aName.Left(2).CompareF(KICalXProperty) == 0))
        {
        //  0..*    X-[anything]
        return ETrue;
        }

    // If we got this far we didn't match any of the possible property names
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRICalEvent::ValidateComponent
// ---------------------------------------------------------------------------
//
/**
Checks that the iCalendar specification allows this calendar component to
contain a nested component of type aType.
@param aType the type of component to validate.
@return ETrue if the component is a valid child for this component, otherwise
EFalse.
@internalTechnology
*/
TBool CESMRICalEvent::ValidateComponent(TICalComponentType aType) const
    {
    FUNC_LOG;
    // VEVENTs can only contain VALARMs.
    return (aType == EICalAlarm);
    }

// End of File

