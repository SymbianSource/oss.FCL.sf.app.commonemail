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
* Description:  Holds the definition of CESMRICalParser.
*
*
*/


#ifndef CESMRICALPARSER_H
#define CESMRICALPARSER_H

// System includes.
#include <e32base.h>	// CBase

// Forward declarations.
class CESMRICal;

/**
Directs parsing of iCalendar streams.
@publishedPartner
*/
class CESMRICalParser : public CBase
	{
public:	// Construction/destruction.
	IMPORT_C static CESMRICalParser* NewL();
	IMPORT_C static CESMRICalParser* NewLC();
	~CESMRICalParser();

public:	// Methods.
	IMPORT_C void InternalizeL(RReadStream& aReadStream);
	IMPORT_C TInt CalCount() const;
	IMPORT_C CESMRICal& Cal(TInt aIndex);

private:	// Construction.
	CESMRICalParser();
	void ConstructL();

private:	// Attributes.
	RPointerArray<CESMRICal> iCals;
	};

#endif //CESMRICALPARSER_H

// End of File
