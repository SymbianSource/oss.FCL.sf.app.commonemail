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
* Description:  Implementation of sorted field collection for fields
 *
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "CFscPresentationContactFieldCollection.h"

// From Phonebook2
#include <MPbk2FieldProperty.h>
#include "CFscFieldPropertyGroup.h"
#include <MPbk2FieldPropertyArray.h>
#include "CFscPresentationContactField.h"

// From Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkFieldType.h>

// ============================= LOCAL FUNCTIONS ============================

namespace
    {

#ifdef _DEBUG
    enum TPanicCode
        {
        EGetStoreContactIndex_PreCond,
        EGetStoreContactIndex_PostCond,
        EPanic_FieldAtLC_OOB
        };

    void Panic(TInt aReason)
        {
        _LIT(KPanicText, "CFscPresentationContactFieldCollection");
        User::Panic(KPanicText, aReason);
        }
#endif // _DEBUG
    // --------------------------------------------------------------------------
    // CompareFields
    // Returns:
    // 1) zero, if the two objects are equal.
    // 2) a negative value, if the first object is less than the second.
    // 3) a positive value, if the first object is greater than the second.
    // --------------------------------------------------------------------------
    //
    TInt CompareFields(const CFscPresentationContactField& aFirst,
            const CFscPresentationContactField& aSecond)
        {
        return aFirst.FieldProperty().OrderingItem() - aSecond.FieldProperty().OrderingItem();
        }

    // --------------------------------------------------------------------------
    // SupportedFieldTypes
    // Returns the supported field type list of the parent contact
    // of the collection
    // --------------------------------------------------------------------------
    //
    inline const MVPbkFieldTypeList& SupportedFieldTypes(
            MVPbkStoreContactFieldCollection& aFieldCollection)
        {
        return aFieldCollection.ParentStoreContact().
        ContactStore().StoreProperties().SupportedFields();
        }

    // --------------------------------------------------------------------------
    // MatchProperty
    // --------------------------------------------------------------------------
    //
    inline const MPbk2FieldProperty* MatchProperty(
            const MPbk2FieldPropertyArray& aFieldProperties,
            const MVPbkStoreContactField& aField, TInt aMaxMatchPriority)
        {
        for (TInt i = 0; i < aMaxMatchPriority; ++i)
            {
            const MVPbkFieldType* type = aField.MatchFieldType(i);
            if (type)
                {
                return aFieldProperties.FindProperty(*type);
                }
            }
        return NULL;
        }

    }

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::
//  CFscPresentationContactFieldCollection
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
inline CFscPresentationContactFieldCollection::CFscPresentationContactFieldCollection(
        const MPbk2FieldPropertyArray& aFieldProperties,
        MVPbkStoreContactFieldCollection& aFieldCollection,
        MVPbkStoreContact& aParentContact) :
    iFieldProperties(aFieldProperties), iFieldCollection(aFieldCollection),
            iParentContact(aParentContact)
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
inline void CFscPresentationContactFieldCollection::ConstructL()
    {
    FUNC_LOG;
    const TInt count = iFieldCollection.FieldCount();
    for (TInt i = 0; i < count; ++i)
        {
        const MVPbkStoreContactField& field = iFieldCollection.FieldAt(i);
        AddFieldMappingL(field);
        }
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
 CFscPresentationContactFieldCollection*
CFscPresentationContactFieldCollection::NewL(
        const MPbk2FieldPropertyArray& aFieldProperties,
        MVPbkStoreContactFieldCollection& aFieldCollection,
        MVPbkStoreContact& aParentContact)
    {
    CFscPresentationContactFieldCollection* self =
    new( ELeave ) CFscPresentationContactFieldCollection(aFieldProperties,
            aFieldCollection, aParentContact);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CFscPresentationContactFieldCollection::~CFscPresentationContactFieldCollection()
    {
    FUNC_LOG;
    iFields.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::At
// --------------------------------------------------------------------------
//
 CFscPresentationContactField&
CFscPresentationContactFieldCollection::At(TInt aIndex) const
    {
    return *iFields[aIndex];
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::ContainsField
// --------------------------------------------------------------------------
//
 TInt CFscPresentationContactFieldCollection::ContainsField(
        const MPbk2FieldProperty& aProperty) const
    {
    FUNC_LOG;
    const TInt count = iFields.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFields[i]->FieldProperty().IsSame(aProperty))
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::ContainsFieldFromGroup
// --------------------------------------------------------------------------
//
 TBool
CFscPresentationContactFieldCollection::ContainsFieldFromGroup(
        const CFscFieldPropertyGroup& aPropertyGroup) const
    {
	FUNC_LOG;
    const TInt count = aPropertyGroup.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (ContainsField(aPropertyGroup.At(i)) != KErrNotFound)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::FindFieldIndex
// --------------------------------------------------------------------------
//
 TInt CFscPresentationContactFieldCollection::FindFieldIndex(
        const MVPbkStoreContactField& aField) const
    {
    FUNC_LOG;
    const TInt count = iFields.Count();
    for(TInt i = 0; i < count; ++i)
        {
        if (iFields[i]->IsSame(aField))
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::AddFieldMappingL
// --------------------------------------------------------------------------
//
void CFscPresentationContactFieldCollection::AddFieldMappingL(
        const MVPbkStoreContactField& aField)
    {
    FUNC_LOG;
    TLinearOrder<CFscPresentationContactField> compareFunc(CompareFields);
    const TInt maxMatchPriority = SupportedFieldTypes(iFieldCollection).MaxMatchPriority();
    const MPbk2FieldProperty* prop = MatchProperty(iFieldProperties, aField,
            maxMatchPriority);
    if (prop)
        {
        CFscPresentationContactField* pbk2Field =
                CFscPresentationContactField::NewLC(*prop, aField,
                        iParentContact);
        iFields.InsertInOrderAllowRepeatsL(pbk2Field, compareFunc);
        CleanupStack::Pop(); // pbk2Field
        }
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::RemoveFieldMapping
// --------------------------------------------------------------------------
//
void CFscPresentationContactFieldCollection::RemoveFieldMapping(TInt aIndex)
    {
    FUNC_LOG;
    if (aIndex >= 0 && aIndex < iFields.Count())
        {
        delete iFields[aIndex];
        iFields.Remove(aIndex);
        }
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::ResetFieldMappings
// --------------------------------------------------------------------------
//
void CFscPresentationContactFieldCollection::ResetFieldMappings()
    {
    FUNC_LOG;
    iFields.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::StoreIndexOfField
// --------------------------------------------------------------------------
//  
 TInt CFscPresentationContactFieldCollection::StoreIndexOfField(
        TInt aFieldIndex) const
    {
    FUNC_LOG;
    __ASSERT_DEBUG(aFieldIndex >= 0 && aFieldIndex < iFields.Count(),
            Panic(EGetStoreContactIndex_PreCond));
    const TInt count = iFieldCollection.FieldCount();
    MVPbkStoreContactField* field = &iFields[aFieldIndex]->StoreField();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldCollection.FieldAt(i).IsSame(*field))
            {
            return i;
            }
        }
    __ASSERT_DEBUG(EFalse, Panic(EGetStoreContactIndex_PostCond));
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::ParentContact
// --------------------------------------------------------------------------
//
MVPbkBaseContact& CFscPresentationContactFieldCollection::ParentContact() const
    {
    FUNC_LOG;
    return iParentContact;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::FieldCount
// --------------------------------------------------------------------------
//
TInt CFscPresentationContactFieldCollection::FieldCount() const
    {
    FUNC_LOG;
    return iFields.Count();
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::FieldAt
// --------------------------------------------------------------------------
//
const MVPbkStoreContactField& CFscPresentationContactFieldCollection::FieldAt(
        TInt aIndex) const
    {
    FUNC_LOG;
    return *iFields[aIndex];
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::FieldAt
// --------------------------------------------------------------------------
//
MVPbkStoreContactField& CFscPresentationContactFieldCollection::FieldAt(
        TInt aIndex)
    {
    FUNC_LOG;
    return *iFields[aIndex];
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::FieldAtLC
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CFscPresentationContactFieldCollection::FieldAtLC(
        TInt aIndex) const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( iFields.Count()> aIndex &&
            aIndex >= 0, Panic(EPanic_FieldAtLC_OOB )); // <cmail>

    TInt storeIndex = StoreIndexOfField(aIndex);
    const MVPbkStoreContactField& field =
            iFieldCollection.FieldAt(storeIndex);

    MVPbkStoreContactField* pbk2Field = CFscPresentationContactField::NewLC(
            iFields[aIndex]->FieldProperty(), field, iParentContact);
    return pbk2Field;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::ParentStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact& CFscPresentationContactFieldCollection::ParentStoreContact() const
    {
    FUNC_LOG;
    return iParentContact;
    }

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::ParentStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CFscPresentationContactFieldCollection::RetrieveField(
        const MVPbkContactLink& aContactLink) const
    {
    FUNC_LOG;
    return iFieldCollection.RetrieveField(aContactLink);
    }

//  End of File  

