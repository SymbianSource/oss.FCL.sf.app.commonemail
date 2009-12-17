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
* Description: This file implements class CESMRICalContentLineReader.
*
*/


// Class include.
#include "emailtrace.h"
#include "cesmricalcontentlinereader.h" // CESMRICalContentLineReader

//debug

// System includes.
#include <s32strm.h>    // RReadStream
#include <utf.h>        // CnvUtfConverter

// User includes.
#include "esmricalkeywords.h"   // Literals.

namespace
{
// Constants.
const TInt KICalSkipWhiteSpaceLength = 1;
const TInt KICalSkipCRLength = 1;
const TInt KICalStartChar = 0;

/*
The iCalendar specification states that a content line should be no longer than
75 octets in length, excluding the CRLF delimiter. It appears that MS Outlook
does not include the length of the property string when calculating the length
of a content line, meaning that a content line in an iCalendar exported from MS
Outlook may be longer than the allowed 75 octets. KICalMaxContentLineLength
allows a little extra space to support this.
*/
const TInt KICalMaxContentLineLength = 120;
}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRICalContentLineReader::NewL
// ---------------------------------------------------------------------------
//
CESMRICalContentLineReader* CESMRICalContentLineReader::NewL(RReadStream& aStream)
    {
    FUNC_LOG;

    CESMRICalContentLineReader* self = CESMRICalContentLineReader::NewLC(aStream);
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineReader::NewLC
// ---------------------------------------------------------------------------
//
CESMRICalContentLineReader* CESMRICalContentLineReader::NewLC(RReadStream& aStream)
    {
    FUNC_LOG;

    CESMRICalContentLineReader* self = new (ELeave) CESMRICalContentLineReader(aStream);
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineReader::~CESMRICalContentLineReader
// ---------------------------------------------------------------------------
//
CESMRICalContentLineReader::~CESMRICalContentLineReader()
    {
    FUNC_LOG;

    delete iCurrentLine;
    delete iNextLine;

    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineReader::GetNextContentLine
// ---------------------------------------------------------------------------
//
TInt CESMRICalContentLineReader::GetNextContentLine(TPtrC& aPtr)
    {
    FUNC_LOG;

    if (iFlags & EEof)
        {
        return KErrEof;
        }

    TInt error(KErrNone);

    // Trap errors as the stream will leave with KErrEof when we reach the end.
    TRAP(error, GetNextContentLineL());

    aPtr.Set(iCurrentLine->Des());

    if (error == KErrEof)
        {
        iFlags |= EEof;
        error = KErrNone;
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineReader::SkipComponentL
// ---------------------------------------------------------------------------
//
void CESMRICalContentLineReader::SkipComponentL(const TDesC& aName)
    {
    FUNC_LOG;

    // Take a copy of the name, since GetNextContentLine may corrupt the original
    HBufC* beginName = aName.AllocLC();

    // Find the next BEGIN: or END:
    TPtrC contentLine;
    TInt error(GetNextContentLine(contentLine));
    while (error == KErrNone)
        {
        // Find the colon separator
        const TInt colonPos = contentLine.Locate(KICalColonChar);
        // If there isn't one it's an error
        if (colonPos == KErrNotFound)
            {
            User::Leave(KErrCorrupt);
            }
        // Find the string preceding the colon
        TPtrC name(contentLine.Left(colonPos));
        // If it's a begin, recursively skip this component also
        if (name.CompareF(KICalBegin) == 0)
            {
            TPtrC value(contentLine.Mid(colonPos + 1));
            SkipComponentL(value);
            }
        // If it's an end, check that it's the right end
        else if (name.CompareF(KICalEnd) == 0)
            {
            TPtrC value(contentLine.Mid(colonPos + 1));
            if (value.CompareF(*beginName) == 0)
                {
                // We have successfully skipped the component
                CleanupStack::PopAndDestroy(beginName);
                return;
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
        // If it is neither a begin nor an end, try the next line
        error = GetNextContentLine(contentLine);
        }
    // If any errors occur in reading we may as well leave with them
    if (error != KErrEof)
        {
        User::Leave(error);
        }
    // If we get this far we have failed to find an end
    User::Leave(KErrCorrupt);

    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineReader::CESMRICalContentLineReader
// ---------------------------------------------------------------------------
//
CESMRICalContentLineReader::CESMRICalContentLineReader(RReadStream& aStream)
    : iReadStream(aStream)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineReader::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRICalContentLineReader::ConstructL()
    {
    FUNC_LOG;

    iCurrentLine = HBufC::NewL(KICalMaxContentLineLength);
    iNextLine = HBufC::NewL(KICalMaxContentLineLength);

    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineReader::GetLineL
// ---------------------------------------------------------------------------
//
/**
Reads from the stream to the next line feed.
@param aPtr A modifiable pointer to a buffer to read the line into.
@leave With KErrEof when the end of the stream is reached or KErrCorrupt if the
line is invalid.
@internalTechnology
*/
void CESMRICalContentLineReader::GetLineL(TPtr& aPtr)
    {
    FUNC_LOG;

    // Read stream as binary. ReadL() will leave with KErrEof when the end of the stream
    // is reached. Note that we will ignore any text read when leaving as the line must end
    // with a carriage return (and line feed) to be valid.
    TBuf8<KICalMaxContentLineLength> text;
    iReadStream.ReadL(text, TChar(KICalCarriageReturnChar));    // Read up to the next CR.

    // Check next char is line feed
    TUint8 nextChar(iReadStream.ReadUint8L());

    if (nextChar != KICalNewlineChar)
        {
        User::Leave(KErrCorrupt);
        }

    // Handle any character set conversion here
    CnvUtfConverter::ConvertToUnicodeFromUtf8(aPtr, text);  // Convert from UTF-8.

    // Set the length of aPtr to exclude the carriage return.
    if (aPtr.Length())
        {
        aPtr.SetLength(aPtr.Length() - KICalSkipCRLength);
        }

    }

// ---------------------------------------------------------------------------
// CESMRICalContentLineReader::GetNextContentLineL
// ---------------------------------------------------------------------------
//
/**
Reads the next content line from the stream into aPtr. If the content line has
been folded, it will be unfolded. After completion aPtr will point to the new
content line
@param aPtr TPtrC& to set to the new content line.
@leave Leaves with KErrEof when we reach the end of the stream.
@internalTechnology
*/
void CESMRICalContentLineReader::GetNextContentLineL()
    {
    FUNC_LOG;

    TPtr currentLine(iCurrentLine->Des());

    if (iNextLine->Length() == 0)
        {
        // If iNextLine is empty then this is the first line of the file.
        GetLineL(currentLine);
        }
    else
        {
        // Copy iNextLine to currentLine.
        currentLine.Copy(*iNextLine);
        }

    // Get the next line.
    TPtr nextPtr(iNextLine->Des());
    GetLineL(nextPtr);

    // Folded lines are delimited by a CRLF followed by single whitepace
    while (iNextLine->Length() && ((nextPtr[KICalStartChar] == KICalSpaceChar) ||
        (nextPtr[KICalStartChar] == KICalHTabChar)))
        {
        // Append the lines - reallocate buffer if needed.
        if (currentLine.MaxLength() < (iCurrentLine->Length() + nextPtr.Length()))
            {
            iCurrentLine = iCurrentLine->ReAllocL(currentLine.MaxLength() + KICalMaxContentLineLength);
            currentLine.Set(iCurrentLine->Des());
            }

        currentLine.Append(nextPtr.Mid(KICalSkipWhiteSpaceLength));
        GetLineL(nextPtr);
        }

    }

// End of File

