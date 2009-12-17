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
* Description: This file implements classes CESMRICalPropertyParam, CESMRICalPropertyParam. 
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricalpropertyparam.h"	// CESMRICalPropertyParam

//debug

// User includes.
#include "cesmricalcontentlinewriter.h"	// CESMRICalContentLineWriter
#include "esmricalerrors.h"				// Error values
#include "esmricalkeywords.h"			// Literals
#include "cesmricalvalue.h"				// CESMRICalValue

/**
Static factory constructor. Takes ownership of the passed value pointer and
deletes it if this function leaves. Note that other values may be added later.
@param aType The type of parameter.
@param aValue A value of the parameter - this class takes ownership.
@return A new property parameter
@leave Leaves with KErrParameterHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C CESMRICalPropertyParam* CESMRICalPropertyParam::NewL(const TDesC& aType, CESMRICalValue* aValue)
	{
    FUNC_LOG;
	
	CESMRICalPropertyParam* self = CESMRICalPropertyParam::NewLC(aType, aValue);
	CleanupStack::Pop(self);
	
	return self;
	}

/**
Static factory constructor. Takes ownership of the passed value pointer and
deletes it if this function leaves. Note that other values may be added later.
The constructed object is pushed onto the Cleanup Stack.
@param aType The type of parameter.
@param aValue A value of the parameter - this class takes ownership.
@return A new property parameter
@leave Leaves with KErrParameterHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C CESMRICalPropertyParam* CESMRICalPropertyParam::NewLC(const TDesC& aType, CESMRICalValue* aValue)
	{
    FUNC_LOG;
	
	// Test that the value is valid before construction.
	if (!aValue)
		{
		User::Leave(KErrParameterHasNoValue);
		}
	
	// Push aValue on the Cleanup Stack.
	CleanupStack::PushL(aValue);
		
	// Create a new object.
	CESMRICalPropertyParam* self = new (ELeave) CESMRICalPropertyParam(aValue);
	
	// Pop aValue from the Cleanup Stack as it is temporarily owned by self.
	CleanupStack::Pop(aValue);
	
	// Construct the new object.
	CleanupStack::PushL(self);
	self->ConstructL(aType, aValue);
	
	return self;
	}
	
/**
Static factory constructor. Note that other values may be added later. The
constructed object is pushed onto the Cleanup Stack.
@param aType The type of parameter.
@param aValue A value of the parameter passed as a descriptor.
@return A new property parameter
@publishedPartner
*/
EXPORT_C CESMRICalPropertyParam* CESMRICalPropertyParam::NewL(const TDesC& aType, const TDesC& aValue)
	{
    FUNC_LOG;
	
	CESMRICalPropertyParam* self = NewLC(aType, aValue);
	CleanupStack::Pop(self);
	
	return self;
	}
	
/**
Static factory constructor. Note that other values may be added later. The
constructed object is pushed onto the Cleanup Stack.
@param aType The type of parameter.
@param aValue A value of the parameter passed as a descriptor.
@return A new property parameter
@publishedPartner
*/
EXPORT_C CESMRICalPropertyParam* CESMRICalPropertyParam::NewLC(const TDesC& aType, const TDesC& aValue)
	{
    FUNC_LOG;
	
	CESMRICalPropertyParam* self = new (ELeave) CESMRICalPropertyParam;
	CleanupStack::PushL(self);
	self->ConstructL(aType, aValue);
	
	return self;
	}
	
/**
Destructor
@internalTechnology
*/
CESMRICalPropertyParam::~CESMRICalPropertyParam()
	{
    FUNC_LOG;
	
	delete iValue;
	delete iType;
	iValues.ResetAndDestroy();
	
	}

/**
Static factory construction. The constructed object is pushed onto the Cleanup
Stack. Note that this constructor is not exported.
@return A new blank property parameter.
@internalTechnology
*/
CESMRICalPropertyParam* CESMRICalPropertyParam::NewLC()
	{
    FUNC_LOG;
	
	CESMRICalPropertyParam* self = new (ELeave) CESMRICalPropertyParam;
	CleanupStack::PushL(self);
	self->ConstructL(KNullDesC);	// No type set - SetTypeL() must be called on this object.
	
	return self;
	}
	
/**
Adds a string to the parameter's list of values. Any quotes are removed from
the value as they are only used to escape other characters and are not a valid
parameter value character in themselves.
@param aValue The value to add.
@leave Leaves if there is an error adding a value.
@publishedPartner
*/
EXPORT_C void CESMRICalPropertyParam::AddValueL(const TDesC& aValue)
	{
    FUNC_LOG;
	
	// First remove any double quotes. These are used to escape colons, etc.
	// in property parameter values and do not need to be stored.
	// Note that double quote is not a valid character here and this assumes
	// that they should all be removed. There is no check for matching quotes.
	HBufC* parameterValue = aValue.AllocLC();
	
	TInt location(parameterValue->Locate(KICalQuoteChar));

	while (location >= 0)
		{
		parameterValue->Des().Delete(location, 1);
		location = parameterValue->Locate(KICalQuoteChar);
		}

	if (parameterValue->Length() > 0)
		{
		// Create a new value and append to the list of values.
		CESMRICalValue* value = CESMRICalValue::NewLC();
		value->SetTextL(*parameterValue);
		User::LeaveIfError(iValues.Append(value));
		CleanupStack::Pop(value);
		}
		
	CleanupStack::PopAndDestroy(parameterValue);
	
	}
	
/**
Adds a value to the parameter's list of values, taking ownership. Note that the
value is deleted if this function leaves.
@param aValue The value to add.
@leave Leaves with KErrParameterHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C void CESMRICalPropertyParam::AddValueL(CESMRICalValue* aValue)
	{
    FUNC_LOG;
	
	if (!aValue)
		{
		User::Leave(KErrParameterHasNoValue);
		}
		
	// Append the value to the list of values.
	TInt err(iValues.Append(aValue));
	
	if (err)
		{
		delete aValue;
		User::Leave(err);
		}
	}
	
/**
Gets the type of the parameter
@return The type of the property parameter or an empty descriptor if no type is
set.
@publishedPartner
*/
EXPORT_C const TDesC& CESMRICalPropertyParam::Type() const
	{
    FUNC_LOG;
	
	if (iType)
		{
		return *iType;
		}
		
	// Else type is not set due to SetTypeL() leaving.
	return KNullDesC;
	}

/**
Access method for the value array.
@return The array of values.
@publishedPartner
*/
EXPORT_C const RPointerArray<CESMRICalValue>& CESMRICalPropertyParam::Values() const
	{
	return iValues;
	}

/**
Sets the type of the parameter to a string value
@param aType the type of the property parameter
@internalTechnology
*/
void CESMRICalPropertyParam::SetTypeL(const TDesC& aType)
	{
    FUNC_LOG;
	
	delete iType;
	iType = NULL;
	iType = aType.AllocL();
	
	}
	
/**
Export this property parameter using the given line writer. Values are quoted
if they contain characters that need to be escaped. Assumes it contains at
least one value from the check in CESMRICalProperty::ExternalizeL().
@param aWriter The content line writer to export to.
@internalTechnology
*/
void CESMRICalPropertyParam::ExternalizeL(CESMRICalContentLineWriter& aWriter)
	{
    FUNC_LOG;

	// Export the property parameter name.
	aWriter.AppendL(Type());
	aWriter.AppendL(KICalEquals());

	// Export each property parameter value.
	
	const TInt lastValue = iValues.Count() - 1;	

	for (TInt x = 0; x <= lastValue; ++x)
		{
		// Decide if this value needs to be escaped - in other words
		// if it contains a colon, a semi-colon or a comma.
		const TDesC& value = iValues[x]->TextL();
		
		if ((value.Locate(KICalCommaChar) >= 0) ||
			(value.Locate(KICalColonChar) >= 0) ||
			(value.Locate(KICalSemiColonChar) >= 0))
			{
			// Escape by quoting entire string.
			aWriter.AppendL(KICalQuote());
			aWriter.AppendL(value);
			aWriter.AppendL(KICalQuote());
			}
		else
			{
			// No need to escape value.
			aWriter.AppendL(value);
			}

		// Write out a comma separator between values.			
		if (x < lastValue)
			{
			aWriter.AppendL(KICalComma());
			}
		}
	}
	
/**
Determines if two property parameters are equal. Comparison is case-sensitive.
@param aParam1 The first property parameter.
@param aParam2 The second property parameter.
@return ETrue if they are equal.
@internalTechnology
*/
TBool CESMRICalPropertyParam::EqualsL(const CESMRICalPropertyParam& aParam1, const CESMRICalPropertyParam& aParam2)
	{
    FUNC_LOG;
	
	if (aParam1.Type() != aParam2.Type())
		{
		return EFalse;
		}
	
	const TInt values1 = aParam1.iValues.Count();
	const TInt values2 = aParam2.iValues.Count();
	
	if (values1 != values2)
		{
		return EFalse;
		}
		
	for (TInt i = 0; i < values1; i++)
		{
		if (aParam1.iValues[i]->TextL() != aParam2.iValues[i]->TextL())
			{
			return EFalse;
			}
		}
	
	return ETrue;
	}
	
/**
Constructor
@internalTechnology
*/
CESMRICalPropertyParam::CESMRICalPropertyParam()
	{
    FUNC_LOG;
	}
	
/**
Constructor
@param aValue The initial value.
@internalTechnology
*/
CESMRICalPropertyParam::CESMRICalPropertyParam(CESMRICalValue* aValue)
 :	iValue(aValue)
	{
    FUNC_LOG;
	}
	
/**
Internal construction.
@param aType The type of parameter.
@internalTechnology
*/
void CESMRICalPropertyParam::ConstructL(const TDesC& aType)
	{
    FUNC_LOG;
	
	iType = aType.AllocL();
	
	}

/**
Internal construction.
@param aType The type of parameter.
@param aValue A value of the parameter.
@internalTechnology
*/
void CESMRICalPropertyParam::ConstructL(const TDesC& aType, const TDesC& aValue)
	{
    FUNC_LOG;
	
	ConstructL(aType);
	
	// Add the value.
	AddValueL(aValue);
	
	}
	
/**
Internal construction. Takes ownership of the passed value pointer and
deletes it if this function leaves.
@param aType The type of parameter.
@param aValue A value of the parameter - this class takes ownership.
@internalTechnology
*/
void CESMRICalPropertyParam::ConstructL(const TDesC& aType, CESMRICalValue* aValue)
	{
    FUNC_LOG;
	
	ConstructL(aType);
	
	// Add the value.
	iValue = NULL;	// If we got this far then we can release direct ownership of the value.
	AddValueL(aValue);	// Value is deleted if this leaves.
	
	}

// End of File

