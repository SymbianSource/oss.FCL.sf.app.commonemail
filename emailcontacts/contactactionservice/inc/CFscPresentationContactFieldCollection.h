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
* Description:  Sorted field collection for fields.
 *
*/


#ifndef CFSCPRESENTATIONCONTACTFIELDCOLLECTION_H
#define CFSCPRESENTATIONCONTACTFIELDCOLLECTION_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkStoreContactFieldCollection.h>

// FORWARD DECLARATIONS
class CFscPresentationContactField;
class MPbk2FieldPropertyArray;
class MPbk2FieldProperty;
class CFscFieldPropertyGroup;
class MVPbkStoreContactField;

// CLASS DECLARATION

/**
 * Phonebook 2 presentation level contact field collection.
 */
class CFscPresentationContactFieldCollection : public CBase,
    public MVPbkStoreContactFieldCollection
    {
public:
    // Constructors and destructor

    /**
     * Creates a new instance of this class.
     *
     * @param aFieldProperties  An array of field properties.
     * @param aFieldCollection  A collection of store fields.
     * @param aParentContact    The contact that owns this collection.
     * @return  A new instance of this class.
     */
    static CFscPresentationContactFieldCollection* NewL(
            const MPbk2FieldPropertyArray& aFieldProperties,
            MVPbkStoreContactFieldCollection& aFieldCollection,
            MVPbkStoreContact& aParentContact );

    /**
     * Destructor.
     */
    virtual ~CFscPresentationContactFieldCollection();

public: // Interface

    /**
     * Returns the presentation level field.
     *
     * @param aIndex    The index of the field in the array.
     * @return  Presentation level field.
     */
    CFscPresentationContactField& At(
            TInt aIndex ) const;

    /**
     * Checks if there is a field with given property in the collection.
     *
     * @param aProperty     The property to search for.
     * @return  The index of the field or KErrNotFound.
     */
    TInt ContainsField(
            const MPbk2FieldProperty& aProperty ) const;

    /**
     * Checks if the collection contains a field from the group of
     * properties.
     *
     * @param aPropertyGroup    A group of field properties.
     * @return  ETrue if the collection has at least one field
     *          with the property in the group.
     */
    TBool ContainsFieldFromGroup(
            const CFscFieldPropertyGroup& aPropertyGroup ) const;

    /**
     * Returns the index of the given field or
     * KErrNotFound if not found.
     *
     * @param aField    The field that is searched for.
     * @return  The index of the field in this collection.
     */
    TInt FindFieldIndex(
            const MVPbkStoreContactField& aField ) const;

    /**
     * Gets the index of the field in the store contact.
     *
     * @param aFieldIndex   The index of the field in this collection.
     * @return  The index of the field in the
     *          store contact field collection.
     */
    TInt StoreIndexOfField(
            TInt aFieldIndex ) const;

    /**
     * Returns the field properties.
     *
     * @return  Field properties.
     */
    inline const MPbk2FieldPropertyArray& FieldProperties() const;

    /**
     * Adds a new field mapping. The field must belong to
     * the same contact as this field collection.
     *
     * @param aField    A new store contact field for the mapping.
     */
    void AddFieldMappingL(
            const MVPbkStoreContactField& aField );

    /**
     * Deletes the field mapping from the given index.
     *
     * @param aIndex    The index from where to remove mapping.
     */
    void RemoveFieldMapping(
            TInt aIndex );

    /**
     * Destroys all the field mappings.
     */
    void ResetFieldMappings();

public: // From MVPbkStoreContactFieldCollection
    MVPbkBaseContact& ParentContact() const;
    TInt FieldCount() const;
    const MVPbkStoreContactField& FieldAt(
            TInt aIndex ) const;
    MVPbkStoreContactField& FieldAt(
            TInt aIndex );
    MVPbkStoreContactField* FieldAtLC(
            TInt aIndex ) const;
    MVPbkStoreContact& ParentStoreContact() const;
    MVPbkStoreContactField* RetrieveField(
            const MVPbkContactLink& aContactLink ) const;

private: // Implementation
    CFscPresentationContactFieldCollection(
            const MPbk2FieldPropertyArray& aFieldProperties,
            MVPbkStoreContactFieldCollection& aFieldCollection,
            MVPbkStoreContact& aParentContact );
    void ConstructL();

private: // Data
    /// Ref: An array of field properties
    const MPbk2FieldPropertyArray& iFieldProperties;
    /// Ref: A collection of store fields
    MVPbkStoreContactFieldCollection& iFieldCollection;
    /// Ref: The parent contact of the collection
    MVPbkStoreContact& iParentContact;
    /// Own: Phonebook 2 presentation contact fields
    RPointerArray<CFscPresentationContactField> iFields;
    };

// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// CFscPresentationContactFieldCollection::FieldProperties
// --------------------------------------------------------------------------
//
inline const MPbk2FieldPropertyArray& CFscPresentationContactFieldCollection::FieldProperties() const
    {
    return iFieldProperties;
    }

#endif // CFSCPRESENTATIONCONTACTFIELDCOLLECTION_H
// End of File
