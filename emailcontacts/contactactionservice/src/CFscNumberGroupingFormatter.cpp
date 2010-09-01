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
* Description:  Implementation of number grouping phone number formatter.
 *
*/


#include "emailtrace.h"
#include "CFscNumberGroupingFormatter.h"

// System includes
#include <NumberGrouping.h>
#include <AknUtils.h>

/// Unnamed namespace for local definitions
namespace
    {

    const TInt KGroupingFactor = 2;

    } /// namespace


// --------------------------------------------------------------------------
// CFscNumberGroupingFormatter::CFscNumberGroupingFormatter
// --------------------------------------------------------------------------
//
CFscNumberGroupingFormatter::CFscNumberGroupingFormatter()
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscNumberGroupingFormatter::~CFscNumberGroupingFormatter
// --------------------------------------------------------------------------
//
CFscNumberGroupingFormatter::~CFscNumberGroupingFormatter()
    {
    FUNC_LOG;
    delete iNumberGrouping;
    delete iFormatterNumberBuffer;
    }

// --------------------------------------------------------------------------
// CFscNumberGroupingFormatter::ConstructL
// --------------------------------------------------------------------------
//
void CFscNumberGroupingFormatter::ConstructL(TInt aMaxDisplayLength)
    {
    FUNC_LOG;
    // The grouping object takes care of calculating the size of the
    // grouped number, just pass the length of the non-grouped number
    iNumberGrouping = CPNGNumberGrouping::NewL(aMaxDisplayLength);

    iFormatterNumberBuffer = HBufC::NewL(KGroupingFactor * aMaxDisplayLength);

    }

// --------------------------------------------------------------------------
// CFscNumberGroupingFormatter::NewL
// --------------------------------------------------------------------------
//
CFscNumberGroupingFormatter* CFscNumberGroupingFormatter::NewL(
        TInt aMaxDisplayLength)
    {
    FUNC_LOG;
    CFscNumberGroupingFormatter* self = new( ELeave ) CFscNumberGroupingFormatter;
    CleanupStack::PushL(self);
    self->ConstructL(aMaxDisplayLength);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CFscNumberGroupingFormatter::SetMaxBufferLengthL
// --------------------------------------------------------------------------
//
void CFscNumberGroupingFormatter::SetMaxBufferLengthL(TInt aMaxLength)
    {
    FUNC_LOG;
    if (aMaxLength > iNumberGrouping->MaxDisplayLength() )
        {
        CPNGNumberGrouping* newNumberGrouping =
                CPNGNumberGrouping::NewL(aMaxLength);
        delete iNumberGrouping;
        iNumberGrouping = newNumberGrouping;
        iFormatterNumberBuffer
                = iFormatterNumberBuffer->ReAllocL(KGroupingFactor
                        * aMaxLength);
        }
    }

// --------------------------------------------------------------------------
// CPbkNumberGroupingFormatter::FormatPhoneNumberForDisplay
// --------------------------------------------------------------------------
//
TPtrC CFscNumberGroupingFormatter::FormatPhoneNumberForDisplay(
        const TDesC& aOriginalPhoneNumber)
    {
    FUNC_LOG;
    if (aOriginalPhoneNumber.Length() <= iNumberGrouping->MaxDisplayLength() )
        {
        iNumberGrouping->Set(aOriginalPhoneNumber);
        TPtr formatterNumber = iFormatterNumberBuffer->Des();
        formatterNumber.Copy(iNumberGrouping->FormattedNumber() );

        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formatterNumber);
        return formatterNumber;
        }
    else
        {
        // Too long number to format, just return the original. This is
        // in line with MPbkPhoneNumberFormatter interface's
        // best-effort promise.
        return aOriginalPhoneNumber;
        }
    }

// End of File

