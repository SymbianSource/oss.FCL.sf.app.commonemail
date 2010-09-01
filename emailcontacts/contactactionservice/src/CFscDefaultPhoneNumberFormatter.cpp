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
* Description:  Implementation of default phone number formatter.
*
*/


#include "emailtrace.h"
#include "CFscDefaultPhoneNumberFormatter.h"

// System includes
#include <AknUtils.h>

// --------------------------------------------------------------------------
// CFscDefaultPhoneNumberFormatter::CFscDefaultPhoneNumberFormatter
// --------------------------------------------------------------------------
//
CFscDefaultPhoneNumberFormatter::CFscDefaultPhoneNumberFormatter()
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscDefaultPhoneNumberFormatter::~CFscDefaultPhoneNumberFormatter
// --------------------------------------------------------------------------
//
CFscDefaultPhoneNumberFormatter::~CFscDefaultPhoneNumberFormatter()
    {
    FUNC_LOG;
    delete iFormatterNumberBuffer;
    }

// --------------------------------------------------------------------------
// CFscDefaultPhoneNumberFormatter::ConstructL
// --------------------------------------------------------------------------
//
void CFscDefaultPhoneNumberFormatter::ConstructL
        ( TInt aMaxDisplayLength )
    {
    FUNC_LOG;
    iFormatterNumberBuffer = HBufC::NewL( aMaxDisplayLength );
    }

// --------------------------------------------------------------------------
// CFscDefaultPhoneNumberFormatter::NewL
// --------------------------------------------------------------------------
//
CFscDefaultPhoneNumberFormatter* CFscDefaultPhoneNumberFormatter::NewL
        ( TInt aMaxDisplayLength )
    {
    FUNC_LOG;
    CFscDefaultPhoneNumberFormatter* self =
        new( ELeave ) CFscDefaultPhoneNumberFormatter;
    CleanupStack::PushL( self );
    self->ConstructL( aMaxDisplayLength );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CFscDefaultPhoneNumberFormatter::SetMaxBufferLengthL
// --------------------------------------------------------------------------
//
void CFscDefaultPhoneNumberFormatter::SetMaxBufferLengthL( TInt aMaxLength )
    {
    FUNC_LOG;
    if ( aMaxLength > iFormatterNumberBuffer->Des().MaxLength() )
        {
        iFormatterNumberBuffer =
            iFormatterNumberBuffer->ReAllocL( aMaxLength );
        }
    }

// --------------------------------------------------------------------------
// CFscDefaultPhoneNumberFormatter::FormatPhoneNumberForDisplay
// --------------------------------------------------------------------------
//
TPtrC CFscDefaultPhoneNumberFormatter::FormatPhoneNumberForDisplay
        ( const TDesC& aOriginalPhoneNumber )
    {
    FUNC_LOG;
    // Convert number according to active number setting
    if ( aOriginalPhoneNumber.Length() <=
         iFormatterNumberBuffer->Des().MaxLength() )
        {
        TPtr formatterNumber = iFormatterNumberBuffer->Des();
        formatterNumber.Copy( aOriginalPhoneNumber );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion
            ( formatterNumber );
        return formatterNumber;
        }
    else
        {
        return aOriginalPhoneNumber;
        }
    }

// End of File

