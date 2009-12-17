/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CESMRICalFreeBusy.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricalfreebusy.h"	// CESMRICalFreeBusy

//debug

// User includes
#include "esmricalkeywords.h"			// Literals

/**
Static factory construction
@return A new CESMRICalFreeBusy
@internalTechnology
*/
CESMRICalFreeBusy* CESMRICalFreeBusy::NewLC(TICalMethod aMethod)
	{
    FUNC_LOG;
	
	CESMRICalFreeBusy* self = new (ELeave) CESMRICalFreeBusy;
	CleanupStack::PushL(self);
	self->ConstructL(aMethod);
	
	return self;
	}

/**
Destructor
@internalTechnology
*/
CESMRICalFreeBusy::~CESMRICalFreeBusy()
	{
    FUNC_LOG;
	}

/**
Constructor
@internalTechnology
*/
CESMRICalFreeBusy::CESMRICalFreeBusy()
	{
    FUNC_LOG;
	
	iComponentType = EICalFreeBusy;
	
	}

/**
Internal construction
@internalTechnology
*/
void CESMRICalFreeBusy::ConstructL(TICalMethod aMethod)
	{
    FUNC_LOG;
	
	iMethod = aMethod;
	switch(iMethod)
		{
		case EMethodNone:
			iComponentMethodBitMask = EMaskFreeBusyNone;
			break;
		case EMethodPublish:
			iComponentMethodBitMask = EMaskFreeBusyPublish;
			break;
		case EMethodRequest:
			iComponentMethodBitMask = EMaskFreeBusyRequest;
			break;
		case EMethodReply:
			iComponentMethodBitMask = EMaskFreeBusyReply;
			break;
		default:
			// This is invalid should we leave
			iComponentMethodBitMask = 0;
			break;
		}
		
	}
	
/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName
@param aName the name of the property to validate
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CESMRICalFreeBusy::ValidatePropertyImpl(const TDesC& aName) const
	{
    FUNC_LOG;
	
	if ((aName.CompareF(KICalContact) == 0) || (aName.CompareF(KICalDtstart) == 0) ||
		(aName.CompareF(KICalDtend) == 0) || (aName.CompareF(KICalDuration) == 0) ||
		(aName.CompareF(KICalDtstamp) == 0) || (aName.CompareF(KICalOrganizer) == 0) ||
		(aName.CompareF(KICalUid) == 0) || (aName.CompareF(KICalUrl) == 0))
		{
		//	0..1	CONTACT
		//	0..1	DTSTART
		//	0..1	DTEND
		//	0..1	DURATION
		//	0..1	DTSTAMP
		//	0..1	ORGANIZER
		//	0..1	UID
		//	0..1	URL
		return (FindProperty(aName) == NULL);
		}
	else if ((aName.CompareF(KICalAttendee) == 0) || (aName.CompareF(KICalComment) == 0) ||
		(aName.CompareF(KICalFreebusy) == 0) || (aName.CompareF(KICalRequeststatus) == 0))
		{
		//	0..*	ATTENDEE
		//	0..*	COMMENT
		//	0..*	FREEBUSY
		//	0..*	REQUEST-STATUS
		return ETrue;
		}

	if ((aName.Length() >= 2) && (aName.Left(2).CompareF(KICalXProperty) == 0))
		{
		//	0..*	X-[anything]
		return ETrue;
		}

	// If we got this far we didn't match any of the possible property names
	return EFalse;	
	}

/**
Checks that the iCalendar specification allows this calendar component to
contain a nested component of type aType.
@param aType the type of component to validate.
@return ETrue if the component is a valid child for this component, otherwise
EFalse.
@internalTechnology
*/
TBool CESMRICalFreeBusy::ValidateComponent(TICalComponentType /*aType*/) const
	{
    FUNC_LOG;
	// VFREEBUSY cannot contain other components.
	return EFalse;
	}

// End of File

