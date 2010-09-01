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
* Description:  Implementation of presentation text data of the contact field.
 *
*/


// INCLUDES
#include "emailtrace.h"
#include "CFscPresentationContactFieldTextData.h"

// Phonebook2
#include <Pbk2FieldProperty.hrh>
#include "CFscPresentationContactField.h"

// Virtual Phonebook
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkFieldType.h>
#include <MPbk2FieldProperty.h>

/// Unnamed namespace for local definitions
namespace
    {

#ifdef _DEBUG

    enum TPanicCode
        {
        EPanicFieldDataTypeMismatch = 1,
        EPanicIncorrectCtrlType
        };

    void Panic(TPanicCode aReason)
        {
        _LIT( KPanicText, "CFscPresentationContactFieldTextData" );
        User::Panic(KPanicText, aReason);
        }

#endif // _DEBUG
    } /// namespace

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::
//  CFscPresentationContactFieldTextData
// --------------------------------------------------------------------------
//
CFscPresentationContactFieldTextData::CFscPresentationContactFieldTextData(
        MVPbkContactFieldTextData& aTextData,
        const MPbk2FieldProperty& aFieldProperty, const TInt aMaxDataLength) :
    iTextData(aTextData), iFieldProperty(aFieldProperty),
            iMaxDataLength(aMaxDataLength)
    {
    FUNC_LOG;
    // Do nothing    
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::NewL
// --------------------------------------------------------------------------
//    
CFscPresentationContactFieldTextData* CFscPresentationContactFieldTextData::NewL(
        MVPbkContactFieldTextData& aTextData,
        const MPbk2FieldProperty& aFieldProperty, const TInt aMaxDataLength)
    {
    FUNC_LOG;
    CFscPresentationContactFieldTextData* self = new(ELeave)CFscPresentationContactFieldTextData(
            aTextData, aFieldProperty, aMaxDataLength );
    return self;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::
//    ~CFscPresentationContactFieldTextData
// --------------------------------------------------------------------------
//            
CFscPresentationContactFieldTextData::~CFscPresentationContactFieldTextData()
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::Cast
// --------------------------------------------------------------------------
//
CFscPresentationContactFieldTextData& CFscPresentationContactFieldTextData::Cast(
        MVPbkContactFieldData& aFieldData)
    {
    FUNC_LOG;
    __ASSERT_DEBUG( aFieldData.DataType() == EVPbkFieldStorageTypeText,
            Panic(EPanicFieldDataTypeMismatch) );
    return static_cast<CFscPresentationContactFieldTextData&>(aFieldData);
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::Cast
// --------------------------------------------------------------------------
//
const CFscPresentationContactFieldTextData& CFscPresentationContactFieldTextData::Cast(
        const MVPbkContactFieldData& aFieldData)
    {
    FUNC_LOG;
    __ASSERT_DEBUG( aFieldData.DataType() == EVPbkFieldStorageTypeText,
            Panic(EPanicFieldDataTypeMismatch) );
    return static_cast<const CFscPresentationContactFieldTextData&>(aFieldData );
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::Text
// --------------------------------------------------------------------------
//    
TPtrC CFscPresentationContactFieldTextData::Text() const
    {
    FUNC_LOG;
    TPtrC ptr = iTextData.Text();
    switch (iFieldProperty.CtrlType() )
        {
        case EPbk2FieldCtrlTypeNumberEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypePhoneNumberEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypeDTMFEditor:
            {
            return ptr.Right(iMaxDataLength);
            }
        case EPbk2FieldCtrlTypeTextEditor: // FALLTHROUGH
        case EPbk2FieldCtrlURLEditor: // FALLTHROUGH
        case EPbk2FieldCtrlEmailEditor: // FALLTHROUGH
        case EPbk2FieldCtrlPostalCodeEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypeReadingEditor:
        case EPbk2FieldCtrlTypeChoiceItems: // FALLTHROUGH
            {
            return ptr.Left(iMaxDataLength);
            }
        case EPbk2FieldCtrlTypeNone:
            {
            return ptr;
            }
        case EPbk2FieldCtrlTypeDateEditor: // FALLTHROUGH
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EPanicIncorrectCtrlType ) );
            }
        };
    return ptr;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::SetTextL
// --------------------------------------------------------------------------
//
void CFscPresentationContactFieldTextData::SetTextL(const TDesC& aText)
    {
    FUNC_LOG;
    TPtrC ptr = iTextData.Text();
    switch (iFieldProperty.CtrlType() )
        {
        case EPbk2FieldCtrlTypeNumberEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypePhoneNumberEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypeDTMFEditor:
            {
            iTextData.SetTextL(aText.Right(iMaxDataLength) );
            break;
            }
        case EPbk2FieldCtrlTypeTextEditor: // FALLTHROUGH
        case EPbk2FieldCtrlURLEditor: // FALLTHROUGH
        case EPbk2FieldCtrlEmailEditor: // FALLTHROUGH
        case EPbk2FieldCtrlPostalCodeEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypeReadingEditor:
        case EPbk2FieldCtrlTypeChoiceItems: // FALLTHROUGH
            {
            iTextData.SetTextL(aText.Left(iMaxDataLength) );
            break;
            }
        case EPbk2FieldCtrlTypeNone:
            {
            iTextData.SetTextL(aText);
            break;
            }
        case EPbk2FieldCtrlTypeDateEditor: // FALLTHROUGH
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EPanicIncorrectCtrlType ) );
            }
        };
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::MaxLength
// --------------------------------------------------------------------------
//
TInt CFscPresentationContactFieldTextData::MaxLength() const
    {
    FUNC_LOG;
    return iTextData.MaxLength();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::DataType
// --------------------------------------------------------------------------
//    
TVPbkFieldStorageType CFscPresentationContactFieldTextData::DataType() const
    {
    FUNC_LOG;
    return iTextData.DataType();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::IsEmpty
// --------------------------------------------------------------------------
//    
TBool CFscPresentationContactFieldTextData::IsEmpty() const
    {
    FUNC_LOG;
    return iTextData.IsEmpty();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldTextData::CopyL
// --------------------------------------------------------------------------
//    
void CFscPresentationContactFieldTextData::CopyL(
        const MVPbkContactFieldData& aFieldData)
    {
    FUNC_LOG;
    iTextData.CopyL(aFieldData);
    }

// End of file

