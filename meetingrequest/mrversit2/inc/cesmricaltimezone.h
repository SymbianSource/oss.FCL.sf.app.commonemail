/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Holds the definition of CESMRICalTimeZone.
*
*
*/


#ifndef CESMRICALTIMEZONE_H
#define CESMRICALTIMEZONE_H


// User includes
#include "cesmricalcomponent.h"	// CESMRICalComponent

/**
Class representing a VTIMEZONE component.
@internalTechnology
*/
class CESMRICalTimeZone : public CESMRICalComponent
	{
public:	// Construction/destruction.
	static CESMRICalTimeZone* NewLC(TICalMethod aMethod);
	~CESMRICalTimeZone();

private:	// Construction.
	CESMRICalTimeZone();
	void ConstructL(TICalMethod aMethod);

private:	// From CESMRICalBase.
	TBool ValidatePropertyImpl(const TDesC& aName) const;
	TBool ValidateComponent(TICalComponentType aType) const;
	};

#endif // CESMRICALTIMEZONE_H

// End of File
