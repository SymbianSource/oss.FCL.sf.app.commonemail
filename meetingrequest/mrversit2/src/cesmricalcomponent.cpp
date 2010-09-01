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
* Description: This file implements class CESMRICalComponent.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricalcomponent.h"		// CESMRICalComponent

//debug

// User includes
#include "cesmricalalarm.h"				// CESMRICalAlarm
#include "cesmricalevent.h"				// CESMRICalEvent
#include "cesmricalfreebusy.h"			// CESMRICalFreeBusy
#include "cesmricaljournal.h"			// CESMRICalJournal
#include "esmricalkeywords.h"			// Literals
#include "cesmricaltimezone.h"			// CESMRICalTimeZone
#include "cesmricaltimezoneinterval.h"	// CESMRICalTimeZoneInterval
#include "cesmricaltodo.h"				// CESMRICalTodo

/**
Destructor
@internalTechnology
*/
CESMRICalComponent::~CESMRICalComponent()
	{
    FUNC_LOG;
	}

/**
Factory function for creating new components of the type indicated by the aName
argument.
@param aName The type of component to create
@return The new component.
@leave Leaves with KErrCorrupt if the requested component is unknown.
@publishedPartner
*/
EXPORT_C CESMRICalComponent* CESMRICalComponent::CreateICalComponentL(const TDesC& aName, TICalMethod aMethod)
	{
    FUNC_LOG;
	
	CESMRICalComponent* self = CreateICalComponentLC(aName, aMethod);
	CleanupStack::Pop(self);
	
	return self;
	}
	
/**
Factory function for creating new components of the type indicated by the aName
argument. Pushes the new component onto the Cleanup Stack.
@param aName The type of component to create.
@return The new component.
@leave Leaves with KErrCorrupt if the requested component is unknown.
@publishedPartner
*/
EXPORT_C CESMRICalComponent* CESMRICalComponent::CreateICalComponentLC(const TDesC& aName, TICalMethod aMethod)
	{
    FUNC_LOG;
	
	CESMRICalComponent* self = NULL;

	if (aName.CompareF(KICalEvent) == 0)
		{
		self = CESMRICalEvent::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalTodo) == 0)
		{
		self = CESMRICalTodo::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalJournal) == 0)
		{
		self = CESMRICalJournal::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalAlarm) == 0)
		{
		self = CESMRICalAlarm::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalTimeZone) == 0)
		{
		self = CESMRICalTimeZone::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalFreeBusy) == 0)
		{
		self = CESMRICalFreeBusy::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalStandard) == 0)
		{
		self = CESMRICalTimeZoneInterval::NewLC(EICalStandard, aMethod);
		}
	else if (aName.CompareF(KICalDaylight) == 0)
		{
		self = CESMRICalTimeZoneInterval::NewLC(EICalDaylight, aMethod);
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
    
	return self;
	}
	
/**
Factory function for creating new components of the type indicated by the aType
argument.
@param aType The type of component to create.
@return The new component.
@leave Leaves with KErrCorrupt if the requested component is unknown.
@publishedPartner
*/
EXPORT_C CESMRICalComponent* CESMRICalComponent::CreateICalComponentL(CESMRICalBase::TICalComponentType aType, TICalMethod aMethod)
	{
    FUNC_LOG;
	
	CESMRICalComponent* self = CreateICalComponentLC(aType, aMethod);
	CleanupStack::Pop(self);
	
	return self;	
	}
	
/**
Factory function for creating new components of the type indicated by the aType
argument. Pushes the new component onto the cleanup stack.
@param aType The type of component to create.
@return The new component.
@leave Leaves with KErrCorrupt if the requested component is unknown.
@publishedPartner
*/
EXPORT_C CESMRICalComponent* CESMRICalComponent::CreateICalComponentLC(CESMRICalBase::TICalComponentType aType, TICalMethod aMethod)
	{
    FUNC_LOG;
	
	CESMRICalComponent* self = NULL;
	
	switch (aType)
		{
		case EICalEvent:
			self = CESMRICalEvent::NewLC(aMethod);
			break;
		case EICalTodo:
			self = CESMRICalTodo::NewLC(aMethod);
			break;
		case EICalJournal:
			self = CESMRICalJournal::NewLC(aMethod);
			break;
		case EICalAlarm:
			self = CESMRICalAlarm::NewLC(aMethod);
			break;
		case EICalFreeBusy:
			self = CESMRICalFreeBusy::NewLC(aMethod);
			break;
		case EICalTimeZone:
			self = CESMRICalTimeZone::NewLC(aMethod);
			break;
		case EICalStandard:
			self = CESMRICalTimeZoneInterval::NewLC(EICalStandard, aMethod);
			break;
		case EICalDaylight:
			self = CESMRICalTimeZoneInterval::NewLC(EICalDaylight, aMethod);
			break;
		default:
			User::Leave(KErrCorrupt);
			break;
		}
    
	return self;
	}

/**
Constructor.
@internalTechnology
*/
CESMRICalComponent::CESMRICalComponent()
	{
    FUNC_LOG;
	}

// End of File

