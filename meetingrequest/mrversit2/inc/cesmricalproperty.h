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
* Description:  Holds the definition of CESMRICalProperty.
*
*
*/


#ifndef CESMRICALPROPERTY_H
#define CESMRICALPROPERTY_H


// System includes.
#include <e32base.h>	// CBase

// Forward declarations.
class CESMRICalPropertyParam;
class CESMRICalValue;
class CESMRICalContentLineWriter;

/**
Class representing a property.
@publishedPartner
*/
class CESMRICalProperty : public CBase
	{
public:	// Construction/destruction.
	IMPORT_C static CESMRICalProperty* NewL(const TDesC& aType, const TDesC& aValue);
	IMPORT_C static CESMRICalProperty* NewLC(const TDesC& aType, const TDesC& aValue);

	IMPORT_C static CESMRICalProperty* NewL(const TDesC& aType, CESMRICalValue* aValue);
	IMPORT_C static CESMRICalProperty* NewLC(const TDesC& aType, CESMRICalValue* aValue);

	~CESMRICalProperty();

	// Non-exported constructors.
	static CESMRICalProperty* NewL(const TDesC& aType);
	static CESMRICalProperty* NewLC(const TDesC& aType);

public:	// Methods.
	IMPORT_C const TDesC& Type() const;
	IMPORT_C CESMRICalPropertyParam& AddPropertyParamL(const TDesC& aType, CESMRICalValue* aValue);
	IMPORT_C CESMRICalPropertyParam& AddPropertyParamL(const TDesC& aType, const TDesC& aValue);
	IMPORT_C void AddValueL(const TDesC& aValue);
	IMPORT_C void AddValueL(CESMRICalValue* aValue);
	IMPORT_C const RPointerArray<CESMRICalPropertyParam>& Parameters() const;
	IMPORT_C const RPointerArray<CESMRICalValue>& Values() const;
	IMPORT_C const CESMRICalPropertyParam* FindParam(const TDesC& aType) const;

	CESMRICalPropertyParam& AddPropertyParamL();
	void RemovePropertyParamL(const CESMRICalPropertyParam& aParam);
	void ExternalizeL(CESMRICalContentLineWriter& aWriter);

private:	// Construction.
	CESMRICalProperty();
	CESMRICalProperty(CESMRICalValue* aValue);
	void ConstructL(const TDesC& aType);
	void ConstructL(const TDesC& aType, const TDesC& aValue);
	void ConstructL(const TDesC& aType, CESMRICalValue* aValue);

private:	// Attributes.
	HBufC* iType;
	CESMRICalValue* iValue;
	RPointerArray<CESMRICalPropertyParam> iParams;
	RPointerArray<CESMRICalValue> iValues;
	};

#endif	// CESMRICALPROPERTY_H

// End of File
