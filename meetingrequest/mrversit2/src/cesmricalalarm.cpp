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
* Description: This file implements class CESMRICalAlarm.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricalalarm.h"	// CESMRICalAlarm

//debug

// User includes.
#include "esmricalkeywords.h"			// Literals

/**
Static factory construction
@return A new CESMRICalAlarm
@internalTechnology
*/
CESMRICalAlarm* CESMRICalAlarm::NewLC(TICalMethod aMethod)
	{
    FUNC_LOG;
	
	CESMRICalAlarm* self = new (ELeave) CESMRICalAlarm;
	CleanupStack::PushL(self);
	self->ConstructL(aMethod);
	
	return self;
	}

/**
Destructor
@internalTechnology
*/
CESMRICalAlarm::~CESMRICalAlarm()
	{
    FUNC_LOG;
	}

/**
Constructor
@internalTechnology
*/
CESMRICalAlarm::CESMRICalAlarm()
	{
    FUNC_LOG;
	
	iComponentType = EICalAlarm;
	
	}

/**
Internal construction
@internalTechnology
*/
void CESMRICalAlarm::ConstructL(TICalMethod aMethod)
	{
    FUNC_LOG;
	
	iMethod = aMethod;
	iComponentMethodBitMask = EMaskAlarmAny;
	
	}
	
/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName
@param aName the name of the property to validate
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CESMRICalAlarm::ValidatePropertyImpl(const TDesC& aName) const
	{
    FUNC_LOG;
	
	 if ((aName.CompareF(KICalAction) == 0) || (aName.CompareF(KICalTrigger) == 0) ||
	 	(aName.CompareF(KICalDuration) == 0) || (aName.CompareF(KICalRepeat) == 0) ||
	 	(aName.CompareF(KICalDescription) == 0) || (aName.CompareF(KICalSummary) == 0))
		{
		//	1	ACTION
		//	1	TRIGGER
		//	0..1	DURATION
		//	0..1	REPEAT
		//	0..1	DESCRIPTION
		//	0..1	SUMMARY
		return (FindProperty(aName) == NULL);
		}
	else if(aName.CompareF(KICalAttendee) == 0 || aName.CompareF(KICalAttach) == 0)
		{
		//	0..*	ATTENDEE
		//	0..*	ATTACH
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
TBool CESMRICalAlarm::ValidateComponent(TICalComponentType /*aType*/) const
	{
    FUNC_LOG;
	// VALARM cannot contain other components.
	return EFalse;
	}
	
// End of File

