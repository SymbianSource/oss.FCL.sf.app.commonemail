/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CESMRICalContentLineWriter.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricalcontentlinewriter.h" // CESMRICalContentLineWriter

//debug

// System includes.
#include <s32strm.h>    // RWriteStream
#include <utf.h>        // CnvUtfConverter

namespace{
// Constants.
const TInt KICalMaxLineLength = 75; // The length of a line allowed by RFC 2445.
const TInt KICalMaxFoldedLineLength = KICalMaxLineLength - 1;   // Allows for the extra space at the start of the line.

// Line folding literals.
_LIT8(KICalCRLF,"\r\n");
_LIT8(KICalFoldLine,"\r\n ");
}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRICalContentLineWriter::NewL
// ---------------------------------------------------------------------------
//
CESMRICalContentLineWriter* CESMRICalContentLineWriter::NewL(RWriteStream& aStream)
    {
    FUNC_LOG;

    CESMRICalContentLineWriter* self = CESMRICalContentLineWriter::NewLC(aStream);
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineWriter::NewLC
// ---------------------------------------------------------------------------
//
CESMRICalContentLineWriter* CESMRICalContentLineWriter::NewLC(RWriteStream& aStream)
    {
    FUNC_LOG;

    CESMRICalContentLineWriter* self = new (ELeave) CESMRICalContentLineWriter(aStream);
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineWriter::~CESMRICalContentLineWriter
// ---------------------------------------------------------------------------
//
CESMRICalContentLineWriter::~CESMRICalContentLineWriter()
    {
    FUNC_LOG;

    delete iCurrentLine;

    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineWriter::WriteContentLineL
// ---------------------------------------------------------------------------
//
void CESMRICalContentLineWriter::WriteContentLineL()
    {
    FUNC_LOG;

    // Convert to UTF-8 for writing
    HBufC8* tmpLine = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*iCurrentLine);
    CleanupStack::PushL(tmpLine);

    TInt pos(0);
    TInt remaining(tmpLine->Length());

    // Fold the line if longer than 75 octets
    TInt lineLength(KICalMaxLineLength);

    while (remaining > lineLength)
        {
        iWriteStream.WriteL(tmpLine->Mid(pos), lineLength);
        iWriteStream.WriteL(KICalFoldLine);
        pos += lineLength;
        remaining -= lineLength;
        lineLength = KICalMaxFoldedLineLength;
        }

    // Complete the line
    iWriteStream.WriteL(tmpLine->Mid(pos));
    iWriteStream.WriteL(KICalCRLF);

    CleanupStack::PopAndDestroy(tmpLine);

    iCurrentLine->Des().SetLength(0);

    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineWriter::AppendL
// ---------------------------------------------------------------------------
//
void CESMRICalContentLineWriter::AppendL(const TDesC& aLine)
    {
    FUNC_LOG;

    if (!iCurrentLine)
        {
        iCurrentLine = HBufC::NewL(KICalMaxLineLength);
        }

    if ((iCurrentLine->Length() + aLine.Length()) > iCurrentLine->Des().MaxLength())
        {
        // Reallocate the buffer
        iCurrentLine = iCurrentLine->ReAllocL(iCurrentLine->Des().MaxLength() + aLine.Length());
        }

    iCurrentLine->Des().Append(aLine);
    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineWriter::AppendL
// ---------------------------------------------------------------------------
//
void CESMRICalContentLineWriter::AppendL(const TChar& aCharacter)
    {
    FUNC_LOG;

    if (!iCurrentLine)
        {
        iCurrentLine = HBufC::NewL(KICalMaxLineLength);
        }

    if ((iCurrentLine->Length() + 1) > iCurrentLine->Des().MaxLength())
        {
        // Reallocate the buffer - expand by KICalMaxLineLength rather than '1'
        // to allow space for further expansion without further reallocation.
        iCurrentLine = iCurrentLine->ReAllocL(iCurrentLine->Des().MaxLength() + KICalMaxLineLength);
        }

    iCurrentLine->Des().Append(aCharacter);

    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineWriter::CESMRICalContentLineWriter
// ---------------------------------------------------------------------------
//
CESMRICalContentLineWriter::CESMRICalContentLineWriter(RWriteStream& aStream)
 :  iWriteStream(aStream)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineWriter::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRICalContentLineWriter::ConstructL()
    {
    FUNC_LOG;
    iCurrentLine = HBufC::NewL(KICalMaxLineLength);
    }

// End of File

