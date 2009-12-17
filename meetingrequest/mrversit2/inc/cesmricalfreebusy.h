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
* Description:  Holds the definition of CESMRICalFreeBusy.
*
*
*/


#ifndef CESMRICALFREEBUSY_H
#define CESMRICALFREEBUSY_H


// User includes
#include "cesmricalcomponent.h"	// CESMRICalComponent

/**
Class representing a VFREEBUSY component.
@internalTechnology
*/
class CESMRICalFreeBusy : public CESMRICalComponent
	{
public:	// Construction/destruction.
	static CESMRICalFreeBusy* NewLC(TICalMethod aMethod);
	~CESMRICalFreeBusy();

private:	// Construction.
	CESMRICalFreeBusy();
	void ConstructL(TICalMethod aMethod);

private:	// From CESMRICalBase.
	TBool ValidatePropertyImpl(const TDesC& aName) const;
	TBool ValidateComponent(TICalComponentType aType) const;
	};

#endif	// CESMRICALFREEBUSY_H

// End of File
