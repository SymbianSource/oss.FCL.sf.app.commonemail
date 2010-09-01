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
* Description:  Holds the definiton of NESMRICalUtil.
*
*
*/


#ifndef ESMRICALUTIL_H
#define ESMRICALUTIL_H

// System includes
#include <e32base.h>	// CBase

// Forward declarations.
class CESMRICalProperty;
class CESMRICalPropertyParam;

/**
Utility functions for parsing iCalendars.
@internalTechnology
*/
namespace NESMRICalUtil
	{
	enum TEscaping
		{
		/** Escape characters are dealt with as per the specification.*/
		EEscapeNormal,
		/** Escaped commas constitute value separators.*/
		EEscapeValueSeparators
		};

	TInt ExtractSectionsL(const TDesC& aLine, TPtrC& aName, TPtrC& aParameters, TPtrC& aValues);
	void ExtractPropertyL(const TDesC& aParameters, const TDesC& aValues, CESMRICalProperty& aProperty);
	void ExtractParametersL(const TDesC& aParameters, CESMRICalProperty& aProperty);
	void ExtractParameterL(const TDesC& aParameter, CESMRICalPropertyParam& aPropertyParam);
	void ExtractParameterValuesL(const TDesC& aValues, CESMRICalPropertyParam& aPropertyParam);
	void ExtractPropertyValuesL(const TDesC& aValues, CESMRICalProperty& aProperty, TEscaping aEscaping = EEscapeNormal);
	TBool PropertyHasTextValueL(const CESMRICalProperty& aProperty);
	}

#endif	// ESMRICALUTIL_H

// End of File
