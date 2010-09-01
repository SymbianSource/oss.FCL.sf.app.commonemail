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
* Description: This file implements class CESMRICalTimeZoneInterval.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricaltimezoneinterval.h"	// CESMRICalTimeZoneInterval

//debug

// User includes.
#include "esmricalkeywords.h"			// Literals

/**
Static factory construction
@param aType The type of time zone interval.
@return A new timezone interval component
@leave Leaves with KErrCorrupt if the requested component is invalid.
@internalTechnology
*/
CESMRICalTimeZoneInterval* CESMRICalTimeZoneInterval::NewLC(TICalComponentType aType, TICalMethod aMethod)
	{
    FUNC_LOG;
	
	if ((aType != EICalDaylight) && (aType != EICalStandard))
		{
		User::Leave(KErrCorrupt);
		}

	CESMRICalTimeZoneInterval* self = new (ELeave) CESMRICalTimeZoneInterval(aType);
	CleanupStack::PushL(self);
	self->ConstructL(aMethod);
	
	return self;
	}

/**
Destructor
@internalTechnology
*/
CESMRICalTimeZoneInterval::~CESMRICalTimeZoneInterval()
	{
    FUNC_LOG;
	}

/**
Constructor
@param aType The type of time zone interval.
@internalTechnology
*/
CESMRICalTimeZoneInterval::CESMRICalTimeZoneInterval(TICalComponentType aType)
	{
    FUNC_LOG;
	
	iComponentType = aType;
	
	}

/**
Internal construction
@internalTechnology
*/
void CESMRICalTimeZoneInterval::ConstructL(TICalMethod aMethod)
	{
    FUNC_LOG;
	
	iMethod = aMethod;
	iComponentMethodBitMask = EMaskTimezoneIntervalAny;
	
	}

/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName.
@param aName the name of the property to validate.
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CESMRICalTimeZoneInterval::ValidatePropertyImpl(const TDesC& aName) const
	{
    FUNC_LOG;
	
	if ((aName.CompareF(KICalDtstart) == 0) || (aName.CompareF(KICalTzoffsetto) == 0) ||
		(aName.CompareF(KICalTzoffsetfrom) == 0) || (aName.CompareF(KICalComment) == 0) ||
		(aName.CompareF(KICalRdate) == 0) || (aName.CompareF(KICalRRule) == 0) ||
		(aName.CompareF(KICalTzname) == 0))
		{
		//	1		dtstart / tzoffsetto / tzoffsetfrom
		//	0..1	comment / rdate / rrule / tzname
		return (FindProperty(aName) == NULL);
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
TBool CESMRICalTimeZoneInterval::ValidateComponent(TICalComponentType /*aType*/) const
	{
    FUNC_LOG;
	// DAYLIGHT or STANDARD components cannot contain other components.
	return EFalse;
	}
	
// End of file


