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
* Description: This file implements class CESMRICalTodo.
*
*/


// Class includes.
#include "emailtrace.h"
#include "cesmricaltodo.h"  // CESMRICalTodo

//debug

// User includes.
#include "esmricalkeywords.h"           // Literals

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRICalTodo::NewLC
// ---------------------------------------------------------------------------
//
CESMRICalTodo* CESMRICalTodo::NewLC(TICalMethod aMethod)
    {
    FUNC_LOG;

    CESMRICalTodo* self = new (ELeave) CESMRICalTodo;
    CleanupStack::PushL(self);
    self->ConstructL(aMethod);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRICalTodo::~CESMRICalTodo
// ---------------------------------------------------------------------------
//
CESMRICalTodo::~CESMRICalTodo()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRICalTodo::CESMRICalTodo
// ---------------------------------------------------------------------------
//
CESMRICalTodo::CESMRICalTodo()
    {
    FUNC_LOG;

    iComponentType = EICalTodo;

    }

// ---------------------------------------------------------------------------
// CESMRICalTodo::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRICalTodo::ConstructL(TICalMethod aMethod)
    {
    FUNC_LOG;

    iMethod = aMethod;
    switch(iMethod)
        {
        case EMethodNone:
            iComponentMethodBitMask = EMaskTodoNone;
            break;
        case EMethodPublish:
            iComponentMethodBitMask = EMaskTodoPublish;
            break;
        case EMethodRequest:
            iComponentMethodBitMask = EMaskTodoRequest;
            break;
        case EMethodReply:
            iComponentMethodBitMask = EMaskTodoReply;
            break;
        case EMethodAdd:
            iComponentMethodBitMask = EMaskTodoAdd;
            break;
        case EMethodCancel:
            iComponentMethodBitMask = EMaskTodoCancel;
            break;
        case EMethodRefresh:
            iComponentMethodBitMask = EMaskTodoRefresh;
            break;
        case EMethodCounter:
            iComponentMethodBitMask = EMaskTodoCounter;
            break;
        case EMethodDeclineCounter:
            iComponentMethodBitMask = EMaskTodoDeclineCounter;
            break;
        }
    }

// ---------------------------------------------------------------------------
// CESMRICalTodo::ValidatePropertyImpl
// ---------------------------------------------------------------------------
//
/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName
@param aName the name of the property to validate
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CESMRICalTodo::ValidatePropertyImpl(const TDesC& aName) const
    {
    FUNC_LOG;

    if ((aName.CompareF(KICalClass) == 0) || (aName.CompareF(KICalCompleted) == 0) ||
        (aName.CompareF(KICalCreated) == 0) || (aName.CompareF(KICalDescription) == 0) ||
        (aName.CompareF(KICalDtstamp) == 0) || (aName.CompareF(KICalDtstart) == 0) ||
        (aName.CompareF(KICalGeo) == 0) || (aName.CompareF(KICalLastmodified) == 0) ||
        (aName.CompareF(KICalLocation) == 0) || (aName.CompareF(KICalOrganizer) == 0) ||
        (aName.CompareF(KICalPercentcomplete) == 0) || (aName.CompareF(KICalPriority) == 0) ||
        (aName.CompareF(KICalRecurrenceId) == 0) || (aName.CompareF(KICalSequence) == 0) ||
        (aName.CompareF(KICalStatus) == 0) || (aName.CompareF(KICalSummary) == 0) ||
        (aName.CompareF(KICalUid) == 0) || (aName.CompareF(KICalUrl) == 0) ||
        (aName.CompareF(KICalDtend) == 0) || (aName.CompareF(KICalDuration) == 0))
        {
        //  0..1    CLASS
        //  0..1    COMPLETED
        //  0..1    CREATED
        //  0..1    DESCRIPTION
        //  0..1    DTSTAMP
        //  0..1    DTSTART
        //  0..1    GEO
        //  0..1    LAST-MOD
        //  0..1    LOCATION
        //  0..1    ORGANIZER
        //  0..1    PERCENT-COMPLETE
        //  0..1    PRIORITY
        //  0..1    RECURRENCE-ID
        //  0..1    SEQUENCE
        //  0..1    STATUS
        //  0..1    SUMMARY
        //  0..1    UID
        //  0..1    URL
        return (FindProperty(aName) == NULL);
        }
    // Either DUE or DURATION may exist but *not* both
    else if (aName.CompareF(KICalDue) == 0)
        {
        //  0..1    DUE
        if (!FindProperty(aName))
            {
            return (FindProperty(KICalDuration) == NULL);
            }
        }
        else if (aName.CompareF(KICalDuration) == 0)
        {
        //  0..1    DURATION
        if (!FindProperty(aName))
            {
            return (FindProperty(KICalDue) == NULL);
            }
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

    if ((aName.Length() >= 2) && (aName.Left(2).CompareF(KICalXProperty) == 0))
        {
        //  0..*    X-[anything]
        return ETrue;
        }

    // If we got this far we didn't match any of the possible property names
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CESMRICalTodo::ValidateComponent
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
TBool CESMRICalTodo::ValidateComponent(TICalComponentType aType) const
    {
    FUNC_LOG;
    // VTODO can only contain VALARMs.
    return (aType == EICalAlarm);
    }

// End of File

