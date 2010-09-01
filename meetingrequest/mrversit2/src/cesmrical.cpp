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
* Description: This file implements class CESMRICal.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmrical.h"	// CESMRICal

//debug

// System includes.
#include <s32strm.h>	// RWriteStream

// User includes.
#include "cesmricalbase.h"				// TICalComponentType
#include "esmricalkeywords.h"			// Literals
#include "cesmricalcontentlinewriter.h"	// CESMRICalContentLineWriter

/**
Static factory construction
@return A new CESMRICal
@publishedPartner
*/
EXPORT_C CESMRICal* CESMRICal::NewL()
	{
    FUNC_LOG;
	
	CESMRICal* self = CESMRICal::NewLC();
	CleanupStack::Pop(self);
	
	return self;
	}

/**
Static factory construction
@return A new CESMRICal
@publishedPartner
*/
EXPORT_C CESMRICal* CESMRICal::NewLC()
	{
    FUNC_LOG;
	
	CESMRICal* self = new (ELeave) CESMRICal;
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return self;
	}
	
EXPORT_C void CESMRICal::SetMethodL(TICalMethod aMethod)
	{
    FUNC_LOG;
	
	iMethod = aMethod;
	
	}

/**
Destructor
@internalTechnology
*/
CESMRICal::~CESMRICal()
	{
    FUNC_LOG;
	}
	
	

/**
Creates a CESMRICalContentLineWriter from aWriteStream and calls
CICalbase::Externalize with it.
@param aWriteStream The stream to externalize to.
@publishedPartner
*/
EXPORT_C void CESMRICal::ExternalizeL(RWriteStream& aWriteStream)
	{
    FUNC_LOG;
	
	CESMRICalContentLineWriter* writer = CESMRICalContentLineWriter::NewLC(aWriteStream);
	CESMRICalBase::ExternalizeL(*writer);
	CleanupStack::PopAndDestroy(writer);
	
	}

/**
Constructor
@internalTechnology
*/
CESMRICal::CESMRICal()
	{
    FUNC_LOG;
	
	iComponentType = EICalCalendar;
	
	}

/**
Internal construction
@internalTechnology
*/
void CESMRICal::ConstructL()
	{
    FUNC_LOG;
	}
	
/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName
@param aName the name of the property to validate
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CESMRICal::ValidatePropertyImpl(const TDesC& aName) const
	{
    FUNC_LOG;
	
	 if ((aName.CompareF(KICalProdId) == 0) || (aName.CompareF(KICalVersion) == 0) ||
	 	(aName.CompareF(KICalCalscale) == 0) || (aName.CompareF(KICalMethod) == 0))
		{
		//	1	PRODID
		//	1	VERSION
		//	0..1	CALSCALE
		//	0..1	METHOD
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
TBool CESMRICal::ValidateComponent(TICalComponentType aType) const
	{
    FUNC_LOG;
	// VCALENDAR can have all components nested except another VCALENDAR or a VALARM, STANDARD or DAYLIGHT.
	// However, we also return false for invalid components.
	return ((aType != EICalCalendar) &&
			(aType != EICalAlarm) &&
			(aType != EICalStandard) &&
			(aType != EICalDaylight) &&
			(aType != EICalInvalid));
	}
	
// End of File

