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
* Description:  Implementation of field property.
 *
*/


#include "emailtrace.h"
#include "CFscFieldProperty.h"

// From Phonebook 2
#include "CFscFieldOrderingManager.h"

// From Virtual Phonebook
#include <MVPbkFieldType.h>

// System includes
#include <barsread.h>
#include <coemain.h>

// --------------------------------------------------------------------------
// CFscFieldProperty::CFscFieldProperty
// --------------------------------------------------------------------------
//    
inline CFscFieldProperty::CFscFieldProperty()
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::~CFscFieldProperty
// --------------------------------------------------------------------------
//    
CFscFieldProperty::~CFscFieldProperty()
    {
    FUNC_LOG;
    delete iAddItemText;
    delete iDefaultLabel;
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::ConstructL
// --------------------------------------------------------------------------
//    
inline void CFscFieldProperty::ConstructL(TResourceReader& aReader,
        const MVPbkFieldTypeList& aSupportedFieldTypeList,
        CFscFieldOrderingManager& aFieldOrderingManager)
    {
    FUNC_LOG;
    aReader.ReadInt8(); // read version number
    const TInt fieldTypeResId = aReader.ReadInt32();

    iFieldType = aSupportedFieldTypeList.Find(fieldTypeResId);
    iMultiplicity = aReader.ReadInt8();
    iMaxLength = aReader.ReadInt16();
    iEditMode = aReader.ReadInt8();
    iDefaultCase = aReader.ReadInt8();
    iIconId = TPbk2IconId(aReader);
    iCtrlType = aReader.ReadInt8();
    iFlags = aReader.ReadUint32();
    iAddItemText = aReader.ReadHBufCL();
    iLocation = aReader.ReadInt8();
    iGroupId = aReader.ReadInt8();
    iDefaultLabel = aReader.ReadHBufCL();

    if (iFieldType)
        {
        // If field type is supported get ordering items
        iOrderingItem = aFieldOrderingManager.OrderingItem( *iFieldType);
        iAddItemOrdering
                = aFieldOrderingManager.AddItemOrdering( *iFieldType);
        }
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::NewLC
// --------------------------------------------------------------------------
//    
CFscFieldProperty* CFscFieldProperty::NewLC(TResourceReader& aReader,
        const MVPbkFieldTypeList& aSupportedFieldTypeList,
        CFscFieldOrderingManager& aFieldOrderingManager)
    {
    FUNC_LOG;
    CFscFieldProperty* self = new( ELeave ) CFscFieldProperty;
    CleanupStack::PushL(self);
    self->ConstructL(aReader, aSupportedFieldTypeList, aFieldOrderingManager);
    return self;
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::IsSupported
// --------------------------------------------------------------------------
//    
TBool CFscFieldProperty::IsSupported() const
    {
    FUNC_LOG;
    TBool ret = EFalse;

    if (iFieldType)
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::IsSame
// --------------------------------------------------------------------------
//
TBool CFscFieldProperty::IsSame(const MPbk2FieldProperty& aOther) const
    {
    FUNC_LOG;
    return aOther.FieldType().IsSame( *iFieldType);
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::FieldType
// --------------------------------------------------------------------------
//
const MVPbkFieldType& CFscFieldProperty::FieldType() const
    {
    FUNC_LOG;
    return *iFieldType;
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::Multiplicity
// --------------------------------------------------------------------------
//    
TPbk2FieldMultiplicity CFscFieldProperty::Multiplicity() const
    {
    FUNC_LOG;
    return static_cast<TPbk2FieldMultiplicity>(iMultiplicity);
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::MaxLength
// --------------------------------------------------------------------------
//
TInt CFscFieldProperty::MaxLength() const
    {
    FUNC_LOG;
    return iMaxLength;
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::EditMode
// --------------------------------------------------------------------------
//
TPbk2FieldEditMode CFscFieldProperty::EditMode() const
    {
    FUNC_LOG;
    return static_cast<TPbk2FieldEditMode>(iEditMode);
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::DefaultCase
// --------------------------------------------------------------------------
//
TPbk2FieldDefaultCase CFscFieldProperty::DefaultCase() const
    {
    FUNC_LOG;
    return static_cast<TPbk2FieldDefaultCase>(iDefaultCase);
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::IconId
// --------------------------------------------------------------------------
//
const TPbk2IconId& CFscFieldProperty::IconId() const
    {
    FUNC_LOG;
    return iIconId;
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::CtrlType
// --------------------------------------------------------------------------
//
TPbk2FieldCtrlType CFscFieldProperty::CtrlType() const
    {
    FUNC_LOG;
    return static_cast<TPbk2FieldCtrlType>(iCtrlType);
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::Flags
// --------------------------------------------------------------------------
//
TUint CFscFieldProperty::Flags() const
    {
    FUNC_LOG;
    return iFlags;
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::OrderingItem
// --------------------------------------------------------------------------
//
TInt CFscFieldProperty::OrderingItem() const
    {
    FUNC_LOG;
    return iOrderingItem;
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::AddItemOrdering
// --------------------------------------------------------------------------
//
TInt CFscFieldProperty::AddItemOrdering() const
    {
    FUNC_LOG;
    return iAddItemOrdering;
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::AddItemText
// --------------------------------------------------------------------------
//
const TDesC& CFscFieldProperty::AddItemText() const
    {
    FUNC_LOG;
    if (iAddItemText)
        {
        return *iAddItemText;
        }
    else
        if (iDefaultLabel)
            {
            return *iDefaultLabel;
            }
        else
            {
            return KNullDesC;
            }
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::Location
// --------------------------------------------------------------------------
//
TPbk2FieldLocation CFscFieldProperty::Location() const
    {
    FUNC_LOG;
    return static_cast<TPbk2FieldLocation>(iLocation);
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::GroupId
// --------------------------------------------------------------------------
//
TPbk2FieldGroupId CFscFieldProperty::GroupId() const
    {
    FUNC_LOG;
    return static_cast<TPbk2FieldGroupId>(iGroupId);
    }

// --------------------------------------------------------------------------
// CFscFieldProperty::DefaultLabel
// --------------------------------------------------------------------------
//
const TDesC& CFscFieldProperty::DefaultLabel() const
    {
    FUNC_LOG;
    return *iDefaultLabel;
    }

// End of file

