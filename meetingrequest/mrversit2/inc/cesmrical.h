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
* Description:  Holds the definiton of CESMRICal.
*
*
*/


#ifndef CESMRICAL_H
#define CESMRICAL_H


// User includes.
#include "cesmricalbase.h"	// CESMRICalBase

// Forward declaration.
class RWriteStream;

/**
Class representing a VCALENDAR component.
@publishedPartner
*/
class CESMRICal : public CESMRICalBase
	{
public:	// Construction/destruction.
	IMPORT_C static CESMRICal* NewL();
	IMPORT_C static CESMRICal* NewLC();
	~CESMRICal();

public:	// Methods.
	IMPORT_C void ExternalizeL(RWriteStream& aWriteStream);
	IMPORT_C void SetMethodL(TICalMethod aMethod);

private:	// Construction.
	CESMRICal();
	void ConstructL();

private:	// From CESMRICalBase.
	TBool ValidatePropertyImpl(const TDesC& aName) const;
	TBool ValidateComponent(TICalComponentType aType) const;
	};

#endif	// CESMRICAL_H

// End of File
