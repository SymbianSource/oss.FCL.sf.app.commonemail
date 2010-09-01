/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of presentation utilities.
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "FscPresentationUtils.h"

// From system
#include <featmgr.h>

/// Namespace for local PresentationUtils definitions
namespace __RVCT_UNS_FscPresentationUtils {

// LOCAL CONSTANTS
enum TPanicCode
    {
    EPanicPreCond_CopyAndReplaceChars = 1
    };

// ================= LOCAL FUNCTIONS =======================
#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "FscPresentationUtils");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

// Zero Width characters
const TUint KZWSCharacter = 0x200B;
const TUint KZWNJCharacter = 0x200C;
const TUint KZWJCharacter = 0x200D;

// ================= LOCAL FUNCTIONS =======================

/** 
 * Check if the given char is a zero width character: 
 * Zero Width Space, Zero Width Non-Joiner
 * or Zero Width Joiner character
 * @param aChar Given character
 * @return ETrue if aChar is zero width character.
 *         EFalse if aChar is not specified zero width character.
 */
inline TBool ZWSCharacter( const TChar aChar )
    {
    const TChar zwsChar( KZWSCharacter );
    const TChar zwnjChar( KZWNJCharacter );
    const TChar zwjChar( KZWJCharacter );
    return ( aChar == zwsChar ) || ( aChar == zwnjChar ) ||
           ( aChar == zwjChar );
    }
      
} // __RVCT_UNS_FscPresentationUtils

using namespace __RVCT_UNS_FscPresentationUtils;


// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// FscPresentationUtils::TrimAllAppend
// --------------------------------------------------------------------------
//
TInt FscPresentationUtils::TrimAllAppend(const TDesC& aText, TDes& aDest)
    {
    FUNC_LOG;
    const TInt oldDestLength = aDest.Length();

    // 1. Find first non space character
    const TInt textLength = aText.Length();
    TInt firstNonSpaceChar = 0;
    while(firstNonSpaceChar < textLength
        && TChar(aText[firstNonSpaceChar]).IsSpace())
        {
        ++firstNonSpaceChar;
        }

    // 2. Append the source text to destination,
    // not including the leading spaces
    aDest.Append(aText.Mid(firstNonSpaceChar));
    // 3. Trim the new part of the destination to remove
    // trailing and middle spaces
    TPtr ptr = aDest.MidTPtr(oldDestLength);
    ptr.TrimAll();
    aDest.SetLength(oldDestLength);
    aDest.Append(ptr);

    return aDest.Length() - oldDestLength;
    }

// --------------------------------------------------------------------------
// FscPresentationUtils::TrimRightAppend
// --------------------------------------------------------------------------
//
 TInt FscPresentationUtils::TrimRightAppend(
        const TDesC& aText, TDes& aDest)
    {
    FUNC_LOG;
    const TInt oldDestLength = aDest.Length();

    // 1. Find first non space character
    const TInt textLength = aText.Length();
    TInt firstNonSpaceChar = 0;
    while(firstNonSpaceChar < textLength
          && TChar(aText[firstNonSpaceChar]).IsSpace())
        {
        ++firstNonSpaceChar;
        }

    // 2. Append the source text to destination,
    // not including the leading spaces
    aDest.Append(aText.Mid(firstNonSpaceChar));
    // 3. Trim the new part of the destination to remove
    // trailing and middle spaces
    TPtr ptr = aDest.MidTPtr(oldDestLength);
    ptr.TrimAll();
    aDest.SetLength(oldDestLength);
    aDest.Append(ptr);
    // 4. Insert the leading spaces to destination
    aDest.Insert(oldDestLength, aText.Left(firstNonSpaceChar));

    return aDest.Length() - oldDestLength;
    }
    
// --------------------------------------------------------------------------
// FscPresentationUtils::AppendAndReplaceChars
// --------------------------------------------------------------------------
//
 TInt FscPresentationUtils::AppendAndReplaceChars
        (TDes& aDest, const TDesC& aSrc,
        const TDesC& aCharsToReplace, const TDesC& aReplaceChars)
    {
    FUNC_LOG;
    // Check that aDest has enough capacity
    __ASSERT_DEBUG((aDest.MaxLength()-aDest.Length()) >= aSrc.Length(),
        Panic(EPanicPreCond_CopyAndReplaceChars));
    // Check that a replacament is provided for all characters
    __ASSERT_DEBUG(aCharsToReplace.Length() == aReplaceChars.Length(),
        Panic(EPanicPreCond_CopyAndReplaceChars));

    TInt count, i;
    const TInt sourceLenght = aSrc.Length();
    for (count=0, i=0; i < sourceLenght; ++i)
        {
        TText ch = aSrc[i];
        const TInt replacePos = aCharsToReplace.Locate(aSrc[i]);
        if (replacePos != KErrNotFound)
            {
            ++count;
            ch = aReplaceChars[replacePos];
            }
        aDest.Append(ch);
        }

    return count;
    }
    
// --------------------------------------------------------------------------
// FscPresentationUtils::ReplaceNonGraphicCharacters
// --------------------------------------------------------------------------
//
 void FscPresentationUtils::ReplaceNonGraphicCharacters
        (TDes& aText, TText aChar)
    {
    FUNC_LOG;
    const TInt len = aText.Length();
    for ( TInt i=0; i < len; ++i )
        {
        if ( !TChar(aText[i]).IsGraph() &&
             !ZWSCharacter( aText[i] ) )
            {
            // If non-graphic char is specified in ZWSCharacter,
            // it will not be replaced. Otherwise replace non-graphic
            // character with aChar.
                aText[i] = aChar;
            }
        }
    }

// --------------------------------------------------------------------------
// FscPresentationUtils::AppendWithNewlineTranslationL
// --------------------------------------------------------------------------
//
 void FscPresentationUtils::AppendWithNewlineTranslationL
        ( TDes& aBuffer, const TDesC& aText )
    {
    FUNC_LOG;
    const TUint16 KCarriageReturn = 0x000d;
    const TUint16 KLineFeed = 0x000a;
    const TUint16 KParagraphSeparator = 0x2029;
    
    const TInt length( aText.Length() );
    const TInt max( aBuffer.MaxLength() );
    
    for ( register TInt i = 0;
          (i < length) && (aBuffer.Length() < max); ++i )
        {
        register TUint16 ch = aText[i];
        if ( ch == KCarriageReturn )
            {
            ch = KParagraphSeparator;
            // Skip linefeeds that follow carriage return
            if ( (i < length-1) && (aText[i+1] == KLineFeed) )
                {
                ++i;
                }
            }
        else if ( ch == KLineFeed )
            {
            ch = KParagraphSeparator;
            }
        aBuffer.Append( ch );
        }
    }
// End of File

