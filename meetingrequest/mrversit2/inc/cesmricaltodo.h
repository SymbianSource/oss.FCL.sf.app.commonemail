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
* Description:  Holds the definition of CESMRICalTodo.
*
*
*/


#ifndef CESMRICALTODO_H
#define CESMRICALTODO_H

// User includes.
#include "cesmricalcomponent.h"	// CESMRICalComponent

/**
Class representing a VTODO component.
@internalTechnology
*/
class CESMRICalTodo : public CESMRICalComponent
	{
public:	// Construction.
	static CESMRICalTodo* NewLC(TICalMethod aMethod);
	~CESMRICalTodo();

private:	// Construction.
	CESMRICalTodo();
	void ConstructL(TICalMethod aMethod);

private:	// From CESMRICalBase.
	TBool ValidatePropertyImpl(const TDesC& aName) const;
	TBool ValidateComponent(TICalComponentType aType) const;
	};

#endif	// CESMRICALTODO_H

// End of File
