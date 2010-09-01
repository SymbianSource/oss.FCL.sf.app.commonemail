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
* Description:  Implementation of presentation level contact field
 *
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "CFscPresentationContactField.h"
#include "CFscPresentationContactFieldTextData.h"

#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldBinaryData.h>
#include <MPbk2FieldProperty.h>

namespace
    {

    const TInt KStaticLength = 35;

    } // namespace

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CFscPresentationContactField::CFscPresentationContactField
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CFscPresentationContactField::CFscPresentationContactField(
        const MPbk2FieldProperty& aFieldProperty,
        MVPbkStoreContact& aParentContact) :
    iFieldProperty(aFieldProperty), iParentContact(aParentContact)
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::~CFscPresentationContactField
// --------------------------------------------------------------------------
//
CFscPresentationContactField::~CFscPresentationContactField()
    {
    FUNC_LOG;
    delete iData;
    delete iStoreField;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
 CFscPresentationContactField* CFscPresentationContactField::NewL
(const MPbk2FieldProperty& aFieldProperty,
        const MVPbkStoreContactField& aStoreField,
        MVPbkStoreContact& aParentContact)
    {
    FUNC_LOG;
    CFscPresentationContactField* self =
    NewLC(aFieldProperty, aStoreField, aParentContact);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
 CFscPresentationContactField* CFscPresentationContactField::NewLC
(const MPbk2FieldProperty& aFieldProperty,
        const MVPbkStoreContactField& aStoreField,
        MVPbkStoreContact& aParentContact)
    {
    FUNC_LOG;
    CFscPresentationContactField* self =
    new(ELeave) CFscPresentationContactField(aFieldProperty,
            aParentContact);
    CleanupStack::PushL(self);
    self->ConstructL(aStoreField);
    return self;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::ConstructL
// --------------------------------------------------------------------------
//
void CFscPresentationContactField::ConstructL(
        const MVPbkStoreContactField& aStoreField)
    {
    FUNC_LOG;
    // Make a permanent copy from the field reference
    iStoreField = aStoreField.CloneLC();
    CleanupStack::Pop(); // iStoreField

    if (FieldData().DataType() == EVPbkFieldStorageTypeText)
        {
        MVPbkContactFieldTextData& textData =
                MVPbkContactFieldTextData::Cast(iStoreField->FieldData() );
        iData = CFscPresentationContactFieldTextData::NewL(textData,
                iFieldProperty, MaxDataLength() );
        }
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::MaxDataLength
// --------------------------------------------------------------------------
//  
 TInt CFscPresentationContactField::MaxDataLength() const
    {
    FUNC_LOG;
    TInt maxLength = KVPbkUnlimitedFieldLength;
    TVPbkFieldStorageType dataType = iStoreField->FieldData().DataType();
    switch (dataType)
        {
        case EVPbkFieldStorageTypeText:
            {
            const MVPbkContactFieldTextData& textData =
            MVPbkContactFieldTextData::Cast(iStoreField->FieldData());
            maxLength = textData.MaxLength();
            break;
            }
        case EVPbkFieldStorageTypeBinary:
            {
            const MVPbkContactFieldBinaryData& binaryData =
            MVPbkContactFieldBinaryData::Cast(iStoreField->FieldData());
            maxLength = binaryData.MaxLength();
            break;
            }
        case EVPbkFieldStorageTypeDateTime: // FALLTHROUGH
        default:
            {
            // Do nothing
            break;
            }
        }

    TInt staticMaxLength = iFieldProperty.MaxLength();
    if (maxLength == KVPbkUnlimitedFieldLength)
        {
        maxLength = staticMaxLength;
        }
    else
        {
        maxLength = Min(staticMaxLength, maxLength);
        }
    return maxLength;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::IsEmpty
// --------------------------------------------------------------------------
//    
 TBool CFscPresentationContactField::IsEmpty() const
    {
    FUNC_LOG;
    return iStoreField->FieldData().IsEmpty();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::IsEditable
// --------------------------------------------------------------------------
//
 TBool CFscPresentationContactField::IsEditable() const
    {
    FUNC_LOG;
    return !(iFieldProperty.Flags() & KPbk2FieldFlagDisableEdit);
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::IsRemovable
// --------------------------------------------------------------------------
//    
 TBool CFscPresentationContactField::IsRemovable() const
    {
    FUNC_LOG;
    return !(iFieldProperty.Flags() & KPbk2FieldFlagCanNotBeRemoved);
    }
// --------------------------------------------------------------------------
// CFscPresentationContactField::ParentObject
// --------------------------------------------------------------------------
//
MVPbkObjectHierarchy& CFscPresentationContactField::ParentObject() const
    {
    FUNC_LOG;
    return iStoreField->ParentObject();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::ParentContact
// --------------------------------------------------------------------------
//
MVPbkBaseContact& CFscPresentationContactField::ParentContact() const
    {
    FUNC_LOG;
    return iStoreField->ParentContact();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::ParentContact
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CFscPresentationContactField::MatchFieldType(
        TInt aMatchPriority) const
    {
    FUNC_LOG;
    return iStoreField->MatchFieldType(aMatchPriority);
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::BestMatchingFieldType
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CFscPresentationContactField::BestMatchingFieldType() const
    {
    FUNC_LOG;
    return iStoreField->BestMatchingFieldType();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::FieldData
// --------------------------------------------------------------------------
//
const MVPbkContactFieldData& CFscPresentationContactField::FieldData() const
    {
    FUNC_LOG;

    TVPbkFieldStorageType dataType = iStoreField->FieldData().DataType();
    switch (dataType)
        {
        case EVPbkFieldStorageTypeText:
            {
            if (iData)
                {
                return *iData;
                }
            else
                {
                return iStoreField->FieldData();
                }
            }
        case EVPbkFieldStorageTypeBinary: // FALLTHROUGH
        case EVPbkFieldStorageTypeDateTime: // FALLTHROUGH
        default:
            {
            return iStoreField->FieldData();
            }
        }
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::IsSame
// --------------------------------------------------------------------------
//
TBool CFscPresentationContactField::IsSame(
        const MVPbkBaseContactField& aOther) const
    {
    FUNC_LOG;
    const CFscPresentationContactField* field =
            dynamic_cast<const CFscPresentationContactField*>(&aOther);
    if (field)
        {
        return iStoreField->IsSame(*field->iStoreField);
        }
    return iStoreField->IsSame(aOther);
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::SupportsLabel
// --------------------------------------------------------------------------
//
TBool CFscPresentationContactField::SupportsLabel() const
    {
    FUNC_LOG;
    return iStoreField->SupportsLabel();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::FieldLabel
// --------------------------------------------------------------------------
//
TPtrC CFscPresentationContactField::FieldLabel() const
    {
    FUNC_LOG;
    if (iStoreField->ContactStore().StoreProperties().SupportsFieldLabels())
        {
        return iStoreField->FieldLabel();
        }
    else
        {
        return iFieldProperty.DefaultLabel();
        }
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::SetFieldLabelL
// --------------------------------------------------------------------------
//
void CFscPresentationContactField::SetFieldLabelL(const TDesC& aText)
    {
    FUNC_LOG;
    iStoreField->SetFieldLabelL(aText);
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::MaxLabelLength
// --------------------------------------------------------------------------
//
TInt CFscPresentationContactField::MaxLabelLength() const
    {
    FUNC_LOG;
    TInt storeLength = iStoreField->MaxLabelLength();
    if (storeLength == KVPbkUnlimitedLabelLength)
        {
        return KStaticLength;
        }
    else
        {
        return Min(KStaticLength, storeLength);
        }
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::FieldData
// --------------------------------------------------------------------------
//
MVPbkContactFieldData& CFscPresentationContactField::FieldData()
    {
    FUNC_LOG;
    TVPbkFieldStorageType dataType = iStoreField->FieldData().DataType();
    switch (dataType)
        {
        case EVPbkFieldStorageTypeText:
            {
            if (iData)
                {
                return *iData;
                }
            else
                {
                return iStoreField->FieldData();
                }
            }
        case EVPbkFieldStorageTypeBinary: // FALLTHROUGH
        case EVPbkFieldStorageTypeDateTime: // FALLTHROUGH
        default:
            {
            return iStoreField->FieldData();
            }
        }
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::CloneLC
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CFscPresentationContactField::CloneLC() const
    {
    FUNC_LOG;
    CFscPresentationContactField* copy =
            CFscPresentationContactField::NewLC(iFieldProperty,
                    *iStoreField, iParentContact);
    return copy;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactField::CreateLinkLC
// --------------------------------------------------------------------------
//  
MVPbkContactLink* CFscPresentationContactField::CreateLinkLC() const
    {
    FUNC_LOG;
    return iStoreField->CreateLinkLC();
    }

//  End of File  

