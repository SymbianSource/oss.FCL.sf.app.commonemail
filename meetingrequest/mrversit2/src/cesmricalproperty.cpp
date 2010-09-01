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
* Description: This file implements classes CESMRICalProperty, CESMRICalProperty. 
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricalproperty.h"	// CESMRICalProperty

//debug

// User includes.
#include "cesmricalcontentlinewriter.h"	// CESMRICalContentLineWriter
#include "esmricalerrors.h"				// Error values
#include "esmricalkeywords.h"			// Literals
#include "cesmricalpropertyparam.h"		// CESMRICalPropertyParam
#include "esmricalutil.h"				// NESMRICalUtil
#include "cesmricalvalue.h"				// CESMRICalValue

/**
Constructs a new CESMRICalProperty with given type and value and returns it.
Note that other values may be added later.
@param aType The type of the property.
@param aValue The value of the property.
@return a pointer to a new CESMRICalProperty
@publishedPartner
*/
EXPORT_C CESMRICalProperty* CESMRICalProperty::NewL(const TDesC& aType, const TDesC& aValue)
	{
    FUNC_LOG;
	
	CESMRICalProperty* self = CESMRICalProperty::NewLC(aType, aValue);
	CleanupStack::Pop(self);
	
	return self;
	}

/**
Constructs a new CESMRICalProperty with given type and value, pushes it onto the
Cleanup Stack, and returns it. Note that other values may be added later.
@param aType The type of the property.
@param aValue The value of the property.
@return a pointer to a new CESMRICalProperty
@publishedPartner
*/
EXPORT_C CESMRICalProperty* CESMRICalProperty::NewLC(const TDesC& aType, const TDesC& aValue)
	{
    FUNC_LOG;
	
	CESMRICalProperty* self = new (ELeave) CESMRICalProperty;
	CleanupStack::PushL(self);
	self->ConstructL(aType, aValue);
	
	return self;
	}

/**
Constructs a new CESMRICalProperty with given type and value and returns it. Takes
ownership of the passed value pointer and deletes it if this function leaves.
Note that other values may be added later.
@param aType The type of the property.
@param aValue The value of the property.
@return a pointer to a new CESMRICalProperty
@leave Leaves with KErrPropertyHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C CESMRICalProperty* CESMRICalProperty::NewL(const TDesC& aType, CESMRICalValue* aValue)
	{
    FUNC_LOG;
	
	CESMRICalProperty* self = CESMRICalProperty::NewLC(aType, aValue);
	CleanupStack::Pop(self);
	
	return self;
	}

/**
Constructs a new CESMRICalProperty with given type and value, pushes it onto the
Cleanup Stack, and returns it. Takes ownership of the passed value pointer and
deletes it if this function leaves. Note that other values may be added later.
@param aType The type of the property.
@param aValue The value of the property.
@return a pointer to a new CESMRICalProperty
@leave Leaves with KErrPropertyHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C CESMRICalProperty* CESMRICalProperty::NewLC(const TDesC& aType, CESMRICalValue* aValue)
	{
    FUNC_LOG;
	
	// Test that the value is valid before construction.
	if (!aValue)
		{
		User::Leave(KErrPropertyHasNoValue);
		}
		
	// Push the passed value onto the Cleanup Stack.
	CleanupStack::PushL(aValue);
	
	// Create a new property.
	CESMRICalProperty* self = new (ELeave) CESMRICalProperty(aValue);

	// Pop the passed value from the Cleanup Stack as it is temporarily owned by self.
	CleanupStack::Pop(aValue);

	// Construct the property.
	CleanupStack::PushL(self);
	self->ConstructL(aType, aValue);
	
	return self;
	}

/**
Destructor.
@internalTechnology
*/
CESMRICalProperty::~CESMRICalProperty()
	{
    FUNC_LOG;
	
	delete iValue;
	delete iType;
	iParams.ResetAndDestroy();
	iValues.ResetAndDestroy();
	
	}

/**
Constructs a new CESMRICalProperty with given type and returns it
@param aType The type of the property.
@return a pointer to a new CESMRICalProperty
@internalTechnology
*/
CESMRICalProperty* CESMRICalProperty::NewL(const TDesC& aType)
	{
    FUNC_LOG;
	
	CESMRICalProperty* self = CESMRICalProperty::NewLC(aType);
	CleanupStack::Pop(self);
	
	return self;
	}

/**
Constructs a new CESMRICalProperty with given type, pushes it onto the Cleanup
Stack, and returns it.
@param aType The type of the property.
@return a pointer to a new CESMRICalProperty.
@internalTechnology
*/
CESMRICalProperty* CESMRICalProperty::NewLC(const TDesC& aType)
	{
    FUNC_LOG;
	
	CESMRICalProperty* self = new (ELeave) CESMRICalProperty;
	CleanupStack::PushL(self);
	self->ConstructL(aType);
	
	return self;
	}

/**
Gets the type of the property.
@return The type of the property as a string.
@publishedPartner
*/
EXPORT_C const TDesC& CESMRICalProperty::Type() const
	{
    FUNC_LOG;
	return *iType;
	}
	
/**
Adds a parameter with given type and value to the property, taking ownership of
the value. Note that further values may be added later.
@param aType The type of property parameter.
@param aValue A value of the property parameter - this will be deleted if this function leaves.
@return A new property parameter.
@leave Leaves with KErrParameterHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C CESMRICalPropertyParam& CESMRICalProperty::AddPropertyParamL(const TDesC& aType, CESMRICalValue* aValue)
	{
    FUNC_LOG;
	
	if (!aValue)
		{
		User::Leave(KErrParameterHasNoValue);
		}
	
	CESMRICalPropertyParam* param = CESMRICalPropertyParam::NewLC(aType, aValue);
	User::LeaveIfError(iParams.Append(param));
	CleanupStack::Pop(param);
	
	return *param;
	}

/**
Adds a parameter with given type and value to the property.
Note that further values may be added later.
@param aType The type of property parameter.
@param aValue A value of the property parameter.
@return A new property parameter.
@leave Leaves with if there is an error adding a parameter.
@publishedPartner
*/
EXPORT_C CESMRICalPropertyParam& CESMRICalProperty::AddPropertyParamL(const TDesC& aType, const TDesC& aValue)
	{
    FUNC_LOG;
	
	CESMRICalPropertyParam* param = CESMRICalPropertyParam::NewLC(aType, aValue);
	User::LeaveIfError(iParams.Append(param));
	CleanupStack::Pop(param);
	
	return *param;
	}

/**
Adds a value to the property. Any escaped characters in the value string are
translated to the actual characters.
@param aValue The text string of the value, as would be entered into an
iCalendar file.
@leave Leaves if there is an error adding a value.
@publishedPartner
*/
EXPORT_C void CESMRICalProperty::AddValueL(const TDesC& aValue)
	{
    FUNC_LOG;
	
	HBufC* parameterValue = NULL;

	// Create a new value.
	CESMRICalValue* value = CESMRICalValue::NewLC();

	// If the property has a text value then find any escaped characters and convert
	// them. Property values use backslash to escape the following characters:
	// commas, semi-colons, newlines (using \n or \N) and backslash itself.

	if (NESMRICalUtil::PropertyHasTextValueL(*this))
		{
		parameterValue = aValue.AllocLC();
		TInt length(parameterValue->Length());
		TBool escaped(EFalse);
		
		for (TInt location = 0; location < length; location++)
			{
			if (escaped)
				{
				if (((*parameterValue)[location] == KICalUpperCaseNChar) ||
					((*parameterValue)[location] == KICalLowerCaseNChar))
					{
					// Found an escaped newline - replace with a single linefeed character.
					(parameterValue->Des()).operator[](location) = KICalNewlineChar;
					}
				
				// Else found an escaped backslash, semi-colon or comma, or
				// a single backslash (which is not valid).
				
				// In all cases remove the previous backslash character.
				// This assumes that (location - 1) >= 0, since escaped == ETrue
				// (The previous character was a backslash, so there *is* a previous character.)
				parameterValue->Des().Delete(location - 1, 1);
				length--;	// The length of the descriptor has shrunk by one.
				location--;	// The current character has slipped back a place.
				escaped = EFalse;
				}
			else 
			    {
			    if ((*parameterValue)[location] == KICalBackslashChar)
				    {
				    escaped = ETrue;
				
				    if (location == (length - 1))
					    {
					    // Found a single backslash at the end - this is not valid, so remove it.
					    parameterValue->Des().Delete(location, 1);
					    break;
					    }
				    }			    
			    }
			}
		}

	// Set the value.
	if (parameterValue)
		{
		value->SetTextL(*parameterValue);
		CleanupStack::PopAndDestroy(parameterValue);
		}
	else
		{
		// If the property does not have a text type then just use the given string as is...
		value->SetTextL(aValue);
		}	

	// Append the new value to the list of values.
	User::LeaveIfError(iValues.Append(value));
	CleanupStack::Pop(value);
	}

/**
Adds value to this property, taking ownership of the passed value pointer and
deleting it if this function leaves.
@param aValue The value to add - this must not be NULL.
@leave Leaves with KErrPropertyHasNoValue if aValue is NULL.
@publishedPartner
*/
EXPORT_C void CESMRICalProperty::AddValueL(CESMRICalValue* aValue)
	{
    FUNC_LOG;
	
	if (!aValue)
		{
		User::Leave(KErrPropertyHasNoValue);
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
Access method for the array of parameters.
@return The array of parameters.
@publishedPartner
*/
EXPORT_C const RPointerArray<CESMRICalPropertyParam>& CESMRICalProperty::Parameters() const
	{
	return iParams;
	}
	
/**
Access method for the array of properties.
@return The array of properties.
@publishedPartner
*/
EXPORT_C const RPointerArray<CESMRICalValue>& CESMRICalProperty::Values() const
	{
	return iValues;
	}
	
/**
Find a parameter with the given name. Ownership is not passed out.
@param aType The type of the parameter to search for
@return A pointer to the parameter, or NULL if it is not found.
@publishedPartner
*/
EXPORT_C const CESMRICalPropertyParam* CESMRICalProperty::FindParam(const TDesC& aType) const
	{
    FUNC_LOG;
	
	const TInt count = iParams.Count();
	
	for (TInt p = 0; p < count; ++p)
		{
		if (iParams[p]->Type().CompareF(aType) == 0)
			{
			return iParams[p];
			}
		}

	return NULL;
	}

/**
Adds an empty parameter to the property.
@return A new, empty parameter.
@leave Leaves if there is an error adding a parameter.
@internalTechnology
*/
CESMRICalPropertyParam& CESMRICalProperty::AddPropertyParamL()
	{
    FUNC_LOG;
	
	CESMRICalPropertyParam* param = CESMRICalPropertyParam::NewLC();
	User::LeaveIfError(iParams.Append(param));
	CleanupStack::Pop(param);
	
	return *param;
	}

/**
Removes the given parameter from the property.
@param aParam The parameter to remove.
@leave Leaves in debug mode if the parameter is not found (release mode ignores
this).
@internalTechnology
*/
void CESMRICalProperty::RemovePropertyParamL(const CESMRICalPropertyParam& aParam)
	{
    FUNC_LOG;
	
	// Find the parameter to remove.
	TInt index = iParams.Find(&aParam, CESMRICalPropertyParam::EqualsL);

#ifdef _DEBUG
	User::LeaveIfError(index);	// Not expecting this.
#else	// _DEBUG
	if (index < 0)
		{
		return;	// Return quietly in release builds
		}
#endif	// _DEBUG
	
	// Destroy the parameter and remove it.
	delete iParams[index];
	iParams.Remove(index);
	
	}
	
/**
Export this property using the given line writer, escaping any necessary
characters. Assumes it contains at least one value from the check in
CESMRICalBase::ExternalizeL().
@param aWriter The content line writer to export to.
@internalTechnology
*/
void CESMRICalProperty::ExternalizeL(CESMRICalContentLineWriter& aWriter)
	{
    FUNC_LOG;
	
	// Export property name.
	aWriter.AppendL(*iType);
	
	// Export parameters. 
	const TInt paramCount = iParams.Count();
	
	for (TInt x = 0; x < paramCount; ++x)
		{
		if (iParams[x]->Values().Count() > 0)
			{
			aWriter.AppendL(KICalSemiColon());
			iParams[x]->ExternalizeL(aWriter);
			}
		}
	
	// Export values.
	aWriter.AppendL(KICalColon());
	
	const TInt valCount = iValues.Count();
	
	for (TInt x = 0; x < valCount; ++x)
		{
		const TDesC& value = iValues[x]->TextL();
		
		// If the property has a text value then escape any necessary characters.
		if (NESMRICalUtil::PropertyHasTextValueL(*this))
			{
			const TInt valueLength = value.Length();
			
			// Export each value a character at a time, translating any characters that need escaping.
			for (TInt character = 0; character < valueLength; character++)
				{
				// Property values use backslash to escape the following characters:
				// commas, semi-colons, newlines (using \n or \N) and backslash itself.
				if ((value[character] == KICalBackslashChar) ||
					(value[character] == KICalSemiColonChar) ||
					(value[character] == KICalCommaChar))
					{
					aWriter.AppendL(KICalBackslash());
					aWriter.AppendL(value[character]);
					}
				else if (value[character] == KICalNewlineChar)
					{
					aWriter.AppendL(KICalBackslashN());
					}
				else
					{
					aWriter.AppendL(value[character]);
					}
				}
			}
		else	// No escaping needed.
			{
			aWriter.AppendL(value);
			}
		
		if (x < valCount -1)
			{
			aWriter.AppendL(KICalComma());
			}
		}
		
	aWriter.WriteContentLineL();
	
	}

/**
Constructor
@internalTechnology
*/
CESMRICalProperty::CESMRICalProperty()
	{
    FUNC_LOG;
	}

/**
Constructor
@param aValue The initial value.
@internalTechnology
*/
CESMRICalProperty::CESMRICalProperty(CESMRICalValue* aValue)
 :	iValue(aValue)
	{
    FUNC_LOG;
	}

/**
Internal construction
@param aType The type of property.
@internalTechnology
*/
void CESMRICalProperty::ConstructL(const TDesC& aType)
	{
    FUNC_LOG;
	
	iType = aType.AllocL();
	
	}

/**
Internal construction.
@param aType The type of property.
@param aValue A value of the property.
@internalTechnology
*/
void CESMRICalProperty::ConstructL(const TDesC& aType, const TDesC& aValue)
	{
    FUNC_LOG;
	
	ConstructL(aType);
	
	// Add the value.
	AddValueL(aValue);
	
	}
	
/**
Internal construction. Takes ownership of the passed value pointer and
deletes it if this function leaves.
@param aType The type of property.
@param aValue A value of the property - this class takes ownership.
@internalTechnology
*/
void CESMRICalProperty::ConstructL(const TDesC& aType, CESMRICalValue* aValue)
	{
    FUNC_LOG;
	
	ConstructL(aType);
	
	// Add the value.
	iValue = NULL;	// If we got this far then we can release direct ownership of the value.
	AddValueL(aValue);	// Value is deleted if this leaves.
	
	}

// End of File

