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
* Description:  Holds the definition of CESMRICalContentLineWriter.
*
*
*/


#ifndef CESMRICALCONTENTLINEWRITER_H
#define CESMRICALCONTENTLINEWRITER_H


// System includes.
#include <e32base.h>	// CBase

// Forward declarations.
class RWriteStream;

/**
Class for writing out content lines to a stream.
@internalTechnology
*/
class CESMRICalContentLineWriter : public CBase
	{
public:	// Construction/destruction.
	static CESMRICalContentLineWriter* NewL(RWriteStream& aStream);
	static CESMRICalContentLineWriter* NewLC(RWriteStream& aStream);
	~CESMRICalContentLineWriter();

public:	// Methods.
	void WriteContentLineL();
	void AppendL(const TDesC& aLine);
	void AppendL(const TChar& aCharacter);

private:	// Construction.
	CESMRICalContentLineWriter(RWriteStream& aStream);
	void ConstructL();

private:	// Attributes.
	RWriteStream& iWriteStream;
	HBufC* iCurrentLine;
	};

#endif	// CESMRICALCONTENTLINEWRITER_H

// End of File
