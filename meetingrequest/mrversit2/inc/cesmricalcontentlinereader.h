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
* Description:  Holds the definition of CESMRICalContentLineReader.
*
*
*/


#ifndef CESMRICALCONTENTLINEREADER_H
#define CESMRICALCONTENTLINEREADER_H


// System includes
#include <e32base.h>	// CBase

// Forward declarations.
class RReadStream;

/**
Class for reading in content lines from a stream.
@internalTechnology
*/
class CESMRICalContentLineReader : public CBase
	{
private: // Types.
    enum Flags
    	{
    	EEof = 1
    	};
public:	// Construction/destruction.
	static CESMRICalContentLineReader* NewL(RReadStream& aStream);
	static CESMRICalContentLineReader* NewLC(RReadStream& aStream);
	~CESMRICalContentLineReader();

public:	// Methods.
	TInt GetNextContentLine(TPtrC& aPtr);
	void SkipComponentL(const TDesC& aName);

private:	// Construction.
	CESMRICalContentLineReader(RReadStream& aStream);
	void ConstructL();

private:	// Methods.
	void GetLineL(TPtr& aPtr);
	void GetNextContentLineL();

private:	// Attributes.
	RReadStream& iReadStream;
	HBufC* iCurrentLine;
	HBufC* iNextLine;
	TInt iFlags;
	};

#endif	// CESMRICALCONTENTLINEREADER_H

// End of File
