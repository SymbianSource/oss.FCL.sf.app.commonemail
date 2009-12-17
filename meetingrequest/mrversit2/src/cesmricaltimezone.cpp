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
* Description: This file implements class CESMRICalTimeZone.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricaltimezone.h"	// CESMRICalTimeZone

//debug

// User includes.
#include "esmricalkeywords.h"			// Literals

/**
Static factory construction
@return A new CESMRICalTimeZone
@internalTechnology
*/
CESMRICalTimeZone* CESMRICalTimeZone::NewLC(TICalMethod aMethod)
	{
    FUNC_LOG;
	
	CESMRICalTimeZone* self = new (ELeave) CESMRICalTimeZone;
	CleanupStack::PushL(self);
	self->ConstructL(aMethod);
	
	return self;
	}

/**
Destructor
@internalTechnology
*/
CESMRICalTimeZone::~CESMRICalTimeZone()
	{
    FUNC_LOG;
	}

/**
Constructor
@internalTechnology
*/
CESMRICalTimeZone::CESMRICalTimeZone()
	{
    FUNC_LOG;
	
	iComponentType = EICalTimeZone;
	iComponentMethodBitMask = EMaskTimezoneAny;
	
	}

/**
Internal construction
@internalTechnology
*/
void CESMRICalTimeZone::ConstructL(TICalMethod aMethod)
	{
    FUNC_LOG;
	
	iMethod = aMethod;
	
	}
	
/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName
@param aName the name of the property to validate
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CESMRICalTimeZone::ValidatePropertyImpl(const TDesC& aName) const
	{
    FUNC_LOG;
	
	if ((aName.CompareF(KICalTzid) == 0) || (aName.CompareF(KICalLastmodified) == 0) || (aName.CompareF(KICalTzurl) == 0))
		{
		//	1		TZID
		//	0..1	LAST-MOD
		//	0..1	TZURL
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
TBool CESMRICalTimeZone::ValidateComponent(TICalComponentType aType) const
	{
    FUNC_LOG;
	// VTIMEZONE can only contain STANDARD or DAYLIGHT components.
	return ((aType == EICalDaylight) || (aType == EICalStandard));
	}
	
// End of File

