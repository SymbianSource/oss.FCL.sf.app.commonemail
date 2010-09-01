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
* Description:  Holds the definition of CESMRICalPropertyParam.
*
*
*/


#ifndef CESMRICALPROPERTYPARAM_H
#define CESMRICALPROPERTYPARAM_H


// System includes
#include <e32base.h>	// CBase

// Forward declarations.
class CESMRICalValue;
class CESMRICalContentLineWriter;

/**
Class representing a property parameter.
@publishedPartner
*/
class CESMRICalPropertyParam : public CBase
	{
public:	// Construction/destruction.
	IMPORT_C static CESMRICalPropertyParam* NewL(const TDesC& aType, CESMRICalValue* aValue);
	IMPORT_C static CESMRICalPropertyParam* NewLC(const TDesC& aType, CESMRICalValue* aValue);

	IMPORT_C static CESMRICalPropertyParam* NewL(const TDesC& aType, const TDesC& aValue);
	IMPORT_C static CESMRICalPropertyParam* NewLC(const TDesC& aType, const TDesC& aValue);

	~CESMRICalPropertyParam();

	// Non-exported constructor.
	static CESMRICalPropertyParam* NewLC();

public:	// Methods
	IMPORT_C void AddValueL(const TDesC& aValue);
	IMPORT_C void AddValueL(CESMRICalValue* aValue);
	IMPORT_C const TDesC& Type() const;
	IMPORT_C const RPointerArray<CESMRICalValue>& Values() const;

	void SetTypeL(const TDesC& aType);
	void ExternalizeL(CESMRICalContentLineWriter& aWriter);
	static TBool EqualsL(const CESMRICalPropertyParam& aParam1, const CESMRICalPropertyParam& aParam2);

private:	// Construction.
	CESMRICalPropertyParam();
	CESMRICalPropertyParam(CESMRICalValue* aValue);
	void ConstructL(const TDesC& aType);
	void ConstructL(const TDesC& aType, const TDesC& aValue);
	void ConstructL(const TDesC& aType, CESMRICalValue* aValue);

private:	// Attributes.
	HBufC* iType;
	CESMRICalValue* iValue;
	RPointerArray<CESMRICalValue> iValues;
	};

#endif	// CESMRICALPROPERTYPARAM_H

// End of File
