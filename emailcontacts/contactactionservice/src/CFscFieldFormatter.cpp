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
* Description:  Conacts field formatter.
 *
*/


#include "emailtrace.h"
#include "CFscFieldFormatter.h"

// Phonebook 2
#include "FscPhoneNumberFormatterFactory.h"
#include "MFscPhoneNumberFormatter.h"
#include "FscPresentationUtils.h"
#include <FscActionUtils.rsg>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkContactFieldData.h>
#include <MPbk2FieldProperty.h>
#include <MPbk2FieldPropertyArray.h>
#include <VPbkFieldType.hrh>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <CVPbkFieldTypeSelector.h>
#include <VPbkSyncConstants.h>

// System includes
#include <AknUtils.h>
#include <barsread.h>

/// Unnamed namespace for local definitions
namespace
    {

    const TInt KBufferLength = 128;

#ifdef _DEBUG

    enum TPanicCode
        {
        EPanicPostCond_ReallocBufferL = 1
        };

    void Panic(TPanicCode aReason)
        {
        _LIT(KPanicText, "CFscFieldFormatter");
        User::Panic(KPanicText, aReason);
        }

#endif // _DEBUG
    } /// namespace


// --------------------------------------------------------------------------
// CFscFieldFormatter::CFscFieldFormatter
// --------------------------------------------------------------------------
//
inline CFscFieldFormatter::CFscFieldFormatter(
        const MPbk2FieldPropertyArray& aFieldProperties,
        const MVPbkFieldTypeList& aFieldTypeList) :
    iFieldProperties(aFieldProperties), iFieldTypeList(aFieldTypeList)
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::~CFscFieldFormatter
// --------------------------------------------------------------------------
//
CFscFieldFormatter::~CFscFieldFormatter()
    {
    FUNC_LOG;
    delete iTimeFormat;
    delete iNumberFormatter;
    delete iBuffer;
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::ConstructL
// --------------------------------------------------------------------------
//
inline void CFscFieldFormatter::ConstructL()
    {
    FUNC_LOG;
    iBuffer = HBufC::NewL(KBufferLength);
    iNumberFormatter
            = FscPhoneNumberFormatterFactory::CreatePhoneNumberFormatterL(KBufferLength);
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::NewL
// --------------------------------------------------------------------------
//
 CFscFieldFormatter* CFscFieldFormatter::NewL
( const MPbk2FieldPropertyArray& aFieldProperties,
        const MVPbkFieldTypeList& aFieldTypeList )
    {
    FUNC_LOG;
    CFscFieldFormatter* self =
    new ( ELeave ) CFscFieldFormatter
    ( aFieldProperties, aFieldTypeList );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::FormatFieldContentL
// --------------------------------------------------------------------------
//
 TPtrC CFscFieldFormatter::FormatFieldContentL
( const MVPbkBaseContactField& aField,
        const MVPbkFieldType& aFieldType )
    {
    FUNC_LOG;
    TPtr text( iBuffer->Des() );

    switch ( aField.FieldData().DataType() )
        {
        case EVPbkFieldStorageTypeText:
            {
            const MVPbkContactFieldTextData* fieldData =
            &MVPbkContactFieldTextData::Cast( aField.FieldData() );
            TPtrC fieldText( fieldData->Text() );

            if ( MatchesFieldTypeL
                    ( aFieldType, R_FSC_PHONENUMBER_SELECTOR ) )
                {
                FormatPhoneNumberContentL( fieldText, text );
                }
            else
                {
                // Default formatting
                FormatTextContentL( fieldText, text, aFieldType );
                }
            break;
            }

        default:
            {
            text.Zero();
            break;
            }
        }

    return text;
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::SetTimeFormatL
// --------------------------------------------------------------------------
//
 void CFscFieldFormatter::SetTimeFormatL(const TDesC& aTimeFormat)
    {
    FUNC_LOG;
    HBufC* timeFormat = aTimeFormat.AllocL();
    delete iTimeFormat;
    iTimeFormat = timeFormat;
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::ReallocBufferL
// --------------------------------------------------------------------------
//
TPtr CFscFieldFormatter::ReallocBufferL(TInt aNewSize)
    {
    FUNC_LOG;
    const TInt currMaxLength = iBuffer->Des().MaxLength();
    if (aNewSize > currMaxLength)
        {
        iBuffer = iBuffer->ReAllocL(aNewSize);
        }

    // PostCond:
    __ASSERT_DEBUG(iBuffer->Des().MaxLength() >= aNewSize,
            Panic(EPanicPostCond_ReallocBufferL));

    return iBuffer->Des();
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::MatchesFieldTypeL
// --------------------------------------------------------------------------
//
TBool CFscFieldFormatter::MatchesFieldTypeL(const MVPbkFieldType& aFieldType,
        TInt aResourceId) const
    {
    FUNC_LOG;
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(reader, aResourceId);

    CVPbkFieldTypeSelector* selector = CVPbkFieldTypeSelector::NewL(reader,
            iFieldTypeList);

    // Check if the field type is the one needed
    TBool ret = selector->IsFieldTypeIncluded(aFieldType);
    CleanupStack::PopAndDestroy(); // resource buffer
    delete selector;
    return ret;
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::FormatPhoneNumberContentL
// --------------------------------------------------------------------------
//
void CFscFieldFormatter::FormatPhoneNumberContentL(TPtrC& aRawContent,
        TPtr& aFormattedContent)
    {
    FUNC_LOG;
    iNumberFormatter->SetMaxBufferLengthL(aRawContent.Length() );

    TPtrC
            formattedPhoneNumber(iNumberFormatter->FormatPhoneNumberForDisplay(aRawContent) );
    aFormattedContent.Set(ReallocBufferL(formattedPhoneNumber.Length() ) );
    aFormattedContent.Copy(formattedPhoneNumber);
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::FormatDtmfContentL
// --------------------------------------------------------------------------
//
void CFscFieldFormatter::FormatDtmfContentL(TPtrC& aRawContent,
        TPtr& aFormattedContent)
    {
    FUNC_LOG;
    HBufC* numberBuffer = HBufC::NewLC(aRawContent.Length() );

    TPtr number = numberBuffer->Des();
    number.Copy(aRawContent);
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(number);

    aFormattedContent.Set(ReallocBufferL(number.Length() ) );
    aFormattedContent.Copy(number);

    CleanupStack::PopAndDestroy(numberBuffer);
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::FormatFileNameContentL
// --------------------------------------------------------------------------
//
void CFscFieldFormatter::FormatFileNameContentL(TPtrC& aRawContent,
        TPtr& aFormattedContent)
    {
    FUNC_LOG;
    aFormattedContent.Set(ReallocBufferL(KMaxFileName) );
    aFormattedContent.Copy(aRawContent);
    TParsePtr fileNameParser(aFormattedContent);
    TPtrC baseName(fileNameParser.Name() );
    aFormattedContent.Copy(baseName);
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::FormatTextContentL
// --------------------------------------------------------------------------
//
void CFscFieldFormatter::FormatTextContentL(TPtrC& aRawContent,
        TPtr& aFormattedContent, const MVPbkFieldType& aFieldType)
    {
    FUNC_LOG;
    aFormattedContent.Set(ReallocBufferL(aRawContent.Length() ) );
    aFormattedContent.Zero();
    FscPresentationUtils::AppendWithNewlineTranslationL(aFormattedContent,
            aRawContent);

    // Check is there need to display the digits in the text with
    // foreign characters. If the field is of numeric type,
    // language specific conversion is needed.
    const MPbk2FieldProperty* property =
            iFieldProperties.FindProperty(aFieldType);

    if (property && property->EditMode() == EPbk2FieldEditModeNumeric)
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(aFormattedContent);
        }
    }

// --------------------------------------------------------------------------
// CFscFieldFormatter::FormatDateTimeContentL
// --------------------------------------------------------------------------
//
void CFscFieldFormatter::FormatDateTimeContentL(
        const MVPbkContactFieldDateTimeData& aRawContent,
        TPtr& aFormattedContent)
    {
    FUNC_LOG;
    TInt error = KErrNone;
    do
        {
        TRAPD( error, aRawContent.DateTime().FormatL
                ( aFormattedContent, *iTimeFormat ) )
        ;
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(aFormattedContent);

        if (error == KErrOverflow)
            {
            // Allocate bigger buffer for formatting text
            aFormattedContent.Set(ReallocBufferL( 2
                    * aFormattedContent.MaxLength() ) );
            }
        else
            {
            // Rethrow other errors
            User::LeaveIfError(error);
            }
        }
    while (error == KErrOverflow);
    }

// End of File

