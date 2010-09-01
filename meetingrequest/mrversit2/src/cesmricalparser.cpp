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
* Description: This file implements class CESMRICalParser.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricalparser.h"	// CESMRICalParser

//debug

// User includes.
#include "cesmrical.h"					// CESMRICal
#include "cesmricalcontentlinereader.h"	// CESMRICalContentLineReader
#include "esmricalkeywords.h"			// Literals
#include "esmricalutil.h"				// NESMRICalUtil

using namespace NESMRICalUtil;

/**
Allocates and constructs a new CESMRICalParser.
@return A new CESMRICalParser.
@publishedPartner
*/
EXPORT_C CESMRICalParser* CESMRICalParser::NewL()
	{
    FUNC_LOG;
	
	CESMRICalParser* self = CESMRICalParser::NewLC();
	CleanupStack::Pop(self);
	
	return self;
	}
	
/**
Allocates and constructs a new CESMRICalParser.
The pointer to the new object is left on the Cleanup Stack.
@return  A new CESMRICalParser.
@publishedPartner
*/
EXPORT_C CESMRICalParser* CESMRICalParser::NewLC()
	{
    FUNC_LOG;
	
	CESMRICalParser* self = new (ELeave) CESMRICalParser;
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return self;
	}

/**
Destructor
@internalTechnology
*/
CESMRICalParser::~CESMRICalParser()
	{
    FUNC_LOG;
	
	iCals.ResetAndDestroy();
	}

/**
Creates a CICalContentLinerReader from aReadStream and parses iCalendar
information.
@param aReadStream The stream to read from.
@publishedPartner
*/
EXPORT_C void CESMRICalParser::InternalizeL(RReadStream& aReadStream)
	{
    FUNC_LOG;
	
	CESMRICalContentLineReader* reader = CESMRICalContentLineReader::NewLC(aReadStream);
	
	TPtrC line;
	TPtrC name;
	TPtrC parameters;
	TPtrC values;
	
	TInt error = KErrNone;
	
	while ((error = reader->GetNextContentLine(line)) == KErrNone)
		{
		if (ExtractSectionsL(line, name, parameters, values) == KErrNone)
			{
			if ((name.CompareF(KICalBegin) == 0) && (values.CompareF(KICalVCalendar) == 0))
				{
				// This is the start of a new iCalendar
				CESMRICal* cal = CESMRICal::NewLC();
				cal->InternalizeL(*reader);
				User::LeaveIfError(iCals.Append(cal));
				CleanupStack::Pop(cal);
				}
				
			// Else not a VCALENDAR - ignore it.
			}
			
		// Else no property value given on the line.
		}
		
	if ((error != KErrEof) && (error != KErrNone))
		{
		User::Leave(error);
		}
		
	CleanupStack::PopAndDestroy(reader);
	}
	
/**
Get the number of CESMRICal objects owned by this CESMRICalParser.
@return The number of CESMRICal objects.
@publishedPartner
*/
EXPORT_C TInt CESMRICalParser::CalCount() const
	{
    FUNC_LOG;
	return iCals.Count();
	}
	
/**
Returns the iCalendar at the given index.
@param aIndex The iCalendar required.
@return A modifiable reference to the required iCalendar.
@publishedPartner
*/
EXPORT_C CESMRICal& CESMRICalParser::Cal(TInt aIndex)
	{
    FUNC_LOG;
	return *iCals[aIndex];
	}
	
/**
Default constructor.
@internalTechnology
*/
CESMRICalParser::CESMRICalParser()
	{
    FUNC_LOG;
	}

/**
Second phase construction.
@internalTechnology
*/
void CESMRICalParser::ConstructL()
	{
    FUNC_LOG;
	}

// End of File

